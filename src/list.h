#pragma once

#include <iostream>
#include <vector>

namespace L_SYSTEM
{

template<typename T>
class ListIterator;
template<typename T>
class ConstListIterator;

template<typename T>
class List
{
public:
  List()            = default;
  List(const List&) = delete;
  List(List&&)      = delete;
  ~List();

  auto operator=(const List&) -> List& = delete;
  auto operator=(List&&) -> List&      = delete;

  [[nodiscard]] auto size() const -> size_t;

  auto append(T* item) -> void;

  // Append a dynamically allocated list; clears the source list.
  auto append(List<T>* list) -> void;

private:
  friend class ListIterator<T>;
  friend class ConstListIterator<T>;
  using ListIter = typename std::vector<T*>::iterator;
  std::vector<T*> m_stdList{};
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
  using ListIter = typename std::vector<T*>::iterator;
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
  using ConstListIter = typename std::vector<T*>::const_iterator;
  const List<T>* m_list;
  ConstListIter m_listIter;
};

template<typename T>
List<T>::~List()
{
  for (ListIter i = m_stdList.begin(); i != m_stdList.end(); ++i)
  {
    delete *i;
  }
}

template<typename T>
inline auto List<T>::size() const -> size_t
{
  return m_stdList.size();
}

template<typename T>
inline auto List<T>::append(T* const item) -> void
{
  m_stdList.push_back(item);
}

template<typename T>
inline auto List<T>::append(List<T>* const list) -> void
{
  for (ListIter i = list->m_stdList.begin(); i != list->m_stdList.end(); ++i)
  {
    m_stdList.push_back(*i);
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
  while (val != 0)
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
  return *m_listIter;
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
  return *m_listIter;
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
  return *m_listIter;
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
  return *m_listIter;
}


template<typename T>
inline ConstListIterator<T>::ConstListIterator(const List<T>& list)
  : m_list(&list), m_listIter(list.m_stdList.begin())
{
}

template<typename T>
inline auto ConstListIterator<T>::current() const -> const T*
{
  return (m_listIter != m_list->m_stdList.end()) ? *m_listIter : nullptr;
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
  return *m_listIter;
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
  return *m_listIter;
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
  return *m_listIter;
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
  return *m_listIter;
}

} // namespace L_SYSTEM