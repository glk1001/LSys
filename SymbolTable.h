#ifndef _SymbolTable_h
  #define _SymbolTable_h

  #include <map>
  #include <string>
  #include <iostream>

  namespace LSys {

    typedef void (*Anyptr)();

    template <typename T> class Symbol;
    template <typename T> std::ostream& operator<<(std::ostream&, const Symbol<T>&);

    template <typename T> class Symbol {
      public:
        Symbol(const std::string& name, const T& v)
        : tag(name), value(v) {}
        ~Symbol() {}
        const std::string& Name() const { return tag; }
        void SetName(const std::string& name) { tag= name; }
        const T& Value() const { return value; }
        void SetValue(const T& v) { value= v; }
        friend std::ostream& operator<< <T>(std::ostream&, const Symbol<T>&);
      private:
        std::string tag;
        T value;
    };

    template <typename T> class SymbolTable;
    template <typename T> std::ostream& operator<<(std::ostream&, const SymbolTable<T>&);

    template <typename T> class SymbolTable {
      public:
        bool enter(const std::string& name, const T&);
        bool lookup(const std::string& name, T&) const;
        bool enter(const char* name, const T& v) { return this->enter(std::string(name), v); }
        bool lookup(const char* name, T& v) const { return this->lookup(std::string(name), v); }
        friend std::ostream& operator<< <T>(std::ostream&, const SymbolTable<T>&);
      private:
        typedef typename std::map<std::string, Symbol<T> > SymbolTableMap;
        SymbolTableMap symTable;
    };

    template <typename T>
      bool SymbolTable<T>::enter(const std::string& name, const T& v)
    {
      typename SymbolTableMap::iterator iter= symTable.find(name);
      if (iter == symTable.end()) {
        Symbol<T> sym(name, v);
        symTable.insert(typename SymbolTableMap::value_type(name, sym));
        return true;
      }
      iter->second.SetValue(v);
      return false;
    }

    template <typename T> bool SymbolTable<T>::lookup(const std::string& name, T& v) const
    {
      typename SymbolTableMap::const_iterator iter= symTable.find(name);
      if (iter == symTable.end())
        return false;
      v= iter->second.Value();
      return true;
    }

  };

#endif
