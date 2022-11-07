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

#include <iostream>
#include <cmath>
#include "Value.h"

using std::cerr;
using std::endl;


namespace LSys {


  // If not initialized to a value, has an undefined value
  Value::Value()
  {
    type= undefinedType;
  }


  Value::Value(bool i)
  {
    type= intType;
    val.ival= int(i);
  }


  Value::Value(int i)
  {
    type= intType;
    val.ival= i;
  }


  Value::Value(float f)
  {
    type= floatType;
    val.fval= f;
  }


  Value::Value(double d)
  {
    type= floatType;
    val.fval= d;
  }


  Value::Value(const Value& v)
  {
    type= v.type;
    val = v.val;
  }


  Value::Optype Value::binary_optype(const Value& v) const
  {
    if (type == intType) {
      if (v.type == intType)
        return II;
    else if (v.type == floatType)
      return IF;
    } else if (type == floatType) {
      if (v.type == intType)
        return FI;
      else if (v.type == floatType)
        return FF;
    }
    return UNDEF;
  }


  // Unary negation
  Value Value::operator-() const
  {
    switch (type) {
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
    switch (type) {
      case intType:
        return Value(~val.ival);
      case floatType:
        cerr << "Value::operator~(): cannot complement non-integer value" << endl;
      case undefinedType:
      default:
        return Value();
    }
  }


  // Unary logical complement
  Value Value::operator!() const
  {
    switch (type) {
      case intType:
        return Value(!val.ival);
      case floatType:
        cerr << "Value::operator!(): cannot complement non-integer value" << endl;
      case undefinedType:
      default:
        return Value();
    }
  }


  // Absolute value
  Value Value::abs() const
  {
    switch (type) {
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
  Value Value::operator& (const Value& v) const
  {
    switch (binary_optype(v)) {
      case II:
        return Value(val.ival & v.val.ival);
      case IF:
      case FI:
      case FF:
        cerr << "Value::operator&(): cannot bitwise AND non-integer values" << endl;
      case UNDEF:
      default:
        return Value();
    }
  }


  // Binary bitwise OR
  Value Value::operator| (const Value& v) const
  {
    switch (binary_optype(v)) {
      case II:
        return Value(val.ival | v.val.ival);
      case IF:
      case FI:
      case FF:
        cerr << "Value::operator|(): cannot bitwise OR non-integer values" << endl;
      case UNDEF:
      default:
        return Value();
    }
  }


  // Binary logical AND
  Value Value::operator&&(const Value& v) const
  {
    switch (binary_optype(v)) {
      case II:
        return Value(val.ival && v.val.ival);
      case IF:
      case FI:
      case FF:
        cerr << "Value::operator&&(): cannot logical AND non-integer values" << endl;
      case UNDEF:
      default:
        return Value();
    }
  }


  // Binary logical OR
  Value Value::operator||(const Value& v) const
  {
    switch (binary_optype(v)) {
      case II:
        return Value(val.ival || v.val.ival);
      case IF:
      case FI:
      case FF:
        cerr << "Value::operator||(): cannot logical OR non-integer values" << endl;
      case UNDEF:
      default:
        return Value();
    }
  }


  // Equality
  Value Value::operator==(const Value &v) const {
      switch (binary_optype(v)) {
    case II:
        return Value(val.ival == v.val.ival);
    case IF:
        return Value(val.ival == v.val.fval);
    case FI:
        return Value(val.fval == v.val.ival);
    case FF:
        return Value(val.fval == v.val.fval);
    case UNDEF:
    default:
        return Value();
      }
  }

  // Inequality
  Value Value::operator!=(const Value &v) const {
      switch (binary_optype(v)) {
    case II:
        return Value(val.ival != v.val.ival);
    case IF:
        return Value(val.ival != v.val.fval);
    case FI:
        return Value(val.fval != v.val.ival);
    case FF:
        return Value(val.fval != v.val.fval);
    case UNDEF:
    default:
        return Value();
      }
  }

  // Less-than
  Value Value::operator< (const Value &v) const {
      switch (binary_optype(v)) {
    case II:
        return Value(val.ival <  v.val.ival);
    case IF:
        return Value(val.ival <  v.val.fval);
    case FI:
        return Value(val.fval <  v.val.ival);
    case FF:
        return Value(val.fval <  v.val.fval);
    case UNDEF:
    default:
        return Value();
      }
  }

  // Less-than or equal-to
  Value Value::operator<=(const Value &v) const {
      switch (binary_optype(v)) {
    case II:
        return Value(val.ival <= v.val.ival);
    case IF:
        return Value(val.ival <= v.val.fval);
    case FI:
        return Value(val.fval <= v.val.ival);
    case FF:
        return Value(val.fval <= v.val.fval);
    case UNDEF:
    default:
        return Value();
      }
  }

  // Greater-than or equal-to
  Value Value::operator>=(const Value &v) const {
      switch (binary_optype(v)) {
    case II:
        return Value(val.ival >= v.val.ival);
    case IF:
        return Value(val.ival >= v.val.fval);
    case FI:
        return Value(val.fval >= v.val.ival);
    case FF:
        return Value(val.fval >= v.val.fval);
    case UNDEF:
    default:
        return Value();
      }
  }

  // Greater-than
  Value Value::operator> (const Value &v) const {
      switch (binary_optype(v)) {
    case II:
        return Value(val.ival >  v.val.ival);
    case IF:
        return Value(val.ival >  v.val.fval);
    case FI:
        return Value(val.fval >  v.val.ival);
    case FF:
        return Value(val.fval >  v.val.fval);
    case UNDEF:
    default:
        return Value();
      }
  }

  // Binary addition
  Value Value::operator+ (const Value &v) const {
      switch (binary_optype(v)) {
    case II:
        return Value(val.ival +  v.val.ival);
    case IF:
        return Value(val.ival +  v.val.fval);
    case FI:
        return Value(val.fval +  v.val.ival);
    case FF:
        return Value(val.fval +  v.val.fval);
    case UNDEF:
    default:
        return Value();
      }
  }

  // Subtraction
  Value Value::operator- (const Value &v) const {
      switch (binary_optype(v)) {
    case II:
        return Value(val.ival -  v.val.ival);
    case IF:
        return Value(val.ival -  v.val.fval);
    case FI:
        return Value(val.fval -  v.val.ival);
    case FF:
        return Value(val.fval -  v.val.fval);
    case UNDEF:
    default:
        return Value();
      }
  }

  // Multiplication
  Value Value::operator* (const Value &v) const {
      switch (binary_optype(v)) {
    case II:
        return Value(val.ival *  v.val.ival);
    case IF:
        return Value(val.ival *  v.val.fval);
    case FI:
        return Value(val.fval *  v.val.ival);
    case FF:
        return Value(val.fval *  v.val.fval);
    case UNDEF:
    default:
        return Value();
      }
  }

  // Division (checks for /0 and returns an undefined number)
  // Note that int / int -> float, unlike C rules
  Value Value::operator/ (const Value &v) const {
      float divisor, dividend;

      if (type == undefinedType || v.type == undefinedType)
    return Value();

      divisor= (v.type == intType) ? (float)v.val.ival : v.val.fval;

      if (divisor == 0)
    return Value();

      dividend= (type == intType) ? (float)val.ival : val.fval;
      return Value(dividend / divisor);
  }

  // Modulo (checks for /0 and returns an undefined number)
  Value Value::operator%(const Value& v) const
  {
    switch (binary_optype(v)) {
      case II:
        if (v.val.ival == 0)
          return Value();
        else
          return Value(val.ival % v.val.ival);
      case IF:
      case FI:
      case FF:
        cerr << "Value::operator%(): cannot compute modulo of non-integer values" << endl;
      case UNDEF:
      default:
        return Value();
    }
  }

  // Power
  Value Value::operator^ (const Value &v) const {
      switch (binary_optype(v)) {
    case II:
        return Value(std::pow(double(val.ival),v.val.ival));
    case IF:
        return Value(std::pow(float(val.ival),v.val.fval));
    case FI:
        return Value(std::pow(val.fval,v.val.ival));
    case FF:
        return Value(std::pow(val.fval,v.val.fval));
    case UNDEF:
    default:
        return Value();
      }
  }

  // Returns numeric value in i, true if a valid integer
  bool Value::value(int &i) const {
      if (type == intType) {
    i= val.ival;
    return true;
      } else
    return false;
  }

  // Returns numeric value in f, true if a valid integer or real number
  //  (integers are converted to floats).
  bool Value::value(float &f) const {
      if (type == intType) {
    f= val.ival;
    return true;
      } else if (type == floatType) {
    f= val.fval;
    return true;
      } else
    return false;
  }

  std::ostream& operator<<(std::ostream& o, const Value& v)
  {
      if (&v == NULL)
    return o;

      switch (v.type) {
    case intType:
        o << v.val.ival;
        break;
    case floatType:
        o.setf(std::ios::showpoint);
        o << v.val.fval;
        o.unsetf(std::ios::showpoint);
        break;
    case undefinedType:
    default:
        o << "(undefined value)";
        break;
      }
      return o;
  }


};  
