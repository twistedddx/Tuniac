#pragma once
#include <strsafe.h>

#include "iinfomanager.h"

#include "tag.h"
#include "tfile.h"
#include "fileRef.h"

#include "apeFile.h"
#include "apetag.h"

#include "asffile.h"

#include "dsffile.h"

#include "ebmlfile.h"

#include "flacFile.h"
#include "flacpicture.h"

#include "itFile.h"

#include "modFile.h"

#include "mp4File.h"
#include "mp4tag.h"
#include "mp4item.h"
#include "mp4atom.h"

#include "mpcFile.h"

#include "mpegfile.h"
#include "id3v1tag.h"
#include "id3v2tag.h"
#include "attachedpictureframe.h"
#include "relativevolumeframe.h"

#include "xiphcomment.h"
#include "oggflacfile.h"
#include "opusfile.h"
#include "speexfile.h"
#include "vorbisfile.h"

#include "aifffile.h"
#include "wavFile.h"

#include "s3mFile.h"

#include "trueaudiofile.h"

#include "wavpackfile.h"

#include "xmFile.h"


class CSTDInfoManager :
	public IInfoManager
{
protected:
	TagLib::FileRef fileRef;
	TagLib::FLAC::File *flacFile;
	TagLib::MPEG::File *mp3File;
	TagLib::MP4::File *mp4File;
	TagLib::MPC::File *mpcFile;
	TagLib::TrueAudio::File *ttaFile;
	TagLib::WavPack::File *wvFile;
	TagLib::Ogg::Vorbis::File *oggFile;
	TagLib::Ogg::FLAC::File *ogaFile;
	TagLib::Ogg::Speex::File *spxFile;
	TagLib::Ogg::Opus::File *opusFile;
	TagLib::ASF::File *wmaFile;
	TagLib::RIFF::AIFF::File *aiffFile;
	TagLib::RIFF::WAV::File *wavFile;
	TagLib::APE::File *apeFile;
	TagLib::IT::File *itFile;
	TagLib::Mod::File *modFile;
	TagLib::S3M::File *s3mFile;
	TagLib::XM::File *xmFile;
	TagLib::DSF::File *dsfFile;
	TagLib::EBML::File *ebmlFile;

public:
	CSTDInfoManager(void);
	~CSTDInfoManager(void);

public:
	void			Destroy(void);

	unsigned long	GetNumExtensions(void);
	LPTSTR			SupportedExtension(unsigned long ulExtentionNum);

	bool			CanHandle(LPTSTR szSource);
	bool			GetInfo(LibraryEntry * libEnt);
	bool			SetInfo(LibraryEntry * libEnt);

	unsigned long	GetNumberOfAlbumArts(void);
	bool			GetAlbumArt(unsigned long		ulImageIndex,
								LPVOID			*	pImageData,
								unsigned long	*	ulImageDataSize,
								LPTSTR				szMimeType,
								unsigned long	*	ulArtType);

	bool			FreeAlbumArt(LPVOID				pImageData);
};
