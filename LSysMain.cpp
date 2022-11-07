/* main.c - driver for parsing and producing L-systems.
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
 * $Log:	main.c,v $
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
#include <string>
#include <fstream>
#include <iomanip>

//#include "Debug_InitialiseFinalise.h"
#include "Standard_InitialiseFinalise.h"
#include "Utilities/CommandLineOptions.h"

#include "Value.h"
#include "Rand.h"
#include "Parser.h"
#include "Interpret.h"
#include "LSysModel.h"
#include "GenericGenerator.h"
#include "Debug.h"

using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;
using std::ofstream;
using std::ostringstream;
using Utilities::CommandLineOptions;
using LSys::LSysModel;
using LSys::Value;
using LSys::Module;
using LSys::SymbolTable;
using LSys::Generator;
using LSys::GenericGenerator;
using LSys::srand48;

#define noArgs CommandLineOptions::noArgs
#define optionalArg CommandLineOptions::optionalArg
#define requiredArg CommandLineOptions::requiredArg
#define zeroOrMoreArgs CommandLineOptions::zeroOrMoreArgs
#define oneOrMoreArgs CommandLineOptions::oneOrMoreArgs


#if YYDEBUG != 0
  extern int yydebug;			/*  nonzero means print parse trace	*/
#endif
int ParseDebug= 1;


namespace {

  void out_of_memory() {
    cerr << "FATAL: no free memory!" << endl;
    std::exit(1);
  }


  // Return a copy of a filename stripped of its trailing
  // extension (.[^.]*), if any.
  const char* BaseFilename(const char *s)
  {
    char* c= strdup(s);
    char* p= strrchr(c, '.');
    if (p != NULL) *p= '\0';
    return c;
  }


  // Concatenate two strings, returning the result in a dynamically
  // allocated buffer.
  const char* concat(const char* p, const char* q)
  {
    const int len= strlen(p) + strlen(q);
    char* s= new char[len+1];

    strcpy(s, p);
    strcat(s, q);

    return s;
  }


  ofstream* OpenOutputFile(const char* filename)
  {
    ofstream* f= new ofstream(filename);
    if (f->bad()) {
      cerr << "Error opening output file " << filename << ", aborting." << endl;
      std::exit(1);
    }
    return f;
  }


