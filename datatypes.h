#pragma once
#include <unordered_map>
#include <vector>
#include <variant>
#include <cassert>
#include <string>
#include <iostream>
#include <compare>

using namespace std;

using nullableString = std::variant<std::string, std::monostate>;

constexpr monostate Null{};

class Table;

class Varchar{
  public:
    // Constructors
    Varchar();
    Varchar(int Length);
    Varchar(int Length, nullableString Value);

    Varchar(Table* table, std::string name);
    Varchar(Table* table, std::string name, nullableString Value);

    string getValue();

    int length;
    nullableString value;

    // Helper function to check for null values
    template <typename Comparator>
    bool inline comparatorHelper
    (const nullableString &lhs, const nullableString &rhs, const Comparator comp){
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
    SQLChar(int Length, variant<string, monostate> Value);

    SQLChar(Table* table, std::string name);
    SQLChar(Table* table, string name, variant<string, monostate> Value);

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