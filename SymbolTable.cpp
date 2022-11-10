#include "SymbolTable.h"

#include <iostream>

namespace LSys
{

std::ostream& operator<<(std::ostream& o, Anyptr p)
{
  return o << *reinterpret_cast<void**>(&p);
}

};
