#pragma once

#include <cstdint>
#include <functional>

namespace L_SYSTEM
{

using SetRandFunc       = std::function<void(uint64_t seed)>;
using GetRandDoubleFunc = std::function<double()>;

auto SetRandFuncs(const SetRandFunc& setRandFunc, const GetRandDoubleFunc& getRandDoubleFunc)
    -> void;

[[nodiscard]] auto GetRandDouble() -> double;
auto SetRandSeed(uint64_t seed) -> void;

} // namespace L_SYSTEM
