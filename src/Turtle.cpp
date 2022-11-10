/* Turtle.c - methods for a 3D turtle geometry engine.
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
 * $Log:	Turtle.c,v $
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
//static char RCSid[]= "$Id: Turtle.c,v 1.5 93/05/12 22:06:47 leech Exp $";

#include "Turtle.h"

#include <iostream>


namespace LSys
{


// Default stack depth
const int stackdepth = 100;

// Line width relative to unit Move
const float relative_line_width = 1;


Turtle::Turtle(float delta, float wscale) : m_position(0, 0, 0), m_boundingBox(m_position)
{
  // Allocate and initialize turtle state stack
  m_stackPtr         = 0;
  m_framePtr         = new Matrix[stackdepth];
  m_positionPtr      = new Vector[stackdepth];
  m_tropismPtr       = new TropismInfo[stackdepth];
  m_defaultDistPtr   = new float[stackdepth];
  m_defaultTurnStack = new float[stackdepth];
  m_widthPtr         = new float[stackdepth];
  m_colorPtr         = new Color[stackdepth];
  m_colorBackStack   = new Color[stackdepth];
  m_textureStack     = new int[stackdepth];

  this->SetDefaults(wscale, delta);

  // Set up initial frame and position, moving in +X with up in Z
  m_frame.identity();
  this->SetGravity(this->CurrentHeading());

  // Default tropism vector is towards ground, but tropism is disabled
  this->SetTropismVector(-this->CurrentHeading());
  this->SetTropismVector(0.2F);
  this->DisableTropism();

  //  this->SetWidth(1);
  this->SetWidth(wscale);
  this->SetColor(0, 0);
  this->SetTexture(0);
  this->SetDefaultDistance(1);
  this->SetDefaultTurnAngle(delta);
}


Turtle::~Turtle()
{
  delete[] m_framePtr;
  delete[] m_positionPtr;
  delete[] m_tropismPtr;
  delete[] m_defaultTurnStack;
  delete[] m_defaultDistPtr;
  delete[] m_widthPtr;
  delete[] m_colorPtr;
  delete[] m_colorBackStack;
  delete[] m_textureStack;
}


// CurrentHeading is first column of frame
Vector Turtle::CurrentHeading() const
{
  return Vector(m_frame[0][0], m_frame[1][0], m_frame[2][0]);
}


// Set heading
void Turtle::SetHeading(const Vector& CurrentHeading)
{
  m_frame[0][0] = CurrentHeading(0);
  m_frame[1][0] = CurrentHeading(1);
  m_frame[2][0] = CurrentHeading(2);
}


// CurrentLeft is second column of frame
Vector Turtle::CurrentLeft() const
{
  return Vector(m_frame[0][1], m_frame[1][1], m_frame[2][1]);
}


// Set left
void Turtle::SetLeft(const Vector& l)
{
  m_frame[0][1] = l(0);
  m_frame[1][1] = l(1);
  m_frame[2][1] = l(2);
}


// CurrentUp is third column of frame
Vector Turtle::CurrentUp() const
{
  return Vector(m_frame[0][2], m_frame[1][2], m_frame[2][2]);
}


// Set up
void Turtle::SetUp(const Vector& u)
{
  m_frame[0][2] = u(0);
  m_frame[1][2] = u(1);
  m_frame[2][2] = u(2);
}


// Set the whole frame at once
void Turtle::SetFrame(const Matrix& m)
{
  m_frame = m;
}


// Set the antigravity vector
void Turtle::SetGravity(const Vector& gvec)
{
  m_gravity = gvec;
}


void Turtle::SetTropismVector(const Vector& vec)
{
  m_tropism.tropismVector = vec;
}


void Turtle::SetTropismVector(const float susceptibility)
{
  m_tropism.susceptibility = susceptibility;
}


void Turtle::DisableTropism()
{
  m_tropism.flag = false;
}


void Turtle::EnableTropism()
{
  m_tropism.flag = true;
}

// Set line width
void Turtle::SetWidth(const float width)
{
  //  width= w * widthScale * relative_line_width;
  m_width = width;
}


// Get the default drawing parameters
void Turtle::GetDefaults(float* wscale, float* delta)
{
  *wscale = m_widthScale;
  *delta  = Maths::ToDegrees(m_defaultTurn);
}


// Set the default drawing parameters
void Turtle::SetDefaults(float wscale, float delta)
{
  m_widthScale  = wscale;
  m_defaultTurn = Maths::ToRadians(delta);
}


void Turtle::SetDefaultDistance(float d)
{
  m_defaultDist = d;
}


void Turtle::SetDefaultTurnAngle(float a)
{
  m_defaultTurn = Maths::ToRadians(a);
}


// Set color index. This is interpreted by the output generator.
// It may index a color map or define a grayscale value.
void Turtle::SetColor(int c)
{
  m_color = Color(c);
}

void Turtle::SetColor(int c1, int c2)
{
  m_color     = Color(c1);
  m_colorBack = Color(c2);
}


// Set color index. This is interpreted by the output generator.
void Turtle::SetTexture(int t)
{
  m_texture = t;
}


// Set RGB color
void Turtle::SetColor(const Vector& c)
{
  m_color = Color(c);
}


// Increment the current color index
void Turtle::IncrementColor()
{
  if (m_color.type == COLOR_INDEX)
    m_color.m_color.index++;
  else
    std::cerr << "Turtle::increment_color(): current color is RGB, not index!" << std::endl;
}


// Turn left or right (rotate around the up vector)
void Turtle::Turn(direction d)
{
  if (d == positive)
    m_frame.rotate(Matrix::z, m_defaultTurn);
  else
    m_frame.rotate(Matrix::z, -m_defaultTurn);
}


void Turtle::Turn(float alpha)
{
  m_frame.rotate(Matrix::z, alpha);
}


// Pitch up or down (rotate around the left vector)
void Turtle::Pitch(direction d)
{
  if (d == positive)
    m_frame.rotate(Matrix::y, m_defaultTurn);
  else
    m_frame.rotate(Matrix::y, -m_defaultTurn);
}


void Turtle::Pitch(float alpha)
{
  m_frame.rotate(Matrix::y, alpha);
}


// Roll left or right (rotate around the heading vector)
void Turtle::Roll(direction d)
{
  if (d == positive)
    m_frame.rotate(Matrix::x, m_defaultTurn);
  else
    m_frame.rotate(Matrix::x, -m_defaultTurn);
}


void Turtle::Roll(float alpha)
{
  m_frame.rotate(Matrix::x, alpha);
}


// Spin around 180 degrees
void Turtle::Reverse()
{
  m_frame.reverse();
}


// Roll the turtle so the left vector is perpendicular to the antigravity
// vector (see pg. 57).
void Turtle::RollHorizontal()
{
  static constexpr auto tolerance = 1e-4F;

  const auto heading = CurrentHeading();
  auto left          = m_gravity ^ heading;

  // Don't do anything if heading is too close to the antigravity vector
  auto magnitude = left.magnitude();
  if (magnitude < tolerance)
  {
    return;
  }

  // new CurrentLeft vector is normalized v ^ CurrentHeading
  magnitude = 1.0F / magnitude;
  left[0] *= magnitude;
  left[1] *= magnitude;
  left[2] *= magnitude;

  // New CurrentUp vector is fixed by heading and left
  const auto up = heading ^ left;

  // Reset the CurrentLeft and CurrentUp vectors
  SetLeft(left);
  SetUp(up);
}

void Turtle::Move()
{
  this->Move(m_defaultDist);
}

// Move along heading vector for distance.
// Keep track of the motion bounds and apply a tropism correction
// if that is enabled.
void Turtle::Move(const float distance)
{
  m_position += distance * this->CurrentHeading();
  m_boundingBox.Expand(m_position);

  // Apply tropism, if enabled.
  // This consists of rotating by the vector e (CurrentHeading ^ T).
  if (m_tropism.flag && (m_tropism.susceptibility != 0))
  {
    const Vector a = CurrentHeading() ^ m_tropism.tropismVector;
    // This is bogus ??????????????????????????????????????????????????????????
    // const float m= a.magnitude();
    //if (m != 0)
    m_frame.rotate(a, m_tropism.susceptibility);
  }
}

// Save state
// Handle over/underflow gracefully
void Turtle::Push()
{
  if (m_stackPtr < 0)
  {
    std::cerr << "Turtle::Push(): can't Push below bottom of stack!" << std::endl;
  }
  else if (m_stackPtr >= stackdepth - 1)
  {
    std::cerr << "Turtle::Push(): stack depth exceeded, lost new frame!" << std::endl;
  }
  else
  {
    m_defaultTurnStack[m_stackPtr] = m_defaultTurn;
    m_defaultDistPtr[m_stackPtr]   = m_defaultDist;
    m_framePtr[m_stackPtr]         = m_frame;
    m_tropismPtr[m_stackPtr]       = m_tropism;
    m_positionPtr[m_stackPtr]      = m_position;
    m_widthPtr[m_stackPtr]         = m_width;
    m_colorPtr[m_stackPtr]         = m_color;
    m_colorBackStack[m_stackPtr]   = m_colorBack;
    m_textureStack[m_stackPtr]     = m_texture;
  }
  m_stackPtr++;
}


// Restore state
// Handle over/underflow gracefully
void Turtle::Pop()
{
  if (m_stackPtr > stackdepth)
  {
    std::cerr << "Turtle::Pop(): can't restore lost frame!" << std::endl;
  }
  else if (m_stackPtr <= 0)
  {
    std::cerr << "Turtle::Pop(): cannot Pop frame below bottom of stack!" << std::endl;
  }
  else
  {
    m_frame       = m_framePtr[m_stackPtr - 1];
    m_tropism     = m_tropismPtr[m_stackPtr - 1];
    m_position    = m_positionPtr[m_stackPtr - 1];
    m_width       = m_widthPtr[m_stackPtr - 1];
    m_color       = m_colorPtr[m_stackPtr - 1];
    m_colorBack   = m_colorBackStack[m_stackPtr - 1];
    m_texture     = m_textureStack[m_stackPtr - 1];
    m_defaultDist = m_defaultDistPtr[m_stackPtr - 1];
    m_defaultTurn = m_defaultTurnStack[m_stackPtr - 1];
  }
  m_stackPtr--;
}


std::ostream& operator<<(std::ostream& o, TropismInfo& t)
{
  if (t.flag)
    o << "[enabled";
  else
    o << "[disabled";

  return o << "; vector: " << t.tropismVector << " e: " << t.susceptibility << " ]";
}


std::ostream& operator<<(std::ostream& o, Turtle& t)
{
  o << "Turtle:\n"
    << "\tpos=         " << t.Location() << '\n'
    << "\tH  =         " << t.CurrentHeading() << '\n'
    << "\tL  =         " << t.CurrentLeft() << '\n'
    << "\tU  =         " << t.CurrentUp() << '\n'
    << "\tTropism=     " << t.m_tropism << '\n'
    << "\tcolor index= " << t.CurrentColor() << '\n'
    << "\tdefaultDist= " << t.DefaultDistance() << '\n'
    << "\twidth=       " << t.CurrentWidth() << std::endl;
  return o;
}


// Interpret the color as an intensity; map RGB color to gray.
float Color::graylevel() const
{
  if (type == COLOR_INDEX)
  {
    return static_cast<float>(m_color.index) / 100.0F;
  }

  const auto* v = reinterpret_cast<const Vector*>(&m_color.rgb);
  // G= .3R + .6G + .1B
  return (0.3F * (*v)(0)) + (0.6F * (*v)(1)) + (0.1F * (*v)(2));
}


// Interpret the color as RGB; map color index into gray scale
Vector Color::rgbcolor() const
{
  if (type == COLOR_INDEX)
  {
    const float g = graylevel();
    return Vector(g, g, g);
  }

  return *reinterpret_cast<const Vector*>(&m_color.rgb);
}


bool Color::operator==(const Color& b) const
{
  if (type != b.type)
  {
    return 0;
  }
  if (type == COLOR_INDEX)
  {
    return m_color.index == b.m_color.index;
  }
  return rgbcolor() == b.rgbcolor();
}


std::ostream& operator<<(std::ostream& o, const Color& c)
{
  if (c.type == COLOR_INDEX)
    o << "index = " << c.m_color.index;
  else
    o << "RGB = " << c.rgbcolor();
  return o;
}


}; // namespace LSys
