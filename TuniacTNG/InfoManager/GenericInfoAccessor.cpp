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

		//vorbis comment, id3v2
		if(!StrCmpI(TEXT(".flac"), PathFindExtension(m_File->name())))
		{
			m_flacFile = (TagLib::FLAC::File *)m_File;
			if(m_flacFile->xiphComment())
				vorbisTag = m_flacFile->xiphComment()->fieldListMap();

			if(m_flacFile->ID3v2Tag()) 
				id3Tag = m_flacFile->ID3v2Tag()->frameListMap();
			return true;
		}

		//id3v1. id3v2, ape
		else if(!StrCmpI(TEXT(".mp3"), PathFindExtension(m_File->name())))
		{
			m_mpegFile = (TagLib::MPEG::File *)m_File;
			if(m_mpegFile->APETag())
				apeTag = m_mpegFile->APETag()->itemListMap();

			if(m_mpegFile->ID3v2Tag()) 
				id3Tag = m_mpegFile->ID3v2Tag()->frameListMap();
			return true;
		}

		//mp4 tag
		else if(!StrCmpI(TEXT(".mp4"), PathFindExtension(m_File->name())))
		{
			m_mp4File = (TagLib::MP4::File *)m_File;
			if(m_mp4File->tag())
				mp4Tag = m_mp4File->tag()->itemListMap();
			return true;
		}

		//ape
		else if(!StrCmpI(TEXT(".mpc"), PathFindExtension(m_File->name())))
		{
			m_mpcFile = (TagLib::MPC::File *)m_File;
			if(m_mpcFile->APETag())
				apeTag = m_mpcFile->APETag()->itemListMap();
			return true;
		}

		//vorbis comment
		else if(!StrCmpI(TEXT(".ogg"), PathFindExtension(m_File->name())))
		{
			m_oggFile = (TagLib::Ogg::Vorbis::File *)m_File;
			if(m_oggFile->tag())
				vorbisTag = m_oggFile->tag()->fieldListMap();
			return true;
		}

		//id3v1, id3v2
		else if(!StrCmpI(TEXT(".tta"), PathFindExtension(m_File->name())))
		{
			m_ttaFile = (TagLib::TrueAudio::File *)m_File;
			if(m_ttaFile->ID3v2Tag()) 
				id3Tag = m_ttaFile->ID3v2Tag()->frameListMap();
			return true;
		}

		//id3v1, ape
		else if(!StrCmpI(TEXT(".wv"), PathFindExtension(m_File->name())))
		{
			m_wvFile = (TagLib::WavPack::File *)m_File;
			if(m_wvFile->APETag())
				apeTag = m_wvFile->APETag()->itemListMap();
			return true;
		}
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
			wcsncpy(toHere, TEXT(""), ulBufferSize);
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
				if(!id3Tag.isEmpty())
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

				if(!mp4Tag.isEmpty())
				{
					if(mp4Tag["trkn"].isValid())
						*toHere = mp4Tag["trkn"].toIntPair().second;
				}

				if(!vorbisTag.isEmpty())
				{
					if(!vorbisTag["TOTALTRACKS"].isEmpty())
						*toHere = vorbisTag["TOTALTRACKS"].toString().toInt();
				}

				if(!apeTag.isEmpty())
				{
					if(!apeTag["Track"].isEmpty())
					{
						TagLib::String pszData = apeTag["Track"].toString();
						int val = pszData.find("/");
						if(val != -1)
						{
							TagLib::String trackMax = pszData.substr(val+1);
							*toHere = trackMax.toInt();
						}
					}
				}

				// check other special accessors!
			}
			break;

		case MaxDisc:
			{
				if(!id3Tag.isEmpty())
				{
					TagLib::ID3v2::FrameList l = id3Tag["TPOS"];
					if(!l.isEmpty())
					{
						//std::cout << l.front()->toString() << std::endl;
						TagLib::String pszData = l.front()->toString();
						int val = pszData.find("/");
						if(val != -1)
						{
							TagLib::String diskMax = pszData.substr(val+1);
							*toHere  = diskMax.toInt();
						}
					}
				}
			}
			break;



		case Disc:
			{
				if(!id3Tag.isEmpty())
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

				if(!mp4Tag.isEmpty())
				{
					if(mp4Tag["disc"].isValid())
						*toHere = mp4Tag["disc"].toInt();
				}

				if(!vorbisTag.isEmpty())
				{
					if(!vorbisTag["DISCNUMBER"].isEmpty())
						*toHere = vorbisTag["DISCNUMBER"].toString().toInt();
				}

				if(!apeTag.isEmpty())
				{
					if(!apeTag["disc"].isEmpty())
						*toHere = apeTag["disc"].toString().toInt();
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

		case ReplayGainTrack:
			{
				if(!id3Tag.isEmpty())
				{
					{
						TagLib::ID3v2::FrameList l = id3Tag["RGAD"];
						if(!l.isEmpty())
						{
							//_ASSERT(0);
							// we dont actually support this yet and need to decode the data
						}
					}
					{
						TagLib::ID3v2::FrameList l = id3Tag["RVA2"];
						if(!l.isEmpty())
						{
							TagLib::ID3v2::RelativeVolumeFrame * relVol = static_cast<TagLib::ID3v2::RelativeVolumeFrame *>(l.front());
							*toHere = relVol->volumeAdjustment();
							//libEnt->fReplayGain_Track_Peak  = relVol->peakVolume();
						}
					}
				}

				if(!vorbisTag.isEmpty())
				{
					if(!vorbisTag["REPLAYGAIN_TRACK_GAIN"].isEmpty())
						*toHere = atof(vorbisTag["REPLAYGAIN_TRACK_GAIN"].toString().toCString());
				}

				if(!apeTag.isEmpty())
				{
					if(!apeTag["REPLAYGAIN_TRACK_GAIN"].isEmpty())
						*toHere = atof(apeTag["REPLAYGAIN_TRACK_GAIN"].toString().toCString());
				}
			}
			break;

		case ReplayPeakTrack:
			{
				if(!vorbisTag.isEmpty())
				{
					if(!vorbisTag["REPLAYGAIN_TRACK_PEAK"].isEmpty())
						*toHere = atof(vorbisTag["REPLAYGAIN_TRACK_PEAK"].toString().toCString());
				}

				if(!apeTag.isEmpty())
				{
					if(!apeTag["REPLAYGAIN_TRACK_PEAK"].isEmpty())
						*toHere = atof(apeTag["REPLAYGAIN_TRACK_PEAK"].toString().toCString());
				}
			}
			break;

		case ReplayGainAlbum:
			{
				if(!vorbisTag.isEmpty())
				{
					if(!vorbisTag["REPLAYGAIN_ALBUM_GAIN"].isEmpty())
						*toHere = atof(vorbisTag["REPLAYGAIN_ALBUM_GAIN"].toString().toCString());
				}

				if(!apeTag.isEmpty())
				{
					if(!apeTag["REPLAYGAIN_ALBUM_GAIN"].isEmpty())
						*toHere = atof(apeTag["REPLAYGAIN_ALBUM_GAIN"].toString().toCString());
				}
			}
			break;

		case ReplayPeakAlbum:
			{
				if(!vorbisTag.isEmpty())
				{
					if(!vorbisTag["REPLAYGAIN_ALBUM_PEAK"].isEmpty())
						*toHere = atof(vorbisTag["REPLAYGAIN_ALBUM_PEAK"].toString().toCString());
				}

				if(!apeTag.isEmpty())
				{
					if(!apeTag["REPLAYGAIN_ALBUM_PEAK"].isEmpty())
						*toHere = atof(apeTag["REPLAYGAIN_ALBUM_PEAK"].toString().toCString());
				}
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
