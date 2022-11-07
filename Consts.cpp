#include <stdhdr.h>
#pragma hdrstop

#include "Utilities/CVSFileInfo.h"
namespace {
  Utilities::CVSFileInfo moduleCVSFileInfo(
    "$RCSfile: Consts.cpp,v $",
    "$Revision: 1.2 $",
    "$Date: 2002/04/04 13:53:54 $",
    "$Name: RenderSource_1_01 $",
    __FILE__,
    __DATE__,
    __TIME__
  );
};

#include <cmath>
#include <assert.h>
#include "Maths/Consts.h"


namespace Maths {

  const double pi= 3.14159265358979323846;

  const float tinyFloat= 1.0e-5;
  const double tinyDouble= 1.0e-14;


  double Round(double val, int numDecimalPlaces)
  {
    assert(numDecimalPlaces >= 0);

    static int lastNumDecimalPlaces= 0;
    static double lastPowerOfTen= 1.0;
//    static double oneOnLastPowerOfTen= 1.0;

    if (numDecimalPlaces != lastNumDecimalPlaces) {
      lastNumDecimalPlaces= numDecimalPlaces;
      lastPowerOfTen= PowerOfTen(numDecimalPlaces);
//      oneOnLastPowerOfTen= 1.0/lastPowerOfTen;
    }

    double roundedVal= double(std::floor(val*lastPowerOfTen))/lastPowerOfTen;
//    if (std::fabs(roundedVal) < oneOnLastPowerOfTen) roundedVal= 0.0;

    return roundedVal;
  }

};

