#pragma once
#include <unordered_map>
#include <vector>
#include <variant>
#include <cassert>
#include <string>
#include <iostream>
#include <compare>

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

    string getValue();

    string value;
    int length;
    
    // Helper function to check for null values
    template <typename Comparator>
    bool inline comparatorHelper
    (const string &lhs, const nullableString &rhs, const Comparator comp){
      if (holds_alternative<monostate>(lhs) || holds_alternative<monostate>(rhs)){
        return false;
      }

      return comp(get<string>(lhs), get<string>(rhs));
    }

    bool operator==(const Varchar &rhs);
    bool operator!=(const Varchar &rhs);
    bool operator<(const Varchar &rhs);
    bool operator>(const Varchar &rhs);
    bool operator<=(const Varchar &rhs);
    bool operator>=(const Varchar &rhs);

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

  private:
    void enforceLengthInvariant() override;
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

using Row = unordered_map<string, Types>;
using Column = vector<Types>;