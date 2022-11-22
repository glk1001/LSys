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
 * $Log: Expression.c,v $
 * Revision 1.5  95/05/24  17:12:51  leech
 * Fix for const-correctness.
 *
 * Revision 1.4  1993/05/12  22:06:24  leech
 * Reduce warnings from cfront 3.0.1.
 *
 * Revision 1.3  91/03/20  10:35:16  leech
 * Support for G++.
 *
 * Revision 1.2  90/10/12  18:48:03  leech
 * First public release.
 *
 */

#include "expression.h"

#include "debug.h"
#include "rand.h"
#include "symbol_table.h"
#include "token.h"
#include "value.h"
#include "vector.h"

#include <cmath>
#include <ctime>
#include <functional>

namespace L_SYSTEM
{

namespace
{

using ExprFunc =
    std::function<Value(const SymbolTable<Value>& symbolTable, List<Expression>& expressionList)>;

[[nodiscard]] auto ExprSin(const SymbolTable<Value>& symbolTable, List<Expression>& expressionList)
    -> Value
{
  if (auto x = 0.0F; GetFloat(symbolTable, expressionList, x))
  {
    return Value(std::sin(MATHS::ToRadians(x)));
  }
  return Value{};
}

[[nodiscard]] auto ExprCos(const SymbolTable<Value>& symbolTable, List<Expression>& expressionList)
    -> Value
{
  if (auto x = 0.0F; GetFloat(symbolTable, expressionList, x))
  {
    return Value(std::cos(MATHS::ToRadians(x)));
  }
  return Value{};
}

[[nodiscard]] auto ExprTan(const SymbolTable<Value>& symbolTable, List<Expression>& expressionList)
    -> Value
{
  if (auto x = 0.0F; GetFloat(symbolTable, expressionList, x))
  {
    return Value(std::tan(MATHS::ToRadians(x)));
  }
  return Value{};
}

[[nodiscard]] auto ExprASin(const SymbolTable<Value>& symbolTable, List<Expression>& expressionList)
    -> Value
{
  if (auto x = 0.0F; GetFloat(symbolTable, expressionList, x))
  {
    return Value(MATHS::ToDegrees(std::asin(x)));
  }
  return Value{};
}

[[nodiscard]] auto ExprACos(const SymbolTable<Value>& symbolTable, List<Expression>& expressionList)
    -> Value
{
  if (auto x = 0.0F; GetFloat(symbolTable, expressionList, x))
  {
    return Value(MATHS::ToDegrees(std::acos(x)));
  }
  return Value{};
}

[[nodiscard]] auto ExprATan(const SymbolTable<Value>& symbolTable, List<Expression>& expressionList)
    -> Value
{
  if (auto x = 0.0F; GetFloat(symbolTable, expressionList, x))
  {
    return Value(MATHS::ToDegrees(std::atan(x)));
  }
  return Value{};
}

[[nodiscard]] auto ExprATan2(const SymbolTable<Value>& symbolTable,
                             List<Expression>& expressionList) -> Value
{
  auto x = 0.0F;
  auto y = 0.0F;
  if (GetFloat(symbolTable, expressionList, y) and GetFloat(symbolTable, expressionList, x, 1))
  {
    return Value(MATHS::ToDegrees(std::atan2(y, x)));
  }
  return Value{};
}

// Returns type of argument
[[nodiscard]] auto ExprAbs(const SymbolTable<Value>& symbolTable, List<Expression>& expressionList)
    -> Value
{
  if (auto v = Value{}; GetValue(symbolTable, expressionList, v))
  {
    return v.Abs();
  }
  return Value{};
}

// Always returns int
[[nodiscard]] auto ExprCeil(const SymbolTable<Value>& symbolTable, List<Expression>& expressionList)
    -> Value
{
  if (auto x = 0.0F; GetFloat(symbolTable, expressionList, x))
  {
    return Value(static_cast<int>(std::ceil(x)));
  }
  return Value{};
}

// Always returns int
[[nodiscard]] auto ExprFloor(const SymbolTable<Value>& symbolTable,
                             List<Expression>& expressionList) -> Value
{
  if (auto x = 0.0F; GetFloat(symbolTable, expressionList, x))
  {
    return Value(static_cast<int>(std::floor(x)));
  }
  return Value{};
}

[[nodiscard]] auto ExprExp(const SymbolTable<Value>& symbolTable, List<Expression>& expressionList)
    -> Value
{
  if (auto x = 0.0F; GetFloat(symbolTable, expressionList, x))
  {
    return Value(std::exp(x));
  }
  return Value{};
}

[[nodiscard]] auto ExprLog(const SymbolTable<Value>& symbolTable, List<Expression>& expressionList)
    -> Value
{
  if (auto x = 0.0F; GetFloat(symbolTable, expressionList, x))
  {
    return Value(std::log(x));
  }
  return Value{};
}

[[nodiscard]] auto ExprLog10(const SymbolTable<Value>& symbolTable,
                             List<Expression>& expressionList) -> Value
{
  if (auto x = 0.0F; GetFloat(symbolTable, expressionList, x))
  {
    return Value(std::log10(x));
  }
  return Value{};
}

// Return a uniformly distributed random number;
//  rand()  returns [0,1)
//  rand(n) returns [0,n)   (floating point)
[[nodiscard]] auto ExprRand(const SymbolTable<Value>& symbolTable, List<Expression>& expressionList)
    -> Value
{
  if (auto x = 0.0F; GetFloat(symbolTable, expressionList, x))
  {
    return Value(static_cast<double>(x) * drand48());
  }
  return Value(drand48());
}

// Reseed the random number generator and return the seed;
//  srand() sets the seed to the current time, while
//  srand(value) sets it to the specified value.
[[nodiscard]] auto ExprSRand(const SymbolTable<Value>& symbolTable,
                             List<Expression>& expressionList) -> Value
{
  int64_t seed;
  // Should have getint()
  if (auto x = 0.0F; GetFloat(symbolTable, expressionList, x))
  {
    seed = static_cast<int64_t>(x);
  }
  else
  {
    seed = ::time(nullptr);
  }

  srand48(static_cast<uint32_t>(seed));
  return Value(static_cast<int>(seed));
}

// The function table for function terms in expressions.
auto GetFunctionSymbolTable() -> SymbolTable<ExprFunc>
{
  auto symbolTable = SymbolTable<ExprFunc>{};

  symbolTable.Enter("sin", ExprSin);
  symbolTable.Enter("cos", ExprCos);
  symbolTable.Enter("tan", ExprTan);
  symbolTable.Enter("asin", ExprASin);
  symbolTable.Enter("acos", ExprACos);
  symbolTable.Enter("atan", ExprATan);
  symbolTable.Enter("atan2", ExprATan2);
  symbolTable.Enter("abs", ExprAbs);
  symbolTable.Enter("ceil", ExprCeil);
  symbolTable.Enter("floor", ExprFloor);
  symbolTable.Enter("exp", ExprExp);
  symbolTable.Enter("log", ExprLog);
  symbolTable.Enter("log10", ExprLog10);
  symbolTable.Enter("rand", ExprRand);
  symbolTable.Enter("srand", ExprSRand);

  return symbolTable;
}

const auto functionSymbolTable = GetFunctionSymbolTable();

[[nodiscard]] auto GetOpName(const int operation) -> const char*
{
  switch (operation)
  {
    case LSYS_UMINUS:
      return "-";
    case '!':
      return "!";
    case '~':
      return "~";
    case LSYS_AND:
      return "&&";
    case LSYS_OR:
      return "||";
    case LSYS_EQ:
      return "==";
    case LSYS_NE:
      return "!=";
    case LSYS_LE:
      return "<=";
    case LSYS_GE:
      return ">=";
    default:
      static char s_str[] = " ";
      s_str[0]            = static_cast<char>(operation);
      return s_str;
  }
}

} // namespace

std::ostream& operator<<(std::ostream& out, const Expression& expression)
{
  switch (expression.m_operation)
  {
    case LSYS_NAME:
      out << expression.GetVarName();
      break;
    case LSYS_FUNCTION:
      out << expression.GetFuncName() << *expression.GetFuncArgs();
      break;
    case LSYS_VALUE:
      out << expression.GetValue();
      break;
    case LSYS_UMINUS:
    case '!':
    case '~':
      out << GetOpName(expression.m_operation) << *expression.GetLChild();
      break;
    default:
      out << '(';
      if (expression.GetLChild() == nullptr)
      {
        out << '(' << " nullptr ";
      }
      else
      {
        out << *expression.GetLChild();
      }
      out << GetOpName(expression.m_operation);
      if (expression.GetRChild() == nullptr)
      {
        out << '(' << " nullptr ";
      }
      else
      {
        out << *expression.GetRChild();
      }
      out << ')';
      break;
  }

  return out;
}

Expression::Expression(const int operation, Expression* const lop, Expression* const rop)
  : m_operation{operation}
{
  PDebug(PD_EXPRESSION,
         std::cerr << "Creating expression w/op " << operation << "='"
                   << static_cast<char>(operation) << "'"
                   << " &lhs= " << *lop << " &rhs= " << *rop << "\n");

  m_expressionValue.args[0].reset(lop);
  m_expressionValue.args[1].reset(rop);
}

Expression::Expression(const int operation,
                       std::unique_ptr<Expression> lop,
                       std::unique_ptr<Expression> rop)
  : m_operation{operation}
{
  PDebug(PD_EXPRESSION,
         std::cerr << "Creating expression w/op " << operation << "='"
                   << static_cast<char>(operation) << "'"
                   << " &lhs= " << *lop << " &rhs= " << *rop << "\n");

  m_expressionValue.args[0] = std::move(lop);
  m_expressionValue.args[1] = std::move(rop);
}

// Create a named variable node.
Expression::Expression(const Name& name)
  : m_operation{LSYS_NAME},
    m_expressionValue{
      {name.id(), nullptr},
      {},
      {},
}
{
  PDebug(PD_EXPRESSION,
         std::cerr << "Creating expression w/op " << LSYS_NAME << " GetName " << name << "\n");
}

Expression::Expression(const Name& name, List<Expression>* funcArgs)
  : m_operation{funcArgs == nullptr ? LSYS_NAME : LSYS_FUNCTION},
    m_expressionValue{
        {name.id(), {funcArgs == nullptr ? nullptr :
             std::unique_ptr<List<Expression>>{funcArgs}}},
        {},
        {},
    }
{
  if (m_operation == LSYS_NAME)
  {
    PDebug(PD_EXPRESSION,
           std::cerr << "Creating expression w/op " << LSYS_NAME << " GetName " << name << "\n");
  }
  else
  {
    PDebug(PD_EXPRESSION,
           std::cerr << "Creating expression w/op " << LSYS_FUNCTION << " function " << name
                     << *funcArgs << "\n");
  }
}

// Create a function call node.
Expression::Expression(const Name& name, std::unique_ptr<List<Expression>> funcArgs)
  : m_operation{LSYS_FUNCTION},
    m_expressionValue{      {name.id(), std::move(funcArgs)},
        {},
        {},
    }
{
  PDebug(PD_EXPRESSION,
         std::cerr << "Creating expression w/op " << LSYS_FUNCTION << " function " << name
                   << *funcArgs << "\n");
}

// Create a value node.
Expression::Expression(const Value& value)
  : m_operation{LSYS_VALUE},
    m_expressionValue{
        {},
        value,
        {},
    }
{
  PDebug(PD_EXPRESSION,
         std::cerr << "Creating expression w/op " << LSYS_VALUE << " GetValue " << value << "\n");
}

Expression::Expression(const Expression& other) : m_operation{other.m_operation},
    m_expressionValue{
  {other.m_expressionValue.name.id, nullptr}, other.m_expressionValue.value,
        GetArgs(other.m_expressionValue.args)}
{
  if (m_operation == LSYS_FUNCTION)
  {
    m_expressionValue.name.funcArgs =
        std::make_unique<List<Expression>>(*other.m_expressionValue.name.funcArgs);
  }
}

auto Expression::GetArgs(const std::array<std::unique_ptr<Expression>, 2>& args) noexcept
    -> std::array<std::unique_ptr<Expression>, 2>
{
  auto newArgs = std::array<std::unique_ptr<Expression>, 2>{
      args[0] == nullptr ? std::unique_ptr<Expression>{} : std::make_unique<Expression>(*args[1]),
      args[1] == nullptr ? std::unique_ptr<Expression>{} : std::make_unique<Expression>(*args[0]),
  };
  return newArgs;
}

Expression::~Expression() = default;

auto Expression::GetName() const -> Name
{
  static const Name s_BOGUS{"[Not a Name]"};
  return (m_operation == LSYS_NAME) ? GetVarName() : s_BOGUS;
}


auto Expression::Evaluate(const SymbolTable<Value>& symbolTable) const -> Value
{
  switch (m_operation)
  {
    case LSYS_VALUE:
      return GetValue();

    case LSYS_FUNCTION:
      if (ExprFunc exprFunc; functionSymbolTable.Lookup(GetFuncName().str(), exprFunc))
      {
        return exprFunc(symbolTable, *GetFuncArgs());
      }
      std::cerr << "Unimplemented function '" << GetFuncName() << "'\n";
      return Value{};

    case LSYS_NAME:
      if (Value value; symbolTable.Lookup(GetVarName().str(), value))
      {
        return value;
      }
      std::cerr << "Fatal error in Expression::Evaluate: no bound variable '" << GetVarName()
                << "'\n";
      return Value{};

    // Unary operators

    // Arithmetic
    case LSYS_UMINUS:
      return -LEval(symbolTable);

    // Bitwise complement
    case '~':
      return ~LEval(symbolTable);

    // Logical complement
    case '!':
      return !LEval(symbolTable);

    // Binary operators

    // Bitwise logical and, or
    case '&':
      return LEval(symbolTable) & REval(symbolTable);
    case '|':
      return LEval(symbolTable) | REval(symbolTable);

    // Logical and, or
    case LSYS_AND:
      return LEval(symbolTable) && REval(symbolTable);
    case LSYS_OR:
      return LEval(symbolTable) || REval(symbolTable);

    // Logical comparisons
    case LSYS_EQ:
      return LEval(symbolTable) == REval(symbolTable);
    case LSYS_NE:
      return LEval(symbolTable) != REval(symbolTable);
    case '<':
      return LEval(symbolTable) < REval(symbolTable);
    case LSYS_LE:
      return LEval(symbolTable) <= REval(symbolTable);
    case LSYS_GE:
      return LEval(symbolTable) >= REval(symbolTable);
    case '>':
      return LEval(symbolTable) > REval(symbolTable);

    // Arithmetic
    case '+':
      return LEval(symbolTable) + REval(symbolTable);
    case '-':
      return LEval(symbolTable) - REval(symbolTable);
    case '*':
      return LEval(symbolTable) * REval(symbolTable);
    case '/':
      return LEval(symbolTable) / REval(symbolTable);
    // Modulo
    case '%':
      return LEval(symbolTable) % REval(symbolTable);
    // Power, *not* XOR as in C
    case '^':
      return LEval(symbolTable) ^ REval(symbolTable);

    // Shouldn't get here
    default:
      std::cerr << "Fatal error in Expression::Evaluate: unrecognized operator '"
                << static_cast<char>(m_operation) << "'= " << m_operation << "\n";
      return Value{};
  }
}

// Bind symbolic names of the list to values in list 'values'
// using the symbol table for evaluation and binding. The two
// lists should conform() for this method to succeed.
// Returns true on success, false otherwise.
auto Bind(const List<Expression>* const formals,
          const List<Expression>* const values,
          SymbolTable<Value>& symbolTable) -> bool
{
  // No binding need be done if one list is NULL
  if ((nullptr == formals) or (nullptr == values))
  {
    return true;
  }

  if (not Conforms(formals, values))
  {
    std::cerr << "Bind: formal and GetValue lists are not the same length\n";
    return false;
  }

  auto left  = ConstListIterator<Expression>{*formals};
  auto right = ConstListIterator<Expression>{*values};
  const Expression* leftPtr;
  const Expression* rightPtr;

  for (leftPtr = left.first(), rightPtr = right.first();
       (leftPtr != nullptr) and (rightPtr != nullptr);
       leftPtr = left.next(), rightPtr = right.next())
  {
    if (leftPtr->GetType() != LSYS_NAME)
    {
      std::cerr << "Bind: left expression " << *leftPtr << " is not a formal\n";
      return false;
    }
    if (rightPtr->GetType() != LSYS_VALUE)
    {
      std::cerr << "Bind: right expression " << *rightPtr << " is not a value\n";
      return false;
    }

    const Value value = rightPtr->Evaluate(symbolTable);
    PDebug(PD_EXPRESSION, std::cerr << "Binding " << leftPtr->GetName() << "= " << value << "\n");
    symbolTable.Enter(leftPtr->GetName().str(), value);
  }

  return true;
}

// Check if list 'el' is conformant with the list, e.g.,
// that they have the same number of expressions.
auto Conforms(const List<Expression>* const formals, const List<Expression>* const values) -> bool
{
  const auto formalsSize = (formals == nullptr) ? 0U : formals->size();
  const auto valuesSize  = (values == nullptr) ? 0U : values->size();
  return formalsSize == valuesSize;
}

// Instantiate the list; that is, return a copy with all the
// expressions evaluated in the context of the symbol table.
// Return a NULL expression list if a NULL list is passed
auto Instantiate(const List<Expression>* const before,
                 const SymbolTable<Value>& symbolTable) noexcept
    -> std::unique_ptr<List<Expression>>
{
  if (nullptr == before)
  {
    return std::unique_ptr<List<Expression>>{};
  }

  auto newExprList = std::make_unique<List<Expression>>();
  auto exprIter    = ConstListIterator<Expression>{*before};

  for (const auto* expr = exprIter.first(); expr != nullptr; expr = exprIter.next())
  {
    auto newExpr = std::make_unique<Expression>(expr->Evaluate(symbolTable));
    newExprList->append(std::move(newExpr));
  }

  return newExprList;
}

// Return the nth (0 base) value in the list, if available.
// Evaluates the expression against specified symbol table; if the
// expression must be a bound value, supply an empty symbol table.
// Returns true on success, false if the list does not have enough
// parameters or the parameter is not a number.
// This should be wrapped around a method of class List(Expression),
// but that's a pain with the template method used.
auto GetValue(const SymbolTable<Value>& symbolTable,
              const List<Expression>& expressionList,
              Value& value,
              const unsigned int n) -> bool
{
  // Ensure there are the right number of parameters.
  if (n >= expressionList.size())
  {
    return false;
  }

  auto exprIter = ConstListIterator<Expression>{expressionList};

  // Fetch the nth expression in the list.
  const auto* expr = exprIter.first();
  for (auto i = 0U; i < n; ++i)
  {
    expr = exprIter.next();
  }

  // Should be an assertion
  if (nullptr == expr)
  {
    return false;
  }

  // Evaluate the expression. Since all expressions should have reduced
  //   to bound values by now, an empty symbol table is used.
  value = expr->Evaluate(symbolTable);

  return true;
}

auto GetFloat(const SymbolTable<Value>& symbolTable,
              const List<Expression>& expressionList,
              float& fltValue,
              const unsigned int n) -> bool
{
  if (Value value; GetValue(symbolTable, expressionList, value, n))
  {
    return value.GetFloatValue(fltValue);
  }
  return false;
}

} // namespace L_SYSTEM
