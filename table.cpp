#include "table.h"


///////////////////////////// static values //////////////////////////////////////

const string Table::filepathPrefix = "./database/";

const int Alias::ORIGINAL_NAME_LOCATION = 0;

///////////////////////////// end static values //////////////////////////////////////



///////////////////// helper classes function /////////////////////////////////////

Alias::Alias(const string &name) : names(vector<string>(0)) {
  names.push_back(name);
  currentAlias = names.size() - 1;
}

string Alias::getOriginalName() const {
  return names[Alias::ORIGINAL_NAME_LOCATION];
}

bool Alias::containsName(const string &name) const {
  return (std::find(names.begin(), names.end(), name) != names.end());
}

Aliases::Aliases() {};

void Aliases::push(const Alias &alias) {
  names.push_back(alias);
}

void Aliases::push(const string &trueName, const string &newName) {
  for (Alias a : names){
    if (a.names[Alias::ORIGINAL_NAME_LOCATION] == trueName) {
      a.names.push_back(newName);
      return;
    }
  }
}

string Aliases::operator[] (const string &name) const {
  for (Alias a : names){
    if (a.containsName(name)) {
      return a.getOriginalName();
    }
  }

  cerr << "Alias or column name could not be found" << endl;
  exit(6);
}
///////////////////// end helper classes function /////////////////////////////////////



//////////////////////////// Table //////////////////////////////////////

////// Table creation and modification functions

Table::Table () {};

void Table::addColumn(const Column formedColumn, const string &name) {
  aliases.push(Alias(name));

  table[name] = formedColumn;
}

void Table::addColumn(const string &name, Datatypes type, string &unprocessedValues) {
  Column formedColumn = commaSeparatedToColumn(type, unprocessedValues);

  addColumn(formedColumn, name);
}

void Table::deleteColumn(const string &name) {
  table.erase(name);
}

void Table::renameColumn(const string &oldName, const string &newName) {
  table[newName];
  swap(table[oldName], table[newName]);
  table.erase(oldName);
}

Column Table::commaSeparatedToColumn(const Datatypes type, string &values) {
  Column col(type);

  auto begin = values.begin(), end = std::find(values.begin(), values.end(), ',');

  // Want everything to be self-contained in the loop. We know it is done when 
  // there are no more , to find, so begin will be the same as end.
  while (true) {
    Types value = convertToVariant(string(begin, end), type);

    col.push(value);

    begin = end;
    if (begin == values.end()) break;
    end = std::find(begin + 1, values.end(), ',');
  }

  return col;
}