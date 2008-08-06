#include "StdAfx.h"
#include ".\mp3streamerdecoder.h"
#include "resource.h"

#define NETBUFFERSIZE		128*1024


CMP3StreamerDecoder::CMP3StreamerDecoder(LPTSTR	szSource, IAudioSourceHelper * pHelper) :
	m_hThread(NULL),
	m_NetworkBufferSize(NETBUFFERSIZE),
	m_pDecoder(NULL),
	m_hReadEnd(NULL),
	m_hWriteEnd(NULL),
	m_DataInterval(4096)
{
	StrCpy(m_URL, szSource);
	m_pHelper = pHelper;
}

CMP3StreamerDecoder::~CMP3StreamerDecoder(void)
{
}

void		CMP3StreamerDecoder::Destroy(void)
{
	m_bDie = true;
	CloseStream();
	delete this;
}

bool		CMP3StreamerDecoder::GetFormat(unsigned long * SampleRate, unsigned long * Channels)
{
	*SampleRate = m_SampleRate;
	*Channels = m_Channels;

	return true;
}

bool		CMP3StreamerDecoder::GetLength(unsigned long * MS)
{
	*MS = LENGTH_UNKNOWN;
	return true;
}

bool		CMP3StreamerDecoder::SetPosition(unsigned long * MS)
{
	return false;
}

bool		CMP3StreamerDecoder::SetState(unsigned long State)
{
	return true;
}

bool		CMP3StreamerDecoder::GetBuffer(float ** ppBuffer, unsigned long * NumSamples)
{
	unsigned long		BytesUsed = 0;

	static float SampleBuffer[2304];

	unsigned long BytesAvail = 0;

	if(m_bDie)
		return false;

	if(!PeekNamedPipe(m_hReadEnd, NULL, 0, NULL, &BytesAvail, NULL))
		return(false);

	if(BytesAvail < (m_NetworkBufferSize * 0.2))
	{
		unsigned long StartTime = GetTickCount();
		while(BytesAvail < (m_NetworkBufferSize * 0.66))
		{
			if(!PeekNamedPipe(m_hReadEnd, NULL, 0, NULL, &BytesAvail, NULL))
			{
				return(false);
			}

			if(m_bDie)
				return false;

			if((GetTickCount() - StartTime) > 30000)
				return false;

			Sleep(10);
		}
	}

	if(m_Splitter.Process(m_hReadEnd, m_Frame))
	{
		if(m_pDecoder == NULL)
		{
			switch(m_Frame.m_Header.GetLayer())
			{
				case LAYER3:
					{
						m_pDecoder = new CLayer3Decoder();
					}
					break;

				default:
					m_pHelper->LogConsoleMessage(TEXT("mp3 decoder"), TEXT("Unsupported Layer!"));
					return false;
					break;
			}
		}

		if(!m_pDecoder->ProcessFrame(&m_Frame, SampleBuffer, NumSamples))
			return false;

		*ppBuffer = SampleBuffer;

		return(true);
	}
	
	return false;
}

bool		CMP3StreamerDecoder::OpenStream()
{
	MSG				msg;
	unsigned long	BytesAvail = 0;


	m_Release = false;
	m_bDie = false;

//	if(m_pHelper)
//		m_hDialog = m_pHelper->CreateMsgDialog((DLGPROC)BufferingDialogStub, this);

	if(!CreatePipe(&m_hReadEnd, &m_hWriteEnd, NULL, m_NetworkBufferSize))
		return(false);

	m_hThread = CreateThread(	NULL,
								16384,
								ThreadStub,
								this,
								0,
								&m_dwThreadID);

	if(!m_hThread)
	{
		// fatal error
		return(false);
	}

	while(!m_Release)
	{
		if(m_bDie)
		{
			DestroyWindow(m_hDialog);
			m_hDialog = NULL;
			// Fatal Error
			return false;
		}

		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		Sleep(10);
	}

	//LogMessage(TEXT("Buffing..."));


	if(!PeekNamedPipe(m_hReadEnd, NULL, 0, NULL, &BytesAvail, NULL))
	{
		DestroyWindow(m_hDialog);
		m_hDialog = NULL;
		// Fatal Error
		return(false);
	}

	while(BytesAvail < (m_NetworkBufferSize * 0.66))
	{
		if(!PeekNamedPipe(m_hReadEnd, NULL, 0, NULL, &BytesAvail, NULL))
		{
			DestroyWindow(m_hDialog);
			m_hDialog = NULL;
			// Fatal Error
			return(false);
		}

		if(m_bDie)
		{
			DestroyWindow(m_hDialog);
			m_hDialog = NULL;
			// Fatal Error
			return false;
		}

		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		Sleep(10);
	}

	if(m_Splitter.Process(m_hReadEnd, m_Frame))
	{

		m_SampleRate = m_Frame.m_Header.GetSampleFrequency();
		m_Channels = m_Frame.m_Header.GetChannels();
	}

	DestroyWindow(m_hDialog);
	m_hDialog = NULL;

	return true;
}

