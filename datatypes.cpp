#pragma once
#include "datatypes.h"
#include "table.h"


Varchar::Varchar() {
  length = 100;
  value = Null;
}

Varchar::Varchar(int Length) : length(Length) {
  value = Null;

  enforceLengthInvariant();
};

Varchar::Varchar(int Length, variant<string, monostate> Value) : length(Length),
  value(Value) {
    enforceLengthInvariant();
  };

Varchar::Varchar(Table* table, std::string name) {
  length = table->getCharTypeLength(name);
  value = Null; 

  enforceLengthInvariant();
}

Varchar::Varchar(Table* table, string name, variant<string, monostate> Value) : value(Value) {
  length = table->getCharTypeLength(name);

  enforceLengthInvariant();
}

void Varchar::enforceLengthInvariant(){
  if (holds_alternative<monostate>(value)) return;

  if (get<string>(value).size() > length){
    cerr << "Length of VARCHAR exceeds the stipulated length" << endl;
    exit(3);
  }
}

string Varchar::getValue(){
  if (holds_alternative<monostate>(value)) return "NULL";

  return get<string>(value);
}

SQLChar::SQLChar() : Varchar() {};

SQLChar::SQLChar(int Length) : Varchar(Length) {
  enforceLengthInvariant();
}

SQLChar::SQLChar(int Length, variant<string, monostate> Value) : Varchar(Length, Value) {
  enforceLengthInvariant();
}

SQLChar::SQLChar(Table* table, std::string name) : Varchar(table, name){
  enforceLengthInvariant();
}

SQLChar::SQLChar(Table* table, string name, variant<string, monostate> Value) : 
Varchar(table, name, Value) {
  enforceLengthInvariant();
}

void SQLChar::enforceLengthInvariant(){
  if (holds_alternative<monostate>(value)) return;

  if (get<string>(value).size() > length){
    cerr << "Length of VARCHAR exceeds the stipulated length" << endl;
    exit(3);
  }

  if (get<string>(value).size() < length){
    int padLength = length - get<string>(value).size();
    string padding(padLength, ' ');

    get<string>(value) += padding;
  }
}


// Comparator hell
bool Varchar::operator==(const Varchar &rhs){
  if (containsNull(value, rhs)){
    return false;
  }

  return get<string>(value) == get<string>(rhs.value);
}

bool Varchar::operator!=(const Varchar &rhs){
  if (containsNull(value, rhs)){
      return false;
    }

  return get<string>(value) != get<string>(rhs.value);
}

bool Varchar::operator<(const Varchar &rhs){
  if (containsNull(value, rhs)){
    return false;
  }

  return get<string>(value) < get<string>(rhs.value);
}
bool Varchar::operator>(const Varchar &rhs){
  if (containsNull(value, rhs)){
    return false;
  }

  return get<string>(value) > get<string>(rhs.value);
}

bool Varchar::operator<=(const Varchar &rhs){
  if (containsNull(value, rhs)){
    return false;
  }

  return get<string>(value) <= get<string>(rhs.value);
}

bool Varchar::operator>=(const Varchar &rhs){
  if (containsNull(value, rhs)){
    return false;
  }

  return get<string>(value) >= get<string>(rhs.value);
}