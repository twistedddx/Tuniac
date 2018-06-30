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
	dsfFile = NULL;
	ebmlFile = NULL;
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
	return 35;
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
		TEXT(".spx"),
		TEXT(".opus"),
		TEXT(".tta"),
		TEXT(".m4a"),
		TEXT(".m4r"),
		TEXT(".m4b"),
		TEXT(".m4p"),
		TEXT(".mp4"),
		TEXT(".3g2"),
		TEXT(".alac"),
		TEXT(".wma"),
		TEXT(".asf"),
		TEXT(".aif"),
		TEXT(".aiff"),
		TEXT(".afc"),
		TEXT(".aifc"),
		TEXT(".wav"),
		TEXT(".ape"),
		TEXT(".mod"),
		TEXT(".module"),
		TEXT(".nst"),
		TEXT(".wow"),
		TEXT(".s3m"),
		TEXT(".it"),
		TEXT(".xm"),
		TEXT(".dsf")

	};

	return exts[ulExtentionNum];
}

bool			CSTDInfoManager::CanHandle(LPTSTR szSource)
{
	fileRef = TagLib::FileRef(szSource, true, TagLib::AudioProperties::Fast);
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
		else if (dsfFile = dynamic_cast<TagLib::DSF::File *>(fileRef.file()))
			return true;
		else if (ebmlFile = dynamic_cast<TagLib::EBML::File *>(fileRef.file()))
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
		libEnt->ulBitRate		= audioProps->bitrate() * 1000;
		libEnt->ulChannels		= audioProps->channels();
		if(audioProps->length() > 0)
			libEnt->ulPlaybackTime = audioProps->length() * 1000;
		else
			libEnt->ulPlaybackTime = LENGTH_UNKNOWN;
		libEnt->ulSampleRate		= audioProps->sampleRate();

		libEnt->ulBitsPerSample = 16; //more accurately updated per file type below
	}

	tag = fileRef.tag();
	if(tag)
	{
		StringCchCopy(libEnt->szTitle, 128, tag->title().toCWString());
		StringCchCopy(libEnt->szArtist, 128, tag->artist().toCWString());
		StringCchCopy(libEnt->szAlbum, 128, tag->album().toCWString());
		StringCchCopy(libEnt->szGenre, 128, tag->genre().toCWString());
		StringCchCopy(libEnt->szComment, 128, tag->comment().toCWString());

		libEnt->ulYear  = tag->year();
		libEnt->dwTrack[0] = tag->track();
	}

	if(flacFile)
	{
		TagLib::FLAC::AudioProperties * flacProps = 0;
		flacProps = dynamic_cast<TagLib::FLAC::AudioProperties *>(fileRef.audioProperties());
		libEnt->ulBitsPerSample = flacProps->bitsPerSample();
		StringCchCopy(libEnt->szFileType, 16, L"flac");
		if(flacFile->xiphComment())
			vorbisTagListMap = flacFile->xiphComment()->fieldListMap();
		if(flacFile->ID3v2Tag())
			id3v2TagListMap = flacFile->ID3v2Tag()->frameListMap();
	}
	else if(mp3File)
	{
		StringCchCopy(libEnt->szFileType, 16, L"mp3");
		if(mp3File->ID3v2Tag())
			id3v2TagListMap = mp3File->ID3v2Tag()->frameListMap();
		if(mp3File->APETag())
			apeTagListMap = mp3File->APETag()->itemListMap();

	}
	else if(mp4File)
	{
		TagLib::MP4::AudioProperties * mp4Props = 0;
		mp4Props = dynamic_cast<TagLib::MP4::AudioProperties *>(fileRef.audioProperties());
		libEnt->ulBitsPerSample = mp4Props->bitsPerSample();
		StringCchCopy(libEnt->szFileType, 16, L"mp4");
		if(mp4File->tag())
			mp4TagListMap = mp4File->tag()->itemListMap();
	}
	else if(mpcFile)
	{
		StringCchCopy(libEnt->szFileType, 16, L"mpc");
		if(mpcFile->APETag())
			apeTagListMap = mpcFile->APETag()->itemListMap();
	}
	else if(ttaFile)
	{
		TagLib::TrueAudio::AudioProperties * ttaProps = 0;
		ttaProps = dynamic_cast<TagLib::TrueAudio::AudioProperties *>(fileRef.audioProperties());
		libEnt->ulBitsPerSample = ttaProps->bitsPerSample();
		StringCchCopy(libEnt->szFileType, 16, L"tta");
		if(ttaFile->ID3v2Tag())
			id3v2TagListMap = ttaFile->ID3v2Tag()->frameListMap();
	}
	else if(wvFile)
	{
		TagLib::WavPack::AudioProperties * wvProps = 0;
		wvProps = dynamic_cast<TagLib::WavPack::AudioProperties *>(fileRef.audioProperties());
		libEnt->ulBitsPerSample = wvProps->bitsPerSample();
		StringCchCopy(libEnt->szFileType, 16, L"wv");
		if(wvFile->APETag())
			apeTagListMap = wvFile->APETag()->itemListMap();;
	}
	else if(oggFile)
	{
		StringCchCopy(libEnt->szFileType, 16, L"ogg");
		if(oggFile->tag())
			vorbisTagListMap = oggFile->tag()->fieldListMap();
	}
	else if(ogaFile)
	{
		TagLib::Ogg::FLAC::AudioProperties * ogaProps = 0;
		ogaProps = dynamic_cast<TagLib::Ogg::FLAC::AudioProperties *>(fileRef.audioProperties());
		libEnt->ulBitsPerSample = ogaProps->bitsPerSample();
		StringCchCopy(libEnt->szFileType, 16, L"oga");
		if(ogaFile->tag())
			vorbisTagListMap = ogaFile->tag()->fieldListMap();
	}
	else if(spxFile)
	{
		StringCchCopy(libEnt->szFileType, 16, L"spx");
		if(spxFile->tag())
			vorbisTagListMap = spxFile->tag()->fieldListMap();
	}
	else if(opusFile)
	{
		StringCchCopy(libEnt->szFileType, 16, L"opus");
		if(opusFile->tag())
			vorbisTagListMap = opusFile->tag()->fieldListMap();
	}
	else if(wmaFile)
	{
		TagLib::ASF::AudioProperties * asfProps = 0;
		asfProps = dynamic_cast<TagLib::ASF::AudioProperties *>(fileRef.audioProperties());
		libEnt->ulBitsPerSample = asfProps->bitsPerSample();
		StringCchCopy(libEnt->szFileType, 16, L"wma");
		if(wmaFile->tag())
			wmaTagListMap = wmaFile->tag()->attributeListMap();
	}
	else if(apeFile)
	{
		TagLib::APE::AudioProperties * apeProps = 0;
		apeProps = dynamic_cast<TagLib::APE::AudioProperties *>(fileRef.audioProperties());
		libEnt->ulBitsPerSample = apeProps->bitsPerSample();
		StringCchCopy(libEnt->szFileType, 16, L"ape");
		if(apeFile->APETag())
			apeTagListMap = apeFile->APETag()->itemListMap();
	}
	else if (aiffFile)
	{
		TagLib::RIFF::AIFF::AudioProperties * aiffProps = 0;
		aiffProps = dynamic_cast<TagLib::RIFF::AIFF::AudioProperties *>(fileRef.audioProperties());
		libEnt->ulBitsPerSample = aiffProps->bitsPerSample();
		StringCchCopy(libEnt->szFileType, 16, L"aiff");
	}
	else if (wavFile)
	{
		TagLib::RIFF::WAV::AudioProperties * wavProps = 0;
		wavProps = dynamic_cast<TagLib::RIFF::WAV::AudioProperties *>(fileRef.audioProperties());
		libEnt->ulBitsPerSample = wavProps->bitsPerSample();
		StringCchCopy(libEnt->szFileType, 16, L"wav");
	}
	else if (itFile)
	{
		StringCchCopy(libEnt->szFileType, 16, L"it");
	}
	else if (modFile)
	{
		StringCchCopy(libEnt->szFileType, 16, L"mod");
	}
	else if (s3mFile)
	{
		StringCchCopy(libEnt->szFileType, 16, L"s3m");
	}
	else if (xmFile)
	{
		StringCchCopy(libEnt->szFileType, 16, L"xm");
	}
	else if (dsfFile)
	{
		TagLib::DSF::AudioProperties * dsfProps = 0;
		dsfProps = dynamic_cast<TagLib::DSF::AudioProperties *>(fileRef.audioProperties());
		libEnt->ulBitsPerSample = dsfProps->bitsPerSample();
		StringCchCopy(libEnt->szFileType, 16, L"dsf");
	}
	else if (ebmlFile)
	{
		StringCchCopy(libEnt->szFileType, 16, L"ebml");
	}

	if(!id3v2TagListMap.isEmpty())
	{
		if(!id3v2TagListMap["TPE2"].isEmpty())
			StringCchCopy(libEnt->szAlbumArtist, 128, id3v2TagListMap["TPE2"].front()->toString().toCWString());

		if (!id3v2TagListMap["TCOM"].isEmpty())
			StringCchCopy(libEnt->szComposer, 128, id3v2TagListMap["TCOM"].front()->toString().toCWString());

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

		if(!id3v2TagListMap["RVA2"].isEmpty())
		{
			TagLib::ID3v2::RelativeVolumeFrame * relVol = static_cast<TagLib::ID3v2::RelativeVolumeFrame *>(id3v2TagListMap["RVA2"].front());
			libEnt->fReplayGain_Album_Gain  = relVol->volumeAdjustment();
			//libEnt->fReplayGain_Track_Peak  = relVol->peakVolume();
		}

		if(!id3v2TagListMap["TBPM"].isEmpty())
		{
			libEnt->ulBPM = id3v2TagListMap["TBPM"].front()->toString().toInt();
		}

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
	}

	if(!mp4TagListMap.isEmpty())
	{
		if(mp4TagListMap["aART"].isValid())
			StringCchCopy(libEnt->szAlbumArtist, 128, mp4TagListMap["aART"].toStringList().front().toCWString());

		if (mp4TagListMap["\251wrt"].isValid()) // ©wrt \xa9wrt
			StringCchCopy(libEnt->szComposer, 128, mp4TagListMap["\251wrt"].toStringList().front().toCWString());

		if(mp4TagListMap["disc"].isValid())
			libEnt->dwDisc[0] = mp4TagListMap["disc"].toInt();

		if(mp4TagListMap["trkn"].isValid())
			libEnt->dwTrack[1] = mp4TagListMap["trkn"].toIntPair().second;

		if(mp4TagListMap["tmpo"].isValid())
			libEnt->ulBPM = mp4TagListMap["tmpo"].toInt();
	}

	if(!vorbisTagListMap.isEmpty())
	{
		if(!vorbisTagListMap["ALBUMARTIST"].isEmpty())
			StringCchCopy(libEnt->szAlbumArtist, 128, vorbisTagListMap["ALBUMARTIST"].front().toCWString());
		else if(!vorbisTagListMap["ALBUM ARTIST"].isEmpty())
			StringCchCopy(libEnt->szAlbumArtist, 128, vorbisTagListMap["ALBUM ARTIST"].front().toCWString());
		else if(!vorbisTagListMap["ALBUM_ARTIST"].isEmpty())
			StringCchCopy(libEnt->szAlbumArtist, 128, vorbisTagListMap["ALBUM_ARTIST"].front().toCWString());

		if (!vorbisTagListMap["COMPOSER"].isEmpty())
			StringCchCopy(libEnt->szComposer, 128, vorbisTagListMap["COMPOSER"].front().toCWString());

		if(!vorbisTagListMap["DISCNUMBER"].isEmpty())
			libEnt->dwDisc[0] = vorbisTagListMap["DISCNUMBER"].front().toInt();
		if(!vorbisTagListMap["TOTALTRACKS"].isEmpty())
			libEnt->dwTrack[1] = vorbisTagListMap["TOTALTRACKS"].front().toInt();

		if(!vorbisTagListMap["REPLAYGAIN_TRACK_GAIN"].isEmpty())
			libEnt->fReplayGain_Track_Gain = atof(vorbisTagListMap["REPLAYGAIN_TRACK_GAIN"].front().toCString());
		if(!vorbisTagListMap["REPLAYGAIN_TRACK_PEAK"].isEmpty())
			libEnt->fReplayGain_Track_Peak = atof(vorbisTagListMap["REPLAYGAIN_TRACK_PEAK"].front().toCString());
		if(!vorbisTagListMap["REPLAYGAIN_ALBUM_GAIN"].isEmpty())
			libEnt->fReplayGain_Album_Gain = atof(vorbisTagListMap["REPLAYGAIN_ALBUM_GAIN"].front().toCString());
		if(!vorbisTagListMap["REPLAYGAIN_ALBUM_PEAK"].isEmpty())
			libEnt->fReplayGain_Album_Peak = atof(vorbisTagListMap["REPLAYGAIN_ALBUM_PEAK"].front().toCString());

		if (!vorbisTagListMap["R128_TRACK_GAIN"].isEmpty())
			libEnt->fReplayGain_Track_Gain = atof(vorbisTagListMap["R128_TRACK_GAIN"].front().toCString()) * 0.00390625;

		if(!vorbisTagListMap["BPM"].isEmpty())
			libEnt->ulBPM = vorbisTagListMap["BPM"].front().toInt();
		else if(!vorbisTagListMap["TEMPO"].isEmpty())
			libEnt->ulBPM = vorbisTagListMap["TEMPO"].front().toInt();
	}

	if(!apeTagListMap.isEmpty())
	{
		if(!apeTagListMap["ALBUMARTIST"].isEmpty())
			StringCchCopy(libEnt->szAlbumArtist, 128, apeTagListMap["ALBUMARTIST"].toString().toCWString());
		else if(!apeTagListMap["ALBUM ARTIST"].isEmpty())
			StringCchCopy(libEnt->szAlbumArtist, 128, apeTagListMap["ALBUM ARTIST"].toString().toCWString());
		else if(!apeTagListMap["ALBUM_ARTIST"].isEmpty())
			StringCchCopy(libEnt->szAlbumArtist, 128, apeTagListMap["ALBUM_ARTIST"].toString().toCWString());

		if (!apeTagListMap["Composer"].isEmpty())
			StringCchCopy(libEnt->szComposer, 128, apeTagListMap["Composer"].toString().toCWString());


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

		if(!apeTagListMap["BPM"].isEmpty())
			libEnt->ulBPM = apeTagListMap["BPM"].toString().toInt();
	}

	if(!wmaTagListMap.isEmpty())
	{
		if(!wmaTagListMap["WM/AlbumArtist"].isEmpty())
			StringCchCopy(libEnt->szAlbumArtist, 128, wmaTagListMap["WM/AlbumArtist"].front().toString().toCWString());

		if (!wmaTagListMap["WM/Composer"].isEmpty())
			StringCchCopy(libEnt->szComposer, 128, wmaTagListMap["WM/Composer"].front().toString().toCWString());

		if(!wmaTagListMap["WM/BeatsPerMinute"].isEmpty())
			libEnt->ulBPM = wmaTagListMap["WM/BeatsPerMinute"].front().toUInt();
	}

	fileRef = TagLib::FileRef();
	if(flacFile)
		flacFile = NULL;
	if(mp3File)
		mp3File = NULL;
	if(mp4File)
		mp4File = NULL;
	if(mpcFile)
		mpcFile = NULL;
	if(ttaFile)
		ttaFile = NULL;
	if(wvFile)
		wvFile = NULL;
	if(oggFile)
		oggFile = NULL;
	if(ogaFile)
		ogaFile = NULL;
	if(spxFile)
		spxFile = NULL;
	if(opusFile)
		opusFile = NULL;
	if(wmaFile)
		wmaFile = NULL;
	if(aiffFile)
		aiffFile = NULL;
	if(wavFile)
		wavFile = NULL;
	if(apeFile)
		apeFile = NULL;
	if(itFile)
		itFile = NULL;
	if(modFile)
		modFile = NULL;
	if(s3mFile)
		s3mFile = NULL;
	if (xmFile)
		xmFile = NULL;
	if (dsfFile)
		dsfFile = NULL;
	if (ebmlFile)
		ebmlFile = NULL;

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
		tag->setYear(libEnt->ulYear);

		if(mp3File = dynamic_cast<TagLib::MPEG::File *>( fileRef.file() ))
		{
			if(mp3File->save(TagLib::MPEG::File::AllTags, false, 3))
			{
				fileRef = TagLib::FileRef();
				return true;
			}
		}
		else if(fileRef.save())
		{
			fileRef = TagLib::FileRef();
			return true;
		}
	}

	fileRef = TagLib::FileRef();
	return false;
}

