#pragma once

typedef enum AudioSourceDataField
{
	AudioSourceField_Lenth,
	AudioSourceField_TrackName,
	AudioSourceField_TrackArtist,
	AudioSourceField_TrackAlbum,
} AudioSourceDataField;

class IAudioSourceCallback
{
public:
	virtual void StreamDataUpdated(AudioSourceDataField ulDataID, wchar_t * wcsNewValue)= 0;
};

class IAudioSource
{
public:
	virtual void		Destroy(void)																	= 0;

	virtual bool		GetLength(unsigned long * MS)													= 0;
	virtual bool		SetPosition(unsigned long * MS)													= 0;	// upon calling, *MS is the position to seek to, on return set it to the actual offset we seeked to

	virtual bool		GetFormat(unsigned long * SampleRate, unsigned long * Channels)					= 0;
	virtual bool		GetBuffer(float ** ppBuffer, unsigned long * NumSamples)						= 0;	// return false to signal that we are done decoding.
};

class IAudioSourceSupplier
{
public:
	virtual void			Destroy(void)																= 0;

	virtual wchar_t * 		GetName(void)																= 0;
	virtual wchar_t * 		GetAboutText(void)															= 0;

	virtual GUID			GetPluginID(void)															= 0; // use guidgen to make a custom guid to return

	virtual bool			CanHandle(wchar_t * wcsSource, unsigned long * ulAccuracy)					= 0;	// 0 - 100% (or > 100% if you absolutely must play this format)
	virtual IAudioSource *	CreateAudioSource(wchar_t * wcsSource, IAudioSourceCallback * pCallback)	= 0;
};

typedef IAudioSourceSupplier * (*CreateAudioSourceSupplierFunc)(void);
extern "C" __declspec(dllexport) IAudioSourceSupplier * CreateAudioSourceSupplier(void);
#define CREATEAUDIOSOURCESUPPLIERNAME	"CreateAudioSourceSupplier"