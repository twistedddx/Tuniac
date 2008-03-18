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

#pragma once

#define SYSEVENTS_TIMERID		0x500

class CSysEvents
{
protected:
	bool			m_IsNT;
	bool			m_WasPlaying;

	bool			m_WorkstationLocked;
	bool			m_ScreensaverActive;


	bool			IsWorkstationLocked(void);
	bool			IsScreensaverActive(void);


public:
	CSysEvents(void);
	~CSysEvents(void);

	bool			Initialize(void);
	bool			Shutdown(void);

	void			CheckSystemState(void);

};