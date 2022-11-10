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

class IGenerator
{
public:
  IGenerator(std::ofstream* output, std::ofstream* boundsOutput);
  IGenerator(const IGenerator&) = default;
  IGenerator(IGenerator&&)      = default;
  virtual ~IGenerator()         = default;

  auto operator=(const IGenerator&) -> IGenerator& = default;
  auto operator=(IGenerator&&) -> IGenerator&      = default;

  auto SetName(const std::string& name) -> void;
  virtual auto SetHeader(const std::string& header) -> void = 0;
  virtual auto OutputFailed() -> void;

  // Functions to provide bracketing information
  virtual auto Prelude(const Turtle& turtle) -> void;
  virtual auto Postscript(const Turtle& turtle) -> void;

  // Functions to start/end a stream of graphics
  virtual auto StartGraphics(const Turtle& turtle) -> void = 0;
  virtual auto FlushGraphics(const Turtle& turtle) -> void = 0;

  // Functions to draw objects in graphics mode
  virtual auto MoveTo(const Turtle& turtle) -> void;
  virtual auto LineTo(const Turtle& turtle) -> void;
  virtual auto DrawObject(const Turtle& turtle,
                          const Module& mod,
                          int numArgs,
                          const ArgsArray& args) -> void                   = 0;
  virtual auto Polygon(const Turtle& turtle, const LSys::Polygon&) -> void = 0;

  // Functions to change rendering parameters
  virtual auto SetColor(const Turtle& turtle) -> void     = 0;
  virtual auto SetBackColor(const Turtle& turtle) -> void = 0;
  virtual auto SetWidth(const Turtle& turtle) -> void     = 0;
  virtual auto SetTexture(const Turtle& turtle) -> void   = 0;

protected:
  std::ofstream* m_output;
  std::ofstream* m_boundsOutput;
  [[nodiscard]] auto GetLastPosition() const -> const Vector& { return m_lastPosition; }
  [[nodiscard]] auto GetLastWidth() const -> float { return m_lastWidth; }
  [[nodiscard]] auto GetLastMove() const -> bool // Was last move/draw a move?
  {
    return m_lastMove;
  }
  [[nodiscard]] auto GetObjectName() const -> const std::string& // Name of generated object
  {
    return m_objectName;
  }

private:
  Vector m_lastPosition;
  float m_lastWidth        = 0.0F;
  bool m_lastMove          = true; // Was last move/draw a move?
  std::string m_objectName = "null_object"; // Name of generated object
};


inline IGenerator::IGenerator(std::ofstream* const output, std::ofstream* const boundsOutput)
  : m_output(output), m_boundsOutput(boundsOutput)
{
}

inline auto IGenerator::SetName(const std::string& name) -> void
{
  m_objectName = name;
}

} // namespace LSys
