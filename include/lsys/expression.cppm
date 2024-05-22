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
 * $Log: Expression.h,v $
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

module;

#include <array>
#include <memory>
#include <ostream>

export module LSys.Expression;

import LSys.List;
import LSys.Name;
import LSys.SymbolTable;
import LSys.Value;

export namespace LSYS
{

class Expression
{
public:
  Expression(int operation, Expression* lop, Expression* rop);
  Expression(int operation, std::unique_ptr<Expression> lop, std::unique_ptr<Expression> rop);
  Expression(const Name& name, List<Expression>* funcArgs);
  explicit Expression(const Name& name);
  Expression(const Name& name, std::unique_ptr<List<Expression>> funcArgs);
  explicit Expression(const Value& value);
  Expression(const Expression& other);
  Expression(Expression&&) = default;
  ~Expression();

  auto operator=(const Expression&) -> Expression& = delete;
  auto operator=(Expression&&) -> Expression&      = default;

  // Access methods
  [[nodiscard]] auto GetType() const -> int { return m_operation; }
  [[nodiscard]] auto GetName() const -> Name;

  // Evaluation methods
  [[nodiscard]] auto Evaluate(const SymbolTable<Value>& symbolTable) const -> Value;
  [[nodiscard]] auto LEval(const SymbolTable<Value>& symbolTable) const -> Value;
  [[nodiscard]] auto REval(const SymbolTable<Value>& symbolTable) const -> Value;

  friend std::ostream& operator<<(std::ostream& out, const Expression& expression);

private:
  int m_operation;

  struct ExpressionValue
  {
    struct Name
    {
      int id{};
      std::unique_ptr<List<Expression>> funcArgs;
    };
    Name name{};
    Value value; // Ensure union is big enough for a Value
    std::array<std::unique_ptr<Expression>, 2> args{}; // Child expressions
  };
  ExpressionValue m_expressionValue{};
  [[nodiscard]] static auto GetArgs(const std::array<std::unique_ptr<Expression>, 2>& args) noexcept
      -> std::array<std::unique_ptr<Expression>, 2>;

  [[nodiscard]] auto GetVarName() const -> Name;
  [[nodiscard]] auto GetValue() const -> Value;
  [[nodiscard]] auto GetLChild() const -> Expression*;
  [[nodiscard]] auto GetRChild() const -> Expression*;
  [[nodiscard]] auto GetFuncName() const -> Name;
  [[nodiscard]] auto GetFuncArgs() const -> List<Expression>*;
};

std::ostream& operator<<(std::ostream& out, const Expression& expression);

[[nodiscard]] auto Bind(const List<Expression>* formals,
                        const List<Expression>* values,
                        SymbolTable<Value>& symbolTable) -> bool;
[[nodiscard]] auto Conforms(const List<Expression>* formals, const List<Expression>* values)
    -> bool;
//TODO(glk) Use unique_ptr.
[[nodiscard]] auto Instantiate(const List<Expression>* before,
                               const SymbolTable<Value>& symbolTable) noexcept
    -> std::unique_ptr<List<Expression>>;
[[nodiscard]] auto GetFloat(const SymbolTable<Value>& symbolTable,
                            const List<Expression>& expressionList,
                            float& fltValue,
                            unsigned int n = 0) -> bool;
[[nodiscard]] auto GetValue(const SymbolTable<Value>& symbolTable,
                            const List<Expression>& expressionList,
                            Value& value,
                            unsigned int n = 0) -> bool;

} // namespace LSYS

namespace LSYS
{

inline auto Expression::LEval(const SymbolTable<Value>& symbolTable) const -> Value
{
  return GetLChild()->Evaluate(symbolTable);
}

inline auto Expression::REval(const SymbolTable<Value>& symbolTable) const -> Value
{
  return GetRChild()->Evaluate(symbolTable);
}

inline auto Expression::GetVarName() const -> Name
{
  return Name{m_expressionValue.name.id};
}

inline auto Expression::GetValue() const -> Value
{
  return m_expressionValue.value;
}

inline auto Expression::GetLChild() const -> Expression*
{
  return m_expressionValue.args[0].get();
}

inline auto Expression::GetRChild() const -> Expression*
{
  return m_expressionValue.args[1].get();
}

inline auto Expression::GetFuncName() const -> Name
{
  return Name{m_expressionValue.name.id};
}

inline auto Expression::GetFuncArgs() const -> List<Expression>*
{
  return m_expressionValue.name.funcArgs.get();
}

} // namespace LSYS
