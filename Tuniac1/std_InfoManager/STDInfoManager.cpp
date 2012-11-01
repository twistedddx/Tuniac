////////////////////////////////////////////////////////////
//
//				EDIT THIS ONE YOU BIG BALD FUCK!
//
//

#include "stdafx.h"
#include "STDinfomanager.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}


extern "C" __declspec(dllexport) IInfoManager * CreateInfoManagerPlugin(void)
{
	IInfoManager * pInfo = new CSTDInfoManager;

	return(pInfo);
}

extern "C" __declspec(dllexport) unsigned long		GetTuniacInfoManagerVersion(void)
{
	return ITUNIACINFOMANAGER_VERSION;
}


// actual class definition here

CSTDInfoManager::CSTDInfoManager(void)
{
	flacFile = NULL;
	mp3File = NULL;
	mp4File = NULL;
	mpcFile = NULL;
	ttaFile = NULL;
	wvFile = NULL;
	oggFile = NULL;
	ogaFile = NULL;
	spxFile = NULL;
	opusFile = NULL;
	wmaFile = NULL;
	aiffFile = NULL;
	wavFile = NULL;
	apeFile = NULL;
	itFile = NULL;
	modFile = NULL;
	s3mFile = NULL;
	xmFile = NULL;

}

CSTDInfoManager::~CSTDInfoManager(void)
{
}

void			CSTDInfoManager::Destroy(void)
{
	delete this;
}

unsigned long	CSTDInfoManager::GetNumExtensions(void)
{
	return 30;
}

LPTSTR			CSTDInfoManager::SupportedExtension(unsigned long ulExtentionNum)
{
	static LPTSTR exts[] = 
	{
		TEXT(".flac"),
		TEXT(".fla"),
		TEXT(".mp3"),
		TEXT(".ogg"),
		TEXT(".oga"),
		TEXT(".mpc"),
		TEXT(".mp+"),
		TEXT(".mpp"),
		TEXT(".wv"),
		TEXT(".m4a"),
		TEXT(".m4b"),
		TEXT(".m4p"),
		TEXT(".mp4"),
		TEXT(".3g2"),
		TEXT(".wma"),
		TEXT(".tta"),
		TEXT(".spx"),
		TEXT(".opus"),
		TEXT(".aif"),
		TEXT(".aiff"),
		TEXT(".wav"),
		TEXT(".ape"),
		TEXT(".alac"),
		TEXT(".it"),
		TEXT(".mod"),
		TEXT(".module"),
		TEXT(".nst"),
		TEXT(".s3m"),
		TEXT(".wow"),
		TEXT(".xm"),

	};

	return exts[ulExtentionNum];
}

bool			CSTDInfoManager::CanHandle(LPTSTR szSource)
{
	fileRef = TagLib::FileRef(szSource, 1, TagLib::AudioProperties::Fast);
    if( !fileRef.isNull() )
    {
		if(flacFile = dynamic_cast<TagLib::FLAC::File *>( fileRef.file() ))
			return true;
		else if(mp3File = dynamic_cast<TagLib::MPEG::File *>( fileRef.file() ))
			return true;
		else if(mp4File = dynamic_cast<TagLib::MP4::File *>( fileRef.file() ))
			return true;
		else if(mpcFile = dynamic_cast<TagLib::MPC::File *>( fileRef.file() ))
			return true;
		else if(ttaFile = dynamic_cast<TagLib::TrueAudio::File *>( fileRef.file() ))
			return true;
		else if(wvFile = dynamic_cast<TagLib::WavPack::File *>( fileRef.file() ))
			return true;
		else if(oggFile = dynamic_cast<TagLib::Ogg::Vorbis::File *>( fileRef.file() ))
			return true;
		else if(ogaFile = dynamic_cast<TagLib::Ogg::FLAC::File *>( fileRef.file() ))
			return true;
		else if(spxFile = dynamic_cast<TagLib::Ogg::Speex::File *>( fileRef.file() ))
			return true;
		else if(opusFile = dynamic_cast<TagLib::Ogg::Opus::File *>( fileRef.file() ))
			return true;
		else if(wmaFile = dynamic_cast<TagLib::ASF::File *>( fileRef.file() ))
			return true;
		else if(aiffFile = dynamic_cast<TagLib::RIFF::AIFF::File *>( fileRef.file() ))
			return true;
		else if(wavFile = dynamic_cast<TagLib::RIFF::WAV::File *>( fileRef.file() ))
			return true;
		else if(apeFile = dynamic_cast<TagLib::APE::File *>( fileRef.file() ))
			return true;
		else if(itFile = dynamic_cast<TagLib::IT::File *>( fileRef.file() ))
			return true;
		else if(modFile = dynamic_cast<TagLib::Mod::File *>( fileRef.file() ))
			return true;
		else if(s3mFile = dynamic_cast<TagLib::S3M::File *>( fileRef.file() ))
			return true;
		else if(xmFile = dynamic_cast<TagLib::XM::File *>( fileRef.file() ))
			return true;
	}

	return false;

}

