#pragma once

#include <map>
#include <string>

namespace L_SYSTEM
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
  using SymbolTableMap = typename std::map<std::string, Symbol<T>>;
  SymbolTableMap m_symbolTable{};
};

template<typename T>
auto SymbolTable<T>::Enter(const std::string& name, const T& value) -> bool
{
  const auto iter = m_symbolTable.find(name);

  if (iter == m_symbolTable.end())
  {
    const auto symbol = Symbol<T>{name, value};
    m_symbolTable.insert(typename SymbolTableMap::value_type(name, symbol));
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

} // namespace L_SYSTEM
