/***************************************************************************
    copyright            : (C) 2003-2004 by Allan Sandfeld Jensen
    email                : kde@carewolf.org
 ***************************************************************************/

/***************************************************************************
 *   This library is free software; you can redistribute it and/or modify  *
 *   it  under the terms of the GNU Lesser General Public License version  *
 *   2.1 as published by the Free Software Foundation.                     *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful, but   *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 ***************************************************************************/

#include <tbytevector.h>
#include <tstring.h>
#include <tlist.h>
#include <tdebug.h>

#include <id3v2header.h>
#include <id3v2tag.h>
#include <id3v1tag.h>

#include "flacfile.h"
#include "flactag.h"

using namespace TagLib;

namespace TagLib {
namespace FLAC {
  enum BLOCK_TYPE { STREAMINFO = 0, PADDING, APPLICATION, SEEKTABLE, VORBISCOMMENT, CUESHEET };
}}

class FLAC::File::FilePrivate
{
public:
  FilePrivate() :
    ID3v2FrameFactory(ID3v2::FrameFactory::instance()),
    ID3v2Tag(0),
    ID3v2Location(-1),
    ID3v2OriginalSize(0),
    ID3v1Tag(0),
    ID3v1Location(-1),
    comment(0),
    properties(0),
    flacStart(0),
    streamStart(0),
    streamLength(0),
    scanned(false),
    hasXiphComment(false),
    hasID3v2(false),
    hasID3v1(false) {}

  ~FilePrivate()
  {
    delete ID3v2Tag;
    delete ID3v1Tag;
    delete comment;
    delete properties;
  }

  const ID3v2::FrameFactory *ID3v2FrameFactory;
  ID3v2::Tag *ID3v2Tag;
  long ID3v2Location;
  uint ID3v2OriginalSize;

  ID3v1::Tag *ID3v1Tag;
  long ID3v1Location;

  Ogg::XiphComment *comment;

  FLAC::Tag *tag;

  Properties *properties;
//  Map<BLOCK_TYPE, ByteVector> metaData;
  ByteVector streamInfoData;
  ByteVector xiphCommentData;
  long flacStart;
  long streamStart;
  long streamLength;
  bool scanned;

  bool hasXiphComment;
  bool hasID3v2;
  bool hasID3v1;
};

////////////////////////////////////////////////////////////////////////////////
// public members
////////////////////////////////////////////////////////////////////////////////

FLAC::File::File(const char *file, bool readProperties,
                 Properties::ReadStyle propertiesStyle) :
  TagLib::File(file)
{
  d = new FilePrivate;
  read(readProperties, propertiesStyle);
}

FLAC::File::File(const char *file, ID3v2::FrameFactory *frameFactory,
                 bool readProperties, Properties::ReadStyle propertiesStyle) :
  TagLib::File(file)
{
  d = new FilePrivate;
  d->ID3v2FrameFactory = frameFactory;
  read(readProperties, propertiesStyle);
}

FLAC::File::~File()
{
  delete d;
}

TagLib::Tag *FLAC::File::tag() const
{
  return d->tag;
}

FLAC::Properties *FLAC::File::audioProperties() const
{
  return d->properties;
}


