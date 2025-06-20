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

    bool operator==(const Varchar &rhs);
    bool operator!=(const Varchar &rhs);
    bool operator<(const Varchar &rhs);
    bool operator>(const Varchar &rhs);
    bool operator<=(const Varchar &rhs);
    bool operator>=(const Varchar &rhs); 

    bool operator==(const string &rhs);
    bool operator!=(const string &rhs);
    bool operator<(const string &rhs);
    bool operator>(const string &rhs);
    bool operator<=(const string &rhs);
    bool operator>=(const string &rhs); 

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
    bool operator==(const SQLChar &rhs);
    bool operator!=(const SQLChar &rhs);
    bool operator<(const SQLChar &rhs);
    bool operator>(const SQLChar &rhs);
    bool operator<=(const SQLChar &rhs);
    bool operator>=(const SQLChar &rhs);
    
    bool operator==(const string &rhs);
    bool operator!=(const string &rhs);
    bool operator<(const string &rhs);
    bool operator>(const string &rhs);
    bool operator<=(const string &rhs);
    bool operator>=(const string &rhs); 
    
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
    template<typename StrictVarchar, typename StrictSQLChar>
    std::enable_if_t< 
      std::is_same_v<std::decay_t<StrictVarchar>, Varchar> && 
      std::is_same_v<std::decay_t<StrictSQLChar>, SQLChar>, 
    bool> 
    friend operator>=(const StrictVarchar &lhs, const StrictSQLChar &rhs);

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

////// Types vs. Anything
template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator==(const StrictTypes& lhs, const T& rhs);

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator!=(const StrictTypes& lhs, const T& rhs);

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator<(const StrictTypes& lhs, const T& rhs);

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator<=(const StrictTypes& lhs, const T& rhs);

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator>=(const StrictTypes& lhs, const T& rhs);

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator>(const StrictTypes& lhs, const T& rhs);

////// Anything vs. Types
template<typename T, typename StrictTypes> 
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes) 
operator==(const T& lhs, const StrictTypes& rhs);

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator!=(const T& lhs, const StrictTypes& rhs);

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator>(const T& lhs, const StrictTypes& rhs);

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator>=(const T& lhs, const StrictTypes& rhs);

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator<=(const T& lhs, const StrictTypes& rhs);

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator<(const T& lhs, const StrictTypes& rhs);


////// Types vs. Types
template<typename T> 
STRICTLY_TYPES(T) 
operator==(const T& lhs, const T& rhs);

template<typename T>
STRICTLY_TYPES(T)
operator!=(const T& lhs, const T& rhs);

template<typename T>
STRICTLY_TYPES(T)
operator<(const T& lhs, const T& rhs);

template<typename T>
STRICTLY_TYPES(T)
operator<=(const T& lhs, const T& rhs);

template<typename T>
STRICTLY_TYPES(T)
operator>(const T& lhs, const T& rhs);

template<typename T>
STRICTLY_TYPES(T)
operator>=(const T& lhs, const T& rhs);


ostream& operator<<(ostream& os, const Types& self);

using Row = unordered_map<string, Types>;
using Column = vector<Types>;