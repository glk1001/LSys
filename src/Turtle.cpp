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

#include "Turtle.h"

#include <iostream>

namespace LSys
{

Turtle::Turtle(const float turn, const float widthScale)
{
  this->SetDefaults(widthScale, turn);

  // Set up initial frame and position, moving in +X with up in Z
  m_frame.Identity();
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

// GetHeading is first column of frame
auto Turtle::GetHeading() const -> Vector
{
  return Vector{m_frame[0][0], m_frame[1][0], m_frame[2][0]};
}

// Set heading
auto Turtle::SetHeading(const Vector& heading) -> void
{
  m_frame[0][0] = heading(0);
  m_frame[1][0] = heading(1);
  m_frame[2][0] = heading(2);
}

// GetLeft is second column of frame
auto Turtle::GetLeft() const -> Vector
{
  return Vector{m_frame[0][1], m_frame[1][1], m_frame[2][1]};
}

// Set left
auto Turtle::SetLeft(const Vector& left) -> void
{
  m_frame[0][1] = left(0);
  m_frame[1][1] = left(1);
  m_frame[2][1] = left(2);
}

// GetUp is third column of frame
auto Turtle::GetUp() const -> Vector
{
  return Vector{m_frame[0][2], m_frame[1][2], m_frame[2][2]};
}

// Set up
auto Turtle::SetUp(const Vector& up) -> void
{
  m_frame[0][2] = up(0);
  m_frame[1][2] = up(1);
  m_frame[2][2] = up(2);
}

// Set the whole frame at once
auto Turtle::SetFrame(const Matrix& frame) -> void
{
  m_frame = frame;
}

// Set the antigravity vector
auto Turtle::SetGravity(const Vector& gravity) -> void
{
  m_gravity = gravity;
}

auto Turtle::SetTropismVector(const Vector& vector) -> void
{
  m_tropism.tropismVector = vector;
}

auto Turtle::SetTropismVector(const float susceptibility) -> void
{
  m_tropism.susceptibility = susceptibility;
}

auto Turtle::DisableTropism() -> void
{
  m_tropism.flag = false;
}

auto Turtle::EnableTropism() -> void
{
  m_tropism.flag = true;
}

// Set line width
auto Turtle::SetWidth(const float width) -> void
{
  //  width= w * widthScale * relative_line_width;
  m_width = width;
}

// Set the default drawing parameters
auto Turtle::SetDefaults(const float widthScale, const float delta) -> void
{
  m_widthScale  = widthScale;
  m_defaultTurn = Maths::ToRadians(delta);
}

auto Turtle::SetDefaultDistance(const float distance) -> void
{
  m_defaultDist = distance;
}

auto Turtle::SetDefaultTurnAngle(const float angle) -> void
{
  m_defaultTurn = Maths::ToRadians(angle);
}

// Set color index. This is interpreted by the output generator.
// It may index a color map or define a grayscale value.
auto Turtle::SetColor(const int color) -> void
{
  m_color = Color(color);
}

auto Turtle::SetColor(const int color, const int backgroundColor) -> void
{
  m_color           = Color(color);
  m_backgroundColor = Color(backgroundColor);
}

// Set color index. This is interpreted by the output generator.
auto Turtle::SetTexture(const int texture) -> void
{
  m_texture = texture;
}

// Set RGB color
auto Turtle::SetColor(const Vector& colorVector) -> void
{
  m_color = Color{colorVector};
}

// Increment the current color index
auto Turtle::IncrementColor() -> void
{
  if (m_color.colorType == ColorType::INDEX)
  {
    ++m_color.m_color.index;
  }
  else
  {
    std::cerr << "Turtle::increment_color(): current color is RGB, not index!\n";
  }
}

// Turn left or right (rotate around the up vector)
auto Turtle::Turn(const Direction direction) -> void
{
  if (direction == Direction::POSITIVE)
  {
    m_frame.Rotate(Matrix::Axis::Z, m_defaultTurn);
  }
  else
  {
    m_frame.Rotate(Matrix::Axis::Z, -m_defaultTurn);
  }
}

auto Turtle::Turn(const float angle) -> void
{
  m_frame.Rotate(Matrix::Axis::Z, angle);
}

// Pitch up or down (rotate around the left vector)
auto Turtle::Pitch(const Direction direction) -> void
{
  if (direction == Direction::POSITIVE)
  {
    m_frame.Rotate(Matrix::Axis::Y, m_defaultTurn);
  }
  else
  {
    m_frame.Rotate(Matrix::Axis::Y, -m_defaultTurn);
  }
}

auto Turtle::Pitch(const float angle) -> void
{
  m_frame.Rotate(Matrix::Axis::Y, angle);
}

// Roll left or right (rotate around the heading vector)
auto Turtle::Roll(const Direction direction) -> void
{
  if (direction == Direction::POSITIVE)
  {
    m_frame.Rotate(Matrix::Axis::X, m_defaultTurn);
  }
  else
  {
    m_frame.Rotate(Matrix::Axis::X, -m_defaultTurn);
  }
}

auto Turtle::Roll(const float angle) -> void
{
  m_frame.Rotate(Matrix::Axis::X, angle);
}

// Spin around 180 degrees
auto Turtle::Reverse() -> void
{
  m_frame.Reverse();
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
  this->Move(m_defaultDist);
}

// Move along heading vector for distance.
// Keep track of the motion bounds and apply a tropism correction
// if that is enabled.
auto Turtle::Move(const float distance) -> void
{
  m_position += distance * this->GetHeading();
  m_boundingBox.Expand(m_position);

  // Apply tropism, if enabled.
  // This consists of rotating by the vector (GetHeading ^ T).
  if (m_tropism.flag and (m_tropism.susceptibility != 0))
  {
    const auto vector = GetHeading() ^ m_tropism.tropismVector;
    // This is bogus ??????????????????????????????????????????????????????????
    // const float m= vector.GetMagnitude();
    //if (m != 0)
    m_frame.Rotate(vector, m_tropism.susceptibility);
  }
}

// Save state
// Handle over/underflow gracefully
auto Turtle::Push() -> void
{
  if (m_stackPtr >= (MAX_STACK_DEPTH - 1))
  {
    std::cerr << "Turtle::Push(): stack depth exceeded, lost new frame!\n";
  }
  else
  {
    m_defaultTurnStack.at(m_stackPtr) = m_defaultTurn;
    m_defaultDistPtr.at(m_stackPtr)   = m_defaultDist;
    m_framePtr.at(m_stackPtr)         = m_frame;
    m_tropismPtr.at(m_stackPtr)       = m_tropism;
    m_positionPtr.at(m_stackPtr)      = m_position;
    m_widthPtr.at(m_stackPtr)         = m_width;
    m_colorPtr.at(m_stackPtr)         = m_color;
    m_colorBackStack.at(m_stackPtr)   = m_backgroundColor;
    m_textureStack.at(m_stackPtr)     = m_texture;

    ++m_stackPtr;
  }
}

// Restore state
// Handle over/underflow gracefully
auto Turtle::Pop() -> void
{
  if (m_stackPtr > MAX_STACK_DEPTH)
  {
    std::cerr << "Turtle::Pop(): can't restore lost frame!\n";
  }
  else if (m_stackPtr <= 0)
  {
    std::cerr << "Turtle::Pop(): cannot Pop frame below bottom of stack!\n";
  }
  else
  {
    m_frame           = m_framePtr.at(m_stackPtr - 1);
    m_tropism         = m_tropismPtr.at(m_stackPtr - 1);
    m_position        = m_positionPtr.at(m_stackPtr - 1);
    m_width           = m_widthPtr.at(m_stackPtr - 1);
    m_color           = m_colorPtr.at(m_stackPtr - 1);
    m_backgroundColor = m_colorBackStack.at(m_stackPtr - 1);
    m_texture         = m_textureStack.at(m_stackPtr - 1);
    m_defaultDist     = m_defaultDistPtr.at(m_stackPtr - 1);
    m_defaultTurn     = m_defaultTurnStack.at(m_stackPtr - 1);

    --m_stackPtr;
  }
}

std::ostream& operator<<(std::ostream& out, const Turtle& turtle)
{
  out << "Turtle:\n"
      << "\tpos=         " << turtle.GetPosition() << "\n"
      << "\tH  =         " << turtle.GetHeading() << "\n"
      << "\tL  =         " << turtle.GetLeft() << "\n"
      << "\tU  =         " << turtle.GetUp() << "\n"
      << "\tTropism=     " << turtle.m_tropism << "\n"
      << "\tcolor index= " << turtle.GetColor() << "\n"
      << "\tdefaultDist= " << turtle.GetDefaultDistance() << "\n"
      << "\twidth=       " << turtle.GetWidth() << "\n";
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

} // namespace LSys
