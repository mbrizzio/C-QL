#pragma once
#include "datatypes.h"
#include "table.h"

/////////////////////////////////// Varchar /////////////////////////////
Varchar::Varchar() {
  length = 100;
  value = "";
}

Varchar::Varchar(int Length) : length(Length) {
  value = "";

  enforceLengthInvariant();
};

Varchar::Varchar(int Length, string Value) : length(Length),
  value(Value) {
    enforceLengthInvariant();
  };

Varchar::Varchar(Table* table, std::string name) {
  length = table->getCharTypeLength(name);
  value = ""; 

  enforceLengthInvariant();
}

Varchar::Varchar(Table* table, string name, string Value) : value(Value) {
  length = table->getCharTypeLength(name);

  enforceLengthInvariant();
}

void Varchar::enforceLengthInvariant(){
  if (value.size() > length){
    cerr << "Length of VARCHAR exceeds the stipulated length" << endl;
    exit(3);
  }
}
/////////////////////////////// End Varchar //////////////////////////////



////////////////////////////// SQLChar //////////////////////////////////
string Varchar::getValue(){
  return value;
}

SQLChar::SQLChar() : Varchar() {};

SQLChar::SQLChar(int Length) : Varchar(Length) {
  enforceLengthInvariant();
}

SQLChar::SQLChar(int Length, string Value) : Varchar(Length, Value) {
  enforceLengthInvariant();
}

SQLChar::SQLChar(Table* table, std::string name) : Varchar(table, name){
  enforceLengthInvariant();
}

SQLChar::SQLChar(Table* table, string name, string Value) : 
Varchar(table, name, Value) {
  enforceLengthInvariant();
}

void SQLChar::enforceLengthInvariant(){
  if (value.size() > length){
    cerr << "Length of VARCHAR exceeds the stipulated length" << endl;
    exit(3);
  }

  if (value.size() < length){
    int padLength = length - value.size();
    string padding(padLength, ' ');

    value += padding;
  }
}
///////////////////////// End SQLChar //////////////////////////////////



/////////////////////// Misc. Operators ////////////////////////////////

ostream& operator<<(ostream& os, const Varchar& self){
  os << self.value;
  return os;
}

ostream& operator<<(ostream& os, const SQLChar& self){
  os << self.value;
  return os;
}

/////////////////////// Comparator hell (all classes) ///////////////////

////// Varchar vs. Varchar
bool Varchar::operator==(const Varchar &rhs){
  return comparatorHelper(value, rhs.value, std::equal_to<>{});
}

bool Varchar::operator!=(const Varchar &rhs){
  return comparatorHelper(value, rhs.value, std::not_equal_to<>{});
}

bool Varchar::operator<(const Varchar &rhs){
  return comparatorHelper(value, rhs.value, std::less<>{});
}

bool Varchar::operator>(const Varchar &rhs){
  return comparatorHelper(value, rhs.value, std::greater<>{});
}

bool Varchar::operator<=(const Varchar &rhs){
  return comparatorHelper(value, rhs.value, std::less_equal<>{});
}

bool Varchar::operator>=(const Varchar &rhs){
  return comparatorHelper(value, rhs.value, std::greater_equal<>{});
}