  // Set the output stream for the generated database to the specified
  // file. If the name has no extension, add one based on the output
  // format.
  ofstream* SetOutputFilename(const char* ofile)
  {
    return OpenOutputFile(ofile);
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
         << "-seed    sets seed value\n"
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
  void SetSymbolTableValues(
    SymbolTable<Value>* st,
    int maxgen, float delta, float width, float distance)
  {
    if (maxgen > 0)
      st->enter("maxgen", Value(maxgen));
    if (delta > 0)
      st->enter("delta", Value(delta));
    if (width > 0)
      st->enter("width", Value(width));
    if (distance > 0)
      st->enter("distance", Value(distance));
  }


  // Look up defaults in the symbol table, if not overridden by specified
  // arguments.
  void SetDefaults(
    const SymbolTable<Value>& st,
    int* maxgen, float* delta, float* width, float* distance)
  {
    int i;
    Value v;
    if (*maxgen < 0) {
      if (st.lookup("maxgen", v)) {
        if (v.value(i))
          *maxgen= i;
        else {
          cerr << "Invalid value specified for maxgen: " << v << endl;
          exit(1);
        }
      } else
        *maxgen= 0; // Default: just do sanity checking
    }
    if (*delta < 0) {
      if (st.lookup("delta", v)) {
        if (!v.value(*delta)) {
          cerr << "Invalid value specified for delta: " << v << endl;
          exit(1);
         }
      } else
        *delta= 90; // Default: turn at right angles
    }
    if (*width < 0) {
      if (st.lookup("width", v)) {
        if (!v.value(*width)) {
          cerr << "Invalid value specified for width: " << v << endl;
          exit(1);
        }
      } else
        *width= 1;	// Default line aspect ratio (1/100)
    }
    if (*distance < 0) {
      if (st.lookup("distance", v)) {
        if (!v.value(*distance)) {
          cerr << "Invalid value specified for distance: " << v << endl;
          exit(1);
        }
      } else
        *distance= 1;	// Default standard distance
    }
  }

};


int main(int argc, const char* argv[])
{
  Standard_InitialiseFinalise initialiseFinalise;
//  Debug_InitialiseFinalise initialiseFinalise;

  try {
    #if YYDEBUG != 0
      yydebug= 1;
    #endif
    ParseDebug= 0;

    bool help1= false;
    bool help2= false;
    int maxgen= -1;      // Default # of generations to produce
    float width= -1;     // Default line width
    float delta= -90;    // Default turn angle
    float distance= -1;  // Default line distance
    int seed= -1;        // Default seed for random numbers: -1 means use time
    bool display= false; // Display module list at each generation?
    bool stats= false;   // Compute statistics
    const char* outputFilename= 0;
    const char* boundsFilename= "bounds.txt";
    enum FileType { generic };
    FileType filetype= generic;
    string inputFilename;
    vector<string> positionalParams;

    CommandLineOptions cmdOpts;
    const string helpDescr= "displays help for this program";
    const string maxgenDescr= "sets the number of generations to produce";
    const string deltaDescr= "sets the default turn angle";
    const string distanceDescr= "sets the default line length";
    const string widthDescr= "sets the default line width";
    const string seedDescr= "sets the seed value";
    const string displayDescr= "displays the L-systems produced at each generation";
    const string statsDescr= "displays module statistics for each generation";
    const string outputDescr= "output filename";
    const string boundsDescr= "bounds filename";

    cmdOpts.Add('?', "", helpDescr, noArgs, &help1);
    cmdOpts.Add('H', "help", helpDescr, noArgs, &help2);
    cmdOpts.Add('m', "maxgen <int>", maxgenDescr, requiredArg, &maxgen);
    cmdOpts.Add('d', "delta <int>", deltaDescr, requiredArg, &delta);
    cmdOpts.Add(' ', "distance <int>", distanceDescr, requiredArg, &distance);
    cmdOpts.Add('w', "width <int>", widthDescr, requiredArg, &width);
    cmdOpts.Add('s', "seed <int>", seedDescr, requiredArg, &seed);
    cmdOpts.Add(' ', "display", displayDescr, noArgs, &display);
    cmdOpts.Add(' ', "stats", statsDescr, noArgs, &stats);
    cmdOpts.Add('o', "output <string>", outputDescr, requiredArg, &outputFilename);
    cmdOpts.Add('b', "bounds <string>", boundsDescr, requiredArg, &boundsFilename);
  //  cmdOpts.Add(' ', "generic", noArgs, &generic);

    cmdOpts.SetPositional(1, 1, &positionalParams);

    CommandLineOptions::OptionReturnCode retCode= cmdOpts.ProcessOptions(argc, argv);

    if (retCode != CommandLineOptions::ok) {
      cerr << "\n";
      cmdOpts.Usage(cerr, "input file...");
      return 1;
    }
    if (help1 || help2) {
      cmdOpts.Usage(cerr, "input file...");
      return 1;
    }
    if (outputFilename == 0) {
      cerr << "Must supply -o (output filename) option\n\n";
      cmdOpts.Usage(cerr, "input file...");
    }
    inputFilename= positionalParams[0];


    std::set_new_handler(out_of_memory);

    // Initialize random number generator
    srand48((seed != -1) ? seed : time(0));

    LSysModel* model= new LSysModel;

    SetSymbolTableValues(model->symbolTable, maxgen, delta, width, distance);

    set_parser_globals(model);
    set_parser_input(inputFilename.c_str());
    ofstream* outputF= SetOutputFilename(outputFilename);
    ofstream* outputBnds= OpenOutputFile(boundsFilename);

    yyparse(); // Parse input file

    if (model->start) {
      PDebug(PD_MAIN, cerr << "Starting module list: " << *model->start << endl);
    } else {
      PDebug(PD_MAIN, cerr << "No starting module list" << endl);
    }
//    PDebug(PD_SYMBOL, cerr << "\nSymbol Table:\n" << *model->symbolTable);
    PDebug(PD_PRODUCTION, cerr << "\nProductions:\n" << *model->rules << endl);

    if (!model->start) {
      cerr << "No starting point found!" << endl;
      std::exit(1);
    }

    SetDefaults(*model->symbolTable, &maxgen, &delta, &width, &distance);

    if (display)
      cout << "Gen0 : " << *model->start << endl;

    // For each generation, apply appropriate productions
    // in parallel to all modules.
    if (stats) cerr << endl;
    LSys::List<Module>* oldModuleList= model->start;
    for (int gen= 1; gen <= maxgen; gen++) {
      LSys::List<Module>* newModuleList= model->Generate(oldModuleList);
      if (display)
        cout << "Gen" << gen << ": " << *newModuleList << endl;
      if (stats)
        cerr << "Gen" << std::setw(3) << gen << ": # modules= "
             << std::setw(5) << newModuleList->size() << endl;
  //////////    delete oldModuleList;
      oldModuleList= newModuleList;
    }

    // Construct an output generator and apply it to the final module
    // list to build a database.
    Generator* g= 0;
    const char* type= "";
    switch (filetype) {
      case generic:
        type= "generic";
        g= new GenericGenerator(outputF, outputBnds);
        break;
      default:
        break;
    }

    ostringstream strStream;
    strStream << "  Input file = " << inputFilename << "\n";
    strStream << "  Output file = " << outputFilename << "\n";
    strStream << "  Bounds file = " << boundsFilename << "\n";
    strStream << "  Seed = " << seed << "\n";
    strStream << "  Maxgen = " << maxgen << "\n";
    strStream << "  Width = " << width << "\n";
    strStream << "  Delta = " << delta << "\n";
    strStream << "  Distance = " << distance << "\n";

    cerr << "\n";
    cerr << "Generating " << type << " database..." << "\n";
    cerr << strStream.str();
    cerr << "\n";

    g->SetName(BaseFilename(outputFilename));
    g->SetHeader(strStream.str());
    Interpret(*oldModuleList, *g, delta, width, distance);

    delete outputF;
    delete outputBnds;
    delete g;

    PDebug(PD_MAIN, cerr << "About to exit" << endl);
    return 0;

  } catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
    initialiseFinalise.CaughtException();
  }
}

