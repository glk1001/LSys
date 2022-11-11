#pragma once

#include <cstdlib>

namespace LSys
{

inline double drand48()
{
  return static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
}

inline void srand48(unsigned int seed)
{
  srand(seed);
}

} // namespace LSys
