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
 * $Log: vector.c,v $
 * Revision 1.6  93/05/12  22:06:49  leech
 * Reduce warnings from cfront 3.0.1.
 *
 * Revision 1.5  92/06/22  00:22:40  leech
 * Add const specifiers where possible.
 *
 * Revision 1.4  91/10/10  20:15:37  leech
 * Include <stdlib.h> for MPW. Avoid namespace clashes between enumeration
 * 'Axis' and variable 'Axis' which exercise a bug in Sun C++.
 *
 * Revision 1.3  91/03/20  10:37:08  leech
 * Include stream.h to ensure we have it.
 *
 * Revision 1.2  90/10/12  18:48:25  leech
 * First public release.
 *
 */

#include "vector.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>

namespace LSYS
{

static constexpr uint32_t X = 0U;
static constexpr uint32_t Y = 1U;
static constexpr uint32_t Z = 2U;
static constexpr uint32_t W = 3U;

// Expand the box to include the specified point
auto BoundingBox::Expand(const Vector& point) -> void
{
  for (auto i = X; i <= Z; ++i)
  {
    if (point(i) > m_maxVec[i])
    {
      m_maxVec[i] = point(i);
    }
    else if (point(i) < m_minVec[i])
    {
      m_minVec[i] = point(i);
    }
  }
}

std::ostream& operator<<(std::ostream& out, const BoundingBox& boundingBox)
{
  return out << "[ min: " << boundingBox.Min() << " max: " << boundingBox.Max() << " ]";
}

// Initialize a matrix by either COLUMNS or ROWS as specified;
// assumes an underlying Identity matrix.
Matrix::Matrix(const Initialize flag, const Vector& vec1, const Vector& vec2, const Vector& vec3)
{
  for (auto i = X; i <= Z; ++i)
  {
    if (flag == Matrix::Initialize::COLUMNS)
    {
      m_matrix[i][X] = vec1(i);
      m_matrix[i][Y] = vec2(i);
      m_matrix[i][Z] = vec3(i);
    }
    else
    { // flag = Matrix::ROWS
      m_matrix[X][i] = vec1(i);
      m_matrix[Y][i] = vec2(i);
      m_matrix[Z][i] = vec3(i);
    }
  }
  m_matrix[X][W] = 0.0F;
  m_matrix[Y][W] = 0.0F;
  m_matrix[Z][W] = 0.0F;
}

// Create a Zero matrix
auto Matrix::Zero() -> Matrix&
{
  for (auto i = X; i <= Z; ++i)
  {
    for (auto j = X; j <= W; ++j)
    {
      m_matrix[i][j] = 0.0F;
    }
  }

  return *this;
}

// Create an Identity matrix
auto Matrix::Identity() -> Matrix&
{
  for (auto i = X; i <= Z; ++i)
  {
    for (auto j = X; j <= W; ++j)
    {
      m_matrix[i][j] = (i == j) ? 1.0F : 0.0F;
    }
  }

  return *this;
}

static auto CosSin(float& cosVal, float& sinVal, float const angle) -> void
{
  // Tolerance before assuming the rotation is aligned with axes
  static constexpr auto TOLERANCE = 1e-10F;

  cosVal = std::cos(angle);
  sinVal = std::sin(angle);

  if (cosVal > (1.0F - TOLERANCE))
  {
    cosVal = 1;
    sinVal = 0;
  }
  else if (cosVal < (-1.0F + TOLERANCE))
  {
    cosVal = -1;
    sinVal = 0;
  }

  if (sinVal > (1.0F - TOLERANCE))
  {
    cosVal = 0;
    sinVal = 1;
  }
  else if (sinVal < (-1.0F + TOLERANCE))
  {
    cosVal = 0;
    sinVal = -1;
  }
}

// Post-multiply by axis rotation around specified Axis (x,y,z) of 'angle' radians
// Note: this is in axis right-handed coordinate system, and transformations
//  are performed by multiplying [matrix][pt].
auto Matrix::Rotate(const Axis axis, const float angle) -> Matrix&
{
  float cosAngle;
  float sinAngle;
  CosSin(cosAngle, sinAngle, angle);

  auto rotatedMatrix = Matrix{};

  switch (axis)
  {
    case Axis::X:
      rotatedMatrix[X][X] = 1;
      rotatedMatrix[X][Y] = 0;
      rotatedMatrix[X][Z] = 0;
      rotatedMatrix[X][W] = 0;
      rotatedMatrix[Y][X] = 0;
      rotatedMatrix[Y][Y] = cosAngle;
      rotatedMatrix[Y][Z] = -sinAngle;
      rotatedMatrix[Y][W] = 0;
      rotatedMatrix[Z][X] = 0;
      rotatedMatrix[Z][Y] = sinAngle;
      rotatedMatrix[Z][Z] = cosAngle;
      rotatedMatrix[Z][W] = 0;
      break;
    case Axis::Y:
      rotatedMatrix[X][X] = cosAngle;
      rotatedMatrix[X][Y] = 0;
      rotatedMatrix[X][Z] = sinAngle;
      rotatedMatrix[X][W] = 0;
      rotatedMatrix[Y][X] = 0;
      rotatedMatrix[Y][Y] = 1;
      rotatedMatrix[Y][Z] = 0;
      rotatedMatrix[Y][W] = 0;
      rotatedMatrix[Z][X] = -sinAngle;
      rotatedMatrix[Z][Y] = 0;
      rotatedMatrix[Z][Z] = cosAngle;
      rotatedMatrix[Z][W] = 0;
      break;
    case Axis::Z:
      rotatedMatrix[X][X] = cosAngle;
      rotatedMatrix[X][Y] = -sinAngle;
      rotatedMatrix[X][Z] = 0;
      rotatedMatrix[X][W] = 0;
      rotatedMatrix[Y][X] = sinAngle;
      rotatedMatrix[Y][Y] = cosAngle;
      rotatedMatrix[Y][Z] = 0;
      rotatedMatrix[Y][W] = 0;
      rotatedMatrix[Z][X] = 0;
      rotatedMatrix[Z][Y] = 0;
      rotatedMatrix[Z][Z] = 1;
      rotatedMatrix[Z][W] = 0;
      break;
    default:
      std::cerr << "Matrix::rotate: unknown axis = " << static_cast<int>(axis) << "\n";
      ::exit(1);
      break;
  }

  *this = *this * rotatedMatrix;

  return *this;
}

// Post-multiply by a rotation around the specified axis of 'angle' radians.
auto Matrix::Rotate(const Vector& vec, const float angle) -> Matrix&
{
  float cosAngle;
  float sinAngle;
  CosSin(cosAngle, sinAngle, angle);

  auto cMatrix       = Matrix{};
  auto sMatrix       = Matrix{};
  auto rotatedMatrix = Matrix{};

  // Rotation axis must be normalized
  auto rotationAxis = vec;
  rotationAxis.Normalize();

  // Matrix effecting P' = (1 - cos alpha) (P dot A) A
  // Rij = (1 - cos alpha) Ai Aj
  cMatrix.Zero();
  for (auto i = X; i <= Z; ++i)
  {
    for (auto j = X; j <= Z; ++j)
    {
      cMatrix[i][j] = (1 - cosAngle) * rotationAxis(i) * rotationAxis(j);
    }
  }

  // matrix effecting P' = (cos alpha) P + (sin alpha) P ^ A
  sMatrix.Zero();
  sMatrix[X][X] = cosAngle;
  sMatrix[X][Y] = -sinAngle * rotationAxis(Z);
  sMatrix[X][Z] = sinAngle * rotationAxis(Y);
  sMatrix[Y][X] = sinAngle * rotationAxis(Z);
  sMatrix[Y][Y] = cosAngle;
  sMatrix[Y][Z] = -sinAngle * rotationAxis(X);
  sMatrix[Z][X] = -sinAngle * rotationAxis(Y);
  sMatrix[Z][Y] = sinAngle * rotationAxis(X);
  sMatrix[Z][Z] = cosAngle;

  for (auto i = X; i <= Z; ++i)
  {
    for (auto j = X; j <= W; ++j)
    {
      rotatedMatrix[i][j] = sMatrix[i][j] + cMatrix[i][j];
    }
  }

  *this = *this * rotatedMatrix;

  return *this;
}

// Negate first and second columns of the matrix (heading and
// left vectors), effecting a turnaround.
auto Matrix::Reverse() -> Matrix&
{
  m_matrix[X][X] = -m_matrix[X][X];
  m_matrix[X][Y] = -m_matrix[X][Y];
  m_matrix[Y][X] = -m_matrix[Y][X];
  m_matrix[Y][Y] = -m_matrix[Y][Y];
  m_matrix[Z][X] = -m_matrix[Z][X];
  m_matrix[Z][Y] = -m_matrix[Z][Y];

  return *this;
}

auto Matrix::operator*(const Vector& vec) const -> Vector
{
  return Vector(
      m_matrix[X][X] * vec(X) + m_matrix[X][Y] * vec(Y) + m_matrix[X][Z] * vec(Z) + m_matrix[X][W],
      m_matrix[Y][X] * vec(X) + m_matrix[Y][Y] * vec(Y) + m_matrix[Y][Z] * vec(Z) + m_matrix[Y][W],
      m_matrix[Z][X] * vec(X) + m_matrix[Z][Y] * vec(Y) + m_matrix[Z][Z] * vec(Z) + m_matrix[Z][W]);
}

auto Matrix::operator*(const Matrix& otherMatrix) const -> Matrix
{
  auto res = Matrix{};

  for (auto i = X; i <= Z; ++i)
  {
    res[i][X] = m_matrix[i][X] * otherMatrix[X][X] + m_matrix[i][Y] * otherMatrix[Y][X] +
                m_matrix[i][Z] * otherMatrix[Z][X];
    res[i][Y] = m_matrix[i][X] * otherMatrix[X][Y] + m_matrix[i][Y] * otherMatrix[Y][Y] +
                m_matrix[i][Z] * otherMatrix[Z][Y];
    res[i][Z] = m_matrix[i][X] * otherMatrix[X][Z] + m_matrix[i][Y] * otherMatrix[Y][Z] +
                m_matrix[i][Z] * otherMatrix[Z][Z];
    res[i][W] = m_matrix[i][X] * otherMatrix[X][W] + m_matrix[i][Y] * otherMatrix[Y][W] +
                m_matrix[i][Z] * otherMatrix[Z][W] + m_matrix[i][W];
  }

  return res;
}

std::ostream& operator<<(std::ostream& out, const Vector& vec)
{
  return out << "( " << vec(X) << ' ' << vec(Y) << ' ' << vec(Z) << " )";
}

std::ostream& operator<<(std::ostream& out, const Matrix& matrix)
{
  for (auto i = X; i <= Z; ++i)
  {
    out << '\t' << "( " << std::setw(8) << matrix[i][X] << ' ' << std::setw(8) << matrix[i][Y]
        << ' ' << std::setw(8) << matrix[i][Z] << ' ' << std::setw(8) << matrix[i][W] << " )\n";
  }

  return out;
}

} // namespace LSYS
