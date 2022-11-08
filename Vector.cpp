/* vector.c - methods for vector and transformation Matrix classes.
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
 * $Log:	vector.c,v $
 * Revision 1.6  93/05/12  22:06:49  leech
 * Reduce warnings from cfront 3.0.1.
 *
 * Revision 1.5  92/06/22  00:22:40  leech
 * Add const specifiers where possible.
 *
 * Revision 1.4  91/10/10  20:15:37  leech
 * Include <stdlib.h> for MPW. Avoid namespace clashes between enumeration
 * 'axis' and variable 'axis' which exercise a bug in Sun C++.
 *
 * Revision 1.3  91/03/20  10:37:08  leech
 * Include stream.h to ensure we have it.
 *
 * Revision 1.2  90/10/12  18:48:25  leech
 * First public release.
 *
 */
//static char RCSid[] = "$Id: vector.c,v 1.6 93/05/12 22:06:49 leech Exp $";

#include "Vector.h"

#include <iomanip>
#include <stdlib.h>


namespace LSys
{


namespace
{
const int X = 0;
const int Y = 1;
const int Z = 2;
const int W = 3;
};


// Expand the box to include the specified point
void BoundingBox::Expand(const Vector& p)
{
  for (int i = X; i <= Z; i++)
  {
    if (p(i) > vmax[i])
      vmax[i] = p(i);
    else if (p(i) < vmin[i])
      vmin[i] = p(i);
  }
}


// Transform the box through the matrix, return a new box. The new
// box will in general not bound the object as well.
BoundingBox BoundingBox::Transform(const Matrix& m)
{
  // For each combination of object (min,max):
  BoundingBox bnew(m * vmin); // 000

  Vector v = m * Vector(vmin[X], vmin[Y], vmax[Z]); // 001
  bnew.Expand(v);

  v = m * Vector(vmin[X], vmax[Y], vmin[Z]); // 010
  bnew.Expand(v);

  v = m * Vector(vmin[X], vmax[Y], vmax[Z]); // 011
  bnew.Expand(v);

  v = m * Vector(vmax[X], vmin[Y], vmin[Z]); // 100
  bnew.Expand(v);

  v = m * Vector(vmax[X], vmin[Y], vmax[Z]); // 101
  bnew.Expand(v);

  v = m * Vector(vmax[X], vmax[Y], vmin[Z]); // 110
  bnew.Expand(v);

  bnew.Expand(m * vmax); // 111

  return bnew;
}


std::ostream& operator<<(std::ostream& s, const BoundingBox& m)
{
  return s << "[ min: " << m.Min() << " max: " << m.Max() << " ]";
}


// Initialize a matrix by either columns or rows as specified;
// assumes an underlying identity matrix.
Matrix::Matrix(initialize flag, const Vector& u, const Vector& v, const Vector& w)
{
  for (int i = X; i <= Z; i++)
  {
    if (flag == Matrix::columns)
    {
      m[i][X] = u(i);
      m[i][Y] = v(i);
      m[i][Z] = w(i);
    }
    else
    { // flag = Matrix::rows
      m[X][i] = u(i);
      m[Y][i] = v(i);
      m[Z][i] = w(i);
    }
  }
  m[X][W] = m[Y][W] = m[Z][W] = 0;
}


// Create a zero matrix
Matrix& Matrix::zero()
{
  for (int i = X; i <= Z; i++)
    for (int j = X; j <= W; j++)
      m[i][j] = 0;

  return *this;
}


// Create an identity matrix
Matrix& Matrix::identity()
{
  for (int i = X; i <= Z; i++)
    for (int j = X; j <= W; j++)
      m[i][j] = (i == j);

  return *this;
}


static void sincos(float& cval, float& sval, float alpha)
{
  // Tolerance before assuming the rotation is aligned with axes
  const float tolerance = 1e-10;

  cval = std::cos(alpha);
  sval = std::sin(alpha);

  if (cval > 1 - tolerance)
  {
    cval = 1;
    sval = 0;
  }
  else if (cval < -1 + tolerance)
  {
    cval = -1;
    sval = 0;
  }

  if (sval > 1 - tolerance)
  {
    cval = 0;
    sval = 1;
  }
  else if (sval < -1 + tolerance)
  {
    cval = 0;
    sval = -1;
  }
}


// Postmultiply by a rotation around specified axis (x,y,z) of alpha radians
// Note: this is in a right handed coordinate system, and transformations
//  are performed by multiplying [matrix][pt].
Matrix& Matrix::rotate(axis a, float alpha)
{
  float ca, sa;
  sincos(ca, sa, alpha);
  Matrix r;

  switch (a)
  {
    case x:
      r[X][X] = 1;
      r[X][Y] = 0;
      r[X][Z] = 0;
      r[X][W] = 0;
      r[Y][X] = 0;
      r[Y][Y] = ca;
      r[Y][Z] = -sa;
      r[Y][W] = 0;
      r[Z][X] = 0;
      r[Z][Y] = sa;
      r[Z][Z] = ca;
      r[Z][W] = 0;
      break;
    case y:
      r[X][X] = ca;
      r[X][Y] = 0;
      r[X][Z] = sa;
      r[X][W] = 0;
      r[Y][X] = 0;
      r[Y][Y] = 1;
      r[Y][Z] = 0;
      r[Y][W] = 0;
      r[Z][X] = -sa;
      r[Z][Y] = 0;
      r[Z][Z] = ca;
      r[Z][W] = 0;
      break;
    case z:
      r[X][X] = ca;
      r[X][Y] = -sa;
      r[X][Z] = 0;
      r[X][W] = 0;
      r[Y][X] = sa;
      r[Y][Y] = ca;
      r[Y][Z] = 0;
      r[Y][W] = 0;
      r[Z][X] = 0;
      r[Z][Y] = 0;
      r[Z][Z] = 1;
      r[Z][W] = 0;
      break;
    default:
      std::cerr << "Matrix::rotate: unknown axis = " << (int)a << std::endl;
      exit(1);
      break;
  }

  *this = *this * r;

  return *this;
}


// Postmultiply by a rotation around the specified axis _axis
// of alpha radians.
Matrix& Matrix::rotate(const Vector& _axis, float alpha)
{
  float ca, sa;
  sincos(ca, sa, alpha);
  Matrix c, s, r;

  // rotation axis must be normalized
  Vector a = _axis;
  a.normalize();

  // matrix effecting P' = (1 - cos alpha) (P dot A) A
  // Rij = (1 - cos alpha) Ai Aj
  c.zero();
  int i, j;
  for (i = X; i <= Z; i++)
    for (j = X; j <= Z; j++)
      c[i][j] = (1 - ca) * a(i) * a(j);

  // matrix effecting P' = (cos alpha) P + (sin alpha) P ^ A
  s.zero();
  s[X][X] = ca;
  s[X][Y] = -sa * a(Z);
  s[X][Z] = sa * a(Y);
  s[Y][X] = sa * a(Z);
  s[Y][Y] = ca;
  s[Y][Z] = -sa * a(X);
  s[Z][X] = -sa * a(Y);
  s[Z][Y] = sa * a(X);
  s[Z][Z] = ca;

  for (i = X; i <= Z; i++)
    for (j = X; j <= W; j++)
      r[i][j] = s[i][j] + c[i][j];

  *this = *this * r;

  return *this;
}


// Postmultiply by a translation t.
Matrix& Matrix::translate(const Vector& t)
{
  Matrix r;

  r.identity();
  r[X][W] = t(X);
  r[Y][W] = t(Y);
  r[Z][W] = t(Z);

  *this = *this * r;
  return *this;
}


inline void swap(float& a, float& b)
{
  float tmp = a;
  a         = b;
  b         = tmp;
}


// Negate first and second columns of the matrix (heading and
// left vectors), effecting a turnaround.
Matrix& Matrix::reverse()
{
  m[X][X] = -m[X][X];
  m[X][Y] = -m[X][Y];
  m[Y][X] = -m[Y][X];
  m[Y][Y] = -m[Y][Y];
  m[Z][X] = -m[Z][X];
  m[Z][Y] = -m[Z][Y];

  return *this;
}


// Matrix * Vector (transform vector s through the transformation)
Vector Matrix::operator*(const Vector& s) const
{
  return Vector(m[X][X] * s(X) + m[X][Y] * s(Y) + m[X][Z] * s(Z) + m[X][W],
                m[Y][X] * s(X) + m[Y][Y] * s(Y) + m[Y][Z] * s(Z) + m[Y][W],
                m[Z][X] * s(X) + m[Z][Y] * s(Y) + m[Z][Z] * s(Z) + m[Z][W]);
}


// Matrix * Matrix (preconcatenate transformation b by postmultiplication)
Matrix Matrix::operator*(const Matrix& b) const
{
  Matrix res;

  for (int i = X; i <= Z; i++)
  {
    res[i][X] = m[i][X] * b[X][X] + m[i][Y] * b[Y][X] + m[i][Z] * b[Z][X];
    res[i][Y] = m[i][X] * b[X][Y] + m[i][Y] * b[Y][Y] + m[i][Z] * b[Z][Y];
    res[i][Z] = m[i][X] * b[X][Z] + m[i][Y] * b[Y][Z] + m[i][Z] * b[Z][Z];
    res[i][W] = m[i][X] * b[X][W] + m[i][Y] * b[Y][W] + m[i][Z] * b[Z][W] + m[i][W];
  }

  return res;
}


// Construct a viewing matrix which effects a translation of the eye to
// the origin, the lookat - eye vector to the -Z axis, and the vup vector
// to the +Y axis. This is drawn from PPHIGS but done in RHS.
Matrix view_matrix(const Point& eye, const Point& lookat, const Vector& vup)
{
  Vector w = eye - lookat; // Look vector

  Matrix trans;
  trans.identity();
  trans.translate(-eye); // Translate eye to origin

  Vector u = vup ^ w; // Construct an orthogonal basis whose up
  Vector v = w ^ u; //  vector v is as close to vup as possible

  u.normalize();
  v.normalize();
  w.normalize();

  Matrix frame(Matrix::rows, u, v, w);

  return frame * trans;
}


// Print a Vector
std::ostream& operator<<(std::ostream& s, const Vector& v)
{
  return s << "( " << v(X) << ' ' << v(Y) << ' ' << v(Z) << " )";
}


// Print a Matrix
std::ostream& operator<<(std::ostream& s, const Matrix& m)
{
  for (int i = X; i <= Z; i++)
    s << '\t' << "( " << std::setw(8) << m[i][X] << ' ' << std::setw(8) << m[i][Y] << ' '
      << std::setw(8) << m[i][Z] << ' ' << std::setw(8) << m[i][W] << " )\n";

  return s;
}


}; // namespace LSys
