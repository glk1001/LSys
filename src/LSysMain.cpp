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
 * $Log: main.c,v $
 * Revision 1.7  94/08/26  16:37:44  leech
 * Move yyparse() declaration to parser.h.
 *
 * Revision 1.6  92/06/22  01:46:17  leech
 * Added trace option for collecting memory use statistics.
 *
 * Revision 1.5  92/06/22  00:21:15  leech
 * G++ iostream workaround.
 *
 * Revision 1.4  91/10/10  20:01:55  leech
 * Use the PlantModel global object containing symbol tables, rules, etc.
 * instead of a separate global for each one. Move command-line argument
 * processing into a separate routine.
 *
 * Revision 1.3  91/03/20  10:34:46  leech
 * Support for new generator. Support for G++.
 *
 * Revision 1.2  90/10/12  18:48:24  leech
 * First public release.
 *
 */
#include "CommandLineOptions.h"
#include "GenericGenerator.h"
#include "IGenerator.h"
#include "Interpret.h"
#include "LSysModel.h"
#include "List.h"
#include "Parser.h"
#include "RadianceGenerator.h"
#include "Rand.h"
#include "Value.h"
#include "debug.h"

#include <filesystem>
#include <iomanip>
#include <stdexcept>
#include <string>

using LSys::GenericGenerator;
using LSys::IGenerator;
using LSys::List;
using LSys::LSysModel;
using LSys::Module;
using LSys::RadianceGenerator;
using LSys::SymbolTable;
using LSys::Value;
using Utilities::CommandLineOptions;

using enum Utilities::CommandLineOptions::OptionTypes;
using enum Utilities::CommandLineOptions::OptionReturnCode;

#if YYDEBUG != 0
extern int yydebug; // nonzero means print parse trace
#endif
int ParseDebug = 0;

