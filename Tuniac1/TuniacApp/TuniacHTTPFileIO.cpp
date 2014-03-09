#include "stdafx.h"

#include "TuniacHTTPFileIO.h"

#include "generaldefs.h"

#define MAXREADSIZE		2048
#define RINGBUFFERSIZE	65536


static bool ReadString(int socket, char *tohere, int maxlen)
{
	int pos = 0;
	
	while(pos < maxlen) 
	{
		if(recv(socket, tohere+pos, 1, 0) == 1) 
		{
			pos++;
			if(tohere[pos-1] == '\n') 
			{
				tohere[pos-2] = 0;
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

CTuniacHTTPFileIO::CTuniacHTTPFileIO(void)
{
}

CTuniacHTTPFileIO::~CTuniacHTTPFileIO(void)
{
}

bool CTuniacHTTPFileIO::Open(LPTSTR szFilename)
{
	bool bChangedPort = false;
	bool bChangesAddress= false;
	char *	szDest = Address;
	
	char * blah = (char *)szFilename;
	while(strnlen_s(blah, MAX_PATH))
	{
		char * oldDest = szDest;
		if((*blah == ':') && !bChangedPort)
		{
			*szDest = '\0';
			szDest = Port;
			bChangedPort = true;
		}
		else if((*blah == '/') && !bChangesAddress)
		{
			*szDest = '\0';
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
	*szDest = '\0';
	
	if(strnlen_s(Request, 2048) == 0)
		strcpy_s(Request, 2048, "/");

	
	if(bChangedPort)
	{
		uPort = atoi(Port);
	}
	else {
		uPort = 80;
	}

	
	if(0 == strnlen_s(Address, 2048))
		return false;
	
	memset(contenttype, 0, 1024);
	
	m_rbuf.setBufSize(RINGBUFFERSIZE);
	m_rbuf.flush();
	
	/* The WinSock DLL is acceptable. Proceed. */
	m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if(m_Socket == -1)
	{
		DLog("socket fail\n");
		return(false);
	}
	
#ifdef _MSC_VER
	// TODO: windows socket timeout shit
#else
	struct timeval tv;
	tv.tv_sec = 10;  /* 10 Secs Timeout */
	setsockopt(m_Socket, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval));
#endif
	
	struct sockaddr_in		addr;
	
	// Fill in the host information
	addr.sin_family			= AF_INET;
	addr.sin_port			= htons(uPort);
	addr.sin_addr.s_addr	= inet_addr(Address);
	
	if(addr.sin_addr.s_addr == INADDR_NONE) 	// The address wasn't in numeric form, resolve it
	{
		struct hostent *host;
		host = gethostbyname(Address);	// Get the IP address of the server
		if(host == NULL)
		{
			DLog("cannot resolve host");
			return false;
		}
		
		memcpy(&addr.sin_addr, host->h_addr, host->h_length);
	}
		
	int ret = connect(m_Socket, (struct sockaddr *) &addr, sizeof(addr));
	if(ret == -1)
	{
		DLog("connection failed");
		
		return false;
	}	
	
	char szMessage[4096];
	strcpy_s(szMessage, 4096, "GET ");
	strcat_s(szMessage, 4096, Request);
	// APPARENTLY THIS ISN'T LIKED EH???
	//strcat(szMessage, " HTTP/1.0\r\n");
	strcat_s(szMessage, 4096, "\r\n");
	strcat_s(szMessage, 4096, "Host: ");
	strcat_s(szMessage, 4096, Address);
	strcat_s(szMessage, 4096, "\r\n");
	strcat_s(szMessage, 4096, "User-Agent: Tuniac2BETA/0.1\r\n");
	strcat_s(szMessage, 4096, "Accept: */*\r\n");
	strcat_s(szMessage, 4096, "Icy-MetaData: 1\r\n");
	strcat_s(szMessage, 4096, "Connection: close\r\n");
	strcat_s(szMessage, 4096, "\r\n\r\n");
	
	ret = send(m_Socket, szMessage, strnlen_s(szMessage, 4096), 0);
	if(ret == -1)
	{
		DLog("socket send fail");
		return false;
	}
	
	m_bMetaData = false;
		
	if(!ReadString(m_Socket, szMessage, 2048))
	{
		DLog("socket receive fail");
		return false;
	}
	
	while(strnlen_s(szMessage, 4096))
	{		
		if(strncasecmp(szMessage, "icy-notice", 10) == 0)
		{
			
		}
		if(strncasecmp(szMessage, "icy-metaint:", 12) == 0)
		{
			m_bMetaData = true;
			m_DataInterval = atoi(&szMessage[12]);
		}
		if(strncasecmp(szMessage, "icy-name:", 9) == 0)
		{
		}
		if(strncasecmp(szMessage, "icy-genre:", 10) == 0)
		{
		}
		if(strncasecmp(szMessage, "content-type:", 13) == 0)
		{
			strcpy_s(contenttype, 1024, szMessage+13);
		}
		
		if(!ReadString(m_Socket, szMessage, 2048))
		{
			DLog("socket receive fail");
			return false;
		}
	}
	
	// now pass all that shit off to a thread to manage for us - thanks..
	
	m_ThreadRun = true;
	m_ThreadEnded = false;
	m_streamThread.StartThread(threadstub, this);
	
	//TODO: we should probably block here untill the buffer is full or something?
	int avail = m_rbuf.getAvailable();
	while ((avail < RINGBUFFERSIZE/2) && m_ThreadRun)
	{
		Sleep(10);	// 10MS
		avail = m_rbuf.getAvailable();
	}

	return true;
}

void CTuniacHTTPFileIO::Destroy(void)
{
	Close();
	delete this;
}


bool CTuniacHTTPFileIO::Close()
{
	m_ThreadRun = false;
	m_streamThread.Join();
	
	closesocket(m_Socket);
	
	return true;
}


bool CTuniacHTTPFileIO::Read(unsigned __int64 numberofbytes, void * pData, unsigned __int64 * pBytesRead)
{
	// if we're at the end of the buffer
	uint64_t avail = m_rbuf.getAvailable();
	if(avail < numberofbytes && 	m_ThreadEnded)
	{
		m_rbuf.read((uint8_t*)pData, avail);
		*pBytesRead	=	avail;
		return true;
	}
	
	while(!m_rbuf.canRead(numberofbytes))
	{
		Sleep(10);
		
		// if we are stuck in here we can assume that avail<size
		if(m_ThreadEnded)
		{
			avail = m_rbuf.getAvailable();			
			m_rbuf.read((uint8_t*)pData, avail);
			*pBytesRead	=	avail;
			return true;			
		}
	}
	m_rbuf.read((uint8_t*)pData, numberofbytes);
	*pBytesRead	=	numberofbytes;
	return true;
}

bool CTuniacHTTPFileIO::Seek(unsigned __int64 Offset, bool bFromEnd)
{
	return false;
}

void CTuniacHTTPFileIO::Tell(unsigned __int64 * pCurrentOffset)
{
	*pCurrentOffset = 0;
}

void CTuniacHTTPFileIO::Size(unsigned __int64 * pSize)
{
	*pSize = 0;
}

bool CTuniacHTTPFileIO::IsEOF(void)
{
	return m_ThreadEnded;
}



int CTuniacHTTPFileIO::threadstub(void * pData)
{
	CTuniacHTTPFileIO * pStream = (CTuniacHTTPFileIO*)pData;
	
	return pStream->thread();
}

int CTuniacHTTPFileIO::thread(void)
{
	uint32_t				BufferLevel;

	BufferLevel = m_DataInterval;
	static uint8_t buffer[MAXREADSIZE];
	
	m_rbuf.flush();
	
	// ideally I'd replace this all with overlapped IO and/or GCD code except for fucking windows
	
	while (m_ThreadRun) 
	{

		if(BufferLevel)
		{
			int ThisTime = MIN(MAXREADSIZE, BufferLevel);
			
			int ret = recv(m_Socket, (char *)buffer, ThisTime, 0);
			if(ret > 0)
			{
				BufferLevel -= ret;
				while (!m_rbuf.canWrite(ret) && m_ThreadRun) 
				{
					// sleep 10MS	- which is an eternity in computer time!
					Sleep(10);
				}
				
				m_rbuf.write(buffer, ret);
			}
			else if(ret == -1)
			{
				// TODO: use notification interface that a problem occurred
				m_ThreadRun = false;
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
						char tbuf[4081];
						memset(tbuf, 0, 4081);
						
						uint32_t read = 0;
						while (iMetaSize - read && m_ThreadRun) 
						{
							ret = recv(m_Socket, tbuf+read, iMetaSize-read, 0);
							if(ret == -1)
							{
								m_ThreadRun = false;
								break;
							}
							
							read += ret;
						}
						
						/*
						TCHAR * szMeta = (TCHAR *)malloc(sizeof(TCHAR) * iMetaSize);
						MultiByteToWideChar(CP_ACP, 0, tbuf, -1, szMeta, iMetaSize);
						free(tbuf);
						szMeta[iMetaSize - 1] = L'\0';
						// StreamTitle='title';StreamUrl='no idea what this is';
						
						LPTSTR szStartTitle = StrStr(szMeta, TEXT("StreamTitle='"));
						if(szStartTitle != NULL)
						{
							szStartTitle += wcsnlen_s(TEXT("StreamTitle='"));
							
							LPTSTR szEndTitle = StrStr(szStartTitle, L"';");
							while(szEndTitle != NULL && (szEndTitle - 1)[0] == L'\\')
								szEndTitle = StrStr(szEndTitle + 1, L"';");
							
							if(szEndTitle != NULL)
							{
								szEndTitle[0] = L'\0';
							}
							m_pHelper->UpdateMetaData(m_URL, szStartTitle, FIELD_TITLE);
							m_pHelper->UpdateMetaData(m_URL, szArtist, FIELD_ARTIST);
							m_pHelper->UpdateMetaData(m_URL, szGenre, FIELD_GENRE);
						}
						free(szMeta);
						 */
					}
				}
			}
			
			
			BufferLevel = m_DataInterval;
		}
		
	}
	
	m_ThreadEnded = true;
	
	return NULL;
}
