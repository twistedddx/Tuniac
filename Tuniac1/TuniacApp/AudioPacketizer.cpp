#include "StdAfx.h"
#include ".\audiopacketizer.h"

#define CopyFloat(dst, src, num) CopyMemory(dst, src, (num) * sizeof(float))

#define NiceCloseHandle(handle)  if(handle) { CloseHandle(handle); handle = NULL; }

CAudioPacketizer::CAudioPacketizer(void) :
	m_PacketSize(0),
	m_hReadEnd(NULL),
	m_hWriteEnd(NULL),
	m_bFinished(false)

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
	if(!CreatePipe(&m_hReadEnd, &m_hWriteEnd, NULL, (Size*sizeof(float))*3))
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

bool	CAudioPacketizer::IsBufferAvailable(void)
{
	unsigned long BytesAvailable;

	if(IsFinished())
	{
		// return true here anyway, because we'll always supply an empty buffer for as long as it takes
		return true;
	}


	if(PeekNamedPipe(m_hReadEnd, NULL, 0, NULL, &BytesAvailable, NULL))
	{
		if(BytesAvailable > m_PacketSizeBytes)
		{
			return true;
		}
	}

	return false;
}

bool	CAudioPacketizer::GetBuffer(float * ToHere)
{
	if(IsFinished())
	{
		// ok this is the last packet. 
		//lets read whatever else is in the pipe and fill the rest with zeros
		// then return true!!

		// lets wipe the entire buffer ahead of time then overwrite it at the beginning!
		ZeroMemory(ToHere, m_PacketSizeBytes);

		unsigned long BytesAvailable;
		unsigned long BytesRead;
		if(PeekNamedPipe(m_hReadEnd, NULL, 0, NULL, &BytesAvailable, NULL))
		{
			if(BytesAvailable)
			{
				if(ReadFile(m_hReadEnd, ToHere, BytesAvailable, &BytesRead, NULL))
				{
					// ok we read the data now to wipe the rest of the buffer.
				}
			}
		}

		return true;
	}

	if(IsBufferAvailable())
	{
		unsigned long BytesRead;

		if(ReadFile(m_hReadEnd, ToHere, m_PacketSizeBytes, &BytesRead, NULL))
		{
			return true;
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

