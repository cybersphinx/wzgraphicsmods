/*
	This file is part of Warzone 2100.
	Copyright (C) 1999-2004  Eidos Interactive
	Copyright (C) 2005-2009  Warzone Resurrection Project

	Warzone 2100 is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	Warzone 2100 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Warzone 2100; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/
/**
 * @file fpath.c
 *
 * Interface to the routing functions.
 *
 */

#include <SDL.h>
#include <SDL_thread.h>

#include "lib/framework/frame.h"
#include "lib/netplay/netplay.h"

#include "objects.h"
#include "map.h"
#include "raycast.h"
#include "geometry.h"
#include "hci.h"
#include "order.h"
#include "multiplay.h"
#include "astar.h"
#include "action.h"
#include "formation.h"

#include "fpath.h"

// If the path finding system is shutdown or not
static volatile bool fpathQuit = false;

/* Beware: Enabling this will cause significant slow-down. */
#undef DEBUG_MAP

/* minimum height difference for VTOL blocking tile */
#define	LIFT_BLOCK_HEIGHT_LIGHTBODY		  30
#define	LIFT_BLOCK_HEIGHT_MEDIUMBODY	 350
#define	LIFT_BLOCK_HEIGHT_HEAVYBODY		 350

typedef struct _jobDone
{
	UDWORD		droidID;	///< Unique droid ID.
	MOVE_CONTROL	sMove;		///< New movement values for the droid.
	struct _jobDone	*next;		///< Next result in the list.
	FPATH_RETVAL	retval;		///< Result value from path-finding.
	bool		done;		///< If the result is finished and ready for use.
} PATHRESULT;


#define NUM_BASIC	 8
#define NUM_DIR		24

// Convert a direction into an offset, spanning two tiles
static const Vector2i aDirOffset[NUM_DIR] =
{
	{  0,  1 }, { -1,  1 }, { -1,  0 }, { -1, -1 }, {  0, -1 }, {  1, -1 }, {  1,  0 }, {  1,  1 },
	{ -2, -2 }, { -1, -2 }, {  0, -2 }, {  1, -2 }, {  2, -2 }, { -2, -1 }, {  2, -1 }, { -2,  0 },
	{  2,  0 }, { -2,  1 }, {  2,  1 }, { -2,  2 }, { -1,  2 }, {  0,  2 }, {  1,  2 }, {  2,  2 },
};

// threading stuff
static WZ_THREAD        *fpathThread = NULL;
static WZ_MUTEX         *fpathMutex = NULL;
static WZ_SEMAPHORE     *fpathSemaphore = NULL;
static PATHJOB		*firstJob = NULL;
static PATHRESULT	*firstResult = NULL;

static bool             waitingForResult = false;
static uint32_t         waitingForResultId;
static WZ_SEMAPHORE     *waitingForResultSemaphore = NULL;

static void fpathExecute(PATHJOB *psJob, PATHRESULT *psResult);


/** Find the length of the job queue. Function is thread-safe. */
static int fpathJobQueueLength(void)
{
	PATHJOB *psJob;
	int count = 0;

	wzMutexLock(fpathMutex);
	psJob = firstJob;
	while (psJob)
	{
		count++;
		psJob = psJob->next;
	}
	wzMutexUnlock(fpathMutex);
	return count;
}


/** Find the length of the result queue, excepting future results. Function is thread-safe. */
static int fpathResultQueueLength(void)
{
	PATHRESULT *psResult;
	int count = 0;

	wzMutexLock(fpathMutex);
	psResult = firstResult;
	while (psResult)
	{
		if (psResult->done)
		{
			count++;
		}
		psResult = psResult->next;
	}
	wzMutexUnlock(fpathMutex);
	return count;
}


