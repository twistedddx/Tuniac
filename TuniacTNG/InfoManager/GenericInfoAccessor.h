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
#include "mp4/mp4file.h"

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

	TagLib::File			*		m_File;

	TagLib::FLAC::File		*	m_flacFile;
	TagLib::MP4::File		*	m_mp4File;
	TagLib::MPC::File		*	m_mpcFile;
	TagLib::MPEG::File		*	m_mpegFile;
	TagLib::Ogg::Vorbis::File *	m_oggFile;
	TagLib::TrueAudio::File	*	m_ttaFile;
	TagLib::WavPack::File	*	m_wvFile;

	TagLib::Ogg::FieldListMap	vorbisTag;
	TagLib::APE::ItemListMap	apeTag;
	TagLib::ID3v2::FrameListMap	id3Tag;
	TagLib::MP4::ItemListMap	mp4Tag;


	TagLib::AudioProperties		*	m_pProperties;
	

public:
	CGenericInfoAccessor(void);
public:
	~CGenericInfoAccessor(void);

public:
	bool	Open(wchar_t * filename);

public:
	void	Destroy();

	bool			ReadMetaData(MediaItem * pItem);
	bool			WriteMetaData(MediaItem * pItem, unsigned long * pPropertiesToStore, unsigned long ulNumProperties);



	bool	GetAlbumArtCount(__int64 * pullCount);
	bool	GetAlbumArtInformation(__int64 ullArtIndex, unsigned __int64 * uqArtDataSize, wchar_t * pwcsMimeType, unsigned long ulMimeTypeBufferSize);
	bool	GetAlbumArtData(unsigned long ulIndex, void * pArtData, unsigned long ullArtDataSize);

	bool	SetAlbumArt(unsigned long ulIndex, void * pArtData, unsigned __int64 uqArtSize, wchar_t * pwcsMimeType);
};
