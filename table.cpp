#pragma once
#include "datatypes.h"
#include "table.h"



using namespace std;


/////////////////////////// Column //////////////////////////////////////
explicit Column::operator vector<Types> () const {
  return col;
}
// Supports 2 indices: regular indexing, and pythonic negative indexing
const Types& Column::operator[] (int index) const {
  if (abs(index) > size()){
    cerr << "Index out of range" << endl;
    exit(10);
  }

  if (index < 0){
    return *(col.end() + index);
  }

  return *(col.begin() + index);
}

Types& Column::operator[] (int index) {
  if (abs(index) > size()){
    cerr << "Index out of range" << endl;
    exit(10);
  }

  if (index < 0){
    return *(col.end() + index);
  }

  return *(col.begin() + index);
}

////// Constructors
Column::Column(const Datatypes Type) : type(Type) {
  ColumnConstraints defaultParams;
  unique = defaultParams.Unique;
  takesNulls = defaultParams.TakesNulls;
  isPrimaryKey = defaultParams.IsPrimaryKey;
  isForeignKey = defaultParams.IsForeignKey;
  defaultValue = defaultParams.DefaultValue;

  timePrecision = defaultParams.TimePrecision;
  charLength = defaultParams.CharLength;
}

Column::Column(const Datatypes Type, ColumnConstraints Constraints) : type(Type),
  unique(Constraints.Unique), takesNulls(Constraints.TakesNulls), defaultValue(Constraints.DefaultValue),
  timePrecision(Constraints.TimePrecision), charLength(Constraints.CharLength), 
  isPrimaryKey(Constraints.IsPrimaryKey), isForeignKey(Constraints.IsForeignKey) {
  
  enforceCellContraint(defaultValue, true);    
}

Column::Column(const vector<Types> Column, const Datatypes Type) : col(Column), type(Type) {
  ColumnConstraints defaultParams;
  unique = defaultParams.Unique;
  takesNulls = defaultParams.TakesNulls;
  isPrimaryKey = defaultParams.IsPrimaryKey;
  isForeignKey = defaultParams.IsForeignKey;
  defaultValue = defaultParams.DefaultValue;

  timePrecision = defaultParams.TimePrecision;
  charLength = defaultParams.CharLength; 
  
  enforceWholeColumnConstraints();
}

Column::Column(const vector<Types> Column, const Datatypes Type, ColumnConstraints Constraints) :
  type(Type), col(Column), unique(Constraints.Unique), takesNulls(Constraints.TakesNulls), 
  defaultValue(Constraints.DefaultValue),timePrecision(Constraints.TimePrecision), 
  charLength(Constraints.CharLength), isPrimaryKey(Constraints.IsPrimaryKey), isForeignKey(Constraints.IsForeignKey) {

  enforceCellContraint(defaultValue, true);
  enforceWholeColumnConstraints();
}

////// Basic column manipulation
int Column::size() const {
  return col.size();
}

void Column::push() {
  col.push_back(defaultValue);
}

void Column::push(const Types value) {
  enforceCellContraint(value);

  col.push_back(value);
}

void Column::update(int index, const Types newValue) {
  enforceCellContraint(newValue);

  col[index] = newValue;
}

void Column::erase(int index) {
  col.erase(col.begin() + index);
}

void Column::bulkErase(vector<int> &indices) {
  // Sorting from greatest to smallest prevents any 'moved indices' shenanigans
  sort(indices.begin(), indices.end(), std::greater<Types>());

  for (int i : indices) {
    col.erase(col.begin() + i);
  }
}

void Column::bulkUpdate(vector<int> &indices, const Types newValue) {
  enforceCellContraint(newValue);

  for (int i : indices) {
    col[i] = newValue;
  }

  enforceWholeColumnConstraints();
}

template <typename Comparator>
bool Column::meetsCondition(const int index, const Types &rhs, const Comparator &comp) const {
  Types &lhs = col[index];

  return comp(lhs, rhs);
}

template <typename Comparator>
vector<int> Column::indicesMeetingCondition(const Types &rhs, const Comparator &comp) const {
  vector<int> goodIndices;
  for (int i : col.size()) {
    if (meetsCondition(i, rhs, comp)) goodIndices.push_back(i);
  }

  return goodIndices;
}

////// Temporary column creation functions

// Does not do type checking, will error if given a non-decimal column
Column Column::round(const vector<int> &indices, int decimals) const {
  Column converted(Datatypes::BIGINT); //we round, so it will be an int always
  float mult = pow(10, decimals);
  
  for (int i : indices){
    if (col[i] == Null){
      converted.push(Null);
    }
    else if (isString(type) || isNumeric(type)){
      converted.push(
        std::roundf(getNumeric<float>(col[i]) * mult ) / mult 
      );
    }
    else {
      converted.push(col[i]);
    }
  }
}

// Does not do type checking. Will attempt to work with a string type
Column Column::ceiling(const vector<int> &indices) const {
  Column converted(Datatypes::BIGINT); 

  for (int i: indices){
    if (col[i] == Null){
      converted.push(Null);
    }
    else if (isNumeric(type) || isString(type)){
      int64_t value = getNumeric<int64_t>(col[i]) + 1;
      converted.push(value);
    }
    else {
      converted.push(col[i]);
    }
  }
}

////// Private methods
void Column::enforceCellContraint(const Types &cell, const bool comesFromBulk) const {
  // Uniqueness (when coming in after the column has been created)
  if (!comesFromBulk && cell != Null && find(col.begin(), col.end(), cell) != col.end()){
    cerr << "Uniqueness constraint not met" << endl;
    exit(5);
  }

  // Takes nulls 
  if (cell == Null && !takesNulls){
    cerr << "(Not) Takes Nulls constraint not met" << endl;
    exit(5);
  }

  // Time Precision
  if (holds_alternative<Time>(cell) && timePrecision != -1 && get<Time>(cell).precision != timePrecision) {
    cerr << "Time precision constraint not met" << endl;
    exit(5);
  }

  if (holds_alternative<Datetime>(cell) && timePrecision != -1 && get<Datetime>(cell).time.precision != timePrecision) {
    cerr << "Time precision constraint not met" << endl;
    exit(5);
  }

  // CharLength Precision
  if (holds_alternative<Varchar>(cell) && timePrecision != -1 && get<Varchar>(cell).length != timePrecision) {
    cerr << "Char length constraint not met" << endl;
    exit(5);
  }

  if (holds_alternative<SQLChar>(cell) && timePrecision != -1 && get<SQLChar>(cell).length != timePrecision) {
    cerr << "Char length constraint not met" << endl;
    exit(5);
  }
}

void Column::enforceWholeColumnConstraints() const {
  for (Types cell : col){
    enforceCellContraint(cell, true);
  }

  // Uniqueness must be enforced here. Since we supoprt instantiating this object
  // with a vector, we must check this independently
  set<Types> uniqueCol(col.begin(), col.end());
  if (uniqueCol.size() != col.size()) {
    cerr << "Uniqueness constraint not met" << endl;
    exit(5);
  }
}


///////////////////////////// Column end ////////////////////////////////



////////////////////////////// Table start ///////////////////////////////

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

