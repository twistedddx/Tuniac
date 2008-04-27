#pragma once

typedef enum InfoHandlerField
{
	Title,			
	Artist,			
	DiscTitle,		
	Composer,		
	Album,			
	Year,			
	Genre,			
	Comment,		

	Track,			
	MaxTrack,		
	Disc,			
	MaxDisc,		

	Rating,			
	BPM,

	PlaybackTime,	
	SampleRate,		
	Channels,		
	Bitrate,		

	ReplayGainTrack,
	ReplayPeakTrack,
	ReplayGainAlbum,
	ReplayPeakAlbum,

	AlbumArtImageCount,
} InfoHandlerField;

#define NEW_ALBUMART_INDEX			(-1)


class IInfoAccessor
{
public:
	virtual void			Destroy() = 0;

	virtual bool			GetTextField(InfoHandlerField field, wchar_t * toHere, unsigned long ulBufferSize) = 0;
	virtual bool			SetTextField(InfoHandlerField field, wchar_t * fromHere) = 0;

	virtual bool			GetIntField(InfoHandlerField field, __int64 * toHere) = 0;
	virtual bool			SetIntField(InfoHandlerField field, __int64 toHere) = 0;

	virtual bool			GetAlbumArtCount(__int64 * pullCount) = 0;
	virtual bool			GetAlbumArtInformation(__int64 ullArtIndex, unsigned __int64 * uqArtDataSize, wchar_t * pwcsMimeType, unsigned long ulMimeTypeBufferSize) = 0;
	virtual bool			GetAlbumArtData(unsigned long ulIndex, void * pArtData, unsigned long ullArtDataSize) = 0;

	virtual bool			SetAlbumArt(unsigned long ulIndex, void * pArtData, unsigned __int64 uqArtSize, wchar_t * pwcsMimeType) = 0;
};

class IInfoHandler
{
public:
	virtual void					Destroy(void) = 0;
	virtual bool					CanHandle(wchar_t * filename, unsigned long * Ability, unsigned long * Merit) = 0;
	virtual IInfoAccessor	*		CreateAccessor(wchar_t * filename) = 0;
};


typedef IInfoHandler * (*CreateInfoHandlerFunc)(void);
extern "C" __declspec(dllexport) IInfoHandler * CreateInfoHandler(void);