#include "StdAfx.h"
#include "GenericInfoAccessor.h"



CGenericInfoAccessor::CGenericInfoAccessor(void)
{
	m_mpegFile = NULL;
}

CGenericInfoAccessor::~CGenericInfoAccessor(void)
{
}

bool	CGenericInfoAccessor::Open(wchar_t * filename)
{
	m_File = TagLib::FileRef::create(filename, true, TagLib::AudioProperties::Accurate);

	if(m_File)
	{
		if(!StrCmpI(TEXT(".mp3"), PathFindExtension(m_File->name())) )
		{
			m_mpegFile = (TagLib::MPEG::File *)m_File;
		}

		return true;
	}

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


/*
			{

			}

			{
				TagLib::ID3v2::FrameList l = mpegFile->ID3v2Tag()->frameListMap()["APIC"];
				if(!l.isEmpty())
				{
					TagLib::ID3v2::AttachedPictureFrame *picframe = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(l.front());

					picframe->mimeType();
				}
			}


		}
	}
	*/

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
			{
				if(m_mpegFile)
				{
					if(m_mpegFile->ID3v2Tag()) 
					{
						// Get the list of frames for a specific frame type
						{
							TagLib::ID3v2::FrameList l = m_mpegFile->ID3v2Tag()->frameListMap()["TRCK"];
							if(!l.isEmpty())
							{
								//std::cout << l.front()->toString() << std::endl;
								TagLib::String pszData = l.front()->toString();
								int val = pszData.find("/");
								if(val != -1)
								{
									TagLib::String trackMax = pszData.substr(val+1);
									*toHere = trackMax.toInt();
								}
							}
						}
					}
				}

				// check other special accessors!
			}
			break;

		case MaxDisc:
		case Disc:
			{
				if(m_mpegFile)
				{
					if(m_mpegFile->ID3v2Tag()) 
					{
						// Get the list of frames for a specific frame type
						{

							TagLib::ID3v2::FrameList l = m_mpegFile->ID3v2Tag()->frameListMap()["TPOS"];
							if(!l.isEmpty())
							{
								//std::cout << l.front()->toString() << std::endl;
								TagLib::String pszData = l.front()->toString();
								int val = pszData.find("/");

								if(val == -1)
								{
									if(field == Disc)
										*toHere = pszData.toInt();
									else
										*toHere = 0;
								}
								else
								{
									TagLib::String disk = pszData.substr(0,val);
									TagLib::String diskMax = pszData.substr(val+1);

									if(field == Disc)
										*toHere = disk.toInt();
									else
										*toHere = diskMax.toInt();
								}
							}

						}
					}
				}
			}
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

bool	CGenericInfoAccessor::GetAlbumArtCount(__int64 * pullCount)
{
	return false;
}

bool	CGenericInfoAccessor::GetAlbumArtInformation(__int64 ullArtIndex, unsigned __int64 * uqArtDataSize, wchar_t * pwcsMimeType, unsigned long ulMimeTypeBufferSize)
{
	return false;
}

bool	CGenericInfoAccessor::GetAlbumArtData(unsigned long ulIndex, void * pArtData, unsigned long ullArtDataSize)
{
	return false;
}

bool	CGenericInfoAccessor::SetAlbumArt(unsigned long ulIndex, void * pArtData, unsigned __int64 uqArtSize, wchar_t * pwcsMimeType)
{
	return false;
}
