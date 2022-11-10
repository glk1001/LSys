#pragma once

#include <stdlib.h>

namespace LSys
{

inline double drand48()
{
  return float(rand()) / float(RAND_MAX);
}

inline void srand48(unsigned int seed)
{
  srand(seed);
}

} // namespace LSys
