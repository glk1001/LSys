/* Module.h - class definition for L-system modules.
 *
 * $Id: Module.h,v 1.4 92/06/22 00:25:23 leech Exp $
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
 * $Log:	Module.h,v $
 * Revision 1.4  92/06/22  00:25:23  leech
 * *** empty log message ***
 *
 * Revision 1.3  91/03/20  10:39:39  leech
 * Support for G++.
 *
 * Revision 1.2  90/10/12  18:48:07  leech
 * First public release.
 *
 */

#pragma once

#include "Expression.h"
#include "Name.h"

namespace LSys
{

// A Module is the basic object of an L-system to which productions
//  are applied, and which is interpreted in terms of turtle movements
//  and graphical objects. Since there are so many of them allocated,
//  the class is tightly packed.
class Module
{
public:
  Module(const Name&, List<Expression>* expressionList, bool ignoreFlag = false);
  Module(const Module&);
  ~Module();

  // Call Empty before deallocating a module if it has previously
  // been used as the argument to a copy constructor.
  void Empty();

  Name GetName() const { return Name(m_tag); }

  bool Bind(const Module& values, SymbolTable<Value>& symbolTable) const;
  bool Conforms(const Module& mod) const;
  bool Ignore() const { return m_ignoreFlag; }
  Module* Instantiate(SymbolTable<Value>& symbolTable) const;
  bool GetFloat(float&, unsigned int n = 0) const;

  friend std::ostream& operator<<(std::ostream&, const Module&);

private:
  int m_tag; // Module name
  bool m_ignoreFlag; // Should module be ignored in context?
  bool m_emptyFlag = false; // Should tag and param be deleted?
  List<Expression>* m_param; // Expressions bound to module
};

// These Names are used in context matching to ascend/descend tree levels.
inline const Name LEFT_BRACKET{"["};
inline const Name RIGHT_BRACKET{"]"};

} // namespace LSys
