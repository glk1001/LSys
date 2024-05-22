module;

#include <map>
#include <string>

export module LSys.SymbolTable;

export namespace LSYS
{

template<typename T>
class Symbol
{
public:
  Symbol(const std::string& name, const T& value) : m_tag(name), m_value(value) {}

  [[nodiscard]] auto GetName() const -> const std::string& { return m_tag; }
  auto SetName(const std::string& name) -> void { m_tag = name; }
  [[nodiscard]] auto GetValue() const -> const T& { return m_value; }
  auto SetValue(const T& value) -> void { m_value = value; }

private:
  std::string m_tag;
  T m_value;
};

template<typename T>
class SymbolTable
{
public:
  auto Enter(const std::string& name, const T& value) -> bool;
  [[nodiscard]] auto Lookup(const std::string& name, T& value) const -> bool;

private:
  std::map<std::string, Symbol<T>> m_symbolTable{};
};

} // namespace LSYS

namespace LSYS
{

template<typename T>
auto SymbolTable<T>::Enter(const std::string& name, const T& value) -> bool
{
  const auto iter = m_symbolTable.find(name);

  if (iter == m_symbolTable.end())
  {
    const auto symbol = Symbol<T>{name, value};
    m_symbolTable.insert({name, symbol});
    return true;
  }

  iter->second.SetValue(value);
  return false;
}

template<typename T>
auto SymbolTable<T>::Lookup(const std::string& name, T& value) const -> bool
{
  const auto iter = m_symbolTable.find(name);
  if (iter == m_symbolTable.end())
  {
    return false;
  }

  value = iter->second.GetValue();
  return true;
}

} // namespace LSYS
