#pragma once

namespace Maths {

    inline constexpr double pi = 3.14159265358979323846;

    inline constexpr float tinyFloat = 1.0e-5;
    inline constexpr double tinyDouble = 1.0e-14;


    double Round(double val, int numDecimalPlaces);

    inline double sq(double val) { return val * val; }

    inline constexpr auto PI = 3.14159265358F;
    inline constexpr auto TWO_PI = 2.0F * PI;
    inline constexpr auto HALF_PI = 0.5F * PI;
    inline constexpr auto THIRD_PI = PI / 3.0F;

    inline constexpr auto DEGREES_90 = 90.0F;
    inline constexpr auto DEGREES_180 = 180.0F;
    inline constexpr auto DEGREES_360 = 360.0F;

    [[nodiscard]] constexpr auto ToRadians(const float degrees) -> float {
        return degrees * (TWO_PI / DEGREES_360);
    }

    [[nodiscard]] constexpr auto ToDegrees(const float radians) -> float {
        return radians * (DEGREES_360 / TWO_PI);
    }

} // namespace Maths

