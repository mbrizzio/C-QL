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
#include <cmath>

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
    int getLength() const;

    string value;
    int length;
    
    // Helper function to check for null values
    template <typename Comparator>
    bool comparatorHelper
    (const string &lhs, const string &rhs, const Comparator comp) const {
      return comp(lhs, rhs);
    }

    explicit operator string () const;

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

enum class DateComponents{
  DAYS,
  MONTHS, 
  YEARS,
  WEEKS,
  QUARTERS, 
  DAYOFYEAR
};

class Date {
  public:
    Date();

    Date(const int Year, const int Month, const int Day);

    // YYYY-MM-DD
    Date(const string &YearMonthDay);

    // Days since 0001-01-01
    Date(const int Epoch);

    bool operator==(const Date &rhs) const;
    bool operator!=(const Date &rhs) const;
    bool operator<(const Date &rhs) const;
    bool operator>(const Date &rhs) const;
    bool operator<=(const Date &rhs) const;
    bool operator>=(const Date &rhs) const;

    Date operator-(const Date &rhs) const;
    Date operator+(const Date &rhs) const;
    void operator-=(const Date &rhs);
    void operator+=(const Date &rhs);

    explicit operator int() const;

    friend ostream& operator<<(ostream& os, const Date& self);

    Date dateAdd(int difference, const DateComponents mode) const;
    Date dateSub(int difference, const DateComponents mode) const;

    int extract(const DateComponents mode) const;

    int day;
    int month;
    int year;

    int epoch; // Days since start = 0001-01-01
    
  private:
    
    void enforceDateInvariants() const;

    bool isLeapYear() const;
    bool isLeapYear(int const currentYear) const;
    
    // IMPORTANT: Does not consider the current year!
    // if year = 4, there have been 0 up to today
    int leapDaysUpToCurrentYear() const;
    int leapDaysUpToCurrentYear(int const currentYear) const;

    // Takes the current epoch, and overwrites year, month, day
    // with what they should be
    void epochToDate();

    // Takes the current year, month, day, and overwrites what
    // the epoch should be
    void dateToEpoch();
};

Date getCurrentDate();

enum class TimeComponents {
  FRACTIONS,
  SECONDS,
  MINUTES,
  HOURS
};

class Time {
  public:
    Time();
    Time(int Precision);

    Time(double Duration, int Precision=6);

    Time(const string &HHMMSS, int Precision=6);

    Time(int Hour, int Minute, int Second, int Fraction=0, int Precision=6);

    bool operator==(const Time &rhs) const;
    bool operator!=(const Time &rhs) const;
    bool operator<(const Time &rhs) const;
    bool operator>(const Time &rhs) const;
    bool operator<=(const Time &rhs) const;
    bool operator>=(const Time &rhs) const;

    friend ostream& operator<<(ostream& os, const Time &self);

    Time timeAdd(int difference, const TimeComponents mode) const;
    Time timeSub(int difference, const TimeComponents mode) const;

    // returns absolute value difference, since negative time is not supported
    Time timeDiff(const Time &rhs) const;

    double extract(const TimeComponents mode) const;

    u_int precision = 0;

    u_int fraction = 0;
    u_int second;
    u_int minute;
    u_int hour;

    double duration;

  private:
    void enforceTimeInvariants();

    void timeToDuration();

    void durationToTime();
};

Time getCurrentTime();

enum class DatetimeComponents {
  DATES,
  TIMES
};

class Datetime {
  public:
    Datetime();

    Datetime(const Date Datepart, const Time Timepart);

    Datetime(const string &Datepart, const string &Timepart);
    Datetime(const string &full); //support YYYY-MM-DDThh:mm:ss.dddddd

    bool operator==(const Datetime &rhs) const;
    bool operator!=(const Datetime &rhs) const;
    bool operator<(const Datetime &rhs) const;
    bool operator>(const Datetime &rhs) const;
    bool operator<=(const Datetime &rhs) const;
    bool operator>=(const Datetime &rhs) const;

    friend ostream& operator<<(ostream& os, const Datetime &self);

    Datetime datetimeAdd(const double &difference, const DateComponents &mode) const;
    Datetime datetimeAdd(const double &difference, const TimeComponents &mode) const;

