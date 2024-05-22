module;

#include <functional>

export module LSys.Rand;

export namespace LSYS
{

using GetRandDoubleInUnitIntervalFunc = std::function<double()>;

auto SetRandFunc(const GetRandDoubleInUnitIntervalFunc& getRandDoubleFunc) -> void;

[[nodiscard]] auto GetRandDoubleInUnitInterval() -> double;

} // namespace LSYS
