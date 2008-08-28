#include "StdAfx.h"
#include "wmadecoder.h"

CWMADecoder::CWMADecoder(void)
: m_theMediaType(NULL)
, m_theOutputsCount(0)
, m_IAudioOutputProps(0)
, m_iAudioOutputNumber(0)
, m_iAudioStreamNumber(0)
{
	CoInitialize(NULL);
	m_ISyncReader = NULL;
	m_pINSSBuffer = NULL;
	m_ulTotalTimeInMS = 0;
	m_bIsSeekable = 0;
}

CWMADecoder::~CWMADecoder(void)
{
}

bool CWMADecoder::Open(LPTSTR szSource)
{
                   
    hr  = WMCreateSyncReader(NULL,0,&m_ISyncReader);
	if(hr!=S_OK)
		return false;

	hr = m_ISyncReader->Open(szSource);
	if(hr!=S_OK)
		return false;

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
				m_iAudioOutputNumber = i;
				m_ISyncReader->GetStreamNumberForOutput(m_iAudioOutputNumber,&m_iAudioStreamNumber);
				WAVEFORMATEX *waveFormatEx = (WAVEFORMATEX*)m_theMediaType->pbFormat;
				ulChannels = waveFormatEx->nChannels;
				ulSampleRate = waveFormatEx->nSamplesPerSec;
			}
		}
	}
	if(m_IAudioOutputProps)
		m_IAudioOutputProps->Release();

	hr = m_ISyncReader->SetReadStreamSamples(m_iAudioStreamNumber,FALSE);
	if(hr!=S_OK)
		return false;

	IWMMetadataEditor		*	pEditor;
	hr = WMCreateEditor(&pEditor);
	if(hr==S_OK)
	{

        WMT_ATTR_DATATYPE Type;
		IWMHeaderInfo* pHeaderInfo;
		WORD wStreamNum = 0;
		BYTE* pbValue = NULL;
        WORD DataSize = sizeof(long);

		pEditor->Open(szSource);
		hr = pEditor->QueryInterface(IID_IWMHeaderInfo,(void**)&pHeaderInfo);


		hr = pHeaderInfo->GetAttributeByName( &wStreamNum, g_wszWMDuration, &Type, NULL, &DataSize);
		if( hr == S_OK && hr != ASF_E_NOTFOUND )
		{
			pbValue = new BYTE[ DataSize ];
			pHeaderInfo->GetAttributeByName( &wStreamNum, g_wszWMDuration, &Type, pbValue, &DataSize);
			if( NULL != pbValue )
				m_ulTotalTimeInMS = *( QWORD* )pbValue / 10000;
		}

		hr = pHeaderInfo->GetAttributeByName( &wStreamNum, g_wszWMSeekable, &Type, NULL, &DataSize);
		if( hr ==S_OK && hr != ASF_E_NOTFOUND )
		{
			pbValue = new BYTE[ DataSize ];
			hr = pHeaderInfo->GetAttributeByName( &wStreamNum, g_wszWMSeekable, &Type, pbValue, &DataSize);
			if( NULL != pbValue )
				m_bIsSeekable = *( BOOL* )pbValue;
		}
		if(pHeaderInfo)
			pHeaderInfo->Release();
	}
	if(pEditor)
		pEditor->Release();

	return(true);
}

bool CWMADecoder::Close()
{
	if(m_ISyncReader)
	{
		m_ISyncReader->Close();
		m_ISyncReader->Release();
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
		m_ISyncReader->SetRange((QWORD)MS*100, 0);

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
		m_pINSSBuffer->Release();

	hr = m_ISyncReader->GetNextSample(m_iAudioStreamNumber,
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
			*pBuffer = (*pData) / 32767.0f;
			pData++;
			pBuffer++;
		}
		*ppBuffer = m_Buffer;


		*NumSamples = numSamples;

		if(m_pINSSBuffer)
			m_pINSSBuffer->Release();
		m_pINSSBuffer = NULL;
	}
	else if(hr == NS_E_NO_MORE_SAMPLES)
		return false;
	else if(hr == E_UNEXPECTED)
		return false;
	else if(hr == E_INVALIDARG)
		return false;
	else if(hr == NS_E_INVALID_REQUEST)
		return false;

	return(true);
}