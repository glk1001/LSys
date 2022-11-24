#include "rand.h"

#include <cassert>

namespace L_SYSTEM
{

namespace
{
SetRandFunc s_setRand = nullptr;
GetRandDoubleFunc s_getRandDouble = nullptr;
}

auto SetRandFuncs(const SetRandFunc& setRandFunc, const GetRandDoubleFunc& getRandDoubleFunc)
    -> void
{
  s_setRand       = setRandFunc;
  s_getRandDouble = getRandDoubleFunc;
}

auto SetRandSeed(const uint64_t seed) -> void
{
  assert(s_setRand != nullptr);
  s_setRand(seed);
}

double GetRandDouble()
{
  assert(s_getRandDouble != nullptr);
  return s_getRandDouble();
}

} // namespace L_SYSTEM
