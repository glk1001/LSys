/* Expression.h - class definition for arithmetic expressions.
 *
 * $Id: Expression.h,v 1.5 95/05/24 17:13:21 leech Exp $
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

#ifndef _Expression_h
  #define _Expression_h

  #include "Value.h"
  #include "Name.h"
  #include "List.h"

  namespace LSys {

    class Expression {
      public:
        Expression(int o, Expression* lop, Expression* rop);
        Expression(const Name&, List<Expression>* args=0);
        Expression(const Value*);
        Expression(const Value&);
        ~Expression();

        // Access methods
        int type() const { return op; }
        Name name() const;

        // Evaluation methods
        Value evaluate(const SymbolTable<Value>&) const;
        Value leval(const SymbolTable<Value>& st) const { return lchild()->evaluate(st); }
        Value reval(const SymbolTable<Value>& st) const { return rchild()->evaluate(st); }

        friend std::ostream& operator<<(std::ostream&, const Expression&);
      private:
        int op;
        union {
          struct {
            int id;   // Name id
            List<Expression>* args; // Function arguments
          } name;
          char v[sizeof(Value)]; // Ensure union is big enough for a Value
          Expression* args[2];   // Child expressions
        } val;
        Name varname() const { return Name(val.name.id); }
        Value value() const { return *(Value *)&val.v; }
        Expression* lchild() const { return val.args[0]; }
        Expression* rchild() const { return val.args[1]; }
        Name funcname() const { return Name(val.name.id); }
        List<Expression>* funcargs() const { return val.name.args; }
    };

    bool bind(const List<Expression>* formals,
      const List<Expression>* values, SymbolTable<Value>&);
    bool conforms(const List<Expression>* formals, const List<Expression>* values);
    List<Expression>* instantiate(const List<Expression>* before, const SymbolTable<Value>&);
    bool getfloat(const SymbolTable<Value>&, const List<Expression>&, float&, unsigned int n=0);
    bool getvalue(const SymbolTable<Value>&, const List<Expression>&, Value&, unsigned int n=0);

  };
  
#endif

