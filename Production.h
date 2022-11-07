/* Production.h - class definition for L-system productions.
 *
 * $Id: Production.h,v 1.3 91/03/20 10:39:45 leech Exp Locker: leech $
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
 * $Log:	Production.h,v $
 * Revision 1.3  91/03/20  10:39:45  leech
 * Support for G++.
 *
 * Revision 1.2  90/10/12  18:48:11  leech
 * First public release.
 *
 */

#ifndef _Production_h
  #define _Production_h

  #include "Module.h"

  namespace LSys {

    // A Predecessor is the left-hand side of a production; it contains
    // the module replaced by a list of successors and optionally,
    // the left and right context required to apply the production.
    // The class is basically just a container for three pointers,
    // so it's declared public.
    struct Predecessor {
      List<Module>* left;
      Module* center;
      List<Module>* right;

      Predecessor(List<Module>* l=0, Module* c=0, List<Module>* r=0)
      : left(l), center(c), right(r)
      {}
      ~Predecessor() { delete left; delete center; delete right; }

      friend std::ostream& operator<<(std::ostream&, const Predecessor&);
    };


    // A Successor is the right-hand side of a production; it contains
    //  a probability of choosing this production and a list of modules
    //  to replace the predecessor module with if chosen.
    // This is also just a container class, but it can be non-public
    //  with judicious use of friends.
    class Successor {
      public:
        Successor(const List<Module>* m, float p=1): probability(p), mlist(m) {}
        ~Successor() { delete mlist; }

        friend class Production;
        friend std::ostream& operator<<(std::ostream&, const Successor&);
      private:
        float probability;
        const List<Module>* mlist;
    };


    // A Production is applied to a Module to produce a new list of Modules.
    // The production may be context-sensitive to surrounding Modules.
    class Production {
      public:
        Production(const Name&, Predecessor* lhs,
          const Expression* cond, const List<Successor>* rhs);
        ~Production();
        bool context_free() const { return cfree; }
        bool matches(const ListIterator<Module>&, const Module*, SymbolTable<Value>&);
        List<Module>* produce(const Module* predecessor, SymbolTable<Value>&);

        friend std::ostream& operator<<(std::ostream&, const Production&);
      private:
        Name prodname;
        bool cfree; // Is the production context-free?
        const Predecessor* input;
        const Expression* condition;
        const List<Successor> *successors;
    };

  };

#endif
