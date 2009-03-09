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

class CAudioPacketizer
{
protected:
	unsigned long	m_PacketSize;
	unsigned long	m_PacketSizeBytes;

	unsigned long	m_ulBytesInPipe;
	unsigned long	m_ulPipeSize;

	bool			m_bFinished;

	HANDLE			m_hReadEnd;
	HANDLE			m_hWriteEnd;

public:
	CAudioPacketizer(void);
	~CAudioPacketizer(void);

	bool SetPacketSize(unsigned long Size);

	bool WriteData(float * Data, unsigned long Samples);

	unsigned long BytesAvailable(void);
	bool GetBuffer(float * ToHere);

	void Reset(void);

	void Finished(void);
	bool IsFinished(void);
	bool AnyMoreBuffer(void);
};
