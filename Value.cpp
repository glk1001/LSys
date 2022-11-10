/* Value.c - methods for mixed int/float arithmetic in Expressions
 *  with runtime typing.
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
 * $Log:	Value.c,v $
 * Revision 1.6  95/05/24  17:14:48  leech
 * Fix for const-correctness.
 *
 * Revision 1.5  92/06/22  00:21:49  leech
 * Make sure switch(enum type) has a default to suppress warning
 * messages.
 *
 * Revision 1.4  91/10/10  19:55:37  leech
 * Added Value::Value(extended) for MPW, in which constants are of type
 * extended by default.
 *
 * Revision 1.3  91/03/20  10:36:06  leech
 * Support for G++.
 *
 * Revision 1.2  90/10/12  18:48:14  leech
 * First public release.
 *
 */
//static char RCSid[] = "$Id: Value.c,v 1.6 95/05/24 17:14:48 leech Exp $";

#include "Value.h"

#include <cmath>
#include <iostream>

using std::cerr;
using std::endl;


namespace LSys
{

// If not initialized to a value, has an undefined value
Value::Value() : type{undefinedType}, val{}
{
}

Value::Value(const bool value) : type{intType}
{
  val.ival = value != 0;
}

Value::Value(const int value) : type{intType}
{
  val.ival = value;
}

Value::Value(const float value) : type{floatType}
{
  val.fval = value;
}

Value::Value(const double value) : type{floatType}
{
  val.fval = static_cast<float>(value);
}

Value::Optype Value::binary_optype(const Value& value) const
{
  if (type == intType)
  {
    if (value.type == intType)
      return II;
    else if (value.type == floatType)
      return IF;
  }
  else if (type == floatType)
  {
    if (value.type == intType)
      return FI;
    else if (value.type == floatType)
      return FF;
  }
  return UNDEF;
}

// Unary negation
Value Value::operator-() const
{
  switch (type)
  {
    case intType:
      return Value(-val.ival);
    case floatType:
      return Value(-val.fval);
    case undefinedType:
    default:
      return *this;
  }
}

// Unary bitwise complement
Value Value::operator~() const
{
  switch (type)
  {
    case intType:
      return Value(~val.ival);
    case floatType:
      cerr << "Value::operator~(): cannot complement non-integer GetFloatValue\n";
      return Value();
    case undefinedType:
    default:
      return Value();
  }
}

// Unary logical complement
Value Value::operator!() const
{
  switch (type)
  {
    case intType:
      return Value(!val.ival);
    case floatType:
      cerr << "Value::operator!(): cannot complement non-integer GetFloatValue\n";
      return Value();
    case undefinedType:
    default:
      return Value();
  }
}

// Absolute value
Value Value::abs() const
{
  switch (type)
  {
    case intType:
      return Value(val.ival > 0 ? val.ival : -val.ival);
    case floatType:
      return Value(std::fabs(val.fval));
    case undefinedType:
    default:
      return Value();
  }
}

// Binary bitwise AND
Value Value::operator&(const Value& value) const
{
  switch (binary_optype(value))
  {
    case II:
      return Value(val.ival & value.val.ival);
    case IF:
    case FI:
    case FF:
      cerr << "Value::operator&(): cannot bitwise AND non-integer values" << endl;
      return Value();
    case UNDEF:
    default:
      return Value();
  }
}

// Binary bitwise OR
Value Value::operator|(const Value& value) const
{
  switch (binary_optype(value))
  {
    case II:
      return Value(val.ival | value.val.ival);
    case IF:
    case FI:
    case FF:
      cerr << "Value::operator|(): cannot bitwise OR non-integer values" << endl;
      return Value();
    case UNDEF:
    default:
      return Value();
  }
}

// Binary logical AND
Value Value::operator&&(const Value& value) const
{
  switch (binary_optype(value))
  {
    case II:
      return Value(val.ival && value.val.ival);
    case IF:
    case FI:
    case FF:
      cerr << "Value::operator&&(): cannot logical AND non-integer values" << endl;
      return Value();
    case UNDEF:
    default:
      return Value();
  }
}

// Binary logical OR
Value Value::operator||(const Value& value) const
{
  switch (binary_optype(value))
  {
    case II:
      return Value(val.ival || value.val.ival);
    case IF:
    case FI:
    case FF:
      cerr << "Value::operator||(): cannot logical OR non-integer values" << endl;
      return Value();
    case UNDEF:
    default:
      return Value();
  }
}

// Equality
Value Value::operator==(const Value& value) const
{
  switch (binary_optype(value))
  {
    case II:
      return Value(val.ival == value.val.ival);
    case IF:
      return Value(static_cast<float>(val.ival) == value.val.fval);
    case FI:
      return Value(val.fval == static_cast<float>(value.val.ival));
    case FF:
      return Value(val.fval == value.val.fval);
    case UNDEF:
    default:
      return Value();
  }
}

// Inequality
Value Value::operator!=(const Value& value) const
{
  switch (binary_optype(value))
  {
    case II:
      return Value(val.ival != value.val.ival);
    case IF:
      return Value(static_cast<float>(val.ival) != value.val.fval);
    case FI:
      return Value(val.fval != static_cast<float>(value.val.ival));
    case FF:
      return Value(val.fval != value.val.fval);
    case UNDEF:
    default:
      return Value();
  }
}

// Less-than
Value Value::operator<(const Value& value) const
{
  switch (binary_optype(value))
  {
    case II:
      return Value(val.ival < value.val.ival);
    case IF:
      return Value(static_cast<float>(val.ival) < value.val.fval);
    case FI:
      return Value(val.fval < static_cast<float>(value.val.ival));
    case FF:
      return Value(val.fval < value.val.fval);
    case UNDEF:
    default:
      return Value();
  }
}

// Less-than or equal-to
Value Value::operator<=(const Value& value) const
{
  switch (binary_optype(value))
  {
    case II:
      return Value(val.ival <= value.val.ival);
    case IF:
      return Value(static_cast<float>(val.ival) <= value.val.fval);
    case FI:
      return Value(val.fval <= static_cast<float>(value.val.ival));
    case FF:
      return Value(val.fval <= value.val.fval);
    case UNDEF:
    default:
      return Value();
  }
}

// Greater-than or equal-to
Value Value::operator>=(const Value& value) const
{
  switch (binary_optype(value))
  {
    case II:
      return Value(val.ival >= value.val.ival);
    case IF:
      return Value(static_cast<float>(val.ival) >= value.val.fval);
    case FI:
      return Value(val.fval >= static_cast<float>(value.val.ival));
    case FF:
      return Value(val.fval >= value.val.fval);
    case UNDEF:
    default:
      return Value();
  }
}

// Greater-than
Value Value::operator>(const Value& value) const
{
  switch (binary_optype(value))
  {
    case II:
      return Value(val.ival > value.val.ival);
    case IF:
      return Value(static_cast<float>(val.ival) > value.val.fval);
    case FI:
      return Value(val.fval > static_cast<float>(value.val.ival));
    case FF:
      return Value(val.fval > value.val.fval);
    case UNDEF:
    default:
      return Value();
  }
}

// Binary addition
Value Value::operator+(const Value& value) const
{
  switch (binary_optype(value))
  {
    case II:
      return Value(val.ival + value.val.ival);
    case IF:
      return Value(static_cast<float>(val.ival) + value.val.fval);
    case FI:
      return Value(val.fval + static_cast<float>(value.val.ival));
    case FF:
      return Value(val.fval + value.val.fval);
    case UNDEF:
    default:
      return Value();
  }
}

// Subtraction
Value Value::operator-(const Value& value) const
{
  switch (binary_optype(value))
  {
    case II:
      return Value(val.ival - value.val.ival);
    case IF:
      return Value(static_cast<float>(val.ival) - value.val.fval);
    case FI:
      return Value(val.fval - static_cast<float>(value.val.ival));
    case FF:
      return Value(val.fval - value.val.fval);
    case UNDEF:
    default:
      return Value();
  }
}

// Multiplication
Value Value::operator*(const Value& value) const
{
  switch (binary_optype(value))
  {
    case II:
      return Value(val.ival * value.val.ival);
    case IF:
      return Value(static_cast<float>(val.ival) * value.val.fval);
    case FI:
      return Value(val.fval * static_cast<float>(value.val.ival));
    case FF:
      return Value(val.fval * value.val.fval);
    case UNDEF:
    default:
      return Value();
  }
}

// Division (checks for /0 and returns an undefined number)
// Note that int / int -> float, unlike C rules
Value Value::operator/(const Value& value) const
{
  if (type == undefinedType || value.type == undefinedType)
  {
    return Value();
  }

  auto divisor = (value.type == intType) ? static_cast<float>(value.val.ival) : value.val.fval;
  if (divisor == 0.0F)
  {
    return Value();
  }

  const auto dividend = (type == intType) ? static_cast<float>(val.ival) : val.fval;

  return Value(dividend / divisor);
}

// Modulo (checks for /0 and returns an undefined number)
Value Value::operator%(const Value& value) const
{
  switch (binary_optype(value))
  {
    case II:
      if (value.val.ival == 0)
        return Value();
      else
        return Value(val.ival % value.val.ival);
    case IF:
    case FI:
    case FF:
      cerr << "Value::operator%(): cannot compute modulo of non-integer values\n";
      return Value();
    case UNDEF:
    default:
      return Value();
  }
}

// Power
Value Value::operator^(const Value& value) const
{
  switch (binary_optype(value))
  {
    case II:
      return Value(std::pow(double(val.ival), value.val.ival));
    case IF:
      return Value(std::pow(float(val.ival), value.val.fval));
    case FI:
      return Value(std::pow(val.fval, value.val.ival));
    case FF:
      return Value(std::pow(val.fval, value.val.fval));
    case UNDEF:
    default:
      return Value();
  }
}

// Returns numeric value in i, true if a valid integer
bool Value::GetIntValue(int& value) const
{
  if (type == intType)
  {
    value = val.ival;
    return true;
  }

  return false;
}

bool Value::GetFloatValue(float& value) const
{
  if (type == intType)
  {
    value = static_cast<float>(val.ival);
    return true;
  }
  if (type == floatType)
  {
    value = val.fval;
    return true;
  }

  return false;
}

std::ostream& operator<<(std::ostream& o, const Value& value)
{
  switch (value.type)
  {
    case intType:
      o << value.val.ival;
      break;
    case floatType:
      o.setf(std::ios::showpoint);
      o << value.val.fval;
      o.unsetf(std::ios::showpoint);
      break;
    case undefinedType:
    default:
      o << "(undefined value)";
      break;
  }
  return o;
}

} // namespace LSys
