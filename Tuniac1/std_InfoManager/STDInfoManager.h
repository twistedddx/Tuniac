#pragma once
#include <strsafe.h>

#include "iinfomanager.h"

#include "tag.h"
#include "tfile.h"
#include "fileRef.h"
#include "mpegfile.h"
#include "mpcFile.h"
#include "trueaudiofile.h"
#include "wavpackfile.h"
#include "flacFile.h"
#include "vorbisfile.h"
#include "speexfile.h"
#include "opusfile.h"
#include "oggflacfile.h"
#include "mp4File.h"
#include "asffile.h"
#include "aifffile.h"
#include "wavFile.h"
#include "apeFile.h"
#include "itFile.h"
#include "modFile.h"
#include "s3mFile.h"
#include "xmFile.h"

#include "id3v1tag.h"
#include "id3v2tag.h"
#include "attachedpictureframe.h"
#include "relativevolumeframe.h"

#include "apetag.h"

#include "xiphcomment.h"
#include "flacpicture.h"

#include "mp4tag.h"
#include "mp4item.h"
#include "mp4atom.h"

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
