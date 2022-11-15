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
 * $Log:	Generator.c,v $
 * Revision 1.3  91/03/20  10:37:32  leech
 * Add SetName() method.
 *
 * Revision 1.2  90/10/12  18:47:21  leech
 * First public release.
 *
 */

#include "IGenerator.h"

#include "Turtle.h"
#include "Vector.h"

#include <cstdlib>
#include <fstream>
#include <stdexcept>

namespace LSys
{

auto IGenerator::OutputFailed() -> void
{
  throw std::runtime_error("Fatal error in output generator, aborting");
}

auto IGenerator::Prelude(const Turtle& turtle) -> void
{
  SetColor(turtle);
  SetWidth(turtle);
}

auto IGenerator::MoveTo(const Turtle& turtle) -> void
{
  m_lastPosition = turtle.GetCurrentState().position;
  m_lastWidth    = turtle.GetCurrentState().width;
  m_lastMove     = true;
}

auto IGenerator::LineTo(const Turtle& turtle) -> void
{
  m_lastPosition = turtle.GetCurrentState().position;
  m_lastWidth    = turtle.GetCurrentState().width;
  m_lastMove     = false;
}

} // namespace LSys
