/***************************************************************************
    copyright            : (C) 2004 by Allan Sandfeld Jensen
    email                : kde@carewolf.com
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

#include <tbytevectorlist.h>

#include "apeitem.h"

using namespace TagLib;
using namespace APE;

class APE::Item::ItemPrivate
{
public:
  ItemPrivate() : type(Text), readOnly(false) {}

  Item::ItemTypes type;
  String key;
  ByteVector value;
  StringList text;
  bool readOnly;
};

APE::Item::Item()
{
  d = new ItemPrivate;
}

APE::Item::Item(const String &key, const String &value)
{
  d = new ItemPrivate;
  d->key = key;
  d->text.append(value);
}

APE::Item::Item(const String &key, const StringList &values)
{
  d = new ItemPrivate;
  d->key = key;
  d->text = values;
}

APE::Item::Item(const Item &item)
{
  d = new ItemPrivate(*item.d);
}

Item &APE::Item::operator=(const Item &item)
{
  delete d;
  d = new ItemPrivate(*item.d);
  return *this;
}

void APE::Item::setReadOnly(bool readOnly)
{
  d->readOnly = readOnly;
}

bool APE::Item::isReadOnly() const
{
  return d->readOnly;
}

void APE::Item::setType(APE::Item::ItemTypes val)
{
  d->type = val;
}

APE::Item::ItemTypes APE::Item::type() const
{
  return d->type;
}

String APE::Item::key() const
{
  return d->key;
}

ByteVector APE::Item::value() const
{
  return d->value;
}

int APE::Item::size() const
{
  return 8 + d->key.size() + 1 + d->value.size();
}

StringList APE::Item::toStringList() const
{
  return d->text;
}

String APE::Item::toString() const
{
  return d->text.front();
}

bool APE::Item::isEmpty() const
{
  switch(d->type) {
    case 0:
    case 1:
      if(d->text.isEmpty())
        return true;
      if(d->text.size() == 1 && d->text.front() == "")
        return true;
      return false;
    case 2:
      return d->value.isEmpty();
    default:
      return false;
  }
}

void APE::Item::parse(const ByteVector &data)
{
  // 11 bytes is the minimum size for an APE item

  if(data.size() < 11) {
    debug("APE::Item::parse() -- no data in item");
    return;
  }

  uint valueLength  = data.mid(0, 4).toUInt(false);
  uint flags        = data.mid(4, 4).toUInt(false);

  d->key = String(data.mid(8), String::UTF8);

  d->value = data.mid(8 + d->key.size() + 1, valueLength);

  setReadOnly(flags & 1);
  setType(ItemTypes((flags >> 1) & 3));

  if(int(d->type) < 2)
    d->text = StringList(ByteVectorList::split(d->value, '\0'), String::UTF8);
}

ByteVector APE::Item::render() const
{
  ByteVector data;
  TagLib::uint flags = ((d->readOnly) ? 1 : 0) | (d->type << 1);
  ByteVector value;

  if(isEmpty())
    return data;

  if(d->type != Item::Binary) {
    StringList::ConstIterator it = d->text.begin();
    value.append(it->data(String::UTF8));
    it++;
    for(; it != d->text.end(); ++it) {
      value.append('\0');
      value.append(it->data(String::UTF8));
    }
    d->value = value;
  }
  else
    value.append(d->value);

  data.append(ByteVector::fromUInt(value.size(), false));
  data.append(ByteVector::fromUInt(flags, false));
  data.append(d->key.data(String::UTF8));
  data.append(ByteVector('\0'));
  data.append(value);

  return data;
}
