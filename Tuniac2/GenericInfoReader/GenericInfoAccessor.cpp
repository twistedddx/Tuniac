#include "StdAfx.h"
#include "GenericInfoAccessor.h"

CGenericInfoAccessor::CGenericInfoAccessor(void)
{
}

CGenericInfoAccessor::~CGenericInfoAccessor(void)
{
}

bool	CGenericInfoAccessor::Open(wchar_t * filename)
{
	m_File = TagLib::FileRef::create(filename, true, TagLib::AudioProperties::Accurate);

	if(m_File)
		return true;

	return false;
}

void	CGenericInfoAccessor::Destroy()
{
	if(m_File)
		delete m_File;

	delete this;
}

bool	CGenericInfoAccessor::GetTextField(InfoHandlerField field, wchar_t * toHere, unsigned long ulBufferSize)
{
	wcsncpy(toHere, TEXT(""), ulBufferSize);

	switch(field)
	{
		case Title:
			{
				wcsncpy(toHere, m_File->tag()->title().toWString().c_str(), ulBufferSize);
			}
			break;

		case Artist:
			{
				wcsncpy(toHere, m_File->tag()->artist().toWString().c_str(), ulBufferSize);
			}
			break;

		case DiscTitle:
		case Composer:
			break;

		case Album:
			{
				wcsncpy(toHere, m_File->tag()->album().toWString().c_str(), ulBufferSize);
			}
			break;


		case Genre:
			{
				wcsncpy(toHere, m_File->tag()->genre().toWString().c_str(), ulBufferSize);
			}
			break;

		case Comment:
			{
				wcsncpy(toHere, m_File->tag()->comment().toWString().c_str(), ulBufferSize);
			}
			break;

		case Rating:
			break;

		default:
			return false;
	}

	return true;
}

bool	CGenericInfoAccessor::SetTextField(InfoHandlerField field, wchar_t * fromHere)
{
	switch(field)
	{
		case Title:
			{
				m_File->tag()->setTitle(fromHere);
			}
			break;

		case Artist:
			{
				m_File->tag()->setArtist(fromHere);
			}
			break;

		case DiscTitle:
		case Composer:
			break;

		case Album:
			{
				m_File->tag()->setAlbum(fromHere);
			}
			break;


		case Genre:
			{
				m_File->tag()->setGenre(fromHere);
			}
			break;

		case Comment:
			{
				m_File->tag()->setComment(fromHere);
			}
			break;

		case Rating:
			break;

		default:
			return false;
	}

	return true;
}

bool	CGenericInfoAccessor::GetIntField(InfoHandlerField field, __int64 * toHere)
{
	*toHere = 0;

	switch(field)
	{
		case Year:
			{
				*toHere = m_File->tag()->year();
			}
			break;

		case Track:
			{
				*toHere = m_File->tag()->track();
			}
			break;

		case MaxTrack:
		case Disc:
		case MaxDisc:
			break;

		case PlaybackTime:
			{
				*toHere = m_File->audioProperties()->length() * 1000;
			}
			break;

		case SampleRate:
			{
				*toHere = m_File->audioProperties()->sampleRate();
			}
			break;

		case Channels:
			{
				*toHere = m_File->audioProperties()->channels();
			}
			break;

		case Bitrate:
			{
				*toHere = m_File->audioProperties()->bitrate() * 1000;
			}
			break;

		default:
			return false;
	}

	return true;
}

bool	CGenericInfoAccessor::SetIntField(InfoHandlerField field, __int64 fromHere)
{
	switch(field)
	{
		case Year:
			{
				m_File->tag()->setYear((TagLib::uint)fromHere);
			}
			break;

		case Track:
			{
				m_File->tag()->setTrack((TagLib::uint)fromHere);
			}
			break;

		case MaxTrack:
		case Disc:
		case MaxDisc:
			break;

		default:
			return false;
	}

	return true;
}

bool	CGenericInfoAccessor::GetAlbumArt(unsigned long ulIndex, void ** pArtData, unsigned __int64 * uqArtDataSize, wchar_t * pwcsMimeType, unsigned long ulMimeTypeBufferSize)
{
	return false;
}

bool	CGenericInfoAccessor::SetAlbumArt(unsigned long ulIndex, void * pArtData, unsigned __int64 uqArtSize, wchar_t * pwcsMimeType)
{
	return false;
}
