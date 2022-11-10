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
 * $Log: Module.c,v $
 * Revision 1.3  91/03/20  10:35:38  leech
 * Bug fix in module printing.
 *
 * Revision 1.2  90/10/12  18:48:06  leech
 * First public release.
 *
 */

#include "Module.h"

#include "Expression.h"
#include "SymbolTable.h"
#include "Value.h"
#include "debug.h"

#include <iostream>

namespace LSys
{

Module::Module(const Name& name, List<Expression>* const expressionList, const bool ignoreFlag)
  : m_tag(name.id()), m_ignoreFlag{ignoreFlag}, m_param{expressionList}
{
  PDebug(PD_MODULE, std::cerr << "Creating module " << *this << " @ " << this << "\n");
}

Module::Module(const Module& mod)
  : m_tag(mod.m_tag), m_ignoreFlag{mod.m_ignoreFlag}, m_param{mod.m_param}
{
  PDebug(PD_MODULE, std::cerr << "Copying module " << *this << "\n");
}

Module::~Module()
{
  PDebug(PD_MODULE, std::cerr << "Deleting module @ " << this << "\n");

  if (not m_emptyFlag)
  {
    delete m_param;
  }
}

// Don't delete parameter list when destructor called, even if it's
//  dynamically allocated.
auto Module::Empty() -> void
{
  m_emptyFlag = true;
}

// Bind symbolic names of the module to values in module 'values'
//  using symbol table st for evaluation and binding. The two
//  modules should conform() for this method to succeed.
auto Module::Bind(const Module& values, SymbolTable<Value>& symbolTable) const -> bool
{
  PDebug(PD_MODULE,
         std::cerr << "Module::Bind: formals= " << *this << " values= " << values << "\n");

  if (not LSys::Bind(m_param, values.m_param, symbolTable))
  {
    std::cerr << "failure binding module " << values << " to " << *this << "\n";
    return false;
  }

  return true;
}

// Check if module 'm' is conformant with the module, e.g.,
//  that they have the same name and their expression lists are
//  conformant.
auto Module::Conforms(const Module& mod) const -> bool
{
  if (m_tag != mod.m_tag)
  {
    return false;
  }

  return LSys::Conforms(m_param, mod.m_param);
}

// Instantiate the module; that is, return a copy with all of the
//  module's expressions evaluated in the context of the symbol table.
auto Module::Instantiate(const SymbolTable<Value>& symbolTable) const -> Module*
{
  auto* const exprList  = LSys::Instantiate(m_param, symbolTable);
  auto* const newModule = new Module{Name(m_tag), exprList, m_ignoreFlag};

  PDebug(PD_MODULE,
         std::cerr << "Module::Instantiate: " << *this << " @ " << this << " -> " << *newModule
                   << " @ " << newModule << "\n");
  PDebug(PD_MODULE, std::cerr << "        old elist: " << *exprList << "\n");

  return newModule;
}

// Return the nth (0 base) parameter of module in f, if available.
// Returns true on success, false if module does not have enough parameters
//  or the parameter is not a number.
auto Module::GetFloat(float& fltValue, const unsigned int n) const -> bool
{
  if (nullptr == m_param)
  {
    return false;
  }

  // An empty symbol table used to ensure the argument is a bound value.
  static const auto s_SYMBOL_TABLE = SymbolTable<Value>{};
  return LSys::GetFloat(s_SYMBOL_TABLE, *m_param, fltValue, n);
}

auto operator<<(std::ostream& out, const Module& mod) -> std::ostream&
{
  out << Name{mod.m_tag};
  if ((mod.m_param != nullptr) and (mod.m_param->size() > 0))
  {
    out << *mod.m_param;
  }

  return out;
}

} // namespace LSys