bool			CSTDInfoManager::GetInfo(LibraryEntry * libEnt)
{
	TagLib::Ogg::FieldListMap vorbisTagListMap;
	TagLib::APE::ItemListMap apeTagListMap;
	TagLib::ID3v2::FrameListMap id3v2TagListMap;
	TagLib::MP4::ItemListMap mp4TagListMap;
	TagLib::ASF::AttributeListMap wmaTagListMap;
	
	TagLib::AudioProperties * audioProps = 0;
	TagLib::Tag * tag = 0;

	audioProps = fileRef.audioProperties();
	if(audioProps)
	{
		libEnt->iBitRate		= audioProps->bitrate() * 1000;
		libEnt->iChannels		= audioProps->channels();
		libEnt->iPlaybackTime	= audioProps->length() * 1000;
		libEnt->iSampleRate		= audioProps->sampleRate();
	}

	tag = fileRef.tag();
	if(tag)
	{
		_snwprintf(libEnt->szTitle, 128, L"%s", tag->title().toWString().c_str());
		_snwprintf(libEnt->szArtist, 128, L"%s", tag->artist().toWString().c_str());
		_snwprintf(libEnt->szAlbum, 128, L"%s", tag->album().toWString().c_str());
		_snwprintf(libEnt->szGenre, 128, L"%s", tag->genre().toWString().c_str());
		_snwprintf(libEnt->szComment, 128, L"%s", tag->comment().toWString().c_str());
		libEnt->iYear  = tag->year();
		libEnt->dwTrack[0] = tag->track();
	}

	if(flacFile)
	{
		if(flacFile->xiphComment())
			vorbisTagListMap = flacFile->xiphComment()->fieldListMap();
		else if(flacFile->ID3v2Tag())
			id3v2TagListMap = flacFile->ID3v2Tag()->frameListMap();
	}
	else if(mp3File)
	{
		if(mp3File->APETag())
			apeTagListMap = mp3File->APETag()->itemListMap();
		else if(mp3File->ID3v2Tag())
			id3v2TagListMap = mp3File->ID3v2Tag()->frameListMap();
	}
	else if(mp4File)
	{
		if(mp4File->tag())
			mp4TagListMap = mp4File->tag()->itemListMap();
	}
	else if(mpcFile)
	{
		if(mpcFile->APETag())
			apeTagListMap = mpcFile->APETag()->itemListMap();
	}
	else if(ttaFile)
	{
		if(ttaFile->ID3v2Tag())
			id3v2TagListMap = ttaFile->ID3v2Tag()->frameListMap();
	}
	else if(wvFile)
	{
		if(wvFile->APETag())
			apeTagListMap = wvFile->APETag()->itemListMap();;
	}
	else if(oggFile)
	{
		if(oggFile->tag())
			vorbisTagListMap = oggFile->tag()->fieldListMap();
	}
	else if(ogaFile)
	{
		if(ogaFile->tag())
			vorbisTagListMap = ogaFile->tag()->fieldListMap();
	}
	else if(spxFile)
	{
		if(spxFile->tag())
			vorbisTagListMap = spxFile->tag()->fieldListMap();
	}
	else if(opusFile)
	{
		if(opusFile->tag())
			vorbisTagListMap = opusFile->tag()->fieldListMap();
	}
	else if(wmaFile)
	{
		if(wmaFile->tag())
			wmaTagListMap = wmaFile->tag()->attributeListMap();
	}
	else if(apeFile)
	{
		if(apeFile->APETag())
			apeTagListMap = apeFile->APETag()->itemListMap();
	}

	if(!id3v2TagListMap.isEmpty())
	{
		if(!id3v2TagListMap["TRCK"].isEmpty())
		{
			//std::cout << l.front()->toString() << std::endl;
			TagLib::String pszData = id3v2TagListMap["TRCK"].front()->toString();
			int val = pszData.find("/");
			if(val != -1)
			{
				TagLib::String trackMax = pszData.substr(val+1);
				libEnt->dwTrack[1] = trackMax.toInt();
			}
		}

		if(!id3v2TagListMap["TPOS"].isEmpty())
		{
			//std::cout << l.front()->toString() << std::endl;
			TagLib::String pszData = id3v2TagListMap["TPOS"].front()->toString();
			int val = pszData.find("/");
			if(val == -1)
			{
				libEnt->dwDisc[0] = pszData.toInt();
			}
			else
			{
				TagLib::String disk = pszData.substr(0,val);
				TagLib::String diskMax = pszData.substr(val+1);
				libEnt->dwDisc[0] = disk.toInt();
				libEnt->dwDisc[1] = diskMax.toInt();
			}
		}

		/* we dont get pictures here see GetAlbumArt()
		if(!id3v2TagListMap["APIC"].isEmpty())
		{
				TagLib::ID3v2::AttachedPictureFrame *id3v2Pic = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(id3v2TagListMap["APIC"].front());
				id3v2Pic->mimeType();
		}
		*/

		/*
				<Header for 'Replay Gain Adjustment', ID: "RGAD">
		Peak Amplitude                          $xx $xx $xx $xx
		Radio Replay Gain Adjustment            $xx $xx
		Audiophile Replay Gain Adjustment       $xx $xx

		Header consists of:
		Frame ID                $52 $47 $41 $44 = "RGAD"
		Size                    $00 $00 $00 $08
		Flags                   $40 $00         (%01000000 %00000000)

		In the RGAD frame, the flags state that the frame should be preserved if the ID3v2 
		tag is altered, but discarded if the audio data is altered.
		if(!id3v2TagListMap["RGAD"].isEmpty())
		{
				//_ASSERT(0);
				// we dont actually support this yet and need to decode the data
		}
		*/

		if(!id3v2TagListMap["RVA2"].isEmpty())
		{
			TagLib::ID3v2::RelativeVolumeFrame * relVol = static_cast<TagLib::ID3v2::RelativeVolumeFrame *>(id3v2TagListMap["RVA2"].front());
			libEnt->fReplayGain_Album_Gain  = relVol->volumeAdjustment();
			//libEnt->fReplayGain_Track_Peak  = relVol->peakVolume();
		}
	}

	if(!mp4TagListMap.isEmpty())
	{
		if(mp4TagListMap["disc"].isValid())
			libEnt->dwDisc[0] = mp4TagListMap["disc"].toInt();

		if(mp4TagListMap["trkn"].isValid())
			libEnt->dwTrack[1] = mp4TagListMap["trkn"].toIntPair().second;
	}

	if(!vorbisTagListMap.isEmpty())
	{
		if(!vorbisTagListMap["DISCNUMBER"].isEmpty())
			libEnt->dwDisc[0] = vorbisTagListMap["DISCNUMBER"].toString().toInt();
		if(!vorbisTagListMap["TOTALTRACKS"].isEmpty())
			libEnt->dwTrack[1] = vorbisTagListMap["TOTALTRACKS"].toString().toInt();

		if(!vorbisTagListMap["REPLAYGAIN_TRACK_GAIN"].isEmpty())
			libEnt->fReplayGain_Track_Gain = atof(vorbisTagListMap["REPLAYGAIN_TRACK_GAIN"].toString().toCString());
		if(!vorbisTagListMap["REPLAYGAIN_TRACK_PEAK"].isEmpty())
			libEnt->fReplayGain_Track_Peak = atof(vorbisTagListMap["REPLAYGAIN_TRACK_PEAK"].toString().toCString());
		if(!vorbisTagListMap["REPLAYGAIN_ALBUM_GAIN"].isEmpty())
			libEnt->fReplayGain_Album_Gain = atof(vorbisTagListMap["REPLAYGAIN_ALBUM_GAIN"].toString().toCString());;
		if(!vorbisTagListMap["REPLAYGAIN_ALBUM_PEAK"].isEmpty())
			libEnt->fReplayGain_Album_Peak = atof(vorbisTagListMap["REPLAYGAIN_ALBUM_PEAK"].toString().toCString());;
	}

	if(!apeTagListMap.isEmpty())
	{
		if(!apeTagListMap["Track"].isEmpty())
		{
			TagLib::String pszData = apeTagListMap["Track"].toString();
			int val = pszData.find("/");
			if(val != -1)
			{
				TagLib::String trackMax = pszData.substr(val+1);
				libEnt->dwTrack[1] = trackMax.toInt();
			}
		}
		if(!apeTagListMap["disc"].isEmpty())
			libEnt->dwDisc[0] = apeTagListMap["disc"].toString().toInt();

		if(!apeTagListMap["REPLAYGAIN_TRACK_GAIN"].isEmpty())
			libEnt->fReplayGain_Track_Gain = atof(apeTagListMap["REPLAYGAIN_TRACK_GAIN"].toString().toCString());
		if(!apeTagListMap["REPLAYGAIN_TRACK_PEAK"].isEmpty())
			libEnt->fReplayGain_Track_Peak = atof(apeTagListMap["REPLAYGAIN_TRACK_PEAK"].toString().toCString());
		if(!apeTagListMap["REPLAYGAIN_ALBUM_GAIN"].isEmpty())
			libEnt->fReplayGain_Album_Gain = atof(apeTagListMap["REPLAYGAIN_ALBUM_GAIN"].toString().toCString());;
		if(!apeTagListMap["REPLAYGAIN_ALBUM_PEAK"].isEmpty())
			libEnt->fReplayGain_Album_Peak = atof(apeTagListMap["REPLAYGAIN_ALBUM_PEAK"].toString().toCString());;
	}
	fileRef = TagLib::FileRef();
	return true;
}

