#pragma once
#include <unordered_map>
#include <vector>
#include <variant>
#include <cassert>
#include <string>
#include <iostream>
#include <compare>
#include <iomanip>

using namespace std;

#define STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes) \
  std::enable_if_t< \
    std::is_same_v<std::decay_t<StrictTypes>, Types> && \
    !std::is_same_v<std::decay_t<T>, Types>, \
  bool>

#define STRICTLY_TYPES(T) \
  enable_if_t<is_same_v<decay_t<T>, Types>, bool>

#define STRICT_SQLCHAR_VARCHAR(StrictVarchar, StrictSQLChar) \
    std::enable_if_t< \
      std::is_same_v<std::decay_t<StrictVarchar>, Varchar> && \
      std::is_same_v<std::decay_t<StrictSQLChar>, SQLChar>, \
    bool> 

constexpr monostate Null{};

class Table;

// Should just remove NULL support from this class...
// Or maybe keep it this way for now and see if it truly becomes a problem
// Make subsequent classes that don't support NULL and test those
class Varchar{
  public:
    // Constructors
    Varchar();
    Varchar(string Value);

    Varchar(int Length);
    Varchar(int Length, string Value);

    Varchar(Table* table, std::string name);
    Varchar(Table* table, std::string name, string Value);

    string getValue() const;

    string value;
    int length;
    
    // Helper function to check for null values
    template <typename Comparator>
    bool comparatorHelper
    (const string &lhs, const string &rhs, const Comparator comp) const {
      return comp(lhs, rhs);
    }

    bool operator==(const Varchar &rhs) const;
    bool operator!=(const Varchar &rhs) const;
    bool operator<(const Varchar &rhs) const;
    bool operator>(const Varchar &rhs) const;
    bool operator<=(const Varchar &rhs) const;
    bool operator>=(const Varchar &rhs) const; 

    bool operator==(const string &rhs) const;
    bool operator!=(const string &rhs) const;
    bool operator<(const string &rhs) const;
    bool operator>(const string &rhs) const;
    bool operator<=(const string &rhs) const;
    bool operator>=(const string &rhs) const; 

    friend bool operator==(const string &lhs, const Varchar &rhs);
    friend bool operator!=(const string &lhs, const Varchar &rhs);
    friend bool operator<(const string &lhs, const Varchar &rhs);
    friend bool operator>(const string &lhs, const Varchar &rhs);
    friend bool operator<=(const string &lhs, const Varchar &rhs);
    friend bool operator>=(const string &lhs, const Varchar &rhs);

    friend ostream& operator<<(ostream& os, const Varchar& self);

  private:
    virtual void enforceLengthInvariant();
};

class SQLChar : public Varchar {
  public:
    using Varchar::Varchar;

    SQLChar();
    SQLChar(string Value);

    SQLChar(int Length);
    SQLChar(int Length, string Value);

    SQLChar(Table* table, std::string name);
    SQLChar(Table* table, string name, string Value);

    // resolve ambiguity by  overloading the parent comparators
    bool operator==(const SQLChar &rhs) const;
    bool operator!=(const SQLChar &rhs) const;
    bool operator<(const SQLChar &rhs) const;
    bool operator>(const SQLChar &rhs) const;
    bool operator<=(const SQLChar &rhs) const;
    bool operator>=(const SQLChar &rhs) const;
    
    bool operator==(const string &rhs) const;
    bool operator!=(const string &rhs) const;
    bool operator<(const string &rhs) const;
    bool operator>(const string &rhs) const;
    bool operator<=(const string &rhs) const;
    bool operator>=(const string &rhs) const; 
    
    friend bool operator==(const string &lhs, const SQLChar &rhs);
    friend bool operator!=(const string &lhs, const SQLChar &rhs);
    friend bool operator<(const string &lhs, const SQLChar &rhs);
    friend bool operator>(const string &lhs, const SQLChar &rhs);
    friend bool operator<=(const string &lhs, const SQLChar &rhs);
    friend bool operator>=(const string &lhs, const SQLChar &rhs);

