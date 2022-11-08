/* Module.c - methods for handling L-system modules.
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
 * $Log:	Module.c,v $
 * Revision 1.3  91/03/20  10:35:38  leech
 * Bug fix in module printing.
 *
 * Revision 1.2  90/10/12  18:48:06  leech
 * First public release.
 *
 */
//static char RCSid[]= "$Id: Module.c,v 1.3 91/03/20 10:35:38 leech Exp $";

#include <iostream>
#include "debug.h"
#include "Value.h"
#include "SymbolTable.h"
#include "Expression.h"
#include "Module.h"

using std::cerr;
using std::endl;


namespace LSys {


  // These Names are used in context matching to ascend/descend tree levels
  Name
      LBRACKET("["),
      RBRACKET("]");

  Module::Module(
      const Name &n,
      List<Expression>* elist,
      bool ignore) : tag(n) {

      //PDebug(PD_MEMLEAK, memalloc("Module::Module(name,elist,ignore)", this));
      param= elist;
      ignoreflag= (ignore == true);
      emptyflag= 0;

      PDebug(PD_MODULE,
       cerr << "Creating module " << *this << " @ " << (void *)this << endl);
  }


  Module::Module(Module &m) : tag(m.tag)
  {
    //PDebug(PD_MEMLEAK, memalloc("Module::Module(Module &)", this));
    param= m.param;
    ignoreflag= m.ignoreflag;
    emptyflag= 0;

    PDebug(PD_MODULE, cerr << "Copying module " << *this << endl);
  }


  Module::~Module()
  {
    //PDebug(PD_MEMLEAK, memfree("Module::~Module()", this));
    PDebug(PD_MODULE, cerr << "Deleting module @ " << (void *)this << endl);

    if (emptyflag == 0)
      delete param;
  }


  // Don't delete parameter list when destructor called, even if it's
  //  dynamically allocated.
  void Module::empty()
  {
    emptyflag= 1;
  }


  // Bind symbolic names of the module to values in module 'values'
  //  using symbol table st for evaluation and binding. The two
  //  modules should conform() for this method to succeed.
  bool Module::bind(const Module& values, SymbolTable<Value>& st) const
  {
    PDebug(PD_MODULE,
      cerr << "Module::bind: formals= " << *this
           << " values= " << values << endl);

    if (LSys::bind(param, values.param, st) == false) {
      cerr << "failure binding module " << values << " to " << *this << endl;
      return false;
    } else
      return true;
  }


  // Check if module 'm' is conformant with the module, e.g.,
  //  that they have the same name and their expression lists are
  //  conformant.
  bool Module::conforms(const Module& m) const
  {
    if (tag != m.tag)
      return false;

    return LSys::conforms(param, m.param);
  }


  // Instantiate the module; that is, return a copy with all of the
  //  module's expressions evaluated in the context of the symbol table.
  Module* Module::instantiate(SymbolTable<Value>& st) const
  {
    List<Expression> *el= LSys::instantiate(param, st);
    Module* new_m= new Module(Name(tag), el, ignoreflag ? true : false);

    PDebug(PD_MODULE,
      cerr << "Module::instantiate: " << *this << " @ " << (void *)this
           << " -> " << *new_m << " @ " << (void *) new_m << endl
    );
    PDebug(PD_MODULE, cerr << "        old elist: " << *el << endl);

    return new_m;
  }


  // Return the n'th (0 base) parameter of module m in f, if available.
  // Returns true on success, false if m does not have enough parameters
  //  or the parameter is not a number.
  bool Module::getfloat(float &f, unsigned int n) const
  {
    if (param == 0) return false;

    // An empty symbol table used to ensure the argument is a bound value.
    static SymbolTable<Value> st;
    return LSys::getfloat(st, *param, f, n);
  }


  std::ostream& operator<<(std::ostream& o, const Module& m)
  {
    if (&m == NULL)
      return o;

    o << Name(m.tag);
    if (m.param != NULL && m.param->size() > 0)
      o << *m.param;

    return o;
  }


};

