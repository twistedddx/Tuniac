#pragma once
#include "iinfohandler.h"

#include "tag.h"
#include "tfile.h"
#include "fileref.h"

#include "mpeg/mpegfile.h"
#include "mpeg/id3v2/id3v2tag.h"
#include "mpeg/id3v2/frames/attachedpictureframe.h"

class CGenericInfoAccessor :
	public IInfoAccessor
{
protected:

	TagLib::File		*		m_File;

	TagLib::MPEG::File	*		m_mpegFile;

public:
	CGenericInfoAccessor(void);
public:
	~CGenericInfoAccessor(void);

public:
	bool	Open(wchar_t * filename);

public:
	void	Destroy();

	bool	GetTextField(InfoHandlerField field, wchar_t * toHere, unsigned long ulBufferSize);
	bool	SetTextField(InfoHandlerField field, wchar_t * fromHere);

	bool	GetIntField(InfoHandlerField field, __int64 * toHere);
	bool	SetIntField(InfoHandlerField field, __int64 toHere);

	bool	GetAlbumArtCount(__int64 * pullCount);
	bool	GetAlbumArtInformation(__int64 ullArtIndex, unsigned __int64 * uqArtDataSize, wchar_t * pwcsMimeType, unsigned long ulMimeTypeBufferSize);
	bool	GetAlbumArtData(unsigned long ulIndex, void * pArtData, unsigned long ullArtDataSize);

	bool	SetAlbumArt(unsigned long ulIndex, void * pArtData, unsigned __int64 uqArtSize, wchar_t * pwcsMimeType);
};
