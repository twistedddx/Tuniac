/***************************************************************************
    copyright            : (C) 2006 by Lukáš Lalinský
    email                : lalinsky@gmail.com

    copyright            : (C) 2004 by Allan Sandfeld Jensen
    email                : kde@carewolf.org
                           (original MPC implementation)
 ***************************************************************************/

/***************************************************************************
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License version   *
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
 *                                                                         *
 *   Alternatively, this file is available under the Mozilla Public        *
 *   License Version 1.1.  You may obtain a copy of the License at         *
 *   http://www.mozilla.org/MPL/                                           *
 ***************************************************************************/

#ifndef TAGLIB_WVPROPERTIES_H
#define TAGLIB_WVPROPERTIES_H

#include "taglib_export.h"
#include "audioproperties.h"

namespace TagLib {

  namespace WavPack {

    class File;

    static const uint HeaderSize = 32;

    //! An implementation of audio property reading for WavPack

    /*!
     * This reads the data from an WavPack stream found in the AudioProperties
     * API.
     */

    class Properties : public AudioProperties
    {
    public:
      /*!
       * Create an instance of WavPack::Properties with the data read from the
       * ByteVector \a data.
       */
      Properties(const ByteVector &data, long streamLength, ReadStyle style = Average);

      /*!
       * Destroys this WavPack::Properties instance.
       */
      virtual ~Properties();

      // Reimplementations.

      virtual int length() const;
      virtual int bitrate() const;
      virtual int sampleRate() const;
      virtual int channels() const;

      /*!
       * Returns number of bits per sample.
       */
      int bitsPerSample() const;

      /*!
       * Returns WavPack version.
       */
      int version() const;

    private:
      Properties(const Properties &);
      Properties &operator=(const Properties &);

      void read();

      class PropertiesPrivate;
      PropertiesPrivate *d;
    };
  }
}

#endif
