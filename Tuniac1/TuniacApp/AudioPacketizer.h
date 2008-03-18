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

	bool IsBufferAvailable(void);
	bool GetBuffer(float * ToHere);

	void Reset(void);

	void Finished(void);
	bool IsFinished(void);
	bool AnyMoreBuffer(void);
};
