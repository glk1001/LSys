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

#include <iostream>
#include "Turtle.h"


namespace LSys {


  // Default stack depth
  const int stackdepth= 100;

  // Line width relative to unit Move
  const float relative_line_width= 1;


  Turtle::Turtle(float delta, float wscale)
  : pos(0, 0, 0),
    bbox(pos)
  {
    // Allocate and initialize turtle state stack
    stackptr= 0;
    f= new Matrix[stackdepth];
    p= new Vector[stackdepth];
    t= new TropismInfo[stackdepth];
    d= new float[stackdepth];
    defaultTurnStack= new float[stackdepth];
    w= new float[stackdepth];
    c= new Color[stackdepth];
    colorBackStack= new Color[stackdepth];
    textureStack= new int[stackdepth];

    this->SetDefaults(wscale, delta);

    // Set up initial frame and position, moving in +X with up in Z
    frame.identity();
    this->SetGravity(this->CurrentHeading());

    // Default tropism vector is towards ground, but tropism is disabled
    this->SetTropismVector(-this->CurrentHeading());
    this->SetTropismVector(0.2);
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
    delete[] f;
    delete[] p;
    delete[] t;
    delete[] defaultTurnStack;
    delete[] d;
    delete[] w;
    delete[] c;
    delete[] colorBackStack;
    delete[] textureStack;
  }


  // CurrentHeading is first column of frame
  Vector Turtle::CurrentHeading() const
  {
    return Vector(frame[0][0], frame[1][0], frame[2][0]);
  }


  // Set heading
  void Turtle::SetHeading(const Vector& CurrentHeading)
  {
    frame[0][0]= CurrentHeading(0);
    frame[1][0]= CurrentHeading(1);
    frame[2][0]= CurrentHeading(2);
  }


  // CurrentLeft is second column of frame
  Vector Turtle::CurrentLeft() const
  {
    return Vector(frame[0][1], frame[1][1], frame[2][1]);
  }


  // Set left
  void Turtle::SetLeft(const Vector& l)
  {
    frame[0][1]= l(0);
    frame[1][1]= l(1);
    frame[2][1]= l(2);
  }


  // CurrentUp is third column of frame
  Vector Turtle::CurrentUp() const
  {
    return Vector(frame[0][2], frame[1][2], frame[2][2]);
  }


  // Set up
  void Turtle::SetUp(const Vector& u)
  {
    frame[0][2]= u(0);
    frame[1][2]= u(1);
    frame[2][2]= u(2);
  }


  // Set the whole frame at once
  void Turtle::SetFrame(const Matrix& m)
  {
    frame= m;
  }


  // Set the antigravity vector
  void Turtle::SetGravity(const Vector& gvec)
  {
    gravity= gvec;
  }


  void Turtle::SetTropismVector(const Vector& t)
  {
    tropism.t= t;
  }


  void Turtle::SetTropismVector(float e)
  {
    tropism.e= e;
  }


  void Turtle::DisableTropism()
  {
    tropism.flag= false;
  }


  void Turtle::EnableTropism()
  {
    tropism.flag= true;
  }

  // Set line width
  void Turtle::SetWidth(float w)
  {
  //  width= w * widthScale * relative_line_width;
    width= w;
  }


  // Get the default drawing parameters
  void Turtle::GetDefaults(float* wscale, float* delta)
  {
    *wscale= widthScale;
    *delta= Maths::RadiansToDegrees(defaultTurn);
  }


  // Set the default drawing parameters
  void Turtle::SetDefaults(float wscale, float delta)
  {
    widthScale= wscale;
    defaultTurn= Maths::DegreesToRadians(delta);
  }


  void Turtle::SetDefaultDistance(float d)
  {
    defaultDist= d;
  }


  void Turtle::SetDefaultTurnAngle(float a)
  {
    defaultTurn= Maths::DegreesToRadians(a);
  }


  // Set color index. This is interpreted by the output generator.
  // It may index a color map or define a grayscale value.
  void Turtle::SetColor(int c)
  {
    color= Color(c);
  }

  void Turtle::SetColor(int c1, int c2)
  {
    color= Color(c1);
    colorBack= Color(c2);
  }


  // Set color index. This is interpreted by the output generator.
  void Turtle::SetTexture(int t)
  {
    texture= t;
  }


  // Set RGB color
  void Turtle::SetColor(const Vector& c)
  {
    color= Color(c);
  }


  // Increment the current color index
  void Turtle::IncrementColor()
  {
    if (color.type == COLOR_INDEX)
      color.c.index++;
    else
      std::cerr << "Turtle::increment_color(): current color is RGB, not index!" << std::endl;
  }


  // Turn left or right (rotate around the up vector)
  void Turtle::Turn(direction d)
  {
    if (d == positive)
      frame.rotate(Matrix::z, defaultTurn);
    else
      frame.rotate(Matrix::z, -defaultTurn);
  }


  void Turtle::Turn(float alpha)
  {
    frame.rotate(Matrix::z, alpha);
  }


  // Pitch up or down (rotate around the left vector)
  void Turtle::Pitch(direction d)
  {
    if (d == positive)
      frame.rotate(Matrix::y, defaultTurn);
    else
      frame.rotate(Matrix::y, -defaultTurn);
  }


  void Turtle::Pitch(float alpha)
  {
    frame.rotate(Matrix::y, alpha);
  }


