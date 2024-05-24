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
 * $Log: Turtle.c,v $
 * Revision 1.5  93/05/12  22:06:47  leech
 * Reduce warnings from cfront 3.0.1.
 *
 * Revision 1.4  92/06/22  00:22:33  leech
 * *** empty log message ***
 *
 * Revision 1.3  91/03/20  10:36:50  leech
 * Better color support. Support for G++.
 *
 * Revision 1.2  90/10/12  18:48:13  leech
 * First public release.
 *
 */

module;

#include "debug.h"

#include <cassert>
#include <iostream>
#include <stdexcept>

module LSys.Turtle;

import LSys.Consts;

namespace LSYS
{
// NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic,
//             cppcoreguidelines-pro-type-union-access)

// TODO(glk) -  Make most of these inline.

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
Turtle::Turtle(const float widthScale, const float turnAngleInDegrees)
{
  ResetDrawingParamsToDefaults();

  m_currentState.widthScale = widthScale;
  SetDefaultTurnAngleInDegrees(turnAngleInDegrees);
}

auto Turtle::ResetDrawingParamsToDefaults() -> void
{
  m_currentState = State{};

  SetDefaultDistance(1.0F);
  static constexpr auto DEFAULT_TURN_ANGLE = 90.0F;
  SetDefaultTurnAngleInDegrees(DEFAULT_TURN_ANGLE);
  SetWidth(1.0F);
  m_currentState.widthScale = 1.0F;
  SetColor(0, 0);
  SetTexture(0);

  // Default tropism vector is towards ground, but tropism is disabled.
  SetTropismVector(-GetHeading());
  static constexpr auto DEFAULT_TROPISM = 0.2F;
  SetTropismSusceptibility(DEFAULT_TROPISM);
  DisableTropism();

  SetGravity(GetHeading());
}

auto Turtle::GetHeading() const -> Vector
{
  // GetHeading is first column of frame
  return Vector{m_currentState.frame[0][0], m_currentState.frame[1][0], m_currentState.frame[2][0]};
}

auto Turtle::SetHeading(const Vector& heading) -> void
{
  m_currentState.frame[0][0] = heading(0);
  m_currentState.frame[1][0] = heading(1);
  m_currentState.frame[2][0] = heading(2);
}

auto Turtle::GetLeft() const -> Vector
{
  // GetLeft is second column of frame
  return Vector{m_currentState.frame[0][1], m_currentState.frame[1][1], m_currentState.frame[2][1]};
}

auto Turtle::SetLeft(const Vector& left) -> void
{
  m_currentState.frame[0][1] = left(0);
  m_currentState.frame[1][1] = left(1);
  m_currentState.frame[2][1] = left(2);
}

auto Turtle::GetUp() const -> Vector
{
  // GetUp is third column of frame
  return Vector{m_currentState.frame[0][2], m_currentState.frame[1][2], m_currentState.frame[2][2]};
}

auto Turtle::SetUp(const Vector& upVec) -> void
{
  m_currentState.frame[0][2] = upVec(0);
  m_currentState.frame[1][2] = upVec(1);
  m_currentState.frame[2][2] = upVec(2);
}

auto Turtle::SetFrame(const Matrix& frame) -> void
{
  m_currentState.frame = frame;
}

auto Turtle::SetGravity(const Vector& gravity) -> void
{
  m_gravity = gravity;
}

auto Turtle::SetTropismVector(const Vector& vector) -> void
{
  m_currentState.tropism.tropismVector = vector;
}

auto Turtle::SetTropismSusceptibility(const float susceptibility) -> void
{
  assert(0.0F <= susceptibility);
  assert(1.0F >= susceptibility);
  m_currentState.tropism.susceptibility = susceptibility;
}

auto Turtle::DisableTropism() -> void
{
  m_currentState.tropism.flag = false;
}

auto Turtle::EnableTropism() -> void
{
  m_currentState.tropism.flag = true;
}

auto Turtle::SetWidth(const float width) -> void
{
  //  width= w * widthScale * relative_line_width;
  m_currentState.width = width;
}

auto Turtle::SetDefaultDistance(const float distance) -> void
{
  m_currentState.defaultDistance = distance;
}

auto Turtle::SetDefaultTurnAngleInDegrees(const float turnAngleInDegrees) -> void
{
  m_currentState.defaultTurnAngleInRadians = MATHS::ToRadians(turnAngleInDegrees);
}

// Set color index. This is interpreted by the output generator.
// It may index a color map or define a grayscale value.
auto Turtle::SetColor(const int color) -> void
{
  m_currentState.color = Color(color);
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
auto Turtle::SetColor(const int color, const int backgroundColor) -> void
{
  m_currentState.color           = Color(color);
  m_currentState.backgroundColor = Color(backgroundColor);
}

auto Turtle::SetColor(const Vector& colorVector) -> void
{
  m_currentState.color = Color{colorVector};
}

auto Turtle::IncrementColor() -> void
{
  if (m_currentState.color.colorType == ColorType::INDEX)
  {
    ++m_currentState.color.m_color.index;
  }
  else
  {
    std::cerr << "Turtle::increment_color(): current color is RGB, not index!\n";
  }
}

// Set texture index. This is interpreted by the output generator.
auto Turtle::SetTexture(const int texture) -> void
{
  m_currentState.texture = texture;
}

auto Turtle::Turn(const Direction direction) -> void
{
  if (direction == Direction::POSITIVE)
  {
    m_currentState.frame.Rotate(Matrix::Axis::Z, m_currentState.defaultTurnAngleInRadians);
  }
  else
  {
    m_currentState.frame.Rotate(Matrix::Axis::Z, -m_currentState.defaultTurnAngleInRadians);
  }
}

auto Turtle::Turn(const float angle) -> void
{
  m_currentState.frame.Rotate(Matrix::Axis::Z, angle);
}

auto Turtle::Pitch(const Direction direction) -> void
{
  if (direction == Direction::POSITIVE)
  {
    m_currentState.frame.Rotate(Matrix::Axis::Y, m_currentState.defaultTurnAngleInRadians);
  }
  else
  {
    m_currentState.frame.Rotate(Matrix::Axis::Y, -m_currentState.defaultTurnAngleInRadians);
  }
}

auto Turtle::Pitch(const float angle) -> void
{
  m_currentState.frame.Rotate(Matrix::Axis::Y, angle);
}

auto Turtle::Roll(const Direction direction) -> void
{
  if (direction == Direction::POSITIVE)
  {
    m_currentState.frame.Rotate(Matrix::Axis::X, m_currentState.defaultTurnAngleInRadians);
  }
  else
  {
    m_currentState.frame.Rotate(Matrix::Axis::X, -m_currentState.defaultTurnAngleInRadians);
  }
}

auto Turtle::Roll(const float angle) -> void
{
  m_currentState.frame.Rotate(Matrix::Axis::X, angle);
}

auto Turtle::Reverse() -> void
{
  m_currentState.frame.Reverse();
}

// Roll the turtle so the left vector is perpendicular to the antigravity
// vector (see pg. 57).
auto Turtle::RollHorizontal() -> void
{
  static constexpr auto TOLERANCE = 1e-4F;

  const auto heading = GetHeading();
  auto left          = m_gravity ^ heading;

  // Don't do anything if heading is too close to the antigravity vector.
  auto magnitude = left.GetMagnitude();
  if (magnitude < TOLERANCE)
  {
    return;
  }

  // new GetLeft vector is normalized v ^ GetHeading
  magnitude = 1.0F / magnitude;
  left[0] *= magnitude;
  left[1] *= magnitude;
  left[2] *= magnitude;

  // New GetUp vector is fixed by heading and left
  const auto up = heading ^ left;

  // Reset the GetLeft and GetUp vectors
  SetLeft(left);
  SetUp(up);
}

auto Turtle::Move() -> void
{
  Move(m_currentState.defaultDistance);
}

// Move along heading vector for distance.
// Keep track of the motion bounds and apply a tropism correction
// if that is enabled.
auto Turtle::Move(const float distance) -> void
{
  m_currentState.position += distance * GetHeading();
  m_boundingBox.Expand(m_currentState.position);

  // TODO(glk) - this seems wrong.
  //   Compare 'Examples/ternary_tree_a' to ABP figure 2.8a
  // Apply tropism, if enabled.
  // This consists of rotating by the vector (GetHeading ^ T).
  if (m_currentState.tropism.flag and (m_currentState.tropism.susceptibility != 0))
  {
    const auto vector = GetHeading() ^ m_currentState.tropism.tropismVector;
    // This is bogus ??????????????????????????????????????????????????????????
    // const float m= vector.GetMagnitude();
    //if (m != 0)
    m_currentState.frame.Rotate(vector, m_currentState.tropism.susceptibility);
  }
}

auto Turtle::Push() -> void
{
  m_stateStack.emplace(m_currentState);
}

auto Turtle::Pop() -> void
{
  if (m_stateStack.empty())
  {
    throw std::runtime_error("Turtle::Pop: turtle stack is empty.");
  }

  m_currentState = m_stateStack.top();
  m_stateStack.pop();
}

std::ostream& operator<<(std::ostream& out, const Turtle& turtle)
{
  out << "Turtle:\n"
      << "\tpos=         " << turtle.GetCurrentState().position << "\n"
      << "\tH  =         " << turtle.GetHeading() << "\n"
      << "\tL  =         " << turtle.GetLeft() << "\n"
      << "\tU  =         " << turtle.GetUp() << "\n"
      << "\tTropism=     " << turtle.GetCurrentState().tropism << "\n"
      << "\tcolor index= " << turtle.GetCurrentState().color << "\n"
      << "\tdefaultDist= " << turtle.GetCurrentState().defaultDistance << "\n"
      << "\twidth=       " << turtle.GetCurrentState().width << "\n";
  return out;
}

auto operator<<(std::ostream& out, const TropismInfo& tropismInfo) -> std::ostream&
{
  if (tropismInfo.flag)
  {
    out << "[enabled";
  }
  else
  {
    out << "[disabled";
  }

  return out << "; vector: " << tropismInfo.tropismVector << " e: " << tropismInfo.susceptibility
             << " ]";
}

// Interpret the color as an intensity; map RGB color to gray.
auto Color::GetGrayLevel() const -> float
{
  if (colorType == ColorType::INDEX)
  {
    return static_cast<float>(m_color.index) / 100.0F;
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto* const vec = reinterpret_cast<const Vector*>(&m_color.rgb);
  // G= .3R + .6G + .1B
  static constexpr auto RED_FRAC   = 0.3F;
  static constexpr auto GREEN_FRAC = 0.6F;
  static constexpr auto BLUE_FRAC  = 0.1F;
  return (RED_FRAC * (*vec)(0)) + (GREEN_FRAC * (*vec)(1)) + (BLUE_FRAC * (*vec)(2));
}

// Interpret the color as RGB; map color index into gray scale
auto Color::GetRGBColor() const -> Vector
{
  if (colorType == ColorType::INDEX)
  {
    const auto grayLevel = GetGrayLevel();
    return Vector(grayLevel, grayLevel, grayLevel);
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return *reinterpret_cast<const Vector*>(&m_color.rgb);
}

[[nodiscard]] auto operator==(const Color& color1, const Color& color2) -> bool
{
  if (color1.colorType != color2.colorType)
  {
    return false;
  }
  if (color1.colorType == ColorType::INDEX)
  {
    return color1.m_color.index == color2.m_color.index;
  }

  return color1.GetRGBColor() == color2.GetRGBColor();
}

std::ostream& operator<<(std::ostream& out, const Color& color)
{
  if (color.colorType == ColorType::INDEX)
  {
    out << "index = " << color.m_color.index;
  }
  else
  {
    out << "RGB = " << color.GetRGBColor();
  }

  return out;
}

// NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic,
//           cppcoreguidelines-pro-type-union-access)

} // namespace LSYS
