#include "parsed_model.h"

#include "debug.h"
#include "expression.h"
#include "l_sys_model.h"
#include "module.h"
#include "parser.h"

#include <iostream>

int ParseDebug = 0;

namespace L_SYSTEM
{

namespace
{

// Set override values for symbol table.
auto SetSymbolTableValues(SymbolTable<Value>& symbolTable, const Properties& properties) -> void
{
  if (properties.maxGen > 0)
  {
    symbolTable.Enter("maxgen", Value(properties.maxGen));
  }
  if (properties.turnAngle > 0.0F)
  {
    symbolTable.Enter("delta", Value(properties.turnAngle));
  }
  if (properties.lineWidth > 0.0F)
  {
    symbolTable.Enter("width", Value(properties.lineWidth));
  }
  if (properties.lineDistance > 0.0F)
  {
    symbolTable.Enter("distance", Value(properties.lineDistance));
  }
}

} // namespace

[[nodiscard]] auto GetFinalProperties(const SymbolTable<Value>& symbolTable,
                                      const Properties& properties) -> Properties
{
  static constexpr auto DEFAULT_MAX_GEN       = 0; // Default: just do sanity checking
  static constexpr auto DEFAULT_TURN_ANGLE    = 90.0F; // Default: turn at right angles
  static constexpr auto DEFAULT_LINE_WIDTH    = 1.0F; // Default line aspect ratio (1/100)
  static constexpr auto DEFAULT_LINE_DISTANCE = 1.0F;

  auto newProperties = properties;

  if (newProperties.maxGen < 0)
  {
    if (Value value; not symbolTable.Lookup("maxgen", value))
    {
      newProperties.maxGen = DEFAULT_MAX_GEN;
    }
    else if (not value.GetIntValue(newProperties.maxGen))
    {
      std::cerr << "Invalid float value specified for maxgen: " << value << "\n";
      throw std::runtime_error("Invalid float value specified for maxgen.");
    }
  }

  if (newProperties.turnAngle < 0.0F)
  {
    if (Value value; not symbolTable.Lookup("delta", value))
    {
      newProperties.turnAngle = DEFAULT_TURN_ANGLE;
    }
    else if (not value.GetFloatValue(newProperties.turnAngle))
    {
      std::cerr << "Invalid float value specified for delta: " << value << "\n";
      throw std::runtime_error("Invalid float value specified for delta.");
    }
  }

  if (newProperties.lineWidth < 0.0F)
  {
    if (Value value; not symbolTable.Lookup("width", value))
    {
      newProperties.lineWidth = DEFAULT_LINE_WIDTH;
    }
    else if (not value.GetFloatValue(newProperties.lineWidth))
    {
      std::cerr << "Invalid float value specified for width: " << value << "\n";
      throw std::runtime_error("Invalid float value specified for width.");
    }
  }

  if (newProperties.lineDistance < 0.0F)
  {
    if (Value value; not symbolTable.Lookup("distance", value))
    {
      newProperties.lineDistance = DEFAULT_LINE_DISTANCE;
    }
    else if (not value.GetFloatValue(newProperties.lineDistance))
    {
      std::cerr << "Invalid float value specified for distance: " << value << ".\n";
      throw std::runtime_error("Invalid float value specified for distance.");
    }
  }

  return newProperties;
}

[[nodiscard]] auto GetParsedModel(const Properties& properties) -> std::unique_ptr<LSysModel>
{
  auto model = std::make_unique<LSysModel>();

  SetSymbolTableValues(model->symbolTable, properties);

  ::set_parser_globals(model.get());
  ::set_parser_input(properties.inputFilename.c_str());

  ::yyparse(); // Parse input file

  if (model->start == nullptr)
  {
    PDebug(PD_MAIN, std::cerr << "No starting module list.\n");
    throw std::runtime_error("No starting module list.");
  }

  PDebug(PD_MAIN, std::cerr << "Starting module list: " << *model->start << "\n");
  PDebug(PD_PRODUCTION, std::cerr << "\nProductions:\n" << model->rules << "\n");

  return model;
}

} // namespace L_SYSTEM
