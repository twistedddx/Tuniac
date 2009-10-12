#pragma once

class COffsetTimer
{
protected:

	unsigned long	dwLastCall;

public:
	COffsetTimer(void);
	~COffsetTimer(void);

	unsigned long MSToSleepSinceLastCall(unsigned long dwMS);
};