  // Roll left or right (rotate around the heading vector)
  void Turtle::Roll(direction d)
  {
    if (d == positive)
      frame.rotate(Matrix::x, defaultTurn);
    else
      frame.rotate(Matrix::x, -defaultTurn);
  }


  void Turtle::Roll(float alpha)
  {
    frame.rotate(Matrix::x, alpha);
  }


  // Spin around 180 degrees
  void Turtle::Reverse()
  {
    frame.reverse();
  }


  // Roll the turtle so the left vector is perpendicular to the antigravity
  // vector (see pg. 57).
  void Turtle::RollHorizontal()
  {
    const float tolerance= 1e-4;
    Vector H= CurrentHeading();
    Vector l= gravity ^ H;
    Vector u;

    // Don't do anything if heading is too close to the antigravity vector
    float m= l.magnitude();
    if (m < tolerance)
      return;

    // new CurrentLeft vector is normalized v ^ CurrentHeading
    m= 1 / m;
    l[0] *= m;
    l[1] *= m;
    l[2] *= m;

    // new CurrentUp vector is fixed by CurrentHeading and l
    u= H ^ l;

    // reset the CurrentLeft and CurrentUp vectors
    SetLeft(l);
    SetUp(u);
  }


  void Turtle::Move()
  {
    this->Move(defaultDist);
  }


  // Move along heading vector for distance d.
  // Keep track of the motion bounds and apply a tropism correction
  // if that is enabled.
  void Turtle::Move(float d)
  {
    pos += d*this->CurrentHeading();
    bbox.Expand(pos);

    // Apply tropism, if enabled.
    // This consists of rotating by the vector e (CurrentHeading ^ T).
    if (tropism.flag && (tropism.e != 0)) {
      const Vector a= CurrentHeading() ^ tropism.t;
      // This is bogus ??????????????????????????????????????????????????????????
      // const float m= a.magnitude();
      //if (m != 0)
      frame.rotate(a, tropism.e);
    }
  }


  // Save state
  // Handle over/underflow gracefully
  void Turtle::Push()
  {
    if (stackptr < 0) {
      std::cerr << "Turtle::Push(): can't Push below bottom of stack!" << std::endl;
    } else if (stackptr >= stackdepth - 1) {
      std::cerr << "Turtle::Push(): stack depth exceeded, lost new frame!" << std::endl;
    } else {
      defaultTurnStack[stackptr]= defaultTurn;
      d[stackptr]= defaultDist;
      f[stackptr]= frame;
      t[stackptr]= tropism;
      p[stackptr]= pos;
      w[stackptr]= width;
      c[stackptr]= color;
      colorBackStack[stackptr]= colorBack;
      textureStack[stackptr]= texture;
    }
    stackptr++;
  }


  // Restore state
  // Handle over/underflow gracefully
  void Turtle::Pop()
  {
    if (stackptr > stackdepth) {
      std::cerr << "Turtle::Pop(): can't restore lost frame!" << std::endl;
    } else if (stackptr <= 0) {
      std::cerr << "Turtle::Pop(): cannot Pop frame below bottom of stack!" << std::endl;
    } else {
      frame= f[stackptr-1];
      tropism= t[stackptr-1];
      pos= p[stackptr-1];
      width= w[stackptr-1];
      color= c[stackptr-1];
      colorBack= colorBackStack[stackptr-1];
      texture= textureStack[stackptr-1];
      defaultDist= d[stackptr-1];
      defaultTurn= defaultTurnStack[stackptr-1];
    }
    stackptr--;
  }


  std::ostream& operator<<(std::ostream& o, TropismInfo& t)
  {
    if (t.flag)
      o << "[enabled";
    else
      o << "[disabled";

    return o << "; vector: " << t.t << " e: " << t.e << " ]";
  }


  std::ostream& operator<<(std::ostream& o, Turtle& t)
  {
    o << "Turtle:\n"
      << "\tpos=         " << t.Location()  << '\n'
      << "\tH  =         " << t.CurrentHeading()	 << '\n'
      << "\tL  =         " << t.CurrentLeft()	 << '\n'
      << "\tU  =         " << t.CurrentUp()	 << '\n'
      << "\tTropism=     " << t.tropism	 << '\n'
      << "\tcolor index= " << t.CurrentColor() << '\n'
      << "\tdefaultDist= " << t.DefaultDistance() << '\n'
      << "\twidth=       " << t.CurrentWidth() << std::endl;
    return o;
  }


  // Interpret the color as an intensity; map RGB color to gray.
  float Color::graylevel() const
  {
    if (type == COLOR_INDEX)
      return c.index / 100.0;
    else {
      Vector *v= (Vector *)&c.rgb;
      // G= .3R + .6G + .1B
      return .3 * (*v)(0) + .6 * (*v)(1) + .1 * (*v)(2);
    }
  }


  // Interpret the color as RGB; map color index into gray scale
  Vector Color::rgbcolor() const
  {
    if (type == COLOR_INDEX) {
      const float g= graylevel();
      return Vector(g, g, g);
    } else
      return *(const Vector*)&c.rgb;
  }


  int Color::operator==(const Color& b) const
  {
    if (type != b.type)
      return 0;
    if (type == COLOR_INDEX)
      return c.index == b.c.index;
    else
      return rgbcolor() == b.rgbcolor();
  }


  std::ostream& operator<<(std::ostream& o, const Color& c)
  {
    if (c.type == COLOR_INDEX)
      o << "index = " << c.c.index;
    else
      o << "RGB = " << c.rgbcolor();
    return o;
  }


};  
