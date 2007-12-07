#pragma once

#define ITUNIACAUDIOSOURCE_VERSION		MAKELONG(0, 2)


#define LENGTH_UNKNOWN	0xffffffff

#define STATE_UNKNOWN	(-1)
#define STATE_STOPPED	0
#define STATE_PLAYING	1

#define FIELD_URL							0
#define FIELD_FILENAME						1
#define FIELD_ARTIST						2
#define FIELD_TITLE							4
#define FIELD_GENRE							6

class IAudioSource
{
public:
	virtual void		Destroy(void)													= 0;

	virtual bool		GetLength(unsigned long * MS)									= 0;
	virtual bool		SetPosition(unsigned long * MS)									= 0;// upon calling, *MS is the position to seek to, on return set it to the actual offset we seeked to

	virtual bool		SetState(unsigned long State)									= 0;

	virtual bool		GetFormat(unsigned long * SampleRate, unsigned long * Channels)	= 0;

	virtual bool		GetBuffer(float ** ppBuffer, unsigned long * NumSamples)		= 0; // return false to signal that we are done decoding.
};


class IAudioSourceHelper
{
public:
	virtual void		UpdateStreamTitle(IAudioSource * pSource, LPTSTR szTitle, unsigned long ulFieldID)		= 0;
	virtual void		LogConsoleMessage(LPTSTR szModuleName, LPTSTR szMessage)		= 0;
};


#define FLAGS_ABOUT			0x00000001
#define FLAGS_CONFIGURE		0x00000002

class IAudioSourceSupplier
{
public:
	virtual void			Destroy(void)												= 0;

	virtual void			SetHelper(IAudioSourceHelper * pHelper)						= 0;

	virtual LPTSTR			GetName(void)												= 0;
	virtual GUID			GetPluginID(void)											= 0; // use guidgen to make a custom guid to return
	virtual unsigned long	GetFlags(void)												= 0;

	virtual bool			About(HWND hParent)											= 0;
	virtual bool			Configure(HWND hParent)										= 0;

	virtual bool			CanHandle(LPTSTR szSource)									= 0;
	virtual	unsigned long	GetNumCommonExts(void)										= 0;
	virtual	LPTSTR			GetCommonExt(unsigned long ulIndex)							= 0;

	virtual IAudioSource *	CreateAudioSource(LPTSTR szSource)							= 0;
};

typedef IAudioSourceSupplier * (*CreateAudioSourceSupplierFunc)(void);
extern "C" __declspec(dllexport) IAudioSourceSupplier * CreateAudioSourceSupplier(void);

typedef unsigned long (*GetTuniacAudioSourceVersionFunc)(void);
extern "C" __declspec(dllexport) unsigned long GetTuniacAudioSourceVersion(void);
