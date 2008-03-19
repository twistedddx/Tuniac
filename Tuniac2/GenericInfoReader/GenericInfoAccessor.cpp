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
				wcsncpy(toHere, 
						m_File->tag()->title().toWString().c_str(), 
						ulBufferSize);
			}
			break;

		default:
			return false;
	}

	return true;
}

bool	CGenericInfoAccessor::SetTextField(InfoHandlerField field, wchar_t * fromHere)
{
	return false;
}

bool	CGenericInfoAccessor::GetIntField(InfoHandlerField field, __int64 * toHere)
{
	switch(field)
	{
		case PlaybackTime:
			{
				*toHere = m_File->audioProperties()->length();
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
				*toHere = m_File->audioProperties()->bitrate();
			}
			break;

		default:
			return false;
	}
	return true;
}

bool	CGenericInfoAccessor::SetIntField(InfoHandlerField field, __int64 toHere)
{
	return false;
}

bool	CGenericInfoAccessor::GetAlbumArt(void * pArtData)
{
	return false;
}

bool	CGenericInfoAccessor::SetAlbumArt(void * pArtData)
{
	return false;
}

bool	CGenericInfoAccessor::FreeAlbumArt(void * pArtData)
{
	return false;
}

/*
bool	CGenericInfoAccessor::GetField(InfoHandlerField field, String & toHere)
{
	switch(field)
	{
		case Title:
			{
				toHere = m_File->tag()->title().toCString();
			}
			break;

		case Artist:
			{
				toHere = m_File->tag()->artist().toCString();
			}
			break;

		case DiscTitle:
		case Composer:
			break;

		case Album:
			{
				toHere = m_File->tag()->album().toCString();
			}
			break;

		case Year:
			{
				toHere = TagLib::String::number(m_File->tag()->year()).toCString();
			}
			break;

		case Genre:
			{
				toHere = m_File->tag()->genre().toCString();
			}
			break;

		case Track:
			{
				toHere = TagLib::String::number(m_File->tag()->track()).toCString();
			}
			break;

		case MaxTrack:
		case Disc:
		case MaxDisc:
			break;

		case Comment:
			{
				toHere = m_File->tag()->comment().toCString();
			}
			break;

		case Rating:
			break;



		default:
			return false;
	}

	return true;
}
bool	CGenericInfoAccessor::SetField(InfoHandlerField field, String fromHere)
{
	return false;
}
*/