/** This runs in a separate thread */
static int fpathThreadFunc(WZ_DECL_UNUSED void *data)
{
	wzMutexLock(fpathMutex);

	while (!fpathQuit)
	{
		PATHJOB		job;
		PATHRESULT	*psResult, result;
		bool		gotWork = false;

		// Pop the first job off the queue
		if (firstJob)
		{
			PATHJOB	*next = firstJob->next;

			job = *firstJob;	// struct copy
			job.next = NULL;
			free(firstJob);
			firstJob = next;
			gotWork = true;
		}

		if (!gotWork)
		{
			ASSERT(!waitingForResult, "Waiting for a result (id %u) that doesn't exist.", waitingForResultId);
			wzMutexUnlock(fpathMutex);
			wzSemaphoreWait(fpathSemaphore);  // Go to sleep until needed.
			wzMutexLock(fpathMutex);
			continue;
		}

		// Create future result
		psResult = malloc(sizeof(*psResult));
		psResult->done = false;
		psResult->droidID = job.droidID;
		psResult->sMove.asPath = NULL;
		psResult->retval = FPR_FAILED;

		// Add to beginning of result list
		psResult->next = firstResult;
		firstResult = psResult;
		psResult = NULL;	// now hands off

		wzMutexUnlock(fpathMutex);

		// Execute path-finding for this job using our local temporaries
		memset(&result, 0, sizeof(result));
		result.sMove.asPath = NULL;
		fpathExecute(&job, &result);

		wzMutexLock(fpathMutex);

		// Find our result again, and replace it with our local temporary
		// We do it this way to avoid a race condition where a droid dies
		// while we are generating its path, and we never free the result.
		psResult = firstResult;
		while (psResult && psResult->droidID != job.droidID)
		{
			psResult = psResult->next;
		}
		if (psResult)
		{
			psResult->sMove = result.sMove; 	// struct copy
			psResult->retval = result.retval;
			psResult->done = true;
		}

		// Unblock the main thread, if it was waiting for this particular result.
		if (waitingForResult && waitingForResultId == job.droidID)
		{
			waitingForResult = false;
			objTrace(waitingForResultId, "These are the droids you are looking for.");
			wzSemaphorePost(waitingForResultSemaphore);
		}
	}
	wzMutexUnlock(fpathMutex);
	return 0;
}


// initialise the findpath module
BOOL fpathInitialise(void)
{
	// The path system is up
	fpathQuit = false;

	if (!fpathThread)
	{
		fpathMutex = wzMutexCreate();
		fpathSemaphore = wzSemaphoreCreate(0);
		waitingForResultSemaphore = wzSemaphoreCreate(0);
		fpathThread = wzThreadCreate(fpathThreadFunc, NULL);
		wzThreadStart(fpathThread);
	}

	return true;
}


void fpathShutdown()
{
	// Signal the path finding thread to quit
	fpathQuit = true;
	wzSemaphorePost(fpathSemaphore);  // Wake up thread.

	fpathHardTableReset();
	if (fpathThread)
	{
		wzThreadJoin(fpathThread);
		fpathThread = NULL;
		wzMutexDestroy(fpathMutex);
		fpathMutex = NULL;
		wzSemaphoreDestroy(fpathSemaphore);
		fpathSemaphore = NULL;
		wzSemaphoreDestroy(waitingForResultSemaphore);
		waitingForResultSemaphore = NULL;
	}
}


/**
 *	Updates the pathfinding system.
 *	@ingroup pathfinding
 */
void fpathUpdate(void)
{
	// Nothing now
}


