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
	flacfile = NULL;
	mp3file = NULL;
	mp4file = NULL;
	mpcfile = NULL;
	ttafile = NULL;
	wvfile = NULL;
	oggfile = NULL;
	ogafile = NULL;
	spxfile = NULL;
	wmafile = NULL;
	aiffile = NULL;
	wavfile = NULL;
	apefile = NULL;
	itfile = NULL;
	modfile = NULL;
	s3mfile = NULL;
	xmfile = NULL;

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
	return 29;
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
	fileref = TagLib::FileRef(szSource, 1, TagLib::AudioProperties::Fast);
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
		else if(apefile = dynamic_cast<TagLib::APE::File *>( fileref.file() ))
			return true;
		else if(itfile = dynamic_cast<TagLib::IT::File *>( fileref.file() ))
			return true;
		else if(modfile = dynamic_cast<TagLib::Mod::File *>( fileref.file() ))
			return true;
		else if(s3mfile = dynamic_cast<TagLib::S3M::File *>( fileref.file() ))
			return true;
		else if(xmfile = dynamic_cast<TagLib::XM::File *>( fileref.file() ))
			return true;
	}

	return false;

}

bool			CSTDInfoManager::GetInfo(LibraryEntry * libEnt)
{
	TagLib::Ogg::FieldListMap vorbisTag;
	TagLib::APE::ItemListMap apeTag;
	TagLib::ID3v2::FrameListMap id3v2Tag;
	TagLib::MP4::ItemListMap mp4Tag;
	TagLib::ASF::AttributeListMap wmaTag;
	
	TagLib::AudioProperties *audprops = 0;
	TagLib::Tag *tag = 0;

	audprops = fileref.audioProperties();
	if(audprops)
	{
		libEnt->iBitRate		= audprops->bitrate() * 1000;
		libEnt->iChannels		= audprops->channels();
		libEnt->iPlaybackTime	= audprops->length() * 1000;
		libEnt->iSampleRate		= audprops->sampleRate();
	}

	tag = fileref.tag();
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

	if(flacfile)
	{
		if(flacfile->xiphComment())
			vorbisTag = flacfile->xiphComment()->fieldListMap();
		else if(flacfile->ID3v2Tag())
			id3v2Tag = flacfile->ID3v2Tag()->frameListMap();
	}
	else if(mp3file)
	{
		if(mp3file->APETag())
			apeTag = mp3file->APETag()->itemListMap();
		else if(mp3file->ID3v2Tag())
			id3v2Tag = mp3file->ID3v2Tag()->frameListMap();
	}
	else if(mp4file)
	{
		if(mp4file->tag())
			mp4Tag = mp4file->tag()->itemListMap();
	}
	else if(mpcfile)
	{
		if(mpcfile->APETag())
			apeTag = mpcfile->APETag()->itemListMap();
	}
	else if(ttafile)
	{
		if(ttafile->ID3v2Tag())
			id3v2Tag = ttafile->ID3v2Tag()->frameListMap();
	}
	else if(wvfile)
	{
		if(wvfile->APETag())
			apeTag = wvfile->APETag()->itemListMap();;
	}
	else if(oggfile)
	{
		if(oggfile->tag())
			vorbisTag = oggfile->tag()->fieldListMap();
	}
	else if(ogafile)
	{
		if(ogafile->tag())
			vorbisTag = ogafile->tag()->fieldListMap();
	}
	else if(spxfile)
	{
		if(spxfile->tag())
			vorbisTag = spxfile->tag()->fieldListMap();
	}
	else if(wmafile)
	{
		if(wmafile->tag())
			wmaTag = wmafile->tag()->attributeListMap();
	}
	else if(apefile)
	{
		if(apefile->APETag())
			apeTag = apefile->APETag()->itemListMap();
	}

	if(!id3v2Tag.isEmpty())
	{
		if(!id3v2Tag["TRCK"].isEmpty())
		{
			//std::cout << l.front()->toString() << std::endl;
			TagLib::String pszData = id3v2Tag["TRCK"].front()->toString();
			int val = pszData.find("/");
			if(val != -1)
			{
				TagLib::String trackMax = pszData.substr(val+1);
				libEnt->dwTrack[1] = trackMax.toInt();
			}
		}

		if(!id3v2Tag["TPOS"].isEmpty())
		{
			//std::cout << l.front()->toString() << std::endl;
			TagLib::String pszData = id3v2Tag["TPOS"].front()->toString();
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
		if(!id3v2Tag["APIC"].isEmpty())
		{
				TagLib::ID3v2::AttachedPictureFrame *picframe = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(id3v2Tag["APIC"].front());
				picframe->mimeType();
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
		if(!id3v2Tag["RGAD"].isEmpty())
		{
				//_ASSERT(0);
				// we dont actually support this yet and need to decode the data
		}
		*/

		if(!id3v2Tag["RVA2"].isEmpty())
		{
			TagLib::ID3v2::RelativeVolumeFrame * relVol = static_cast<TagLib::ID3v2::RelativeVolumeFrame *>(id3v2Tag["RVA2"].front());
			libEnt->fReplayGain_Album_Gain  = relVol->volumeAdjustment();
			//libEnt->fReplayGain_Track_Peak  = relVol->peakVolume();
		}
	}

	if(!mp4Tag.isEmpty())
	{
		if(mp4Tag["disc"].isValid())
			libEnt->dwDisc[0] = mp4Tag["disc"].toInt();

		if(mp4Tag["trkn"].isValid())
			libEnt->dwTrack[1] = mp4Tag["trkn"].toIntPair().second;
	}

	if(!vorbisTag.isEmpty())
	{
		if(!vorbisTag["DISCNUMBER"].isEmpty())
			libEnt->dwDisc[0] = vorbisTag["DISCNUMBER"].toString().toInt();
		if(!vorbisTag["TOTALTRACKS"].isEmpty())
			libEnt->dwTrack[1] = vorbisTag["TOTALTRACKS"].toString().toInt();

		if(!vorbisTag["REPLAYGAIN_TRACK_GAIN"].isEmpty())
			libEnt->fReplayGain_Track_Gain = atof(vorbisTag["REPLAYGAIN_TRACK_GAIN"].toString().toCString());
		if(!vorbisTag["REPLAYGAIN_TRACK_PEAK"].isEmpty())
			libEnt->fReplayGain_Track_Peak = atof(vorbisTag["REPLAYGAIN_TRACK_PEAK"].toString().toCString());
		if(!vorbisTag["REPLAYGAIN_ALBUM_GAIN"].isEmpty())
			libEnt->fReplayGain_Album_Gain = atof(vorbisTag["REPLAYGAIN_ALBUM_GAIN"].toString().toCString());;
		if(!vorbisTag["REPLAYGAIN_ALBUM_PEAK"].isEmpty())
			libEnt->fReplayGain_Album_Peak = atof(vorbisTag["REPLAYGAIN_ALBUM_PEAK"].toString().toCString());;
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
				libEnt->dwTrack[1] = trackMax.toInt();
			}
		}
		if(!apeTag["disc"].isEmpty())
			libEnt->dwDisc[0] = apeTag["disc"].toString().toInt();

		if(!apeTag["REPLAYGAIN_TRACK_GAIN"].isEmpty())
			libEnt->fReplayGain_Track_Gain = atof(apeTag["REPLAYGAIN_TRACK_GAIN"].toString().toCString());
		if(!apeTag["REPLAYGAIN_TRACK_PEAK"].isEmpty())
			libEnt->fReplayGain_Track_Peak = atof(apeTag["REPLAYGAIN_TRACK_PEAK"].toString().toCString());
		if(!apeTag["REPLAYGAIN_ALBUM_GAIN"].isEmpty())
			libEnt->fReplayGain_Album_Gain = atof(apeTag["REPLAYGAIN_ALBUM_GAIN"].toString().toCString());;
		if(!apeTag["REPLAYGAIN_ALBUM_PEAK"].isEmpty())
			libEnt->fReplayGain_Album_Peak = atof(apeTag["REPLAYGAIN_ALBUM_PEAK"].toString().toCString());;
	}
	fileref = TagLib::FileRef();
	return true;
}

