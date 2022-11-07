#include <iostream>
#include "SymbolTable.h"

namespace LSys {

  std::ostream& operator<<(std::ostream& o, Anyptr p)
  {
    return o << *(void**)&p;
  }

};  

