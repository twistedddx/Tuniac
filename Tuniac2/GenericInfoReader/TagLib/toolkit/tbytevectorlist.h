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

#ifndef TAGLIB_BYTEVECTORLIST_H
#define TAGLIB_BYTEVECTORLIST_H

#include "tbytevector.h"
#include "tlist.h"

namespace TagLib {

  //! A list of ByteVectors

  /*!
   * A List specialization with some handy features useful for ByteVectors.
   */

  class ByteVectorList : public List<ByteVector>
  {
  public:

    /*!
     * Construct an empty ByteVectorList.
     */
    ByteVectorList();

    /*!
     * Destroys this ByteVectorList instance.
     */
    virtual ~ByteVectorList();

    /*!
     * Make a shallow, implicitly shared, copy of \a l.  Because this is
     * implicitly shared, this method is lightweight and suitable for
     * pass-by-value usage.
     */
    ByteVectorList(const ByteVectorList &l);

    /*!
     * Convert the ByteVectorList to a ByteVector separated by \a separator.  By
     * default a space is used.
     */
    ByteVector toByteVector(const ByteVector &separator = " ") const;

    /*!
     * Splits the ByteVector \a v into several strings at \a pattern.  This will
     * not include the pattern in the returned ByteVectors.
     */
    static ByteVectorList split(const ByteVector &v, const ByteVector &pattern,
                                int byteAlign = 1);

  private:
    class ByteVectorListPrivate;
    ByteVectorListPrivate *d;
  };

}

#endif
