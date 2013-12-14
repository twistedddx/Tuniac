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
/*
	Modification and addition to Tuniac originally written by Tony Million
	Copyright (C) 2003-2012 Brett Hoyle
*/

#pragma pack(16)
#pragma once

#define ITUNIACAUDIOSOURCE_VERSION		MAKELONG(0, 3)

#define INVALID_ENTRY_ID	0xffffffff
#define LENGTH_STREAM		0xfffffffe
#define LENGTH_UNKNOWN		0xffffffff

#define STATE_STOPPED	0
#define STATE_PLAYING	1

#define PLAYTIME_CONTINUOUS	(-1)

#define ENTRY_KIND_FILE					0
#define ENTRY_KIND_URL					1

#define AVAILABLILITY_AVAILABLE			0
#define AVAILABLILITY_UNAVAILABLE		1
#define AVAILABLILITY_UNKNOWN			2

#define FIELD_URL							0
#define FIELD_FILENAME						1
#define FIELD_ARTIST						2
#define FIELD_ALBUM							3
#define FIELD_TITLE							4
#define FIELD_TRACKNUM						5
#define FIELD_GENRE							6
#define FIELD_YEAR							7
#define FIELD_PLAYBACKTIME					8
#define FIELD_KIND							9
#define FIELD_FILESIZE						10
#define FIELD_DATEADDED						11
#define FIELD_DATEFILECREATION				12
#define FIELD_DATELASTPLAYED				13
#define FIELD_PLAYCOUNT						14
#define FIELD_RATING						15
#define FIELD_COMMENT						16
#define FIELD_BITRATE						17
#define FIELD_SAMPLERATE					18
#define FIELD_NUMCHANNELS					19
#define FIELD_FILEEXTENSION					20
#define FIELD_PLAYORDER						21
#define FIELD_REPLAYGAIN_TRACK_GAIN			22
#define FIELD_REPLAYGAIN_TRACK_PEAK			23
#define FIELD_REPLAYGAIN_ALBUM_GAIN			24
#define FIELD_REPLAYGAIN_ALBUM_PEAK			25
#define FIELD_AVAILABILITY					26
#define FIELD_BPM							27
#define FIELD_ALBUMARTIST					28
//#define FIELD_STATIONNAME					29

class IAudioFileIO
{
public:
	virtual void Destroy(void) = 0;

	virtual bool Read(unsigned __int64 numberofbytes, void * pData, unsigned __int64 * pBytesRead) = 0;

	virtual bool Seek(unsigned __int64 Offset, bool bFromEnd = false) = 0;

	virtual void Tell(unsigned __int64 * pCurrentOffset) = 0;
	virtual void Size(unsigned __int64 * pSize) = 0;

	virtual bool IsEOF(void) = 0;
};


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
	virtual void		UpdateMetaData(LPTSTR szSource, void * pNewData, unsigned long ulFieldID)		= 0;
	virtual void		UpdateMetaData(LPTSTR szSource, unsigned long pNewData, unsigned long ulFieldID)= 0;
	virtual void		UpdateMetaData(LPTSTR szSource, float pNewData, unsigned long ulFieldID)		= 0;
	virtual void		LogConsoleMessage(LPTSTR szModuleName, LPTSTR szMessage)		= 0;
};


#define FLAGS_ABOUT			0x00000001
#define FLAGS_CONFIGURE		0x00000002
#define FLAGS_PROVIDEMEMORYFILEIO	0x00000004
#define FLAGS_PROVIDESTANDARDFILEIO	0x00000008
#define FLAGS_PROVIDEHTTPFILEIO		0x00000010

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
	virtual bool			Close(void)													= 0;
	virtual	unsigned long	GetNumCommonExts(void)										= 0;
	virtual	LPTSTR			GetCommonExt(unsigned long ulIndex)							= 0;

	virtual IAudioSource *	CreateAudioSource(LPTSTR szSource, IAudioFileIO * pFileIO = NULL)	= 0;
};

typedef IAudioSourceSupplier * (*CreateAudioSourceSupplierFunc)(void);
extern "C" __declspec(dllexport) IAudioSourceSupplier * CreateAudioSourceSupplier(void);

typedef unsigned long (*GetTuniacAudioSourceVersionFunc)(void);
extern "C" __declspec(dllexport) unsigned long GetTuniacAudioSourceVersion(void);
