////////////////////////////////////////////////////////////
//
//				EDIT THIS ONE YOU BIG BALD FUCK!
//
//

#include "stdafx.h"
#include "STDinfomanager.h"

#include "tag.h"
#include "tfile.h"
#include "fileref.h"
#include "mpeg/mpegfile.h"
#include "mpc/mpcfile.h"
#include "trueaudio/trueaudiofile.h"
#include "wavpack/wavpackfile.h"
#include "flac/flacfile.h"
#include "ogg/vorbis/vorbisfile.h"
//#include "mp4/mp4file.h"

#include "mpeg/id3v2/id3v2tag.h"
#include "mpeg/id3v2/frames/attachedpictureframe.h"
#include "mpeg/id3v2/frames/relativevolumeframe.h"
#include "ape/apetag.h"
#include "xiphcomment.h"
//#include "mp4tag.h"




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
	return 13;
}

LPTSTR			CSTDInfoManager::SupportedExtension(unsigned long ulExtentionNum)
{
	static LPTSTR exts[] = 
	{
		TEXT(".flac"),
		TEXT(".mp3"),
		TEXT(".ogg"),
		TEXT(".mpc"),
		TEXT(".wv"),
		TEXT(".m4a"),
		TEXT(".m4b"),
		TEXT(".m4p"),
		TEXT(".mp4"),
		TEXT(".3g2"),
		TEXT(".wma"),
		TEXT(".tta"),
		TEXT(".spx")
	};

	return exts[ulExtentionNum];
}

bool			CSTDInfoManager::CanHandle(LPTSTR szSource)
{
	if(PathIsURL(szSource))
		return(false);


	TagLib::String sFilename = szSource;

	// lets only deal with formats we know about pls....
	TagLib::StringList list =  TagLib::FileRef::defaultFileExtensions();
	for(unsigned long x=0; x<list.size(); x++)
	{
		if(sFilename.find(list[x]) != -1)
		{
			return true;
		}
	}

	return false;
/*
	for(unsigned int x=0; x<GetNumExtensions(); x++)
	{
		if(!StrCmpI(SupportedExtension(x), PathFindExtension(szSource)))
		{
			return(true);
		}
	}

	return(false);
	*/
}

