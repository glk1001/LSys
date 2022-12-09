#pragma once

#include <functional>

namespace LSYS
{

using GetRandDoubleInUnitIntervalFunc = std::function<double()>;

auto SetRandFunc(const GetRandDoubleInUnitIntervalFunc& getRandDoubleFunc) -> void;

[[nodiscard]] auto GetRandDoubleInUnitInterval() -> double;

} // namespace LSYS
