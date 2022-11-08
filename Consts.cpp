#include "Consts.h"

#include <assert.h>
#include <cmath>


namespace Maths
{

double Round(double val, int numDecimalPlaces)
{
  assert(numDecimalPlaces >= 0);

  static int lastNumDecimalPlaces = 0;
  static double lastPowerOfTen    = 1.0;
  //    static double oneOnLastPowerOfTen= 1.0;

  if (numDecimalPlaces != lastNumDecimalPlaces)
  {
    lastNumDecimalPlaces = numDecimalPlaces;
    lastPowerOfTen       = std::pow(10.0F, numDecimalPlaces);
    //      oneOnLastPowerOfTen= 1.0/lastPowerOfTen;
  }

  double roundedVal = double(std::floor(val * lastPowerOfTen)) / lastPowerOfTen;
  //    if (std::fabs(roundedVal) < oneOnLastPowerOfTen) roundedVal= 0.0;

  return roundedVal;
}

}; // namespace Maths