// Check if the map tile at a location blocks a droid
BOOL fpathBaseBlockingTile(SDWORD x, SDWORD y, PROPULSION_TYPE propulsion, int player, FPATH_MOVETYPE moveType)
{
	MAPTILE	*psTile;

	/* All tiles outside of the map and on map border are blocking. */
	if (x < 1 || y < 1 || x > mapWidth - 1 || y > mapHeight - 1)
	{
		return true;
	}

	/* Check scroll limits (used in campaign to partition the map. */
	if (propulsion != PROPULSION_TYPE_LIFT && (x < scrollMinX + 1 || y < scrollMinY + 1 || x >= scrollMaxX - 1 || y >= scrollMaxY - 1))
	{
		// coords off map - auto blocking tile
		return true;
	}

	psTile = mapTile(x, y);

	// Only tall structures are blocking VTOL now
	if (propulsion == PROPULSION_TYPE_LIFT && !TileHasTallStructure(psTile))
	{
		return false;
	}
	else if (propulsion == PROPULSION_TYPE_LIFT)
	{
		return true;
	}
	else if (propulsion == PROPULSION_TYPE_PROPELLOR && terrainType(psTile) != TER_WATER)
	{
		return true;
	}

	if (TileIsOccupied(psTile) && !TileIsNotBlocking(psTile))
	{
		// Implement gates by completely ignoring them
		if (psTile->psObject->type == OBJ_STRUCTURE && psTile->psObject->player == player
		    && ((STRUCTURE *)psTile->psObject)->status == SS_BUILT
		    && ((STRUCTURE *)psTile->psObject)->pStructureType->type == REF_GATE)
		{
			return false;
		}
		// If the type of movement order is simple movement (FMT_MOVE) then we treat all genuine obstacles as
		// impassable. However, if it is an attack type order, we assume we can blast our way through enemy buildings.
		else if (moveType == FMT_MOVE)
		{
			return true;
		}
		else if (moveType == FMT_ATTACK && psTile->psObject
		         && psTile->psObject->type == OBJ_STRUCTURE && aiCheckAlliances(psTile->psObject->player, player))
		{
			return true;
		}
	}

	if (psTile->tileInfoBits & BITS_FPATHBLOCK 
	    || terrainType(psTile) == TER_CLIFFFACE
	    || (terrainType(psTile) == TER_WATER && propulsion != PROPULSION_TYPE_HOVER && propulsion != PROPULSION_TYPE_PROPELLOR))
	{
		return true;
	}

	return false;
}

// Check if the map tile at a location blocks a droid
BOOL fpathBlockingTile(SDWORD x, SDWORD y, PROPULSION_TYPE propulsion)
{
	return fpathBaseBlockingTile(x, y, propulsion, MAX_PLAYERS, FMT_MOVE);
}


/** Calculate the distance to a tile from a point
 *
 *  @ingroup pathfinding
 */
static inline int fpathDistToTile(int tileX, int tileY, int pointX, int pointY)
{
	// get the difference in world coords
	int xdiff = world_coord(tileX) - pointX;
	int ydiff = world_coord(tileY) - pointY;

	return iHypot(xdiff, ydiff);
}


static void fpathSetMove(MOVE_CONTROL *psMoveCntl, SDWORD targetX, SDWORD targetY)
{
	psMoveCntl->asPath = realloc(psMoveCntl->asPath, sizeof(*psMoveCntl->asPath));
	psMoveCntl->DestinationX = targetX;
	psMoveCntl->DestinationY = targetY;
	psMoveCntl->numPoints = 1;
	psMoveCntl->asPath[0].x = targetX;
	psMoveCntl->asPath[0].y = targetY;
}


void fpathSetDirectRoute(DROID *psDroid, SDWORD targetX, SDWORD targetY)
{
	fpathSetMove(&psDroid->sMove, targetX, targetY);
}


