#pragma once

#include <iostream>
#include <vector>

namespace LSys
{

template<typename T>
class ListIterator;
template<typename T>
class ConstListIterator;

template<typename T>
class List
{
public:
  List();
  ~List(); // Delete all items on the list
  size_t size() const;
  void append(T*);
  void append(List<T>*);
  // Append a dynamically allocated list; clears the source list.
private:
  friend class ListIterator<T>;
  friend class ConstListIterator<T>;
  typedef typename std::vector<T*>::iterator ListIter;
  std::vector<T*> stdlist;
  void Clear();
};

template<typename T>
class ListIterator
{
public:
  ListIterator(List<T>&);
  ListIterator(List<T>*);
  ~ListIterator();
  T* current();
  T* first();
  T* last();
  T* next();
  T* previous();

private:
  typedef typename std::vector<T*>::iterator ListIter;
  List<T>* list;
  ListIter ptr;
};

template<typename T>
class ConstListIterator
{
public:
  ConstListIterator(const List<T>&);
  ~ConstListIterator();
  const T* current();
  const T* first();
  const T* last();
  const T* next();
  const T* previous();

private:
  typedef typename std::vector<T*>::const_iterator ConstListIter;
  const List<T>* list;
  ConstListIter ptr;
};


template<typename T>
inline List<T>::List()
{
}

template<typename T>
inline List<T>::~List()
{
  for (ListIter i = stdlist.begin(); i != stdlist.end(); i++)
    delete *i;
}

template<typename T>
inline size_t List<T>::size() const
{
  return stdlist.size();
}

template<typename T>
inline void List<T>::append(T* o)
{
  stdlist.push_back(o);
}

template<typename T>
inline void List<T>::append(List<T>* l)
{
  for (ListIter i = l->stdlist.begin(); i != l->stdlist.end(); i++)
    stdlist.push_back(*i);
  l->Clear();
}

template<typename T>
inline void List<T>::Clear()
{
  stdlist.clear();
}

template<typename T>
std::ostream& operator<<(std::ostream& f, const List<T>& l)
{
  ConstListIterator<T> iter(l);
  const T* val = iter.first();
  while (val != 0)
  {
    f << *val;
    val = iter.next();
  }
  return f;
}


template<typename T>
inline ListIterator<T>::ListIterator(List<T>& l) : list(&l), ptr(l.stdlist.begin())
{
}

template<typename T>
inline ListIterator<T>::ListIterator(List<T>* l) : list(l), ptr(l->stdlist.begin())
{
}

template<typename T>
inline ListIterator<T>::~ListIterator()
{
}

template<typename T>
inline T* ListIterator<T>::current()
{
  return (ptr != list->stdlist.end()) ? *ptr : 0;
}

template<typename T>
inline T* ListIterator<T>::next()
{
  if (ptr == list->stdlist.end())
    return 0;
  ptr++;
  if (ptr == list->stdlist.end())
  {
    return 0;
  }
  return *ptr;
}

template<typename T>
inline T* ListIterator<T>::previous()
{
  if (ptr == list->stdlist.end())
    return 0;
  if (ptr == list->stdlist.begin())
  {
    ptr = list->stdlist.end();
    return 0;
  }
  ptr--;
  return *ptr;
}

template<typename T>
inline T* ListIterator<T>::first()
{
  if (list->stdlist.empty())
  {
    ptr = list->stdlist.end();
    return 0;
  }
  ptr = list->stdlist.begin();
  return *ptr;
}

template<typename T>
inline T* ListIterator<T>::last()
{
  if (list->stdlist.empty())
  {
    ptr = list->stdlist.end();
    return 0;
  }
  ptr = list->stdlist.end() - 1;
  return *ptr;
}


template<typename T>
inline ConstListIterator<T>::ConstListIterator(const List<T>& l) : list(&l), ptr(l.stdlist.begin())
{
}

template<typename T>
inline ConstListIterator<T>::~ConstListIterator()
{
}

template<typename T>
inline const T* ConstListIterator<T>::current()
{
  return (ptr != list->stdlist.end()) ? *ptr : 0;
}

template<typename T>
inline const T* ConstListIterator<T>::next()
{
  if (ptr == list->stdlist.end())
    return 0;
  ptr++;
  if (ptr == list->stdlist.end())
  {
    return 0;
  }
  return *ptr;
}

template<typename T>
inline const T* ConstListIterator<T>::previous()
{
  if (ptr == list->stdlist.end())
    return 0;
  if (ptr == list->stdlist.begin())
  {
    return 0;
  }
  ptr--;
  return *ptr;
}

template<typename T>
inline const T* ConstListIterator<T>::first()
{
  if (list->stdlist.empty())
  {
    ptr = list->stdlist.end();
    return 0;
  }
  ptr = list->stdlist.begin();
  return *ptr;
}

template<typename T>
inline const T* ConstListIterator<T>::last()
{
  if (list->stdlist.empty())
  {
    ptr = list->stdlist.end();
    return 0;
  }
  ptr = list->stdlist.end() - 1;
  return *ptr;
}

} // namespace LSys
