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
 * $Log: interpret.c,v $
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
#include "IGenerator.h"
#include "Module.h"
#include "SymbolTable.h"
#include "Turtle.h"
#include "debug.h"

#include <iostream>
#include <string>

namespace LSys
{

namespace
{

// Set up the default actions for interpretation
[[nodiscard]] auto GetActionSymbolTable() -> SymbolTable<ActionFunc>
{
  SymbolTable<ActionFunc> symbolTable{};

  symbolTable.enter(DRAW_OBJECT_START, DrawObject);
  symbolTable.enter("f", Move);
  symbolTable.enter("z", MoveHalf);
  symbolTable.enter("F", Draw);
  symbolTable.enter("Fl", Draw);
  symbolTable.enter("Fr", Draw);
  symbolTable.enter("Z", DrawHalf);
  symbolTable.enter("+", TurnLeft);
  symbolTable.enter("-", TurnRight);
  symbolTable.enter("&", PitchDown);
  symbolTable.enter("^", PitchUp);
  symbolTable.enter("\\", RollLeft);
  symbolTable.enter("/", RollRight);
  symbolTable.enter("|", Reverse);
  symbolTable.enter("$", RollHorizontal);
  symbolTable.enter("[", Push);
  symbolTable.enter("]", Pop);
  symbolTable.enter("%", CutBranch);
  symbolTable.enter("@md", MultiplyDefaultDistance);
  symbolTable.enter("@ma", MultiplyDefaultTurnAngle);
  symbolTable.enter("@mw", MultiplyWidth);
  symbolTable.enter("!", ChangeWidth);
  symbolTable.enter("'", ChangeColor);
  symbolTable.enter("@Tx", ChangeTexture);
  symbolTable.enter("{", StartPolygon);
  symbolTable.enter(".", PolygonVertex);
  symbolTable.enter("G", PolygonMove);
  symbolTable.enter("}", EndPolygon);
  symbolTable.enter("t", Tropism);
  symbolTable.enter("@Gs", GeneralisedCylinderStart);
  symbolTable.enter("@Gc", GeneralisedCylinderControlPoint);
  symbolTable.enter("@Ge", GeneralisedCylinderEnd);
  symbolTable.enter("@Gr", GeneralisedCylinderTangents);
  symbolTable.enter("@Gt", GeneralisedCylinderTangentLengths);

  return symbolTable;
}

[[nodiscard]] auto GetModuleName(const Module& mod) -> std::string
{
  if (const auto moduleName = mod.GetName().str(); moduleName[0] != DRAW_OBJECT_START_CHAR)
  {
    return moduleName;
  }

  return DRAW_OBJECT_START;
}

[[nodiscard]] auto GetActionArgsArray(const Module& mod) -> std::pair<int, ArgsArray>
{
  int numArgs = 0;
  ArgsArray args{};

  for (auto i = 0U; i < MAX_ARGS; ++i)
  {
    if (not mod.GetFloat(args[i], i))
    {
      break;
    }
    ++numArgs;
  }

  return {numArgs, args};
}

} // namespace


// Interpret a bound Left-system, producing output on the specified stream.
// Default values for line width, movement, and turn angles are specified.
void Interpret(const List<Module>& moduleList,
               IGenerator& generator,
               const float turn,
               const float width,
               const float distance)
{
  Turtle turtle(turn, width);
  turtle.SetHeading(Vector(0, 1, 0)); // H = +Y
  turtle.SetLeft(Vector(-1, 0, 0)); // Left = -X
  turtle.SetUp(Vector(0, 0, 1)); // Up = +Z
  turtle.SetGravity(Vector(0, 1, 0));
  turtle.SetWidth(width); // ???????????????????????????????????????????????????????
  turtle.SetDefaultDistance(distance);

  generator.Prelude(turtle);

  const auto actionSymbolTable = GetActionSymbolTable();
  auto modList                 = ConstListIterator<Module>{moduleList};

  for (const auto* mod = modList.first(); mod != nullptr; mod = modList.next())
  {
    // TODO(glk) - Make failed lookup an exception.
    ActionFunc actionFunc;
    if (not actionSymbolTable.lookup(GetModuleName(*mod), actionFunc))
    {
      PDebug(PD_INTERPRET, std::cerr << "Unknown action for " << *mod << "\n");
      continue;
    }

    // Fetch defined parameters
    const auto [numArgs, args] = GetActionArgsArray(*mod);

    actionFunc(modList, turtle, generator, numArgs, args);

    PDebug(PD_INTERPRET, std::cerr << turtle);
  }

  generator.Postscript(turtle);
}

} // namespace LSys
