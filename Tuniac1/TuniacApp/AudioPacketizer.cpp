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

#include "StdAfx.h"
#include ".\audiopacketizer.h"

#define NiceCloseHandle(handle)  if(handle) { CloseHandle(handle); handle = NULL; }

#ifdef SSE
#include "SSE_Utils.h"
#define ClearFloat(src, num)  SSE_ClearFloat(src, num)
#else
#define ClearFloat(src, num)  ZeroMemory(src, (num) * sizeof(float))
#endif

CAudioPacketizer::CAudioPacketizer(void) :
	m_PacketSize(0),
	m_hReadEnd(NULL),
	m_hWriteEnd(NULL),
	m_bFinished(false),
	m_ulBytesInPipe(0)

{
}

CAudioPacketizer::~CAudioPacketizer(void)
{
}

bool CAudioPacketizer::SetPacketSize(unsigned long Size)
{
	NiceCloseHandle(m_hReadEnd);
	NiceCloseHandle(m_hWriteEnd);

	m_PacketSize		= Size;
	m_PacketSizeBytes	= Size*sizeof(float);

	m_ulPipeSize		= m_PacketSizeBytes * 5;

	if(!CreatePipe(&m_hReadEnd, &m_hWriteEnd, NULL, m_ulPipeSize))
		return false;

	return(true);
}

bool CAudioPacketizer::WriteData(float * Data, unsigned long Samples)
{
	unsigned long BytesWritten;

	if(!WriteFile(m_hWriteEnd, Data, Samples*sizeof(float), &BytesWritten, NULL))
	{
		return false;
	}
	
	return true;
}

unsigned long CAudioPacketizer::BytesAvailable(void)
{
	unsigned long BytesAvailable;
	if(PeekNamedPipe(m_hReadEnd, NULL, 0, NULL, &BytesAvailable, NULL))
	{		
		return (m_ulPipeSize - BytesAvailable);
	}

	return 0;	
}


bool	CAudioPacketizer::GetBuffer(float * ToHere)
{
	unsigned long BytesAvailable = 0;
	if(PeekNamedPipe(m_hReadEnd, NULL, 0, NULL, &BytesAvailable, NULL))
	{
		if(BytesAvailable >= m_PacketSizeBytes)
		{
			unsigned long BytesRead;
			if(ReadFile(m_hReadEnd, ToHere, m_PacketSizeBytes, &BytesRead, NULL))
			{
				/*
				if(BytesRead != m_PacketSizeBytes)
				{
					//we didnt read as much as expected?
				}
				*/
				return true;
			}
		}
		else if(BytesAvailable && IsFinished())
		{
			ClearFloat(ToHere, m_PacketSize);

			unsigned long BytesRead;
			if(ReadFile(m_hReadEnd, ToHere, BytesAvailable, &BytesRead, NULL))
			{
				/*
				if(BytesRead != BytesAvailable)
				{
					//we didnt read as much as expected?
				}
				*/
				return true;
			}
		}
	}

	return false;
}

void CAudioPacketizer::Reset(void)
{
	//FlushFileBuffers
	SetPacketSize(m_PacketSize);
}

void CAudioPacketizer::Finished(void)
{
	m_bFinished = true;
}

bool CAudioPacketizer::IsFinished(void)
{
	return m_bFinished;
}

bool CAudioPacketizer::AnyMoreBuffer(void)
{
	unsigned long BytesAvailable;
	if(PeekNamedPipe(m_hReadEnd, NULL, 0, NULL, &BytesAvailable, NULL))
	{
		if(BytesAvailable)
		{
			return true;
		}
	}
	return false;
}