bool		CMP3StreamerDecoder::CloseStream()
{
	if(m_hThread)
	{
		int Count = 10;
		while(!PostThreadMessage(m_dwThreadID, WM_QUIT, 0, 0) && Count--)
			Sleep(10);

		unsigned long BytesAvail;

		unsigned char temp[2048];
		PeekNamedPipe(m_hReadEnd, NULL, 0, NULL, &BytesAvail, NULL);
		while(BytesAvail > 2048)
		{
			unsigned long BytesRead;
			if(!ReadFile(m_hReadEnd, temp, 2048, &BytesRead, NULL))
				break;

			PeekNamedPipe(m_hReadEnd, NULL, 0, NULL, &BytesAvail, NULL);

			Count = 10;
			while(!PostThreadMessage(m_dwThreadID, WM_QUIT, 0, 0) && Count--)
				Sleep(10);
		}

		if(WaitForSingleObject(m_hThread, 1000) == WAIT_TIMEOUT)
		{
			TerminateThread(m_hThread, 0);
		}

		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	return true;
}

unsigned long CMP3StreamerDecoder::ThreadStub(void * in)
{
	CMP3StreamerDecoder * pDec = (CMP3StreamerDecoder *)in;

	return(pDec->ThreadProc());
}

unsigned long CMP3StreamerDecoder::ThreadProc(void)
{
#define MAXREADSIZE		2048

	MSG					msg;

	unsigned long		BytesWritten;
	unsigned char	*	buffer;
	unsigned long		BufferLevel = m_DataInterval;

	LPTSTR szArtist;
	LPTSTR szGenre;

//	WORD			wsaVersion;
	WSADATA			wsaData;
    SOCKADDR_IN		addr;

	if(WSAStartup(MAKEWORD(1,1), &wsaData))
	{
		//LogMessage(TEXT("Error initializing winsock."));
		m_bDie = true;
		return false;
	}

REURL:

	TCHAR Address[2048];
	TCHAR Port[8];
	TCHAR Request[2048];

	bool bChangedPort = false;
	bool bChangesAddress= false;
	LPTSTR	szDest = Address;

	LPTSTR blah = &m_URL[7];
	while(lstrlen(blah))
	{
		LPTSTR oldDest = szDest;
		if((*blah == L':') && !bChangedPort)
		{
			*szDest = L'\0';
			szDest = Port;
			bChangedPort = true;
		}
		else if((*blah == L'/') && !bChangesAddress)
		{
			*szDest = L'\0';
			szDest = Request;
			bChangedPort = true;
			bChangesAddress = true;
		}
		else
		{
			*szDest = *blah;
		}
		if(oldDest == szDest)
			szDest = &szDest[1];
		blah = &blah[1];
	}

	*szDest = L'\0';

	TCHAR grrr[2049];
	wsprintf( grrr, TEXT("/%s"), Request);

	WideCharToMultiByte(CP_ACP, 0, Address, -1, m_Server, 1024, NULL, FALSE);
	WideCharToMultiByte(CP_ACP, 0, grrr, -1, m_Request, 2048, NULL, FALSE);
	if(strlen(m_Request)==0)
		strcpy(m_Request, "/");



	m_Port = _wtoi(Port);


	/* The WinSock DLL is acceptable. Proceed. */
	m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if(m_Socket == SOCKET_ERROR)
	{
		//LogMessage(TEXT("Error Creating Socket."));
		m_bDie = true;

		return(false);
	}

	// Fill in the host information
	addr.sin_family			= AF_INET;
	addr.sin_port			= htons(m_Port);
	addr.sin_addr.s_addr	= inet_addr(m_Server);

	if(addr.sin_addr.s_addr == INADDR_NONE) 	// The address wasn't in numeric form, resolve it
	{
		struct hostent *host;
		host = gethostbyname(m_Server);	// Get the IP address of the server
		if(host == NULL)
		{
			//LogMessage(TEXT("Error Resolving Address."));
			m_bDie = true;
			return false;
		}

		memcpy(&addr.sin_addr, host->h_addr, host->h_length);
	}

	// connect to server here!
	if(m_hDialog)
	{
		//LogMessage(TEXT("Connecting..."));
	}

	int noblock= 1;
	ioctlsocket(m_Socket, FIONBIO, (u_long *)&noblock);

	int ret = connect(m_Socket, (struct sockaddr *) &addr, sizeof(addr));
	if(ret == SOCKET_ERROR)
	{
		if(WSAGetLastError() == WSAEWOULDBLOCK)
		{
			timeval tv;
			tv.tv_sec = 10;
			tv.tv_usec = 0;

			fd_set set;

			set.fd_count = 1;
			set.fd_array[0] = m_Socket;
			if(select(0, &set, &set, &set, &tv) <= 0)
			{
				//LogMessage(TEXT("Error Connecting To Server."));
				m_bDie = true;
				return false;
			}
		}
		else
		{
			//LogMessage(TEXT("Error Connecting To Server."));
			m_bDie = true;
			return false;
		}
	}

	noblock= 0;
	ioctlsocket(m_Socket, FIONBIO, (u_long *)&noblock);

	if(m_hDialog)
	{
		//LogMessage(TEXT("Connected, Sending Request...\r\n"));
	}


	char szMessage[2048];
	strcpy(szMessage, "GET ");
	strcat(szMessage, m_Request);
	strcat(szMessage, " HTTP/1.0\r\n");
	strcat(szMessage, "Host: ");
	strcat(szMessage, m_Server);
	strcat(szMessage, "\r\n");
	strcat(szMessage, "User-Agent: TuniacBETA/0.1\r\n");
	strcat(szMessage, "Accept: */*\r\n");
	strcat(szMessage, "Icy-MetaData: 1\r\n");
	strcat(szMessage, "Connection: close\r\n");
	strcat(szMessage, "\r\n");

	ret = send(m_Socket, szMessage, strlen(szMessage), 0);
	if(ret == SOCKET_ERROR)
	{
		//LogMessage(TEXT("Error Sending Request."));
		m_bDie = true;
		return false;
	}

	//LogMessage(TEXT("Request Sent, Awaiting Response..."));


	m_bMetaData = false;

	if(!ReadString(szMessage, 2048))
	{
		//LogMessage(TEXT("No Response From Server."));
		m_bDie = true;
		return false;
	}

	// uh oh... redirect.
	if(strncmp(szMessage, "HTTP/1.0 30", 11) == 0)
	{
		while(strlen(szMessage))
		{
			if(_strnicmp(szMessage, "Location: ", 10) == 0)
			{
				MultiByteToWideChar(CP_ACP, 0, &szMessage[10], -1, m_URL, 4096);

				TCHAR szLogMsg[512];
				StrCpy(szLogMsg, TEXT("Redirect To: "));
				StrCatN(szLogMsg, m_URL, 512);

				//LogMessage(szLogMsg);

				closesocket(m_Socket);
				goto REURL;

			}

			if(!ReadString(szMessage, 2048))
			{
				//LogMessage(TEXT("No Response From Server."));
				m_bDie = true;
				return false;
			}
		}
	}

	while(strlen(szMessage))
	{
		if(_strnicmp(szMessage, "icy-notice", 10) == 0)
		{
			TCHAR szLogMsg[2048];
			MultiByteToWideChar(CP_ACP, 0, szMessage, -1, szLogMsg, 2048);
			//LogMessage(szLogMsg);
		}
		if(_strnicmp(szMessage, "icy-metaint:", 12) == 0)
		{
			m_bMetaData = true;
			m_DataInterval = atoi(&szMessage[12]);

			//LogMessage(TEXT("Got Metadata Interval."));
		}
		if(_strnicmp(szMessage, "icy-name:", 9) == 0)
		{
			TCHAR szName[128];
			MultiByteToWideChar(CP_ACP, 0, szMessage, -1, szName, 128);
			szArtist = StrStr(szName, TEXT("icy-name:"));
			szArtist += wcslen(TEXT("icy-name:"));
		}
		if(_strnicmp(szMessage, "icy-genre:", 10) == 0)
		{
			TCHAR szName[128];
			MultiByteToWideChar(CP_ACP, 0, szMessage, -1, szName, 128);
			szGenre = StrStr(szName, TEXT("icy-genre:"));
			szGenre += wcslen(TEXT("icy-genre:"));
		}

		if(!ReadString(szMessage, 2048))
		{
			//LogMessage(TEXT("No Response From Server."));
			m_bDie = true;
			return false;
		}
	}
	//LogMessage(TEXT("Connection Complete, Streaming..."));


	m_Release = true;

	BufferLevel = m_DataInterval;
	buffer = (unsigned char*)malloc(MAXREADSIZE);

	while(true)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{
				break;
			}
		}

		if(BufferLevel)
		{
			int ThisTime = min(MAXREADSIZE, BufferLevel);

			int ret = recv(m_Socket, (char *)buffer, ThisTime, 0);
			if(ret)
			{
				BufferLevel -= ret;
				WriteFile(m_hWriteEnd, buffer, ret, &BytesWritten, NULL);
			}
			else
			{
				break;
			}
		}
		else
		{
			if(m_bMetaData)
			{
				unsigned char c = 0;

				int ret = recv(m_Socket, (char *)&c, 1, 0);
				if(ret)
				{
					int iMetaSize = c * 16;

					if(iMetaSize)
					{
						char * tbuf = (char *)malloc(iMetaSize);

						recv(m_Socket, tbuf, iMetaSize, 0);

						TCHAR * szMeta = (TCHAR *)malloc(sizeof(TCHAR) * iMetaSize);
						MultiByteToWideChar(CP_ACP, 0, tbuf, -1, szMeta, iMetaSize);
						free(tbuf);
						szMeta[iMetaSize - 1] = L'\0';
						// StreamTitle='title';StreamUrl='no idea what this is';

						LPTSTR szStartTitle = StrStr(szMeta, TEXT("StreamTitle='"));
						if(szStartTitle != NULL)
						{
							szStartTitle += wcslen(TEXT("StreamTitle='"));

							LPTSTR szEndTitle = StrStr(szStartTitle, L"';");
							while(szEndTitle != NULL && (szEndTitle - 1)[0] == L'\\')
								szEndTitle = StrStr(szEndTitle + 1, L"';");

							if(szEndTitle != NULL)
							{
								szEndTitle[0] = L'\0';
							}
							m_pHelper->UpdateStreamTitle(this, szStartTitle, FIELD_TITLE);
							m_pHelper->UpdateStreamTitle(this, szArtist, FIELD_ARTIST);
							m_pHelper->UpdateStreamTitle(this, szGenre, FIELD_GENRE);
						}
						free(szMeta);
					}
				}
			}

			BufferLevel = m_DataInterval;
		}
	}

	free(buffer);

	m_bDie = true;

	if(m_hWriteEnd)
	{
		CloseHandle(m_hWriteEnd);
		m_hWriteEnd = NULL;
	}

	if(m_hReadEnd)
	{
		CloseHandle(m_hReadEnd);
		m_hReadEnd = NULL;
	}

	closesocket(m_Socket);

	WSACleanup();

	return(0);
}

bool CMP3StreamerDecoder::ReadString(char *string, int maxlen)
{
	int pos = 0;

	while(pos < maxlen) 
	{
		if(recv(m_Socket, string+pos, 1, 0) == 1) 
		{
			pos++;
			if(string[pos-1] == '\n') 
			{
				string[pos-2] = 0;
				break;
			}
		}
		else 
		{
			return false;
		}
	}

	return true;
}
