#pragma once
#include "datatypes.h"
#include "table.h"
#include <iostream>
#include <sstream>


using namespace std;

// Public

Table::Table() {}

// Add a new column to the table
// Ensures the datatype is supported
void Table::insertColumn(const string name, const string dataType){    
  assert(validDatatype(dataType));

  columns[name] = {};
  columnNames.push_back(name);

  ++numCols;

  // Populate the column with nulls for the number of rows in table
  for (int i = 0; i < numRows; ++i){
    columns[name].push_back(Null);
  }
}

// Add a new char/varchar column to the table
void Table::insertColumn(const string name, const string dataType, const int length){    
  assert(dataType == "CHAR" || dataType == "VARCHAR");

  columns[name] = {};
  columnNames.push_back(name);
  charTypeColumnLengths[name] = length;

  ++numCols;

  // Populate the column with nulls for the number of rows in table
  for (int i = 0; i < numRows; ++i){
    columns[name].push_back(Null);
  }
}

// Add a new row to the table
// Skips incorrectly names/non-existent columns
// Columns not specified get initialized as NULL
void Table::insertRow(Row row){
  for (string column : columnNames){
    if (row.find(column) == row.end()) row[column] = Null;

    columns[column].push_back(row[column]);
  }

  order.push_back(numRows);
  ++numRows;
}


int Table::getCharTypeLength(const string name) const {
  assert(columnNameExists(name));

  return charTypeColumnLengths.at(name);
}

// Prints out the existing SQL table
void Table::print() {
  tableValidityCheck();
  bool hadToCutOutValue = 0;
  string delimiter = " | ", numHeader = "#   ";

  // Header
  cout << numHeader;

  unordered_map<string, int> width;
  int totalSize = numHeader.size();
  for (string name : columnNames){
    width[name] = name.size();
    totalSize += name.size();
    cout << delimiter << name;
  }
  cout << "\n";

  // Separator
  string separator(totalSize, '='); // Why is - not monospace...
  cout << separator << "\n";

  // Rows, in order
  for (int i : order){
    int length = numHeader.size() - to_string(i).size();
    string whitespace(length, ' ');
    cout << i << whitespace;

    for (string name : columnNames) {
      Types value = columns[name][i];
      

      // Extract the value from the cell
      ostringstream os;
      os << value;
      string toPrint = os.str();

      length = width[name] - toPrint.size();
      if (length < 0) { // Edge case where value would not fit in col
        toPrint.resize(width[name]); 
        length = 0;
        hadToCutOutValue = true;
      }

      string whitespace(length, ' ');
      cout << delimiter << toPrint << whitespace;

    }

    cout << "\n";

  }

  if (hadToCutOutValue){
    cout << "\nWARNING: Had to cut out some values since they did not fit\nConsider making your column names longer\n";
  }
}
  
// Private

// General validity checks

bool Table::validDatatype(const string datatype) const {
  for (string valid : supportedDatatypes){
    if (datatype == valid) return true;
  }

  return false;
}

bool Table::columnNameExists(string const name) const {
  return find(columnNames.begin(), columnNames.end(), name) != columnNames.end();
}

// TODO: two way check: all columns in table are in columnNames,
// all columnNames columns exist
void Table::columnNameValidityCheck(){
  for (string name : columnNames){
    if (columns.find(name) == columns.end()){
      cerr << "Column " << name << " found in columnNames but not in columns" << endl;
      exit(1);
    }
  }

  if (columnNames.size() != columns.size()){
    cerr << "mismatch between the number of elements in columns and columnNames" << endl;
    exit(1);
  }
}
    
void Table::tableValidityCheck() {
  if (numCols != columns.size()) {
    cerr << "numCols and actual number of columns missaligned" << endl;
    exit(1);
  }

  for (string name : columnNames) {
    if (numRows != columns[name].size()) {
      cerr << "numRows and actual number of entries missaligned in " << name << endl;
      exit(1);
    }
  }

}