    Datetime datetimeSub(const double &difference, const DateComponents &mode) const;
    Datetime datetimeSub(const double &difference, const TimeComponents &mode) const;

    double extract(const TimeComponents &mode) const; // Extract HH, MM, SS, DDDDDD
    int extract(const DateComponents &mode) const; // Extract YYYY, DD, MM
    // WARNING: This extarct the DURATION; the actual date/time must be constructed from it
    double extract(const DatetimeComponents &mode) const;
    
    Date date;
    explicit operator Date () const;
    
    Time time;
    explicit operator Time () const;
};

int dateDiff(const Datetime &rhs, const Datetime &lhs, const DateComponents &mode);
int dateDiff(const Datetime &rhs, const Datetime &lhs, const TimeComponents &mode);

/////////////////// Comparison functor and type traits //////////////

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
  bool,       //BOOL
  int,        //INTEGER
  int16_t,    //SMALLINT
  int64_t,    //BIGINT
  float,      //FLOAT
  string,     //TEXT
  Varchar,    //VARCHAR
  SQLChar,    //CHAR
  Date,       //DATE 
  Time,       //TIME  
  Datetime,   //DATETIME  
  monostate   //NULL
>;

enum class Datatypes {
  BOOL,
  INT,
  SMALLINT,
  BIGINT,
  FLOAT,
  TEXT,
  VARCHAR,
  CHAR,
  DATE,
  TIME,
  DATETIME,
  NULLVALUE
};

bool isNumeric(const Datatypes type) {
  return type == Datatypes::INT || type == Datatypes::SMALLINT || 
         type == Datatypes::BIGINT || type == Datatypes::FLOAT;
}

bool isString(const Datatypes type) {
  return type == Datatypes::TEXT || type == Datatypes::VARCHAR || 
         type == Datatypes::CHAR;
}

string getString(const Types &value){
  return std::visit([] (auto &value) -> string {
    using Type = decay_t<decltype(value)>;

    if constexpr (is_same_v<Type, string> || is_same_v<Type, Varchar> 
                  || is_same_v<Type, SQLChar>){
      return static_cast<string>(value);
    }

    cerr << "Value with no conversion to string passed" << endl;
    exit(9);
  }, value);
}

template <typename T>
enable_if_t<is_arithmetic_v<T>, T>
getNumeric(const Types &value) {
  return std::visit([] (auto &value) -> T {
    using Type = decay_t<decltype(value)>;

    if constexpr (is_same_v<Type, int> || is_same_v<Type, int16_t> ||
                  is_same_v<Type, int64_t> || is_same_v<Type, float>){
      return static_cast<T>(value);
    }

    else if constexpr (is_same_v<Type, string> || is_same_v<Type, Varchar> 
                  || is_same_v<Type, SQLChar>){
      return static_cast<T>(stod(static_cast<string>(value)));
    }

    cerr << "Value with no conversion to double passed" << endl;
    exit(9); 
  }, value);
}

// Arcane C++ syntax; try to understand variadic types later
template <class... Ts>
struct typesDatatypesConversionHelper : Ts... {using Ts::operator()...;};
template <class... Ts>
typesDatatypesConversionHelper(Ts...) -> typesDatatypesConversionHelper<Ts...>;

Datatypes getType(const Types &value) {
  return std::visit(typesDatatypesConversionHelper{
    [](monostate type) {return Datatypes::NULLVALUE;},
    [](int type) {return Datatypes::INT;},
    [](int16_t type) {return Datatypes::SMALLINT;},
    [](int64_t type) {return Datatypes::BIGINT;},
    [](float type) {return Datatypes::FLOAT;},
    [](Varchar type) {return Datatypes::VARCHAR;},
    [](SQLChar type) {return Datatypes::CHAR;},
    [](string type) {return Datatypes::TEXT;},
    [](Date type) {return Datatypes::DATE;},
    [](Time type) {return Datatypes::TIME;},
    [](Datetime type) {return Datatypes::DATETIME;},
    [](bool type) {return Datatypes::BOOL;}

  }, value);
}

ostream& operator<<(ostream& os, const Types& self);

using Row = unordered_map<string, Types>;
using ColumnOld = vector<Types>;


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
