#include "stdafx.h"

#include "OffsetTimer.h"


COffsetTimer::COffsetTimer(void)
{
	dwLastCall = GetTickCount();
}

COffsetTimer::~COffsetTimer(void)
{
}

unsigned long COffsetTimer::MSToSleepSinceLastCall(unsigned long dwMS)
{
	int difference = ((long long)GetTickCount() - (long long)dwLastCall);

	dwLastCall = GetTickCount();

	if(difference >= dwMS)
		return 0;
	else
		return dwMS - difference;

	return 0;
}
