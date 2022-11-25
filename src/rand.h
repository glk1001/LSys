#pragma once

#include <functional>

namespace L_SYSTEM
{

using GetRandDoubleInUnitIntervalFunc = std::function<double()>;

auto SetRandFunc(const GetRandDoubleInUnitIntervalFunc& getRandDoubleFunc) -> void;

[[nodiscard]] auto GetRandDoubleInUnitInterval() -> double;

} // namespace L_SYSTEM
