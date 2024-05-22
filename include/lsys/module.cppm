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
 * $Log: Module.h,v $
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

module;

#include <memory>

export module LSys.Module;

import LSys.Expression;
import LSys.List;
import LSys.Name;
import LSys.SymbolTable;
import LSys.Value;

export namespace LSYS
{

// A Module is the basic object of an L-system to which productions
//  are applied, and which is interpreted in terms of turtle movements
//  and graphical objects. Since there are so many of them allocated,
//  the class is tightly packed.

class Module
{
public:
  Module(const Name& name,
         std::unique_ptr<List<Expression>> expressionList,
         bool ignoreFlag = false);
  Module(const Module& other) noexcept;
  Module(Module&&) noexcept = default;
  ~Module() noexcept        = default;

  auto operator=(const Module&) -> Module& = delete;
  auto operator=(Module&&) -> Module&      = default;

  [[nodiscard]] auto GetName() const -> Name { return Name(m_tag); }

  auto Bind(const Module& values, SymbolTable<Value>& symbolTable) const -> void;
  [[nodiscard]] auto Conforms(const Module& mod) const -> bool;
  [[nodiscard]] auto Ignore() const -> bool { return m_ignoreFlag; }
  [[nodiscard]] auto Instantiate(const SymbolTable<Value>& symbolTable) const
      -> std::unique_ptr<Module>;
  [[nodiscard]] auto GetFloat(float& fltValue, unsigned int n = 0) const -> bool;

  friend auto operator<<(std::ostream& out, const Module& mod) -> std::ostream&;

private:
  int m_tag; // Module name
  bool m_ignoreFlag; // Should module be ignored in context?
  std::unique_ptr<List<Expression>> m_param; // Expressions bound to module
};

inline const Name LEFT_BRACKET{"["};
inline const Name RIGHT_BRACKET{"]"};

} // namespace LSYS

namespace LSYS
{

inline Module::Module(const Module& other) noexcept
  : m_tag{other.m_tag},
    m_ignoreFlag{other.m_ignoreFlag},
    m_param{other.m_param == nullptr ? std::make_unique<List<Expression>>()
                                     : std::make_unique<List<Expression>>(*other.m_param)}
{
}

} // namespace LSYS
