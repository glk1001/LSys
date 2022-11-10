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
 * $Log:	Expression.h,v $
 * Revision 1.5  95/05/24  17:13:21  leech
 * Fix for const-correctness.
 *
 * Revision 1.4  1993/05/12  22:06:43  leech
 * Reduce warnings from cfront 3.0.1.
 *
 * Revision 1.3  91/03/20  10:39:26  leech
 * Support for G++.
 *
 * Revision 1.2  90/10/12  18:48:03  leech
 * First public release.
 *
 */

#pragma once

#include "List.h"
#include "Name.h"
#include "Value.h"

#include <array>

namespace LSys
{

class Expression
{
public:
  Expression(int operation, Expression* lop, Expression* rop);
  explicit Expression(const Name& name, List<Expression>* funcArgs = nullptr);
  explicit Expression(const Value&);
  Expression(const Expression&) = default;
  Expression(Expression&&)      = default;
  ~Expression();

  auto operator=(const Expression&) -> Expression& = default;
  auto operator=(Expression&&) -> Expression&      = default;

  // Access methods
  [[nodiscard]] auto GetType() const -> int { return m_operation; }
  [[nodiscard]] auto GetName() const -> Name;

  // Evaluation methods
  [[nodiscard]] auto Evaluate(const SymbolTable<Value>& symbolTable) const -> Value;
  [[nodiscard]] auto LEval(const SymbolTable<Value>& st) const -> Value
  {
    return GetLChild()->Evaluate(st);
  }
  [[nodiscard]] auto REval(const SymbolTable<Value>& st) const -> Value
  {
    return GetRChild()->Evaluate(st);
  }

  friend std::ostream& operator<<(std::ostream&, const Expression&);

private:
  int m_operation;

  struct ExpressionValue
  {
    struct Name
    {
      int id{};
      List<Expression>* funcArgs{};
    };
    Name name;
    Value value; // Ensure union is big enough for a Value
    std::array<Expression*, 2> args{}; // Child expressions
  };
  ExpressionValue m_expressionValue;
  [[nodiscard]] static auto GetExpressionValue(const Name& name, List<Expression>* funcArgs)
      -> ExpressionValue;

  [[nodiscard]] auto GetVarName() const -> Name { return Name{m_expressionValue.name.id}; }
  [[nodiscard]] auto GetValue() const -> Value { return m_expressionValue.value; }
  [[nodiscard]] auto GetLChild() const -> Expression* { return m_expressionValue.args[0]; }
  [[nodiscard]] auto GetRChild() const -> Expression* { return m_expressionValue.args[1]; }
  [[nodiscard]] auto GetFuncName() const -> Name { return Name(m_expressionValue.name.id); }
  [[nodiscard]] auto GetFuncArgs() const -> List<Expression>*
  {
    return m_expressionValue.name.funcArgs;
  }
};

[[nodiscard]] auto Bind(const List<Expression>* formals,
                        const List<Expression>* values,
                        SymbolTable<Value>& symbolTable) -> bool;
[[nodiscard]] auto Conforms(const List<Expression>* formals, const List<Expression>* values)
    -> bool;
[[nodiscard]] auto Instantiate(const List<Expression>* before,
                               const SymbolTable<Value>& symbolTable) -> List<Expression>*;
[[nodiscard]] auto GetFloat(const SymbolTable<Value>& symbolTable,
                            const List<Expression>& expressionList,
                            float& fltValue,
                            unsigned int n = 0) -> bool;
[[nodiscard]] auto GetValue(const SymbolTable<Value>& symbolTable,
                            const List<Expression>& expressionList,
                            Value& value,
                            unsigned int n = 0) -> bool;

} // namespace LSys
