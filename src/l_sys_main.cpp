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
#include "command_line_options.h"
#include "generator.h"
#include "generic_generator.h"
#include "interpret.h"
#include "list.h"
#include "parsed_model.h"
#include "radiance_generator.h"
#include "rand.h"
#include "value.h"

#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <string>

using L_SYSTEM::GenericGenerator;
using L_SYSTEM::GetFinalProperties;
using L_SYSTEM::IGenerator;
using L_SYSTEM::Interpreter;
using L_SYSTEM::List;
using L_SYSTEM::LSysModel;
using L_SYSTEM::Module;
using L_SYSTEM::Properties;
using L_SYSTEM::RadianceGenerator;
using L_SYSTEM::SetRandFunc;
using Utilities::CommandLineOptions;

using OptionTypes      = Utilities::CommandLineOptions::OptionTypes;
using OptionReturnCode = Utilities::CommandLineOptions::OptionReturnCode;

namespace
{

struct CommandLineArgs
{
  bool success = false;
  Properties properties{};
  const char* outputFilename = "";
  const char* boundsFilename = "";
  bool display               = false;
  bool stats                 = false;
};

// Return a copy of a filename stripped of its trailing extension.
[[nodiscard]] auto GetBaseFilename(const std::string& filename) -> std::string
{
  return std::filesystem::path{filename}.stem();
}

[[nodiscard]] auto GetFormattedHeader(const Properties& properties,
                                      const std::string& outputFilename,
                                      const std::string& boundsFilename) -> std::string
{
  auto strStream = std::ostringstream{};

  strStream << "  Input file = " << properties.inputFilename << "\n";
  strStream << "  Output file = " << outputFilename << "\n";
  strStream << "  Bounds file = " << boundsFilename << "\n";
  strStream << "  Seed = " << properties.seed << "\n";
  strStream << "  Maxgen = " << properties.maxGen << "\n";
  strStream << "  Width = " << properties.lineWidth << "\n";
  strStream << "  Delta = " << properties.turnAngle << "\n";
  strStream << "  Distance = " << properties.lineDistance << "\n";

  return strStream.str();
}

[[nodiscard]] auto GetGenerator(const Properties& properties,
                                const std::string& outputFilename,
                                const std::string& boundsFilename) -> std::unique_ptr<IGenerator>
{
  //auto generator = std::make_unique<RadianceGenerator>(outputFilename, boundsFilename);
  auto generator = std::make_unique<GenericGenerator>(outputFilename, boundsFilename);
  generator->SetName(GetBaseFilename(outputFilename));
  generator->SetHeader(GetFormattedHeader(properties, outputFilename, boundsFilename));

  return generator;
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
  cmdOpts.Add('?', "", HELP_DESCR, OptionTypes::NO_ARGS, &help1);
  cmdOpts.Add('H', "help", HELP_DESCR, OptionTypes::NO_ARGS, &help2);
  cmdOpts.Add(' ', "display", DISPLAY_DESCR, OptionTypes::NO_ARGS, &commandLineArgs.display);
  cmdOpts.Add(' ', "stats", STATS_DESCR, OptionTypes::NO_ARGS, &commandLineArgs.stats);
  cmdOpts.Add('m',
              "maxgen <int>",
              MAX_GEN_DESCR,
              OptionTypes::REQUIRED_ARG,
              &commandLineArgs.properties.maxGen);
  cmdOpts.Add('d',
              "delta <int>",
              DELTA_DESCR,
              OptionTypes::REQUIRED_ARG,
              &commandLineArgs.properties.turnAngle);
  cmdOpts.Add(' ',
              "distance <int>",
              DISTANCE_DESCR,
              OptionTypes::REQUIRED_ARG,
              &commandLineArgs.properties.lineDistance);
  cmdOpts.Add('w',
              "width <int>",
              WIDTH_DESCR,
              OptionTypes::REQUIRED_ARG,
              &commandLineArgs.properties.lineWidth);
  cmdOpts.Add(
      's', "seed <int>", SEED_DESCR, OptionTypes::REQUIRED_ARG, &commandLineArgs.properties.seed);
  cmdOpts.Add('o',
              "output <string>",
              OUTPUT_DESCR,
              OptionTypes::REQUIRED_ARG,
              &commandLineArgs.outputFilename);
  cmdOpts.Add('b',
              "bounds <string>",
              BOUNDS_DESCR,
              OptionTypes::REQUIRED_ARG,
              &commandLineArgs.boundsFilename);
  //  cmdOpts.Add(' ', "generic", noArgs, &generic);

  std::vector<std::string> positionalParams{};
  cmdOpts.SetPositional(1, 1, &positionalParams);

  if (const CommandLineOptions::OptionReturnCode retCode = cmdOpts.ProcessOptions(argc, argv);
      retCode != OptionReturnCode::OK)
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

  if (nullptr == commandLineArgs.outputFilename)
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
    std::cout << "Gen 0: " << *model.GetStartModuleList() << "\n";
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
    const auto cmdArgs = GetPropertiesFromCommandLine(argc, argv);
    if (not cmdArgs.success)
    {
      return 1;
    }

    // Initialize random number generator.
    ::srand48((cmdArgs.properties.seed != -1) ? cmdArgs.properties.seed : ::time(nullptr));
    SetRandFunc([]() { return static_cast<double>(rand()) / static_cast<double>(RAND_MAX); });

    const auto model           = GetParsedModel(cmdArgs.properties);
    const auto finalProperties = GetFinalProperties(model->GetSymbolTable(), cmdArgs.properties);

    // For each generation, apply appropriate productions in parallel to all modules.
    PrintStartInfo(*model, cmdArgs.display, cmdArgs.stats);
    auto moduleList = std::make_unique<List<Module>>(*model->GetStartModuleList());
    for (int gen = 1; gen <= finalProperties.maxGen; ++gen)
    {
      moduleList = model->Generate(moduleList.get());
      PrintGenInfo(gen, *moduleList, cmdArgs.display, cmdArgs.stats);
    }

    // Construct an output generator and apply it to the final module list.
    auto generator = GetGenerator(finalProperties, cmdArgs.outputFilename, cmdArgs.boundsFilename);
    PrintInterpretStart(generator->GetHeader());
    auto interpreter = Interpreter(*generator);
    interpreter.SetDefaults(
        finalProperties.turnAngle, finalProperties.lineWidth, finalProperties.lineDistance);
    interpreter.InterpretAllModules(*moduleList);

    return 0;
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
    return 1;
  }
}
