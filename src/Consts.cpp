#include "Consts.h"

#include <cassert>
#include <cmath>

namespace L_SYSTEM::MATHS
{

auto Round(const float val, const int numDecimalPlaces) -> float
{
  assert(numDecimalPlaces >= 0);

  static auto lastNumDecimalPlaces = 0;
  static auto lastPowerOfTen       = 1.0;
  //    static double oneOnLastPowerOfTen= 1.0;

  if (numDecimalPlaces != lastNumDecimalPlaces)
  {
    lastNumDecimalPlaces = numDecimalPlaces;
    lastPowerOfTen       = std::pow(10.0, static_cast<double>(numDecimalPlaces));
    //      oneOnLastPowerOfTen= 1.0/lastPowerOfTen;
  }

  const auto roundedVal = std::floor(static_cast<double>(val) * lastPowerOfTen) / lastPowerOfTen;
  //    if (std::fabs(roundedVal) < oneOnLastPowerOfTen) roundedVal= 0.0;

  return static_cast<float>(roundedVal);
}

} // namespace L_SYSTEM::MATHS
