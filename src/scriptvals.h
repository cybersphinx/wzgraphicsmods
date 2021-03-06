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
/** @file
 *  Common functions for the scriptvals loader
 */

#ifndef __INCLUDED_SRC_SCRIPTVALS_H__
#define __INCLUDED_SRC_SCRIPTVALS_H__

#include "lib/script/interpreter.h"
#include "lib/script/event.h"
#include "basedef.h"
#include <physfs.h>

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

// The possible types of initialisation values
typedef enum _init_type
{
	IT_BOOL,
	IT_INDEX,
	IT_STRING,
} INIT_TYPE;


// All the possible values that may be used to initialise a variable
typedef struct _var_init
{
	INIT_TYPE	type;
	SDWORD		index;
	char		*pString;
} VAR_INIT;


// store array access data
typedef struct _array_indexes
{
	SDWORD		dimensions;
	SDWORD		elements[VAR_MAX_DIMENSIONS];
} ARRAY_INDEXES;

/* A simple error reporting routine */

extern void scrv_error(const char* fmt, ...) WZ_DECL_FORMAT(printf, 1, 2);

// Lookup a type
extern BOOL scrvLookUpType(const char *pIdent, INTERP_TYPE *pType);

// Lookup a variable identifier
extern BOOL scrvLookUpVar(const char *pIdent, UDWORD *pIndex);

// Lookup an array identifier
extern BOOL scrvLookUpArray(const char *pIdent, UDWORD *pIndex);

// Whether the script is run immediately or stored for later use
typedef enum _scrv_type
{
	SCRV_EXEC,
	SCRV_NOEXEC,
} SCRV_TYPE;

// Add a new context to the list
extern BOOL scrvAddContext(char *pID, SCRIPT_CONTEXT *psContext, SCRV_TYPE type);

// Get a context from the list
extern BOOL scrvGetContext(char *pID, SCRIPT_CONTEXT **ppsContext);

// Add a new base pointer variable
extern BOOL scrvAddBasePointer(INTERP_VAL *psVal);

// Check all the base pointers to see if they have died
extern void scrvUpdateBasePointers(void);

// remove a base pointer from the list
extern void scrvReleaseBasePointer(INTERP_VAL *psVal);

// create a group structure for a ST_GROUP variable
extern BOOL scrvNewGroup(INTERP_VAL *psVal);

// release a ST_GROUP variable
extern void scrvReleaseGroup(INTERP_VAL *psVal);

// Initialise the script value module
extern BOOL scrvInitialise(void);

// Shut down the script value module
extern void scrvShutDown(void);

// reset the script value module
extern void scrvReset(void);

// Load a script value file
extern BOOL scrvLoad(PHYSFS_file* fileHandle);

// Link any object types to the actual pointer values
//extern BOOL scrvLinkValues(void);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __INCLUDED_SRC_SCRIPTVALS_H__
