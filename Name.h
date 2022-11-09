/* Name.h - class definition for space-efficient hashed names.
 *
 * $Id: Name.h,v 1.5 93/01/21 12:06:36 leech Exp $
 *
 * Copyright (C) 1990, Jonathan P. Leech
 *
 * This software may be freely copied, modified, and redistributed,
 * provided that this copyright notice is preserved on all copies.
 *
 * There is no warranty or other guarantee of fitness for this software,
 * it is provided solely "as is". Bug reports or fixes may be sent
 * to the author, who may or may not act on them as he desires.
 *
 * You may not include this software in a program or other software product
 * without supplying the source, or without informing the end-user that the
 * source is available for no extra charge.
 *
 * If you modify this software, you should include a notice giving the
 * name of the person performing the modification, the date of modification,
 * and the reason for such modification.
 *
 * $Log:	Name.h,v $
 * Revision 1.5  93/01/21  12:06:36  leech
 * Put workaround for g++ bug back in.
 *
 * Revision 1.4  92/06/22  00:26:45  leech
 * G++ does default copy constructor correctly now.
 *
 * Revision 1.3  91/03/20  10:41:18  leech
 * Support for G++ bug (no automatically generated Name::Name(Name &)
 * constructor).
 *
 * Revision 1.2  90/10/12  18:48:08  leech
 * First public release.
 *
 */

#pragma once

#include "SymbolTable.h"

namespace LSys
{

class Name
{
public:
  explicit Name(const char*);
  explicit Name(int id);
  operator int() const { return index; }
  operator const char*() const { return reverse_map[index]; }
  const char* str() const { return reverse_map[index]; }

private:
  static SymbolTable<int>* map;
  static char** reverse_map;
  static int next_index;
  static int reverse_map_size;
  int index;
};

inline Name::Name(int id) : index((id < 0 || id >= reverse_map_size) ? 0 : id)
{
}

inline bool operator==(const Name& a, const Name& b)
{
  return static_cast<int>(a) == static_cast<int>(b);
}

inline int operator!=(const Name& a, const Name& b)
{
  return static_cast<int>(a) != static_cast<int>(b);
}

std::ostream& operator<<(std::ostream&, const Name&);

} // namespace LSys