    friend bool operator==(const Varchar &lhs, const SQLChar &rhs);
    friend bool operator!=(const Varchar &lhs, const SQLChar &rhs);
    friend bool operator<(const Varchar &lhs, const SQLChar &rhs);
    friend bool operator>(const Varchar &lhs, const SQLChar &rhs);
    friend bool operator<=(const Varchar &lhs, const SQLChar &rhs);
    friend bool operator>=(const Varchar &lhs, const SQLChar &rhs);

    friend bool operator==(const SQLChar &lhs, const Varchar &rhs);
    friend bool operator!=(const SQLChar &lhs, const Varchar &rhs);
    friend bool operator<(const SQLChar &lhs, const Varchar &rhs);
    friend bool operator>(const SQLChar &lhs, const Varchar &rhs);
    friend bool operator<=(const SQLChar &lhs, const Varchar &rhs);
    friend bool operator>=(const SQLChar &lhs, const Varchar &rhs);

    friend ostream& operator<<(ostream& os, const SQLChar& self);

  private:
    void enforceLengthInvariant() override;

    string getUnpaddedValue() const;
};

using Types = variant<
  int,        //INTEGER
  int16_t,    //SMALLINT
  int64_t,    //BIGINT
  float,      //FLOAT
  string,     //TEXT
  Varchar,    //VARCHAR
  SQLChar,    //CHAR
  monostate   //NULL
>;

ostream& operator<<(ostream& os, const Types& self);

using Row = unordered_map<string, Types>;
using Column = vector<Types>;


/////////////// Comparator hell part 2 ////////////////////////////
////// Types vs. Anything
template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator==(const StrictTypes& lhs, const T& rhs) {
    if (const T* pval = std::get_if<T>(&lhs)) {
        return *pval == rhs;
    }
    return false;
}

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator!=(const StrictTypes& lhs, const T& rhs) {
    if (const T* pval = std::get_if<T>(&lhs)) {
        return *pval != rhs;
    }
    return true;
}

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator<(const StrictTypes& lhs, const T& rhs) {
    if (const T* pval = std::get_if<T>(&lhs)) {
        return *pval < rhs;
    }
    return false;
}

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator>(const StrictTypes& lhs, const T& rhs) {
    if (const T* pval = std::get_if<T>(&lhs)) {
        return *pval > rhs;
    }
    return false;
}

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator<=(const StrictTypes& lhs, const T& rhs) {
    if (const T* pval = std::get_if<T>(&lhs)) {
        return *pval <= rhs;
    }
    return false;
}

////// Anything vs. Types
template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator==(const T& lhs, const StrictTypes& rhs) {
  return rhs == lhs;
}

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator!=(const T& lhs, const StrictTypes& rhs) {
  return rhs != lhs;
}

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator>(const T& lhs, const StrictTypes& rhs) {
  return rhs > lhs;
}

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator>=(const T& lhs, const StrictTypes& rhs) {
  return rhs >= lhs;
}

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator<=(const T& lhs, const StrictTypes& rhs) {
  return rhs <= lhs;
}

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator<(const T& lhs, const StrictTypes& rhs) {
  return rhs < lhs;
}

////// Anything vs. Monostate
template<typename T>
enable_if_t<!is_same_v<decay_t<T>, monostate>, bool> 
operator==(const monostate&, const T &_) {
  return false;
}
template<typename T>
enable_if_t<!is_same_v<decay_t<T>, monostate>, bool> 
operator!=(const monostate&, const T &_) {
  return true;
}
template<typename T>
enable_if_t<!is_same_v<decay_t<T>, monostate>, bool> 
operator<(const monostate&, const T &_) {
  return false;
}
template<typename T>
enable_if_t<!is_same_v<decay_t<T>, monostate>, bool> 
operator<=(const monostate&, const T &_) {
  return false;
}
template<typename T>
enable_if_t<!is_same_v<decay_t<T>, monostate>, bool>
operator>(const monostate&, const T &_) {
  return false;
}
template<typename T>
enable_if_t<!is_same_v<decay_t<T>, monostate>, bool> 
operator>=(const monostate&, const T &_) {
  return false;
}


