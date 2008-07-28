#pragma once

#include <mediaitem.h>

#define NEW_ALBUMART_INDEX			(-1)


class IInfoAccessor
{
public:
	virtual void			Destroy() = 0;

	virtual bool			ReadMetaData(MediaItem * pItem) = 0;
	virtual bool			WriteMetaData(MediaItem * pItem, unsigned long * pPropertiesToStore, unsigned long ulNumProperties) = 0;

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