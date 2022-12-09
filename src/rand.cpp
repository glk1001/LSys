#include "rand.h"

#include <cassert>

namespace LSYS
{

static GetRandDoubleInUnitIntervalFunc s_getRandDouble = nullptr;

auto SetRandFunc(const GetRandDoubleInUnitIntervalFunc& getRandDoubleFunc) -> void
{
  s_getRandDouble = getRandDoubleFunc;
}

double GetRandDoubleInUnitInterval()
{
  assert(s_getRandDouble != nullptr);
  return s_getRandDouble();
}

} // namespace LSYS