bool FLAC::File::save()
{
  if(readOnly()) {
    debug("FLAC::File::save() - Cannot save to a read only file.");
    return false;
  }

  // Create new vorbis comments

  if(!d->comment) {
    d->comment = new Ogg::XiphComment;
    if(d->tag)
      Tag::duplicate(d->tag, d->comment, true);
  }

  d->xiphCommentData = d->comment->render(false);

  ByteVector v = ByteVector::fromUInt(d->xiphCommentData.size());

  // Set the type of the comment to be a Xiph / Vorbis comment
  // (See scan() for comments on header-format)
  v[0] = 4;
  v.append(d->xiphCommentData);


   // If file already have comment => find and update it
   //                       if not => insert one
   // TODO: Search for padding and use that

  if(d->hasXiphComment) {
    long nextPageOffset = d->flacStart;
    seek(nextPageOffset);
    ByteVector header = readBlock(4);
    uint length = header.mid(1, 3).toUInt();

    nextPageOffset += length + 4;

    // Search through the remaining metadata

    char blockType = header[0] & 0x7f;
    bool lastBlock = header[0] & 0x80;

    while(!lastBlock) {
      seek(nextPageOffset);

      header = readBlock(4);
      blockType = header[0] & 0x7f;
      lastBlock = header[0] & 0x80;
      length = header.mid(1, 3).toUInt();

      // Type is vorbiscomment
      if(blockType == 4) {
        v[0] = header[0];
        insert(v, nextPageOffset, length + 4);
        break;
      }

      nextPageOffset += length + 4;
    }
  }
  else {
    long nextPageOffset = d->flacStart;

    seek(nextPageOffset);

    ByteVector header = readBlock(4);
    // char blockType = header[0] & 0x7f;
    bool lastBlock = header[0] & 0x80;
    uint length = header.mid(1, 3).toUInt();

    // If last block was last, make this one last

    if(lastBlock) {

      // Copy the bottom seven bits into the new value

      ByteVector h(static_cast<char>(header[0] & 0x7F));
      insert(h, nextPageOffset, 1);

      // Set the last bit
      v[0] |= 0x80;
    }

    insert(v, nextPageOffset + length + 4, 0);
    d->hasXiphComment = true;
  }

  // Update ID3 tags

  if(d->ID3v2Tag) {
    if(d->hasID3v2)
      insert(d->ID3v2Tag->render(), d->ID3v2Location, d->ID3v2OriginalSize);
    else
      insert(d->ID3v2Tag->render(), 0, 0);
  }

  if(d->ID3v1Tag) {
    if(d->hasID3v1)
      seek(-128, End);
    else
      seek(0, End);
    writeBlock(d->ID3v1Tag->render());
  }

  return true;
}

ID3v2::Tag *FLAC::File::ID3v2Tag(bool create)
{
  if(!create || d->ID3v2Tag)
    return d->ID3v2Tag;

  // no ID3v2 tag exists and we've been asked to create one

  d->ID3v2Tag = new ID3v2::Tag;
  return d->ID3v2Tag;
}

ID3v1::Tag *FLAC::File::ID3v1Tag(bool create)
{
  if(!create || d->ID3v1Tag)
    return d->ID3v1Tag;

  // no ID3v1 tag exists and we've been asked to create one

  d->ID3v1Tag = new ID3v1::Tag;
  return d->ID3v1Tag;
}

Ogg::XiphComment *FLAC::File::xiphComment(bool create)
{
  if(!create || d->comment)
    return d->comment;

  // no XiphComment exists and we've been asked to create one

  d->comment = new Ogg::XiphComment;
  return d->comment;
}

void FLAC::File::setID3v2FrameFactory(const ID3v2::FrameFactory *factory)
{
  d->ID3v2FrameFactory = factory;
}


////////////////////////////////////////////////////////////////////////////////
// private members
////////////////////////////////////////////////////////////////////////////////

void FLAC::File::read(bool readProperties, Properties::ReadStyle propertiesStyle)
{
  // Look for an ID3v2 tag

  d->ID3v2Location = findID3v2();

  if(d->ID3v2Location >= 0) {

    d->ID3v2Tag = new ID3v2::Tag(this, d->ID3v2Location, d->ID3v2FrameFactory);

    d->ID3v2OriginalSize = d->ID3v2Tag->header()->completeTagSize();

    if(d->ID3v2Tag->header()->tagSize() <= 0) {
      delete d->ID3v2Tag;
      d->ID3v2Tag = 0;
    }
    else
      d->hasID3v2 = true;
  }

  // Look for an ID3v1 tag

  d->ID3v1Location = findID3v1();

  if(d->ID3v1Location >= 0) {
    d->ID3v1Tag = new ID3v1::Tag(this, d->ID3v1Location);
    d->hasID3v1 = true;
  }

  // Look for FLAC metadata, including vorbis comments

  scan();

  if (!isValid()) return;

  if(d->hasXiphComment)
    d->comment = new Ogg::XiphComment(xiphCommentData());

  if(d->hasXiphComment || d->hasID3v2 || d->hasID3v1)
    d->tag = new FLAC::Tag(d->comment, d->ID3v2Tag, d->ID3v1Tag);
  else
    d->tag = new FLAC::Tag(new Ogg::XiphComment);

  if(readProperties)
    d->properties = new Properties(streamInfoData(), streamLength(), propertiesStyle);
}

