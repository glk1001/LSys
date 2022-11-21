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

#include "list.h"
#include "module.h"
#include "production.h"
#include "symbol_table.h"
#include "value.h"

#include <memory>

namespace L_SYSTEM
{

class LSysModel
{
public:
  LSysModel() = default;

  [[nodiscard]] auto Generate(List<Module>* oldModuleList) -> std::unique_ptr<List<Module>>;

  [[nodiscard]] auto GetSymbolTable() noexcept -> SymbolTable<Value>&;
  [[nodiscard]] auto GetIgnoreTable() noexcept -> SymbolTable<Value>&;
  [[nodiscard]] auto GetRules() noexcept -> List<Production>&;

  auto ResetStartModuleList(List<Module>* moduleList) noexcept;
  [[nodiscard]] auto GetStartModuleList() const noexcept -> const List<Module>*;

private:
  SymbolTable<Value> ignoreTable = SymbolTable<Value>{}; // Symbols ignored in context.
  SymbolTable<Value> symbolTable = SymbolTable<Value>{}; // Variables and bound formal parameters.
  List<Production> rules         = List<Production>{};

  std::shared_ptr<List<Module>> start{};
};

inline auto LSysModel::GetSymbolTable() noexcept -> SymbolTable<Value>&
{
  return symbolTable;
}

inline auto LSysModel::GetIgnoreTable() noexcept -> SymbolTable<Value>&
{
  return ignoreTable;
}

inline auto LSysModel::GetRules() noexcept -> List<Production>&
{
  return rules;
}

inline auto LSysModel::ResetStartModuleList(List<Module>* moduleList) noexcept
{
  start.reset(moduleList);
}

inline auto LSysModel::GetStartModuleList() const noexcept -> const List<Module>*
{
  return start.get();
}

} // namespace L_SYSTEM
