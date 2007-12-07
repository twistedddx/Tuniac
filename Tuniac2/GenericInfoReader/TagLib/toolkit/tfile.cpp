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

#include "tfile.h"
#include "tstring.h"
#include "tdebug.h"

#include <stdio.h>
#include <sys/stat.h>
#include <windows.h>

using namespace TagLib;

class File::FilePrivate
{
public:
  FilePrivate(const char *fileName) :
    hFile(0),
    name(fileName),
    readOnly(true),
    valid(true)
    {}

  ~FilePrivate()
  {
    free((void *)name);
  }

  HANDLE		hFile;
  const char *name;
  bool readOnly;
  bool valid;
  static const uint bufferSize = 1024;
};

////////////////////////////////////////////////////////////////////////////////
// public members
////////////////////////////////////////////////////////////////////////////////

File::File(const char *file)
{
  d = new FilePrivate(::strdup(file));

  d->readOnly = !isWritable(file);
  d->hFile = CreateFileA(	file, 
							d->readOnly ? GENERIC_READ : GENERIC_READ | GENERIC_WRITE, 
							FILE_SHARE_READ,
							NULL,
							OPEN_EXISTING,
							FILE_FLAG_RANDOM_ACCESS,
							NULL);

	if(d->hFile == INVALID_HANDLE_VALUE)
	{
		d->hFile = NULL;
		debug("Could not open file " + String(file));
	}
}

File::~File()
{
	if(d->hFile)
	{
		CloseHandle(d->hFile);
	}
	delete d;
}

const char *File::name() const
{
  return d->name;
}

ByteVector File::readBlock(ulong length)
{
  if(!d->hFile) 
  {
    debug("File::readBlock() -- Invalid File");
    return ByteVector::null;
  }

  ByteVector v(static_cast<uint>(length));
  unsigned long count;// = fread(v.data(), sizeof(char), length, d->file);

  ReadFile(d->hFile, v.data(), length, &count, NULL);
  v.resize(count);
  return v;
}

void File::writeBlock(const ByteVector &data)
{
  if(!d->hFile) 
    return;

  if(d->readOnly) {
    debug("File::writeBlock() -- attempted to write to a file that is not writable");
    return;
  }

  unsigned long bytesWritten;
	WriteFile(d->hFile, data.data(), data.size(), &bytesWritten, NULL);
}

long File::find(const ByteVector &pattern, long fromOffset, const ByteVector &before)
{
  if(!d->hFile || pattern.size() > d->bufferSize)
      return -1;

  // The position in the file that the current buffer starts at.

  long bufferOffset = fromOffset;
  ByteVector buffer;

  // These variables are used to keep track of a partial match that happens at
  // the end of a buffer.

  int previousPartialMatch = -1;
  int beforePreviousPartialMatch = -1;

  // Save the location of the current read pointer.  We will restore the
  // position using seek() before all returns.

  long originalPosition = tell();

  // Start the search at the offset.

  seek(fromOffset);

  // This loop is the crux of the find method.  There are three cases that we
  // want to account for:
  //
  // (1) The previously searched buffer contained a partial match of the search
  // pattern and we want to see if the next one starts with the remainder of
  // that pattern.
  //
  // (2) The search pattern is wholly contained within the current buffer.
  //
  // (3) The current buffer ends with a partial match of the pattern.  We will
  // note this for use in the next itteration, where we will check for the rest
  // of the pattern.
  //
  // All three of these are done in two steps.  First we check for the pattern
  // and do things appropriately if a match (or partial match) is found.  We
  // then check for "before".  The order is important because it gives priority
  // to "real" matches.

  for(buffer = readBlock(d->bufferSize); buffer.size() > 0; buffer = readBlock(d->bufferSize)) {

    // (1) previous partial match

    if(previousPartialMatch >= 0 && int(d->bufferSize) > previousPartialMatch) {
      const int patternOffset = (d->bufferSize - previousPartialMatch);
      if(buffer.containsAt(pattern, 0, patternOffset)) {
        seek(originalPosition);
        return bufferOffset - d->bufferSize + previousPartialMatch;
      }
    }

    if(!before.isNull() && beforePreviousPartialMatch >= 0 && int(d->bufferSize) > beforePreviousPartialMatch) {
      const int beforeOffset = (d->bufferSize - beforePreviousPartialMatch);
      if(buffer.containsAt(before, 0, beforeOffset)) {
        seek(originalPosition);
        return -1;
      }
    }

    // (2) pattern contained in current buffer

    long location = buffer.find(pattern);
    if(location >= 0) {
      seek(originalPosition);
      return bufferOffset + location;
    }

    if(!before.isNull() && buffer.find(before) >= 0) {
      seek(originalPosition);
      return -1;
    }

    // (3) partial match

    previousPartialMatch = buffer.endsWithPartialMatch(pattern);

    if(!before.isNull())
      beforePreviousPartialMatch = buffer.endsWithPartialMatch(before);

    bufferOffset += d->bufferSize;
  }

  // Since we hit the end of the file, reset the status before continuing.

  clear();

  seek(originalPosition);

  return -1;
}