void fpathRemoveDroidData(int id)
{
	PATHJOB		*psJob;
	PATHJOB		*psPrevJob = NULL;
	PATHRESULT	*psResult;
	PATHRESULT	*psPrevResult = NULL;

	wzMutexLock(fpathMutex);

	psJob = firstJob;
	psResult = firstResult;

	while (psJob)
	{
		if (psJob->droidID == id)
		{
			if (psPrevJob)
			{
				psPrevJob->next = psJob->next;
				free(psJob);
				psJob = psPrevJob->next;
			}
			else
			{
				firstJob = psJob->next;
				free(psJob);
				psJob = firstJob;
			}
		}
		else
		{
			psPrevJob = psJob;
			psJob = psJob->next;
		}
	}
	while (psResult)
	{
		if (psResult->droidID == id)
		{
			if (psPrevResult)
			{
				psPrevResult->next = psResult->next;
				free(psResult->sMove.asPath);
				free(psResult);
				psResult = psPrevResult->next;
			}
			else
			{
				firstResult = psResult->next;
				free(psResult->sMove.asPath);
				free(psResult);
				psResult = firstResult;
			}
		}
		else
		{
			psPrevResult = psResult;
			psResult = psResult->next;
		}
	}
	wzMutexUnlock(fpathMutex);
}


static FPATH_RETVAL fpathRoute(MOVE_CONTROL *psMove, int id, int startX, int startY, int tX, int tY, PROPULSION_TYPE propulsionType, DROID_TYPE droidType, FPATH_MOVETYPE moveType, int owner)
{
	PATHJOB		*psJob = NULL;
	int 		count;

	objTrace(id, "called(*,id=%d,sx=%d,sy=%d,ex=%d,ey=%d,prop=%d,type=%d,move=%d,owner=%d)", id, startX, startY, tX, tY, (int)propulsionType, (int)droidType, (int)moveType, owner);

	// don't have to do anything if already there
	if (startX == tX && startY == tY)
	{
		// return failed to stop them moving anywhere
		objTrace(id, "Tried to move nowhere");
		return FPR_FAILED;
	}

	// Check if waiting for a result
	while (psMove->Status == MOVEWAITROUTE)
	{
		PATHRESULT *psPrev = NULL, *psNext;

		objTrace(id, "Checking if we have a path yet");
		wzMutexLock(fpathMutex);

		// psNext should be _declared_ here, after the mutex lock! Used to be a race condition, thanks to -Wdeclaration-after-statement style pseudocompiler compatibility.
		psNext = firstResult;

		while (psNext)
		{
			if (psNext->droidID == id && psNext->done)
			{
				FPATH_RETVAL	retval;

				ASSERT(psNext->retval != FPR_OK || psNext->sMove.asPath, "Ok result but no path in list");

				// Remove it from the result list
				if (psPrev)
				{
					psPrev->next = psNext->next;
				}
				else
				{
					firstResult = psNext->next;
				}

				// Copy over select fields - preserve others
				psMove->DestinationX = psNext->sMove.DestinationX;
				psMove->DestinationY = psNext->sMove.DestinationY;
				psMove->numPoints = psNext->sMove.numPoints;
				psMove->Position = 0;
				psMove->Status = MOVENAVIGATE;
				if (psMove->asPath)
				{
					free(psMove->asPath);
				}
				psMove->asPath = psNext->sMove.asPath;
				retval = psNext->retval;
				ASSERT(retval != FPR_OK || psMove->asPath, "Ok result but no path after copy");
				ASSERT(retval != FPR_OK || psMove->numPoints > 0, "Ok result but path empty after copy");
				free(psNext);
				wzMutexUnlock(fpathMutex);
				objTrace(id, "Got a path to (%d, %d)! Length=%d Retval=%d", (int)psMove->DestinationX,
				         (int)psMove->DestinationY, (int)psMove->numPoints, (int)retval);
				return retval;
			}
			psPrev = psNext;
			psNext = psNext->next;
		}

		objTrace(id, "No path yet. Waiting.");
		waitingForResult = true;
		waitingForResultId = id;
		wzMutexUnlock(fpathMutex);
		wzSemaphoreWait(waitingForResultSemaphore);  // keep waiting
	}

	// We were not waiting for a result, and found no trivial path, so create new job and start waiting
	psJob = malloc(sizeof(*psJob));
	ASSERT(psJob, "Out of memory");
	if (!psJob)
	{
		return FPR_FAILED;
	}
	psJob->origX = startX;
	psJob->origY = startY;
	psJob->droidID = id;
	psJob->destX = tX;
	psJob->destY = tY;
	psJob->next = NULL;
	psJob->droidType = droidType;
	psJob->propulsion = propulsionType;
	psJob->moveType = moveType;
	psJob->owner = owner;

	// Clear any results or jobs waiting already. It is a vital assumption that there is only one
	// job or result for each droid in the system at any time.
	fpathRemoveDroidData(id);

	wzMutexLock(fpathMutex);

	// Add to end of list
	count = 0;
	if (!firstJob)
	{
		firstJob = psJob;
		wzSemaphorePost(fpathSemaphore);  // Wake up processing thread.
	}
	else
	{
		PATHJOB *psNext = firstJob;
		++count;

		while (psNext->next != NULL)
		{
			psNext = psNext->next;
			++count;
		}

		psNext->next = psJob;
	}

	wzMutexUnlock(fpathMutex);

	objTrace(id, "Queued up a path-finding request to (%d, %d), %d items earlier in queue", tX, tY, count);
	return FPR_WAIT;	// wait while polling result queue
}


