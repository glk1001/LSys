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
#include "Parser.h"
#include "Rand.h"
#include "Value.h"
#include "debug.h"

#include <fstream>
#include <iomanip>
#include <string>

using LSys::GenericGenerator;
using LSys::IGenerator;
using LSys::LSysModel;
using LSys::Module;
using LSys::SymbolTable;
using LSys::Value;
using Utilities::CommandLineOptions;

using enum Utilities::CommandLineOptions::OptionTypes;
using enum Utilities::CommandLineOptions::OptionReturnCode;

#if YYDEBUG != 0
extern int yydebug; // nonzero means print parse trace
#endif
int ParseDebug = 1;

namespace
{

auto OutOfMemory() -> void
{
  std::cerr << "FATAL: no free memory!\n";
  std::exit(1);
}

// Return a copy of a filename stripped of its trailing
// extension (.[^.]*), if any.
[[nodiscard]] auto BaseFilename(const char* const filename) -> const char*
{
  char* const filenameCopy = ::strdup(filename);
  if (char* const ptr = ::strrchr(filenameCopy, '.'); ptr != nullptr)
  {
    *ptr = '\0';
  }
  return filenameCopy;
}

[[nodiscard]] auto OpenOutputFile(const char* const filename) -> std::ofstream*
{
  auto* const out = new std::ofstream(filename);
  if (out->bad())
  {
    std::cerr << "Error opening output file " << filename << ", aborting.\n";
    std::exit(1);
  }
  return out;
}

// Set the output stream for the generated database to the specified
// file. If the name has no extension, add one based on the output
// format.
[[nodiscard]] auto SetOutputFilename(const char* const filename) -> std::ofstream*
{
  return OpenOutputFile(filename);
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

// Set override values for symbol table.
auto SetSymbolTableValues(SymbolTable<Value>* const symbolTable,
                          const int maxGen,
                          const float delta,
                          const float width,
                          const float distance) -> void
{
  if (maxGen > 0)
  {
    symbolTable->Enter("maxgen", Value(maxGen));
  }
  if (delta > 0.0F)
  {
    symbolTable->Enter("delta", Value(delta));
  }
  if (width > 0.0F)
  {
    symbolTable->Enter("width", Value(width));
  }
  if (distance > 0.0F)
  {
    symbolTable->Enter("distance", Value(distance));
  }
}

// Look up defaults in the symbol table, if not overridden by specified
// arguments.
auto SetDefaults(const SymbolTable<Value>& symbolTable,
                 int* const maxGen,
                 float* const delta,
                 float* const width,
                 float* const distance) -> void
{
  if (*maxGen < 0)
  {
    if (Value value; not symbolTable.Lookup("maxgen", value))
    {
      *maxGen = 0; // Default: just do sanity checking
    }
    else
    {
      if (auto i = 0; value.GetIntValue(i))
      {
        *maxGen = i;
      }
      else
      {
        std::cerr << "Invalid GetFloatValue specified for maxgen: " << value << "\n";
        ::exit(1);
      }
    }
  }

  if (*delta < 0.0F)
  {
    if (Value value; not symbolTable.Lookup("delta", value))
    {
      *delta = 90.0F; // Default: turn at right angles
    }
    else
    {
      if (!value.GetFloatValue(*delta))
      {
        std::cerr << "Invalid GetFloatValue specified for delta: " << value << "\n";
        ::exit(1);
      }
    }
  }

  if (*width < 0.0F)
  {
    if (Value value; not symbolTable.Lookup("width", value))
    {
      *width = 1.0F; // Default line aspect ratio (1/100)
    }
    else
    {
      if (!value.GetFloatValue(*width))
      {
        std::cerr << "Invalid GetFloatValue specified for width: " << value << "\n";
        ::exit(1);
      }
    }
  }

  if (*distance < 0.0F)
  {
    if (Value value; not symbolTable.Lookup("distance", value))
    {
      *distance = 1.0F; // Default standard distance
    }
    else
    {
      if (!value.GetFloatValue(*distance))
      {
        std::cerr << "Invalid GetFloatValue specified for distance: " << value << "\n";
        ::exit(1);
      }
    }
  }
}

} // namespace


int main(int argc, const char* argv[])
{
  try
  {
#if YYDEBUG != 0
    //      yydebug= 1;
#endif
    ::ParseDebug = 0;

    auto help1                 = false;
    auto help2                 = false;
    auto maxGen                = -1; // Default # of generations to produce
    auto width                 = -1.0F; // Default line width
    auto delta                 = -90.0F; // Default turn angle
    auto distance              = -1.0F; // Default line distance
    auto seed                  = -1L; // Default seed for random numbers: -1 means use time
    auto display               = false; // Display module list at each generation?
    auto stats                 = false; // Compute statistics
    const auto* outputFilename = "";
    const auto* boundsFilename = "bounds.txt";

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

    CommandLineOptions cmdOpts{};
    cmdOpts.Add('?', "", HELP_DESCR, NO_ARGS, &help1);
    cmdOpts.Add('H', "help", HELP_DESCR, NO_ARGS, &help2);
    cmdOpts.Add('m', "maxgen <int>", MAX_GEN_DESCR, REQUIRED_ARG, &maxGen);
    cmdOpts.Add('d', "delta <int>", DELTA_DESCR, REQUIRED_ARG, &delta);
    cmdOpts.Add(' ', "distance <int>", DISTANCE_DESCR, REQUIRED_ARG, &distance);
    cmdOpts.Add('w', "width <int>", WIDTH_DESCR, REQUIRED_ARG, &width);
    cmdOpts.Add('s', "seed <int>", SEED_DESCR, REQUIRED_ARG, &seed);
    cmdOpts.Add(' ', "display", DISPLAY_DESCR, NO_ARGS, &display);
    cmdOpts.Add(' ', "stats", STATS_DESCR, NO_ARGS, &stats);
    cmdOpts.Add('o', "output <string>", OUTPUT_DESCR, REQUIRED_ARG, &outputFilename);
    cmdOpts.Add('b', "bounds <string>", BOUNDS_DESCR, REQUIRED_ARG, &boundsFilename);
    //  cmdOpts.Add(' ', "generic", noArgs, &generic);

    std::vector<std::string> positionalParams{};
    cmdOpts.SetPositional(1, 1, &positionalParams);

    if (const CommandLineOptions::OptionReturnCode retCode = cmdOpts.ProcessOptions(argc, argv);
        retCode != OK)
    {
      std::cerr << "\n";
      cmdOpts.Usage(std::cerr, "input file...");
      return 1;
    }
    if (help1 or help2)
    {
      cmdOpts.Usage(std::cerr, "input file...");
      return 1;
    }
    if (nullptr == outputFilename)
    {
      std::cerr << "Must supply -o (output filename) option\n\n";
      cmdOpts.Usage(std::cerr, "input file...");
      return 1;
    }
    const auto inputFilename = positionalParams[0];

    std::set_new_handler(OutOfMemory);

    // Initialize random number generator
    ::srand48((seed != -1) ? seed : ::time(nullptr));

    auto* model = new LSysModel;

    SetSymbolTableValues(model->symbolTable, maxGen, delta, width, distance);

    ::set_parser_globals(model);
    ::set_parser_input(inputFilename.c_str());
    std::ofstream* outputF    = SetOutputFilename(outputFilename);
    std::ofstream* outputBnds = OpenOutputFile(boundsFilename);

    //      yydebug = 1;
    ::yyparse(); // Parse input file

    if (model->start != nullptr)
    {
      PDebug(PD_MAIN, std::cerr << "Starting module list: " << *model->start << "\n");
    }
    else
    {
      PDebug(PD_MAIN, std::cerr << "No starting module list\n");
    }
    //    PDebug(PD_SYMBOL, cerr << "\nSymbol Table:\n" << *model->symbolTable);
    PDebug(PD_PRODUCTION, std::cerr << "\nProductions:\n" << *model->rules << "\n");

    if (model->start == nullptr)
    {
      std::cerr << "No starting point found!\n";
      std::exit(1);
    }

    SetDefaults(*model->symbolTable, &maxGen, &delta, &width, &distance);

    if (display)
    {
      std::cout << "Gen0 : " << *model->start << "\n";
    }

    // For each generation, apply appropriate productions
    // in parallel to all modules.
    if (stats)
    {
      std::cerr << "\n";
    }
    LSys::List<Module>* oldModuleList = model->start;
    for (int gen = 1; gen <= maxGen; ++gen)
    {
      LSys::List<Module>* newModuleList = model->Generate(oldModuleList);
      if (display)
      {
        std::cout << "Gen" << gen << ": " << *newModuleList << "\n";
      }
      if (stats)
      {
        std::cerr << "Gen" << std::setw(3) << gen << ": # modules= " << std::setw(5)
                  << newModuleList->size() << "\n";
      }
      //////////    delete oldModuleList;
      oldModuleList = newModuleList;
    }

    // Construct an output generator and apply it to the final module list
    // to build a database.
    enum class FileType
    {
      GENERIC
    };
    const auto filetype   = FileType::GENERIC;
    IGenerator* generator = nullptr;
    const char* type      = "";
    switch (filetype)
    {
      case FileType::GENERIC:
        type      = "generic";
        generator = new GenericGenerator(outputF, outputBnds);
        break;
      default:
        break;
    }

    auto strStream = std::ostringstream{};
    strStream << "  Input file = " << inputFilename << "\n";
    strStream << "  Output file = " << outputFilename << "\n";
    strStream << "  Bounds file = " << boundsFilename << "\n";
    strStream << "  Seed = " << seed << "\n";
    strStream << "  Maxgen = " << maxGen << "\n";
    strStream << "  Width = " << width << "\n";
    strStream << "  Delta = " << delta << "\n";
    strStream << "  Distance = " << distance << "\n";

    std::cerr << "\n";
    std::cerr << "Generating " << type << " database..."
              << "\n";
    std::cerr << strStream.str();
    std::cerr << "\n";

    generator->SetName(BaseFilename(outputFilename));
    generator->SetHeader(strStream.str());
    Interpret(*oldModuleList, *generator, delta, width, distance);

    delete outputF;
    delete outputBnds;
    delete generator;

    PDebug(PD_MAIN, std::cerr << "About to exit\n");
    return 0;
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
}
