/*
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
 * $Log: Name.h,v $
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

#include "symbol_table.h"

#include <cstdint>
#include <string>

namespace LSYS
{

class Name
{
public:
  explicit Name(const char* name);
  explicit Name(int id);

#if __clang_major__ >= 16
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wunsafe-buffer-usage"
#endif
  [[nodiscard]] auto str() const -> std::string { return s_reverseMap[m_index]; }
#if __clang_major__ >= 16
#pragma GCC diagnostic pop
#endif
  [[nodiscard]] auto id() const -> int { return m_index; }

private:
  static inline SymbolTable<int> s_map{};
#if __clang_major__ >= 16
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wunsafe-buffer-usage"
#endif
  static char** s_reverseMap;
#if __clang_major__ >= 16
#pragma GCC diagnostic pop
#endif
  static int s_nextIndex;
  static uint32_t s_reverseMapSize;
  int m_index = 0;
  friend auto operator==(const Name& a, const Name& b) -> bool;
};

inline Name::Name(const int id)
  : m_index((id < 0) or (id >= static_cast<int>(s_reverseMapSize)) ? 0 : id)
{
}

inline auto operator==(const Name& a, const Name& b) -> bool
{
  return a.m_index == b.m_index;
}

inline auto operator!=(const Name& a, const Name& b) -> bool
{
  return not(a == b);
}

std::ostream& operator<<(std::ostream& out, const Name& name);

} // namespace LSYS