// Find a route for an DROID to a location in world coordinates
FPATH_RETVAL fpathDroidRoute(DROID* psDroid, SDWORD tX, SDWORD tY, FPATH_MOVETYPE moveType)
{
	PROPULSION_STATS *psPropStats = getPropulsionStats(psDroid);

	// override for AI to blast our way through stuff
	if (!isHumanPlayer(psDroid->player) && moveType == FMT_MOVE)
	{
		moveType = (psDroid->asWeaps[0].nStat == 0) ? FMT_MOVE : FMT_ATTACK;
	}

	ASSERT_OR_RETURN(FPR_FAILED, psPropStats != NULL, "invalid propulsion stats pointer");
	ASSERT_OR_RETURN(FPR_FAILED, psDroid->type == OBJ_DROID, "We got passed an object that isn't a DROID!");

	// check whether the end point of the route
	// is a blocking tile and find an alternative if it is
	if (psDroid->sMove.Status != MOVEWAITROUTE && fpathBlockingTile(map_coord(tX), map_coord(tY), psPropStats->propulsionType))
	{
		// find the nearest non blocking tile to the DROID
		int minDist = SDWORD_MAX;
		int nearestDir = NUM_DIR;
		int dir;

		objTrace(psDroid->id, "BLOCKED(%d,%d) - trying workaround", map_coord(tX), map_coord(tY));
		for (dir = 0; dir < NUM_DIR; dir++)
		{
			int x = map_coord(tX) + aDirOffset[dir].x;
			int y = map_coord(tY) + aDirOffset[dir].y;

			if (tileOnMap(x, y) && !fpathBlockingTile(x, y, psPropStats->propulsionType))
			{
				// pick the adjacent tile closest to our starting point
				int tileDist = fpathDistToTile(x, y, psDroid->pos.x, psDroid->pos.y);

				if (tileDist < minDist)
				{
					minDist = tileDist;
					nearestDir = dir;
				}
			}

			if (dir == NUM_BASIC - 1 && nearestDir != NUM_DIR)
			{
				break;	// found a solution without checking at greater distance
			}
		}

		if (nearestDir == NUM_DIR)
		{
			// surrounded by blocking tiles, give up
			objTrace(psDroid->id, "route to (%d, %d) failed - target blocked", map_coord(tX), map_coord(tY));
			return FPR_FAILED;
		}
		else
		{
			tX = world_coord(map_coord(tX) + aDirOffset[nearestDir].x) + TILE_SHIFT / 2;
			tY = world_coord(map_coord(tY) + aDirOffset[nearestDir].y) + TILE_SHIFT / 2;
			objTrace(psDroid->id, "Workaround found at (%d, %d)", map_coord(tX), map_coord(tY));
		}
	}
	return fpathRoute(&psDroid->sMove, psDroid->id, psDroid->pos.x, psDroid->pos.y, tX, tY, psPropStats->propulsionType, psDroid->droidType, moveType, psDroid->player);
}

