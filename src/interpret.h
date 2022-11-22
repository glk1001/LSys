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
 * $Log: interpret.h,v $
 * Revision 1.2  90/10/12  18:48:20  leech
 * First public release.
 *
 */

#pragma once

#include "actions.h"
#include "generator.h"
#include "list.h"
#include "module.h"
#include "symbol_table.h"

#include <string>
#include <utility>

namespace L_SYSTEM
{

class Interpreter
{
public:
  explicit Interpreter(IGenerator& generator);

  auto SetDefaults(float turnAngleInDegrees = 90.0F,
                   float width              = 1.0F,
                   float defaultDistance    = 1.0F) -> void;

  // Iteratively interpret a bound left-system, producing output to the specified generator.
  auto Start(const List<Module>& moduleList) -> void;
  auto Finish() -> void;
  [[nodiscard]] auto InterpretNext() -> bool;

  // Interpret all of a bound left-system, producing output to the specified generator.
  auto InterpretAllModules(const List<Module>& moduleList) -> void;

private:
  Turtle m_turtle{};
  std::unique_ptr<ConstListIterator<Module>> m_moduleIter{};
  IGenerator& m_generator;

  const Module* m_currentModule{};

  auto InterpretNextModule(const Module& mod) -> bool;
  static const SymbolTable<ActionFunc> ACTION_SYMBOL_TABLE;
  [[nodiscard]] static auto GetActionSymbolTable() -> SymbolTable<ActionFunc>;
  [[nodiscard]] static auto GetModuleName(const Module& mod) -> std::string;
  [[nodiscard]] static auto GetActionArgsArray(const Module& mod) -> std::pair<int, ArgsArray>;
};

inline auto Interpreter::Start(const List<Module>& moduleList) -> void
{
  m_generator.Prelude();
  m_moduleIter    = std::make_unique<ConstListIterator<Module>>(moduleList);
  m_currentModule = m_moduleIter->first();
}

inline auto Interpreter::Finish() -> void
{
  m_currentModule = nullptr;
  m_moduleIter    = nullptr;
  m_generator.Postscript();
}

inline auto Interpreter::InterpretNext() -> bool
{
  if (m_currentModule == nullptr)
  {
    return false;
  }

  InterpretNextModule(*m_currentModule);
  m_currentModule = m_moduleIter->next();

  return true;
}

} // namespace L_SYSTEM
