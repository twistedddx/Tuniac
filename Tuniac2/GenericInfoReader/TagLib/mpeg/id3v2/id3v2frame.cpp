/***************************************************************************
    copyright            : (C) 2002, 2003 by Scott Wheeler
    email                : wheeler@kde.org
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

//#include <config.h>

#include <bitset>

#if HAVE_ZLIB
#include <zlib.h>
#endif

#include <tdebug.h>

#include "id3v2frame.h"
#include "id3v2synchdata.h"

using namespace TagLib;
using namespace ID3v2;

class Frame::FramePrivate
{
public:
  FramePrivate() :
    header(0)
    {}

  ~FramePrivate()
  {
    delete header;
  }

  Frame::Header *header;
};

////////////////////////////////////////////////////////////////////////////////
// static methods
////////////////////////////////////////////////////////////////////////////////

TagLib::uint Frame::headerSize()
{
  return Header::size();
}

TagLib::uint Frame::headerSize(uint version)
{
  return Header::size(version);
}

ByteVector Frame::textDelimiter(String::Type t)
{
  ByteVector d = char(0);
  if(t == String::UTF16 || t == String::UTF16BE)
    d.append(char(0));
  return d;
}

////////////////////////////////////////////////////////////////////////////////
// public members
////////////////////////////////////////////////////////////////////////////////

Frame::~Frame()
{
  delete d;
}

ByteVector Frame::frameID() const
{
  if(d->header)
    return d->header->frameID();
  else
    return ByteVector::null;
}

TagLib::uint Frame::size() const
{
  if(d->header)
    return d->header->frameSize();
  else
    return 0;
}

void Frame::setData(const ByteVector &data)
{
  parse(data);
}

void Frame::setText(const String &)
{

}

ByteVector Frame::render() const
{
  ByteVector fieldData = renderFields();
  d->header->setFrameSize(fieldData.size());
  ByteVector headerData = d->header->render();

  return headerData + fieldData;
}

////////////////////////////////////////////////////////////////////////////////
// protected members
////////////////////////////////////////////////////////////////////////////////

Frame::Frame(const ByteVector &data)
{
  d = new FramePrivate;
  d->header = new Header(data);
}

Frame::Frame(Header *h)
{
  d = new FramePrivate;
  d->header = h;
}

Frame::Header *Frame::header() const
{
  return d->header;
}

void Frame::setHeader(Header *h, bool deleteCurrent)
{
  if(deleteCurrent)
    delete d->header;

  d->header = h;
}

void Frame::parse(const ByteVector &data)
{
  if(d->header)
    d->header->setData(data);
  else
    d->header = new Header(data);

  parseFields(fieldData(data));
}

ByteVector Frame::fieldData(const ByteVector &frameData) const
{
  uint headerSize = Header::size(d->header->version());

  uint frameDataOffset = headerSize;
  uint frameDataLength = size();

  if(d->header->compression() || d->header->dataLengthIndicator()) {
    frameDataLength = frameData.mid(headerSize, 4).toUInt();
    frameDataOffset += 4;
  }

#if HAVE_ZLIB
  if(d->header->compression()) {
    ByteVector data(frameDataLength);
    uLongf uLongTmp = frameDataLength;
    ::uncompress((Bytef *) data.data(),
                 (uLongf *) &uLongTmp,
                 (Bytef *) frameData.data() + frameDataOffset,
                 size());
    return data;
  }
  else
#endif
    return frameData.mid(frameDataOffset, frameDataLength);
}

////////////////////////////////////////////////////////////////////////////////
// Frame::Header class
////////////////////////////////////////////////////////////////////////////////

class Frame::Header::HeaderPrivate
{
public:
  HeaderPrivate() :
    frameSize(0),
    version(4),
    tagAlterPreservation(false),
    fileAlterPreservation(false),
    readOnly(false),
    groupingIdentity(false),
    compression(false),
    encryption(false),
    unsyncronisation(false),
    dataLengthIndicator(false)
    {}

  ByteVector frameID;
  uint frameSize;
  uint version;

  // flags

  bool tagAlterPreservation;
  bool fileAlterPreservation;
  bool readOnly;
  bool groupingIdentity;
  bool compression;
  bool encryption;
  bool unsyncronisation;
  bool dataLengthIndicator;
};

////////////////////////////////////////////////////////////////////////////////
// static members (Frame::Header)
////////////////////////////////////////////////////////////////////////////////

TagLib::uint Frame::Header::size()
{
  return size(4);
}

TagLib::uint Frame::Header::size(uint version)
{
  switch(version) {
  case 0:
  case 1:
  case 2:
    return 6;
  case 3:
  case 4:
  default:
    return 10;
  }
}

////////////////////////////////////////////////////////////////////////////////
// public members (Frame::Header)
////////////////////////////////////////////////////////////////////////////////

Frame::Header::Header(const ByteVector &data, bool synchSafeInts)
{
  d = new HeaderPrivate;
  setData(data, synchSafeInts);
}

Frame::Header::Header(const ByteVector &data, uint version)
{
  d = new HeaderPrivate;
  setData(data, version);
}

Frame::Header::~Header()
{
  delete d;
}

void Frame::Header::setData(const ByteVector &data, bool synchSafeInts)
{
  setData(data, uint(synchSafeInts ? 4 : 3));
}

void Frame::Header::setData(const ByteVector &data, uint version)
{
  d->version = version;

  switch(version) {
  case 0:
  case 1:
  case 2:
  {
    // ID3v2.2

    if(data.size() < 3) {
      debug("You must at least specify a frame ID.");
      return;
    }

    // Set the frame ID -- the first three bytes

    d->frameID = data.mid(0, 3);

    // If the full header information was not passed in, do not continue to the
    // steps to parse the frame size and flags.

    if(data.size() < 6) {
      d->frameSize = 0;
      return;
    }

    d->frameSize = data.mid(3, 3).toUInt();

    break;
  }
  case 3:
  {
    // ID3v2.3

    if(data.size() < 4) {
      debug("You must at least specify a frame ID.");
      return;
    }

    // Set the frame ID -- the first four bytes

    d->frameID = data.mid(0, 4);

    // If the full header information was not passed in, do not continue to the
    // steps to parse the frame size and flags.

    if(data.size() < 10) {
      d->frameSize = 0;
      return;
    }

    // Set the size -- the frame size is the four bytes starting at byte four in
    // the frame header (structure 4)

    d->frameSize = data.mid(4, 4).toUInt();

    { // read the first byte of flags
      std::bitset<8> flags(data[8]);
      d->tagAlterPreservation  = flags[7]; // (structure 3.3.1.a)
      d->fileAlterPreservation = flags[6]; // (structure 3.3.1.b)
      d->readOnly              = flags[5]; // (structure 3.3.1.c)
    }

    { // read the second byte of flags
      std::bitset<8> flags(data[9]);
      d->compression         = flags[7]; // (structure 3.3.1.i)
      d->encryption          = flags[6]; // (structure 3.3.1.j)
      d->groupingIdentity    = flags[5]; // (structure 3.3.1.k)
    }
    break;
  }
  case 4:
  default:
  {
    // ID3v2.4

    if(data.size() < 4) {
      debug("You must at least specify a frame ID.");
      return;
    }

    // Set the frame ID -- the first four bytes

    d->frameID = data.mid(0, 4);

    // If the full header information was not passed in, do not continue to the
    // steps to parse the frame size and flags.

    if(data.size() < 10) {
      d->frameSize = 0;
      return;
    }

    // Set the size -- the frame size is the four bytes starting at byte four in
    // the frame header (structure 4)

    d->frameSize = SynchData::toUInt(data.mid(4, 4));

    { // read the first byte of flags
      std::bitset<8> flags(data[8]);
      d->tagAlterPreservation  = flags[6]; // (structure 4.1.1.a)
      d->fileAlterPreservation = flags[5]; // (structure 4.1.1.b)
      d->readOnly              = flags[4]; // (structure 4.1.1.c)
    }

    { // read the second byte of flags
      std::bitset<8> flags(data[9]);
      d->groupingIdentity    = flags[6]; // (structure 4.1.2.h)
      d->compression         = flags[3]; // (structure 4.1.2.k)
      d->encryption          = flags[2]; // (structure 4.1.2.m)
      d->unsyncronisation    = flags[1]; // (structure 4.1.2.n)
      d->dataLengthIndicator = flags[0]; // (structure 4.1.2.p)
    }
    break;
  }
  }
}

ByteVector Frame::Header::frameID() const
{
  return d->frameID;
}

void Frame::Header::setFrameID(const ByteVector &id)
{
  d->frameID = id.mid(0, 4);
}

TagLib::uint Frame::Header::frameSize() const
{
  return d->frameSize;
}

void Frame::Header::setFrameSize(uint size)
{
  d->frameSize = size;
}

TagLib::uint Frame::Header::version() const
{
  return d->version;
}

bool Frame::Header::tagAlterPreservation() const
{
  return d->tagAlterPreservation;
}

bool Frame::Header::fileAlterPreservation() const
{
  return d->fileAlterPreservation;
}

bool Frame::Header::readOnly() const
{
  return d->readOnly;
}

bool Frame::Header::groupingIdentity() const
{
  return d->groupingIdentity;
}

bool Frame::Header::compression() const
{
  return d->compression;
}

bool Frame::Header::encryption() const
{
  return d->encryption;
}

bool Frame::Header::unsycronisation() const
{
  return d->unsyncronisation;
}

bool Frame::Header::dataLengthIndicator() const
{
  return d->dataLengthIndicator;
}

ByteVector Frame::Header::render() const
{
  ByteVector flags(2, char(0)); // just blank for the moment

  ByteVector v = d->frameID + SynchData::fromUInt(d->frameSize) + flags;

  return v;
}

bool Frame::Header::frameAlterPreservation() const // deprecated
{
  return fileAlterPreservation();
}
