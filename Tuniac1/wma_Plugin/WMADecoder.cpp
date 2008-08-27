#include "StdAfx.h"
#include "wmadecoder.h"

CWMADecoder::CWMADecoder(void)
: m_pINSSBuffer(NULL)
, m_IAudioOutputProps(NULL)
, m_theMediaType(NULL)
, m_theOutputsCount(0)
, m_iAudioOutputNumber(0)
, m_iAudioStreamNumber(0)
{
	m_ISyncReader = NULL;
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
		SAFE_RELEASE( m_IAudioOutputProps );
		SAFE_ARRAYDELETE( m_theMediaType );
		m_ISyncReader->GetOutputProps(i,&m_IAudioOutputProps); 
		m_IAudioOutputProps->GetMediaType(NULL,&theSize);
		m_theMediaType = ( WM_MEDIA_TYPE* ) new BYTE[theSize ];
		m_IAudioOutputProps->GetMediaType(m_theMediaType,&theSize);

		if( FAILED( hr ) )
			return false;

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
	//release the memory
	SAFE_RELEASE( m_IAudioOutputProps );
	SAFE_ARRAYDELETE( m_theMediaType );

	IWMMetadataEditor		*	pEditor;
	hr = WMCreateEditor(&pEditor);
	if(hr==S_OK)
	{
		pEditor->Open(szSource);
        WMT_ATTR_DATATYPE Type;
        WORD DataSize = sizeof(long);
		IWMHeaderInfo3* pHeaderInfo;
		WORD StreamNo = 0;
		hr = pEditor->QueryInterface(IID_IWMHeaderInfo3,(void**)&pHeaderInfo);
		hr = pHeaderInfo->GetAttributeByName( &StreamNo, g_wszWMDuration, &Type, NULL, &DataSize);
		if( FAILED( hr ))
			return false;

		if (Type != WMT_TYPE_QWORD   || DataSize != sizeof(QWORD))
			return false;

		QWORD qwDuration = 0;
		hr = pHeaderInfo->GetAttributeByName( &StreamNo, g_wszWMDuration, &Type, (BYTE *) &qwDuration, &DataSize);
		if( FAILED( hr ))
			return false;

		m_qwTotalTimeInMS = (long) (qwDuration / 10000);

		SAFE_RELEASE(pHeaderInfo);
		SAFE_RELEASE(pEditor);
	}

	return(true);
}

bool CWMADecoder::Close()
{
	SAFE_RELEASE(m_ISyncReader);
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

	*MS = m_qwTotalTimeInMS;

	return(true);
}

bool		CWMADecoder::SetPosition(unsigned long * MS)
{
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
		
	hr = m_ISyncReader->GetNextSample(m_iAudioStreamNumber, &m_pINSSBuffer, &cnsSampleTime, &cnsSampleDuration, &dwFlags, NULL, NULL);
	if(hr == NS_E_NO_MORE_SAMPLES)
		return false;

	if(SUCCEEDED(hr))
	{
		unsigned char *buffer;
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

		m_pINSSBuffer->Release();
		m_pINSSBuffer = NULL;
	}
	else
		return false;

	return(true);
}