// Run only from path thread
static void fpathExecute(PATHJOB *psJob, PATHRESULT *psResult)
{
	FPATH_RETVAL retval = fpathAStarRoute(&psResult->sMove, psJob);

	ASSERT(retval != ASR_OK || psResult->sMove.asPath, "Ok result but no path in result");
	ASSERT(retval == ASR_FAILED || psResult->sMove.numPoints > 0, "Ok result but no length of path in result");
	switch (retval)
	{
	case ASR_NEAREST:
		objTrace(psJob->droidID, "** Nearest route **");
		psResult->retval = FPR_OK;
		break;
	case ASR_FAILED:
		objTrace(psJob->droidID, "** Failed route **");
		// Is this really a good idea? Was in original code.
		if (psJob->propulsion == PROPULSION_TYPE_LIFT && psJob->droidType != DROID_TRANSPORTER)
		{
			objTrace(psJob->droidID, "Doing fallback for non-transport VTOL");
			fpathSetMove(&psResult->sMove, psJob->destX, psJob->destY);
			psResult->retval = FPR_OK;
		}
		else
		{
			psResult->retval = FPR_FAILED;
		}
		break;
	case ASR_OK:
		objTrace(psJob->droidID, "Got route of length %d", psResult->sMove.numPoints);
		psResult->retval = FPR_OK;
		break;
	}
}

// Variables for the callback
static BOOL		obstruction;

/** The visibility ray callback
 */
static bool fpathVisCallback(Vector3i pos, int32_t dist, void *data)
{
	DROID *psDroid = (DROID *)data;

	if (fpathBlockingTile(map_coord(pos.x), map_coord(pos.y), getPropulsionStats(psDroid)->propulsionType))
	{
		// found an obstruction
		obstruction = true;
		return false;
	}

	return true;
}

BOOL fpathTileLOS(DROID *psDroid, Vector3i dest)
{
	Vector3i dir = Vector3i_Sub(dest, psDroid->pos);

	// Initialise the callback variables
	obstruction = false;

	rayCast(psDroid->pos, iAtan2(dir.x, dir.y), iHypot(dir.x, dir.y), fpathVisCallback, psDroid);

	return !obstruction;
}

static FPATH_RETVAL fpathSimpleRoute(MOVE_CONTROL *psMove, int id, int startX, int startY, int tX, int tY)
{
	return fpathRoute(psMove, id, startX, startY, tX, tY, PROPULSION_TYPE_WHEELED, DROID_WEAPON, FMT_MOVE, 0);
}

