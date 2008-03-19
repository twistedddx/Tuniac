#pragma once

#include "ITuniacPlugin.h"

// {78935719-B9E1-40a0-99D6-75506D494DA8}
static const GUID GUID_TUNIACINFOHANDLER = { 0x78935719, 0xb9e1, 0x40a0, { 0x99, 0xd6, 0x75, 0x50, 0x6d, 0x49, 0x4d, 0xa8 } };

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

class ITuniacInfoAccessor
{
public:
	virtual void	Destroy() = 0;

	virtual bool	GetTextField(InfoHandlerField field, wchar_t * toHere, unsigned long ulBufferSize) = 0;
	virtual bool	SetTextField(InfoHandlerField field, wchar_t * fromHere) = 0;

	virtual bool	GetIntField(InfoHandlerField field, __int64 * toHere) = 0;
	virtual bool	SetIntField(InfoHandlerField field, __int64 toHere) = 0;

	virtual bool	GetAlbumArt(void ** pArtData, unsigned __int64 * uqArtDataSize) = 0;
	virtual bool	SetAlbumArt(void * pArtData, unsigned __int64 uqArtSize) = 0;
};

class ITuniacInfoHandler : public ITuniacPlugin
{
public:
	virtual bool					CanHandle(wchar_t * filename, unsigned long * Ability, unsigned long * Merit) = 0;

	virtual ITuniacInfoAccessor	*	CreateAccessor(wchar_t * filename) = 0;

};