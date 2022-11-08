// ****************************************************************************
// ^FILE: options.h - option parsing classes
//
// ^DESCRIPTION:
//    This file defines classes used to parse command-line options.
//    Options may be parsed from an array of strings, or from any structure
//    for which a corresponding option-iterator exists.
//
// ^HISTORY:
//    03/06/92  Brad Appleton   <bradapp@enteract.com>   Created
//
//    03/23/93	Brad Appleton	<bradapp@enteract.com>
//    - Added OptIstreamIter class
//
//    03/08/94	Brad Appleton	<bradapp@enteract.com>
//    - Added Options::reset() member function
//
//    07/31/97	Brad Appleton	<bradapp@enteract.com>
//    - Added ParsePos control flag and Positional return value
// ^^**************************************************************************

#pragma once

#include <iosfwd>

namespace Utilities
{

//
//
// CLASS
// =====
// Options  -- parse command-line options
//
// SYNOPSIS
// ========
//   #include <options.h>
//
//   Options opts(cmdname, optv);
//   char cmdname[], *optv[];
//
// DESCRIPTION
// ===========
// The Options constructor expects a command-name (usually argv[0]) and
// a pointer to an array of strings.  The last element in this array MUST
// be NULL. Each non-NULL string in the array must have the following format:
//
//   The 1st character must be the option-name ('c' for a -c option).
//
//   The 2nd character must be one of '|', '?', ':', '*', or '+'.
//      '|' -- indicates that the option takes NO argument;
//      '?' -- indicates that the option takes an OPTIONAL argument;
//      ':' -- indicates that the option takes a REQUIRED argument;
//      '*' -- indicates that the option takes 0 or more arguments;
//      '+' -- indicates that the option takes 1 or more arguments;
//
//   The remainder of the string must be the long-option name.
//
//   If desired, the long-option name may be followed by one or more
//   spaces and then by the name of the option value. This name will
//   be used when printing usage messages. If the option-value-name
//   is not given then the string "<value>" will be used in usage
//   messages.
//
//   One may use a space to indicate that a particular option does not
//   have a corresponding long-option.  For example, "c: " (or "c:")
//   means the -c option takes a value & has NO corresponding long-option.
//
//   To specify a long-option that has no corresponding single-character
//   option is a bit trickier: Options::operator() still needs an "option-
//   character" to return when that option is matched. One may use a whitespace
//   character or a non-printable character as the single-character option
//   in such a case. (hence " |hello" would only match "--hello").
//
//   EXCEPTIONS TO THE ABOVE:
//   ------------------------
//   If the 1st character of the string is '-', then the rest of the
//   string must correspond to the above format, and the option is
//   considered to be a hidden-option. This means it will be parsed
//   when actually matching options from the command-line, but will
//   NOT show-up if a usage message is printed using the usage() member
//   function. Such an example might be "-h|hidden". If you want to
//   use any "dummy" options (options that are not parsed, but that
//   to show up in the usage message), you can specify them along with
//   any positional parameters to the usage() member function.
//
//   If the 2nd character of the string is '\0' then it is assumed
//   that there is no corresponding long-option and that the option
//   takes no argument (hence "f", and "f| " are equivalent).
//
//   Examples:
//      const char * optv[]= {
//          "c:count   <number>",
//          "s?str     <string>",
//          "x",
//          " |hello",
//          "g+groups  <newsgroup>",
//          NULL
//      } ;
//
//      optv[] now corresponds to the following:
//
//            usage: cmdname [-c|--count <number>] [-s|--str [<string>]]
//                           [-x] [--hello] [-g|--groups <newsgroup> ...]
//
// Long-option names are matched case-insensitive and only a unique prefix
// of the name needs to be specified.
//
// Option-name characters are case-sensitive!
//
// CAVEAT
// ======
// Because of the way in which multi-valued options and options with optional
// values are handled, it is NOT possible to supply a value to an option in
// a separate argument (different argv[] element) if the value is OPTIONAL
// and begins with a '-'. What this means is that if an option "-s" takes an
// optional value value and you wish to supply a value of "-foo" then you must
// specify this on the command-line as "-s-foo" instead of "-s -foo" because
// "-s -foo" will be considered to be two separate sets of options.
//
// A multi-valued option is terminated by another option or by the end-of
// options. The following are all equivalent (if "-l" is a multi-valued
// option and "-x" is an option that takes no value):
//
//    cmdname -x -l item1 item2 item3 -- arg1 arg2 arg3
//    cmdname -x -litem1 -litem2 -litem3 -- arg1 arg2 arg3
//    cmdname -l item1 item2 item3 -x arg1 arg2 arg3
//
//
// EXAMPLE
// =======
//    #include <options.h>
//
//    static const char * optv[]= {
//       "H|help",
//       "c:count   <number>",
//       "s?str     <string>",
//       "x",
//       " |hello",
//       "g+groups  <newsgroup>",
//       NULL
//    } ;
//
//    main(int argc, char * argv[]) {
//       int  optchar;
//       const char * optarg;
//       const char * str= "default_string";
//       int  count= 0, xflag= 0, hello= 0;
//       int  errors= 0, ngroups= 0;
//
//       Options  opts(*argv, optv);
//       OptArgvIter  iter(--argc, ++argv);
//
//       while( optchar= opts(iter, optarg) ) {
//          switch (optchar) {
//          case 'H' :
//             opts.usage(cout, "files ...");
//             exit(0);
//             break;
//          case 'g' :
//             ++ngroups; break;  // the groupname is in "optarg"
//          case 's' :
//             str= optarg; break;
//          case 'x' :
//             ++xflag; break;
//          case ' ' :
//             ++hello; break;
//          case 'c' :
//             if (optarg == NULL)  ++errors;
//             else  count= (int) atol(optarg);
//             break;
//          default :  ++errors; break;
//          } //switch
//       }
//
//       if (errors || (iter.Index() == argc)) {
//          if (! errors) {
//             cerr << opts.name() << ": no filenames given." << endl ;
//          }
//          opts.usage(cerr, "files ...");
//          exit(1);
//       }
//
//       cout << "xflag=" << ((xflag) ? "ON"  : "OFF") << endl
//            << "hello=" << ((hello) ? "YES" : "NO") << endl
//            << "count=" << count << endl
//            << "str=\"" << ((str) ? str : "No value given!") << "\"" << endl
//            << "ngroups=" << ngroups << endl ;
//
//       if (iter.Index() < argc) {
//          cout << "files=" ;
//          for (int i= iter.Index() ; i < argc ; i++) {
//             cout << "\"" << argv[i] << "\" " ;
//          }
//          cout << endl ;
//       }
//    }
//

class OptIter;
class OptionSpec;

class Options
{
public:
  enum OptCtrl
  {
    Default   = 0x00, // Default setting
    AnyCase   = 0x01, // Ignore case when matching short-options
    Quiet     = 0x02, // Don't print error messages
    Plus      = 0x04, // Allow "+" as a long-option prefix
    ShortOnly = 0x08, // Don't accept long-options
    LongOnly  = 0x10, // Don't accept short-options
    //   (also allows "-" as a long-option prefix).
    NoGuessing = 0x20, // Normally, when we see a short (long) option
    //   on the command line that doesn't match any
    //   known short (long) options, then we try to
    //   "guess" by seeing if it will match any known
    //   long (short) option. Setting this mask prevents
    //   this "guessing" from occurring.
    ParsePos = 0x40 // By default, Options will not present positional
    //   command-line arguments to the user and will
    //   instead stop parsing when the first positonal
    //   argument has been encountered. If this flag
    //   is given, Options will present positional
    //   arguments to the user with a return code of
    //   Positional; EndOpts will be returned only
    //   when the end of the argument list is reached.
  };

