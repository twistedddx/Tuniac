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

#include "tbytevectorlist.h"

using namespace TagLib;

class ByteVectorListPrivate
{

};

////////////////////////////////////////////////////////////////////////////////
// static members
////////////////////////////////////////////////////////////////////////////////

ByteVectorList ByteVectorList::split(const ByteVector &v, const ByteVector &pattern,
                                     int byteAlign)
{
  ByteVectorList l;

  uint previousOffset = 0;
  for(int offset = v.find(pattern, 0, byteAlign);
      offset != -1;
      offset = v.find(pattern, offset + pattern.size(), byteAlign))
  {
    l.append(v.mid(previousOffset, offset - previousOffset));
    previousOffset = offset + pattern.size();
  }

  if(previousOffset < v.size())
    l.append(v.mid(previousOffset, v.size() - previousOffset));

  return l;
}

////////////////////////////////////////////////////////////////////////////////
// public members
////////////////////////////////////////////////////////////////////////////////

ByteVectorList::ByteVectorList() : List<ByteVector>()
{

}

ByteVectorList::ByteVectorList(const ByteVectorList &l) : List<ByteVector>(l)
{

}

ByteVectorList::~ByteVectorList()
{

}

ByteVector ByteVectorList::toByteVector(const ByteVector &separator) const
{
  ByteVector v;

  ConstIterator it = begin();

  while(it != end()) {
    v.append(*it);
    it++;
    if(it != end())
      v.append(separator);
  }

  return v;
}
