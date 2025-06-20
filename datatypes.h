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

constexpr monostate Null{};

class Table;

// Should just remove NULL support from this class...
// Or maybe keep it this way for now and see if it truly becomes a problem
// Make subsequent classes that don't support NULL and test those
class Varchar{
  public:
    // Constructors
    Varchar();
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