  // Error return values for operator()
  //
  enum OptRC
  {
    EndOpts     = 0,
    BadChar     = -1,
    BadKeyword  = -2,
    Ambiguous   = -3,
    Positional  = -4,
    ArgRequired = -5
  };

  Options();
  Options(const char* name, const char* const optv[], const char* const optDescriptions[]);
  virtual ~Options();

  void SetOptions(const char* name, const char* const optv[], const char* const optDescriptions[]);

  // Name() returns the command name
  const char* Name() const { return cmdname; }

  // Controls() (with no arguments) returns the existing control settings
  unsigned Controls() const { return optctrls; }

  // Controls() (with 1 argument) sets new control settings
  void Controls(unsigned newctrls) { optctrls = newctrls; }
  void Controls(const char* flagsStr);

  // reset for another pass to parse for options
  void Reset() { nextchar = listopt = NULL; }

  // usage() prints options usage (followed by any positional arguments
  // listed in the parameter "positionals") on the given outstream
  void Usage(std::ostream&, const char* positionals) const;

  // operator() iterates through the arguments as necessary (using the
  // given iterator) and returns the character value of the option
  // (or long-option) that it matched. If the option has a value
  // then the value given may be found in optarg (otherwise optarg
  // will be NULL).
  //
  // 0 is returned upon end-of-options. At this point, "iter" may
  // be used to process any remaining positional parameters. If the
  // ParsePos control-flag is set then 0 is returned only when all
  // arguments in "iter" have been exhausted.
  //
  // If an invalid option is found then BadChar is returned and *optarg
  // is the unrecognized option character.
  //
  // If an invalid long-option is found then BadKeyword is returned and optarg
  // points to the bad long-option.
  //
  // If an ambiguous long-option is found then Ambiguous is returned and
  // optarg points to the ambiguous long-option.
  //
  // If the ParsePos control-flag is set then Positional is returned
  // when a positional argument is encountered and optarg points to
  // the positonal argument (and "iter" is advanced to the next argument
  // in the iterator).
  //
  // Unless Options::Quiet is used, missing option-arguments and
  // invalid options (and the like) will automatically cause error
  // messages to be issued to cerr.
  int operator()(OptIter&, const char** optarg, const char** longOpt = 0);

