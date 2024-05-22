module;

#include <cassert>

module LSys.Rand;

namespace LSYS
{

static GetRandDoubleInUnitIntervalFunc s_getRandDouble{};

auto SetRandFunc(const GetRandDoubleInUnitIntervalFunc& getRandDoubleFunc) -> void
{
  s_getRandDouble = getRandDoubleFunc;
}

double GetRandDoubleInUnitInterval()
{
  assert(s_getRandDouble);
  return s_getRandDouble();
}

} // namespace LSYS
