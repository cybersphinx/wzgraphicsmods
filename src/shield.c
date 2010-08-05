
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

#include "lib/framework/frame.h"
#include "lib/ivis_common/ivisdef.h"
#include "lib/ivis_common/imd.h"
#include "lib/ivis_common/rendmode.h"
#include "lib/ivis_common/piefunc.h"
#include "lib/ivis_common/tex.h"
#include "lib/ivis_common/piedef.h"
#include "lib/ivis_common/piestate.h"
#include "lib/ivis_common/piepalette.h"
#include "lib/ivis_common/pieclip.h"
#include "lib/ivis_opengl/piematrix.h"


#include "action.h"
#include "component.h"
#include "display3d.h"
#include "e3demo.h"
#include "effects.h"
#include "intdisplay.h"
#include "loop.h"
#include "map.h"
#include "miscimd.h"
#include "order.h"
#include "projectile.h"
#include "transporter.h"
#include "shield.h"




void addShieldHitEffect(DROID *psDroid, Vector3i *pos)
{
	EFFECT *psEffect = Effect_malloc();
	Vector3i rotation, va, vb; 
	Vector3f ocen;
	BODY_STATS		*psBdyStats;
	int scale;
	float distance;
	va.x = psDroid->pos.x;
	va.y = psDroid->pos.y;
	va.z = psDroid->pos.z;
	vb.x = pos->x;
	vb.y = pos->y;
	vb.z = pos->z;

	distance = sqrt((float)((va.x*vb.x) + (va.y*vb.y) + (va.z*vb.z)));
	va.x = va.x/distance;
	va.y = va.y/distance;
	va.z = va.z/distance;
	vb.x = vb.x/distance;
	vb.y = vb.y/distance;
	vb.z = vb.z/distance;
		// flip z and y
	rotation.x = va.x - vb.x;
	rotation.z = va.y - vb.y;
	rotation.y = va.z - vb.z;


	
	// first we check to see if the unit actually has shield power.
	if(psDroid->shield > 0)
	{
		// Now we work out which way it should be facing
	

	// Flip Y and Z
	ocen.x = psDroid->pos.x;
	ocen.z = psDroid->pos.y;
	ocen.y = psDroid->pos.z;

	psEffect->position = ocen;
	psEffect->rotation = rotation;
	psEffect->type = HIT_TYPE_SHIELD;
	psEffect->group = EFFECT_HIT;
	
	psEffect->lifeSpan = 800;
	psEffect->imd = getImdFromIndex(MI_HITSPHERE);
	psEffect->birthTime = psEffect->lastFrame = graphicsTime;
	psBdyStats = &asBodyStats[psDroid->asBits[COMP_BODY].nStat];
	psEffect->frameNumber = 0;

	if(psDroid->sDisplay.imd->radius > getImdFromIndex(MI_HITSPHERE)->radius)
		scale = 100+PERCENT(getImdFromIndex(MI_HITSPHERE)->radius,psDroid->sDisplay.imd->radius);
	else
		scale =  PERCENT(psDroid->sDisplay.imd->radius,getImdFromIndex(MI_HITSPHERE)->radius);

	switch (psBdyStats->size)
	{
		case SIZE_LIGHT:
			scale += 30;
			break;
		case SIZE_MEDIUM:
			scale += 30;
			break;
		case SIZE_HEAVY:
			scale += 50;
			break;
		case SIZE_SUPER_HEAVY:
			scale += 90;
			break;
		default:
			scale += 10;
	}

	psEffect->size = scale;

	}

}

// Render the pie file used for forcefields
void displayShieldHit(DROID *psDroid)
{
	int32_t				xShift,zShift;
	PIELIGHT			brightness;
	PIELIGHT			specular = WZCOL_BLACK;
	iIMDShape			*psShape, *psBodyImd, *psTempImd;
	int					scale;
	Vector3i			position;
	SPACETIME st = interpolateObjectSpacetime((SIMPLE_OBJECT *)psDroid, graphicsTime);

	psShape = getImdFromIndex(MI_SSPHERE);
	psBodyImd = BODY_IMD(psDroid,((BASE_OBJECT*)psDroid)->player);
	
	if (psBodyImd != NULL)
	{
		scale = psBodyImd->radius;
	}
	else // I'm not sure if this would ever happen.
	{
		psBodyImd = ((BASE_OBJECT*)psDroid)->sDisplay.imd;
		if (psBodyImd != NULL)
		{
			scale = psBodyImd->radius;
		}
		else
		{
			scale = 1;
		}
	}

	if (!cyborgDroid(psDroid))
	{
		psTempImd = getRightPropulsionIMD(psDroid);
		if (psTempImd != NULL)
		{
			scale += psTempImd->radius/4;
		}

		psTempImd = getLeftPropulsionIMD(psDroid);
		if (psTempImd != NULL)
		{
			scale += psTempImd->radius/4;
		}
	}
	else
	{
		scale += scale/2; // FIXME
	}

	/* Get internal tile units coordinates */
	xShift = map_round(player.p.x);
	zShift = map_round(player.p.z);

	/* Get the real position */
	position.x = (st.pos.x - player.p.x) - terrainMidX*TILE_UNITS;
	position.z = terrainMidY*TILE_UNITS - (st.pos.y - player.p.z);
	position.y = st.pos.z;

	if(psDroid->droidType == DROID_TRANSPORTER)
	{
		position.y += bobTransporterHeight();
	}

	// Apply colour mask to achieve fade effect.
	brightness.byte.a = 0;
	brightness.byte.r = (PERCENT(psDroid->shield,psDroid->originalShield)+70)*1.5;
	brightness.byte.g = (PERCENT(psDroid->shield,psDroid->originalShield)+70)*1.5;
	brightness.byte.b = (PERCENT(psDroid->shield,psDroid->originalShield)+70)*1.5;
	specular = brightness;

	/* Push the matrix */
	pie_MatBegin();

		/* Mask out to tile_units resolution */
		pie_TRANSLATE(xShift,0,-zShift);

		/* Translate origin */
		pie_TRANSLATE(position.x,position.y,position.z);

		pie_MatScale(scale);

	//{x=41.000000 y=136.00000 z=52.000000 }
	// Make sure its not TCMask and is animated
	if(psShape->numFrames > 0 && psShape->numFrames != 8)
	{
		//glScalef(0.01f*scale, 0.01f*scale, 0.01f*scale);
		pie_Draw3DShape(psShape,
						getModularScaledGraphicsTime(psShape->animInterval, psShape->numFrames),
						getPlayerColour(psDroid->player),
						brightness,
						specular,
						pie_TRANSLUCENT,
						DEFAULT_COMPONENT_TRANSLUCENCY);
	}
	else
	{
		// Do it without animation.
			//glScalef(0.01f*scale, 0.01f*scale, 0.01f*scale);
		pie_Draw3DShape(psShape,
						0,
						getPlayerColour(psDroid->player),
						brightness,specular,
						pie_TRANSLUCENT,
						DEFAULT_COMPONENT_TRANSLUCENCY);
	}
	// end the matrix
	pie_MatEnd();
	
}