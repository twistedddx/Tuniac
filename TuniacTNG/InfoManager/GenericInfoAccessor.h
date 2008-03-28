#pragma once
#include "iinfohandler.h"

#include "tag.h"
#include "tfile.h"
#include "fileref.h"

class CGenericInfoAccessor :
	public IInfoAccessor
{
protected:

	TagLib::File		*		m_File;

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

	bool	GetAlbumArt(unsigned long ulIndex, void ** pArtData, unsigned __int64 * uqArtDataSize, wchar_t * pwcsMimeType, unsigned long ulMimeTypeBufferSize);
	bool	SetAlbumArt(unsigned long ulIndex, void * pArtData, unsigned __int64 uqArtSize, wchar_t * pwcsMimeType);
};
