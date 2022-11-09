/* DBGenerator.h - class definition for abstract database generators
 *
 * $Id: DBGenerator.h,v 1.3 91/03/20 10:39:07 leech Exp Locker: leech $
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
 * $Log:	DBGenerator.h,v $
 * Revision 1.3  91/03/20  10:39:07  leech
 * Added set_name() method. Support for G++.
 *
 * Revision 1.2  90/10/12  18:48:02  leech
 * First public release.
 *
 */

#pragma once

#include "Consts.h"
#include "Module.h"
#include "Polygon.h"
#include "Turtle.h"

#include <iosfwd>
#include <string>


namespace LSys
{
// A class to generate a database; uses virtual methods and should be
// subclassed for a specific type of database, e.g. PostScript or
// PPHIGS databases.

class Generator
{
public:
  Generator(std::ofstream* output, std::ofstream* boundsOutput);

  void SetName(const std::string&);
  virtual void SetHeader(const std::string&) = 0;
  virtual void OutputFailed();

  // Functions to provide bracketing information
  virtual void Prelude(const Turtle&);
  virtual void Postscript(const Turtle&);

  // Functions to start/end a stream of graphics
  virtual void StartGraphics(const Turtle&) = 0;
  virtual void FlushGraphics(const Turtle&) = 0;

  // Functions to draw objects in graphics mode
  virtual void MoveTo(const Turtle&);
  virtual void LineTo(const Turtle&);
  virtual void DrawObject(const Turtle&, const Module&, int nargs, const ArgsArray& args) = 0;
  virtual void Polygon(const Turtle&, const LSys::Polygon&)                               = 0;

  // Functions to change rendering parameters
  virtual void SetColor(const Turtle&)     = 0;
  virtual void SetBackColor(const Turtle&) = 0;
  virtual void SetWidth(const Turtle&)     = 0;
  virtual void SetTexture(const Turtle&)   = 0;

protected:
  std::ofstream* out;
  std::ofstream* boundsOutput;
  const Vector& LastPosition() const { return lastPosition; }
  float LastWidth() const { return lastWidth; }
  bool LastMove() const { return lastMove; } // Was last move/draw a move?
  const std::string& ObjectName() const { return objectName; } // Name of generated object
private:
  Vector lastPosition;
  float lastWidth;
  bool lastMove; // Was last move/draw a move?
  std::string objectName; // Name of generated object
};


inline Generator::Generator(std::ofstream* o, std::ofstream* bo)
  : out(o), boundsOutput(bo), objectName("null_object"), lastMove(true)
{
}

} // namespace LSys
