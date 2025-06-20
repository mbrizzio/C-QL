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

string Varchar::getValue() const {
  return value;
}
/////////////////////////////// End Varchar //////////////////////////////



////////////////////////////// SQLChar //////////////////////////////////

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

string SQLChar::getUnpaddedValue() const{
  auto end = value.end();
  auto begin = value.begin();

  while (end != begin && *(--end) == ' ');

  return string(begin, ++end);
}
///////////////////////// End SQLChar //////////////////////////////////



/////////////////////// os operators (all classes) //////////////////////

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


////// Varchar vs. String
bool Varchar::operator==(const string &rhs){
  return comparatorHelper(value, rhs, std::equal_to<>{});
} 

bool Varchar::operator!=(const string &rhs){
  return comparatorHelper(value, rhs, std::not_equal_to<>{});
}

bool Varchar::operator<(const string &rhs){
  return comparatorHelper(value, rhs, std::less<>{});
}

bool Varchar::operator>(const string &rhs){
  return comparatorHelper(value, rhs, std::greater<>{});
}

bool Varchar::operator<=(const string &rhs){
  return comparatorHelper(value, rhs, std::less_equal<>{});
}

bool Varchar::operator>=(const string &rhs){
  return comparatorHelper(value, rhs, std::greater_equal<>{});
} 


///// String vs. Varchar
bool operator==(const string &lhs, const Varchar &rhs){
  return rhs.comparatorHelper(lhs, rhs.value, std::equal_to<>{});
}

bool operator!=(const string &lhs, const Varchar &rhs){
  return rhs.comparatorHelper(lhs, rhs.value, std::not_equal_to<>{});
}

bool operator<(const string &lhs, const Varchar &rhs){
  return rhs.comparatorHelper(lhs, rhs.value, std::less<>{});
}

bool operator>(const string &lhs, const Varchar &rhs){
  return rhs.comparatorHelper(lhs, rhs.value, std::greater<>{});
}

bool operator<=(const string &lhs, const Varchar &rhs){
  return rhs.comparatorHelper(lhs, rhs.value, std::less_equal<>{});
}

bool operator>=(const string &lhs, const Varchar &rhs){
  return rhs.comparatorHelper(lhs, rhs.value, std::greater_equal<>{});
}


///// SQLChar vs. SQLChar
bool SQLChar::operator==(const SQLChar &rhs){
  return comparatorHelper(value, rhs.value, std::equal_to<>{});
}

bool SQLChar::operator!=(const SQLChar &rhs){
  return comparatorHelper(value, rhs.value, std::not_equal_to<>{});
}

bool SQLChar::operator<(const SQLChar &rhs){
  return comparatorHelper(value, rhs.value, std::less<>{});
}

bool SQLChar::operator>(const SQLChar &rhs){
  return comparatorHelper(value, rhs.value, std::greater<>{});
}

bool SQLChar::operator<=(const SQLChar &rhs){
  return comparatorHelper(value, rhs.value, std::less_equal<>{});
}

bool SQLChar::operator>=(const SQLChar &rhs){
  return comparatorHelper(value, rhs.value, std::greater_equal<>{});
}


///// String vs. SQLChar
bool operator==(const string &lhs, const SQLChar &rhs){
  return rhs.comparatorHelper(lhs, rhs.getUnpaddedValue(), std::equal_to<>{});
}

bool operator!=(const string &lhs, const SQLChar &rhs){
  return rhs.comparatorHelper(lhs, rhs.getUnpaddedValue(), std::not_equal_to<>{});
}

bool operator<(const string &lhs, const SQLChar &rhs){
  return rhs.comparatorHelper(lhs, rhs.getUnpaddedValue(), std::less<>{});
}

bool operator>(const string &lhs, const SQLChar &rhs){
  return rhs.comparatorHelper(lhs, rhs.getUnpaddedValue(), std::greater<>{});
}

bool operator<=(const string &lhs, const SQLChar &rhs){
  return rhs.comparatorHelper(lhs, rhs.getUnpaddedValue(), std::less_equal<>{});
}

bool operator>=(const string &lhs, const SQLChar &rhs){
  return rhs.comparatorHelper(lhs, rhs.getUnpaddedValue(), std::greater_equal<>{});
}


////// SQLChar vs. String
bool SQLChar::operator==(const string &rhs){
  return comparatorHelper(getUnpaddedValue(), rhs, std::equal_to<>{});
} 

bool SQLChar::operator!=(const string &rhs){
  return comparatorHelper(getUnpaddedValue(), rhs, std::not_equal_to<>{});
}

bool SQLChar::operator<(const string &rhs){
  return comparatorHelper(getUnpaddedValue(), rhs, std::less<>{});
}

bool SQLChar::operator>(const string &rhs){
  return comparatorHelper(getUnpaddedValue(), rhs, std::greater<>{});
}

bool SQLChar::operator<=(const string &rhs){
  return comparatorHelper(getUnpaddedValue(), rhs, std::less_equal<>{});
}

bool SQLChar::operator>=(const string &rhs){
  return comparatorHelper(getUnpaddedValue(), rhs, std::greater_equal<>{});
} 



///// Varchar vs. SQLChar
bool operator==(const Varchar &lhs, const SQLChar &rhs){
  return rhs.comparatorHelper(lhs.value, rhs.getUnpaddedValue(), std::equal_to<>{});
}

bool operator!=(const Varchar &lhs, const SQLChar &rhs){
  return rhs.comparatorHelper(lhs.value, rhs.getUnpaddedValue(), std::not_equal_to<>{});
}

bool operator<(const Varchar &lhs, const SQLChar &rhs){
  return rhs.comparatorHelper(lhs.value, rhs.getUnpaddedValue(), std::less<>{});
}

bool operator>(const Varchar &lhs, const SQLChar &rhs){
  return rhs.comparatorHelper(lhs.value, rhs.getUnpaddedValue(), std::greater<>{});
}

bool operator<=(const Varchar &lhs, const SQLChar &rhs){
  return rhs.comparatorHelper(lhs.value, rhs.getUnpaddedValue(), std::less_equal<>{});
}

bool operator>=(const Varchar &lhs, const SQLChar &rhs){
  return rhs.comparatorHelper(lhs.value, rhs.getUnpaddedValue(), std::greater_equal<>{});
}

///// SQLChar vs. Varchar
bool operator==(const SQLChar &lhs, const Varchar &rhs){
  return rhs.comparatorHelper(lhs.getUnpaddedValue(), rhs.value, std::equal_to<>{});
}

bool operator!=(const SQLChar &lhs, const Varchar &rhs){
  return rhs.comparatorHelper(lhs.getUnpaddedValue(), rhs.value, std::not_equal_to<>{});
}

bool operator<(const SQLChar &lhs, const Varchar &rhs){
  return rhs.comparatorHelper(lhs.getUnpaddedValue(), rhs.value, std::less<>{});
}

bool operator>(const SQLChar &lhs, const Varchar &rhs){
  return rhs.comparatorHelper(lhs.getUnpaddedValue(), rhs.value, std::greater<>{});
}

bool operator<=(const SQLChar &lhs, const Varchar &rhs){
  return rhs.comparatorHelper(lhs.getUnpaddedValue(), rhs.value, std::less_equal<>{});
}

bool operator>=(const SQLChar &lhs, const Varchar &rhs){
  return rhs.comparatorHelper(lhs.getUnpaddedValue(), rhs.value, std::greater_equal<>{});
}
