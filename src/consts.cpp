module;

#include <cassert>
#include <cmath>

module LSys.Consts;

namespace LSYS::MATHS
{

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
auto Round(const float val, const int numDecimalPlaces) -> float
{
  assert(numDecimalPlaces >= 0);

  static constexpr auto TEN          = 10.0F;
  static auto s_lastNumDecimalPlaces = 0;
  static auto s_lastPowerOfTen       = 1.0;
  //    static double oneOnLastPowerOfTen= 1.0;

  if (numDecimalPlaces != s_lastNumDecimalPlaces)
  {
    s_lastNumDecimalPlaces = numDecimalPlaces;
    s_lastPowerOfTen       = std::pow(TEN, static_cast<double>(numDecimalPlaces));
    //      oneOnLastPowerOfTen= 1.0/lastPowerOfTen;
  }

  const auto roundedVal =
      std::floor(static_cast<double>(val) * s_lastPowerOfTen) / s_lastPowerOfTen;
  //    if (std::fabs(roundedVal) < oneOnLastPowerOfTen) roundedVal= 0.0;

  return static_cast<float>(roundedVal);
}

} // namespace LSYS::MATHS
