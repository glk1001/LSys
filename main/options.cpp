// ****************************************************************************
// ^FILE: options.c - implement the functions defined in <options.h>
//
// ^HISTORY:
//    01/16/92	Brad Appleton	<bradapp@enteract.com>	Created
//
//    03/23/93	Brad Appleton	<bradapp@enteract.com>
//    - Added OptIstreamIter class
//
//    10/08/93	Brad Appleton	<bradapp@enteract.com>
//    - Added "hidden" options
//
//    02/08/94	Brad Appleton	<bradapp@enteract.com>
//    - Added "OptionSpec" class
//    - Permitted use of stdio instead of iostreams via #ifdef USE_STDIO
//
//    03/08/94	Brad Appleton	<bradapp@enteract.com>
//    - completed support for USE_STDIO
//    - added #ifdef NO_USAGE for people who always want to print their own
//    - Fixed stupid 0 pointer error in OptionsSpec class
//
//    07/31/97	Brad Appleton	<bradapp@enteract.com>
//    - Added ParsePos control flag and POSITIONAL return value.
// ^^**************************************************************************

#include "options.h"

#include <cstdlib>
#include <ctype.h>
#include <iostream>
#include <string.h>
#include <string>

using std::cerr;
using std::endl;
using std::istream;
using std::ostream;

namespace Utilities
{

OptIter::~OptIter() = default;

// I need a portable version of "tolower" that does NOT modify
// non-uppercase characters.
//
#define TOLOWER(c) (isupper(c) ? tolower(c) : c)


// **************************************************************** OptionSpec

// Class that represents an option-specification
//    *NOTE*:: Assumes that the char-ptr given to the constructor points
//             to storage that will NOT be modified and whose lifetime will
//             be as least as long as the OptionSpec object we construct.
//

class OptionSpec
{
public:
  OptionSpec(const char* decl, const char* _description)
    : hidden(0), spec(decl), description(_description)
  {
    if (spec == nullptr)
      spec = Null_spec;
    CheckHidden();
  }

  OptionSpec(const OptionSpec& cp) : hidden(cp.hidden), spec(cp.spec) {}

  // NOTE: use default destructor!

  OptionSpec& operator=(const OptionSpec& cp)
  {
    if (this != &cp)
    {
      spec        = cp.spec;
      description = cp.description;
      hidden      = cp.hidden;
    }
    return *this;
  }

  /**
      OptionSpec& operator=(const char * decl)
      {
        if (spec != decl) {
          spec= decl;
          hidden= 0;
          CheckHidden();
        }
        return *this;
      }
      **/

  // Convert to char-ptr by returning the original declaration-string
  operator const char*() { return isHiddenOpt() ? (spec - 1) : spec; }

  // Is this option 0?
  int isNull() const { return ((spec == nullptr) || (spec == Null_spec)); }

  // Is this options incorrectly specified?
  int isSyntaxError(const char* Name) const;

  // See if this is a Hidden option
  int isHiddenOpt() const { return hidden; }

  // Get the corresponding option-character
  char OptChar() const { return *spec; }

  // Get the corresponding long-option string
  const char* LongOpt() const
  {
    return (spec[1] && spec[2] && (!isspace(spec[2]))) ? (spec + 2) : nullptr;
  }

  // Get the corresponding description string
  const char* Description() const { return description; }

  // Does this option require an argument?
  int isValRequired() const { return ((spec[1] == ':') || (spec[1] == '+')); }

  // Does this option take an optional argument?
  int isValOptional() const { return ((spec[1] == '?') || (spec[1] == '*')); }

  // Does this option take no arguments?
  int isNoArg() const { return ((spec[1] == '|') || (!spec[1])); }

  // Can this option take more than one argument?
  int isList() const { return ((spec[1] == '+') || (spec[1] == '*')); }

  // Does this option take any arguments?
  int isValTaken() const { return (isValRequired() || isValOptional()); }

