/*
 * Copyright (C) 1990, Jonathan P. Leech
 *
 * This software may be freely copied, modified, and redistributed,
 * provided that this copyright notice is preserved on all copies.
 *
 * There is no warranty or other guarantee of fitness for this software,
 * it is provided solely "as is". Bug reports or fixes may be sent
 * to the author, who may or may not act on them as he desires.
 *
 * You may not include this software in a program or other software product
 * without supplying the source, or without informing the end-user that the
 * source is available for no extra charge.
 *
 * If you modify this software, you should include a notice giving the
 * name of the person performing the modification, the date of modification,
 * and the reason for such modification.
 *
 * $Log: vector.h,v $
 * Revision 1.6  93/05/12  22:26:51  leech
 * Reduce warnings from cfront 3.0.1.
 *
 * Revision 1.5  92/06/22  00:26:29  leech
 * Add const specifiers where possible, and overload operator[] on const.
 *
 * Revision 1.4  91/10/10  20:16:13  leech
 * Define M_PI for MPW, which doesn't have it in <math.h>
 *
 * Revision 1.3  91/03/20  10:41:13  leech
 * Support for G++.
 *
 * Revision 1.2  90/10/12  18:48:26  leech
 * First public release.
 *
 */

#pragma once

#include "consts.h"

#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>

namespace LSYS
{
// This is a Cartesian vector class.
// The usual scalar and vector operators are defined.
// Cross product is denoted by a ^ b.

class Vector
{
public:
  auto operator[](const uint32_t i) -> float& { return m_vec[i]; }
  auto operator[](const uint32_t i) const -> float { return m_vec[i]; }
  auto operator()(const uint32_t i) const -> float { return m_vec[i]; }

  Vector() = default;
  Vector(const float x, const float y, const float z) : m_vec{x, y, z} {}

  [[nodiscard]] auto GetMagnitude() -> float
  {
    return std::sqrt(MATHS::sq(m_vec[0]) + MATHS::sq(m_vec[1]) + MATHS::sq(m_vec[2]));
  }

  auto Normalize() -> Vector&
  {
    if (const auto mag = this->GetMagnitude(); mag != 0.0F)
    {
      m_vec[0] /= mag;
      m_vec[1] /= mag;
      m_vec[2] /= mag;
    }
    return *this;
  }

  auto operator+=(const Vector& vec) -> Vector&
  {
    m_vec[0] += vec(0);
    m_vec[1] += vec(1);
    m_vec[2] += vec(2);
    return *this;
  }

  auto operator-=(const Vector& vec) -> Vector&
  {
    m_vec[0] -= vec(0);
    m_vec[1] -= vec(1);
    m_vec[2] -= vec(2);
    return *this;
  }

  auto operator*=(const float val) -> Vector&
  {
    m_vec[0] *= val;
    m_vec[1] *= val;
    m_vec[2] *= val;
    return *this;
  }

  auto operator/=(const float val) -> Vector&
  {
    m_vec[0] /= val;
    m_vec[1] /= val;
    m_vec[2] /= val;
    return *this;
  }

  friend auto operator-(const Vector& vec) -> Vector; // Negation
  friend auto operator*(float val, const Vector& vec) -> Vector; // Scalar multiply
  friend auto operator/(const Vector& vec, float val) -> Vector; // Scalar division
  friend auto operator+(const Vector& vec1, const Vector& vec2) -> Vector; // Vector addition
  friend auto operator-(const Vector& vec1, const Vector& vec2) -> Vector; // Vector subtraction
  friend auto operator^(const Vector& vec1, const Vector& vec2) -> Vector; // Vector cross product
  friend auto operator*(const Vector& vec1, const Vector& vec2) -> float; // Vector inner product
  friend auto operator==(const Vector& vec1, const Vector& vec2) -> bool;

private:
  std::array<float, 3> m_vec{};
};

inline auto operator-(const Vector& vec) -> Vector
{
  return Vector(-vec(0), -vec(1), -vec(2));
}

inline auto operator*(const float val, const Vector& vec) -> Vector
{
  return Vector(val * vec(0), val * vec(1), val * vec(2));
}

inline auto operator/(const Vector& vec, const float val) -> Vector
{
  const auto recip = 1.0F / val;
  return Vector(vec(0) * recip, vec(1) * recip, vec(2) * recip);
}

inline auto operator+(const Vector& vec1, const Vector& vec2) -> Vector
{
  return Vector(vec1(0) + vec2(0), vec1(1) + vec2(1), vec1(2) + vec2(2));
}

inline auto operator-(const Vector& vec1, const Vector& vec2) -> Vector
{
  return Vector(vec1(0) - vec2(0), vec1(1) - vec2(1), vec1(2) - vec2(2));
}

inline auto operator^(const Vector& vec1, const Vector& vec2) -> Vector
{
  return Vector((vec1(1) * vec2(2)) - (vec1(2) * vec2(1)),
                (vec1(2) * vec2(0)) - (vec1(0) * vec2(2)),
                (vec1(0) * vec2(1)) - (vec1(1) * vec2(0)));
}

inline auto operator*(const Vector& vec1, const Vector& vec2) -> float
{
  return (vec1(0) * vec2(0)) + (vec1(1) * vec2(1)) + (vec1(2) * vec2(2));
}

inline auto operator==(const Vector& vec1, const Vector& vec2) -> bool
{
  return vec1.m_vec == vec2.m_vec;
}

inline auto operator!=(const Vector& vec1, const Vector& vec2) -> bool
{
  return not(vec1 == vec2);
}

inline auto Distance(const Vector& vec1, const Vector& vec2) -> float
{
  return std::sqrt(MATHS::sq(vec1(0) - vec2(0)) + MATHS::sq(vec1(1) - vec2(1)) +
                   MATHS::sq(vec1(2) - vec2(2)));
}

auto operator<<(std::ostream& out, const Vector& vec) -> std::ostream&;

// 3x4 transformation matrix (no perspective)
class Matrix
{
public:
  enum class Initialize
  {
    COLUMNS,
    ROWS
  };
  enum class Axis
  {
    X,
    Y,
    Z
  };

  Matrix() = default;
  Matrix(Initialize flag, const Vector& vec1, const Vector& vec2, const Vector& vec3);

  auto operator[](const uint32_t i) -> float* { return m_matrix[i]; }
  auto operator[](const uint32_t i) const -> const float* { return m_matrix[i]; }
  auto Zero() -> Matrix&;
  auto Identity() -> Matrix&;
  auto Rotate(Axis axis, float angle) -> Matrix&;
  auto Rotate(const Vector& vec, float angle) -> Matrix&;
  auto Reverse() -> Matrix&;
  [[nodiscard]] auto operator*(const Vector& vec) const -> Vector;
  [[nodiscard]] auto operator*(const Matrix& otherMatrix) const -> Matrix;

private:
  float m_matrix[3][4];
};

std::ostream& operator<<(std::ostream& out, const Matrix& matrix);

class BoundingBox
{
public:
  BoundingBox() : m_minVec{0.0F, 0.0F, 0.0F}, m_maxVec{0.0F, 0.0F, 0.0F} {}
  explicit BoundingBox(const Vector& vec) : m_minVec{vec}, m_maxVec{vec} {}

  auto Expand(const Vector& point) -> void;
  [[nodiscard]] auto Min() const -> Vector { return m_minVec; }
  [[nodiscard]] auto Max() const -> Vector { return m_maxVec; }

private:
  Vector m_minVec;
  Vector m_maxVec;
};

std::ostream& operator<<(std::ostream& out, const BoundingBox& boundingBox);

} // namespace LSYS
