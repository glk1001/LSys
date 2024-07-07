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

module;

#include <string>

export module LSys.Generator;

import LSys.Consts;
import LSys.Module;
import LSys.Polygon;
import LSys.Turtle;
import LSys.Vector;

export namespace LSYS
{

// A class to generate a database; uses virtual methods and should be
// subclassed for a specific type of database, e.g. PostScript or
// PPHIGS databases.

class IGenerator
{
public:
  explicit IGenerator(const std::string& name);
  IGenerator()                  = default;
  IGenerator(const IGenerator&) = default;
  IGenerator(IGenerator&&)      = default;
  virtual ~IGenerator()         = default;

  auto operator=(const IGenerator&) -> IGenerator& = default;
  auto operator=(IGenerator&&) -> IGenerator&      = default;

  auto SetTurtle(const Turtle& turtle) -> void;

  auto SetName(const std::string& name) -> void;
  [[nodiscard]] auto GetHeader() const -> std::string;
  virtual auto SetHeader(const std::string& header) -> void;

  // Functions to provide bracketing information
  virtual auto Prelude() -> void;
  virtual auto Postscript() -> void = 0;

  // Functions to start/end a stream of graphics
  virtual auto StartGraphics() -> void = 0;
  virtual auto FlushGraphics() -> void = 0;

  // Functions to draw objects in graphics mode
  virtual auto MoveTo() -> void;
  virtual auto LineTo() -> void;
  virtual auto DrawObject(const Module& mod, int numArgs, const ArgsArray& args) -> void = 0;
  virtual auto Polygon(const LSYS::Polygon& polygon) -> void                             = 0;

  // Functions to change rendering parameters
  virtual auto SetColor() -> void     = 0;
  virtual auto SetBackColor() -> void = 0;
  virtual auto SetTexture() -> void   = 0;
  virtual auto SetWidth() -> void     = 0;

protected:
  [[nodiscard]] auto GetTurtle() const -> const Turtle&;
  [[noreturn]] virtual auto OutputFailed() -> void;

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
  const Turtle* m_turtle = nullptr;
  Vector m_lastPosition;
  float m_lastWidth        = 0.0F;
  bool m_lastMove          = true; // Was last move/draw a move?
  std::string m_objectName = "null_object"; // Name of generated object
  std::string m_objectHeader;
};

} // namespace LSYS

namespace LSYS
{

inline IGenerator::IGenerator(const std::string& name) : m_objectName{name}
{
}

inline auto IGenerator::GetTurtle() const -> const Turtle&
{
  return *m_turtle;
}

inline auto IGenerator::SetTurtle(const Turtle& turtle) -> void
{
  m_turtle = &turtle;
}

inline auto IGenerator::SetName(const std::string& name) -> void
{
  m_objectName = name;
}

inline auto IGenerator::GetHeader() const -> std::string
{
  return m_objectHeader;
}

inline auto IGenerator::SetHeader(const std::string& header) -> void
{
  m_objectHeader = header;
}

} // namespace LSYS
