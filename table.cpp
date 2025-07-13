#include "table.h"


///////////////////////////// static values //////////////////////////////////////

const string Table::filepathPrefix = "./database/";

const int Alias::ORIGINAL_NAME_LOCATION = 0;

///////////////////////////// end static values //////////////////////////////////////



///////////////////// helper classes function /////////////////////////////////////

Alias::Alias(const string &name) : names(vector<string>(0)) {
  names.push_back(name);
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
    if (find(a.names.begin(), a.names.end(), name) != a.names.end()) {
      return a.names[Alias::ORIGINAL_NAME_LOCATION];
    }
  }

  cerr << "Alias or column name could not be found" << endl;
  exit(6);
}
///////////////////// end helper classes function /////////////////////////////////////