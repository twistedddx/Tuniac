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
	fileref = TagLib::FileRef(filename, 1, TagLib::AudioProperties::Fast);
    if( !fileref.isNull() )
    {
		if(flacfile = dynamic_cast<TagLib::FLAC::File *>( fileref.file() ))
			return true;
		else if(mp3file = dynamic_cast<TagLib::MPEG::File *>( fileref.file() ))
			return true;
		else if(mp4file = dynamic_cast<TagLib::MP4::File *>( fileref.file() ))
			return true;
		else if(mpcfile = dynamic_cast<TagLib::MPC::File *>( fileref.file() ))
			return true;
		else if(ttafile = dynamic_cast<TagLib::TrueAudio::File *>( fileref.file() ))
			return true;
		else if(wvfile = dynamic_cast<TagLib::WavPack::File *>( fileref.file() ))
			return true;
		else if(oggfile = dynamic_cast<TagLib::Ogg::Vorbis::File *>( fileref.file() ))
			return true;
		else if(ogafile = dynamic_cast<TagLib::Ogg::FLAC::File *>( fileref.file() ))
			return true;
		else if(spxfile = dynamic_cast<TagLib::Ogg::Speex::File *>( fileref.file() ))
			return true;
		else if(wmafile = dynamic_cast<TagLib::ASF::File *>( fileref.file() ))
			return true;
		else if(aiffile = dynamic_cast<TagLib::RIFF::AIFF::File *>( fileref.file() ))
			return true;
		else if(wavfile = dynamic_cast<TagLib::RIFF::WAV::File *>( fileref.file() ))
			return true;
	}

	return false;

}

void	CGenericInfoAccessor::Destroy()
{
	delete this;
}

bool	CGenericInfoAccessor::ReadMetaData(MediaItem & pItem)
{
	TagLib::AudioProperties *	audprops	 = 0;
	TagLib::Tag				*	tag			= 0;

	TagLib::Ogg::FieldListMap		vorbisTag;
	TagLib::APE::ItemListMap		apeTag;
	TagLib::ID3v2::FrameListMap		id3v2Tag;
	TagLib::MP4::ItemListMap		mp4Tag;
	TagLib::ASF::AttributeListMap	wmaTag;

	audprops = fileref.audioProperties();
	if(audprops)
	{
		pItem.ulPlayTimeMS		= audprops->length() * 1000;
		pItem.ulSampleRate		= audprops->sampleRate();
		pItem.ulChannelCount	= audprops->channels();
		pItem.ulBitRate			= audprops->bitrate() * 1000;
	}

	tag = fileref.tag();
	if(tag)
	{
		pItem.title		= tag->title().toWString();
		pItem.artist	= tag->artist().toWString();
		pItem.album		= tag->album().toWString();
		pItem.genre		= tag->genre().toWString();
		pItem.comment	= tag->comment().toWString();

		pItem.ulYear	= tag->year();
		pItem.ulTrack	= tag->track();
	}

	if(!id3v2Tag.isEmpty())
	{
		{
			TagLib::ID3v2::FrameList l = id3v2Tag["RGAD"];
			if(!l.isEmpty())
			{
				//_ASSERT(0);
				// we dont actually support this yet and need to decode the data
			}
		}
		{
			TagLib::ID3v2::FrameList l = id3v2Tag["RVA2"];
			if(!l.isEmpty())
			{
				TagLib::ID3v2::RelativeVolumeFrame * relVol = static_cast<TagLib::ID3v2::RelativeVolumeFrame *>(l.front());
				pItem.fReplayGainTrack = relVol->volumeAdjustment();
			}
		}
		{
			TagLib::ID3v2::FrameList l = id3v2Tag["TRCK"];
			if(!l.isEmpty())
			{
				//std::cout << l.front()->toString() << std::endl;
				TagLib::String pszData = l.front()->toString();
				int val = pszData.find("/");
				if(val == -1)
				{
					pItem.ulTrack = pszData.toInt();
					pItem.ulMaxTrack = 0;
				}
				else
				{
					TagLib::String track = pszData.substr(0,val);
					TagLib::String trackMax = pszData.substr(val+1);

					pItem.ulTrack	= track.toInt();
					pItem.ulMaxTrack = trackMax.toInt();
				}
			}
		}

		{
			TagLib::ID3v2::FrameList l = id3v2Tag["TPOS"];
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
				pItem.ulTrack = pszData.toInt();
				pItem.ulMaxTrack = 0;
			}
			else
			{
				TagLib::String track = pszData.substr(0,val);
				TagLib::String trackMax = pszData.substr(val+1);

				pItem.ulTrack	= track.toInt();
				pItem.ulMaxTrack = trackMax.toInt();
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
			pItem.ulMaxTrack	= mp4Tag["trkn"].toIntPair().second;
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

/*
	m_File->tag()->setTitle(pItem.title);
	m_File->tag()->setArtist(pItem.artist);
	m_File->tag()->setAlbum(pItem.album);
	m_File->tag()->setGenre(pItem.genre);
	m_File->tag()->setComment(pItem.comment);

	m_File->tag()->setYear(pItem.ulYear);
	m_File->tag()->setTrack(pItem.ulTrack);
*/
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
