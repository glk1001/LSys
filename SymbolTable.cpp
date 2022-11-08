#include "SymbolTable.h"

#include <iostream>

namespace LSys
{

std::ostream& operator<<(std::ostream& o, Anyptr p)
{
  return o << *(void**)&p;
}

};
