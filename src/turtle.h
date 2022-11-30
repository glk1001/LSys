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
 * $Log: Turtle.h,v $
 * Revision 1.4  93/05/12  22:06:48  leech
 * Reduce warnings from cfront 3.0.1.
 *
 * Revision 1.3  91/03/20  10:40:26  leech
 * Better color support.
 *
 * Revision 1.2  90/10/12  18:48:14  leech
 * First public release.
 *
 */

#pragma once

#include "vector.h"

#include <array>
#include <iostream>
#include <stack>

namespace L_SYSTEM
{

// TODO(glk) - This should be restricted to the scope of class Turtle
struct TropismInfo
{
  Vector tropismVector; // Tropism vector
  float susceptibility; // Susceptibility parameter
  bool flag; // Whether to apply it
};

enum class ColorType
{
  INDEX,
  RGB
};
struct Color
{
  ColorType colorType = ColorType::INDEX;
  union
  {
    int index;
    char rgb[sizeof(Vector)];
  } m_color{};

  Color() = default;
  explicit Color(const int i) : colorType{ColorType::INDEX} { m_color.index = i; }
  explicit Color(const Vector& color) : colorType{ColorType::RGB}
  {
    m_color.rgb[0] = static_cast<char>(color[0]);
    m_color.rgb[1] = static_cast<char>(color[1]);
    m_color.rgb[2] = static_cast<char>(color[2]);
  }

  [[nodiscard]] auto GetGrayLevel() const -> float; // Force interpretation as gray scale [0..1]
  [[nodiscard]] auto GetRGBColor() const -> Vector; // Force interpretation as RGB [0..1]^3

  friend auto operator==(const Color& color1, const Color& color2) -> bool;
};

auto operator<<(std::ostream& out, const Color& color) -> std::ostream&;

class Turtle
{
public:
  enum class Direction
  {
    POSITIVE,
    NEGATIVE
  };

  explicit Turtle(float widthScale = 1.0F, float turnAngleInDegrees = 90.0F);

  [[nodiscard]] auto GetBoundingBox() const -> BoundingBox { return m_boundingBox; }

  struct State
  {
    Vector position{0.0F, 0.0F, 0.0F};
    Matrix frame{};
    float defaultDistance           = 0.0F;
    float defaultTurnAngleInRadians = 90.0F;
    float width                     = 1.0F;
    float widthScale                = 1.0F;
    Color color{};
    Color backgroundColor{};
    int texture = 0;
    TropismInfo tropism{};
  };
  [[nodiscard]] auto GetCurrentState() const -> const State& { return m_currentState; }

  auto ResetDrawingParamsToDefaults() -> void;

  // Methods to modify turtle parameters
  [[nodiscard]] auto GetHeading() const -> Vector;
  auto SetHeading(const Vector& heading) -> void;
  [[nodiscard]] auto GetLeft() const -> Vector;
  auto SetLeft(const Vector& left) -> void;
  [[nodiscard]] auto GetUp() const -> Vector;
  auto SetUp(const Vector& up) -> void;

  auto SetDefaultDistance(float distance = 1.0F) -> void;
  auto SetDefaultTurnAngleInDegrees(float turnAngleInDegrees = 90.0F) -> void;

  auto SetFrame(const Matrix& frame) -> void;
  auto SetGravity(const Vector& gravity) -> void;
  auto SetWidth(float width = 1.0F) -> void;
  auto SetTexture(int texture = 0) -> void;

  auto IncrementColor() -> void;
  auto SetColor(int color = 0) -> void;
  auto SetColor(int color, int backgroundColor) -> void;
  auto SetColor(const Vector& colorVector) -> void;

  // Functions for enabling/disabling application of tropism
  // after each segment is drawn.
  auto SetTropismVector(const Vector& vector) -> void;
  auto SetTropismSusceptibility(float susceptibility) -> void;
  auto DisableTropism() -> void;
  auto EnableTropism() -> void;

  // The turtle may be instructed to rotate a default amount
  // in the POSITIVE or NEGATIVE direction, or a specified
  // number of degrees.
  auto Turn(Direction direction) -> void;
  auto Turn(float angle) -> void;

  auto Pitch(Direction direction) -> void;
  auto Pitch(float angle) -> void;

  auto Roll(Direction direction) -> void;
  auto Roll(float angle) -> void;

  auto Reverse() -> void; // Spin around 180 degrees
  auto RollHorizontal() -> void; // Align up vector with v

  auto Move() -> void;
  auto Move(float distance) -> void;

  auto Push() -> void;
  auto Pop() -> void;

  friend auto operator<<(std::ostream& out, const Turtle& turtle) -> std::ostream&;

private:
  State m_currentState{};
  std::stack<State> m_stateStack{};

  BoundingBox m_boundingBox{}; // Bounding box of turtle path
  Vector m_gravity{0.0F, 0.0F, 0.0F}; // Antigravity vector
};

auto operator<<(std::ostream& out, const TropismInfo& tropismInfo) -> std::ostream&;

} // namespace L_SYSTEM
