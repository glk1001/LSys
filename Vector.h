/* vector.h - class definitions for vector and transformation Matrix classes.
 *
 * $Id: vector.h,v 1.6 93/05/12 22:26:51 leech Exp $
 *
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
 * $Log:	vector.h,v $
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

#include "Consts.h"

#include <cmath>
#include <iostream>

namespace LSys
{
// This is a Cartesian vector class.
// The usual scalar and vector operators are defined.
// Cross product is denoted by a ^ b.

#define Point Vector
class Vector
{
public:
  float& operator[](int i) { return x[i]; }
  float operator[](int i) const { return x[i]; }
  float operator()(int i) const { return x[i]; }

  Vector(){};
  Vector(const Vector& v)
  {
    x[0] = v[0];
    x[1] = v[1];
    x[2] = v[2];
  }

  Vector(const float* v)
  {
    x[0] = v[0];
    x[1] = v[1];
    x[2] = v[2];
  }
  Vector(float xval, float yval, float zval)
  {
    x[0] = xval;
    x[1] = yval;
    x[2] = zval;
  };

  operator float*() { return x; }

  float magnitude() { return std::sqrt(Maths::sq(x[0]) + Maths::sq(x[1]) + Maths::sq(x[2])); }

  Vector& normalize()
  {
    float mag = this->magnitude();
    if (mag != 0.0)
    {
      mag = 1.0 / mag;
      x[0] *= mag;
      x[1] *= mag;
      x[2] *= mag;
    }
    return *this;
  }

  Vector& operator+=(const Vector& v)
  {
    x[0] += v(0);
    x[1] += v(1);
    x[2] += v(2);
    return *this;
  }

  Vector& operator-=(const Vector& v)
  {
    x[0] -= v(0);
    x[1] -= v(1);
    x[2] -= v(2);
    return *this;
  }

  Vector& operator*=(float s)
  {
    x[0] *= s;
    x[1] *= s;
    x[2] *= s;
    return *this;
  }

  Vector& operator/=(float s)
  {
    x[0] /= s;
    x[1] /= s;
    x[2] /= s;
    return *this;
  }

  friend Vector operator-(const Vector&); // Negation
  friend Vector operator*(float, const Vector&); // Scalar multiply
  friend Vector operator/(const Vector&, float); // Scalar division
  friend Vector operator+(const Vector&, const Vector&); // Vector addition
  friend Vector operator-(const Vector&, const Vector&); // Vector subtraction
  friend Vector operator^(const Vector&, const Vector&); // Vector cross product
  friend float operator*(const Vector&, const Vector&); // Vector inner product
private:
  float x[3];
};

inline Vector operator-(const Vector& v)
{
  return Vector(-v(0), -v(1), -v(2));
}

inline Vector operator*(float s, const Vector& v)
{
  return Vector(s * v(0), s * v(1), s * v(2));
}

inline Vector operator/(const Vector& v, float s)
{
  float recip = 1 / s;
  return Vector(v(0) * recip, v(1) * recip, v(2) * recip);
}

inline Vector operator+(const Vector& a, const Vector& b)
{
  return Vector(a(0) + b(0), a(1) + b(1), a(2) + b(2));
}

inline Vector operator-(const Vector& a, const Vector& b)
{
  return Vector(a(0) - b(0), a(1) - b(1), a(2) - b(2));
}

inline Vector operator^(const Vector& a, const Vector& b)
{
  return Vector(a(1) * b(2) - a(2) * b(1), a(2) * b(0) - a(0) * b(2), a(0) * b(1) - a(1) * b(0));
}

inline float operator*(const Vector& a, const Vector& b)
{
  return a(0) * b(0) + a(1) * b(1) + a(2) * b(2);
}

inline bool operator==(const Vector& a, const Vector& b)
{
  return (a(0) == b(0)) && (a(1) == b(1)) && (a(2) == b(2));
}

inline bool operator!=(const Vector& a, const Vector& b)
{
  return not(a == b);
}

inline float Distance(const Vector& a, const Vector& b)
{
  return std::sqrt(Maths::sq(a(0) - b(0)) + Maths::sq(a(1) - b(1)) + Maths::sq(a(2) - b(2)));
}

std::ostream& operator<<(std::ostream&, const Vector&);


// 3x4 transformation matrix (no perspective)
class Matrix
{
public:
  enum initialize
  {
    columns,
    rows
  };
  enum axis
  {
    x,
    y,
    z
  };

  Matrix() {}
  Matrix(initialize flag, const Vector&, const Vector&, const Vector&);

  float* operator[](int i) { return m[i]; }
  const float* operator[](int i) const { return m[i]; }
  Matrix& zero();
  Matrix& identity();
  Matrix& rotate(axis a, float alpha);
  Matrix& rotate(const Vector&, float alpha);
  Matrix& translate(const Vector&);
  Matrix& reverse();
  Vector operator*(const Vector&) const;
  Matrix operator*(const Matrix&) const;

private:
  float m[3][4];
};

Matrix view_matrix(const Point& eye, const Point& lookat, const Vector& vup);

std::ostream& operator<<(std::ostream&, const Matrix&);


class BoundingBox
{
public:
  BoundingBox() : vmin(0, 0, 0), vmax(0, 0, 0) {}
  BoundingBox(const Vector& p) : vmin(p), vmax(p) {}
  void Expand(const Vector&);
  Vector Min() const { return vmin; }
  Vector Max() const { return vmax; }
  BoundingBox Transform(const Matrix&);

private:
  Vector vmin;
  Vector vmax;
};

std::ostream& operator<<(std::ostream&, const BoundingBox&);

} // namespace LSys