unsigned long	CSTDInfoManager::GetNumberOfAlbumArts(void)
{
	int count = 0;
	TagLib::ID3v2::FrameList id3TagList;

	if(mp3File = dynamic_cast<TagLib::MPEG::File *>( fileRef.file() ))
	{
		if(mp3File->ID3v2Tag()) 
			count = mp3File->ID3v2Tag()->frameListMap()["APIC"].size();
	}
	else if(flacFile = dynamic_cast<TagLib::FLAC::File *>( fileRef.file() ))
	{
		if(flacFile->pictureList().size())
			count = flacFile->pictureList().size();
		else if(flacFile->ID3v2Tag()) 
			count = flacFile->ID3v2Tag()->frameListMap()["APIC"].size();
	}
	else if (oggFile = dynamic_cast<TagLib::Ogg::Vorbis::File *>(fileRef.file()))
	{
		if (oggFile->tag())
			count = oggFile->tag()->pictureList().size();
	}
	else if (ogaFile = dynamic_cast<TagLib::Ogg::FLAC::File *>(fileRef.file()))
	{
		if (ogaFile->tag())
			count = ogaFile->tag()->pictureList().size();
	}
	else if (spxFile = dynamic_cast<TagLib::Ogg::Speex::File *>(fileRef.file()))
	{
		if (spxFile->tag())
			count = spxFile->tag()->pictureList().size();
	}
	else if (opusFile = dynamic_cast<TagLib::Ogg::Opus::File *>(fileRef.file()))
	{
		if (opusFile->tag())
			count = opusFile->tag()->pictureList().size();
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
	else if (ttaFile = dynamic_cast<TagLib::TrueAudio::File *>(fileRef.file()))
	{
		if (ttaFile->ID3v2Tag())
			count = ttaFile->ID3v2Tag()->frameListMap()["APIC"].size();
	}

	fileRef = TagLib::FileRef();
	return count;
}

bool			CSTDInfoManager::GetAlbumArt(	unsigned long		ulImageIndex,
												LPVOID			*	pImageData,
												unsigned long	*	ulImageDataSize,
												LPTSTR				szMimeType,
												unsigned long	*	ulArtType)
{
	bool bRet = false;

	TagLib::ID3v2::FrameList id3v2TagList;

	if(mp3File = dynamic_cast<TagLib::MPEG::File *>( fileRef.file() ))
	{
		if(mp3File->ID3v2Tag()) 
			id3v2TagList = mp3File->ID3v2Tag()->frameListMap()["APIC"];
	}
	else if(flacFile = dynamic_cast<TagLib::FLAC::File *>( fileRef.file() ))
	{
		if(ulImageIndex < flacFile->pictureList().size())
		{
			TagLib::FLAC::Picture * flacPic = flacFile->pictureList()[ulImageIndex];

			*ulImageDataSize = flacPic->data().size();
			*pImageData = malloc(*ulImageDataSize);

			CopyMemory(*pImageData, flacPic->data().data(), *ulImageDataSize);
			StringCchCopy((LPWSTR)szMimeType, 128, (LPTSTR)flacPic->mimeType().toCWString());

			*ulArtType = flacPic->type();
				bRet = true;
		}
		else if(flacFile->ID3v2Tag()) 
			id3v2TagList = flacFile->ID3v2Tag()->frameListMap()["APIC"];
	}
	else if (oggFile = dynamic_cast<TagLib::Ogg::Vorbis::File *>(fileRef.file() ))
	{
		if (oggFile->tag())
		{
			if (ulImageIndex < oggFile->tag()->pictureList().size())
			{
				TagLib::FLAC::Picture * flacPic = oggFile->tag()->pictureList()[ulImageIndex];

				*ulImageDataSize = flacPic->data().size();
				*pImageData = malloc(*ulImageDataSize);

				CopyMemory(*pImageData, flacPic->data().data(), *ulImageDataSize);
				StringCchCopy((LPWSTR)szMimeType, 128, (LPTSTR)flacPic->mimeType().toCWString());

				*ulArtType = flacPic->type();
				bRet = true;
			}
		}
	}
	else if (ogaFile = dynamic_cast<TagLib::Ogg::FLAC::File *>(fileRef.file()))
	{
		if (ogaFile->tag())
		{
			if (ulImageIndex < ogaFile->tag()->pictureList().size())
			{
				TagLib::FLAC::Picture * flacPic = oggFile->tag()->pictureList()[ulImageIndex];

				*ulImageDataSize = flacPic->data().size();
				*pImageData = malloc(*ulImageDataSize);

				CopyMemory(*pImageData, flacPic->data().data(), *ulImageDataSize);
				StringCchCopy((LPWSTR)szMimeType, 128, (LPTSTR)flacPic->mimeType().toCWString());

				*ulArtType = flacPic->type();
				bRet = true;
			}
		}
	}
	else if (spxFile = dynamic_cast<TagLib::Ogg::Speex::File *>(fileRef.file()))
	{
		if (spxFile->tag())
		{
			if (ulImageIndex < spxFile->tag()->pictureList().size())
			{
				TagLib::FLAC::Picture * flacPic = oggFile->tag()->pictureList()[ulImageIndex];

				*ulImageDataSize = flacPic->data().size();
				*pImageData = malloc(*ulImageDataSize);

				CopyMemory(*pImageData, flacPic->data().data(), *ulImageDataSize);
				StringCchCopy((LPWSTR)szMimeType, 128, (LPTSTR)flacPic->mimeType().toCWString());

				*ulArtType = flacPic->type();
				bRet = true;
			}
		}
	}
	else if (opusFile = dynamic_cast<TagLib::Ogg::Opus::File *>(fileRef.file()))
	{
		if (opusFile->tag())
		{
			if (ulImageIndex < opusFile->tag()->pictureList().size())
			{
				TagLib::FLAC::Picture * flacPic = oggFile->tag()->pictureList()[ulImageIndex];

				*ulImageDataSize = flacPic->data().size();
				*pImageData = malloc(*ulImageDataSize);

				CopyMemory(*pImageData, flacPic->data().data(), *ulImageDataSize);
				StringCchCopy((LPWSTR)szMimeType, 128, (LPTSTR)flacPic->mimeType().toCWString());

				*ulArtType = flacPic->type();
				bRet = true;
			}
		}
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
					StringCchCopy((LPWSTR)szMimeType, 128, (LPTSTR)asfPic.mimeType().toCWString());

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
						StringCchCopy(szMimeType, 128, TEXT("image/jpeg"));
					else if(coverArt.format() == 14)
						StringCchCopy(szMimeType, 128, TEXT("image/png"));
					else if(coverArt.format() == 27)
						StringCchCopy(szMimeType, 128, TEXT("image/bmp"));
					else if(coverArt.format() == 12)
						StringCchCopy(szMimeType, 128, TEXT("image/gif"));

					CopyMemory(*pImageData, coverArt.data().data(), *ulImageDataSize);

					*ulArtType = 3;
				
					bRet = true;
				}
			}
		}
	}
	else if (ttaFile = dynamic_cast<TagLib::TrueAudio::File *>(fileRef.file()))
	{
		if (ttaFile->ID3v2Tag())
			id3v2TagList = ttaFile->ID3v2Tag()->frameListMap()["APIC"];
	}

	if(!id3v2TagList.isEmpty() && (ulImageIndex < id3v2TagList.size()))
	{
		TagLib::ID3v2::AttachedPictureFrame * id3v2Pic = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(id3v2TagList[ulImageIndex]);

		*ulImageDataSize = id3v2Pic->picture().size();
		*pImageData = malloc(*ulImageDataSize);

		CopyMemory(*pImageData, id3v2Pic->picture().data(), *ulImageDataSize);
		StringCchCopy((LPWSTR)szMimeType, 128, (LPTSTR)id3v2Pic->mimeType().toCWString());

		*ulArtType = id3v2Pic->type();

		bRet = true;
	}
	fileRef = TagLib::FileRef();

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
