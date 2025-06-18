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

class Varchar{
  public:
    // Constructors
    Varchar();
    Varchar(int Length);
    Varchar(int Length, std::variant<std::string, std::monostate> Value);

    Varchar(Table* table, std::string name);
    Varchar(Table* table, std::string name, std::variant<std::string, std::monostate> Value);

    string getValue();

    int length;
    std::variant<std::string, std::monostate> value;

    // Helper function to check for null values
    bool inline containsNull
    (const std::variant<std::string, std::monostate> &lhs, const Varchar &rhs){
      if (holds_alternative<monostate>(lhs) || holds_alternative<monostate>(rhs.value)){
        return true;
      }

      return false;
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
  SQLChar,       //CHAR
  monostate   //NULL
>;

using Row = unordered_map<string, Types>;
using Column = vector<Types>;