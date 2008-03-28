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
	Track,			
	MaxTrack,		
	Disc,			
	MaxDisc,		
	Comment,		
	Rating,			
	PlaybackTime,	
	SampleRate,		
	Channels,		
	Bitrate,		
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

	virtual bool			GetAlbumArt(unsigned long ulIndex, void ** pArtData, unsigned __int64 * uqArtDataSize, wchar_t * pwcsMimeType, unsigned long ulMimeTypeBufferSize) = 0;
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