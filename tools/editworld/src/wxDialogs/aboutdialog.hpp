/*
	This file is part of Warzone 2100.
	Copyright (C) 2007  Giel van Schijndel
	Copyright (C) 2007-2009  Warzone Resurrection Project

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
	$Id: aboutdialog.hpp 6617 2009-02-10 18:01:48Z devurandom $
	$HeadURL: https://warzone2100.svn.sourceforge.net/svnroot/warzone2100/trunk/tools/editworld/src/wxDialogs/aboutdialog.hpp $
*/

#ifndef __INCLUDED_ABOUTDIALOG_HPP__
#define __INCLUDED_ABOUTDIALOG_HPP__

#include <wx/wxprec.h>

//(*Headers(AboutDialog)
#include <wx/dialog.h>
//*)

class AboutDialog: public wxDialog
{
	public:
		AboutDialog(wxWindow* parent = NULL);
};

#endif // __INCLUDED_ABOUTDIALOG_HPP__
