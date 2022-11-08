/* Generator.c - methods for abstract database generator (do nothing)
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
 * $Log:	Generator.c,v $
 * Revision 1.3  91/03/20  10:37:32  leech
 * Add SetName() method.
 *
 * Revision 1.2  90/10/12  18:47:21  leech
 * First public release.
 *
 */
//static char RCSid[]= "$Id: Generator.c,v 1.3 91/03/20 10:37:32 leech Exp Locker: leech $";

#include "Generator.h"

#include "Turtle.h"
#include "Vector.h"

#include <cstdlib>
#include <fstream>

using std::cerr;
using std::endl;


namespace LSys
{


void Generator::OutputFailed()
{
  cerr << "Fatal error in output generator, aborting\n";
  std::exit(1);
}


void Generator::SetName(const std::string& name)
{
  objectName = name;
}


void Generator::Prelude(const Turtle& t)
{
  SetColor(t);
  SetWidth(t);
  if (out->bad())
    OutputFailed();
}


void Generator::Postscript(const Turtle& t)
{
  (void)t;
  if (out->bad())
    OutputFailed();
  out->close();
}


void Generator::MoveTo(const Turtle& t)
{
  lastPosition = t.Location();
  lastWidth    = t.CurrentWidth();
  lastMove     = true;
}


void Generator::LineTo(const Turtle& t)
{
  lastPosition = t.Location();
  lastWidth    = t.CurrentWidth();
  lastMove     = false;
}


}; // namespace LSys
