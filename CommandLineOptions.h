#pragma once

#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <string>
#include <vector>

namespace Utilities
{

class Options;
class CommandLineOptionBase;

class CommandLineOptions
{
public:
  enum OptionReturnCode
  {
    ok = 0,
    shortCircuitOption,
    endOfOptions,
    badShortOption,
    badLongOption,
    ambiguousOption,
    argumentRequired,
    notEnoughPositionParams,
    tooManyPositionParams
  };
  enum OptionTypes
  {
    noArgs = 0,
    noArgsShortCircuitOption,
    optionalArg,
    requiredArg,
    zeroOrMoreArgs,
    oneOrMoreArgs
  };

  CommandLineOptions(bool use_f_flag = true);
  CommandLineOptions(const CommandLineOptions&) = delete;
  ~CommandLineOptions();

  template<typename T>
  void Add(char optChar,
           const std::string& longOpt,
           const std::string& optDescription,
           OptionTypes,
           T* theVal);
  void Add(const CommandLineOptionBase&);
  void SetPositional(int minNum, int maxNum, std::vector<std::string>*);
  OptionReturnCode ProcessOptions(int argc, const char* argv[]);

  char LastShortOption() const;
  const char* LastLongOption() const;
  void Usage(std::ostream&, const char* positionalParamsDescription);
  static const char optTypeChar[oneOrMoreArgs + 1];
  static char OptTypeChar(OptionTypes t) { return optTypeChar[t]; }

private:
  Options* rawOptions;
  std::vector<CommandLineOptionBase*> cmdOptions{};
  const char** optionDefinitions = nullptr;
  const char** optionDescriptions = nullptr;
  int minNumPositional = 0;
  int maxNumPositional = 0;
  std::vector<std::string>* positionalParams = nullptr;
  char lastShortOption{};
  const char* lastLongOption = nullptr;
  void FreeCmdOptions();
  void FreeOptionDefinitions();
  void FreeOptionDescriptions();
};

class CommandLineOptionBase
{
public:
  CommandLineOptionBase(char optChar,
                        const std::string& longOpt,
                        const std::string& optDescription,
                        CommandLineOptions::OptionTypes);
  CommandLineOptionBase(const CommandLineOptionBase&);
  virtual ~CommandLineOptionBase() {}
  virtual CommandLineOptionBase* Clone() const;
  virtual void SetValue([[maybe_unused]] const char* valStr) {}

  char OptChar() const { return optChar; }
  CommandLineOptions::OptionTypes OptType() const { return optType; }
  std::string LongOpt() const { return longOpt; }
  std::string OptDescription() const { return optDescription; }

private:
  const char optChar;
  const CommandLineOptions::OptionTypes optType;
  std::string longOpt;
  std::string optDescription;
};

template<typename T>
class CommandLineOption : public CommandLineOptionBase
{
public:
  CommandLineOption(char optionChar,
                    const std::string& longOption,
                    const std::string& optionDescription,
                    CommandLineOptions::OptionTypes,
                    T* theVal);
  CommandLineOption(const CommandLineOption&);
  virtual CommandLineOption* Clone() const;
  virtual void SetValue(const char* valStr);
  const T& Value() const { return *theVal; }

private:
  T* theVal;
};

template<typename T>
CommandLineOption<T>::CommandLineOption(char optionChar,
                                        const std::string& longOption,
                                        const std::string& optionDescription,
                                        CommandLineOptions::OptionTypes optionType,
                                        T* _theVal)
  : CommandLineOptionBase(optionChar, longOption, optionDescription, optionType), theVal(_theVal)
{
  if (theVal == 0)
  {
    throw std::runtime_error("Cannot have null option address for short option " +
                             std::string(1, optionChar) + ", long option " + longOption);
  }
}

template<typename T>
inline CommandLineOption<T>::CommandLineOption(const CommandLineOption<T>& c)
  : CommandLineOptionBase(c), theVal(c.theVal)
{
}

template<typename T>
inline CommandLineOption<T>* CommandLineOption<T>::Clone() const
{
  return new CommandLineOption<T>(*this);
}

template<typename T>
inline void CommandLineOption<T>::SetValue(const char* valStr)
{
  *theVal = boost::lexical_cast<T>(valStr);
}

template<>
inline void CommandLineOption<bool>::SetValue(const char*)
{
  *theVal = true;
}

template<>
inline void CommandLineOption<const char*>::SetValue(const char* valStr)
{
  *theVal = valStr;
}

template<>
inline void CommandLineOption<std::vector<std::string>>::SetValue(const char* valStr)
{
  if (valStr != 0)
    theVal->push_back(valStr); // ????????????????????????????????????????
}

template<typename T>
inline void CommandLineOptions::Add(char optChar,
                                    const std::string& longOpt,
                                    const std::string& optDescription,
                                    OptionTypes optType,
                                    T* theVal)
{
  this->Add(CommandLineOption<T>(optChar, longOpt, optDescription, optType, theVal));
}

} // namespace Utilities
