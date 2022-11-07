// GLK: New production is faulty!!


/* Production.c - methods for L-system productions.
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
 * $Log:	Production.c,v $
 * Revision 1.2  90/10/12  18:48:11  leech
 * First public release.
 *
 */
//static char RCSid[]= "$Id: Production.c,v 1.2 90/10/12 18:48:11 leech Exp Locker: leech $";

#include <cstdlib>
#include "debug.h"
#include "Rand.h"
#include "Value.h"
#include "SymbolTable.h"
#include "Expression.h"
#include "Module.h"
#include "Production.h"

using std::cerr;
using std::endl;


namespace LSys {


  Production::Production(
    const Name& name, Predecessor* lhs, const Expression* cond, const List<Successor> *rhs)
  : prodname(name)
  {
    // Ensure that empty context lists are represented by 0 pointers
    if (lhs->left && (lhs->left->size() == 0)) {
      delete lhs->left;
      lhs->left= 0;
    }
    if (lhs->right && lhs->right->size() == 0) {
      delete lhs->right;
      lhs->right= 0;
    }

    input= lhs;
    condition= cond;
    successors= rhs;

    if (lhs->left == 0 && lhs->right == 0)
      cfree= true;
    else
      cfree= false;

    PDebug(PD_PRODUCTION, cerr << "Production::Production: created " << *this << endl);
  }


  Production::~Production()
  {
    delete input;
    delete condition;
    delete successors;
  }


  // See if module m matches the left hand side of this production and
  //  satisfies the conditional expression attached to it. The list
  //  iterator must be set at m, as it provides context for context-sensitive
  //  productions. Neither the iterator nor the module are modified.
  bool Production::matches(
    const ListIterator<Module>& mi, const Module* m, SymbolTable<Value>& st)
  {
    PDebug(PD_PRODUCTION, cerr << "Production::matches: testing module " << *m << " against " << *this << endl);
    PDebug(PD_PRODUCTION, cerr << "\t" << *input->center << " matches? " << *m << '\n');

    // Test the predecessor module itself
    if (input->center->conforms(*m) == false) return false;

    // Bind formal parameters of the predecessor
    // Should test return value to ensure binding occurred
    input->center->bind(*m, st);

    // Now match context-sensitive surroundings, if any.

    // Left context
    if (input->left) {
      PDebug(PD_PRODUCTION, cerr << "    [left context]\n");
      // Scan each list in reverse order
      ListIterator<Module> li_formal(input->left);
      ListIterator<Module> li_value= mi;
      Module* formal;
      Module* value;
      for (formal= li_formal.last(), value= li_value.previous();
        formal != 0 && value != 0;
        formal= li_formal.previous(), value= li_value.previous()) {

        // Find the next potentially matching module; skip over
        //	ignored modules as well as bracketed substrings
        //	(e.g. A < B matches A[anything]B)
        for (int brackets= 0; value != 0; value= li_value.previous()) {
          // Skip over ignored modules
          if (value->ignore())
            continue;
          // Skip over ], and increase bracket level.
          if (value->name() == RBRACKET) {    // ]
            brackets++;
            continue;
          }
          // Skip over [, and decrease bracket level iff > 0
          if (value->name() == LBRACKET) {    // [
            if (brackets > 0)
              brackets--;
            continue;
          }
          // Found a potentially matching module
          if (brackets == 0)
            break;
        }

        // If start of string was reached without finding a potentially
        //	matching module, context matching failed.
        if (value == 0)
          return false;

        PDebug(PD_PRODUCTION, cerr << "\t" << *formal << " matches? " << *value << '\n');

        // See if the formal and value modules conform
        if (formal->conforms(*value) == false)
          return false;
        // Bind formal arguments
        formal->bind(*value, st);
      }

      // If the formal parameter list is non-0, context matching failed
      // This could be due to a nonconforming value module, or simply
      //  running out of value modules to test.
      if (formal != 0)
        return false;
    }

    // Right context
    if (input->right) {
      ListIterator<Module> li_formal(input->right), li_value= mi;
      Module *formal, *value;

      PDebug(PD_PRODUCTION, cerr << "    [right context]\n");
      // Scan each list in reverse order
      for (formal= li_formal.first(), value= li_value.next();
        formal != 0 && value != 0;
        formal= li_formal.next(), value= li_value.next()) {

        // Find the next potentially matching module; skip over
        //	bracketed substrings, e.g. A < B matches A[anything]B
        //	as well as modules which should be ignored.
        if (formal->name() == LBRACKET) {		    // [
          // Must find a matching [; skip only ignored modules
          while (value && value->ignore())
            value= li_value.next();
        } else if (formal->name() == RBRACKET) {	    // ]
          // Must find a matching ]; skip anything else including
          //  bracketed substrings.
          for (int brackets= 0; value; value= li_value.next()) {
            if (value->name() == RBRACKET) {	    // ]
              if (brackets-- == 0)
                break;
            } else if (value->name() == LBRACKET)   // [
              brackets++;
            }
        } else {
          // Find the next potentially matching module; skip over
          //  ignored modules as well as bracketed substrings,
          //  (e.g. A > B matches A[anything]B)
          for (int brackets= 0; value != 0; value= li_value.next()) {
            // Skip over ignored modules
            if (value->ignore())
              continue;
            if (value->name() == LBRACKET) {	    // [
              brackets++;
              continue;
            }
            if (value->name() == RBRACKET) {	    // ]
              if (brackets > 0)
                brackets--;
              else {
                // This is a case like B > C against A[B]C; it
                //	should not match, because C is not along
                //	the same path from root to branch as B.
                return false;
              }
              continue;
            }
            // Found a potentially matching module
            if (brackets == 0)
              break;
          }
        }

        // If start of string was reached without finding a potentially
        //	matching module, context matching failed.
        if (value == 0)
          return false;

        PDebug(PD_PRODUCTION, cerr << "\t" << *formal << " matches? " << *value << '\n');

        // See if the formal and value modules conform
        if (formal->conforms(*value) == false)
          return false;
        // Bind formal arguments
        formal->bind(*value, st);
      }

      // If the formal parameter list is non-0, context matching failed
      // This could be due to a nonconforming value module, or simply
      //  running out of value modules to test.
      if (formal != 0)
        return false;
    }

    // Finally, evaluate the optional conditional expression with the
    //	bound formals; return its boolean value if it evaluated to an
    //	integer, false otherwise.
    if (!condition)
      return true;
    else {
      Value v= condition->evaluate(st);
      PDebug(PD_PRODUCTION, cerr << "    [condition] -> " << v << endl);
      int i;
      if (v.value(i) == true)
        return i ? true : false;
      else
        return false;
    }
  }


