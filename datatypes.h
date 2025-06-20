#pragma once
#include <unordered_map>
#include <vector>
#include <variant>
#include <cassert>
#include <string>
#include <iostream>
#include <compare>
#include <iomanip>
#include <type_traits>

using namespace std;

#define STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes) \
  std::enable_if_t< \
    std::is_same_v<std::decay_t<StrictTypes>, Types> && \
    !std::is_same_v<std::decay_t<T>, Types>, \
  bool>

#define STRICTLY_TYPES(T) \
  enable_if_t<is_same_v<decay_t<T>, Types>, bool>


constexpr monostate Null{};

class Table;

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

constexpr array<int, 12> daysPerMonth = 
          {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

enum Components{
  DAYS,
  MONTHS, 
  YEARS
};

class Date {
  public:
    Date(const int Year, const int Month, const int Day);

    // YYYY-MM-DD
    Date(string &YearMonthDay);

    // Days since 0001-01-01
    Date(const int Epoch);

    bool operator==(const Date &rhs) const;
    bool operator!=(const Date &rhs) const;
    bool operator<(const Date &rhs) const;
    bool operator>(const Date &rhs) const;
    bool operator<=(const Date &rhs) const;
    bool operator>=(const Date &rhs) const;

    bool operator-(const Date &rhs) const;
    bool operator+(const Date &rhs) const;
    bool operator-=(const Date &rhs);
    bool operator+=(const Date &rhs);

    explicit operator int() const;

    friend ostream& operator<<(ostream& os, const Date& self);

    Date dateAdd(const int rhs, const Components mode);
    Date dateSub(const int rhs, const Components mode);

    int day;
    int month;
    int year;
    int epoch; // Days since start = 0001-01-01

  private:
    void enforceDateInvariants() const;

    bool isLeapYear() const;
    
    // IMPORTANT: Does not consider the current year!
    // if year = 4, there have been 0 up to today
    int leapDaysUpToCurrentYear() const;

    // Takes the current epoch, and overwrites year, month, day
    // with what they should be
    void epochToDate();

    // Takes the current year, month, day, and overwrites what
    // the epoch should be
    void dateToEpoch();
};

// Defining a type trait for string-like classes
template <typename T>
struct is_string : std::false_type {};

template <typename T>
constexpr bool is_string_v = is_string<T>::value;

template <>
struct is_string<string> : std::true_type {};

template<>
struct is_string<Varchar> : std::true_type{};

template<>
struct is_string<SQLChar> : std::true_type{};

// Defining a functor that can handle mixed-type comparisons
template <typename Comparator>
struct GenericTypesVisitor {
  GenericTypesVisitor(Comparator Comp, bool Value) 
    : comp(Comp), mismatchValue(Value) {}

  template <typename T1, typename T2>
  bool operator()(const T1& lhs, const T2& rhs) const {
    if constexpr (std::is_same_v<T1, T2>) {
      return comp(lhs, rhs);
    }
    else if constexpr (is_string_v<T1> && is_string_v<T2>) {
      return comp(lhs, rhs);
    }
    else if constexpr (std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>) {
      return comp(static_cast<double>(lhs), static_cast<double>(rhs));
    }
    else {
      return mismatchValue;
    }
  }

  Comparator comp;
  bool mismatchValue;
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
////// Types vs. Types
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


////// Types vs. Anything
template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator==(const StrictTypes& lhs, const T& rhs) {
  return lhs == static_cast<Types>(rhs);
}

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator!=(const StrictTypes& lhs, const T& rhs) {
  return lhs != static_cast<Types>(rhs);
}

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator<(const StrictTypes& lhs, const T& rhs) {
  return lhs < static_cast<Types>(rhs);
}

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator>(const StrictTypes& lhs, const T& rhs) {
  return lhs > static_cast<Types>(rhs);
}

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator<=(const StrictTypes& lhs, const T& rhs) {
  return lhs <= static_cast<Types>(rhs);
}

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator>=(const StrictTypes& lhs, const T& rhs) {
  return lhs >= static_cast<Types>(rhs);
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
  return rhs < lhs;
}

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator>=(const T& lhs, const StrictTypes& rhs) {
  return rhs <= lhs;
}

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator<=(const T& lhs, const StrictTypes& rhs) {
  return rhs >= lhs;
}

template<typename T, typename StrictTypes>
STRICTLY_NOT_TYPES_AND_STRICTLY_TYPES(T, StrictTypes)
operator<(const T& lhs, const StrictTypes& rhs) {
  return rhs > lhs;
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
