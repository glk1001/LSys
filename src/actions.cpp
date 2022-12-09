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
 * $Log: actions.c,v $
 * Revision 1.3  91/03/20  10:36:16  leech
 * Better color support.
 *
 * Revision 1.2  90/10/12  18:48:16  leech
 * First public release.
 *
 */

#include "actions.h"

#include "consts.h"
#include "debug.h"
#include "expression.h"
#include "generator.h"
#include "module.h"
#include "polygon.h"
#include "turtle.h"
#include "value.h"

#include <cassert>
#include <stack>
#include <stdexcept>

namespace LSYS
{

namespace
{

// The stack is used by the '{ }' commands to define polygons. It can
// get quite deep in recursive L-system productions, thus we use a depth
// of 100 (probably should use a dynamically allocated list).
constexpr auto MAX_POLYGONS = 100;
std::stack<LSYS::Polygon> polygonStack{};

enum class State
{
  START,
  DRAWING,
  POLYGON
};
auto state = State::START;

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

// Add an edge to the current polygon while moving
auto AddPolygonEdge(Turtle& turtle, const int numArgs, const ArgsArray& args) noexcept -> void
{
  // Add an edge to the current polygon
  const auto& lastPolygon = polygonStack.top();

  // See if the starting point needs to be added (only if
  // it's different from the last point defined in
  // the polygon, or there are no points defined in the polygon
  // yet).
  if (const auto& point = turtle.GetCurrentState().position;
      lastPolygon.empty() or (lastPolygon.back() != point))
  {
    PDebug(PD_INTERPRET, std::cerr << "AddPolygonEdge: adding first vertex " << point << "\n");
    polygonStack.top().emplace_back(point);
  }

  // Move and add the ending point to the polygon.
  MoveTurtle(turtle, numArgs, args);
  PDebug(PD_INTERPRET,
         std::cerr << "AddPolygonEdge: adding last vertex  " << turtle.GetCurrentState().position
                   << "\n");
  polygonStack.top().emplace_back(turtle.GetCurrentState().position);
}

// Set line width only if changed too much
auto SetLineWidth(const Turtle& turtle, IGenerator& generator) noexcept -> void
{
  static constexpr auto EPSILON = 1e-6F;
  static float s_lastLineWidth  = -1.0F;

  // Don't bother changing line width if 'small enough'.
  // This is an optimization to handle e.g. !(w)[!(w/2)F][!(w/2)F]
  //sort of cases, which happen a lot with trees.
  if (std::fabs(turtle.GetCurrentState().width - s_lastLineWidth) < EPSILON)
  {
    return;
  }

  if (state == State::DRAWING)
  {
    generator.FlushGraphics();
    state = State::START;
  }

  generator.SetWidth();
  s_lastLineWidth = turtle.GetCurrentState().width;
}

// Set color only if changed
auto SetColor(const Turtle& turtle, IGenerator& generator) noexcept -> void
{
  static Color s_lastColor(-1);

  // Don't change color if not needed, again an optimization
  if (turtle.GetCurrentState().color == s_lastColor)
  {
    return;
  }

  if (state == State::DRAWING)
  {
    generator.FlushGraphics();
    state = State::START;
  }

  generator.SetColor();
  s_lastColor = turtle.GetCurrentState().color;
}

// Set texture only if changed
auto SetTexture(const Turtle& turtle, IGenerator& generator) noexcept -> void
{
  static int s_lastTexture(-1);

  // Don't change texture if not needed, again an optimization
  if (turtle.GetCurrentState().texture == s_lastTexture)
  {
    return;
  }

  if (state == State::DRAWING)
  {
    generator.FlushGraphics();
    state = State::START;
  }

  generator.SetTexture();
  s_lastTexture = turtle.GetCurrentState().texture;
}

// f(l) Move without drawing
auto MoveImpl([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
              Turtle& turtle,
              IGenerator& generator,
              const int numArgs,
              const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, std::cerr << "Move          \n");

  if ((state == State::DRAWING) or (state == State::START))
  {
    MoveTurtle(turtle, numArgs, args);
    generator.MoveTo();
  }
  else
  {
    assert(state == State::POLYGON);
    AddPolygonEdge(turtle, numArgs, args);
  }
}

// z Move half standard distance without drawing
auto MoveHalfImpl(const ConstListIterator<Module>& moduleIter,
                  Turtle& turtle,
                  IGenerator& generator,
                  [[maybe_unused]] const int numArgs,
                  [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, std::cerr << "MoveHalf      \n");

  const ArgsArray oneArg = {0.5F * turtle.GetCurrentState().defaultDistance};
  MoveImpl(moduleIter, turtle, generator, 1, oneArg);
}

// F(l) Move while drawing
// Fr(l), Fl(l) - Right and GetLeft edges respectively
auto DrawImpl([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
              Turtle& turtle,
              IGenerator& generator,
              const int numArgs,
              const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, std::cerr << "Draw          \n");

  if (state == State::START)
  {
    generator.StartGraphics();
    state = State::DRAWING;
  }

  if (state == State::DRAWING)
  {
    MoveTurtle(turtle, numArgs, args);
    generator.LineTo();
  }
  else
  {
    assert(state == State::POLYGON);
    AddPolygonEdge(turtle, numArgs, args);
  }
}

// Z Draw half standard distance while drawing
auto DrawHalfImpl(const ConstListIterator<Module>& moduleIter,
                  Turtle& turtle,
                  IGenerator& generator,
                  [[maybe_unused]] const int numArgs,
                  [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, std::cerr << "DrawHalf      \n");

  const auto oneArg = ArgsArray{0.5F * turtle.GetCurrentState().defaultDistance};
  DrawImpl(moduleIter, turtle, generator, 1, oneArg);
}

// -(t) Turn right: NEGATIVE rotation about Z
auto TurnRightImpl([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                   Turtle& turtle,
                   [[maybe_unused]] const IGenerator& generator,
                   const int numArgs,
                   const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, std::cerr << "TurnRight     \n");

  if (0 == numArgs)
  {
    turtle.Turn(Turtle::Direction::NEGATIVE);
  }
  else
  {
    turtle.Turn(-MATHS::ToRadians(args[0]));
  }
}

// +(t) Turn left; POSITIVE rotation about Z
auto TurnLeftImpl([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                  Turtle& turtle,
                  [[maybe_unused]] const IGenerator& generator,
                  const int numArgs,
                  const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, std::cerr << "TurnLeft      \n");

  if (0 == numArgs)
  {
    turtle.Turn(Turtle::Direction::POSITIVE);
  }
  else
  {
    turtle.Turn(MATHS::ToRadians(args[0]));
  }
}

// ^(t) Pitch up; NEGATIVE rotation about Y
auto PitchUpImpl([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                 Turtle& turtle,
                 [[maybe_unused]] const IGenerator& generator,
                 const int numArgs,
                 const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, std::cerr << "PitchUp       \n");

  if (0 == numArgs)
  {
    turtle.Pitch(Turtle::Direction::NEGATIVE);
  }
  else
  {
    turtle.Pitch(-MATHS::ToRadians(args[0]));
  }
}

// &(t) Pitch down; POSITIVE rotation about Y
auto PitchDownImpl([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                   Turtle& turtle,
                   [[maybe_unused]] const IGenerator& generator,
                   const int numArgs,
                   const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, std::cerr << "PitchDown     \n");

  if (0 == numArgs)
  {
    turtle.Pitch(Turtle::Direction::POSITIVE);
  }
  else
  {
    turtle.Pitch(MATHS::ToRadians(args[0]));
  }
}

// /(t) Roll right; POSITIVE rotation about X
auto RollRightImpl([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                   Turtle& turtle,
                   [[maybe_unused]] const IGenerator& generator,
                   const int numArgs,
                   const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, std::cerr << "RollRight     \n");

  if (0 == numArgs)
  {
    turtle.Roll(Turtle::Direction::POSITIVE);
  }
  else
  {
    turtle.Roll(MATHS::ToRadians(args[0]));
  }
}

// \(t) Roll left; NEGATIVE rotation about X
auto RollLeftImpl([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                  Turtle& turtle,
                  [[maybe_unused]] const IGenerator& generator,
                  const int numArgs,
                  const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, std::cerr << "RollLeft      \n");

  if (0 == numArgs)
  {
    turtle.Roll(Turtle::Direction::NEGATIVE);
  }
  else
  {
    turtle.Roll(-MATHS::ToRadians(args[0]));
  }
}

// |  Turn around
auto ReverseImpl([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                 Turtle& turtle,
                 [[maybe_unused]] const IGenerator& generator,
                 [[maybe_unused]] const int numArgs,
                 [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, std::cerr << "Reverse       \n");

  turtle.Reverse();
}

// [  Push turtle state
auto PushImpl([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
              Turtle& turtle,
              [[maybe_unused]] const IGenerator& generator,
              [[maybe_unused]] const int numArgs,
              [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, std::cerr << "Push          \n");

  turtle.Push();
}

// ] Pop turtle state
auto PopImpl(ConstListIterator<Module>& moduleIter,
             Turtle& turtle,
             IGenerator& generator,
             [[maybe_unused]] const int numArgs,
             [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, std::cerr << "Pop           \n");

  turtle.Pop();

  // Look ahead; if the next module is also a Pop, don't do anything.
  // Otherwise, reset the line width, color, and position.
  // This is an optimization to handle deep nesting ([[...][...[...]]]),
  // which happens a lot with trees.

  const Module* const obj = moduleIter.next();
  if (obj != nullptr)
  {
    if (obj->GetName() != RIGHT_BRACKET)
    {
      SetLineWidth(turtle, generator);
      SetColor(turtle, generator);
      generator.MoveTo();
    }
    // Back off one step so the next module is interpreted properly
    moduleIter.previous();
  }
}

// $ Roll to horizontal plane (pg. 57)
auto RollHorizontalImpl([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                        Turtle& turtle,
                        [[maybe_unused]] const IGenerator& generator,
                        [[maybe_unused]] const int numArgs,
                        [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, std::cerr << "RollHorizontal\n");

  turtle.RollHorizontal();
}

// {	Start a new polygon
auto StartPolygonImpl([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                      IGenerator& generator,
                      [[maybe_unused]] const int numArgs,
                      [[maybe_unused]] const ArgsArray& args) -> void
{
  PDebug(PD_INTERPRET, std::cerr << "StartPolygon  \n");

  if (state == State::DRAWING)
  {
    generator.FlushGraphics();
  }

  state = State::POLYGON;
  if (polygonStack.size() > MAX_POLYGONS)
  {
    throw std::runtime_error("StartPolygon: polygon stack filled.");
  }

  polygonStack.emplace();
}

// .	Add a vertex to the current polygon
auto PolygonVertexImpl([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                       const Turtle& turtle,
                       [[maybe_unused]] const IGenerator& generator,
                       [[maybe_unused]] const int numArgs,
                       [[maybe_unused]] const ArgsArray& args) -> void
{
  PDebug(PD_INTERPRET, std::cerr << "PolygonVertex \n");

  if (state != State::POLYGON)
  {
    throw std::runtime_error("PolygonVertexImpl: Add polygon vertex while not in polygon mode.");
  }

  if (polygonStack.empty())
  {
    throw std::runtime_error("PolygonVertexImpl: no polygon being defined.");
  }
  assert(polygonStack.size() <= MAX_POLYGONS);

  polygonStack.top().emplace_back(turtle.GetCurrentState().position);
}

// G	Move without creating a polygon edge
// This seems like it should be illegal outside a polygon, but
// the rose leaf example in the text uses G in this context.
// Until the behavior is specified, just Move the turtle without
// other effects.
auto PolygonMoveImpl([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                     Turtle& turtle,
                     [[maybe_unused]] const IGenerator& generator,
                     const int numArgs,
                     const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, std::cerr << "PolygonMove   \n");

  //if (State != POLYGON) {
  //	  cerr << "Illegal: Polygon Move-without-draw while not in polygon mode\n";
  //	  return;
  //}

  MoveTurtle(turtle, numArgs, args);
}

// }	Close the current polygon
auto EndPolygonImpl([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                    IGenerator& generator,
                    [[maybe_unused]] const int numArgs,
                    [[maybe_unused]] const ArgsArray& args) -> void
{
  PDebug(PD_INTERPRET, std::cerr << "EndPolygon    \n");

  if ((state != State::POLYGON) or (polygonStack.empty()))
  {
    throw std::runtime_error("EndPolygonImpl: no polygon being defined.");
  }

  if (polygonStack.size() > MAX_POLYGONS)
  {
    throw std::runtime_error("EndPolygon: polygon stack too deep, polygon lost.");
  }

  generator.Polygon(polygonStack.top());
  polygonStack.pop();
  // Return to start state if no more polys on stack
  if (polygonStack.empty())
  {
    state = State::START;
  }
}

// @md(f) Multiply GetDefaultDistance by f
auto MultiplyDefaultDistanceImpl([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                                 Turtle& turtle,
                                 [[maybe_unused]] const IGenerator& generator,
                                 const int numArgs,
                                 const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, std::cerr << "MultiplyDefaultDist  \n");

  if (0 == numArgs)
  {
    static constexpr auto MULTIPLIER = 1.1F;
    turtle.SetDefaultDistance(MULTIPLIER * turtle.GetCurrentState().defaultDistance);
  }
  else
  {
    turtle.SetDefaultDistance(args.at(0) * turtle.GetCurrentState().defaultDistance);
  }
}

// @ma(f) Multiply GetDefaultTurnAngle by f
auto MultiplyDefaultTurnAngleImpl([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                                  Turtle& turtle,
                                  [[maybe_unused]] const IGenerator& generator,
                                  const int numArgs,
                                  const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, std::cerr << "MultiplyDefaultTurnAngle  \n");

  const auto currentTurnAngleInDegrees =
      MATHS::ToDegrees(turtle.GetCurrentState().defaultTurnAngleInRadians);

  if (0 == numArgs)
  {
    static constexpr auto MULTIPLIER = 1.1F;
    turtle.SetDefaultTurnAngleInDegrees(MULTIPLIER * currentTurnAngleInDegrees);
  }
  else
  {
    turtle.SetDefaultTurnAngleInDegrees(args[0] * currentTurnAngleInDegrees);
  }
}

// @mw(f) Multiply width by f
auto MultiplyWidthImpl([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                       Turtle& turtle,
                       IGenerator& generator,
                       const int numArgs,
                       const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, std::cerr << "MultiplyWidth  \n");

  if (0 == numArgs)
  {
    static constexpr auto MULTIPLIER = 1.4F;
    turtle.SetWidth(MULTIPLIER * turtle.GetCurrentState().width);
  }
  else
  {
    turtle.SetWidth(args[0] * turtle.GetCurrentState().width);
  }

  SetLineWidth(turtle, generator);
}

// !(d) Set line width
auto ChangeWidthImpl([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                     Turtle& turtle,
                     IGenerator& generator,
                     const int numArgs,
                     const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, std::cerr << "ChangeWidth     \n");

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
auto ChangeColorImpl([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                     Turtle& turtle,
                     IGenerator& generator,
                     const int numArgs,
                     const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, std::cerr << "ChangeColor   \n");

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
auto ChangeTextureImpl([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                       Turtle& turtle,
                       IGenerator& generator,
                       [[maybe_unused]] const int numArgs,
                       const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, std::cerr << "ChangeTexture   \n");

  turtle.SetTexture(static_cast<int>(args[0]));

  SetTexture(turtle, generator);
}

// ~	Draw the following object at the turtle's position and frame
// Needs a standardized object file format for this.
// Should leave graphics mode before drawing object.
auto DrawObjectImpl(const ConstListIterator<Module>& moduleIter,
                    IGenerator& generator,
                    const int numArgs,
                    const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, std::cerr << "DrawObject    \n");

  //  Module* obj= mi.next();
  const Module* const obj = moduleIter.current();
  if (obj != nullptr)
  {
    generator.DrawObject(*obj, numArgs, args);
  }
}

// %	Truncate a branch
// E.g., ignore all modules up to the next ]
// Note that this code is cloned from right-context
// matching in Production::matches()
auto CutBranchImpl(ConstListIterator<Module>& moduleIter,
                   [[maybe_unused]] const Turtle& turtle,
                   [[maybe_unused]] const IGenerator& generator,
                   [[maybe_unused]] const int numArgs,
                   [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  PDebug(PD_INTERPRET, std::cerr << "CutBranch     \n");

  // Must find a matching ]; skip anything else including
  //	bracketed substrings.
  const Module* obj{};
  int brackets;
  for (brackets = 0, obj = moduleIter.next(); obj; obj = moduleIter.next())
  {
    if (obj->GetName() == RIGHT_BRACKET)
    {
      --brackets;
      if (0 == brackets)
      {
        break;
      }
    }
    else if (obj->GetName() == LEFT_BRACKET)
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
//              'e' is the 'susceptibility parameter', between 0 and 1.
// t(0)       - disable tropism
// t(1)       - re-enable tropism with last (T,e) parameters

auto TropismImpl([[maybe_unused]] const ConstListIterator<Module>& moduleIter,
                 Turtle& turtle,
                 [[maybe_unused]] const IGenerator& generator,
                 const int numArgs,
                 const ArgsArray& args) -> void
{
  if (0 == numArgs)
  {
    throw std::runtime_error("Tropism error: no arguments.");
  }

  if (1 == numArgs)
  {
    // Only one parameter; disable tropism if it equals 0,
    // otherwise enable tropism with the specified parameter.
    if (const auto susceptibility = args.at(0); std::fabs(susceptibility) < MATHS::SMALL_FLOAT)
    {
      turtle.DisableTropism();
    }
    else
    {
      turtle.EnableTropism();
      turtle.SetTropismSusceptibility(susceptibility);
    }
    return;
  }

  if (numArgs < 4)
  {
    throw std::runtime_error("Tropism error: not enough arguments.");
  }

  // Construct the tropism vector
  turtle.SetTropismVector(Vector(args.at(0), args.at(1), args.at(2)));
  turtle.SetTropismSusceptibility(args.at(3));
  turtle.EnableTropism();
}

} // namespace

auto Move(ConstListIterator<Module>& moduleIter,
          Turtle& turtle,
          IGenerator& generator,
          const int numArgs,
          const ArgsArray& args) noexcept -> void
{
  MoveImpl(moduleIter, turtle, generator, numArgs, args);
}

auto MoveHalf(ConstListIterator<Module>& moduleIter,
              Turtle& turtle,
              IGenerator& generator,
              const int numArgs,
              const ArgsArray& args) noexcept -> void
{
  MoveHalfImpl(moduleIter, turtle, generator, numArgs, args);
}

auto Draw(ConstListIterator<Module>& moduleIter,
          Turtle& turtle,
          IGenerator& generator,
          const int numArgs,
          const ArgsArray& args) noexcept -> void
{
  DrawImpl(moduleIter, turtle, generator, numArgs, args);
}

auto DrawHalf(ConstListIterator<Module>& moduleIter,
              Turtle& turtle,
              IGenerator& generator,
              const int numArgs,
              const ArgsArray& args) noexcept -> void
{
  DrawHalfImpl(moduleIter, turtle, generator, numArgs, args);
}

auto DrawObject(ConstListIterator<Module>& moduleIter,
                [[maybe_unused]] const Turtle& turtle,
                IGenerator& generator,
                const int numArgs,
                const ArgsArray& args) noexcept -> void
{
  DrawObjectImpl(moduleIter, generator, numArgs, args);
}

auto GeneralisedCylinderStart([[maybe_unused]] ConstListIterator<Module>& moduleIter,
                              [[maybe_unused]] Turtle& turtle,
                              [[maybe_unused]] IGenerator& generator,
                              [[maybe_unused]] const int numArgs,
                              [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  // Not implemented
}

auto GeneralisedCylinderControlPoint([[maybe_unused]] ConstListIterator<Module>& moduleIter,
                                     [[maybe_unused]] Turtle& turtle,
                                     [[maybe_unused]] IGenerator& generator,
                                     [[maybe_unused]] const int numArgs,
                                     [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  // Not implemented
}

auto GeneralisedCylinderEnd([[maybe_unused]] ConstListIterator<Module>& moduleIter,
                            [[maybe_unused]] Turtle& turtle,
                            [[maybe_unused]] IGenerator& generator,
                            [[maybe_unused]] const int numArgs,
                            [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  // Not implemented
}

auto GeneralisedCylinderTangents([[maybe_unused]] ConstListIterator<Module>& moduleIter,
                                 [[maybe_unused]] Turtle& turtle,
                                 [[maybe_unused]] IGenerator& generator,
                                 [[maybe_unused]] const int numArgs,
                                 [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  // Not implemented
}

auto GeneralisedCylinderTangentLengths([[maybe_unused]] ConstListIterator<Module>& moduleIter,
                                       [[maybe_unused]] Turtle& turtle,
                                       [[maybe_unused]] IGenerator& generator,
                                       [[maybe_unused]] const int numArgs,
                                       [[maybe_unused]] const ArgsArray& args) noexcept -> void
{
  // Not implemented
}

auto TurnRight(ConstListIterator<Module>& moduleIter,
               Turtle& turtle,
               const IGenerator& generator,
               const int numArgs,
               const ArgsArray& args) noexcept -> void
{
  TurnRightImpl(moduleIter, turtle, generator, numArgs, args);
}

auto TurnLeft(ConstListIterator<Module>& moduleIter,
              Turtle& turtle,
              const IGenerator& generator,
              const int numArgs,
              const ArgsArray& args) noexcept -> void
{
  TurnLeftImpl(moduleIter, turtle, generator, numArgs, args);
}

auto PitchUp(ConstListIterator<Module>& moduleIter,
             Turtle& turtle,
             const IGenerator& generator,
             const int numArgs,
             const ArgsArray& args) noexcept -> void
{
  PitchUpImpl(moduleIter, turtle, generator, numArgs, args);
}

auto PitchDown(ConstListIterator<Module>& moduleIter,
               Turtle& turtle,
               const IGenerator& generator,
               const int numArgs,
               const ArgsArray& args) noexcept -> void
{
  PitchDownImpl(moduleIter, turtle, generator, numArgs, args);
}

auto RollRight(ConstListIterator<Module>& moduleIter,
               Turtle& turtle,
               const IGenerator& generator,
               const int numArgs,
               const ArgsArray& args) noexcept -> void
{
  RollRightImpl(moduleIter, turtle, generator, numArgs, args);
}

auto RollLeft(ConstListIterator<Module>& moduleIter,
              Turtle& turtle,
              const IGenerator& generator,
              const int numArgs,
              const ArgsArray& args) noexcept -> void
{
  RollLeftImpl(moduleIter, turtle, generator, numArgs, args);
}

auto Reverse(ConstListIterator<Module>& moduleIter,
             Turtle& turtle,
             const IGenerator& generator,
             const int numArgs,
             const ArgsArray& args) noexcept -> void
{
  ReverseImpl(moduleIter, turtle, generator, numArgs, args);
}

auto RollHorizontal(ConstListIterator<Module>& moduleIter,
                    Turtle& turtle,
                    const IGenerator& generator,
                    const int numArgs,
                    const ArgsArray& args) noexcept -> void
{
  RollHorizontalImpl(moduleIter, turtle, generator, numArgs, args);
}

auto Push(ConstListIterator<Module>& moduleIter,
          Turtle& turtle,
          const IGenerator& generator,
          const int numArgs,
          const ArgsArray& args) noexcept -> void
{
  PushImpl(moduleIter, turtle, generator, numArgs, args);
}

auto Pop(ConstListIterator<Module>& moduleIter,
         Turtle& turtle,
         IGenerator& generator,
         const int numArgs,
         const ArgsArray& args) noexcept -> void
{
  PopImpl(moduleIter, turtle, generator, numArgs, args);
}

auto CutBranch(ConstListIterator<Module>& moduleIter,
               const Turtle& turtle,
               const IGenerator& generator,
               const int numArgs,
               const ArgsArray& args) noexcept -> void
{
  CutBranchImpl(moduleIter, turtle, generator, numArgs, args);
}

auto MultiplyDefaultDistance(ConstListIterator<Module>& moduleIter,
                             Turtle& turtle,
                             const IGenerator& generator,
                             const int numArgs,
                             const ArgsArray& args) noexcept -> void
{
  MultiplyDefaultDistanceImpl(moduleIter, turtle, generator, numArgs, args);
}

auto MultiplyDefaultTurnAngle(ConstListIterator<Module>& moduleIter,
                              Turtle& turtle,
                              const IGenerator& generator,
                              const int numArgs,
                              const ArgsArray& args) noexcept -> void
{
  MultiplyDefaultTurnAngleImpl(moduleIter, turtle, generator, numArgs, args);
}

auto MultiplyWidth(ConstListIterator<Module>& moduleIter,
                   Turtle& turtle,
                   IGenerator& generator,
                   const int numArgs,
                   const ArgsArray& args) noexcept -> void
{
  MultiplyWidthImpl(moduleIter, turtle, generator, numArgs, args);
}

auto ChangeWidth(ConstListIterator<Module>& moduleIter,
                 Turtle& turtle,
                 IGenerator& generator,
                 const int numArgs,
                 const ArgsArray& args) noexcept -> void
{
  ChangeWidthImpl(moduleIter, turtle, generator, numArgs, args);
}

auto ChangeColor(ConstListIterator<Module>& moduleIter,
                 Turtle& turtle,
                 IGenerator& generator,
                 const int numArgs,
                 const ArgsArray& args) noexcept -> void
{
  ChangeColorImpl(moduleIter, turtle, generator, numArgs, args);
}

auto ChangeTexture(ConstListIterator<Module>& moduleIter,
                   Turtle& turtle,
                   IGenerator& generator,
                   const int numArgs,
                   const ArgsArray& args) noexcept -> void
{
  ChangeTextureImpl(moduleIter, turtle, generator, numArgs, args);
}

auto StartPolygon(ConstListIterator<Module>& moduleIter,
                  [[maybe_unused]] const Turtle& turtle,
                  IGenerator& generator,
                  const int numArgs,
                  const ArgsArray& args) -> void
{
  StartPolygonImpl(moduleIter, generator, numArgs, args);
}

auto PolygonVertex(ConstListIterator<Module>& moduleIter,
                   const Turtle& turtle,
                   const IGenerator& generator,
                   const int numArgs,
                   const ArgsArray& args) -> void
{
  PolygonVertexImpl(moduleIter, turtle, generator, numArgs, args);
}

auto PolygonMove(ConstListIterator<Module>& moduleIter,
                 Turtle& turtle,
                 const IGenerator& generator,
                 const int numArgs,
                 const ArgsArray& args) noexcept -> void
{
  PolygonMoveImpl(moduleIter, turtle, generator, numArgs, args);
}

auto EndPolygon(ConstListIterator<Module>& moduleIter,
                [[maybe_unused]] const Turtle& turtle,
                IGenerator& generator,
                const int numArgs,
                const ArgsArray& args) -> void
{
  EndPolygonImpl(moduleIter, generator, numArgs, args);
}

auto Tropism(ConstListIterator<Module>& moduleIter,
             Turtle& turtle,
             const IGenerator& generator,
             const int numArgs,
             const ArgsArray& args) -> void
{
  TropismImpl(moduleIter, turtle, generator, numArgs, args);
}

} // namespace LSYS
