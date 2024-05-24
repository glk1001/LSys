module;

#include <cassert>

module LSys.Rand;

namespace LSYS
{

namespace
{
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
GetRandDoubleInUnitIntervalFunc getRandDouble{};
}

auto SetRandFunc(const GetRandDoubleInUnitIntervalFunc& getRandDoubleFunc) -> void
{
  getRandDouble = getRandDoubleFunc;
}

double GetRandDoubleInUnitInterval()
{
  assert(getRandDouble);
  return getRandDouble();
}

} // namespace LSYS
