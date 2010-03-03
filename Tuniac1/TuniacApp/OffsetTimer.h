#pragma once

class COffsetTimer
{
protected:

	DWORD	dwTickCount;
	DWORD	dwLastCall;
	DWORD	dwDifference;

public:
	COffsetTimer(void);
	~COffsetTimer(void);

	unsigned long MSToSleepSinceLastCall(unsigned long dwMS);
};