  // Format this option in the given buffer
  unsigned Format(char* buf, unsigned optctrls) const;

private:
  void CheckHidden()
  {
    if ((!hidden) && (*spec == '-'))
    {
      ++hidden;
      ++spec;
    }
  }
  unsigned hidden : 1; // hidden-flag
  const char* spec; // string specification
  const char* description;
  static const char Null_spec[];
};


namespace
{
// Options class utilities

// Is this option-char null?
inline int isNullOpt(char optchar)
{
  return ((!optchar) || isspace(optchar) || (!isprint(optchar)));
}


// Check for explicit "end-of-options"
inline int isEndOpts(const char* token)
{
  return ((token == nullptr) || (!::strcmp(token, "--")));
}


// See if an argument is an option
inline int isOption(unsigned flags, const char* arg)
{
  return (((*arg != '\0') || (arg[1] != '\0')) &&
          ((*arg == '-') || ((flags & Options::Plus) && (*arg == '+'))));
}


// See if we should be parsing only options or if we also need to
// parse positional arguments
inline int isOptsOnly(unsigned flags)
{
  return (flags & Options::ParsePos) ? 0 : 1;
}


// return values for a keyword matching function
enum kwdmatch_t
{
  NO_MATCH,
  PARTIAL_MATCH,
  EXACT_MATCH
};

// ---------------------------------------------------------------------------
// ^FUNCTION: kwdmatch - match a keyword
//
// ^SYNOPSIS:
//    static kwdmatch_t kwdmatch(src, attempt, len)
//
// ^PARAMETERS:
//    char * src -- the actual keyword to match
//    char * attempt -- the possible keyword to compare against "src"
//    int len -- number of character of "attempt" to consider
//               (if 0 then we should use all of "attempt")
//
// ^DESCRIPTION:
//    See if "attempt" matches some prefix of "src" (case insensitive).
//
// ^REQUIREMENTS:
//    - attempt should be non-0 and non-empty
//
// ^SIDE-EFFECTS:
//    None.
//
// ^RETURN-VALUE:
//    An enumeration value of type kwdmatch_t corresponding to whether
//    We had an exact match, a partial match, or no match.
//
// ^ALGORITHM:
//    Trivial
// ^^-------------------------------------------------------------------------
kwdmatch_t kwdmatch(const char* src, const char* attempt, int len = 0);

} // namespace


// ******************************************************************* Options

#if (defined(MSWIN) || defined(OS2) || defined(MSDOS))
#define pathDelimiter '\\'
#else
#define pathDelimiter '/'
#endif

Options::Options(const char* name, const char* const optv[], const char* const optDescriptions[])
  : explicitEnd(0),
    optctrls(Default),
    optvec(optv),
    optDesc(optDescriptions),
    nextchar(nullptr),
    listopt(nullptr),
    cmdname(name)
{
  const char* basename = ::strrchr(cmdname, pathDelimiter);
  if (basename != nullptr)
    cmdname = basename + 1;
  this->CheckSyntax();
}

Options::~Options() = default;

void Options::SetOptions(const char* name,
                         const char* const optv[],
                         const char* const optDescriptions[])
{
  cmdname              = name;
  optvec               = optv;
  optDesc              = optDescriptions;
  const char* basename = ::strrchr(cmdname, pathDelimiter);
  if (basename != nullptr)
    cmdname = basename + 1;
  this->CheckSyntax();
}

// Make sure each option-specifier has correct syntax.
//
// If there is even one invalid specifier, then exit ungracefully!
//
void Options::CheckSyntax() const
{
  int errors = 0;
  if ((optvec == nullptr) || (!*optvec))
    return;

  int i = 0;
  while (optvec[i] != nullptr)
  {
    OptionSpec optspec(optvec[i], optDesc[i]);
    i++;
    errors += optspec.isSyntaxError(cmdname);
  }
  if (errors)
    std::exit(127);
}


void Options::Controls(const char* flagsStr)
{
  if ((flagsStr != nullptr) && (*flagsStr != '\0'))
  {
    unsigned flags = this->Controls();
    for (const char* p = flagsStr; (*p != '\0'); p++)
    {
      switch (*p)
      {
        case '+':
          flags |= Options::Plus;
          break;
        case 'A':
        case 'a':
          flags |= Options::AnyCase;
          break;
        case 'L':
        case 'l':
          flags |= Options::LongOnly;
          break;
        case 'S':
        case 's':
          flags |= Options::ShortOnly;
          break;
        case 'Q':
        case 'q':
          flags |= Options::Quiet;
          break;
        case 'N':
        case 'n':
          flags |= Options::NoGuessing;
          break;
        case 'P':
        case 'p':
          flags |= Options::ParsePos;
          break;
        default:
          break;
      }
    }
    this->Controls(flags);
  }
}


// ---------------------------------------------------------------------------
// ^FUNCTION: Options::MatchOpt - match an option
//
// ^SYNOPSIS:
//    const char * MatchOpt(opt, int  ignore_case) const
//
// ^PARAMETERS:
//    char opt -- the option-character to match
//    int  ignore_case -- should we ignore character-case?
//
// ^DESCRIPTION:
//    See if "opt" is found in "optvec"
//
// ^REQUIREMENTS:
//    - optvec should be non-0 and terminated by a 0 pointer.
//
// ^SIDE-EFFECTS:
//    None.
//
// ^RETURN-VALUE:
//    0 if no match is found,
//    otherwise a pointer to the matching option-spec.
//
// ^ALGORITHM:
//    foreach option-spec
//       - see if "opt" is a match, if so return option-spec
//    end-for
// ^^-------------------------------------------------------------------------
OptionSpec Options::MatchOpt(char opt, int ignore_case) const
{
  if ((optvec == nullptr) || (!*optvec))
    return OptionSpec("", "");

  int i = 0;
  while (optvec[i] != nullptr)
  {
    OptionSpec optspec(optvec[i], optDesc[i]);
    i++;
    char optchar = optspec.OptChar();
    if (isNullOpt(optchar))
      continue;
    if (opt == optchar)
    {
      return optspec;
    }
    else if (ignore_case && (TOLOWER(opt) == TOLOWER(optchar)))
    {
      return optspec;
    }
  }

  return OptionSpec("", ""); // not found
}


// ---------------------------------------------------------------------------
// ^FUNCTION: Options::MatchLongOpt - match a long-option
//
// ^SYNOPSIS:
//   const char * Options::MatchLongOpt(opt, len, ambiguous)
//
// ^PARAMETERS:
//    char * opt -- the long-option to match
//    int len -- the number of character of "opt" to match
//    int & ambiguous -- set by this routine before returning.
//
// ^DESCRIPTION:
//    Try to match "opt" against some unique prefix of a long-option
//    (case insensitive).
//
// ^REQUIREMENTS:
//    - optvec should be non-0 and terminated by a 0 pointer.
//
// ^SIDE-EFFECTS:
//    - *ambiguous is set to '1' if "opt" matches >1 long-option
//      (otherwise it is set to 0).
//
// ^RETURN-VALUE:
//    0 if no match is found,
//    otherwise a pointer to the matching option-spec.
//
// ^ALGORITHM:
//    ambiguous is FALSE
//    foreach option-spec
//       if we have an EXACT-MATCH, return the option-spec
//       if we have a partial-match then
//          if we already had a previous partial match then
//             set ambiguous= TRUE and return 0
//          else
//             remember this options spec and continue matching
//          end-if
//       end-if
//    end-for
//    if we had exactly 1 partial match return it, else return 0
// ^^-------------------------------------------------------------------------
OptionSpec Options::MatchLongOpt(const char* opt, int len, int& ambiguous) const
{
  kwdmatch_t result;
  OptionSpec matched("", "");

  ambiguous = 0;
  if ((optvec == nullptr) || (!*optvec))
    return OptionSpec("", "");

  int i = 0;
  while (optvec[i] != nullptr)
  {
    OptionSpec optspec(optvec[i], optDesc[i]);
    i++;
    const char* longopt = optspec.LongOpt();
    if (longopt == nullptr)
      continue;
    result = kwdmatch(longopt, opt, len);
    if (result == EXACT_MATCH)
    {
      return optspec;
    }
    else if (result == PARTIAL_MATCH)
    {
      if (matched.isNull())
      {
        matched = optspec;
      }
      else
      {
        ++ambiguous;
        return OptionSpec("", ""); // not found
      }
    }
  }

  return matched;
}


// ---------------------------------------------------------------------------
// ^FUNCTION: Options::ParseOpt - parse an option
//
// ^SYNOPSIS:
//    int Options::ParseOpt(iter, optarg)
//
// ^PARAMETERS:
//    OptIter & iter -- option iterator
//    const char * & optarg -- where to store any option-argument
//
// ^DESCRIPTION:
//    Parse the Next option in iter (advancing as necessary).
//    Make sure we update the nextchar pointer along the way. Any option
//    we find should be returned and optarg should point to its argument.
//
// ^REQUIREMENTS:
//    - nextchar must point to the prospective option character
//
// ^SIDE-EFFECTS:
//    - iter is advanced when an argument completely parsed
//    - optarg is modified to point to any option argument
//    - if Options::Quiet is not set, error messages are printed on cerr
//
// ^RETURN-VALUE:
//    'c' if the -c option was matched (optarg points to its argument)
//    BADCHAR if the option is invalid (optarg points to the bad
//                                                   option-character).
//
// ^ALGORITHM:
//    It gets complicated -- follow the comments in the source.
// ^^-------------------------------------------------------------------------
int Options::ParseOpt(OptIter& iter, const char** optarg, const char** longOpt)
{
  listopt = nullptr; // Reset the list pointer

  if ((optvec == nullptr) || (!*optvec))
    return Options::EndOpts;

  // Try to match a known option
  OptionSpec optspec = MatchOpt(*(nextchar++), (optctrls & Options::AnyCase));

  // Check for an unknown option
  if (optspec.isNull())
  {
    // See if this was a long-option in disguise
    if (!(optctrls & Options::NoGuessing))
    {
      unsigned save_ctrls       = optctrls;
      const char* save_nextchar = nextchar;
      nextchar -= 1;
      optctrls |= (Options::Quiet | Options::NoGuessing);
      int optchar = ParseLongOpt(iter, optarg, longOpt);
      optctrls    = save_ctrls;
      if (optchar > 0)
      {
        return optchar;
      }
      else
      {
        nextchar = save_nextchar;
      }
    }
    if (!(optctrls & Options::Quiet))
    {
      cerr << cmdname << ": unknown option -" << *(nextchar - 1) << "." << endl;
    }
    *optarg = (nextchar - 1); // record the bad option in optarg
    return Options::BadChar;
  }

  // If no argument is taken, then leave now
  if (optspec.isNoArg())
  {
    *optarg = nullptr;
    return optspec.OptChar();
  }

  // Check for argument in this arg
  if (*nextchar)
  {
    *optarg  = nextchar; // the argument is right here
    nextchar = nullptr; // we've exhausted this arg
    if (optspec.isList())
      listopt = optspec; // save the list-spec
    return optspec.OptChar();
  }

  // Check for argument in Next arg
  const char* nextarg = iter.Current();
  if ((nextarg != nullptr) && (optspec.isValRequired() || (!isOption(optctrls, nextarg))))
  {
    *optarg = nextarg; // the argument is here
    iter.Next(); // end of arg - advance
    if (optspec.isList())
      listopt = optspec; // save the list-spec
    return optspec.OptChar();
  }

  // No argument given - if its required, that's an error
  if (optspec.isValRequired() && !(optctrls & Options::Quiet))
  {
    cerr << cmdname << ": argument required for -" << optspec.OptChar() << " option." << endl;
    *optarg = optspec;
    return Options::ArgRequired;
  }

  return optspec.OptChar();
}


// ---------------------------------------------------------------------------
// ^FUNCTION: Options::ParseLongOpt - parse a long-option
//
// ^SYNOPSIS:
//    int Options::ParseLongOpt(iter, optarg)
//
// ^PARAMETERS:
//    OptIter & iter -- option iterator
//    const char * & optarg -- where to store any option-argument
//
// ^DESCRIPTION:
//    Parse the Next long-option in iter (advancing as necessary).
//    Make sure we update the nextchar pointer along the way. Any option
//    we find should be returned and optarg should point to its argument.
//
// ^REQUIREMENTS:
//    - nextchar must point to the prospective option character
//
// ^SIDE-EFFECTS:
//    - iter is advanced when an argument completely parsed
//    - optarg is modified to point to any option argument
//    - if Options::Quiet is not set, error messages are printed on cerr
//
// ^RETURN-VALUE:
//    'c' if the the long-option corresponding to the -c option was matched
//         (optarg points to its argument)
//    BADKWD if the option is invalid (optarg points to the bad long-option
//                                                                     Name).
//
// ^ALGORITHM:
//    It gets complicated -- follow the comments in the source.
// ^^-------------------------------------------------------------------------
int Options::ParseLongOpt(OptIter& iter, const char** optarg, const char** longOpt)
{
  int len = 0, ambiguous = 0;

  listopt = nullptr; // Reset the list-spec

  if ((optvec == nullptr) || (!*optvec))
    return Options::EndOpts;

  // if a value is supplied in this argv element, get it now
  const char* val = strpbrk(nextchar, ":=");
  if (val)
  {
    len = static_cast<int>(val - nextchar);
    ++val;
  }

  // Try to match a known long-option
  OptionSpec optspec = MatchLongOpt(nextchar, len, ambiguous);

  // Check for an unknown long-option
  if (optspec.isNull())
  {
    // See if this was a short-option in disguise
    if ((!ambiguous) && (!(optctrls & Options::NoGuessing)))
    {
      unsigned save_ctrls       = optctrls;
      const char* save_nextchar = nextchar;
      optctrls |= (Options::Quiet | Options::NoGuessing);
      int optchar = ParseOpt(iter, optarg, longOpt);
      optctrls    = save_ctrls;
      if (optchar > 0)
      {
        return optchar;
      }
      else
      {
        nextchar = save_nextchar;
      }
    }
    if (!(optctrls & Options::Quiet))
    {
      cerr << cmdname << ": " << ((ambiguous) ? "ambiguous" : "unknown") << " option "
           << ((optctrls & Options::LongOnly) ? "-" : "--") << nextchar << "." << endl;
    }
    *optarg  = nextchar; // record the bad option in optarg
    nextchar = nullptr; // we've exhausted this argument
    return (ambiguous) ? Options::Ambiguous : Options::BadKeyword;
  }

  *longOpt = optspec.LongOpt();

  // If no argument is taken, then leave now
  if (optspec.isNoArg())
  {
    if ((val) && !(optctrls & Options::Quiet))
    {
      cerr << cmdname << ": option " << ((optctrls & Options::LongOnly) ? "-" : "--")
           << optspec.LongOpt() << " does NOT take an argument." << endl;
    }
    *optarg  = val; // record the unexpected argument
    nextchar = nullptr; // we've exhausted this argument
    return optspec.OptChar();
  }

  // Check for argument in this arg
  if (val)
  {
    *optarg  = val; // the argument is right here
    nextchar = nullptr; // we exhausted the rest of this arg
    if (optspec.isList())
      listopt = optspec; // save the list-spec
    return optspec.OptChar();
  }

  // Check for argument in Next arg
  const char* nextarg = iter.Current(); // find the Next argument to parse
  if ((nextarg != nullptr) && (optspec.isValRequired() || (!isOption(optctrls, nextarg))))
  {
    *optarg = nextarg; // the argument is right here
    iter.Next(); // end of arg - advance
    nextchar = nullptr; // we exhausted the rest of this arg
    if (optspec.isList())
      listopt = optspec; // save the list-spec
    return optspec.OptChar();
  }

  // No argument given - if it's required, that's an error
  optarg = nullptr;
  if (optspec.isValRequired() && !(optctrls & Options::Quiet))
  {
    const auto* spc = ::strchr(*longOpt, ' ');
    const auto longopt_len =
        (spc != nullptr) ? static_cast<int>(spc - *longOpt) : static_cast<int>(::strlen(*longOpt));
    cerr << cmdname << ": argument required for " << ((optctrls & Options::LongOnly) ? "-" : "--");
    cerr.write(*longOpt, longopt_len) << " option." << endl;
  }
  nextchar = nullptr; // we exhausted the rest of this arg
  return optspec.OptChar();
}


// ---------------------------------------------------------------------------
// ^FUNCTION: Options::Usage - print Usage
//
// ^SYNOPSIS:
//    void Options::Usage(os, positionals)
//
// ^PARAMETERS:
//    ostream & os -- where to print the Usage
//    char * positionals -- command-line syntax for any positional args
//
// ^DESCRIPTION:
//    Print command-Usage (using either option or long-option syntax) on os.
//
// ^REQUIREMENTS:
//    os should correspond to an open output file.
//
// ^SIDE-EFFECTS:
//    Prints on os
//
// ^RETURN-VALUE:
//    None.
//
// ^ALGORITHM:
//    Print Usage on os, wrapping long lines where necessary.
// ^^-------------------------------------------------------------------------
void Options::Usage(ostream& os, const char* positionals) const
{
#ifdef NO_USAGE
  return;
#else
  const char* const* optv = optvec;
  if ((optv == nullptr) || (!*optv))
    return;

  // print first portion "Usage: progname"
  os << "Usage: " << cmdname << endl;

  // print the options and the positional arguments
  int i = 0;
  while (optvec[i] != nullptr)
  {
    OptionSpec optspec(optvec[i], optDesc[i]);
    i++;
    char buf[512];
    if (optspec.isHiddenOpt())
      continue;
    optspec.Format(buf, optctrls);
    os << "  " << buf << endl;
  }
  if (positionals != nullptr)
    os << "  " << positionals << endl;
#endif
}


// ---------------------------------------------------------------------------
// ^FUNCTION: Options::operator() - get options from the command-line
//
// ^SYNOPSIS:
//   int Options::operator()(iter, optarg)
//
// ^PARAMETERS:
//    OptIter & iter -- option iterator
//    const char * & optarg -- where to store any option-argument
//
// ^DESCRIPTION:
//    Parse the Next option in iter (advancing as necessary).
//    Make sure we update the nextchar pointer along the way. Any option
//    we find should be returned and optarg should point to its argument.
//
// ^REQUIREMENTS:
//    None.
//
// ^SIDE-EFFECTS:
//    - iter is advanced when an argument is completely parsed
//    - optarg is modified to point to any option argument
//    - if Options::Quiet is not set, error messages are printed on cerr
//
// ^RETURN-VALUE:
//     0 if all options have been parsed.
//    'c' if the the option or long-option corresponding to the -c option was
//         matched (optarg points to its argument).
//    BADCHAR if the option is invalid (optarg points to the bad option char).
//    BADKWD if the option is invalid (optarg points to the bad long-opt Name).
//    AMBIGUOUS if an ambiguous keyword name was given (optarg points to the
//         ambiguous keyword name).
//    POSITIONAL if ParsePos was set and the current argument is a positional
//         parameter (in which case optarg points to the positional argument).
//
// ^ALGORITHM:
//    It gets complicated -- follow the comments in the source.
// ^^-------------------------------------------------------------------------
int Options::operator()(OptIter& iter, const char** optarg, const char** _longOpt)
{
  const char* longOpt   = nullptr;
  const char** pLongOpt = (_longOpt != nullptr) ? _longOpt : &longOpt;

  const int parse_opts_only = isOptsOnly(optctrls);
  if (parse_opts_only)
    explicitEnd = 0;

  // See if we have an option left over from before ...
  if ((nextchar) && *nextchar)
  {
    return this->ParseOpt(iter, optarg, pLongOpt);
  }

  // Check for end-of-options ...
  const char* arg  = nullptr;
  int get_next_arg = 0;
  do
  {
    arg          = iter.Current();
    get_next_arg = 0;
    if (arg == nullptr)
    {
      listopt = nullptr;
      return Options::EndOpts;
    }
    else if ((!explicitEnd) && isEndOpts(arg))
    {
      iter.Next(); // advance past end-of-options arg
      listopt     = nullptr;
      explicitEnd = 1;
      if (parse_opts_only)
        return Options::EndOpts;
      get_next_arg = 1; // make sure we look at the Next argument.
    }
  } while (get_next_arg);

  // Do we have a positional arg?
  if (explicitEnd || ((!isOption(optctrls, arg) && listopt == nullptr)))
  {
    if (parse_opts_only)
    {
      return Options::EndOpts;
    }
    else
    {
      *optarg = arg; // set optarg to the positional argument
      iter.Next(); // advance iterator to the Next argument
      return Options::Positional;
    }
  }

  iter.Next(); // pass the argument that arg already points to

  // See if we have a long option ...
  if (!(optctrls & Options::ShortOnly))
  {
    if ((*arg == '-') && (arg[1] == '-'))
    {
      nextchar = arg + 2;
      return ParseLongOpt(iter, optarg, pLongOpt);
    }
    else if ((optctrls & Options::Plus) && (*arg == '+'))
    {
      nextchar = arg + 1;
      return ParseLongOpt(iter, optarg, pLongOpt);
    }
  }
  if (*arg == '-')
  {
    nextchar = arg + 1;
    if (!(optctrls & Options::LongOnly))
    {
      return ParseOpt(iter, optarg, pLongOpt);
    }
    else
    {
      if (*nextchar != '-')
        return Options::BadKeyword;
      return ParseLongOpt(iter, optarg, pLongOpt);
    }
  }

  // If we get here - it is because we have a list value
  OptionSpec optspec(listopt, "");
  *optarg = arg; // record the list value
  return optspec.OptChar();
}


namespace
{

kwdmatch_t kwdmatch(const char* src, const char* attempt, int len)
{
  int i;

  if (src == attempt)
    return EXACT_MATCH;
  if ((src == nullptr) || (attempt == nullptr))
    return NO_MATCH;
  if ((!*src) && (!*attempt))
    return EXACT_MATCH;
  if ((!*src) || (!*attempt))
    return NO_MATCH;

  for (i = 0; ((i < len) || (len == 0)) && (attempt[i]) && (attempt[i] != ' '); i++)
  {
    if (TOLOWER(src[i]) != TOLOWER(attempt[i]))
      return NO_MATCH;
  }

  return (src[i]) ? PARTIAL_MATCH : EXACT_MATCH;
}

} // namespace


const char OptionSpec::Null_spec[] = "\0\0\0";

int OptionSpec::isSyntaxError(const char* Name) const
{
  int error = 0;
  if ((!spec) || (!*spec))
  {
    cerr << Name << ": Empty option specifier." << endl;
    cerr << "\tmust be at least 1 character long." << endl;
    ++error;
  }
  else if (spec[1] && (strchr("|?:*+", spec[1]) == nullptr))
  {
    cerr << Name << ": bad option specifier \"" << spec << "\"." << endl;
    cerr << "\t2nd character must be in the set \"|?:*+\"." << endl;
    ++error;
  }
  return error;
}


// ---------------------------------------------------------------------------
// ^FUNCTION: OptionSpec::Format - format an option-spec for a usage message
//
// ^SYNOPSIS:
//    unsigned OptionSpec::Format(buf, optctrls) const
//
// ^PARAMETERS:
//    char * buf -- where to print the formatted option
//    unsigned  optctrls -- option-parsing configuration flags
//
// ^DESCRIPTION:
//    Self-explanatory.
//
// ^REQUIREMENTS:
//    - buf must be large enough to hold the result
//
// ^SIDE-EFFECTS:
//    - writes to buf.
//
// ^RETURN-VALUE:
//    Number of characters written to buf.
//
// ^ALGORITHM:
//    Follow along in the source - it's not hard but it is tedious!
// ^^-------------------------------------------------------------------------
unsigned OptionSpec::Format(char* buf, unsigned optctrls) const
{
#ifdef NO_USAGE
  return (*buf = '\0');
#else
  static char default_value[] = "<GetFloatValue>";
  if (isHiddenOpt())
  {
    *buf = '\0';
    return static_cast<unsigned int>(*buf);
  }
  char optchar        = this->OptChar();
  const char* longopt = this->LongOpt();
  const char* optDesc = this->Description();
  char* p             = buf;

  const char* value = nullptr;
  int longopt_len   = 0;
  int value_len     = 0;

  if (longopt)
  {
    value       = ::strchr(longopt, ' ');
    longopt_len = (value) ? static_cast<int>(value - longopt) : static_cast<int>(::strlen(longopt));
  }
  else
  {
    value = ::strchr(spec + 1, ' ');
  }
  while (value && (*value == ' '))
  {
    ++value;
  }
  if (value && *value)
  {
    value_len = static_cast<int>(::strlen(value));
  }
  else
  {
    value     = default_value;
    value_len = sizeof(default_value) - 1;
  }

  if ((optctrls & Options::ShortOnly) &&
      ((!isNullOpt(optchar)) || (optctrls & Options::NoGuessing)))
  {
    longopt = nullptr;
  }
  if ((optctrls & Options::LongOnly) && (longopt || (optctrls & Options::NoGuessing)))
  {
    optchar = '\0';
  }
  if (isNullOpt(optchar) && (longopt == nullptr))
  {
    *buf = '\0';
    return 0;
  }

  *(p++) = '[';

  // print the single character option
  if (!isNullOpt(optchar))
  {
    *(p++) = '-';
    *(p++) = optchar;
  }

  if ((!isNullOpt(optchar)) && (longopt))
  {
    *(p++) = '|';
  }

  // print the long option
  if (longopt)
  {
    *(p++) = '-';
    if (!(optctrls & (Options::LongOnly | Options::ShortOnly)))
    {
      *(p++) = '-';
    }
    strncpy(p, longopt, static_cast<size_t>(longopt_len));
    p += longopt_len;
  }

  // print any argument the option takes
  if (isValTaken())
  {
    *(p++) = ' ';
    if (isValOptional())
    {
      *(p++) = '[';
    }
    strcpy(p, value);
    p += value_len;
    if (isList())
    {
      strcpy(p, " ...");
      p += 4;
    }
    if (isValOptional())
    {
      *(p++) = ']';
    }
  }
  *(p++) = ']';

  if (optDesc[0] != '\0')
  {
    *(p++) = ',';
    *(p++) = ' ';
  }
  *p = '\0';
  strcat(p, optDesc);

  return static_cast<unsigned int>(strlen(buf));
#endif
}

const char* OptIter::operator()()
{
  const char* elt = this->Current();
  this->Next();
  return elt;
}


OptIterRwd::OptIterRwd()
{
}


OptIterRwd::~OptIterRwd()
{
}


OptArgvIter::~OptArgvIter()
{
}


const char* OptArgvIter::Current()
{
  return ((ndx == ac) || (av[ndx] == nullptr)) ? nullptr : av[ndx];
}


void OptArgvIter::Next()
{
  if ((ndx != ac) && av[ndx])
    ++ndx;
}


const char* OptArgvIter::operator()()
{
  return ((ndx == ac) || (av[ndx] == nullptr)) ? nullptr : av[ndx++];
}


void OptArgvIter::Rewind()
{
  ndx = 0;
}


static const char WHITESPACE[]           = " \t\n\r\v\f";
const char* OptStrTokIter::defaultDelims = WHITESPACE;


OptStrTokIter::OptStrTokIter(const char* tokens, const char* delimiters)
  : len(unsigned(strlen(tokens))), str(tokens), seps(delimiters), cur(nullptr), tokstr(nullptr)
{
  if (seps == nullptr)
    seps = defaultDelims;
  tokstr = new char[len + 1];
  ::strcpy(tokstr, str);
  cur = ::strtok(tokstr, seps);
}


OptStrTokIter::~OptStrTokIter()
{
  delete[] tokstr;
}


const char* OptStrTokIter::Current()
{
  return cur;
}


void OptStrTokIter::Next()
{
  if (cur)
    cur = ::strtok(nullptr, seps);
}


const char* OptStrTokIter::operator()()
{
  const char* elt = cur;
  if (cur)
    cur = ::strtok(nullptr, seps);
  return elt;
}


void OptStrTokIter::Rewind()
{
  ::strcpy(tokstr, str);
  cur = ::strtok(tokstr, seps);
}


enum
{
  c_COMMENT = '#'
};

const unsigned OptIstreamIter::MAX_LINE_LEN = 1024;


OptIstreamIter::OptIstreamIter(istream& input) : is(input), tok_iter(nullptr)
{
}


OptIstreamIter::~OptIstreamIter()
{
  delete tok_iter;
}


const char* OptIstreamIter::Current()
{
  const char* result = nullptr;
  if (tok_iter)
    result = tok_iter->Current();
  if (result)
    return result;
  this->Fill();
  return (!is) ? nullptr : tok_iter->Current();
}


void OptIstreamIter::Next()
{
  const char* result = nullptr;
  if (tok_iter)
    result = tok_iter->operator()();
  if (result)
    return;
  this->Fill();
  if (!is)
    tok_iter->Next();
}


const char* OptIstreamIter::operator()()
{
  const char* result = nullptr;
  if (tok_iter)
    result = tok_iter->operator()();
  if (result)
    return result;
  this->Fill();
  return (!is) ? nullptr : tok_iter->operator()();
}


// What we do is this: for each line of text in the istream, we use
// a OptStrTokIter to iterate over each token on the line.
//
// If the first non-white character on a line is c_COMMENT, then we
// consider the line to be a comment and we ignore it.
//

void OptIstreamIter::Fill()
{
  char* buf = new char[OptIstreamIter::MAX_LINE_LEN];
  do
  {
    *buf = '\0';
    is.getline(buf, sizeof(buf));
    char* ptr = buf;
    while (isspace(*ptr))
      ++ptr;
    if (*ptr && (*ptr != c_COMMENT))
    {
      delete tok_iter;
      tok_iter = new OptStrTokIter(ptr);
      return;
    }
  } while (is);

  delete[] buf;
}


} // namespace Utilities