void fpathTest(int x, int y, int x2, int y2)
{
	MOVE_CONTROL sMove;
	FPATH_RETVAL r;
	int i;

	// On non-debug builds prevent warnings about defining but not using fpathJobQueueLength
	(void)fpathJobQueueLength;

	/* Check initial state */
	assert(fpathThread != NULL);
	assert(fpathMutex != NULL);
	assert(fpathSemaphore != NULL);
	assert(firstJob == NULL);
	assert(firstResult == NULL);
	assert(fpathJobQueueLength() == 0);
	assert(fpathResultQueueLength() == 0);
	fpathRemoveDroidData(0);	// should not crash

	/* This should not leak memory */
	sMove.asPath = NULL;
	for (i = 0; i < 100; i++) fpathSetMove(&sMove, 1, 1);
	free(sMove.asPath);
	sMove.asPath = NULL;

	/* Test one path */
	sMove.Status = MOVEINACTIVE;
	r = fpathSimpleRoute(&sMove, 1, x, y, x2, y2);
	assert(r == FPR_WAIT);
	sMove.Status = MOVEWAITROUTE;
	assert(fpathJobQueueLength() == 1 || fpathResultQueueLength() == 1);
	fpathRemoveDroidData(2);	// should not crash, nor remove our path
	assert(fpathJobQueueLength() == 1 || fpathResultQueueLength() == 1);
	while (fpathResultQueueLength() == 0) wzYieldCurrentThread();
	assert(fpathJobQueueLength() == 0);
	assert(fpathResultQueueLength() == 1);
	r = fpathSimpleRoute(&sMove, 1, x, y, x2, y2);
	assert(r == FPR_OK);
	assert(sMove.numPoints > 0 && sMove.asPath);
	assert(sMove.asPath[sMove.numPoints - 1].x == x2);
	assert(sMove.asPath[sMove.numPoints - 1].y == y2);
	assert(fpathResultQueueLength() == 0);

	/* Let one hundred paths flower! */
	sMove.Status = MOVEINACTIVE;
	for (i = 1; i <= 100; i++)
	{
		r = fpathSimpleRoute(&sMove, i, x, y, x2, y2);
		assert(r == FPR_WAIT);
	}
	while (fpathResultQueueLength() != 100) wzYieldCurrentThread();
	assert(fpathJobQueueLength() == 0);
	for (i = 1; i <= 100; i++)
	{
		sMove.Status = MOVEWAITROUTE;
		r = fpathSimpleRoute(&sMove, i, x, y, x2, y2);
		assert(r == FPR_OK);
		assert(sMove.numPoints > 0 && sMove.asPath);
		assert(sMove.asPath[sMove.numPoints - 1].x == x2);
		assert(sMove.asPath[sMove.numPoints - 1].y == y2);
	}
	assert(fpathResultQueueLength() == 0);

	/* Kill a hundred flowers */
	sMove.Status = MOVEINACTIVE;
	for (i = 1; i <= 100; i++)
	{
		r = fpathSimpleRoute(&sMove, i, x, y, x2, y2);
		assert(r == FPR_WAIT);
	}
	for (i = 1; i <= 100; i++)
	{
		fpathRemoveDroidData(i);
	}
	assert(fpathJobQueueLength() == 0);
	assert(fpathResultQueueLength() == 0);
	assert(firstJob == NULL);
	assert(firstResult == NULL);
}

bool fpathCheck(Position orig, Position dest, PROPULSION_TYPE propulsion)
{
	MAPTILE *origTile;
	MAPTILE *destTile;

	// We have to be careful with this check because it is called on
	// load when playing campaign on droids that are on the other
	// map during missions, and those maps are usually larger.
	if (!worldOnMap(orig.x, orig.y) || !worldOnMap(dest.x, dest.y))
	{
		return false;
	}

	origTile = worldTile(orig.x, orig.y);
	destTile = worldTile(dest.x, dest.y);

	ASSERT(propulsion != PROPULSION_TYPE_NUM, "Bad propulsion type");
	ASSERT(origTile != NULL && destTile != NULL, "Bad tile parameter");

	switch (propulsion)
	{
	case PROPULSION_TYPE_PROPELLOR:
	case PROPULSION_TYPE_WHEELED:
	case PROPULSION_TYPE_TRACKED:
	case PROPULSION_TYPE_LEGGED:
	case PROPULSION_TYPE_SKI: 	// ?!
	case PROPULSION_TYPE_HALF_TRACKED:
		return origTile->limitedContinent == destTile->limitedContinent;
	case PROPULSION_TYPE_HOVER:
		return origTile->hoverContinent == destTile->hoverContinent;
	case PROPULSION_TYPE_JUMP:
	case PROPULSION_TYPE_LIFT:
		return true;	// FIXME: This is not entirely correct for all possible maps. - Per
	case PROPULSION_TYPE_NUM:
		break;
	}
	return true;	// should never get here
}
