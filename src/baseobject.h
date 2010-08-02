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
/** \file
 *  Functions for the base object type.
 */

#ifndef __INCLUDED_BASEOBJECT_H__
#define __INCLUDED_BASEOBJECT_H__

#include "basedef.h"

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

static const unsigned int max_check_object_recursion = 4;

/// Get interpolated direction at time t.
Rotation interpolateRot(Rotation v1, Rotation v2, uint32_t t1, uint32_t t2, uint32_t t);
/// Get interpolated object spacetime at time t.
SPACETIME interpolateObjectSpacetime(const SIMPLE_OBJECT *obj, uint32_t t);

void checkObject(const BASE_OBJECT* psObject, const char * const location_description, const char * function, const int recurse);

/* assert if object is bad */
#define CHECK_OBJECT(object) checkObject((object), AT_MACRO, __FUNCTION__, max_check_object_recursion)

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __INCLUDED_BASEOBJECT_H__