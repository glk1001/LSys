/* PlantModel.c - object encapsulating a generatable plant model
 *
 * $Id: PlantModel.c,v 1.1 91/10/10 19:53:58 leech Exp $
 *
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
 * $Log:	PlantModel.c,v $
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
#include "SymbolTable.h"
#include "Value.h"
#include "debug.h"

using std::cerr;
using std::endl;


namespace LSys
{


LSysModel::LSysModel()
{
  ignoreTable = new SymbolTable<Value>;
  symbolTable = new SymbolTable<Value>;

  rules = new List<Production>;
  start = 0;
}


LSysModel::~LSysModel()
{
  delete ignoreTable;
  delete symbolTable;
  delete rules;
  delete start;
}


// Apply the model to the specified list for one generation, generating a new list.
List<Module>* LSysModel::Generate(List<Module>* oldModuleList)
{
  ListIterator<Production> pi(rules);
  List<Module>* newModuleList = new List<Module>;

  // For each input module
  ListIterator<Module> mi(*oldModuleList);
  for (Module* m = mi.first(); m != 0; m = mi.next())
  {
    PDebug(PD_PRODUCTION, cerr << "Searching for matching production to " << *m << endl);

    // Find a matching production.
    // NOTE: This could be optimized a bunch.
    Production* p;
    for (p = pi.first(); p != 0; p = pi.next())
    {
      if (p->matches(mi, m, *symbolTable))
      {
        PDebug(PD_PRODUCTION, cerr << "\tmatched by: " << *p << endl);
        break;
      }
    }
    // If we found one, replace the module by its successor.
    if (p != 0)
    {
      List<Module>* result = p->produce(m, *symbolTable);
      PDebug(PD_PRODUCTION, cerr << "\tapplied production yielding: " << *result << endl);
      newModuleList->append(result);
      delete result;
    }
    else
    {
      PDebug(PD_PRODUCTION, cerr << "\tno match found, passing production unchanged\n");
      newModuleList->append(new Module(*m));
      m->empty();
    }
  }

  return newModuleList;
}


}; // namespace LSys
