#pragma once

#include <iostream>
#include <memory>
#include <vector>

namespace LSYS
{

template<typename T>
class ListIterator;
template<typename T>
class ConstListIterator;

template<typename T>
class List
{
public:
  List() = default;
  List(const List& other) noexcept;
  List(List&&) noexcept = default;
  ~List() noexcept      = default;

  auto operator=(const List&) -> List& = delete;
  auto operator=(List&&) -> List&      = delete;

  [[nodiscard]] auto size() const -> size_t;
  using ListAsArray = std::vector<std::unique_ptr<T>>;
  [[nodiscard]] auto GetListAsArray() const noexcept -> const ListAsArray&;

  auto append(std::unique_ptr<T> item) -> void;

  // Append a dynamically allocated list; clears the source list.
  auto append(List<T>* list) -> void;

private:
  friend class ListIterator<T>;
  friend class ConstListIterator<T>;
  using ListIter = typename std::vector<std::unique_ptr<T>>::iterator;
  std::vector<std::unique_ptr<T>> m_stdList{};
  auto Clear() -> void;
};

template<typename T>
class ListIterator
{
public:
  explicit ListIterator(List<T>& list);
  explicit ListIterator(List<T>* list);

  [[nodiscard]] auto first() -> T*;
  [[nodiscard]] auto last() -> T*;
  [[nodiscard]] auto next() -> T*;
  [[nodiscard]] auto previous() -> T*;

private:
  using ListIter = typename std::vector<std::unique_ptr<T>>::iterator;
  List<T>* m_list;
  ListIter m_listIter;
};

template<typename T>
class ConstListIterator
{
public:
  explicit ConstListIterator(const List<T>& list);

  [[nodiscard]] auto current() const -> const T*;
  [[nodiscard]] auto first() -> const T*;
  [[nodiscard]] auto last() -> const T*;
  [[nodiscard]] auto next() -> const T*;
  auto previous() -> const T*;

private:
  using ConstListIter = typename std::vector<std::unique_ptr<T>>::const_iterator;
  const List<T>* m_list;
  ConstListIter m_listIter;
};

template<typename T>
inline List<T>::List(const List<T>& other) noexcept
{
  m_stdList.resize(other.m_stdList.size());
  for (auto i = 0U; i < m_stdList.size(); ++i)
  {
    if (other.m_stdList[i] == nullptr)
    {
      m_stdList[i] = nullptr;
    }
    else
    {
      m_stdList[i] = std::make_unique<T>(*other.m_stdList[i]);
    }
  }
}

template<typename T>
inline auto List<T>::size() const -> size_t
{
  return m_stdList.size();
}

template<typename T>
inline auto List<T>::GetListAsArray() const noexcept -> const ListAsArray&
{
  return m_stdList;
}

template<typename T>
inline auto List<T>::append(std::unique_ptr<T> item) -> void
{
  m_stdList.emplace_back(std::move(item));
}

template<typename T>
inline auto List<T>::append(List<T>* list) -> void
{
  for (ListIter i = list->m_stdList.begin(); i != list->m_stdList.end(); ++i)
  {
    m_stdList.emplace_back(std::move(*i));
  }
  list->Clear();
}

template<typename T>
inline auto List<T>::Clear() -> void
{
  m_stdList.clear();
}

template<typename T>
auto operator<<(std::ostream& out, const List<T>& list) -> std::ostream&
{
  ConstListIterator<T> iter(list);
  const T* val = iter.first();
  while (val != nullptr)
  {
    out << *val;
    val = iter.next();
  }
  return out;
}


template<typename T>
inline ListIterator<T>::ListIterator(List<T>& list)
  : m_list(&list), m_listIter(list.m_stdList.begin())
{
}

template<typename T>
inline ListIterator<T>::ListIterator(List<T>* const list)
  : m_list(list), m_listIter(list->m_stdList.begin())
{
}

template<typename T>
inline auto ListIterator<T>::next() -> T*
{
  if (m_listIter == m_list->m_stdList.end())
  {
    return nullptr;
  }
  ++m_listIter;
  if (m_listIter == m_list->m_stdList.end())
  {
    return nullptr;
  }
  return m_listIter->get();
}

template<typename T>
inline auto ListIterator<T>::previous() -> T*
{
  if (m_listIter == m_list->m_stdList.end())
  {
    return nullptr;
  }
  if (m_listIter == m_list->m_stdList.begin())
  {
    m_listIter = m_list->m_stdList.end();
    return nullptr;
  }
  --m_listIter;
  return m_listIter->get();
}

template<typename T>
inline auto ListIterator<T>::first() -> T*
{
  if (m_list->m_stdList.empty())
  {
    m_listIter = m_list->m_stdList.end();
    return nullptr;
  }
  m_listIter = m_list->m_stdList.begin();
  return m_listIter->get();
}

template<typename T>
inline auto ListIterator<T>::last() -> T*
{
  if (m_list->m_stdList.empty())
  {
    m_listIter = m_list->m_stdList.end();
    return nullptr;
  }
  m_listIter = m_list->m_stdList.end() - 1;
  return m_listIter->get();
}


template<typename T>
inline ConstListIterator<T>::ConstListIterator(const List<T>& list)
  : m_list{&list}, m_listIter(list.m_stdList.begin())
{
}

template<typename T>
inline auto ConstListIterator<T>::current() const -> const T*
{
  return (m_listIter != m_list->m_stdList.end()) ? m_listIter->get() : nullptr;
}

template<typename T>
inline auto ConstListIterator<T>::next() -> const T*
{
  if (m_listIter == m_list->m_stdList.end())
  {
    return nullptr;
  }
  ++m_listIter;
  if (m_listIter == m_list->m_stdList.end())
  {
    return nullptr;
  }
  return m_listIter->get();
}

template<typename T>
inline auto ConstListIterator<T>::previous() -> const T*
{
  if (m_listIter == m_list->m_stdList.end())
  {
    return nullptr;
  }
  if (m_listIter == m_list->m_stdList.begin())
  {
    return nullptr;
  }
  --m_listIter;
  return m_listIter->get();
}

template<typename T>
inline auto ConstListIterator<T>::first() -> const T*
{
  if (m_list->m_stdList.empty())
  {
    m_listIter = m_list->m_stdList.end();
    return nullptr;
  }
  m_listIter = m_list->m_stdList.begin();
  return m_listIter->get();
}

template<typename T>
inline auto ConstListIterator<T>::last() -> const T*
{
  if (m_list->m_stdList.empty())
  {
    m_listIter = m_list->m_stdList.end();
    return nullptr;
  }
  m_listIter = m_list->m_stdList.end() - 1;
  return m_listIter->get();
}

} // namespace LSYS
