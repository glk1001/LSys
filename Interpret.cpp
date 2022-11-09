/* interpret.c - interprets an Left-system using a Turtle and taking
 *  actions to render the resulting database.
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
 * $Log:	interpret.c,v $
 * Revision 1.3  91/03/20  10:36:40  leech
 * Support for G++.
 *
 * Revision 1.2  90/10/12  18:48:19  leech
 * First public release.
 *
 */
//static char RCSid[] = "$Id: interpret.c,v 1.3 91/03/20 10:36:40 leech Exp $";

#include "Interpret.h"

#include "Actions.h"
#include "Consts.h"
#include "Expression.h"
#include "Generator.h"
#include "Module.h"
#include "SymbolTable.h"
#include "Turtle.h"
#include "Value.h"
#include "debug.h"

using std::cerr;
using std::endl;


namespace LSys
{

namespace
{

// Set up the default actions for interpretation
void SetupActions(SymbolTable<Anyptr>& st)
{
  st.enter(DRAW_OBJECT_START, Anyptr(DrawObject));
  st.enter("f", Anyptr(Move));
  st.enter("z", Anyptr(MoveHalf));
  st.enter("F", Anyptr(Draw));
  st.enter("Fl", Anyptr(Draw));
  st.enter("Fr", Anyptr(Draw));
  st.enter("Z", Anyptr(DrawHalf));
  st.enter("+", Anyptr(TurnLeft));
  st.enter("-", Anyptr(TurnRight));
  st.enter("&", Anyptr(PitchDown));
  st.enter("^", Anyptr(PitchUp));
  st.enter("\\", Anyptr(RollLeft));
  st.enter("/", Anyptr(RollRight));
  st.enter("|", Anyptr(Reverse));
  st.enter("$", Anyptr(RollHorizontal));
  st.enter("[", Anyptr(Push));
  st.enter("]", Anyptr(Pop));
  st.enter("%", Anyptr(CutBranch));
  st.enter("@md", Anyptr(MultiplyDefaultDistance));
  st.enter("@ma", Anyptr(MultiplyDefaultTurnAngle));
  st.enter("@mw", Anyptr(MultiplyWidth));
  st.enter("!", Anyptr(ChangeWidth));
  st.enter("'", Anyptr(ChangeColor));
  st.enter("@Tx", Anyptr(ChangeTexture));
  st.enter("{", Anyptr(StartPolygon));
  st.enter(".", Anyptr(PolygonVertex));
  st.enter("G", Anyptr(PolygonMove));
  st.enter("}", Anyptr(EndPolygon));
  st.enter("t", Anyptr(Tropism));
  st.enter("@Gs", Anyptr(GeneralisedCylinderStart));
  st.enter("@Gc", Anyptr(GeneralisedCylinderControlPoint));
  st.enter("@Ge", Anyptr(GeneralisedCylinderEnd));
  st.enter("@Gr", Anyptr(GeneralisedCylinderTangents));
  st.enter("@Gt", Anyptr(GeneralisedCylinderTangentLengths));
}

} // namespace


// Interpret a bound Left-system, producing output on the specified stream.
// Default values for line width, movement, and turn angles are specified.
void Interpret(const List<Module>& ml, Generator& g, float turn, float width, float distance)
{
  SymbolTable<Anyptr> action;
  SetupActions(action);

  Turtle t(turn, width);
  t.SetHeading(Vector(0, 1, 0)); // H = +Y
  t.SetLeft(Vector(-1, 0, 0)); // Left = -X
  t.SetUp(Vector(0, 0, 1)); // Up = +Z
  t.SetGravity(Vector(0, 1, 0));
  t.SetWidth(width); // ???????????????????????????????????????????????????????
  t.SetDefaultDistance(distance);

  g.Prelude(t);

  ConstListIterator<Module> mi(ml);
  for (const Module* m = mi.first(); m; m = mi.next())
  {
    std::string mname(m->name());
    if (mname[0] == DRAW_OBJECT_START_CHAR)
      mname = DRAW_OBJECT_START;
    Anyptr p;
    if (!action.lookup(mname.c_str(), p))
    {
      PDebug(PD_INTERPRET, cerr << "Unknown action for " << *m << endl);
    }
    else
    {
      // Fetch defined parameters
      int numArgs = 0;
      ArgsArray args{};
      for (auto i = 0; i < maxargs; ++i)
      {
        if (not m->getfloat(args[i], i))
        {
          break;
        }
        ++numArgs;
      }

      Actionfunc f = reinterpret_cast<Actionfunc>(p);
      (*f)(mi, t, g, numArgs, args);
    }
    PDebug(PD_INTERPRET, cerr << t);
  }

  g.Postscript(t);
}


}; // namespace LSys
