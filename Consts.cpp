#include "Consts.h"

#include <cassert>
#include <cmath>

namespace Maths
{

auto Round(const double val, const int numDecimalPlaces) -> double
{
  assert(numDecimalPlaces >= 0);

  static auto lastNumDecimalPlaces = 0;
  static auto lastPowerOfTen       = 1.0;
  //    static double oneOnLastPowerOfTen= 1.0;

  if (numDecimalPlaces != lastNumDecimalPlaces)
  {
    lastNumDecimalPlaces = numDecimalPlaces;
    lastPowerOfTen       = std::pow(10.0F, numDecimalPlaces);
    //      oneOnLastPowerOfTen= 1.0/lastPowerOfTen;
  }

  const auto roundedVal = std::floor(val * lastPowerOfTen) / lastPowerOfTen;
  //    if (std::fabs(roundedVal) < oneOnLastPowerOfTen) roundedVal= 0.0;

  return roundedVal;
}

} // namespace Maths
