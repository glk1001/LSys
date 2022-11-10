#include "CommandLineOptions.h"

#include "Options.h"

#include <iostream>
#include <string.h>
#include <string>
#include <vector>

using std::cerr;
using std::endl;
using std::string;
using std::vector;


namespace Utilities
{

CommandLineOptionBase::CommandLineOptionBase(char _optChar,
                                             const std::string& _longOpt,
                                             const std::string& _optDescription,
                                             CommandLineOptions::OptionTypes _optType)
  : optChar(_optChar), optType(_optType), longOpt(_longOpt), optDescription(_optDescription)
{
}


CommandLineOptionBase::CommandLineOptionBase(const CommandLineOptionBase& c)
  : optChar(c.optChar), optType(c.optType), longOpt(c.longOpt), optDescription(c.optDescription)
{
}


CommandLineOptionBase* CommandLineOptionBase::Clone() const
{
  return new CommandLineOptionBase(*this);
}


const char CommandLineOptions::optTypeChar[oneOrMoreArgs + 1] = {
    '|', /* noArgs */
    '|', /* noArgsShortCircuitOption */
    '?', /* optionalArg */
    ':', /* requiredArg */
    '*', /* zeroOrMoreArgs */
    '+' /* oneOrMoreArgs */
};


CommandLineOptions::CommandLineOptions(const bool use_f_flag)
  : rawOptions{new Options}
{
  if (use_f_flag)
  {
    this->Add(CommandLineOptionBase('f', "control <string>", "", optionalArg));
  }
}

CommandLineOptions::~CommandLineOptions()
{
  delete rawOptions;
  this->FreeCmdOptions();
  this->FreeOptionDefinitions();
  this->FreeOptionDescriptions();
}


void CommandLineOptions::FreeCmdOptions()
{
  for (size_t i = 0; i < cmdOptions.size(); i++)
  {
    delete cmdOptions[i];
  }
  cmdOptions.clear();
}


void CommandLineOptions::FreeOptionDefinitions()
{
  if (optionDefinitions != nullptr)
  {
    int i = 0;
    while (true)
    {
      if (optionDefinitions[i] == 0)
      {
        break;
      }
      free(reinterpret_cast<void*>(const_cast<char*>(optionDefinitions[i])));
      ++i;
    }
    delete[] optionDefinitions;
  }
}


void CommandLineOptions::FreeOptionDescriptions()
{
  if (optionDescriptions != nullptr)
  {
    int i = 0;
    while (true)
    {
      if (optionDescriptions[i] == 0)
      {
        break;
      }
      free(reinterpret_cast<void*>(const_cast<char*>(optionDescriptions[i])));
      ++i;
    }
    delete[] optionDescriptions;
  }
}


void CommandLineOptions::Add(const CommandLineOptionBase& opt)
{
  cmdOptions.push_back(opt.Clone());
}


void CommandLineOptions::SetPositional(int minNum, int maxNum, std::vector<std::string>* params)
{
  minNumPositional = minNum;
  maxNumPositional = maxNum;
  positionalParams = params;
}


char CommandLineOptions::LastShortOption() const
{
  return lastShortOption;
}


const char* CommandLineOptions::LastLongOption() const
{
  return lastLongOption;
}


void CommandLineOptions::Usage(std::ostream& f, const char* positionalParamsDescription)
{
  rawOptions->Usage(f, positionalParamsDescription);
}


CommandLineOptions::OptionReturnCode CommandLineOptions::ProcessOptions(int argc,
                                                                        const char* argv[])
{
  positionalParams->clear();

  this->FreeOptionDefinitions();
  optionDefinitions  = new const char*[cmdOptions.size() + 1];
  optionDescriptions = new const char*[cmdOptions.size() + 1];
  for (size_t i = 0; i < cmdOptions.size(); i++)
  {
    const string optChar  = string(1, cmdOptions[i]->OptChar());
    const string optType  = string(1, OptTypeChar(cmdOptions[i]->OptType()));
    string str            = optChar + optType + cmdOptions[i]->LongOpt();
    optionDefinitions[i]  = strdup(str.c_str());
    optionDescriptions[i] = strdup(cmdOptions[i]->OptDescription().c_str());
  }
  optionDefinitions[cmdOptions.size()] = 0;

  argv++;
  argc--;
  rawOptions->SetOptions(*(argv - 1), optionDefinitions, optionDescriptions);
  OptArgvIter optIter(argc, argv);

  int npos = 0;
  const char* longOpt;
  const char* optArg;
  int optChar;
  while ((optChar = (*rawOptions)(optIter, &optArg, &longOpt)) != 0)
  {
    lastShortOption = (optArg == 0) ? '\0' : optArg[0];
    lastLongOption  = longOpt;

    switch (optChar)
    {
      case 'f':
        rawOptions->Controls(optArg);
        break;

      case Options::BadChar: // bad option ("-%c", *optArg)
        return badShortOption;

      case Options::BadKeyword: // bad long-option ("--%s", optArg)
        lastLongOption = optArg;
        return badLongOption;

      case Options::Ambiguous: // ambiguous long-option ("--%s", optArg)
        return ambiguousOption;

      case Options::ArgRequired:
        return argumentRequired;

      case Options::Positional:
        // Push positional arguments to the front
        argv[npos++] = optArg;
        break;

      case ' ':
        for (size_t i = 0; i < cmdOptions.size(); i++)
        {
          if (string(longOpt) == cmdOptions[i]->LongOpt())
          {
            cmdOptions[i]->SetValue(optArg);
            if (cmdOptions[i]->OptType() == noArgsShortCircuitOption)
              return shortCircuitOption;
          }
        }
        break;

      default:
        for (size_t i = 0; i < cmdOptions.size(); i++)
        {
          if (optChar == cmdOptions[i]->OptChar())
          {
            cmdOptions[i]->SetValue(optArg);
            if (cmdOptions[i]->OptType() == noArgsShortCircuitOption)
              return shortCircuitOption;
          }
        }
        break;
    }
  }

  const int index = optIter.Index();
  if ((npos > 0) || (index < argc))
  {
    const int first     = (npos > 0) ? 0 : index;
    const int lastPlus1 = (npos > 0) ? npos : argc;
    for (int i = first; i < lastPlus1; i++)
      positionalParams->push_back(argv[i]);
  }

  const auto numPositional = static_cast<int>(positionalParams->size());
  if (numPositional < minNumPositional)
  {
    if (!(rawOptions->Controls() & Options::Quiet))
    {
      cerr << rawOptions->Name() << ": not enough positional params. "
           << "Expecting at least " << minNumPositional << " param"
           << ((minNumPositional == 1) ? "" : "s") << "." << endl;
    }
    return notEnoughPositionParams;
  }
  if (numPositional > maxNumPositional)
  {
    if (!(rawOptions->Controls() & Options::Quiet))
    {
      cerr << rawOptions->Name() << ": too many positional params. "
           << "Expecting no more than " << maxNumPositional << " param"
           << ((maxNumPositional == 1) ? "" : "s") << "." << endl;
    }
    return tooManyPositionParams;
  }

  return ok;
}


}; // namespace Utilities