long File::rfind(const ByteVector &pattern, long fromOffset, const ByteVector &before)
{
  if(!d->hFile || pattern.size() > d->bufferSize)
      return -1;

  // The position in the file that the current buffer starts at.

  ByteVector buffer;

  // These variables are used to keep track of a partial match that happens at
  // the end of a buffer.

  /*
  int previousPartialMatch = -1;
  int beforePreviousPartialMatch = -1;
  */

  // Save the location of the current read pointer.  We will restore the
  // position using seek() before all returns.

  long originalPosition = tell();

  // Start the search at the offset.

  long bufferOffset;
  if(fromOffset == 0) {
    seek(-1 * int(d->bufferSize), End);
    bufferOffset = tell();
  }
  else {
    seek(fromOffset + -1 * int(d->bufferSize), Beginning);
    bufferOffset = tell();    
  }

  // See the notes in find() for an explanation of this algorithm.

  for(buffer = readBlock(d->bufferSize); buffer.size() > 0; buffer = readBlock(d->bufferSize)) {

    // TODO: (1) previous partial match

    // (2) pattern contained in current buffer

    long location = buffer.rfind(pattern);
    if(location >= 0) {
      seek(originalPosition);
      return bufferOffset + location;
    }

    if(!before.isNull() && buffer.find(before) >= 0) {
      seek(originalPosition);
      return -1;
    }

    // TODO: (3) partial match

    bufferOffset -= d->bufferSize;
    seek(bufferOffset);
  }

  // Since we hit the end of the file, reset the status before continuing.

  clear();

  seek(originalPosition);

  return -1;
}

void File::insert(const ByteVector &data, ulong start, ulong replace)
{
  if(!d->hFile)
    return;

  if(data.size() == replace) {
    seek(start);
    writeBlock(data);
    return;
  }
  else if(data.size() < replace) {
      seek(start);
      writeBlock(data);
      removeBlock(start + data.size(), replace - data.size());
      return;
  }

  // Woohoo!  Faster (about 20%) than id3lib at last.  I had to get hardcore
  // and avoid TagLib's high level API for rendering just copying parts of
  // the file that don't contain tag data.
  //
  // Now I'll explain the steps in this ugliness:

  // First, make sure that we're working with a buffer that is longer than
  // the *differnce* in the tag sizes.  We want to avoid overwriting parts
  // that aren't yet in memory, so this is necessary.

  ulong bufferLength = bufferSize();
  while(data.size() - replace > bufferLength)
    bufferLength += bufferSize();

  // Set where to start the reading and writing.

  long readPosition = start + replace;
  long writePosition = start;

  ByteVector buffer;
  ByteVector aboutToOverwrite(static_cast<uint>(bufferLength));

  // This is basically a special case of the loop below.  Here we're just
  // doing the same steps as below, but since we aren't using the same buffer
  // size -- instead we're using the tag size -- this has to be handled as a
  // special case.  We're also using File::writeBlock() just for the tag.
  // That's a bit slower than using char *'s so, we're only doing it here.

  seek(readPosition);
  unsigned long bytesRead;
  ReadFile(d->hFile, aboutToOverwrite.data(), bufferLength, &bytesRead, NULL);

  readPosition += bufferLength;

  seek(writePosition);
  writeBlock(data);
  writePosition += data.size();

  buffer = aboutToOverwrite;

  // Ok, here's the main loop.  We want to loop until the read fails, which
  // means that we hit the end of the file.

  while(bytesRead != 0) {

    // Seek to the current read position and read the data that we're about
    // to overwrite.  Appropriately increment the readPosition.

    seek(readPosition);
	ReadFile(d->hFile, aboutToOverwrite.data(), bufferLength, &bytesRead, NULL);

    aboutToOverwrite.resize(bytesRead);
    readPosition += bufferLength;

    // Check to see if we just read the last block.  We need to call clear()
    // if we did so that the last write succeeds.

    if(ulong(bytesRead) < bufferLength)
      clear();

    // Seek to the write position and write our buffer.  Increment the
    // writePosition.

    seek(writePosition);
	unsigned long bytesWritten;
	WriteFile(d->hFile, buffer.data(), bufferLength, &bytesWritten, NULL);
    writePosition += bufferLength;

    // Make the current buffer the data that we read in the beginning.

    buffer = aboutToOverwrite;

    // Again, we need this for the last write.  We don't want to write garbage
    // at the end of our file, so we need to set the buffer size to the amount
    // that we actually read.

    bufferLength = bytesRead;
  }
}

