/* Value.h - class definition for mixed int/float arithmetic in Expressions
 *  with runtime typing.
 *
 * $Id: Value.h,v 1.5 95/05/24 17:14:50 leech Exp $
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
 * $Log:	Value.h,v $
 * Revision 1.5  95/05/24  17:14:50  leech
 * Fix for const-correctness.
 *
 * Revision 1.4  91/10/10  19:56:17  leech
 * Added Value::Value(extended) for MPW, in which constants are of type
 * extended by default.
 *
 * Revision 1.3  91/03/20  10:40:38  leech
 * Support for G++.
 *
 * Revision 1.2  90/10/12  18:48:15  leech
 * First public release.
 *
 */

#ifndef _Values_h
  #define _Values_h

  #include <iostream>

  namespace LSys {

    enum ValueType { intType, floatType, undefinedType };

    class Value {
      public:
        Value();
        Value(bool); //GLK
        Value(int);
        Value(float);
        Value(double);
        Value(const Value&);

        Value operator-() const;
        Value operator~() const;
        Value operator!() const;
        Value abs() const;
        Value operator& (const Value&) const;
        Value operator| (const Value&) const;
        Value operator&&(const Value&) const;
        Value operator||(const Value&) const;
        Value operator==(const Value&) const;
        Value operator!=(const Value&) const;
        Value operator< (const Value&) const;
        Value operator<=(const Value&) const;
        Value operator>=(const Value&) const;
        Value operator> (const Value&) const;
        Value operator+ (const Value&) const;
        Value operator- (const Value&) const;
        Value operator* (const Value&) const;
        Value operator/ (const Value&) const;
        Value operator% (const Value&) const;
        Value operator^ (const Value&) const;

        bool value(int&) const;
        bool value(float&) const;

        friend std::ostream& operator<<(std::ostream&, const Value&);

      private:
        ValueType type;
        union {
          int ival;
          float fval;
        } val;
        enum Optype { II, IF, FI, FF, UNDEF };
        Optype binary_optype(const Value&) const;
    };

  };

#endif
