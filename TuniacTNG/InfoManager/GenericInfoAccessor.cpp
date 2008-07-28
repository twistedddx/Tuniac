#include "StdAfx.h"
#include "GenericInfoAccessor.h"



CGenericInfoAccessor::CGenericInfoAccessor(void)
{
	m_mpegFile = NULL;
	m_pProperties = NULL;
}

CGenericInfoAccessor::~CGenericInfoAccessor(void)
{
}

bool	CGenericInfoAccessor::Open(wchar_t * filename)
{
	m_File = TagLib::FileRef::create(filename, true, TagLib::AudioProperties::Accurate);

	if(m_File)
	{
		m_pProperties = m_File->audioProperties();


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
		else if(	(!StrCmpI(TEXT(".mp4"), PathFindExtension(m_File->name()))) ||
					(!StrCmpI(TEXT(".m4a"), PathFindExtension(m_File->name()))) )
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

bool	CGenericInfoAccessor::ReadMetaData(MediaItem & pItem)
{
	pItem.title		= m_File->tag()->title().toWString();
	pItem.artist	= m_File->tag()->artist().toWString();
	pItem.album		= m_File->tag()->album().toWString();
	pItem.genre		= m_File->tag()->genre().toWString();
	pItem.comment	= m_File->tag()->comment().toWString();

	pItem.ulYear	= m_File->tag()->year();
	pItem.ulTrack	= m_File->tag()->track();

	pItem.ulPlayTimeMS		= m_pProperties->length() * 1000;
	pItem.ulSampleRate		= m_pProperties->sampleRate();
	pItem.ulChannelCount	= m_pProperties->channels();
	pItem.ulBitRate			= m_pProperties->bitrate() * 1000;


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
				pItem.fReplayGainTrack = relVol->volumeAdjustment();
			}
		}
		{
			TagLib::ID3v2::FrameList l = id3Tag["TRCK"];
			if(!l.isEmpty())
			{
				//std::cout << l.front()->toString() << std::endl;
				TagLib::String pszData = l.front()->toString();
				int val = pszData.find("/");
				if(val == -1)
				{
					pItem.ulDisk = pszData.toInt();
					pItem.ulMaxDisk = 0;
				}
				else
				{
					TagLib::String track = pszData.substr(0,val);
					TagLib::String trackMax = pszData.substr(val+1);

					pItem.ulDisk	= track.toInt();
					pItem.ulMaxDisk = trackMax.toInt();
				}
			}
		}

		{
			TagLib::ID3v2::FrameList l = id3Tag["TPOS"];
			if(!l.isEmpty())
			{
				//std::cout << l.front()->toString() << std::endl;
				TagLib::String pszData = l.front()->toString();
				int val = pszData.find("/");

				if(val == -1)
				{
					pItem.ulDisk = pszData.toInt();
					pItem.ulMaxDisk = 0;
				}
				else
				{
					TagLib::String disk = pszData.substr(0,val);
					TagLib::String diskMax = pszData.substr(val+1);

					pItem.ulDisk = disk.toInt();
					pItem.ulMaxDisk = diskMax.toInt();
				}
			}

		}
	}

	if(!vorbisTag.isEmpty())
	{
		// REPLAY GAIN
		if(!vorbisTag["REPLAYGAIN_TRACK_GAIN"].isEmpty())
			pItem.fReplayGainTrack = atof(vorbisTag["REPLAYGAIN_TRACK_GAIN"].toString().to8Bit().c_str());

		if(!vorbisTag["REPLAYGAIN_TRACK_PEAK"].isEmpty())
			pItem.fReplayPeakTrack = atof(vorbisTag["REPLAYGAIN_TRACK_PEAK"].toString().to8Bit().c_str());

		if(!vorbisTag["REPLAYGAIN_ALBUM_GAIN"].isEmpty())
			pItem.fReplayGainAlbum = atof(vorbisTag["REPLAYGAIN_ALBUM_GAIN"].toString().to8Bit().c_str());

		if(!vorbisTag["REPLAYGAIN_ALBUM_PEAK"].isEmpty())
			pItem.fReplayPeakAlbum = atof(vorbisTag["REPLAYGAIN_ALBUM_PEAK"].toString().to8Bit().c_str());

		// MAX TRACK
		if(!vorbisTag["TOTALTRACKS"].isEmpty())
		{
			pItem.ulMaxTrack = vorbisTag["TOTALTRACKS"].toString().toInt();
		}
		else if(!vorbisTag["TRACKTOTAL"].isEmpty())
		{
			pItem.ulMaxTrack = vorbisTag["TRACKTOTAL"].toString().toInt();
		}
		
		if(!vorbisTag["DISCNUMBER"].isEmpty())
		{
			TagLib::String pszData = vorbisTag["DISCNUMBER"].toString();
			int val = pszData.find("/");

			if(val == -1)
			{
				pItem.ulDisk = pszData.toInt();
				pItem.ulMaxDisk = 0;
			}
			else
			{
				TagLib::String disk = pszData.substr(0,val);
				TagLib::String diskMax = pszData.substr(val+1);

				pItem.ulDisk = disk.toInt();
				pItem.ulMaxDisk = diskMax.toInt();
			}
		}
	}

	if(!apeTag.isEmpty())
	{
		// REPLAY GAIN
		if(!apeTag["REPLAYGAIN_TRACK_GAIN"].isEmpty())
			pItem.fReplayGainTrack = atof(vorbisTag["REPLAYGAIN_TRACK_GAIN"].toString().to8Bit().c_str());

		if(!apeTag["REPLAYGAIN_TRACK_PEAK"].isEmpty())
			pItem.fReplayPeakTrack = atof(vorbisTag["REPLAYGAIN_TRACK_PEAK"].toString().to8Bit().c_str());

		if(!apeTag["REPLAYGAIN_ALBUM_GAIN"].isEmpty())
			pItem.fReplayGainAlbum = atof(vorbisTag["REPLAYGAIN_ALBUM_GAIN"].toString().to8Bit().c_str());

		if(!apeTag["REPLAYGAIN_ALBUM_PEAK"].isEmpty())
			pItem.fReplayPeakAlbum = atof(vorbisTag["REPLAYGAIN_ALBUM_PEAK"].toString().to8Bit().c_str());

		// max track
		if(!apeTag["Track"].isEmpty())
		{
			TagLib::String pszData = apeTag["Track"].toString();
			int val = pszData.find("/");

			if(val == -1)
			{
				pItem.ulDisk = pszData.toInt();
				pItem.ulMaxDisk = 0;
			}
			else
			{
				TagLib::String track = pszData.substr(0,val);
				TagLib::String trackMax = pszData.substr(val+1);

				pItem.ulDisk	= track.toInt();
				pItem.ulMaxDisk = trackMax.toInt();
			}
		}

		if(!apeTag["disc"].isEmpty())
		{
			TagLib::String pszData = apeTag["disc"].toString();
			int val = pszData.find("/");

			if(val == -1)
			{
				pItem.ulDisk = pszData.toInt();
				pItem.ulMaxDisk = 0;
			}
			else
			{
				TagLib::String disk = pszData.substr(0,val);
				TagLib::String diskMax = pszData.substr(val+1);

				pItem.ulDisk = disk.toInt();
				pItem.ulMaxDisk = diskMax.toInt();
			}
		}

	}


	if(!mp4Tag.isEmpty())
	{
		if(mp4Tag["trkn"].isValid())
		{
			pItem.ulTrack		= mp4Tag["trkn"].toIntPair().first;
			pItem.ulMaxTrack	= mp4Tag["trkn"].toIntPair().first;
		}

		if(mp4Tag["disc"].isValid())
			pItem.ulDisk = mp4Tag["disc"].toInt();
	}



	return true;
}

bool	CGenericInfoAccessor::WriteMetaData(MediaItem & pItem, unsigned long * pPropertiesToStore, unsigned long ulNumProperties)
{
	for(unsigned long prop = 0; prop<ulNumProperties; prop++)
	{
		switch(pPropertiesToStore[prop])
		{
			default:
				{
				}
				break;
		}
	}


	m_File->tag()->setTitle(pItem.title);
	m_File->tag()->setArtist(pItem.artist);
	m_File->tag()->setAlbum(pItem.album);
	m_File->tag()->setGenre(pItem.genre);
	m_File->tag()->setComment(pItem.comment);

	m_File->tag()->setYear(pItem.ulYear);
	m_File->tag()->setTrack(pItem.ulTrack);

	return false;
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
