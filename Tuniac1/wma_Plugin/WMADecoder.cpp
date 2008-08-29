#include "StdAfx.h"
#include "wmadecoder.h"

CWMADecoder::CWMADecoder(void)
{
}

CWMADecoder::~CWMADecoder(void)
{
}

bool CWMADecoder::Open(LPTSTR szSource)
{
 	CoInitialize(NULL);
	m_ISyncReader = NULL;
	m_pINSSBuffer = NULL;
	m_theMediaType = NULL;
	m_IAudioOutputProps = NULL;
	pHeaderInfo = NULL;
	pEditor = NULL;
	m_ulTotalTimeInMS = LENGTH_UNKNOWN;
	m_bIsSeekable = 0;
	m_wAudioStreamNumber = 0;
	m_dwAudioOutputNumber = 0;
	m_theOutputsCount = 0;
	ulChannels = 0;
	ulSampleRate = 0;
	ulBitrate = 0;
    WMT_ATTR_DATATYPE Type;
	WORD wStreamNum = 0;
	BYTE* pbValue = NULL;
    WORD DataSize = sizeof(long);

	hr = S_OK;

    hr  = WMCreateSyncReader(NULL,0,&m_ISyncReader);
	if(hr!=S_OK)
	{
		Close();
		return false;
	}

	hr = m_ISyncReader->Open(szSource);
	if(hr!=S_OK)
	{
		Close();
		return false;
	}

    DWORD m_theOutputCount; 
    m_ISyncReader->GetOutputCount(&m_theOutputsCount);

	DWORD theSize;
	for (int i=0;i<m_theOutputsCount;i++)
	{
		if(m_IAudioOutputProps)
			m_IAudioOutputProps->Release();
		m_ISyncReader->GetOutputProps(i,&m_IAudioOutputProps); 
		m_IAudioOutputProps->GetMediaType(NULL,&theSize);
		m_theMediaType = ( WM_MEDIA_TYPE* ) new BYTE[theSize ];
		m_IAudioOutputProps->GetMediaType(m_theMediaType,&theSize);

		if( WMMEDIATYPE_Audio == m_theMediaType->majortype)
		{
			if( WMFORMAT_WaveFormatEx == m_theMediaType->formattype)
			{
				//only handle audio files
				m_dwAudioOutputNumber = i;
				m_ISyncReader->GetStreamNumberForOutput(m_dwAudioOutputNumber,&m_wAudioStreamNumber);
				WAVEFORMATEX * waveFormatEx = ( WAVEFORMATEX * )m_theMediaType->pbFormat;
				ulChannels = waveFormatEx->nChannels;
				ulSampleRate = waveFormatEx->nSamplesPerSec;
				ulBitrate = waveFormatEx->wBitsPerSample;
			}
		}
	}
	if(m_IAudioOutputProps)
	{
		m_IAudioOutputProps->Release();
		m_IAudioOutputProps = NULL;
	}

	hr = m_ISyncReader->QueryInterface(IID_IWMHeaderInfo3,(void**)&pHeaderInfo);
	if( hr != S_OK )
	{
		Close();
		return false;
	}

	hr = pHeaderInfo->GetAttributeByName( &wStreamNum, g_wszWMDuration, &Type, NULL, &DataSize);
	if( hr == S_OK && hr != ASF_E_NOTFOUND )
	{
		pbValue = new BYTE[ DataSize ];
		pHeaderInfo->GetAttributeByName( &wStreamNum, g_wszWMDuration, &Type, pbValue, &DataSize);
		if( NULL != pbValue )
			m_ulTotalTimeInMS = *( QWORD* )pbValue / 10000;
		delete [] pbValue;
	}

	hr = pHeaderInfo->GetAttributeByName( &wStreamNum, g_wszWMSeekable, &Type, NULL, &DataSize);
	if( hr ==S_OK && hr != ASF_E_NOTFOUND )
	{
		pbValue = new BYTE[ DataSize ];
		hr = pHeaderInfo->GetAttributeByName( &wStreamNum, g_wszWMSeekable, &Type, pbValue, &DataSize);
		if( NULL != pbValue )
			m_bIsSeekable = *( BOOL* )pbValue;
		delete [] pbValue;
	}
	if(pHeaderInfo)
	{
		pHeaderInfo->Release();
		pHeaderInfo = NULL;
	}


	WMT_STREAM_SELECTION	wmtSS = WMT_ON;
    hr = m_ISyncReader->SetStreamsSelected( 1, &m_wAudioStreamNumber, &wmtSS );
    if ( FAILED( hr ) )
	{
		Close();
		return false;
	}

	hr = m_ISyncReader->SetReadStreamSamples(m_wAudioStreamNumber,FALSE);
	if(hr!=S_OK)
	{
		Close();
		return false;
	}

	m_ISyncReader->SetRange(0, 0);


	if(ulBitrate == 8)
		m_divider = 128.0f;
	else if(ulBitrate == 16)
		m_divider = 32767.0f;
	else if(ulBitrate == 24)
		m_divider = 8388608.0f;
	else if(ulBitrate == 32)
		m_divider = 2147483648.0f;

	return(true);
}

