/* Expression.c - methods to evaluate arithmetic expressions.
 *
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
//static char RCSid[]= "$Id: Expression.c,v 1.5 95/05/24 17:12:51 leech Exp $";

#include "Expression.h"

#include "Rand.h"
#include "SymbolTable.h"
#include "Value.h"
#include "Vector.h"
#include "debug.h"
#include "token.h"

#include <cmath>
#include <ctime>

using std::cerr;
using std::endl;


namespace LSys
{


#define EXPRFUNC(name) \
  Value name(const SymbolTable<Value>& symbolTable, List<Expression>& expressionList)
typedef EXPRFUNC((*Exprfunc));


[[nodiscard]] auto expr_sin(const SymbolTable<Value>& symbolTable, List<Expression>& expressionList)
    -> Value
{
  if (auto x = 0.0F; GetFloat(symbolTable, expressionList, x))
  {
    return Value(std::sin(Maths::ToRadians(x)));
  }
  return Value();
}


[[nodiscard]] auto expr_cos(const SymbolTable<Value>& symbolTable, List<Expression>& expressionList)
    -> Value
{
  if (auto x = 0.0F; GetFloat(symbolTable, expressionList, x))
  {
    return Value(std::cos(Maths::ToRadians(x)));
  }
  return Value();
}


[[nodiscard]] auto expr_tan(const SymbolTable<Value>& symbolTable, List<Expression>& expressionList)
    -> Value
{
  if (auto x = 0.0F; GetFloat(symbolTable, expressionList, x))
  {
    return Value(std::tan(Maths::ToRadians(x)));
  }
  return Value();
}


[[nodiscard]] auto expr_asin(const SymbolTable<Value>& symbolTable,
                             List<Expression>& expressionList) -> Value
{
  if (auto x = 0.0F; GetFloat(symbolTable, expressionList, x))
  {
    return Value(Maths::ToDegrees(std::asin(x)));
  }
  return Value();
}


[[nodiscard]] auto expr_acos(const SymbolTable<Value>& symbolTable,
                             List<Expression>& expressionList) -> Value
{
  if (auto x = 0.0F; GetFloat(symbolTable, expressionList, x))
  {
    return Value(Maths::ToDegrees(std::acos(x)));
  }
  return Value();
}


[[nodiscard]] auto expr_atan(const SymbolTable<Value>& symbolTable,
                             List<Expression>& expressionList) -> Value
{
  if (auto x = 0.0F; GetFloat(symbolTable, expressionList, x))
  {
    return Value(Maths::ToDegrees(std::atan(x)));
  }
  return Value();
}


[[nodiscard]] auto expr_atan2(const SymbolTable<Value>& symbolTable,
                              List<Expression>& expressionList) -> Value
{
  auto x = 0.0F;
  auto y = 0.0F;
  if (GetFloat(symbolTable, expressionList, y) and GetFloat(symbolTable, expressionList, x, 1))
  {
    return Value(Maths::ToDegrees(std::atan2(y, x)));
  }
  return Value();
}


// Returns type of argument
[[nodiscard]] auto expr_abs(const SymbolTable<Value>& symbolTable, List<Expression>& expressionList)
    -> Value
{
  if (auto v = Value{}; GetValue(symbolTable, expressionList, v))
  {
    return v.abs();
  }
  return Value();
}


// Always returns int
[[nodiscard]] auto expr_ceil(const SymbolTable<Value>& symbolTable,
                             List<Expression>& expressionList) -> Value
{
  if (auto x = 0.0F; GetFloat(symbolTable, expressionList, x))
  {
    return Value(static_cast<int>(std::ceil(x)));
  }
  return Value();
}


// Always returns int
[[nodiscard]] auto expr_floor(const SymbolTable<Value>& symbolTable,
                              List<Expression>& expressionList) -> Value
{
  if (auto x = 0.0F; GetFloat(symbolTable, expressionList, x))
  {
    return Value(static_cast<int>(std::floor(x)));
  }
  return Value();
}


[[nodiscard]] auto expr_exp(const SymbolTable<Value>& symbolTable, List<Expression>& expressionList)
    -> Value
{
  if (auto x = 0.0F; GetFloat(symbolTable, expressionList, x))
  {
    return Value(std::exp(x));
  }
  return Value();
}


[[nodiscard]] auto expr_log(const SymbolTable<Value>& symbolTable, List<Expression>& expressionList)
    -> Value
{
  if (auto x = 0.0F; GetFloat(symbolTable, expressionList, x))
  {
    return Value(std::log(x));
  }
  return Value();
}


[[nodiscard]] auto expr_log10(const SymbolTable<Value>& symbolTable,
                              List<Expression>& expressionList) -> Value
{
  if (auto x = 0.0F; GetFloat(symbolTable, expressionList, x))
  {
    return Value(std::log10(x));
  }
  return Value();
}


// Return a uniformly distributed random number;
//  rand()  returns [0,1)
//  rand(n) returns [0,n)   (floating point)
[[nodiscard]] auto expr_rand(const SymbolTable<Value>& symbolTable,
                             List<Expression>& expressionList) -> Value
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
[[nodiscard]] auto expr_srand(const SymbolTable<Value>& symbolTable,
                              List<Expression>& expressionList) -> Value
{
  auto seed = 0L;
  // Should have getint()
  if (auto x = 0.0F; GetFloat(symbolTable, expressionList, x))
  {
    seed = static_cast<long>(x);
  }
  else
  {
    seed = ::time(nullptr);
  }

  srand48(static_cast<uint32_t>(seed));
  return Value(static_cast<int>(seed));
}

// The function table for function terms in expressions
static SymbolTable<Anyptr>* funcTab = nullptr;

static auto SetupFunctions() -> void
{
  if (funcTab != nullptr)
  {
    return;
  }

  funcTab = new SymbolTable<Anyptr>;
  funcTab->enter("sin", Anyptr(expr_sin));
  funcTab->enter("cos", Anyptr(expr_cos));
  funcTab->enter("tan", Anyptr(expr_tan));
  funcTab->enter("asin", Anyptr(expr_asin));
  funcTab->enter("acos", Anyptr(expr_acos));
  funcTab->enter("atan", Anyptr(expr_atan));
  funcTab->enter("atan2", Anyptr(expr_atan2));
  funcTab->enter("abs", Anyptr(expr_abs));
  funcTab->enter("ceil", Anyptr(expr_ceil));
  funcTab->enter("floor", Anyptr(expr_floor));
  funcTab->enter("exp", Anyptr(expr_exp));
  funcTab->enter("log", Anyptr(expr_log));
  funcTab->enter("log10", Anyptr(expr_log10));
  funcTab->enter("rand", Anyptr(expr_rand));
  funcTab->enter("srand", Anyptr(expr_srand));
}

[[nodiscard]] auto GetOpName(const int op) -> const char*
{
  static char str[] = " ";

  switch (op)
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
      str[0] = static_cast<char>(op);
      return str;
  }
}

std::ostream& operator<<(std::ostream& o, const Expression& expression)
{
  switch (expression.op)
  {
    case LSYS_NAME:
      o << expression.GetVarName();
      break;
    case LSYS_FUNCTION:
      o << expression.GetFuncName() << *expression.GetFuncArgs();
      break;
    case LSYS_VALUE:
      o << expression.GetValue();
      break;
    case LSYS_UMINUS:
    case '!':
    case '~':
      o << GetOpName(expression.op) << *expression.GetLChild();
      break;
    default:
      o << '(' << *expression.GetLChild() << GetOpName(expression.op) << *expression.GetRChild()
        << ')';
      break;
  }
  return o;
}

Expression::Expression(const int o, Expression* const lop, Expression* const rop) : op{o}
{
  PDebug(PD_EXPRESSION,
         cerr << "Creating expression w/op " << o << "='" << static_cast<char>(o) << "'"
              << " &lhs= " << lop << " &rhs= " << rop << "\n");

  m_expressionValue.args[0] = lop;
  m_expressionValue.args[1] = rop;
}

// Create a function call node, or a named variable node if there are
//  no arguments.
Expression::Expression(const Name& name, List<Expression>* const funcArgs)
{
  if (nullptr == funcArgs)
  {
    PDebug(PD_EXPRESSION,
           cerr << "Creating expression w/op " << LSYS_NAME << " GetName " << name << endl);

    op            = LSYS_NAME;
    m_expressionValue.name.id   = name;
    m_expressionValue.name.args = nullptr;
  }
  else
  {
    PDebug(PD_EXPRESSION,
           cerr << "Creating expression w/op " << LSYS_FUNCTION << " function " << name << *funcArgs
                << endl);

    op            = LSYS_FUNCTION;
    m_expressionValue.name.id   = name;
    m_expressionValue.name.args = funcArgs;
  }
}


// Create a value node
Expression::Expression(const Value& value) : op{LSYS_VALUE}
{
  PDebug(PD_EXPRESSION,
         cerr << "Creating expression w/op " << LSYS_VALUE << " GetValue " << value << endl);

  m_expressionValue.value = value;
}

Expression::~Expression()
{
  switch (op)
  {
    case LSYS_NAME:
      PDebug(PD_EXPRESSION, cerr << "Deleting expression::GetName (not dynamic)" << endl);
      break;
    case LSYS_FUNCTION:
      PDebug(PD_EXPRESSION,
             cerr << "Deleting expression::function " << GetVarName() << "args @ " << GetFuncArgs()
                  << endl);
      delete GetFuncArgs();
      break;
    case LSYS_VALUE:
      PDebug(PD_EXPRESSION, cerr << "Deleting expression::GetValue (not dynamic)" << endl);
      break;
    default:
      PDebug(PD_EXPRESSION,
             cerr << "Deleting expression::op= " << op << " kids @ " << GetLChild() << " @ "
                  << GetRChild() << "\n");
      delete GetLChild();
      delete GetRChild();
      break;
  }
}

auto Expression::GetName() const -> Name
{
  static const Name s_BOGUS{"[Not a Name]"};
  return (op == LSYS_NAME) ? GetVarName() : s_BOGUS;
}


auto Expression::Evaluate(const SymbolTable<Value>& symbolTable) const -> Value
{
  Value value;
  Anyptr anyPtr;

  switch (op)
  {
    case LSYS_VALUE:
      return GetValue();

    case LSYS_FUNCTION:
      if (nullptr == funcTab)
      {
        SetupFunctions();
      }
      if (funcTab->lookup(GetFuncName(), anyPtr))
      {
        const auto func = reinterpret_cast<Exprfunc>(anyPtr);
        return (*func)(symbolTable, *GetFuncArgs());
      }
      cerr << "Unimplemented function '" << GetFuncName() << "'\n";
      return Value();

    case LSYS_NAME:
      if (symbolTable.lookup(GetVarName(), value))
      {
        return value;
      }
      cerr << "Fatal error in Expression::Evaluate: no bound variable '" << GetVarName() << "'\n";
      return Value();

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
      cerr << "Fatal error in Expression::Evaluate: unrecognized operator '"
           << static_cast<char>(op) << "'= " << op << "\n";
      return Value();
  }
}

// Bind symbolic names of the list to values in list 'values'
// using symbol table st for evaluation and binding. The two
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
    cerr << "Bind: formal and GetValue lists are not the same length\n";
    return false;
  }

  auto left  = ConstListIterator<Expression>{*formals};
  auto right = ConstListIterator<Expression>{*values};
  const Expression* lp;
  const Expression* rp;

  for (lp = left.first(), rp = right.first(); (lp != nullptr) and (rp != nullptr);
       lp = left.next(), rp = right.next())
  {
    if (lp->GetType() != LSYS_NAME)
    {
      cerr << "Bind: left expression " << *lp << " is not a formal\n";
      return false;
    }
    if (rp->GetType() != LSYS_VALUE)
    {
      cerr << "Bind: right expression " << *rp << " is not a GetValue\n";
      return false;
    }
    const Value v = rp->Evaluate(symbolTable);
    PDebug(PD_EXPRESSION, cerr << "Binding " << lp->GetName() << "= " << v << "\n");
    symbolTable.enter(lp->GetName(), v);
  }

  return true;
}

// Check if list 'el' is conformant with the list, e.g.,
// that they have the same number of expressions.
auto Conforms(const List<Expression>* const formals, const List<Expression>* const values) -> bool
{
  const auto fSize = (formals != nullptr) ? formals->size() : 0U;
  const auto vSize = (values != nullptr) ? values->size() : 0U;
  return fSize == vSize;
}

// Instantiate the list; that is, return a copy with all of the
// expressions evaluated in the context of the symbol table.
// Return a NULL expression list if a NULL list is passed
auto Instantiate(const List<Expression>* const before, const SymbolTable<Value>& symbolTable)
    -> List<Expression>*
{
  if (nullptr == before)
  {
    return nullptr;
  }

  auto* const new_el = new List<Expression>;
  auto ei            = ConstListIterator<Expression>{*before};

  for (const auto* e = ei.first(); e != nullptr; e = ei.next())
  {
    auto* const new_e = new Expression(e->Evaluate(symbolTable));
    new_el->append(new_e);
  }

  return new_el;
}

// Return the n'th (0 base) value in the list, if available.
// Evaluates the expression against specified symbol table; if the
// the expression must be a bound value, supply an empty symbol table.
// Returns true on success, false if the list does not have enough
// parameters or the parameter is not a number.
// This should be wrapped around a method of class List(Expression),
// but that's a pain with the template method used.
auto GetValue(const SymbolTable<Value>& symbolTable,
              const List<Expression>& expressionList,
              Value& val,
              unsigned int n) -> bool
{
  // Ensure there are enough parameters
  if (n >= expressionList.size())
  {
    return false;
  }

  auto ei = ConstListIterator<Expression>{expressionList};

  // Fetch the n'th expression in the list
  const Expression* e = nullptr;
  for (e = ei.first(); n > 0; --n)
  {
    e = ei.next();
  }

  // Should be an assertion
  if (nullptr == e)
  {
    return false;
  }

  // Evaluate the expression. Since all expressions should have reduced
  //   to bound values by now, an empty symbol table is used.
  val = e->Evaluate(symbolTable);

  return true;
}

auto GetFloat(const SymbolTable<Value>& symbolTable,
              const List<Expression>& expressionList,
              float& val,
              const unsigned int n) -> bool
{
  if (Value v; GetValue(symbolTable, expressionList, v, n))
  {
    return v.GetFloatValue(val);
  }
  return false;
}

} // namespace LSys
