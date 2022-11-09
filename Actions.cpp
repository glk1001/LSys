/* actions.c - actions taken in interpretation of an CurrentLeft-system to
 *  produce PostScript.
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
 * $Log: actions.c,v $
 * Revision 1.3  91/03/20  10:36:16  leech
 * Better color support.
 *
 * Revision 1.2  90/10/12  18:48:16  leech
 * First public release.
 *
 */
//static char RCSid[]= "$Id: actions.c,v 1.3 91/03/20 10:36:16 leech Exp Locker: leech $";

#include "Actions.h"

#include "Consts.h"
#include "Expression.h"
#include "Generator.h"
#include "Module.h"
#include "Polygon.h"
#include "Turtle.h"
#include "Value.h"
#include "debug.h"

#include <cassert>
#include <cstring>

using std::cerr;
using std::endl;


namespace LSys
{

namespace
{

// The stack is used by { } commands to define polygons.
// The stack can get quite deep in recursive CurrentLeft-system
// productions, thus we use a depth of 100 (probably should
// use a dynamically allocated list).
int polyPtr                 = -1;
constexpr auto MAX_POLYGONS = 100;
std::array<LSys::Polygon*, MAX_POLYGONS> polygonStack{};

enum class State
{
  START,
  DRAWING,
  POLYGON
};
auto state = State::START;

// Common Move/draw routine
auto MoveTurtle(Turtle& turtle, int numArgs, const ArgsArray& args) noexcept -> void;

// Set line width only if changed too much
auto SetLineWidth(const Turtle& turtle, Generator& generator) noexcept -> void;

// Set color only if changed
auto SetColor(const Turtle& turtle, Generator& generator) noexcept -> void;

// Set texture only if changed
auto SetTexture(const Turtle& turtle, Generator& generator) noexcept -> void;

// Add an edge to the current polygon while moving
auto AddPolygonEdge(Turtle& turtle, int numArgs, const ArgsArray& args) noexcept -> void;

} // namespace

// f(l) Move without drawing
auto Move([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
          Turtle& turtle,
          Generator& generator,
          const int numArgs,
          const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, cerr << "Move          " << endl);

  if ((state == State::DRAWING) or (state == State::START))
  {
    MoveTurtle(turtle, numArgs, args);
    generator.MoveTo(turtle);
  }
  else
  {
    assert(state == State::POLYGON);
    AddPolygonEdge(turtle, numArgs, args);
  }
}

// z Move half standard distance without drawing
auto MoveHalf(const ConstListIterator<Module>& moduleIter,
              Turtle& turtle,
              Generator& generator,
              [[maybe_unused]] const int numArgs,
              [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, cerr << "MoveHalf      " << endl);

  const ArgsArray oneArg = {0.5F * turtle.DefaultDistance()};
  Move(moduleIter, turtle, generator, 1, oneArg);
}

// F(l) Move while drawing
// Fr(l), Fl(l) - Right and CurrentLeft edges respectively
auto Draw([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
          Turtle& turtle,
          Generator& generator,
          const int numArgs,
          const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, cerr << "Draw          " << endl);

  if (state == State::START)
  {
    generator.StartGraphics(turtle);
    state = State::DRAWING;
  }

  if (state == State::DRAWING)
  {
    MoveTurtle(turtle, numArgs, args);
    generator.LineTo(turtle);
  }
  else
  {
    assert(state == State::POLYGON);
    AddPolygonEdge(turtle, numArgs, args);
  }
}

// Z Draw half standard distance while drawing
auto DrawHalf(ConstListIterator<Module>& moduleIter,
              Turtle& turtle,
              Generator& generator,
              [[maybe_unused]] const int numArgs,
              [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, cerr << "DrawHalf      " << endl);

  const auto oneArg = ArgsArray{0.5F * turtle.DefaultDistance()};
  Draw(moduleIter, turtle, generator, 1, oneArg);
}

// -(t) Turn right: negative rotation about Z
auto TurnRight([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
               Turtle& turtle,
               [[maybe_unused]] const Generator& generator,
               const int numArgs,
               const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, cerr << "TurnRight     " << endl);

  if (0 == numArgs)
  {
    turtle.Turn(Turtle::negative);
  }
  else
  {
    turtle.Turn(-Maths::ToRadians(args[0]));
  }
}

// +(t) Turn left; positive rotation about Z
auto TurnLeft([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
              Turtle& turtle,
              [[maybe_unused]] const Generator& generator,
              const int numArgs,
              const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, cerr << "TurnLeft      " << endl);

  if (0 == numArgs)
  {
    turtle.Turn(Turtle::positive);
  }
  else
  {
    turtle.Turn(Maths::ToRadians(args[0]));
  }
}

// ^(t) Pitch up; negative rotation about Y
auto PitchUp([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
             Turtle& turtle,
             [[maybe_unused]] const Generator& generator,
             const int numArgs,
             const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, cerr << "PitchUp       " << endl);

  if (0 == numArgs)
  {
    turtle.Pitch(Turtle::negative);
  }
  else
  {
    turtle.Pitch(-Maths::ToRadians(args[0]));
  }
}

// &(t) Pitch down; positive rotation about Y
auto PitchDown([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
               Turtle& turtle,
               [[maybe_unused]] const Generator& generator,
               const int numArgs,
               const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, cerr << "PitchDown     " << endl);

  if (0 == numArgs)
  {
    turtle.Pitch(Turtle::positive);
  }
  else
  {
    turtle.Pitch(Maths::ToRadians(args[0]));
  }
}

// /(t) Roll right; positive rotation about X
auto RollRight([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
               Turtle& turtle,
               [[maybe_unused]] const Generator& generator,
               const int numArgs,
               const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, cerr << "RollRight     " << endl);

  if (0 == numArgs)
  {
    turtle.Roll(Turtle::positive);
  }
  else
  {
    turtle.Roll(Maths::ToRadians(args[0]));
  }
}

// \(t) Roll left; negative rotation about X
auto RollLeft([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
              Turtle& turtle,
              [[maybe_unused]] const Generator& generator,
              const int numArgs,
              const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, cerr << "RollLeft      " << endl);

  if (0 == numArgs)
  {
    turtle.Roll(Turtle::negative);
  }
  else
  {
    turtle.Roll(-Maths::ToRadians(args[0]));
  }
}

// |  Turn around
auto Reverse([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
             Turtle& turtle,
             [[maybe_unused]] const Generator& generator,
             [[maybe_unused]] const int numArgs,
             [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, cerr << "Reverse       " << endl);

  turtle.Reverse();
}

// [  Push turtle state
auto Push([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
          Turtle& turtle,
          [[maybe_unused]] const Generator& generator,
          [[maybe_unused]] const int numArgs,
          [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, cerr << "Push          " << endl);

  turtle.Push();
}

// ]	Pop turtle state
auto Pop(ConstListIterator<Module>& moduleIter,
         Turtle& turtle,
         Generator& generator,
         [[maybe_unused]] const int numArgs,
         [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, cerr << "Pop           " << endl);

  turtle.Pop();

  // Look ahead; if the next module is also a Pop, don't do anything.
  // Otherwise, reset the line width, color, and position.
  // This is an optimization to handle deep nesting ([[...][...[...]]]),
  // which happens a lot with trees.

  const Module* const obj = moduleIter.next();
  if (obj != nullptr)
  {
    if (obj->name() != Name{"]"})
    {
      SetLineWidth(turtle, generator);
      SetColor(turtle, generator);
      generator.MoveTo(turtle);
    }
    // Back off one step so the next module is interpreted properly
    moduleIter.previous();
  }
}

// $	Roll to horizontal plane (pg. 57)
auto RollHorizontal([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                    Turtle& turtle,
                    [[maybe_unused]] const Generator& generator,
                    [[maybe_unused]] const int numArgs,
                    [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, cerr << "RollHorizontal" << endl);

  turtle.RollHorizontal();
}

// {	Start a new polygon
auto StartPolygon([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                  const Turtle& turtle,
                  Generator& generator,
                  [[maybe_unused]] const int numArgs,
                  [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, cerr << "StartPolygon  " << endl);

  if (state == State::DRAWING)
  {
    generator.FlushGraphics(turtle);
  }

  state = State::POLYGON;
  ++polyPtr;
  if (polyPtr >= MAX_POLYGONS)
  {
    cerr << "StartPolygon: polygon stack filled.\n";
    return;
  }

  polygonStack.at(polyPtr) = new LSys::Polygon;
}

// .	Add a vertex to the current polygon
auto PolygonVertex([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                   const Turtle& turtle,
                   [[maybe_unused]] const Generator& generator,
                   [[maybe_unused]] const int numArgs,
                   [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, cerr << "PolygonVertex " << endl);

  if (state != State::POLYGON)
  {
    cerr << "Illegal: Polygon vertex while not in polygon mode\n";
    return;
  }

  if (polyPtr < 0)
  {
    cerr << "PolygonVertex: no polygon being defined\n";
    return;
  }
  if (polyPtr >= MAX_POLYGONS)
  {
    return; // Already got an error from StartPolygon
  }

  auto* const vec = new Vector(turtle.Location());
  polygonStack.at(polyPtr)->append(vec);
}

// G	Move without creating a polygon edge
// This seems like it should be illegal outside a polygon, but
// the rose leaf example in the text uses G in this context.
// Until the behavior is specified, just Move the turtle without
// other effects.
auto PolygonMove([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                 Turtle& turtle,
                 [[maybe_unused]] const Generator& generator,
                 const int numArgs,
                 const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, cerr << "PolygonMove   " << endl);

  //if (State != POLYGON) {
  //	  cerr << "Illegal: Polygon Move-without-draw while not in polygon mode\n";
  //	  return;
  //}

  MoveTurtle(turtle, numArgs, args);
}

// }	Close the current polygon
auto EndPolygon([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                const Turtle& turtle,
                Generator& generator,
                [[maybe_unused]] const int numArgs,
                [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, cerr << "EndPolygon    \n");

  if ((state != State::POLYGON) or (polyPtr < 0))
  {
    cerr << "EndPolygon: no polygon being defined\n";
    return;
  }

  if (polyPtr >= MAX_POLYGONS)
  {
    cerr << "EndPolygon: polygon stack too deep, polygon lost\n";
    --polyPtr;
  }
  else
  {
    generator.Polygon(turtle, *polygonStack.at(polyPtr));
    delete polygonStack.at(polyPtr);
    --polyPtr;
    // Return to start state if no more polys on stack
    if (polyPtr < 0)
    {
      state = State::START;
    }
  }
}

// @md(f) Multiply DefaultDistance by f
auto MultiplyDefaultDistance([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                             Turtle& turtle,
                             [[maybe_unused]] const Generator& generator,
                             const int numArgs,
                             const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, cerr << "MultiplyDefaultDist  " << endl);

  if (0 == numArgs)
  {
    turtle.SetDefaultDistance(1.1F * turtle.DefaultDistance());
  }
  else
  {
    turtle.SetDefaultDistance(args.at(0) * turtle.DefaultDistance());
  }
}

// @ma(f) Multiply DefaultTurnAngle by f
auto MultiplyDefaultTurnAngle([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                              Turtle& turtle,
                              [[maybe_unused]] const Generator& generator,
                              const int numArgs,
                              const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, cerr << "MultiplyDefaultTurnAngle  " << endl);

  if (0 == numArgs)
  {
    turtle.SetDefaultTurnAngle(1.1F * turtle.DefaultTurnAngle());
  }
  else
  {
    turtle.SetDefaultTurnAngle(args[0] * turtle.DefaultTurnAngle());
  }
}

// @mw(f) Multiply width by f
auto MultiplyWidth([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                   Turtle& turtle,
                   Generator& generator,
                   const int numArgs,
                   const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, cerr << "MultiplyWidth  \n");

  if (0 == numArgs)
  {
    turtle.SetWidth(1.4F * turtle.CurrentWidth());
  }
  else
  {
    turtle.SetWidth(args[0] * turtle.CurrentWidth());
  }

  SetLineWidth(turtle, generator);
}

// !(d) Set line width
auto ChangeWidth([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                 Turtle& turtle,
                 Generator& generator,
                 const int numArgs,
                 const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, cerr << "ChangeWidth     " << endl);

  if (0 == numArgs)
  {
    turtle.SetWidth();
  }
  else
  {
    turtle.SetWidth(args[0]);
  }

  SetLineWidth(turtle, generator);
}

// '	Increment color index
// '(n) Set color index
// '(r,g,b) Set RGB color
auto ChangeColor([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                 Turtle& turtle,
                 Generator& generator,
                 const int numArgs,
                 const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, cerr << "ChangeColor   " << endl);

  if (numArgs >= 3)
  {
    turtle.SetColor(Vector(args.at(0), args.at(1), args.at(2)));
  }
  if (numArgs >= 2)
  {
    turtle.SetColor(static_cast<int>(args.at(0)), static_cast<int>(args.at(1)));
  }
  else if (numArgs > 0)
  {
    turtle.SetColor(static_cast<int>(args.at(0)));
  }
  else
  {
    turtle.IncrementColor();
  }

  SetColor(turtle, generator);
}

// @Tx(n)	Change texture index
auto ChangeTexture([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                   Turtle& turtle,
                   Generator& generator,
                   [[maybe_unused]] const int numArgs,
                   const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, cerr << "ChangeTexture   " << endl);

  turtle.SetTexture(static_cast<int>(args[0]));

  SetTexture(turtle, generator);
}

// ~	Draw the following object at the turtle's position and frame
// Needs a standardized object file format for this.
// Should leave graphics mode before drawing object.
auto DrawObject(const ConstListIterator<Module>& moduleIter,
                const Turtle& turtle,
                Generator& generator,
                const int numArgs,
                const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, cerr << "DrawObject    " << endl);

  //  Module* obj= mi.next();
  const Module* const obj = moduleIter.current();
  if (obj != nullptr)
  {
    generator.DrawObject(turtle, *obj, numArgs, args);
  }
}

auto GeneralisedCylinderStart([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                              [[maybe_unused]] const Turtle& turtle,
                              [[maybe_unused]] const Generator& generator,
                              [[maybe_unused]] const int numArgs,
                              [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  // Not implemented
}

auto GeneralisedCylinderControlPoint([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                                     [[maybe_unused]] const Turtle& turtle,
                                     [[maybe_unused]] const Generator& generator,
                                     [[maybe_unused]] const int numArgs,
                                     [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  // Not implemented
}

auto GeneralisedCylinderEnd([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                            [[maybe_unused]] const Turtle& turtle,
                            [[maybe_unused]] const Generator& generator,
                            [[maybe_unused]] const int numArgs,
                            [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  // Not implemented
}

auto GeneralisedCylinderTangents([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                                 [[maybe_unused]] const Turtle& turtle,
                                 [[maybe_unused]] const Generator& generator,
                                 [[maybe_unused]] const int numArgs,
                                 [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  // Not implemented
}

auto GeneralisedCylinderTangentLengths([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                                       [[maybe_unused]] const Turtle& turtle,
                                       [[maybe_unused]] const Generator& generator,
                                       [[maybe_unused]] const int numArgs,
                                       [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  // Not implemented
}

// %	Truncate a branch
// E.g., ignore all modules up to the next ]
// Note that this code is cloned from right-context
// matching in Production::matches()
auto CutBranch(ConstListIterator<Module>& moduleIter,
               [[maybe_unused]] const Turtle& turtle,
               [[maybe_unused]] const Generator& generator,
               [[maybe_unused]] const int numArgs,
               [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, cerr << "CutBranch     " << endl);

  // Must find a matching ]; skip anything else including
  //	bracketed substrings.
  const Module* obj{};
  int brackets;
  for (brackets = 0, obj = moduleIter.next(); obj; obj = moduleIter.next())
  {
    if (obj->name() == RBRACKET)
    {
      --brackets;
      if (0 == brackets)
      {
        break;
      }
    }
    else if (obj->name() == LBRACKET)
    {
      ++brackets;
    }
  }

  // Back off one step so the pop itself is handled by interpret()
  if (obj != nullptr)
  {
    moduleIter.previous();
  }
}

// t	Enable/disable tropism corrections after each Move
// t(x,y,z,e)	- enable tropism; tropism vector is T= (x,y,z),
// e is `suspectibility parameter', preferably between 0 and 1.
// t(0)		- disable tropism
// t(1)		- reenable tropism with last (T,e) parameters

static auto TropismError() noexcept -> void
{
  cerr << "Tropism: expect arguments (x,y,z,e) or (e).\n";
}


auto Tropism([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
             Turtle& turtle,
             [[maybe_unused]] const Generator& generator,
             const int numArgs,
             const ArgsArray& args) noexcept -> void
{
  if (0 == numArgs)
  {
    TropismError();
    return;
  }

  if (1 == numArgs)
  {
    // Only one parameter; disable tropism if it equals 0,
    // otherwise enable tropism with the specified parameter.
    if (const auto susceptibility = args.at(0); std::fabs(susceptibility) < Maths::SMALL_FLOAT)
    {
      turtle.DisableTropism();
    }
    // TODO(glk) - Is this check for 1 OK?
    else if (std::fabs(susceptibility - 1.0F) < Maths::SMALL_FLOAT)
    {
      turtle.SetTropismVector(susceptibility);
      turtle.EnableTropism();
    }
    return;
  }

  if (numArgs < 4)
  {
    TropismError();
    return;
  }

  // Construct the tropism vector
  turtle.SetTropismVector(Vector(args.at(0), args.at(1), args.at(2)));
  turtle.SetTropismVector(args.at(3));
  turtle.EnableTropism();
}

namespace
{

auto MoveTurtle(Turtle& turtle, const int numArgs, const ArgsArray& args) noexcept -> void
{
  if (0 == numArgs)
  {
    turtle.Move();
  }
  else
  {
    turtle.Move(args[0]);
  }
}

auto AddPolygonEdge(Turtle& turtle, const int numArgs, const ArgsArray& args) noexcept -> void
{
  // Add an edge to the current polygon
  ConstPolygonIterator polygonIter(*polygonStack.at(polyPtr));
  const Vector* const vec = polygonIter.last();

  // See if the starting point needs to be added (only if
  // it's different from the last point defined in
  // the polygon, or there are no points defined in the polygon
  // yet).
  if (auto* const point = new Vector(turtle.Location()); (nullptr == vec) or (*vec != *point))
  {
    PDebug(PD_INTERPRET, cerr << "AddPolygonEdge: adding first vertex " << *point << endl);
    polygonStack.at(polyPtr)->append(point);
  }

  // Move and add the ending point to the polygon.
  MoveTurtle(turtle, numArgs, args);
  PDebug(PD_INTERPRET, cerr << "AddPolygonEdge: adding last vertex  " << turtle.Location() << endl);
  polygonStack.at(polyPtr)->append(new Vector(turtle.Location()));
}

auto SetLineWidth(const Turtle& turtle, Generator& generator) noexcept -> void
{
  static constexpr auto epsilon = 1e-6F;
  static float lastLineWidth    = -1.0F;

  // Don't bother changing line width if 'small enough'.
  // This is an optimization to handle e.g. !(w)[!(w/2)F][!(w/2)F]
  //sort of cases, which happen a lot with trees.
  if (std::fabs(turtle.CurrentWidth() - lastLineWidth) < epsilon)
  {
    return;
  }

  if (state == State::DRAWING)
  {
    generator.FlushGraphics(turtle);
    state = State::START;
  }

  generator.SetWidth(turtle);
  lastLineWidth = turtle.CurrentWidth();
}

auto SetColor(const Turtle& turtle, Generator& generator) noexcept -> void
{
  static Color lastcolor(-1);

  // Don't change color if not needed, again an optimization
  if (turtle.CurrentColor() == lastcolor)
  {
    return;
  }

  if (state == State::DRAWING)
  {
    generator.FlushGraphics(turtle);
    state = State::START;
  }

  generator.SetColor(turtle);
  lastcolor = turtle.CurrentColor();
}

auto SetTexture(const Turtle& turtle, Generator& generator) noexcept -> void
{
  static int lastTexture(-1);

  // Don't change txture if not needed, again an optimization
  if (turtle.CurrentTexture() == lastTexture)
  {
    return;
  }

  if (state == State::DRAWING)
  {
    generator.FlushGraphics(turtle);
    state = State::START;
  }

  generator.SetTexture(turtle);
  lastTexture = turtle.CurrentTexture();
}

} // namespace

} // namespace LSys