  // Given a module which matches() the left hand side of this
  //  production, apply the production and return the resulting
  //  module list.
  List<Module>* Production::produce(
    const Module* predecessor, SymbolTable<Value>& st)
  {
    List<Module>* ml= new List<Module>;

    // If no successors for this production, die (could return an empty list
    //	or return a copy of the predecessor).
    if (successors->size() == 0) {
      cerr << "Error in Production::produce: production has no successors:\n"
           << *this << endl;
      std::exit(1);
    }

    // Pick one of the successors of the production at random.
    const float random_var= drand48();
    float cumulative_probability= 0;

    const List<Module>* mlist;
    const Successor* succ;
    ConstListIterator<Successor> si(*successors);
    for (mlist= 0, succ= si.first(); succ != 0; succ= si.next()) {
      cumulative_probability += succ->probability;
      if (random_var <= cumulative_probability) {
        mlist= succ->mlist;
        break;
      }
    }

    // If no successor was chosen, complain and return an empty list
    if (mlist == 0) {
      cerr << "Error in Production::produce: no successor was chosen:\n"
           << *this << endl;
      return ml;
    }

    // For each module in the successor side, instantiate it and
    //	add to the list.
    ConstListIterator<Module> li(*mlist);
    for (const Module* m= li.first(); m; m= li.next()) {
      Module* new_m= m->instantiate(st);
      ml->append(new_m);
    }

    PDebug(PD_PRODUCTION,
      cerr << "Production::produce:\n"
           << "Production is:  " << *this << '\n'
           << "Predecessor is: " << *predecessor << '\n'
           << "Result is:      " << *ml << endl);

    return ml;
  }


  std::ostream& operator<<(std::ostream& o, const Successor& s)
  {
    if (&s == 0)
      return o;

    o << "\t-> ";
    if (s.probability < 1)
      o << '(' << s.probability << ") ";
    o << *s.mlist;

    return o;
  }


  std::ostream& operator<<(std::ostream& o, const Predecessor& c)
  {
    if (c.left != NULL)
      o << *c.left << " < ";
    // Should always be non-NULL
    if (c.center != NULL)
      o << *c.center;
    if (c.right != NULL)
      o << " > " << *c.right;

    return o;
  }


  std::ostream& operator<<(std::ostream& o, const Production& p)
  {
    if (&p == 0)
      return o;

    o << "( " << p.prodname << " : " << *p.input;
    if (p.cfree)
      o << " (CF) ";
    if (p.condition)
      o << " : "   << *p.condition;
    o << *p.successors;

    return o;
  }


};

