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
 *
 */
//static char RCSid[] = "$Id: PlantModel.c,v 1.1 91/10/10 19:53:58 leech Exp $";

#include "LSysModel.h"

#include "Expression.h"
#include "Module.h"
#include "Production.h"
#include "debug.h"

#include <iostream>

namespace LSys
{

// Apply the model to the specified list for one generation, generating a new list.
auto LSysModel::Generate(List<Module>* const oldModuleList) -> List<Module>*
{
  auto prodIter             = ListIterator<Production>{&rules};
  auto* const newModuleList = new List<Module>;

  auto modIter = ListIterator<Module>{*oldModuleList};
  for (Module* mod = modIter.first(); mod != nullptr; mod = modIter.next())
  {
    PDebug(PD_PRODUCTION, std::cerr << "Searching for matching production to " << *mod << "\n");

    // Find a matching production.
    // NOTE: This could be optimized a bunch.
    Production* prod;
    for (prod = prodIter.first(); prod != nullptr; prod = prodIter.next())
    {
      if (prod->Matches(modIter, mod, symbolTable))
      {
        PDebug(PD_PRODUCTION, std::cerr << "\tmatched by: " << *prod << "\n");
        break;
      }
    }
    // If we found one, replace the module by its successor.
    if (prod != nullptr)
    {
      auto* const result = prod->Produce(mod, symbolTable);
      PDebug(PD_PRODUCTION, std::cerr << "\tapplied production yielding: " << *result << "\n");
      newModuleList->append(result);
      delete result;
    }
    else
    {
      PDebug(PD_PRODUCTION, std::cerr << "\tno match found, passing production unchanged\n");
      newModuleList->append(new Module(*mod));
      mod->Empty();
    }
  }

  return newModuleList;
}

} // namespace LSys
