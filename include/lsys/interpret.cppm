/* Copyright (C) 1990, Jonathan P. Leech
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

module;

#include <memory>
#include <string>
#include <utility>

export module LSys.Interpret;

import LSys.Actions;
import LSys.Consts;
import LSys.Generator;
import LSys.List;
import LSys.Module;
import LSys.SymbolTable;
import LSys.Turtle;

export namespace LSYS
{

class Interpreter
{
public:
  explicit Interpreter(IGenerator& generator);

  struct DefaultParams
  {
    float turnAngleInDegrees = 90.0F;
    float width              = 1.0F;
    float distance           = 1.0F;
  };
  auto SetDefaults(const DefaultParams& defaultParams) -> void;

  // Iteratively interpret a bound left-system, producing output to the specified generator.
  auto Start(const List<Module>& moduleList) -> void;
  auto Finish() -> void;

  auto InterpretNext() -> void;
  [[nodiscard]] auto AllDone() -> bool;

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

} // namespace LSYS

namespace LSYS
{

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

inline auto Interpreter::InterpretNext() -> void
{
  InterpretNextModule(*m_currentModule);
  m_currentModule = m_moduleIter->next();
}

inline auto Interpreter::AllDone() -> bool
{
  return m_currentModule == nullptr;
}

} // namespace LSYS
