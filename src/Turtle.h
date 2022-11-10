/* Turtle.h - class definition for a 3D turtle geometry engine.
 *
 * $Id: Turtle.h,v 1.4 93/05/12 22:06:48 leech Exp $
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
 * $Log:	Turtle.h,v $
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

#include "Vector.h"

#include <iostream>

namespace LSys
{

// This should be restricted to the scope of class Turtle
struct TropismInfo
{
  Vector tropismVector; // Tropism vector
  float susceptibility; // Susceptibility parameter
  bool flag; // Whether to apply it
};

enum ColorType
{
  COLOR_INDEX,
  COLOR_RGB
};
struct Color
{
  ColorType type;
  union
  {
    int index;
    char rgb[sizeof(Vector)];
  } m_color{};

  Color() = default;
  explicit Color(const int i) : type{COLOR_INDEX} { m_color.index = i; }
  explicit Color(const Vector& v) : type{COLOR_RGB}
  {
    m_color.rgb[0] = static_cast<char>(v[0]);
    m_color.rgb[1] = static_cast<char>(v[1]);
    m_color.rgb[2] = static_cast<char>(v[2]);
  }

  float graylevel() const; // Force interpretation as gray scale [0..1]
  Vector rgbcolor() const; // Force interpretation as RGB [0..1]^3
  bool operator==(const Color&) const;
};

std::ostream& operator<<(std::ostream&, const Color&);

class Turtle
{
public:
  enum direction
  {
    positive,
    negative
  };

  Turtle(float turn = 90, float widthScale = 1);
  ~Turtle();

  void GetDefaults(float* wscale, float* delta);
  void SetDefaults(float wscale, float delta);
  BoundingBox Bounds() const { return m_boundingBox; }

  Vector Location() const { return m_position; }

  // Methods to modify turtle parameters
  Vector CurrentHeading() const;
  void SetHeading(const Vector&);
  Vector CurrentLeft() const;
  void SetLeft(const Vector&);
  Vector CurrentUp() const;
  void SetUp(const Vector&);

  float DefaultDistance() const { return m_defaultDist; }
  void SetDefaultDistance(float d = 1);

  float DefaultTurnAngle() const { return Maths::ToDegrees(m_defaultTurn); }
  void SetDefaultTurnAngle(float a = 90);

  Matrix CurrentFrame() const { return m_frame; }
  void SetFrame(const Matrix&);
  void SetGravity(const Vector&);

  float CurrentWidth() const { return m_width; }
  //      float CurrentUnscaledWidth() const;
  void SetWidth(float width = 1);

  Color CurrentColor() const { return m_color; }
  Color CurrentBackColor() const { return m_colorBack; }
  void IncrementColor();
  void SetColor(int c = 0);
  void SetColor(int c1, int colorBack);
  void SetColor(const Vector&);

  int CurrentTexture() const { return m_texture; } // Line texture
  void SetTexture(int t = 0);

  // Functions for enabling/disabling application of tropism
  // after each segment is drawn.
  TropismInfo GetTropism() const { return m_tropism; }
  void SetTropismVector(const Vector&);
  void SetTropismVector(float susceptibility);
  void DisableTropism();
  void EnableTropism();

  // The turtle may be instructed to rotate a default amount
  // in the positive or negative direction, or a specified
  // number of degrees.
  void Turn(direction);
  void Turn(float alpha);

  void Pitch(direction);
  void Pitch(float alpha);

  void Roll(direction);
  void Roll(float alpha);

  void Reverse(); // Spin around 180 degrees
  void RollHorizontal(); // Align up vector with v

  void Move();
  void Move(float distance);

  void Push();
  void Pop();

  friend std::ostream& operator<<(std::ostream&, Turtle&);

private:
  // Orientation frame of turtle
  Matrix m_frame;
  Matrix* m_framePtr;

  // Position of turtle
  Vector m_position;
  Vector* m_positionPtr;

  // Standard distance to Move turtle
  float m_defaultDist;
  float* m_defaultDistPtr;

  float m_defaultTurn;
  float* m_defaultTurnStack;

  // Line width
  float m_width;
  float* m_widthPtr;

  // Color index
  Color m_color;
  Color* m_colorPtr;
  Color m_colorBack;
  Color* m_colorBackStack;

  // Texture index
  int m_texture;
  int* m_textureStack;

  TropismInfo m_tropism;
  TropismInfo* m_tropismPtr;

  int m_stackPtr; // Stack depth

  BoundingBox m_boundingBox; // Bounding box of turtle path

  Vector m_gravity; // Antigravity vector

  float m_widthScale; // Amount to scale linewidth by
};

std::ostream& operator<<(std::ostream&, TropismInfo&);

} // namespace LSys
