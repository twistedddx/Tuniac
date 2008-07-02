#pragma once
#include "iinfomanager.h"

#include "tag.h"
#include "tfile.h"
#include "fileref.h"
#include "mpeg/mpegfile.h"
#include "mpc/mpcfile.h"
#include "trueaudio/trueaudiofile.h"
#include "wavpack/wavpackfile.h"
#include "flac/flacfile.h"
#include "ogg/vorbis/vorbisfile.h"
#include "ogg/speex/speexfile.h"
#include "ogg/flac/oggflacfile.h"
#include "mp4/mp4file.h"
#include "asf/asffile.h"

#include "mpeg/id3v1/id3v1tag.h"
#include "mpeg/id3v2/id3v2tag.h"
#include "mpeg/id3v2/frames/attachedpictureframe.h"
#include "mpeg/id3v2/frames/relativevolumeframe.h"
#include "ape/apetag.h"
#include "ogg/xiphcomment.h"
#include "mp4/mp4tag.h"
#include "mp4/mp4item.h"
#include "mp4/mp4atom.h"

class CSTDInfoManager :
	public IInfoManager
{
protected:
	TagLib::FileRef fileref;
	TagLib::FLAC::File *flacfile;
	TagLib::MPEG::File *mp3file;
	TagLib::MP4::File *mp4file;
	TagLib::MPC::File *mpcfile;
	TagLib::TrueAudio::File *ttafile;
	TagLib::WavPack::File *wvfile;
	TagLib::Ogg::Vorbis::File *oggfile;
	TagLib::Ogg::FLAC::File *ogafile;
	TagLib::Ogg::Speex::File *spxfile;
	TagLib::ASF::File *wmafile;

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

	unsigned long	GetNumberOfAlbumArts(LPTSTR		szFilename);
	bool			GetAlbumArt(LPTSTR				szFilename, 
								unsigned long		ulImageIndex,
								LPVOID			*	pImageData,
								unsigned long	*	ulImageDataSize,
								LPTSTR				szMimeType,
								unsigned long	*	ulArtType);

	bool			FreeAlbumArt(LPVOID				pImageData);
};