namespace
{

// Return a copy of a filename stripped of its trailing extension.
[[nodiscard]] auto GetBaseFilename(const char* const filename) -> std::string
{
  return std::filesystem::path{filename}.stem();
}

/***
  // Print a message describing program options
  void usage(const char* progname)
  {
    cerr << "Usage: " << progname
         << " [-maxgen n] [-delta angle] [-width w]"
         << " [-display] [-stats] [-generic] [-o file] [-D [lmEGILMNPSA]\n"
         << "-maxgen  sets number of generations to produce\n"
         << "-delta   sets default turn angle\n"
         << "-width   sets default line width\n"
         << "-seed    sets seed GetFloatValue\n"
         << "-display displays the L-systems produced at each generation\n"
         << "-stats   displays module stats for each generation\n";

    cerr << "-generic generates a generic database format\n"
         << "-i       specifies input file (default standard input)\n"
         << "-o       specifies output file (default 'output.ps')\n"
         << "-D enables debugging statements for any of:\n"
         << "    g - output generation\n"
         << "    l - memory allocation\n";

    cerr << "    m - main program loop\n"
         << "    E - Expressions\n"
         << "    G - Parsing\n"
         << "    I - Interpretation\n"
         << "    L - Lexical scanning\n"
         << "    M - Modules\n"
         << "    N - Hashed names\n"
         << "    P - Productions\n"
         << "    S - Symbols\n"
         << "    A - all of the above" << endl;
  }
  ***/

struct Properties
{
  std::string inputFilename  = "";
  const char* outputFilename = "";
  const char* boundsFilename = "";
  int maxGen                 = -1;
  float turnAngle            = -1.0F;
  float lineWidth            = -1.0F;
  float lineDistance         = -1.0F;
  int64_t seed               = -1L; // Seed for random numbers: -1 means use time
};

constexpr auto DEFAULT_MAX_GEN       = 0; // Default: just do sanity checking
constexpr auto DEFAULT_TURN_ANGLE    = 90.0F; // Default: turn at right angles
constexpr auto DEFAULT_LINE_WIDTH    = 1.0F; // Default line aspect ratio (1/100)
constexpr auto DEFAULT_LINE_DISTANCE = 1.0F;

// Look up defaults in the symbol table, if not overridden by specified
// arguments.
auto SetNewDefaults(const SymbolTable<Value>& symbolTable, Properties& properties) -> void
{
  if (properties.maxGen < 0)
  {
    if (Value value; not symbolTable.Lookup("maxgen", value))
    {
      properties.maxGen = DEFAULT_MAX_GEN;
    }
    else if (not value.GetIntValue(properties.maxGen))
    {
      std::cerr << "Invalid float value specified for maxgen: " << value << "\n";
      throw std::runtime_error("Invalid float value specified for maxgen.");
    }
  }

  if (properties.turnAngle < 0.0F)
  {
    if (Value value; not symbolTable.Lookup("delta", value))
    {
      properties.turnAngle = DEFAULT_TURN_ANGLE;
    }
    else if (not value.GetFloatValue(properties.turnAngle))
    {
      std::cerr << "Invalid float value specified for delta: " << value << "\n";
      throw std::runtime_error("Invalid float value specified for delta.");
    }
  }

  if (properties.lineWidth < 0.0F)
  {
    if (Value value; not symbolTable.Lookup("width", value))
    {
      properties.lineWidth = DEFAULT_LINE_WIDTH;
    }
    else if (not value.GetFloatValue(properties.lineWidth))
    {
      std::cerr << "Invalid float value specified for width: " << value << "\n";
      throw std::runtime_error("Invalid float value specified for width.");
    }
  }

  if (properties.lineDistance < 0.0F)
  {
    if (Value value; not symbolTable.Lookup("distance", value))
    {
      properties.lineDistance = DEFAULT_LINE_DISTANCE;
    }
    else if (not value.GetFloatValue(properties.lineDistance))
    {
      std::cerr << "Invalid float value specified for distance: " << value << "\n";
      throw std::runtime_error("Invalid float value specified for distance.");
    }
  }
}

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

[[nodiscard]] auto GetFormattedHeader(const Properties& properties) -> std::string
{
  auto strStream = std::ostringstream{};

  strStream << "  Input file = " << properties.inputFilename << "\n";
  strStream << "  Output file = " << properties.outputFilename << "\n";
  strStream << "  Bounds file = " << properties.boundsFilename << "\n";
  strStream << "  Seed = " << properties.seed << "\n";
  strStream << "  MaxGen = " << properties.maxGen << "\n";
  strStream << "  Width = " << properties.lineWidth << "\n";
  strStream << "  Delta = " << properties.turnAngle << "\n";
  strStream << "  Distance = " << properties.lineDistance << "\n";

  return strStream.str();
}

[[nodiscard]] auto GetGenerator(const Properties& properties) -> std::unique_ptr<IGenerator>
{
  // auto generator =
  //     std::make_unique<RadianceGenerator>(properties.outputFilename, properties.boundsFilename);
  auto generator =
      std::make_unique<GenericGenerator>(properties.outputFilename, properties.boundsFilename);
  generator->SetName(GetBaseFilename(properties.outputFilename));
  generator->SetHeader(GetFormattedHeader(properties));

  return generator;
}

struct CommandLineArgs
{
  bool success = false;
  Properties properties{};
  bool display = false;
  bool stats   = false;
};

[[nodiscard]] auto GetPropertiesFromCommandLine(const int argc, const char* argv[])
    -> CommandLineArgs
{
  static constexpr const auto* HELP_DESCR     = "displays help for this program";
  static constexpr const auto* MAX_GEN_DESCR  = "sets the number of generations to produce";
  static constexpr const auto* DELTA_DESCR    = "sets the default turn angle";
  static constexpr const auto* DISTANCE_DESCR = "sets the default line length";
  static constexpr const auto* WIDTH_DESCR    = "sets the default line width";
  static constexpr const auto* SEED_DESCR     = "sets the seed GetFloatValue";
  static constexpr const auto* DISPLAY_DESCR  = "displays the L-systems for each generation";
  static constexpr const auto* STATS_DESCR    = "displays module statistics for each generation";
  static constexpr const auto* OUTPUT_DESCR   = "output filename";
  static constexpr const auto* BOUNDS_DESCR   = "bounds filename";

  auto help1 = false;
  auto help2 = false;

  auto commandLineArgs = CommandLineArgs{};

  CommandLineOptions cmdOpts{};
  cmdOpts.Add('?', "", HELP_DESCR, NO_ARGS, &help1);
  cmdOpts.Add('H', "help", HELP_DESCR, NO_ARGS, &help2);
  cmdOpts.Add(' ', "display", DISPLAY_DESCR, NO_ARGS, &commandLineArgs.display);
  cmdOpts.Add(' ', "stats", STATS_DESCR, NO_ARGS, &commandLineArgs.stats);
  cmdOpts.Add('m', "maxgen <int>", MAX_GEN_DESCR, REQUIRED_ARG, &commandLineArgs.properties.maxGen);
  cmdOpts.Add('d', "delta <int>", DELTA_DESCR, REQUIRED_ARG, &commandLineArgs.properties.turnAngle);
  cmdOpts.Add(' ',
              "distance <int>",
              DISTANCE_DESCR,
              REQUIRED_ARG,
              &commandLineArgs.properties.lineDistance);
  cmdOpts.Add('w', "width <int>", WIDTH_DESCR, REQUIRED_ARG, &commandLineArgs.properties.lineWidth);
  cmdOpts.Add('s', "seed <int>", SEED_DESCR, REQUIRED_ARG, &commandLineArgs.properties.seed);
  cmdOpts.Add('o',
              "output <string>",
              OUTPUT_DESCR,
              REQUIRED_ARG,
              &commandLineArgs.properties.outputFilename);
  cmdOpts.Add('b',
              "bounds <string>",
              BOUNDS_DESCR,
              REQUIRED_ARG,
              &commandLineArgs.properties.boundsFilename);
  //  cmdOpts.Add(' ', "generic", noArgs, &generic);

  std::vector<std::string> positionalParams{};
  cmdOpts.SetPositional(1, 1, &positionalParams);

  if (const CommandLineOptions::OptionReturnCode retCode = cmdOpts.ProcessOptions(argc, argv);
      retCode != OK)
  {
    std::cerr << "\n";
    cmdOpts.Usage(std::cerr, "input file...");
    return commandLineArgs;
  }
  if (help1 or help2)
  {
    std::cerr << "\n";
    cmdOpts.Usage(std::cerr, "input file...");
    return commandLineArgs;
  }

  if (nullptr == commandLineArgs.properties.outputFilename)
  {
    std::cerr << "\n";
    std::cerr << "Must supply -o (output filename) option\n\n";
    cmdOpts.Usage(std::cerr, "input file...");
    return commandLineArgs;
  }
  commandLineArgs.properties.inputFilename = positionalParams[0];

  commandLineArgs.success = true;

  return commandLineArgs;
}

auto PrintStartInfo(const LSysModel& model, const bool display, const bool stats) -> void
{
  if (display)
  {
    std::cout << "Gen 0: " << *model.start << "\n";
  }
  if (stats)
  {
    std::cerr << "\n";
  }
}

auto PrintGenInfo(const int gen,
                  const List<Module>& moduleList,
                  const bool display,
                  const bool stats) -> void
{
  if (display)
  {
    std::cout << "Gen " << gen << ": " << moduleList << "\n";
  }
  if (stats)
  {
    std::cerr << "Gen " << std::setw(3) << gen << ": # modules= " << std::setw(5)
              << moduleList.size() << "\n";
  }
}

auto PrintInterpretStart(const std::string& header) -> void
{
  std::cerr << "\n";
  std::cerr << "Generating database...\n";
  std::cerr << header;
  std::cerr << "\n";
}

} // namespace

int main(const int argc, const char* argv[])
{
  try
  {
    auto [success, properties, display, stats] = GetPropertiesFromCommandLine(argc, argv);
    if (not success)
    {
      return 1;
    }

    // Initialize random number generator.
    ::srand48((properties.seed != -1) ? properties.seed : ::time(nullptr));

    const auto model = GetParsedModel(properties);
    SetNewDefaults(model->symbolTable, properties);

    // For each generation, apply appropriate productions in parallel to all modules.
    PrintStartInfo(*model, display, stats);
    auto moduleList = model->start;
    for (int gen = 1; gen <= properties.maxGen; ++gen)
    {
      moduleList = model->Generate(moduleList.get());
      PrintGenInfo(gen, *moduleList, display, stats);
    }

    // Construct an output generator and apply it to the final module list.
    auto generator = GetGenerator(properties);
    PrintInterpretStart(generator->GetHeader());
    Interpret(*moduleList,
              *generator,
              properties.turnAngle,
              properties.lineWidth,
              properties.lineDistance);

    PDebug(PD_MAIN, std::cerr << "About to exit.\n");
    return 0;
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
    return 1;
  }
}