ByteVector FLAC::File::streamInfoData()
{
  if (isValid())
    return d->streamInfoData;
//    return d->metaData[STREAMINFO];
  else
    return ByteVector();
}

ByteVector FLAC::File::xiphCommentData()
{
  if (isValid() && d->hasXiphComment)
    return d->xiphCommentData;
//    return d->metaData[VORBISCOMMENT];
  else
    return ByteVector();
}

long FLAC::File::streamLength()
{
  return d->streamLength;
}

void FLAC::File::scan()
{
  // Scan the metadata pages

  if(d->scanned)
    return;

  if(!isValid())
    return;

  long nextPageOffset;
  long fileSize = length();

  if (d->hasID3v2)
    nextPageOffset = find("fLaC", d->ID3v2Location+d->ID3v2OriginalSize);
  else
    nextPageOffset = find("fLaC");

  if(nextPageOffset < 0) {
    debug("FLAC::File::scan() -- FLAC stream not found");
    setValid(false);
    return;
  }

  nextPageOffset += 4;
  d->flacStart = nextPageOffset;

  seek(nextPageOffset);

  ByteVector header = readBlock(4);

  // Header format (from spec):
  // <1> Last-metadata-block flag
  // <7> BLOCK_TYPE
  //	0 : STREAMINFO
  //    1 : PADDING
  //    ..
  //    4 : VORBIS_COMMENT
  //    ..
  // <24> Length of metadata to follow

  char blockType = header[0] & 0x7f;
  bool lastBlock = header[0] & 0x80;
  uint length = header.mid(1, 3).toUInt();

  // First block should be the stream_info metadata
  if(blockType != 0) {
    debug("FLAC::File::scan() -- invalid FLAC stream");
    setValid(false);
    return;
  }
  d->streamInfoData = readBlock(length);
//  d->metadata.insert(STREAMINFO, readBlock(length));
  nextPageOffset += length + 4;

  // Search through the remaining metadata

  while(!lastBlock) {
    header = readBlock(4);
    blockType = header[0] & 0x7f;
    lastBlock = header[0] & 0x80;
    length = header.mid(1, 3).toUInt();

    if(blockType == 1) {
      // debug("FLAC::File::scan() -- Padding found");
    }
    // Found the vorbis-comment
    else if(blockType == 4) {
      d->xiphCommentData = readBlock(length);
//      d->metadata.insert(VORBISCOMMENT, readBlock(length));
      d->hasXiphComment = true;
    }

    nextPageOffset += length + 4;
    if (nextPageOffset >= fileSize) {
      debug("FLAC::File::scan() -- FLAC stream corrupted");
      setValid(false);
      return;
    }
    seek(nextPageOffset);
  }

  // End of metadata, now comes the datastream
  d->streamStart = nextPageOffset;
  d->streamLength = File::length() - d->streamStart;
  if (d->hasID3v1)
    d->streamLength -= 128;

  d->scanned = true;
}

long FLAC::File::findID3v1()
{
  if(!isValid())
    return -1;

  seek(-128, End);
  long p = tell();

  if(readBlock(3) == ID3v1::Tag::fileIdentifier())
    return p;

  return -1;
}

long FLAC::File::findID3v2()
{
  if(!isValid())
    return -1;

  seek(0);

  if(readBlock(3) == ID3v2::Header::fileIdentifier())
    return 0;

  return -1;
}
