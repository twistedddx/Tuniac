/*
	Copyright (C) 2003-2008 Tony Million

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
		claim that you wrote the original software. If you use this software
		in a product, an acknowledgment in the product documentation is required.

	2. Altered source versions must be plainly marked as such, and must not be
		misrepresented as being the original software.

	3. This notice may not be removed or altered from any source distribution.
*/
/*
	Modification and addition to Tuniac originally written by Tony Million
	Copyright (C) 2003-2012 Brett Hoyle
*/

#pragma once

class ISourceView
{
public:
	virtual bool	ShowSourceViewOptions(HWND hWndParent)		= 0;
	virtual bool	Update(void)								= 0;

	virtual bool	CreateSourceView(HWND hWndParent)			= 0;
	virtual bool	DestroySourceView(void)						= 0;

	virtual bool	ShowSourceView(bool bShow)					= 0;		// if bShow == false then hide
	virtual bool	MoveSourceView(int x, int y, int w, int h)	= 0;

	//Added Mark 7th October
	virtual bool	EditTrackInfo()								= 0;		//Edits the info for each source
	virtual bool	ShowCurrentItem()							= 0;
};