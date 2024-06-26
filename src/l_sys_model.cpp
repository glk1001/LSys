/*
 * Copyright (C) 1991, Jonathan P. Leech
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
 * $Log: PlantModel.c,v $
 * Revision 1.1  91/10/10  19:53:58  leech
 * Initial revision
 *
 */

module;

#include "debug.h"

#include <iostream>
#include <memory>
#include <string>

module LSys.LSysModel;

import LSys.Expression;
import LSys.List;
import LSys.Module;
import LSys.Production;
import LSys.Value;

namespace LSYS
{

LSysModel::~LSysModel() noexcept = default;

auto LSysModel::ResetArgument(const std::string& name, const Value& newValue) -> void
{
  if (Value value; not m_symbolTable.Lookup(name, value))
  {
    throw std::runtime_error("Could not find argument to reset.");
  }
  m_symbolTable.Enter(name, newValue);
}

// Apply the model to the specified list for one generation, generating a new list.
auto LSysModel::Generate(List<Module>* const oldModuleList) -> std::unique_ptr<List<Module>>
{
  auto ruleIter      = ConstListIterator<Production>{m_rules};
  auto newModuleList = std::make_unique<List<Module>>();

  auto oldModIter = ListIterator<Module>{*oldModuleList};
  for (Module* oldMod = oldModIter.first(); oldMod != nullptr; oldMod = oldModIter.next())
  {
    PDebug(PD_PRODUCTION, std::cerr << "Searching for matching production to " << *oldMod << "\n");

    // Find a matching production.
    // NOTE: This could be optimized a bunch.
    const Production* rule = nullptr;
    for (rule = ruleIter.first(); rule != nullptr; rule = ruleIter.next())
    {
      if (rule->Matches(oldModIter, oldMod, m_symbolTable))
      {
        PDebug(PD_PRODUCTION, std::cerr << "\tmatched by: " << *rule << "\n");
        break;
      }
    }
    // If we found one, replace the module by its successor.
    if (rule != nullptr)
    {
      const auto result = rule->Produce(oldMod, m_symbolTable);
      PDebug(PD_PRODUCTION, std::cerr << "\tapplied production yielding: " << *result << "\n");
      newModuleList->append(result.get());
    }
    else
    {
      PDebug(PD_PRODUCTION, std::cerr << "\tno match found, passing production unchanged\n");
      auto oldModPtr = std::make_unique<Module>(*oldMod);
      newModuleList->append(std::move(oldModPtr));
    }
  }

  return newModuleList;
}

} // namespace LSYS
