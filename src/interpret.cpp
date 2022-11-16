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
 * $Log: interpret.c,v $
 * Revision 1.3  91/03/20  10:36:40  leech
 * Support for G++.
 *
 * Revision 1.2  90/10/12  18:48:19  leech
 * First public release.
 *
 */

#include "interpret.h"

#include "actions.h"
#include "consts.h"
#include "debug.h"
#include "expression.h"
#include "generator.h"
#include "module.h"
#include "symbol_table.h"
#include "turtle.h"

#include <iostream>
#include <stdexcept>
#include <string>

namespace L_SYSTEM
{

namespace
{

// Set up the default actions for interpretation.
[[nodiscard]] auto GetActionSymbolTable() -> SymbolTable<ActionFunc>
{
  SymbolTable<ActionFunc> symbolTable{};

  symbolTable.Enter(DRAW_OBJECT_START, DrawObject);
  symbolTable.Enter("f", Move);
  symbolTable.Enter("z", MoveHalf);
  symbolTable.Enter("F", Draw);
  symbolTable.Enter("Fl", Draw);
  symbolTable.Enter("Fr", Draw);
  symbolTable.Enter("Z", DrawHalf);
  symbolTable.Enter("+", TurnLeft);
  symbolTable.Enter("-", TurnRight);
  symbolTable.Enter("&", PitchDown);
  symbolTable.Enter("^", PitchUp);
  symbolTable.Enter("\\", RollLeft);
  symbolTable.Enter("/", RollRight);
  symbolTable.Enter("|", Reverse);
  symbolTable.Enter("$", RollHorizontal);
  symbolTable.Enter("[", Push);
  symbolTable.Enter("]", Pop);
  symbolTable.Enter("%", CutBranch);
  symbolTable.Enter("@md", MultiplyDefaultDistance);
  symbolTable.Enter("@ma", MultiplyDefaultTurnAngle);
  symbolTable.Enter("@mw", MultiplyWidth);
  symbolTable.Enter("!", ChangeWidth);
  symbolTable.Enter("'", ChangeColor);
  symbolTable.Enter("@Tx", ChangeTexture);
  symbolTable.Enter("{", StartPolygon);
  symbolTable.Enter(".", PolygonVertex);
  symbolTable.Enter("G", PolygonMove);
  symbolTable.Enter("}", EndPolygon);
  symbolTable.Enter("t", Tropism);
  symbolTable.Enter("@Gs", GeneralisedCylinderStart);
  symbolTable.Enter("@Gc", GeneralisedCylinderControlPoint);
  symbolTable.Enter("@Ge", GeneralisedCylinderEnd);
  symbolTable.Enter("@Gr", GeneralisedCylinderTangents);
  symbolTable.Enter("@Gt", GeneralisedCylinderTangentLengths);

  return symbolTable;
}

[[nodiscard]] auto GetModuleName(const Module& mod) -> std::string
{
  if (auto moduleName = mod.GetName().str(); moduleName[0] != DRAW_OBJECT_START_CHAR)
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
auto Interpret(const List<Module>& moduleList,
               IGenerator& generator,
               const float turn,
               const float width,
               const float distance) -> void
{
  Turtle turtle(width, turn);
  turtle.SetHeading(Vector(0, 1, 0)); // H = +Y
  turtle.SetLeft(Vector(-1, 0, 0)); // Left = -X
  turtle.SetUp(Vector(0, 0, 1)); // Up = +Z
  turtle.SetGravity(Vector(0, 1, 0));
  turtle.SetWidth(width); // ???????????????????????????????????????????????????????
  turtle.SetDefaultDistance(distance);

  generator.SetTurtle(turtle);
  generator.Prelude();

  const auto actionSymbolTable = GetActionSymbolTable();
  auto modList                 = ConstListIterator<Module>{moduleList};

  for (const auto* mod = modList.first(); mod != nullptr; mod = modList.next())
  {
    // TODO(glk) - Make failed lookup an exception.
    ActionFunc actionFunc;
    if (not actionSymbolTable.Lookup(GetModuleName(*mod), actionFunc))
    {
      PDebug(PD_INTERPRET, std::cerr << "Unknown action for " << *mod << "\n");
      continue;
      // TODO(glk) - This is what we want in the long term.
      std::cerr << "Unknown action for " << *mod << "\n";
      throw std::runtime_error("Unknown action.");
    }

    // Fetch defined parameters
    const auto [numArgs, args] = GetActionArgsArray(*mod);

    actionFunc(modList, turtle, generator, numArgs, args);

    PDebug(PD_INTERPRET, std::cerr << turtle);
  }

  generator.Postscript();
}

} // namespace L_SYSTEM
