#pragma once
#include "ituniacinfohandler.h"

#include "tag.h"
#include "tfile.h"
#include "fileref.h"

class CGenericInfoAccessor :
	public ITuniacInfoAccessor
{
protected:

	TagLib::File		*		m_File;

public:
	CGenericInfoAccessor(void);
public:
	~CGenericInfoAccessor(void);

public:
	bool	Open(String filename);

public:
	void	Destroy();

	bool	GetField(InfoHandlerField field, String & toHere);
	bool	SetField(InfoHandlerField field, String fromHere);

	bool	GetAlbumArt(void * pArtData);
	bool	FreeAlbumArt(void * pArtData);

	bool	SetAlbumArt(void * pArtData);
};