bool CWMADecoder::Close()
{
	if(m_pINSSBuffer)
	{
		m_pINSSBuffer->Release();
		m_pINSSBuffer = NULL;
	}
	if(m_ISyncReader)
	{
		m_ISyncReader->Close();
		m_ISyncReader->Release();
		m_ISyncReader = NULL;
	}
	CoUninitialize();
	return(true);
}

void		CWMADecoder::Destroy(void)
{
	Close();
	delete this;
}

bool		CWMADecoder::GetFormat(unsigned long * SampleRate, unsigned long * Channels)
{
	*SampleRate = ulSampleRate;
	*Channels	= ulChannels;

	return(true);
}

bool		CWMADecoder::GetLength(unsigned long * MS)
{
	*MS = m_ulTotalTimeInMS;

	return(true);
}

bool		CWMADecoder::SetPosition(unsigned long * MS)
{
	if(m_bIsSeekable)
		m_ISyncReader->SetRange((QWORD)*MS*10000, 0);

	return(true);
}

bool		CWMADecoder::SetState(unsigned long State)
{
	return(true);
}

bool		CWMADecoder::GetBuffer(float ** ppBuffer, unsigned long * NumSamples)
{
	*NumSamples =0;

	QWORD cnsSampleTime = 0;
	QWORD cnsSampleDuration = 0;
	DWORD dwFlags = 0;
	DWORD dwOutputNum = 0;
	WORD wStream = 0;

	if(m_pINSSBuffer)
	{
		m_pINSSBuffer->Release();
		m_pINSSBuffer = NULL;
	}

	hr = m_ISyncReader->GetNextSample(m_wAudioStreamNumber,
										&m_pINSSBuffer,
										&cnsSampleTime,
										&cnsSampleDuration,
										&dwFlags,
										NULL,//&dwOutputNum
										NULL);//&wStream

	if(hr == S_OK)
	{
		BYTE        *buffer = NULL;
		DWORD		m_audioBufferLength;
		m_pINSSBuffer->GetBufferAndLength(&buffer,&m_audioBufferLength);

		unsigned long numSamples = m_audioBufferLength / ulChannels;

		short	* pData		= (short *)buffer;
		float	* pBuffer	= m_Buffer;
	
		for(unsigned long x=0; x<numSamples;x++)
		{
			*pBuffer = (*pData) / m_divider;
			pData++;
			pBuffer++;
		}
		*ppBuffer = m_Buffer;

		*NumSamples = numSamples;


	}
	else
	{
		Close();
		return false;
	}
	/*
	else if(hr == NS_E_NO_MORE_SAMPLES)
		return false;
	else if(hr == E_UNEXPECTED)
		return false;
	else if(hr == E_INVALIDARG)
		return false;
	else if(hr == NS_E_INVALID_REQUEST)
		return false;
*/
	return(true);
}