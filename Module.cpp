/* Module.c - methods for handling L-system modules.
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
 * $Log:	Module.c,v $
 * Revision 1.3  91/03/20  10:35:38  leech
 * Bug fix in module printing.
 *
 * Revision 1.2  90/10/12  18:48:06  leech
 * First public release.
 *
 */
//static char RCSid[]= "$Id: Module.c,v 1.3 91/03/20 10:35:38 leech Exp $";

#include "Module.h"

#include "Expression.h"
#include "SymbolTable.h"
#include "Value.h"
#include "debug.h"

#include <iostream>

using std::cerr;
using std::endl;


namespace LSys
{

Module::Module(const Name& name, List<Expression>* expressionList, const bool ignoreFlag)
  : m_tag(name), m_ignoreFlag{ignoreFlag}, m_param{expressionList}
{
  PDebug(PD_MODULE, cerr << "Creating module " << *this << " @ " << this << "\n");
}

Module::Module(const Module& mod)
  : m_tag(mod.m_tag), m_ignoreFlag{mod.m_ignoreFlag}, m_emptyFlag{false}, m_param{mod.m_param}
{
  PDebug(PD_MODULE, cerr << "Copying module " << *this << "\n");
}

Module::~Module()
{
  PDebug(PD_MODULE, cerr << "Deleting module @ " << this << "\n");

  if (not m_emptyFlag)
  {
    delete m_param;
  }
}

// Don't delete parameter list when destructor called, even if it's
//  dynamically allocated.
void Module::Empty()
{
  m_emptyFlag = true;
}

// Bind symbolic names of the module to values in module 'values'
//  using symbol table st for evaluation and binding. The two
//  modules should conform() for this method to succeed.
bool Module::Bind(const Module& values, SymbolTable<Value>& symbolTable) const
{
  PDebug(PD_MODULE, cerr << "Module::Bind: formals= " << *this << " values= " << values << endl);

  if (not LSys::Bind(m_param, values.m_param, symbolTable))
  {
    cerr << "failure binding module " << values << " to " << *this << endl;
    return false;
  }

  return true;
}

// Check if module 'm' is conformant with the module, e.g.,
//  that they have the same name and their expression lists are
//  conformant.
bool Module::Conforms(const Module& mod) const
{
  if (m_tag != mod.m_tag)
  {
    return false;
  }

  return LSys::Conforms(m_param, mod.m_param);
}

// Instantiate the module; that is, return a copy with all of the
//  module's expressions evaluated in the context of the symbol table.
Module* Module::Instantiate(SymbolTable<Value>& symbolTable) const
{
  List<Expression>* el = LSys::Instantiate(m_param, symbolTable);
  Module* new_m        = new Module{Name(m_tag), el, m_ignoreFlag};

  PDebug(PD_MODULE,
         cerr << "Module::Instantiate: " << *this << " @ " << this << " -> " << *new_m << " @ "
              << new_m << "\n");
  PDebug(PD_MODULE, cerr << "        old elist: " << *el << "\n");

  return new_m;
}


// Return the n'th (0 base) parameter of module in f, if available.
// Returns true on success, false if module does not have enough parameters
//  or the parameter is not a number.
bool Module::GetFloat(float& fltValue, unsigned int n) const
{
  if (m_param == 0)
  {
    return false;
  }

  // An empty symbol table used to ensure the argument is a bound value.
  static const SymbolTable<Value> symbolTable{};
  return LSys::GetFloat(symbolTable, *m_param, fltValue, n);
}

std::ostream& operator<<(std::ostream& o, const Module& mod)
{
  o << Name(mod.m_tag);
  if (mod.m_param != NULL && mod.m_param->size() > 0)
    o << *mod.m_param;

  return o;
}

} // namespace LSys
