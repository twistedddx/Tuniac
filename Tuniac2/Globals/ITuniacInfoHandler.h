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
} InfoHandlerField;

class ITuniacInfoAccessor
{
public:
	virtual void	Destroy() = 0;
	virtual bool	GetField(InfoHandlerField field, String & toHere) = 0;
	virtual bool	SetField(InfoHandlerField field, String fromHere) = 0;

	virtual bool	GetAlbumArt(void * pArtData) = 0;
	virtual bool	FreeAlbumArt(void * pArtData) = 0;

	virtual bool	SetAlbumArt(void * pArtData) = 0;
};

class ITuniacInfoHandler : public ITuniacPlugin
{
public:
	virtual bool					CanHandle(String filename, unsigned long * Ability, unsigned long * Merit) = 0;

	virtual ITuniacInfoAccessor	*	CreateAccessor(String filename) = 0;

};