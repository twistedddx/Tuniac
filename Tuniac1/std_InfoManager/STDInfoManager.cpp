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
#include "mpeg/id3v2/id3v2tag.h"
#include "mpeg/id3v2/frames/attachedpictureframe.h"
#include "mpeg/id3v2/frames/relativevolumeframe.h"
#include "ape/apetag.h"




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

//mpc,wv,tta? generic ape tagged? ape, ofr, tak

	if(!StrCmpI(TEXT(".mp3"), PathFindExtension(libEnt->szURL)))
	{
		TagLib::MPEG::File * mpegFile = (TagLib::MPEG::File *)m_File;
		if(mpegFile->APETag())
		{
			TagLib::APE::ItemListMap apemap = mpegFile->APETag()->itemListMap();
			if(!apemap["REPLAYGAIN_TRACK_GAIN"].isEmpty())
			{
				int x=0;
				//libEnt->fRPTG = atof(apemap["REPLAYGAIN_TRACK_GAIN"].toString().toCString());
			}
			if(!apemap["REPLAYGAIN_ALBUM_GAIN"].isEmpty())
			{
				int y=0;
				//libEnt->fRPAG = atof(apemap["REPLAYGAIN_ALBUM_GAIN"].toString().toCString());
			}
		}
		if(mpegFile->ID3v2Tag()) 
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
				TagLib::ID3v2::FrameList l = mpegFile->ID3v2Tag()->frameListMap()["TRCK"];
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
				TagLib::ID3v2::FrameList l = mpegFile->ID3v2Tag()->frameListMap()["TPOS"];
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
				TagLib::ID3v2::FrameList l = mpegFile->ID3v2Tag()->frameListMap()["APIC"];
				if(!l.isEmpty())
				{
					TagLib::ID3v2::AttachedPictureFrame *picframe = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(l.front());

					picframe->mimeType();
				}
			}

			{
				TagLib::ID3v2::FrameList l = mpegFile->ID3v2Tag()->frameListMap()["RGAD"];
				if(!l.isEmpty())
				{
					_ASSERT(0);
				}			
			}
			{
				TagLib::ID3v2::FrameList l = mpegFile->ID3v2Tag()->frameListMap()["RVA2"];
				if(!l.isEmpty())
				{
					TagLib::ID3v2::RelativeVolumeFrame * relVol = static_cast<TagLib::ID3v2::RelativeVolumeFrame *>(l.front());
//					_ASSERT(0);

					int x=0;
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
	if(!StrCmpI(TEXT(".mp3"), PathFindExtension(szFilename)))
	{
		TagLib::MPEG::File mpegFile(szFilename);

		if(mpegFile.ID3v2Tag()) 
		{
			TagLib::ID3v2::FrameList l = mpegFile.ID3v2Tag()->frameListMap()["APIC"];

			return l.size();
		}
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
	if(!StrCmpI(TEXT(".mp3"), PathFindExtension(szFilename)))
	{
		TagLib::MPEG::File mpegFile(szFilename);

		if(mpegFile.ID3v2Tag()) 
		{
			TagLib::ID3v2::FrameList l = mpegFile.ID3v2Tag()->frameListMap()["APIC"];

			if(!l.isEmpty() && (ulImageIndex < l.size()))
			{
				TagLib::ID3v2::AttachedPictureFrame *picframe = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(l[ulImageIndex]);

				*ulImageDataSize = picframe->picture().size();
				*pImageData = malloc(*ulImageDataSize);

				CopyMemory(*pImageData, picframe->picture().data(), *ulImageDataSize);
				StrCpy((LPWSTR)szMimeType, (LPTSTR)picframe->mimeType().toWString().c_str());

				*ulArtType = picframe->type();

				return true;
			}
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
