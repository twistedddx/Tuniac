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