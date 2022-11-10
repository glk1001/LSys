/* Name.c - methods for space-efficient hashed names.
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
 * $Log: Name.c,v $
 * Revision 1.3  91/03/20  10:38:10  leech
 * Added debugging output.
 *
 * Revision 1.2  90/10/12  18:48:07  leech
 * First public release.
 *
 */
//static char RCSid[]= "$Id: Name.c,v 1.3 91/03/20 10:38:10 leech Exp $";

#include "Name.h"

#include "debug.h"

#include <cstring>
#include <iostream>

namespace LSys
{

SymbolTable<int>* Name::s_map   = nullptr;
char** Name::s_reverseMap       = nullptr;
int Name::s_nextIndex           = 0;
uint32_t Name::s_reverseMapSize = 0U;

// Amount to extend name table by when it fills up.
static constexpr auto MAP_INCR = 20;

Name::Name(const char* name)
{
  if (nullptr == name)
  {
    name = "";
  }
  if (nullptr == s_map)
  {
    s_map = new SymbolTable<int>;
  }

  PDebug(PD_NAME, std::cerr << "Name(" << name << std::flush << "= " << name << ")\n");

  // If the name already exists in the symbol table, use the assigned
  // hash value. Otherwise, enter it in the table with the next
  // sequential value.
  if (s_map->lookup(name, m_index))
  {
    return;
  }

  if (s_nextIndex == static_cast<int>(s_reverseMapSize))
  {
    // Allocate more space for the inverse map from IDs to strings
    //GLK int size= reverse_map_size + map_incr;
    auto** const strPtr = new char*[s_reverseMapSize + MAP_INCR];

    for (auto i = 0U; i < s_reverseMapSize; ++i)
    {
      strPtr[i] = s_reverseMap[i];
    }
    delete s_reverseMap;
    s_reverseMap = strPtr;
    s_reverseMapSize += MAP_INCR;
  }

  s_map->enter(name, s_nextIndex);
  s_reverseMap[s_nextIndex] = ::strdup(name);
  m_index                   = s_nextIndex++;
}

std::ostream& operator<<(std::ostream& out, const Name& name)
{
  return out << name.str();
  //  return o << "[id: " << int(n) << " name: " << (const char *)n << "]";
}

} // namespace LSys
