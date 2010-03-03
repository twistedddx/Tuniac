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
	dwTickCount = GetTickCount();

	dwDifference = (dwTickCount - dwLastCall);

	dwLastCall = dwTickCount;

	if(dwDifference >= dwMS)
		return 0;
	else
		return dwMS - dwDifference;
}