////// Monostate vs. Anything
template<typename T>
enable_if_t<!is_same_v<decay_t<T>, monostate>, bool>  
operator==(const T &_, const monostate&) {
  return false;
}
template<typename T>
enable_if_t<!is_same_v<decay_t<T>, monostate>, bool>  
operator!=(const T &_, const monostate&) {
  return true;
}
template<typename T>
enable_if_t<!is_same_v<decay_t<T>, monostate>, bool>  
operator<(const T &_, const monostate&) {
  return false;
}
template<typename T>
enable_if_t<!is_same_v<decay_t<T>, monostate>, bool>  
operator<=(const T &_, const monostate&) {
  return false;
}
template<typename T>
enable_if_t<!is_same_v<decay_t<T>, monostate>, bool>  
operator>(const T &_, const monostate&) {
  return false;
}
template<typename T>
enable_if_t<!is_same_v<decay_t<T>, monostate>, bool>  
operator>=(const T &_, const monostate&) {
  return false;
}



// Monostate vs. Monostate
// bool operator==(const monostate &a, const monostate &b){
//   return true;
// }

// bool operator!=(const monostate &a, const monostate &b){
//   return false;
// }

// bool operator<(const monostate &a, const monostate &b){
//   return false;
// }

// bool operator>(const monostate &a, const monostate &b){
//   return false;
// }

// bool operator<=(const monostate &a, const monostate &b){
//   return false;
// }

// bool operator>=(const monostate &a, const monostate &b){
//   return false;
// }

// Copy-pasting code like this feels AWFUL; think of an alternativ way of doinh this
template <typename Comparator>
struct GenericTypesVisitor {
    Comparator comp;
    bool mismatchValue;

    GenericTypesVisitor(Comparator Comp, bool Value) 
        : comp(Comp), mismatchValue(Value) {}

    template <typename T1, typename T2>
    bool operator()(const T1& lhs, const T2& rhs) const {
        // USE THIS CHAINED STRUCTURE
        if constexpr (std::is_same_v<T1, T2>) {
            return comp(lhs, rhs);
        }
        else if constexpr (
            (std::is_same_v<T1, Varchar> && std::is_same_v<T2, SQLChar>) ||
            (std::is_same_v<T1, SQLChar> && std::is_same_v<T2, Varchar>)
        ) {
            return comp(lhs, rhs);
        }
        else if constexpr (std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>) {
            return comp(static_cast<double>(lhs), static_cast<double>(rhs));
        }
        else {
            return mismatchValue;
        }
    }
};

inline bool operator==(const Types& lhs, const Types& rhs) {
    return std::visit(GenericTypesVisitor{std::equal_to<>{}, false}, lhs, rhs);
}

inline bool operator!=(const Types& lhs, const Types& rhs) {
    return std::visit(GenericTypesVisitor{std::not_equal_to<>{}, true}, lhs, rhs);
}

inline bool operator<(const Types& lhs, const Types& rhs) {
    return std::visit(GenericTypesVisitor{std::less<>{}, false}, lhs, rhs);
}

inline bool operator>(const Types& lhs, const Types& rhs) {
    return std::visit(GenericTypesVisitor{std::greater<>{}, false}, lhs, rhs);
}

inline bool operator<=(const Types& lhs, const Types& rhs) {
    return std::visit(GenericTypesVisitor{std::less_equal<>{}, false}, lhs, rhs);
}

inline bool operator>=(const Types& lhs, const Types& rhs) {
    return std::visit(GenericTypesVisitor{std::greater_equal<>{}, false}, lhs, rhs);
}