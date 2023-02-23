#include "parsed_model.h"

#include "debug.h"
#include "expression.h"
#include "l_sys_model.h"
#include "parser.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

int ParseDebug = 0;

namespace LSYS
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
                                      const Properties& initialProperties,
                                      const bool setDefaults) -> Properties
{
  static constexpr auto DEFAULT_MAX_GEN       = 0; // Default: just do sanity checking
  static constexpr auto DEFAULT_TURN_ANGLE    = 90.0F; // Default: turn at right angles
  static constexpr auto DEFAULT_LINE_WIDTH    = 1.0F; // Default line aspect ratio (1/100)
  static constexpr auto DEFAULT_LINE_DISTANCE = 1.0F;

  auto newProperties = initialProperties;

  if (newProperties.maxGen < 0)
  {
    if (Value value; not symbolTable.Lookup("maxgen", value))
    {
      if (setDefaults)
      {
        newProperties.maxGen = DEFAULT_MAX_GEN;
      }
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
      if (setDefaults)
      {
        newProperties.turnAngle = DEFAULT_TURN_ANGLE;
      }
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
      if (setDefaults)
      {
        newProperties.lineWidth = DEFAULT_LINE_WIDTH;
      }
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
      if (setDefaults)
      {
        newProperties.lineDistance = DEFAULT_LINE_DISTANCE;
      }
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
  if (not std::filesystem::exists(properties.inputFilename))
  {
    std::cerr << "Could not find input file '" << properties.inputFilename << "'.\n";
    throw std::runtime_error("Could not find input file.");
  }

  auto model = std::make_unique<LSysModel>();

  SetSymbolTableValues(model->GetSymbolTable(), properties);

  ::set_parser_globals(model.get());
  ::set_parser_input(properties.inputFilename.c_str());

  ::yyparse(); // Parse input file

  if (model->GetStartModuleList() == nullptr)
  {
    std::cerr << "No starting module list.\n";
    throw std::runtime_error("No starting module list.");
  }

  PDebug(PD_MAIN, std::cerr << "Starting module list: " << *model->GetStartModuleList() << "\n");
  PDebug(PD_PRODUCTION, std::cerr << "\nProductions:\n" << model->GetRules() << "\n");

  return model;
}

namespace
{

// TODO(glk) - String utils
const std::string WHITESPACE = " \n\r\t\f\v";

[[nodiscard]] auto LeftTrim(const std::string& str) -> std::string
{
  const auto start = str.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : str.substr(start);
}

[[nodiscard]] auto RightTrim(const std::string& str) -> std::string
{
  const auto end = str.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}

[[nodiscard]] inline auto Trim(const std::string& str) -> std::string
{
  return RightTrim(LeftTrim(str));
}

[[nodiscard]] auto GetTrimmedLine(std::istream& file) -> std::string
{
  std::string line;
  std::getline(file, line);
  line = Trim(line);
  return line;
}

} // namespace

auto GetBoundingBox3d(const std::string& filename) -> BoundingBox3d
{
  auto boundingBox3d = BoundingBox3d{};
  auto inputFile     = std::make_unique<std::ifstream>(filename);
  if (not *inputFile)
  {
    std::cerr << "Could not open bounds file '" << filename << "'.\n";
    throw std::runtime_error("Could not open bounds file.");
  }

  while (true)
  {
    assert(not inputFile->eof());

    const auto line = GetTrimmedLine(*inputFile);

    if (line.empty())
    {
      continue;
    }

    if (0 == line.rfind("bounds", 0))
    {
      auto nextLine = GetTrimmedLine(*inputFile);
      assert(nextLine.rfind("min: ", 0) == 0);
      std::string ignore;
      std::istringstream iss{nextLine};
      if (not(iss >> ignore >> boundingBox3d.min.x >> boundingBox3d.min.y >> boundingBox3d.min.z))
      {
        throw std::runtime_error("Expected boundingBox3d.min.");
      }

      nextLine = GetTrimmedLine(*inputFile);
      assert(nextLine.rfind("max: ", 0) == 0);
      iss = std::istringstream{nextLine};
      if (not(iss >> ignore >> boundingBox3d.max.x >> boundingBox3d.max.y >> boundingBox3d.max.z))
      {
        throw std::runtime_error("Expected boundingBox3d.max.");
      }

      break;
    }
  }

  return boundingBox3d;
}

} // namespace LSYS
