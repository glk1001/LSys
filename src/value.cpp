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
 * $Log: Value.c,v $
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

#include "value.h"

#include <cmath>
#include <iostream>

namespace LSYS
{

Value::Value() : m_type{ValueType::UNDEFINED}, m_val{}
{
}

Value::Value(const bool value) : m_type{ValueType::INT}
{
  m_val.intVal = value ? 1 : 0;
}

Value::Value(const int value) : m_type{ValueType::INT}
{
  m_val.intVal = value;
}

Value::Value(const float value) : m_type{ValueType::FLOAT}
{
  m_val.fltVal = value;
}

Value::Value(const double value) : m_type{ValueType::FLOAT}
{
  m_val.fltVal = static_cast<float>(value);
}

auto Value::GetBinaryOpType(const Value& value) const -> OpType
{
  if (m_type == ValueType::INT)
  {
    if (value.m_type == ValueType::INT)
    {
      return OpType::II;
    }
    if (value.m_type == ValueType::FLOAT)
    {
      return OpType::IF;
    }
  }
  else if (m_type == ValueType::FLOAT)
  {
    if (value.m_type == ValueType::INT)
    {
      return OpType::FI;
    }
    if (value.m_type == ValueType::FLOAT)
    {
      return OpType::FF;
    }
  }
  return OpType::UNDEF;
}

// Unary negation
auto Value::operator-() const -> Value
{
  switch (m_type)
  {
    case ValueType::INT:
      return Value(-m_val.intVal);
    case ValueType::FLOAT:
      return Value(-m_val.fltVal);
    case ValueType::UNDEFINED:
      return *this;
  }
}

// Unary bitwise complement
auto Value::operator~() const -> Value
{
  switch (m_type)
  {
    case ValueType::INT:
      return Value(~m_val.intVal);
    case ValueType::FLOAT:
      std::cerr << "Value::operator~(): cannot complement non-integer GetFloatValue\n";
      return Value{};
    case ValueType::UNDEFINED:
      return Value{};
  }
}

// Unary logical complement
auto Value::operator!() const -> Value
{
  switch (m_type)
  {
    case ValueType::INT:
      return Value{0 == m_val.intVal};
    case ValueType::FLOAT:
      std::cerr << "Value::operator!(): cannot complement non-integer GetFloatValue\n";
      return Value{};
    case ValueType::UNDEFINED:
      return Value{};
  }
}

// Absolute value
auto Value::Abs() const -> Value
{
  switch (m_type)
  {
    case ValueType::INT:
      return Value{m_val.intVal > 0 ? m_val.intVal : -m_val.intVal};
    case ValueType::FLOAT:
      return Value{std::fabs(m_val.fltVal)};
    case ValueType::UNDEFINED:
      return Value{};
  }
}

// Binary bitwise AND
auto Value::operator&(const Value& otherValue) const -> Value
{
  switch (GetBinaryOpType(otherValue))
  {
    case OpType::II:
      return Value(m_val.intVal & otherValue.m_val.intVal);
    case OpType::IF:
    case OpType::FI:
    case OpType::FF:
      std::cerr << "Value::operator&(): cannot bitwise AND non-integer values\n";
      return Value{};
    case OpType::UNDEF:
      return Value{};
  }
}

// Binary bitwise OR
auto Value::operator|(const Value& otherValue) const -> Value
{
  switch (GetBinaryOpType(otherValue))
  {
    case OpType::II:
      return Value(m_val.intVal | otherValue.m_val.intVal);
    case OpType::IF:
    case OpType::FI:
    case OpType::FF:
      std::cerr << "Value::operator|(): cannot bitwise OR non-integer values\n";
      return Value{};
    case OpType::UNDEF:
      return Value{};
  }
}

// Binary logical AND
auto Value::operator&&(const Value& otherValue) const -> Value
{
  switch (GetBinaryOpType(otherValue))
  {
    case OpType::II:
      return Value{(m_val.intVal != 0) and (otherValue.m_val.intVal != 0)};
    case OpType::IF:
    case OpType::FI:
    case OpType::FF:
      std::cerr << "Value::operator&&(): cannot logical AND non-integer values\n";
      return Value{};
    case OpType::UNDEF:
      return Value{};
  }
}

// Binary logical OR
auto Value::operator||(const Value& otherValue) const -> Value
{
  switch (GetBinaryOpType(otherValue))
  {
    case OpType::II:
      return Value{(m_val.intVal != 0) or (otherValue.m_val.intVal != 0)};
    case OpType::IF:
    case OpType::FI:
    case OpType::FF:
      std::cerr << "Value::operator||(): cannot logical OR non-integer values\n";
      return Value{};
    case OpType::UNDEF:
      return Value{};
  }
}

// Equality
auto Value::operator==(const Value& otherValue) const -> Value
{
  switch (GetBinaryOpType(otherValue))
  {
    case OpType::II:
      return Value{m_val.intVal == otherValue.m_val.intVal};
    case OpType::IF:
      return Value{static_cast<float>(m_val.intVal) == otherValue.m_val.fltVal};
    case OpType::FI:
      return Value{m_val.fltVal == static_cast<float>(otherValue.m_val.intVal)};
    case OpType::FF:
      return Value{m_val.fltVal == otherValue.m_val.fltVal};
    case OpType::UNDEF:
      return Value{};
  }
}

// Inequality
auto Value::operator!=(const Value& otherValue) const -> Value
{
  switch (GetBinaryOpType(otherValue))
  {
    case OpType::II:
      return Value(m_val.intVal != otherValue.m_val.intVal);
    case OpType::IF:
      return Value(static_cast<float>(m_val.intVal) != otherValue.m_val.fltVal);
    case OpType::FI:
      return Value(m_val.fltVal != static_cast<float>(otherValue.m_val.intVal));
    case OpType::FF:
      return Value(m_val.fltVal != otherValue.m_val.fltVal);
    case OpType::UNDEF:
      return Value{};
  }
}

// Less-than
auto Value::operator<(const Value& otherValue) const -> Value
{
  switch (GetBinaryOpType(otherValue))
  {
    case OpType::II:
      return Value(m_val.intVal < otherValue.m_val.intVal);
    case OpType::IF:
      return Value(static_cast<float>(m_val.intVal) < otherValue.m_val.fltVal);
    case OpType::FI:
      return Value(m_val.fltVal < static_cast<float>(otherValue.m_val.intVal));
    case OpType::FF:
      return Value(m_val.fltVal < otherValue.m_val.fltVal);
    case OpType::UNDEF:
      return Value{};
  }
}

// Less-than or equal-to
auto Value::operator<=(const Value& otherValue) const -> Value
{
  switch (GetBinaryOpType(otherValue))
  {
    case OpType::II:
      return Value(m_val.intVal <= otherValue.m_val.intVal);
    case OpType::IF:
      return Value(static_cast<float>(m_val.intVal) <= otherValue.m_val.fltVal);
    case OpType::FI:
      return Value(m_val.fltVal <= static_cast<float>(otherValue.m_val.intVal));
    case OpType::FF:
      return Value(m_val.fltVal <= otherValue.m_val.fltVal);
    case OpType::UNDEF:
      return Value{};
  }
}

// Greater-than or equal-to
auto Value::operator>=(const Value& otherValue) const -> Value
{
  switch (GetBinaryOpType(otherValue))
  {
    case OpType::II:
      return Value(m_val.intVal >= otherValue.m_val.intVal);
    case OpType::IF:
      return Value(static_cast<float>(m_val.intVal) >= otherValue.m_val.fltVal);
    case OpType::FI:
      return Value(m_val.fltVal >= static_cast<float>(otherValue.m_val.intVal));
    case OpType::FF:
      return Value(m_val.fltVal >= otherValue.m_val.fltVal);
    case OpType::UNDEF:
      return Value{};
  }
}

// Greater-than
auto Value::operator>(const Value& otherValue) const -> Value
{
  switch (GetBinaryOpType(otherValue))
  {
    case OpType::II:
      return Value(m_val.intVal > otherValue.m_val.intVal);
    case OpType::IF:
      return Value(static_cast<float>(m_val.intVal) > otherValue.m_val.fltVal);
    case OpType::FI:
      return Value(m_val.fltVal > static_cast<float>(otherValue.m_val.intVal));
    case OpType::FF:
      return Value(m_val.fltVal > otherValue.m_val.fltVal);
    case OpType::UNDEF:
      return Value{};
  }
}

// Binary addition
auto Value::operator+(const Value& otherValue) const -> Value
{
  switch (GetBinaryOpType(otherValue))
  {
    case OpType::II:
      return Value(m_val.intVal + otherValue.m_val.intVal);
    case OpType::IF:
      return Value(static_cast<float>(m_val.intVal) + otherValue.m_val.fltVal);
    case OpType::FI:
      return Value(m_val.fltVal + static_cast<float>(otherValue.m_val.intVal));
    case OpType::FF:
      return Value(m_val.fltVal + otherValue.m_val.fltVal);
    case OpType::UNDEF:
      return Value{};
  }
}

// Subtraction
auto Value::operator-(const Value& otherValue) const -> Value
{
  switch (GetBinaryOpType(otherValue))
  {
    case OpType::II:
      return Value(m_val.intVal - otherValue.m_val.intVal);
    case OpType::IF:
      return Value(static_cast<float>(m_val.intVal) - otherValue.m_val.fltVal);
    case OpType::FI:
      return Value(m_val.fltVal - static_cast<float>(otherValue.m_val.intVal));
    case OpType::FF:
      return Value(m_val.fltVal - otherValue.m_val.fltVal);
    case OpType::UNDEF:
      return Value{};
  }
}

// Multiplication
auto Value::operator*(const Value& otherValue) const -> Value
{
  switch (GetBinaryOpType(otherValue))
  {
    case OpType::II:
      return Value(m_val.intVal * otherValue.m_val.intVal);
    case OpType::IF:
      return Value(static_cast<float>(m_val.intVal) * otherValue.m_val.fltVal);
    case OpType::FI:
      return Value(m_val.fltVal * static_cast<float>(otherValue.m_val.intVal));
    case OpType::FF:
      return Value(m_val.fltVal * otherValue.m_val.fltVal);
    case OpType::UNDEF:
      return Value{};
  }
}

// Division (checks for /0 and returns an undefined number)
// Note that int / int -> float, unlike C rules
auto Value::operator/(const Value& otherValue) const -> Value
{
  if ((m_type == ValueType::UNDEFINED) or (otherValue.m_type == ValueType::UNDEFINED))
  {
    return Value{};
  }

  const auto divisor = (otherValue.m_type == ValueType::INT)
                           ? static_cast<float>(otherValue.m_val.intVal)
                           : otherValue.m_val.fltVal;
  if (divisor == 0.0F)
  {
    return Value{};
  }

  const auto dividend =
      (m_type == ValueType::INT) ? static_cast<float>(m_val.intVal) : m_val.fltVal;

  return Value(dividend / divisor);
}

// Modulo (checks for /0 and returns an undefined number)
auto Value::operator%(const Value& otherValue) const -> Value
{
  switch (GetBinaryOpType(otherValue))
  {
    case OpType::II:
      if (0 == otherValue.m_val.intVal)
      {
        return Value{};
      }
      return Value{m_val.intVal % otherValue.m_val.intVal};
    case OpType::IF:
    case OpType::FI:
    case OpType::FF:
      std::cerr << "Value::operator%(): cannot compute modulo of non-integer values\n";
      return Value{};
    case OpType::UNDEF:
      return Value{};
  }
}

// Power
auto Value::operator^(const Value& otherValue) const -> Value
{
  switch (GetBinaryOpType(otherValue))
  {
    case OpType::II:
      return Value(std::pow(static_cast<double>(m_val.intVal), otherValue.m_val.intVal));
    case OpType::IF:
      return Value(std::pow(static_cast<float>(m_val.intVal), otherValue.m_val.fltVal));
    case OpType::FI:
      return Value(std::pow(m_val.fltVal, otherValue.m_val.intVal));
    case OpType::FF:
      return Value(std::pow(m_val.fltVal, otherValue.m_val.fltVal));
    case OpType::UNDEF:
      return Value{};
  }
}

// Returns numeric intValue in i, true if a valid integer
auto Value::GetIntValue(int& intValue) const -> bool
{
  if (m_type == ValueType::INT)
  {
    intValue = m_val.intVal;
    return true;
  }

  return false;
}

auto Value::GetFloatValue(float& fltValue) const -> bool
{
  if (m_type == ValueType::INT)
  {
    fltValue = static_cast<float>(m_val.intVal);
    return true;
  }
  if (m_type == ValueType::FLOAT)
  {
    fltValue = m_val.fltVal;
    return true;
  }

  return false;
}

auto operator<<(std::ostream& out, const Value& value) -> std::ostream&
{
  switch (value.m_type)
  {
    case ValueType::INT:
      out << value.m_val.intVal;
      break;
    case ValueType::FLOAT:
      out.setf(std::ios::showpoint);
      out << value.m_val.fltVal;
      out.unsetf(std::ios::showpoint);
      break;
    case ValueType::UNDEFINED:
      out << "(undefined value)";
      break;
  }
  return out;
}

} // namespace LSYS