bool			CSTDInfoManager::GetInfo(LibraryEntry * libEnt)
{
	TagLib::File		*		m_File;
	m_File = TagLib::FileRef::create(libEnt->szURL, 1, TagLib::AudioProperties::Fast);

	if(m_File)
	{
		libEnt->iSampleRate = m_File->audioProperties()->sampleRate();
		libEnt->iChannels = m_File->audioProperties()->channels();
		libEnt->iBitRate = m_File->audioProperties()->bitrate() * 1000;
		libEnt->iPlaybackTime = m_File->audioProperties()->length() * 1000;

		libEnt->iYear = m_File->tag()->year();
		libEnt->dwTrack[0] = m_File->tag()->track();

		swprintf(libEnt->szTitle, 128, L"%s", m_File->tag()->title().toWString().c_str());

		swprintf(libEnt->szArtist, 128, L"%s", m_File->tag()->artist().toWString().c_str());

		swprintf(libEnt->szAlbum, 128, L"%s", m_File->tag()->album().toWString().c_str());

		swprintf(libEnt->szGenre, 128, L"%s", m_File->tag()->genre().toWString().c_str());

		swprintf(libEnt->szComment, 128, L"%s", m_File->tag()->comment().toWString().c_str());
	}

	TagLib::Ogg::FieldListMap vorbisTag;
	TagLib::APE::ItemListMap apeTag;
	TagLib::ID3v2::FrameListMap id3Tag;

	//vorbis comment, id3v2
	if(!StrCmpI(TEXT(".flac"), PathFindExtension(libEnt->szURL)))
	{
		TagLib::FLAC::File * tagFile = (TagLib::FLAC::File *)m_File;
		if(tagFile->xiphComment())
			vorbisTag = tagFile->xiphComment()->fieldListMap();

		if(tagFile->ID3v2Tag()) 
			id3Tag = tagFile->ID3v2Tag()->frameListMap();
	}

/*
	else if(!StrCmpI(TEXT(".mp4"), PathFindExtension(libEnt->szURL)))
	{
		TagLib::MP4::Tag * tagFile = (TagLib::MP4::File *)m_File;
		if(tagFile)
		{

		}

	}
*/

	//vorbis comment
	else if(!StrCmpI(TEXT(".ogg"), PathFindExtension(libEnt->szURL)))
	{
		TagLib::Ogg::Vorbis::File * tagFile = (TagLib::Ogg::Vorbis::File *)m_File;
		if(tagFile->tag())
			vorbisTag = tagFile->tag()->fieldListMap();
	}

	//ape
	else if(!StrCmpI(TEXT(".mpc"), PathFindExtension(libEnt->szURL)))
	{
		TagLib::MPC::File * tagFile = (TagLib::MPC::File *)m_File;
		if(tagFile->APETag())
			apeTag = tagFile->APETag()->itemListMap();
	}

	//id3v1. id3v2, ape
	else if(!StrCmpI(TEXT(".mp3"), PathFindExtension(libEnt->szURL)))
	{
		TagLib::MPEG::File * tagFile = (TagLib::MPEG::File *)m_File;
		if(tagFile->APETag())
			apeTag = tagFile->APETag()->itemListMap();

		if(tagFile->ID3v2Tag()) 
			id3Tag = tagFile->ID3v2Tag()->frameListMap();

	}

	//id3v1, ape
	else if(!StrCmpI(TEXT(".wv"), PathFindExtension(libEnt->szURL)))
	{
		TagLib::WavPack::File * tagFile = (TagLib::WavPack::File *)m_File;
		if(tagFile->APETag())
			apeTag = tagFile->APETag()->itemListMap();
	}

	//id3v1, id3v2
	else if(!StrCmpI(TEXT(".tta"), PathFindExtension(libEnt->szURL)))
	{
		TagLib::TrueAudio::File * tagFile = (TagLib::TrueAudio::File *)m_File;
		if(tagFile->ID3v2Tag()) 
			id3Tag = tagFile->ID3v2Tag()->frameListMap();
	}

	if(!id3Tag.isEmpty())
	{
		/*
		TagLib::ID3v2::FrameListMap::Iterator it;
		
		it = mpegFile->ID3v2Tag()->frameListMap().begin();
		while(it != mpegFile->ID3v2Tag()->frameListMap().end())
		{
			it++;
		}
		*/

		// Get the list of frames for a specific frame type
		{
			TagLib::ID3v2::FrameList l = id3Tag["TRCK"];
			if(!l.isEmpty())
			{
				//std::cout << l.front()->toString() << std::endl;
				TagLib::String pszData = l.front()->toString();
				int val = pszData.find("/");
				if(val != -1)
				{
					TagLib::String trackMax = pszData.substr(val+1);
					libEnt->dwTrack[1] = trackMax.toInt();
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
		}

		{
			TagLib::ID3v2::FrameList l = id3Tag["APIC"];
			if(!l.isEmpty())
			{
				TagLib::ID3v2::AttachedPictureFrame *picframe = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(l.front());

				picframe->mimeType();
			}
		}

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
				libEnt->fReplayGain_Album_Gain  = relVol->volumeAdjustment();
				//libEnt->fReplayGain_Track_Peak  = relVol->peakVolume();
			}			
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
		*/
	}

	if(!vorbisTag.isEmpty())
	{
		if(!vorbisTag["DISCNUMBER"].isEmpty())
		{
			libEnt->dwDisc[0] = vorbisTag["DISCNUMBER"].toString().toInt();
		}
		if(!vorbisTag["TOTALTRACKS"].isEmpty())
		{
			libEnt->dwTrack[1] = vorbisTag["DISCNUMBER"].toString().toInt();
		}
		if(!vorbisTag["REPLAYGAIN_TRACK_GAIN"].isEmpty())
		{
			libEnt->fReplayGain_Track_Gain = atof(vorbisTag["REPLAYGAIN_TRACK_GAIN"].toString().toCString());
		}
		if(!vorbisTag["REPLAYGAIN_TRACK_PEAK"].isEmpty())
		{
			libEnt->fReplayGain_Track_Peak = atof(vorbisTag["REPLAYGAIN_TRACK_PEAK"].toString().toCString());
		}
		if(!vorbisTag["REPLAYGAIN_ALBUM_GAIN"].isEmpty())
		{
			libEnt->fReplayGain_Album_Gain = atof(vorbisTag["REPLAYGAIN_ALBUM_GAIN"].toString().toCString());;
		}
		if(!vorbisTag["REPLAYGAIN_ALBUM_PEAK"].isEmpty())
		{
			libEnt->fReplayGain_Album_Peak = atof(vorbisTag["REPLAYGAIN_ALBUM_PEAK"].toString().toCString());;
		}
	}

	if(!apeTag.isEmpty())
	{
		if(!apeTag["disc"].isEmpty())
		{
			libEnt->dwDisc[0] = apeTag["disc"].toString().toInt();
		}
		if(!apeTag["Track"].isEmpty())
		{
			TagLib::String pszData = apeTag["Track"].toString();
			int val = pszData.find("/");
			if(val == -1)
			{
				libEnt->dwTrack[0] = pszData.toInt();
			}
			else
			{
				TagLib::String track = pszData.substr(0,val);
				TagLib::String trackMax = pszData.substr(val+1);
				libEnt->dwTrack[0] = track.toInt();
				libEnt->dwTrack[1] = trackMax.toInt();
			}
		}
		if(!apeTag["REPLAYGAIN_TRACK_GAIN"].isEmpty())
		{
			libEnt->fReplayGain_Track_Gain = atof(apeTag["REPLAYGAIN_TRACK_GAIN"].toString().toCString());
		}
		if(!apeTag["REPLAYGAIN_TRACK_PEAK"].isEmpty())
		{
			libEnt->fReplayGain_Track_Peak = atof(apeTag["REPLAYGAIN_TRACK_PEAK"].toString().toCString());
		}
		if(!apeTag["REPLAYGAIN_ALBUM_GAIN"].isEmpty())
		{
			libEnt->fReplayGain_Album_Gain = atof(apeTag["REPLAYGAIN_ALBUM_GAIN"].toString().toCString());;
		}
		if(!apeTag["REPLAYGAIN_ALBUM_PEAK"].isEmpty())
		{
			libEnt->fReplayGain_Album_Peak = atof(apeTag["REPLAYGAIN_ALBUM_PEAK"].toString().toCString());;
		}
	}

	delete m_File;
	return true;
}

bool			CSTDInfoManager::SetInfo(LibraryEntry * libEnt)
{

	return false;
}

unsigned long	CSTDInfoManager::GetNumberOfAlbumArts(LPTSTR		szFilename)
{
	TagLib::ID3v2::FrameList id3Tag;
	if(!StrCmpI(TEXT(".mp3"), PathFindExtension(szFilename)))
	{
		TagLib::MPEG::File tagFile(szFilename);
		if(tagFile.ID3v2Tag()) 
			id3Tag = tagFile.ID3v2Tag()->frameListMap()["APIC"];
	}

	else if(!StrCmpI(TEXT(".tta"), PathFindExtension(szFilename)))
	{
		TagLib::TrueAudio::File tagFile(szFilename);
		if(tagFile.ID3v2Tag()) 
			id3Tag = tagFile.ID3v2Tag()->frameListMap()["APIC"];
	}

	else if(!StrCmpI(TEXT(".flac"), PathFindExtension(szFilename)))
	{
		TagLib::FLAC::File tagFile(szFilename);
		if(tagFile.ID3v2Tag()) 
			id3Tag = tagFile.ID3v2Tag()->frameListMap()["APIC"];
	}

	if(!id3Tag.isEmpty())
	{
		return id3Tag.size();
	}

	return 0;
}

bool			CSTDInfoManager::GetAlbumArt(	LPTSTR				szFilename, 
												unsigned long		ulImageIndex,
												LPVOID			*	pImageData,
												unsigned long	*	ulImageDataSize,
												LPTSTR				szMimeType,
												unsigned long	*	ulArtType)
{
	TagLib::ID3v2::FrameList id3Tag;
	if(!StrCmpI(TEXT(".mp3"), PathFindExtension(szFilename)))
	{
		TagLib::MPEG::File mpegFile(szFilename);
		if(mpegFile.ID3v2Tag()) 
			id3Tag = mpegFile.ID3v2Tag()->frameListMap()["APIC"];
	}

	else if(!StrCmpI(TEXT(".tta"), PathFindExtension(szFilename)))
	{
		TagLib::TrueAudio::File tagFile(szFilename);
		if(tagFile.ID3v2Tag()) 
			id3Tag = tagFile.ID3v2Tag()->frameListMap()["APIC"];
	}

	else if(!StrCmpI(TEXT(".flac"), PathFindExtension(szFilename)))
	{
		TagLib::FLAC::File tagFile(szFilename);
		if(tagFile.ID3v2Tag()) 
			id3Tag = tagFile.ID3v2Tag()->frameListMap()["APIC"];
	}

	if(!id3Tag.isEmpty())
	{
		if(!id3Tag.isEmpty() && (ulImageIndex < id3Tag.size()))
		{
			TagLib::ID3v2::AttachedPictureFrame *picframe = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(id3Tag[ulImageIndex]);

			*ulImageDataSize = picframe->picture().size();
			*pImageData = malloc(*ulImageDataSize);

			CopyMemory(*pImageData, picframe->picture().data(), *ulImageDataSize);
			StrCpy((LPWSTR)szMimeType, (LPTSTR)picframe->mimeType().toWString().c_str());

			*ulArtType = picframe->type();

			return true;
		}
	}

	return false;
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
