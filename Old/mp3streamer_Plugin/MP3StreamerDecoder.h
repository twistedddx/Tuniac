#pragma once
#include "IAudioSource.h"

#include "FrameSplitter.h"
#include "Layer3Decoder.h"

#define BUFFERSIZE (2304 * sizeof(float))

class CMP3StreamerDecoder :
	public IAudioSource
{
protected:
	IAudioSourceHelper *		m_pHelper;
	HWND						m_hDialog;

	SOCKET						m_Socket;
	bool						m_bDie;

	bool						m_Release;

	bool						m_bMetaData;
	unsigned long				m_DataInterval;

	CFrameSplitter				m_Splitter;
	Frame						m_Frame;
	IMPEGDecoder			*	m_pDecoder;

	unsigned long				m_LastLayer;

	float					*	m_SampleBuffer;

	TCHAR						m_URL[4096];
	char						m_Server[1024];
	short						m_Port;
	char						m_Request[2048];

	HANDLE						m_hReadEnd;
	HANDLE						m_hWriteEnd;

	unsigned long				m_NetworkBufferSize;

	unsigned long				m_SampleRate;
	unsigned long				m_Channels;

	HANDLE						m_hThread;
	unsigned long				m_dwThreadID;

	static unsigned long __stdcall ThreadStub(void * in);
	unsigned long ThreadProc(void);

	bool ReadString(char *string, int maxlen);

	static LRESULT CALLBACK		BufferingDialogStub(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK			BufferingDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

public:
	CMP3StreamerDecoder(LPTSTR szSource, IAudioSourceHelper * pHelper);
	~CMP3StreamerDecoder(void);

	bool		OpenStream();
	bool		CloseStream();

public:
	void		Destroy(void);
	bool		GetLength(unsigned long * MS);
	bool		SetPosition(unsigned long * MS);
	bool		SetState(unsigned long State);
	bool		GetFormat(unsigned long * SampleRate, unsigned long * Channels);
	bool		GetBuffer(float ** ppBuffer, unsigned long * NumSamples);
};
