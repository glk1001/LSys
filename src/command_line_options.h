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
  enum class OptionReturnCode
  {
    OK = 0,
    SHORT_CIRCUIT_OPTION,
    END_OF_OPTIONS,
    BAD_SHORT_OPTION,
    BAD_LONG_OPTION,
    AMBIGUOUS_OPTION,
    ARGUMENT_REQUIRED,
    NOT_ENOUGH_POSITIONAL_PARAMS,
    TOO_MANY_POSITIONAL_PARAMS,
  };
  enum class OptionTypes
  {
    NO_ARGS = 0,
    NO_ARGS_SHORT_CIRCUIT_OPTION,
    OPTIONAL_ARG,
    REQUIRED_ARG,
    ZERO_OR_MORE_ARGS,
    ONE_OR_MORE_ARGS,
  };

  CommandLineOptions(bool use_f_flag = true);
  CommandLineOptions(const CommandLineOptions&) = delete;
  CommandLineOptions(CommandLineOptions&&)      = default;
  ~CommandLineOptions();

  auto operator=(const CommandLineOptions&) -> CommandLineOptions& = delete;
  auto operator=(CommandLineOptions&&) -> CommandLineOptions&      = delete;

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
  static const char optTypeChar[static_cast<size_t>(OptionTypes::ONE_OR_MORE_ARGS) + 1];
  static char OptTypeChar(OptionTypes t) { return optTypeChar[static_cast<size_t>(t)]; }

private:
  Options* rawOptions;
  std::vector<CommandLineOptionBase*> cmdOptions{};
  const char** optionDefinitions             = nullptr;
  const char** optionDescriptions            = nullptr;
  int minNumPositional                       = 0;
  int maxNumPositional                       = 0;
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
  CommandLineOptionBase(const CommandLineOptionBase&) = default;
  CommandLineOptionBase(CommandLineOptionBase&&)      = default;
  virtual ~CommandLineOptionBase();

  auto operator=(const CommandLineOptionBase&) -> CommandLineOptionBase& = delete;
  auto operator=(CommandLineOptionBase&&) -> CommandLineOptionBase&      = delete;

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
                    T* val);
  CommandLineOption(const CommandLineOption&);
  CommandLineOption(CommandLineOption&&) = default;
  virtual ~CommandLineOption()           = default;

  auto operator=(const CommandLineOption&) -> CommandLineOption& = delete;
  auto operator=(CommandLineOption&&) -> CommandLineOption&      = delete;

  CommandLineOption* Clone() const override;
  void SetValue(const char* valStr) override;
  const T& Value() const { return *m_val; }

private:
  T* m_val;
};

template<typename T>
CommandLineOption<T>::CommandLineOption(char optionChar,
                                        const std::string& longOption,
                                        const std::string& optionDescription,
                                        CommandLineOptions::OptionTypes optionType,
                                        T* val)
  : CommandLineOptionBase(optionChar, longOption, optionDescription, optionType), m_val(val)
{
  if (m_val == nullptr)
  {
    throw std::runtime_error("Cannot have null option address for short option " +
                             std::string(1, optionChar) + ", long option " + longOption);
  }
}

template<typename T>
inline CommandLineOption<T>::CommandLineOption(const CommandLineOption<T>& c)
  : CommandLineOptionBase(c), m_val(c.m_val)
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
  *m_val = boost::lexical_cast<T>(valStr);
}

template<>
inline void CommandLineOption<bool>::SetValue(const char*)
{
  *m_val = true;
}

template<>
inline void CommandLineOption<const char*>::SetValue(const char* valStr)
{
  *m_val = valStr;
}

template<>
inline void CommandLineOption<std::vector<std::string>>::SetValue(const char* valStr)
{
  if (valStr != nullptr)
    m_val->push_back(valStr); // ????????????????????????????????????????
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
