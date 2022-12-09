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
 * $Log: Production.h,v $
 * Revision 1.3  91/03/20  10:39:45  leech
 * Support for G++.
 *
 * Revision 1.2  90/10/12  18:48:11  leech
 * First public release.
 *
 */

#pragma once

#include "module.h"

#include <memory>

namespace LSYS
{

// A Predecessor is the left-hand side of a production; it contains
// the module replaced by a list of successors and optionally,
// the left and right context required to apply the production.
// The class is basically just a container for three pointers,
// so it's declared public.
struct Predecessor
{
  Predecessor(std::unique_ptr<List<Module>> lft,
              std::unique_ptr<Module> cen,
              std::unique_ptr<List<Module>> rgt)
    : left{std::move(lft)}, center{std::move(cen)}, right{std::move(rgt)}
  {
  }

  friend auto operator<<(std::ostream& out, const Predecessor& predecessor) -> std::ostream&;

  std::unique_ptr<List<Module>> left;
  std::unique_ptr<Module> center;
  std::unique_ptr<List<Module>> right;
};

// A Successor is the right-hand side of a production; it contains
//  a probability of choosing this production and a list of modules
//  to replace the predecessor module with if chosen.
// This is also just a container class, but it can be non-public
//  with judicious use of friends.
class Successor
{
public:
  explicit Successor(std::unique_ptr<const List<Module>> moduleList, const float probability = 1.0F)
    : m_probability{probability}, m_moduleList{std::move(moduleList)}
  {
  }

  friend class Production;
  friend auto operator<<(std::ostream& out, const Successor& successor) -> std::ostream&;

private:
  float m_probability;
  std::unique_ptr<const List<Module>> m_moduleList;
};

// A Production is applied to a Module to produce a new list of Modules.
// The production may be context-sensitive to surrounding Modules.
class Production
{
public:
  Production(const Name& name,
             std::unique_ptr<Predecessor> input,
             std::unique_ptr<const Expression> condition,
             std::unique_ptr<const List<Successor>> successors);

  auto IsContextFree() const -> bool { return m_contextFree; }
  auto Matches(const ListIterator<Module>& modIter,
               const Module* mod,
               SymbolTable<Value>& symbolTable) -> bool;
  auto Produce(const Module* predecessor, SymbolTable<Value>& symbolTable)
      -> std::unique_ptr<List<Module>>;

  friend auto operator<<(std::ostream& out, const Production& production) -> std::ostream&;

private:
  Name m_productionName;
  bool m_contextFree = false; // Is the production context-free?
  std::unique_ptr<Predecessor> m_input;
  std::unique_ptr<const Expression> m_condition;
  std::unique_ptr<const List<Successor>> m_successors;
};

} // namespace LSYS
