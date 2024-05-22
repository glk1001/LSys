// GLK: New production is faulty!!


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
 * $Log: Production.c,v $
 * Revision 1.2  90/10/12  18:48:11  leech
 * First public release.
 *
 */

module;

#include "debug.h"

#include <iostream>
#include <memory>
#include <stdexcept>

module LSys.Production;

import LSys.Expression;
import LSys.Module;
import LSys.Name;
import LSys.Rand;
import LSys.SymbolTable;
import LSys.Value;

namespace LSYS
{

Production::Production(const Name& name,
                       std::unique_ptr<Predecessor> input,
                       std::unique_ptr<const Expression> condition,
                       std::unique_ptr<const List<Successor>> successors)
  : m_productionName{name},
    m_input{std::move(input)},
    m_condition{std::move(condition)},
    m_successors{std::move(successors)}
{
  // Ensure that empty context lists are represented by 0 pointers
  if ((m_input->left != nullptr) and (0 == m_input->left->size()))
  {
    m_input->left = nullptr;
  }
  if ((m_input->right != nullptr) and (0 == m_input->right->size()))
  {
    m_input->right = nullptr;
  }

  m_contextFree = (nullptr == m_input->left) and (nullptr == m_input->right);

  PDebug(PD_PRODUCTION, std::cerr << "Production::Production: created " << *this << "\n");
}

// See if module m matches the left hand side of this production and
//  satisfies the conditional expression attached to it. The list
//  iterator must be set at m, as it provides context for context-sensitive
//  productions. Neither the iterator nor the module are modified.
auto Production::Matches(const ListIterator<Module>& modIter,
                         const Module* const mod,
                         SymbolTable<Value>& symbolTable) const -> bool
{
  PDebug(PD_PRODUCTION,
         std::cerr << "Production::Matches: testing module " << *mod << " against " << *this
                   << "\n");
  PDebug(PD_PRODUCTION, std::cerr << "\t" << *m_input->center << " matches? " << *mod << "\n");

  // Test the predecessor module itself
  if (not m_input->center->Conforms(*mod))
  {
    return false;
  }

  // Bind formal parameters of the predecessor
  // Should test return value to ensure binding occurred
  m_input->center->Bind(*mod, symbolTable);

  // Now match context-sensitive surroundings, if any.

  // Left context
  if (m_input->left != nullptr)
  {
    PDebug(PD_PRODUCTION, std::cerr << "    [left context]\n");
    // Scan each list in Reverse order
    auto listIterFormal = ListIterator<Module>{*m_input->left};
    auto listIterValue  = ListIterator<Module>{modIter};
    const Module* formal;
    const Module* value;
    for (formal = listIterFormal.last(), value = listIterValue.previous();
         (formal != nullptr) and (value != nullptr);
         formal = listIterFormal.previous(), value = listIterValue.previous())
    {

      // Find the next potentially matching module; skip over ignored modules
      // as well as bracketed substrings (e.g. A < B matches A[anything]B).
      for (auto brackets = 0; value != nullptr; value = listIterValue.previous())
      {
        // Skip over ignored modules
        if (value->Ignore())
        {
          continue;
        }
        // Skip over ], and increase bracket level.
        if (value->GetName() == RIGHT_BRACKET)
        { // ]
          ++brackets;
          continue;
        }
        // Skip over [, and decrease bracket level iff > 0
        if (value->GetName() == LEFT_BRACKET)
        { // [
          if (brackets > 0)
          {
            --brackets;
          }
          continue;
        }
        // Found a potentially matching module
        if (0 == brackets)
        {
          break;
        }
      }

      // If start of string was reached without finding a potentially
      // matching module, context matching failed.
      if (nullptr == value)
      {
        return false;
      }

      PDebug(PD_PRODUCTION, std::cerr << "\t" << *formal << " matches? " << *value << '\n');

      // See if the formal and value modules conform
      if (not formal->Conforms(*value))
      {
        return false;
      }
      // Bind formal arguments
      formal->Bind(*value, symbolTable);
    }

    // If the formal parameter list is non-0, context matching failed
    // This could be due to a nonconforming value module, or simply
    //  running out of value modules to test.
    if (formal != nullptr)
    {
      return false;
    }
  }

  // Right context
  if (m_input->right != nullptr)
  {
    auto listIterFormal = ListIterator<Module>{*m_input->right};
    auto listIterValue  = ListIterator<Module>{modIter};
    const Module* formal;
    const Module* value;

    PDebug(PD_PRODUCTION, std::cerr << "    [right context]\n");
    // Scan each list in Reverse order
    for (formal = listIterFormal.first(), value = listIterValue.next();
         (formal != nullptr) and (value != nullptr);
         formal = listIterFormal.next(), value = listIterValue.next())
    {
      // Find the next potentially matching module; skip over
      //	bracketed substrings, e.g. A < B matches A[anything]B
      //	as well as modules which should be ignored.
      if (formal->GetName() == LEFT_BRACKET)
      { // [
        // Must find a matching [; skip only ignored modules
        while ((value != nullptr) and value->Ignore())
        {
          value = listIterValue.next();
        }
      }
      else if (formal->GetName() == RIGHT_BRACKET)
      { // ]
        // Must find a matching ]; skip anything else including
        //  bracketed substrings.
        for (auto brackets = 0; value != nullptr; value = listIterValue.next())
        {
          if (value->GetName() == RIGHT_BRACKET)
          { // ]
            if (0 == brackets)
            {
              break;
            }
            --brackets;
          }
          else if (value->GetName() == LEFT_BRACKET)
          { // [
            ++brackets;
          }
        }
      }
      else
      {
        // Find the next potentially matching module; skip over
        //  ignored modules as well as bracketed substrings,
        //  (e.g. A > B matches A[anything]B)
        for (auto brackets = 0; value != nullptr; value = listIterValue.next())
        {
          // Skip over ignored modules
          if (value->Ignore())
          {
            continue;
          }
          if (value->GetName() == LEFT_BRACKET)
          { // [
            ++brackets;
            continue;
          }
          if (value->GetName() == RIGHT_BRACKET)
          { // ]
            if (brackets > 0)
            {
              --brackets;
            }
            else
            {
              // This is a case like B > C against A[B]C; it
              //	should not match, because C is not along
              //	the same path from root to branch as B.
              return false;
            }
            continue;
          }
          // Found a potentially matching module
          if (0 == brackets)
          {
            break;
          }
        }
      }

      // If start of string was reached without finding a potentially
      //	matching module, context matching failed.
      if (nullptr == value)
      {
        return false;
      }

      PDebug(PD_PRODUCTION, std::cerr << "\t" << *formal << " Matches? " << *value << '\n');

      // See if the formal and value modules conform
      if (not formal->Conforms(*value))
      {
        return false;
      }
      // Bind formal arguments
      formal->Bind(*value, symbolTable);
    }

    // If the formal parameter list is non-0, context matching failed
    // This could be due to a nonconforming value module, or simply
    //  running out of value modules to test.
    if (formal != nullptr)
    {
      return false;
    }
  }

  // Finally, evaluate the optional conditional expression with the
  //	bound formals; return its boolean value if it evaluated to an
  //	integer, false otherwise.
  if (nullptr == m_condition)
  {
    return true;
  }

  const auto value = m_condition->Evaluate(symbolTable);
  PDebug(PD_PRODUCTION, std::cerr << "    [condition] -> " << value << "\n");
  if (int i; value.GetIntValue(i))
  {
    return i != 0;
  }
  return false;
}

// Given a module which matches() the left hand side of this
//  production, apply the production and return the resulting
//  module list.
auto Production::Produce(const Module* const predecessor, SymbolTable<Value>& symbolTable) const
    -> std::unique_ptr<List<Module>>
{
  auto moduleList = std::make_unique<List<Module>>();

  // If no successors for this production, die (could return an empty list
  //	or return a copy of the predecessor).
  if (0 == m_successors->size())
  {
    throw std::runtime_error("Production::Produce: Error: production has no successors");
  }

  // Pick one of the successors of the production at random.
  const auto randomVar       = static_cast<float>(GetRandDoubleInUnitInterval());
  auto cumulativeProbability = 0.0F;

  const List<Module>* modList;
  const Successor* succ;
  auto successorIter = ConstListIterator<Successor>{*m_successors};
  for (modList = nullptr, succ = successorIter.first(); succ != nullptr;
       succ = successorIter.next())
  {
    cumulativeProbability += succ->m_probability;
    if (randomVar <= cumulativeProbability)
    {
      modList = succ->m_moduleList.get();
      break;
    }
  }

  // If no successor was chosen, complain and return an empty list
  if (nullptr == modList)
  {
    std::cerr << "Error in Production::Produce: no successor was chosen:\n" << *this << "\n";
    throw std::runtime_error("Error in Production::Produce: no successor was chosen");
    //    return moduleList;
  }

  // For each module in the successor side, instantiate it and add to the list.
  auto modIter = ConstListIterator<Module>{*modList};
  for (const auto* mod = modIter.first(); mod != nullptr; mod = modIter.next())
  {
    auto newModule = mod->Instantiate(symbolTable);
    moduleList->append(std::move(newModule));
  }

  PDebug(PD_PRODUCTION,
         std::cerr << "Production::Produce:\n"
                   << "Production is:  " << *this << "\n"
                   << "Predecessor is: " << *predecessor << "\n"
                   << "Result is:      " << *moduleList << "\n");

  return moduleList;
}

auto operator<<(std::ostream& out, const Successor& successor) -> std::ostream&
{
  out << "\t-> ";
  if (successor.m_probability < 1.0F)
  {
    out << '(' << successor.m_probability << ") ";
  }
  out << *successor.m_moduleList;

  return out;
}

auto operator<<(std::ostream& out, const Predecessor& predecessor) -> std::ostream&
{
  if (predecessor.left != nullptr)
  {
    out << *predecessor.left << " < ";
  }
  // TODO(glk) - Should always be non-NULL
  if (predecessor.center != nullptr)
  {
    out << *predecessor.center;
  }
  if (predecessor.right != nullptr)
  {
    out << " > " << *predecessor.right;
  }

  return out;
}

auto operator<<(std::ostream& out, const Production& production) -> std::ostream&
{
  out << "( " << production.m_productionName << " : " << *production.m_input;
  if (production.m_contextFree)
  {
    out << " (CF) ";
  }
  if (production.m_condition != nullptr)
  {
    out << " : " << *production.m_condition;
  }
  out << *production.m_successors;

  return out;
}

} // namespace LSYS
