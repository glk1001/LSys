/* Name.c - methods for space-efficient hashed names.
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
 * $Log:	Name.c,v $
 * Revision 1.3  91/03/20  10:38:10  leech
 * Added debugging output.
 *
 * Revision 1.2  90/10/12  18:48:07  leech
 * First public release.
 *
 */
//static char RCSid[]= "$Id: Name.c,v 1.3 91/03/20 10:38:10 leech Exp $";

#include <iostream>
#include <cassert>
#include <cstring>
#include "Name.h"
#include "debug.h"

using std::cerr;
using std::endl;


namespace LSys {


  SymbolTable<int>* Name::map= 0;
  char** Name::reverse_map= 0;
  int Name::next_index= 0;
  int Name::reverse_map_size= 0;

  // Amount to extend name table by when it fills up
  const int map_incr= 20;


  Name::Name(const char* tag)
  {
    if (tag == nullptr)
        tag = "";

    if (map == 0)
      map= new SymbolTable<int>;

    PDebug(
      PD_NAME,
      cerr << "Name(" << (void *)tag << std::flush << "= " << tag << ")" << endl
    );

    // If the name already exists in the symbol table, use the assigned
    // hash value. Otherwise, enter it in the table with the next
    // sequential value.
    if (map->lookup(tag, index) == false) {
      if (next_index == reverse_map_size) {
        // Allocate more space for the inverse map from IDs to strings
        //GLK int size= reverse_map_size + map_incr;
        char** s= new char*[reverse_map_size + map_incr];

        for (int i= 0; i < reverse_map_size; i++)
          s[i]= reverse_map[i];
        if (reverse_map != 0)
          delete reverse_map;
        reverse_map= s;
        reverse_map_size += map_incr;
      }
      map->enter(tag, next_index);
      reverse_map[next_index]= strdup(tag);
      index= next_index++;
    }
  }


  std::ostream& operator<<(std::ostream& o, const Name& n)
  {
    return o << (const char*)n;
  //  return o << "[id: " << int(n) << " name: " << (const char *)n << "]";
  }


};
