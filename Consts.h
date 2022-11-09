#pragma once

#include <array>

namespace LSys
{
inline constexpr int MAX_ARGS = 10;
using ArgsArray               = std::array<float, MAX_ARGS>;
}

namespace Maths
{
inline constexpr auto SMALL_FLOAT = 0.00001F;

[[nodiscard]] auto Round(double val, int numDecimalPlaces) -> double;

[[nodiscard]] constexpr auto sq(const double val) -> double
{
  return val * val;
}

inline constexpr auto PI       = 3.14159265358F;
inline constexpr auto TWO_PI   = 2.0F * PI;
inline constexpr auto HALF_PI  = 0.5F * PI;
inline constexpr auto THIRD_PI = PI / 3.0F;

inline constexpr auto DEGREES_90  = 90.0F;
inline constexpr auto DEGREES_180 = 180.0F;
inline constexpr auto DEGREES_360 = 360.0F;

[[nodiscard]] constexpr auto ToRadians(const float degrees) -> float
{
  return degrees * (TWO_PI / DEGREES_360);
}

[[nodiscard]] constexpr auto ToDegrees(const float radians) -> float
{
  return radians * (DEGREES_360 / TWO_PI);
}

} // namespace Maths
