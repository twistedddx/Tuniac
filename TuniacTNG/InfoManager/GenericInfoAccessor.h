#pragma once
#include "iinfohandler.h"

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
#include "riff/aiff/aifffile.h"
#include "riff/wav/wavfile.h"

#include "mpeg/id3v1/id3v1tag.h"
#include "mpeg/id3v2/id3v2tag.h"
#include "mpeg/id3v2/frames/attachedpictureframe.h"
#include "mpeg/id3v2/frames/relativevolumeframe.h"

#include "ape/apetag.h"

#include "ogg/xiphcomment.h"

#include "mp4/mp4tag.h"
#include "mp4/mp4item.h"
#include "mp4/mp4atom.h"

class CGenericInfoAccessor :
	public IInfoAccessor
{
protected:

	TagLib::FileRef					fileref;
	TagLib::FLAC::File				*flacfile;
	TagLib::MPEG::File				*mp3file;
	TagLib::MP4::File				*mp4file;
	TagLib::MPC::File				*mpcfile;
	TagLib::TrueAudio::File			*ttafile;
	TagLib::WavPack::File			*wvfile;
	TagLib::Ogg::Vorbis::File		*oggfile;
	TagLib::Ogg::FLAC::File			*ogafile;
	TagLib::Ogg::Speex::File		*spxfile;
	TagLib::ASF::File				*wmafile;
	TagLib::RIFF::AIFF::File		*aiffile;
	TagLib::RIFF::WAV::File			*wavfile;

	TagLib::AudioProperties		*	m_pProperties;
	

public:
	CGenericInfoAccessor(void);
public:
	~CGenericInfoAccessor(void);

public:
	bool	Open(wchar_t * filename);

public:
	void	Destroy();

	virtual bool			ReadMetaData(MediaItem & pItem);
	virtual bool			WriteMetaData(MediaItem & pItem, unsigned long * pPropertiesToStore, unsigned long ulNumProperties);

	virtual bool	GetAlbumArtCount(__int64 * pullCount);
	virtual bool	GetAlbumArtInformation(__int64 ullArtIndex, unsigned __int64 * uqArtDataSize, wchar_t * pwcsMimeType, unsigned long ulMimeTypeBufferSize);
	virtual bool	GetAlbumArtData(unsigned long ulIndex, void * pArtData, unsigned long ullArtDataSize);

	virtual bool	SetAlbumArt(unsigned long ulIndex, void * pArtData, unsigned __int64 uqArtSize, wchar_t * pwcsMimeType);
};
