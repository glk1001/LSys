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
 * $Log: PlantModel.h,v $
 * Revision 1.1  91/10/10  19:54:36  leech
 * Initial revision
 *
 */

#pragma once

#include "List.h"
#include "Module.h"
#include "Production.h"
#include "SymbolTable.h"
#include "Value.h"

namespace LSys
{

struct LSysModel
{
  LSysModel() = default;
  LSysModel(const LSysModel&) = delete;
  LSysModel(LSysModel&&) = delete;
  ~LSysModel();

  auto operator=(const LSysModel&) -> LSysModel& = delete;
  auto operator=(LSysModel&&) -> LSysModel& = delete;

  List<Module>* Generate(List<Module>* oldModuleList) const;

  SymbolTable<Value>* ignoreTable = new SymbolTable<Value>; // Symbols ignored in context.
  SymbolTable<Value>* symbolTable =
      new SymbolTable<Value>; // Variables and bound formal parameters.
  List<Production>* rules = new List<Production>; // Production list.
  List<Module>* start     = nullptr; // Starting module list.
};

} // namespace LSys
