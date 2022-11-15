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

#include "turtle.h"

#include <iostream>
#include <stdexcept>

namespace L_SYSTEM
{

// TODO(glk) -  Make most of these inline.

Turtle::Turtle(const float turn, const float widthScale)
{
  this->SetDefaultDrawingParams(widthScale, turn);

  // Set up initial frame and position, moving in +X with up in Z
  m_currentState.frame.Identity();
  this->SetGravity(this->GetHeading());

  // Default tropism vector is towards ground, but tropism is disabled
  this->SetTropismVector(-this->GetHeading());
  static constexpr auto DEFAULT_TROPISM = 0.2F;
  this->SetTropismVector(DEFAULT_TROPISM);
  this->DisableTropism();

  //  this->SetWidth(1);
  this->SetWidth(widthScale);
  this->SetColor(0, 0);
  this->SetTexture(0);
  this->SetDefaultDistance(1);
  this->SetDefaultTurnAngle(turn);
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

auto Turtle::SetUp(const Vector& up) -> void
{
  m_currentState.frame[0][2] = up(0);
  m_currentState.frame[1][2] = up(1);
  m_currentState.frame[2][2] = up(2);
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

auto Turtle::SetTropismVector(const float susceptibility) -> void
{
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

auto Turtle::SetDefaultDrawingParams(const float widthScale, const float delta) -> void
{
  m_currentState.widthScale       = widthScale;
  m_currentState.defaultTurnAngle = MATHS::ToRadians(delta);
}

auto Turtle::SetDefaultDistance(const float distance) -> void
{
  m_currentState.defaultDistance = distance;
}

auto Turtle::SetDefaultTurnAngle(const float angle) -> void
{
  m_currentState.defaultTurnAngle = MATHS::ToRadians(angle);
}

// Set color index. This is interpreted by the output generator.
// It may index a color map or define a grayscale value.
auto Turtle::SetColor(const int color) -> void
{
  m_currentState.color = Color(color);
}

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
    m_currentState.frame.Rotate(Matrix::Axis::Z, m_currentState.defaultTurnAngle);
  }
  else
  {
    m_currentState.frame.Rotate(Matrix::Axis::Z, -m_currentState.defaultTurnAngle);
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
    m_currentState.frame.Rotate(Matrix::Axis::Y, m_currentState.defaultTurnAngle);
  }
  else
  {
    m_currentState.frame.Rotate(Matrix::Axis::Y, -m_currentState.defaultTurnAngle);
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
    m_currentState.frame.Rotate(Matrix::Axis::X, m_currentState.defaultTurnAngle);
  }
  else
  {
    m_currentState.frame.Rotate(Matrix::Axis::X, -m_currentState.defaultTurnAngle);
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
  this->Move(m_currentState.defaultDistance);
}

// Move along heading vector for distance.
// Keep track of the motion bounds and apply a tropism correction
// if that is enabled.
auto Turtle::Move(const float distance) -> void
{
  m_currentState.position += distance * this->GetHeading();
  m_boundingBox.Expand(m_currentState.position);

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

  const auto* const vec = reinterpret_cast<const Vector*>(&m_color.rgb);
  // G= .3R + .6G + .1B
  return (0.3F * (*vec)(0)) + (0.6F * (*vec)(1)) + (0.1F * (*vec)(2));
}

// Interpret the color as RGB; map color index into gray scale
auto Color::GetRGBColor() const -> Vector
{
  if (colorType == ColorType::INDEX)
  {
    const auto grayLevel = GetGrayLevel();
    return Vector(grayLevel, grayLevel, grayLevel);
  }

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

} // namespace L_SYSTEM