//todo bits: per column tag writing?
//bool			CSTDInfoManager::SetInfo(LPTSTR szFilename, unsigned long ulFieldID, void * pNewData)
bool			CSTDInfoManager::SetInfo(LibraryEntry * libEnt)
{
	TagLib::Tag *tag = 0;

	tag = fileref.tag();
	if(tag)
	{
		tag->setAlbum(libEnt->szAlbum);
		tag->setArtist(libEnt->szArtist);
		tag->setComment(libEnt->szComment);
		tag->setGenre(libEnt->szGenre);
		tag->setTitle(libEnt->szTitle);
		tag->setTrack(libEnt->dwTrack[0]);
		tag->setYear(libEnt->iYear);

		if(fileref.save())
		{
			fileref = TagLib::FileRef();
			return true;
		}
	}

	fileref = TagLib::FileRef();
	return false;
}

unsigned long	CSTDInfoManager::GetNumberOfAlbumArts(LPTSTR		szFilename)
{
	int count = 0;

	fileref = TagLib::FileRef(szFilename, false);

    if( !fileref.isNull() )
    {
		if(mp3file = dynamic_cast<TagLib::MPEG::File *>( fileref.file() ))
		{
			if(mp3file->ID3v2Tag()) 
				count = mp3file->ID3v2Tag()->frameListMap()["APIC"].size();
		}

		else if(ttafile = dynamic_cast<TagLib::TrueAudio::File *>( fileref.file() ))
		{
			if(ttafile->ID3v2Tag()) 
				count = ttafile->ID3v2Tag()->frameListMap()["APIC"].size();
		}

		else if(flacfile = dynamic_cast<TagLib::FLAC::File *>( fileref.file() ))
		{
			if(flacfile->pictureList().size())
				count = flacfile->pictureList().size();
			else if(flacfile->ID3v2Tag()) 
				count = flacfile->ID3v2Tag()->frameListMap()["APIC"].size();
		}
		else if(wmafile = dynamic_cast<TagLib::ASF::File *>( fileref.file() ))
		{
			if(wmafile->tag());
				count = wmafile->tag()->attributeListMap()["WM/Picture"].size();
		}
		else if(mp4file = dynamic_cast<TagLib::MP4::File *>( fileref.file() ))
		{
			if(mp4file->tag())
				count = mp4file->tag()->itemListMap()["covr"].toCoverArtList().size();
		}

	}

	fileref = TagLib::FileRef();
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

	TagLib::ID3v2::FrameList id3Tag;
	fileref = TagLib::FileRef(szFilename, false);

    if( !fileref.isNull() )
    {

		if(mp3file = dynamic_cast<TagLib::MPEG::File *>( fileref.file() ))
		{
			if(mp3file->ID3v2Tag()) 
				id3Tag = mp3file->ID3v2Tag()->frameListMap()["APIC"];
		}
		else if(ttafile = dynamic_cast<TagLib::TrueAudio::File *>( fileref.file() ))
		{
			if(ttafile->ID3v2Tag()) 
				id3Tag = ttafile->ID3v2Tag()->frameListMap()["APIC"];
		}
		else if(flacfile = dynamic_cast<TagLib::FLAC::File *>( fileref.file() ))
		{
			if(ulImageIndex < flacfile->pictureList().size())
			{
				TagLib::FLAC::Picture *pic = flacfile->pictureList()[ulImageIndex];

				*ulImageDataSize = pic->data().size();
				*pImageData = malloc(*ulImageDataSize);

				CopyMemory(*pImageData, pic->data().data(), *ulImageDataSize);
				StrCpy((LPWSTR)szMimeType, (LPTSTR)pic->mimeType().toWString().c_str());

				*ulArtType = pic->type();
				 bRet = true;
			}
			else if(flacfile->ID3v2Tag()) 
				id3Tag = flacfile->ID3v2Tag()->frameListMap()["APIC"];
		}
		else if(wmafile = dynamic_cast<TagLib::ASF::File *>( fileref.file() ))
		{
			if(wmafile->tag());
			{
				TagLib::ASF::AttributeList wmaTag = wmafile->tag()->attributeListMap()["WM/Picture"];

				if(!wmaTag.isEmpty() && (ulImageIndex < wmaTag.size()))
				{
					TagLib::ASF::Picture pic = wmaTag[ulImageIndex].toPicture();
					if(pic.isValid())
					{
						*ulImageDataSize = pic.picture().size();
						*pImageData = malloc(*ulImageDataSize);

						CopyMemory(*pImageData, pic.picture().data(), *ulImageDataSize);
						StrCpy((LPWSTR)szMimeType, (LPTSTR)pic.mimeType().toWString().c_str());

						*ulArtType = pic.type();

						bRet = true;
					}
				}
			}
		}
		else if(mp4file = dynamic_cast<TagLib::MP4::File *>( fileref.file() ))
		{
			if(mp4file->tag())
			{
				TagLib::MP4::Item mp4Item = mp4file->tag()->itemListMap()["covr"];

				if(mp4Item.isValid())
				{
					if(ulImageIndex < mp4Item.toCoverArtList().size())
					{
						TagLib::MP4::CoverArt coverArt = mp4Item.toCoverArtList()[ulImageIndex];

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

		if(!id3Tag.isEmpty() && (ulImageIndex < id3Tag.size()))
		{
			TagLib::ID3v2::AttachedPictureFrame *picframe = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(id3Tag[ulImageIndex]);

			*ulImageDataSize = picframe->picture().size();
			*pImageData = malloc(*ulImageDataSize);

			CopyMemory(*pImageData, picframe->picture().data(), *ulImageDataSize);
			StrCpy((LPWSTR)szMimeType, (LPTSTR)picframe->mimeType().toWString().c_str());

			*ulArtType = picframe->type();

			bRet = true;
		}
		fileref = TagLib::FileRef();
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