//todo bits: per column tag writing?
//bool			CSTDInfoManager::SetInfo(LPTSTR szFilename, unsigned long ulFieldID, void * pNewData)
bool			CSTDInfoManager::SetInfo(LibraryEntry * libEnt)
{
	TagLib::Tag * tag = 0;

	tag = fileRef.tag();
	if(tag)
	{
		tag->setAlbum(libEnt->szAlbum);
		tag->setArtist(libEnt->szArtist);
		tag->setComment(libEnt->szComment);
		tag->setGenre(libEnt->szGenre);
		tag->setTitle(libEnt->szTitle);
		tag->setTrack(libEnt->dwTrack[0]);
		tag->setYear(libEnt->iYear);

		if(fileRef.save())
		{
			fileRef = TagLib::FileRef();
			return true;
		}
	}

	fileRef = TagLib::FileRef();
	return false;
}

unsigned long	CSTDInfoManager::GetNumberOfAlbumArts(LPTSTR		szFilename)
{
	int count = 0;

	TagLib::ID3v2::FrameList id3TagList;
	fileRef = TagLib::FileRef(szFilename, false);

    if( !fileRef.isNull() )
    {
		if(mp3File = dynamic_cast<TagLib::MPEG::File *>( fileRef.file() ))
		{
			if(mp3File->ID3v2Tag()) 
				count = mp3File->ID3v2Tag()->frameListMap()["APIC"].size();
		}

		else if(ttaFile = dynamic_cast<TagLib::TrueAudio::File *>( fileRef.file() ))
		{
			if(ttaFile->ID3v2Tag()) 
				count = ttaFile->ID3v2Tag()->frameListMap()["APIC"].size();
		}

		else if(flacFile = dynamic_cast<TagLib::FLAC::File *>( fileRef.file() ))
		{
			if(flacFile->pictureList().size())
				count = flacFile->pictureList().size();
			else if(flacFile->ID3v2Tag()) 
				count = flacFile->ID3v2Tag()->frameListMap()["APIC"].size();
		}
		else if(wmaFile = dynamic_cast<TagLib::ASF::File *>( fileRef.file() ))
		{
			if(wmaFile->tag());
				count = wmaFile->tag()->attributeListMap()["WM/Picture"].size();
		}
		else if(mp4File = dynamic_cast<TagLib::MP4::File *>( fileRef.file() ))
		{
			if(mp4File->tag())
				count = mp4File->tag()->itemListMap()["covr"].toCoverArtList().size();
		}

	}

	fileRef = TagLib::FileRef();
	return count;
}

