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

	$Revision: 6617 $
	$Id: debugprint.cpp 6617 2009-02-10 18:01:48Z devurandom $
	$HeadURL: https://warzone2100.svn.sourceforge.net/svnroot/warzone2100/trunk/tools/editworld/src/debugprint.cpp $
*/

#include "windows.h"
#include "windowsx.h"
#include <fstream>
#include "tchar.h"
#include "debugprint.hpp"

#ifdef _DEBUG

extern void DebugWinPrint(char *String);

static std::ostream* DebugStream = NULL;

void DebugOpen(const char* LogName)
{
	DebugClose();

	DebugStream = new std::ofstream(LogName, std::ios_base::binary);

	*DebugStream << "This build : " << __DATE__ << " " << __TIME__ << "\n" << std::endl;
}

void DebugClose()
{
	delete DebugStream;
	DebugStream = NULL;
}

void DebugPrint(const TCHAR *format, ...)
{
	TCHAR buf[4096];

	va_list args;
	va_start(args,format);
	_vsntprintf(buf,4096,format,args);
	va_end(args);

	// Display the debug string in any debugger (WINAPI function)
	OutputDebugString(buf);

	if(DebugStream)
		*DebugStream << buf << std::endl;
}

#endif