void File::removeBlock(ulong start, ulong length)
{
  if(!d->hFile)
    return;

  ulong bufferLength = bufferSize();

  long readPosition = start + length;
  long writePosition = start;

  ByteVector buffer(static_cast<uint>(bufferLength));

  ulong bytesRead = true;

  while(bytesRead != 0) {
    seek(readPosition);
	ReadFile(d->hFile, buffer.data(), bufferLength, &bytesRead, NULL);
    buffer.resize(bytesRead);
    readPosition += bytesRead;

    // Check to see if we just read the last block.  We need to call clear()
    // if we did so that the last write succeeds.

    if(bytesRead < bufferLength)
      clear();

    seek(writePosition);
	unsigned long bytesWritten;
	WriteFile(d->hFile, buffer.data(), bytesRead, &bytesWritten, NULL);
    writePosition += bytesRead;
  }
  truncate(writePosition);
}

bool File::readOnly() const
{
  return d->readOnly;
}

bool File::isReadable(const char *file)
{
	return true;
//  return access(file, R_OK) == 0;
}

bool File::isOpen() const
{
  return d->hFile != NULL;
}

bool File::isValid() const
{
  return d->hFile && d->valid;
}

void File::seek(long offset, Position p)
{
  if(!d->hFile) {
    debug("File::seek() -- trying to seek in a file that isn't opened.");
    return;
  }

	unsigned long movemode;


  switch(p) 
  {
  case Beginning:
    movemode = FILE_BEGIN;
    break;
  case Current:
    movemode = FILE_CURRENT;
    break;
  case End:
    movemode = FILE_END;
    break;
  }

  SetFilePointer(d->hFile, offset, NULL, movemode);

}

void File::clear()
{
}

long File::tell() const
{
  return SetFilePointer(d->hFile, 0, NULL, FILE_CURRENT);  //ftell(d->file);
}

long File::length()
{
  if(!d->hFile)
    return 0;	  

	long len = GetFileSize(d->hFile, NULL);
  return len;
}

bool File::isWritable(const char *file)
{
	return ((GetFileAttributesA(file) & FILE_ATTRIBUTE_READONLY) == 0);
}

////////////////////////////////////////////////////////////////////////////////
// protected members
////////////////////////////////////////////////////////////////////////////////

void File::setValid(bool valid)
{
  d->valid = valid;
}

void File::truncate(long length)
{
	unsigned long oldPos = SetFilePointer(d->hFile, 0, NULL, FILE_CURRENT);
	SetFilePointer(d->hFile, length, NULL, FILE_BEGIN);
	SetEndOfFile(d->hFile);
	SetFilePointer(d->hFile, oldPos, NULL, FILE_BEGIN);
}

TagLib::uint File::bufferSize()
{
  return FilePrivate::bufferSize;
}