  // Call this member function after operator() has returned 0
  // if you want to know whether or not options were explicitly
  // terminated because "--" appeared on the command-line.
  //
  int ExplicitEndOpts() const { return explicitEnd; }

private:
  unsigned explicitEnd : 1; // were we terminated because of "--"?
  unsigned optctrls : 7; // control settings (a set of OptCtrl masks)
  const char* const* optvec; // vector of option-specifications (last=NULL)
  const char* const* optDesc; // vector of option-descriptions (must match optvec)
  const char* nextchar; // next option-character to process
  const char* listopt; // last list-option we matched
  const char* cmdname; // name of the command
  bool autoSetControlFlags;
  void CheckSyntax() const;
  OptionSpec MatchOpt(char opt, int ignore_case = 0) const;
  OptionSpec MatchLongOpt(const char* opt, int len, int& ambiguous) const;
  int ParseOpt(OptIter& iter, const char** optarg, const char** longOpt);
  int ParseLongOpt(OptIter& iter, const char** optarg, const char** longOpt);
};


// Abstract class to iterate through options/arguments
//
class OptIter
{
public:
  OptIter() {}
  virtual ~OptIter();

  // curr() returns the current item in the iterator without
  // advancing on to the next item. If we are at the end of items
  // then NULL is returned.
  virtual const char* Current() = 0;

  // next() advances to the next item.
  virtual void Next() = 0;

  // operator() returns the current item in the iterator and then
  // advances on to the Next item. If we are at the end of items
  // then NULL is returned.
  virtual const char* operator()();
};


// Abstract class for a rewindable OptIter
//
class OptIterRwd : public OptIter
{
public:
  OptIterRwd();
  virtual ~OptIterRwd();

  virtual const char* Current()    = 0;
  virtual void Next()              = 0;
  virtual const char* operator()() = 0;

  // Rewind() resets the "current-element" to the first one in the "list"
  virtual void Rewind() = 0;
};


// Class to iterate through an array of tokens. The array may be terminated
// by NULL or a count containing the number of tokens may be given.
//
class OptArgvIter : public OptIterRwd
{
public:
  OptArgvIter(const char* const argv[]) : av(argv), ac(-1), ndx(0) {}

  OptArgvIter(int argc, const char* const argv[]) : av(argv), ac(argc), ndx(0) {}

  virtual ~OptArgvIter();

  virtual const char* Current();
  virtual void Next();
  virtual const char* operator()();
  virtual void Rewind();

  // Index returns the current Index to use for argv[]
  int Index() { return ndx; }

private:
  int ndx; // Index of current arg
  int ac; // arg count
  const char* const* av; // arg vector
};


// Class to iterate through a string containing delimiter-separated tokens
//
class OptStrTokIter : public OptIterRwd
{
public:
  OptStrTokIter(const char* tokens, const char* delimiters = 0);
  virtual ~OptStrTokIter();

  virtual const char* Current();
  virtual void Next();
  virtual const char* operator()();
  virtual void Rewind();

  // delimiters() with NO arguments returns the current set of delimiters,
  // If an argument is given then it is used as the new set of delimiters.
  const char* Delimiters() { return seps; }
  void Delimiters(const char* delims) { seps = (delims) ? delims : defaultDelims; }

private:
  unsigned len; // length of token-string
  const char* str; // the token-string
  const char* seps; // delimiter-set (separator-characters)
  const char* cur; // current token
  char* tokstr; // our copy of the token-string
  static const char* defaultDelims; // default delimiters = whitespace
};


// OptIstreamIter is a class for iterating over arguments that come
// from an input stream. Each line of the input stream is considered
// to be a set of white-space separated tokens. If the the first
// non-white character on a line is '#' ('!' for VMS systems) then
// the line is considered a comment and is ignored.
//
// *Note:: If a line is more than 1022 characters in length then we
// treat it as if it were several lines of length 1022 or less.
//
// *Note:: The string tokens returned by this iterator are pointers
//         to temporary buffers which may not necessarily stick around
//         for too long after the call to Current() or operator(), hence
//         if you need the string value to persist - you will need to
//         make a copy.
//
class OptIstreamIter : public OptIter
{
public:
  static const unsigned MAX_LINE_LEN;

  OptIstreamIter(std::istream& input);
  virtual ~OptIstreamIter();

  virtual const char* Current();
  virtual void Next();
  virtual const char* operator()();

private:
  std::istream& is;
  OptStrTokIter* tok_iter;
  void Fill();
};

} // namespace Utilities
