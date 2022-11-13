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
#include <stdexcept>

namespace LSys
{

Module::Module(const Name& name, List<Expression>* const expressionList, const bool ignoreFlag)
  : m_tag(name.id()), m_ignoreFlag{ignoreFlag}, m_param{expressionList}
{
  PDebug(PD_MODULE, std::cerr << "Creating module " << *this << " @ " << this << "\n");
}

// Bind symbolic names of the module to values in module 'values'
//  using symbol table st for evaluation and binding. The two
//  modules should conform() for this method to succeed.
auto Module::Bind(const Module& values, SymbolTable<Value>& symbolTable) const -> void
{
  PDebug(PD_MODULE,
         std::cerr << "Module::Bind: formals= " << *this << " values= " << values << "\n");

  if (not LSys::Bind(m_param.get(), values.m_param.get(), symbolTable))
  {
    throw std::runtime_error("Failure binding module.");
  }
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

  return LSys::Conforms(m_param.get(), mod.m_param.get());
}

// Instantiate the module; that is, return a copy with all of the
//  module's expressions evaluated in the context of the symbol table.
auto Module::Instantiate(const SymbolTable<Value>& symbolTable) const -> Module*
{
  auto* const exprList  = LSys::Instantiate(m_param.get(), symbolTable);
  auto* const newModule = new Module{Name(m_tag), exprList, m_ignoreFlag};

  PDebug(PD_MODULE,
         std::cerr << "Module::Instantiate: " << *this << " @ " << this << " -> " << *newModule
                   << " @ " << newModule << "\n");
  if (exprList != nullptr)
  {
    PDebug(PD_MODULE, std::cerr << "        old elist: " << *exprList << "\n");
  }

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
