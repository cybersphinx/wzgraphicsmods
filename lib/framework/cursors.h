/*
	This file is part of Warzone 2100.
	Copyright (C) 2008  Giel van Schijndel
	Copyright (C) 2008-2009  Warzone Resurrection Project

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
 *  Cursor access functions.
 */

#ifndef __INCLUDED_LIB_FRAMEWORK_CURSORS_H__
#define __INCLUDED_LIB_FRAMEWORK_CURSORS_H__

#include <SDL/SDL_mouse.h>

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus


typedef enum
{
	CURSOR_ARROW,
	CURSOR_DEST,
	CURSOR_SIGHT,
	CURSOR_TARGET,
	CURSOR_LARROW,
	CURSOR_RARROW,
	CURSOR_DARROW,
	CURSOR_UARROW,
	CURSOR_DEFAULT,
	CURSOR_EDGEOFMAP,
	CURSOR_ATTACH,
	CURSOR_ATTACK,
	CURSOR_BOMB,
	CURSOR_BRIDGE,
	CURSOR_BUILD,
	CURSOR_EMBARK,
	CURSOR_DISEMBARK,
	CURSOR_FIX,
	CURSOR_GUARD,
	CURSOR_JAM,
	CURSOR_LOCKON,
	CURSOR_SCOUT,
	CURSOR_MENU,
	CURSOR_MOVE,
	CURSOR_NOTPOSSIBLE,
	CURSOR_PICKUP,
	CURSOR_SEEKREPAIR,
	CURSOR_SELECT,

	CURSOR_MAX,
} CURSOR;

enum CURSOR_TYPE
{
	CURSOR_16,
	CURSOR_32,
};

extern SDL_Cursor* init_system_cursor(CURSOR cur, enum CURSOR_TYPE type);
extern SDL_Cursor* init_system_cursor16(CURSOR cur);
extern SDL_Cursor* init_system_cursor32(CURSOR cur);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __INCLUDED_LIB_FRAMEWORK_CURSORS_H__