bool			CSTDInfoManager::GetAlbumArt(	LPTSTR				szFilename, 
												unsigned long		ulImageIndex,
												LPVOID			*	pImageData,
												unsigned long	*	ulImageDataSize,
												LPTSTR				szMimeType,
												unsigned long	*	ulArtType)
{
	bool bRet = false;

	TagLib::ID3v2::FrameList id3v2TagList;
	fileRef = TagLib::FileRef(szFilename, false);

    if( !fileRef.isNull() )
    {

		if(mp3File = dynamic_cast<TagLib::MPEG::File *>( fileRef.file() ))
		{
			if(mp3File->ID3v2Tag()) 
				id3v2TagList = mp3File->ID3v2Tag()->frameListMap()["APIC"];
		}
		else if(ttaFile = dynamic_cast<TagLib::TrueAudio::File *>( fileRef.file() ))
		{
			if(ttaFile->ID3v2Tag()) 
				id3v2TagList = ttaFile->ID3v2Tag()->frameListMap()["APIC"];
		}
		else if(flacFile = dynamic_cast<TagLib::FLAC::File *>( fileRef.file() ))
		{
			if(ulImageIndex < flacFile->pictureList().size())
			{
				TagLib::FLAC::Picture * flacPic = flacFile->pictureList()[ulImageIndex];

				*ulImageDataSize = flacPic->data().size();
				*pImageData = malloc(*ulImageDataSize);

				CopyMemory(*pImageData, flacPic->data().data(), *ulImageDataSize);
				StrCpy((LPWSTR)szMimeType, (LPTSTR)flacPic->mimeType().toWString().c_str());

				*ulArtType = flacPic->type();
				 bRet = true;
			}
			else if(flacFile->ID3v2Tag()) 
				id3v2TagList = flacFile->ID3v2Tag()->frameListMap()["APIC"];
		}
		else if(wmaFile = dynamic_cast<TagLib::ASF::File *>( fileRef.file() ))
		{
			if(wmaFile->tag());
			{
				TagLib::ASF::AttributeList wmaTagList = wmaFile->tag()->attributeListMap()["WM/Picture"];

				if(!wmaTagList.isEmpty() && (ulImageIndex < wmaTagList.size()))
				{
					TagLib::ASF::Picture asfPic = wmaTagList[ulImageIndex].toPicture();
					if(asfPic.isValid())
					{
						*ulImageDataSize = asfPic.picture().size();
						*pImageData = malloc(*ulImageDataSize);

						CopyMemory(*pImageData, asfPic.picture().data(), *ulImageDataSize);
						StrCpy((LPWSTR)szMimeType, (LPTSTR)asfPic.mimeType().toWString().c_str());

						*ulArtType = asfPic.type();

						bRet = true;
					}
				}
			}
		}
		else if(mp4File = dynamic_cast<TagLib::MP4::File *>( fileRef.file() ))
		{
			if(mp4File->tag())
			{
				TagLib::MP4::Item mp4TagItem = mp4File->tag()->itemListMap()["covr"];

				if(mp4TagItem.isValid())
				{
					if(ulImageIndex < mp4TagItem.toCoverArtList().size())
					{
						TagLib::MP4::CoverArt coverArt = mp4TagItem.toCoverArtList()[ulImageIndex];

						*ulImageDataSize = coverArt.data().size();
						*pImageData = malloc(*ulImageDataSize);

						if(coverArt.format() == 13)
							StrCpy(szMimeType, TEXT("image/jpeg"));
						else if(coverArt.format() == 14)
							StrCpy(szMimeType, TEXT("image/png"));
						else if(coverArt.format() == 27)
							StrCpy(szMimeType, TEXT("image/bmp"));
						else if(coverArt.format() == 12)
							StrCpy(szMimeType, TEXT("image/gif"));

						CopyMemory(*pImageData, coverArt.data().data(), *ulImageDataSize);

						*ulArtType = 3;
				
						bRet = true;
					}
				}
			}
		}

		if(!id3v2TagList.isEmpty() && (ulImageIndex < id3v2TagList.size()))
		{
			TagLib::ID3v2::AttachedPictureFrame * id3v2Pic = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(id3v2TagList[ulImageIndex]);

			*ulImageDataSize = id3v2Pic->picture().size();
			*pImageData = malloc(*ulImageDataSize);

			CopyMemory(*pImageData, id3v2Pic->picture().data(), *ulImageDataSize);
			StrCpy((LPWSTR)szMimeType, (LPTSTR)id3v2Pic->mimeType().toWString().c_str());

			*ulArtType = id3v2Pic->type();

			bRet = true;
		}
		fileRef = TagLib::FileRef();
	}

	return bRet;
}

bool			CSTDInfoManager::FreeAlbumArt(LPVOID				pImageData)
{
	if(pImageData)
	{
		free(pImageData);
		return true;
	}

	return false;
}
