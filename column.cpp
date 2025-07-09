#pragma once
#include "datatypes.h"
#include "column.h"

using namespace std;

/////////////////////////// Column //////////////////////////////////////
Column::operator vector<Types> () const {
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



////// Temporary column creation functions
// Does not do type checking, will error if given a non-decimal column
Column Column::round(const vector<int> &indices, int decimals) const {
  Column converted(Datatypes::FLOAT); //we round to certain decimals, so needs to be float
  float mult = pow(10, decimals);
  
  for (int i : indices){
    if (isNull(col[i])){
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

  return converted;
}

// Does not do type checking. Will attempt to work with a string type
Column Column::ceiling(const vector<int> &indices) const {
  Column converted(Datatypes::BIGINT); 

  for (int i: indices){
    if (isNull(col[i])){
      converted.push(Null);
    }
    else if (isNumeric(type) || isString(type)){
      int64_t ceiled = (int64_t)std::ceil(getNumeric<float>(col[i]));
      converted.push(ceiled);
    }
    else {
      converted.push(col[i]);
    }
  }

  return converted;
}

Column Column::floor(const vector<int> &indices) const {
  Column converted(Datatypes::BIGINT);

  for (int i : indices){
    if (isNull(col[i])) {
      converted.push(Null);
    }
    else if (isNumeric(type) || isString(type)){
      int64_t floored = (int64_t)std::floor(getNumeric<float>(col[i]));
      
      converted.push(floored);
    }
    else{
      converted.push(col[i]);
    }
  }

  return converted;
}

// Works strictly with numeric types
Column Column::absolute(const vector<int> &indices) const {
  if (!isNumeric(type)){
    cerr << "Column is not numeric" << endl;
    exit(9);
  }

  Column converted(Datatypes::FLOAT);

  for (int i : indices){
    if (isNull(col[i])) {
      converted.push(Null);
    }
    else{
      converted.push((float)abs(getNumeric<float>(col[i])));
    }
  }

  return converted;
}

// Works strictly with string types
Column Column::length(const vector<int> &indices) const {
  if (!isString(type)){
    cerr << "Column is not string based" << endl;
    exit(9);
  }

  Column converted(Datatypes::INT);

  for (int i : indices){
    if (isNull(col[i])){
      converted.push(Null);
    }
    else{
      converted.push((int)getString(col[i]).length());
    }
  }

  return converted;
}

// Works strictly with string types
Column Column::concat(const vector<int> &indices, string toConcatenate) const {
  if (!isString(type)){
    cerr << "Column is not string based" << endl;
    exit(9);
  }
  
  Column converted(Datatypes::TEXT);

  for (int i : indices){
    if (isNull(col[i])){
      converted.push(Null);
    }
    else{
      converted.push(getString(col[i]) + toConcatenate);
    }
  }

  return converted;
}

// Works strictly with string types
Column Column::upper(const vector<int> &indices) const {
  if (!isString(type)){
    cerr << "Column is not string based" << endl;
    exit(9);
  }

  Column converted(Datatypes::TEXT);

  for (int i : indices){
    if (isNull(col[i])){
      converted.push(Null);
    }
    else{
      string text = getString(col[i]);

      for (char &c : text) {c = 'a' <= c && c <= 'z' ? c + ('A' - 'a') : c;}

      converted.push(text);
    }
  }

  return converted;
}

// Works strictly with string types
Column Column::lower(const vector<int> &indices) const {
  if (!isString(type)){
    cerr << "Column is not string based" << endl;
    exit(9);
  }

  Column converted(Datatypes::TEXT);

  for (int i : indices){
    if (isNull(col[i])){
      converted.push(Null);
    }
    else{
      string text = getString(col[i]);

      for (char &c : text) {c = 'A' <= c && c <= 'Z' ? c + ('a' - 'A') : c;}

      converted.push(text);
    }
  }

  return converted;
} 

// Works strictly with string types
Column Column::initCap(const vector<int> &indices) const {
  if (!isString(type)){
    cerr << "Column is not string based" << endl;
    exit(9);
  }

  Column converted(Datatypes::TEXT);

  for (int i : indices){
    if (isNull(col[i])){
      converted.push(Null);
    }

    else{
      string text = getString(col[i]);

      if (text.empty()){
        converted.push(text);
        continue;
      }

      bool pastWasDelimiter = true;
      auto isDelimiter = [] (char c) -> bool {
        return (!('A' <= c && c <= 'Z') &&
                !('a' <= c && c <= 'z') &&
                !('0' <= c && c <= '9'));
      };

      for (char &c : text) {
        if (isDelimiter(c)){
          pastWasDelimiter = true;
          continue;
        }
        
        if ('0' <= c && c <= '9'){
          pastWasDelimiter = false;
          continue;
        }

        c = pastWasDelimiter ? static_cast<char>(std::toupper(c)) :
                               static_cast<char>(std::tolower(c));
        pastWasDelimiter = false;
      }
      
      converted.push(text);
    }
  }

  return converted;
}

// Works strictly on strings. startPos is 1-indexed!
Column Column::substring(const vector<int> &indices, int startPos, int length) const {
  if (!isString(type)){
    cerr << "Column is not string based" << endl;
    exit(9);
  }
  
  Column converted(Datatypes::TEXT);
  --startPos;

  for (int i : indices){
    if (isNull(col[i])){
      converted.push(Null);
    }
    else{
      string text = getString(col[i]);
      
      if (startPos < 0) startPos = 0;

      if (static_cast<int>(text.size()) < startPos){
        converted.push("");
        continue;
      }

      converted.push(text.substr(startPos, std::min(startPos + length, (int)text.size()) - startPos));
    }
  }

  return converted;
}

// Works strictly on string types
Column Column::trim(const vector<int> &indices, 
                    const TrimModes mode, char toRemove) const {
  if (!isString(type)){
    cerr << "Column is not string based" << endl;
    exit(9);    
  }

  Column converted(Datatypes::TEXT);

  for (int i : indices){
    if (isNull(col[i])){
      converted.push(Null);
      continue;
    }

    string text = getString(col[i]);

    switch (mode){
      case TrimModes::LEADING: {
        auto cut = text.begin();
        while (cut != text.end() && *cut == toRemove){
          ++cut;
        }

        converted.push(string(cut, text.end()));
        break;
      }

      case TrimModes::TRAILING: {
        auto cut = text.rbegin();
        while (cut != text.rend() && *cut == toRemove){
          ++cut;
        }

        converted.push(string(text.begin(), cut.base()));
        break;
      }

      case TrimModes::BOTH: {
        auto bcut = text.begin();
        while (bcut != text.end() && *bcut == toRemove){
          ++bcut;
        }

        text = string(bcut, text.end());

        auto ecut = text.rbegin();
        while (ecut != text.rend() && *ecut == toRemove){
          ++ecut;
        }

        converted.push(string(text.begin(), ecut.base()));
        break;
      }
    } // Switch statement
  } // For loop

  return converted;
}

// Works strictly on string types
Column Column::replace(const vector<int> &indices, 
                       const string &substr, const string &newVal) const {
  if (!isString(type)){
    cerr << "Column is not string based" << endl;
    exit(9);    
  }

  Column converted(Datatypes::TEXT);  

  for (int i : indices){
    if (isNull(col[i])){
      converted.push(Null);
      continue;
    }

    string text = getString(col[i]);

    // Bug: there can be a case where a conversion generates extra cases of what needs 
    // to be replaced, thereby deleting extra information
    size_t start = text.find(substr, 0);
    
    while (start != std::string::npos){
      text.replace(start, substr.size(), newVal);
      start = text.find(substr, 0);
    }

    converted.push(text);
  }

  return converted;
}

Column Column::left(const vector<int> &indices, int cutoff) const {
  if (!isString(type)){
    cerr << "Column is not string based" << endl;
    exit(9);    
  }
  
  Column converted(Datatypes::TEXT);

  for (int i : indices){
    if (isNull(col[i])){
      converted.push(Null);
    }
    else{
      string text = getString(col[i]);

      if (cutoff > text.size()){
        converted.push(text);
        continue;
      }

      converted.push(string(text.begin(), text.begin() + cutoff));
    }
  }

  return converted;
}

Column Column::right(const vector<int> &indices, int start) const {
  if (!isString(type)){
    cerr << "Column is not string based" << endl;
    exit(9);    
  }
  
  Column converted(Datatypes::TEXT);

  for (int i : indices){
    if (isNull(col[i])){
      converted.push(Null);
    }
    else{
      string text = getString(col[i]);

      if (start > text.size()){
        converted.push("");
        continue;
      }

      converted.push(string(text.begin(), (text.rbegin() + start).base()));
    }
  }

  return converted;
}

template <typename Component>
std::enable_if_t<is_same_v<decay_t<Component>, DateComponents> 
              || is_same_v<decay_t<Component>, TimeComponents>
              , Column> 
Column::extract(const vector<int> &indices, const Component &mode) const {
  if (!isDate(type)){
    cerr << "Column is not date based" << endl;
    exit(9);
  }

  Column converted(Datatypes::FLOAT);

  for (int i : indices){
    if (isNull(col[i])){
      converted.push(Null);
    }
    else{
      float component = std::visit([&mode] (auto &value) {
        return static_cast<float>(value.extract(mode));
      }, col[i]);

      converted.push(component);
    }
  }

  return converted;
}

Column Column::nullIf(const vector<int> &indices, const Types &rhs) const {
  Column converted(type, {unique, true, isPrimaryKey, isForeignKey, 
                          defaultValue, timePrecision, charLength});

  for (int i : indices){
    bool areEqual = std::visit([] (auto &lhs, auto &rhs) -> bool {
      using lhsT = decay_t<decltype(lhs)>;
      using rhsT = decay_t<decltype(rhs)>;

      if constexpr (is_same_v<lhsT, rhsT> || (is_arithmetic_v<lhsT> && is_arithmetic_v<rhsT>) ||
                    (is_string_v<lhsT> && is_string_v<rhsT>)){
        return lhs == rhs;
      }
      
      return false;
    }, col[i], rhs);

    if (areEqual) converted.push(Null);
    else converted.push(col[i]);
  }

  return converted;
}

Column Column::coalesce(const vector<int> &indices, const Types &rhs) const {
  Column converted(type, {unique, false, isPrimaryKey, isForeignKey, 
                          validNonNullDefaultValue(type), timePrecision, charLength});  

  for (int i : indices) {
    if (isNull(col[i])){
      converted.push(rhs);
    }
    else{
      converted.push(col[i]);
    }
  }

  return converted;
}

double Column::sum(const vector<int> &indices) const {
  double total = 0;
  
  for (int i : indices){ //ugly af, shoudl be abstracted into a function
    if (isNull(col[i])) continue;
    
    total += getNumeric<double>(col[i]);
  }

  return total;
}

double Column::sumDistinct(const vector<int> &indices) const { 
  double total = 0;
  set<double> seenNumbers; // This is a sum, so we can safely assume that we have a number

  for (int i : indices) {
    if (isNull(col[i])) continue;

    double value = getNumeric<double>(col[i]);

    if (seenNumbers.contains(value)) continue;

    seenNumbers.insert(value);
    total += value;
  }

  return total;
}

int Column::count(const vector<int> &indices) const {
  int total = 0;
  for (int i : indices) {
    if (isNull(col[i])) continue;

    ++total;
  }

  return total;
}

int Column::countDistinct(const vector<int> &indices) const {
  int total = 0;
  set<Types> seenValues;
  for (int i : indices){
    if (isNull(col[i]) || seenValues.contains(col[i])) continue;

    seenValues.insert(col[i]);
    ++total;
  }

  return total;
}

// Both can be made from O(2N) to O(N) by reqriting them, but this is cleaner
double Column::avg(const vector<int> &indices) const {
  return sum(indices) / (double)count(indices);
}

double Column::avgDistinct(const vector<int> &indices) const {
  return sumDistinct(indices) / (double)countDistinct(indices);
}

// Cannot be done simply due to the fact that these are Types and need to be unpacked individually
double Column::max(const vector<int> &indices) const {
  double max = std::numeric_limits<double>::lowest();

  for (int i : indices){
    if (isNull(col[i])) continue;

    max = std::max(max, (getNumeric<double>(col[i])));
  }

  return max;
}

double Column::min(const vector<int> &indices) const {
  double min = std::numeric_limits<double>::max();

  for (int i : indices){
    if (isNull(col[i])) continue;

    min = std::min(min, (getNumeric<double>(col[i])));
  }

  return min;
}

string Column::stringAggregate(const vector<int> &indices, string separator) const {
  string result = "";

  for (int i : indices){
    if (isNull(col[i])) continue;
    ostringstream stringHolder;
    stringHolder << col[i];

    result += stringHolder.str();
    result += separator;
  }

  result.resize(result.size() - separator.size());
  return result;
}

// sqrt(sigma((xi - avg)^2 / N))
// Does not do type checking. will attempt to convert strings to numbers
double Column::standardDeviation(const vector<int> &indices) const {
  float average = (float)avg(indices);
  int N = count(indices);

  double stddev = 0;

  for (int i : indices) {
    stddev += pow(getNumeric<float>(col[i]) - average, 2);
  }

  stddev /= N;

  return stddev;  
}

////// Private methods
void Column::enforceCellContraint(const Types &cell, const bool comesFromBulk) const {
  // Data type check
  if (getType(cell) != type && getType(cell) != Datatypes::NULLVALUE){
    cerr << "Datatype does not match the type of column" << endl;
    exit(5);
  }
  
  // Uniqueness (when coming in after the column has been created)
  if (unique && !comesFromBulk && cell != Null && find(col.begin(), col.end(), cell) != col.end()){
    cerr << "Uniqueness constraint not met" << endl;
    exit(5);
  }

  // Takes nulls 
  if (isNull(cell) && !takesNulls){
    cerr << "(Does not take) Nulls constraint not met" << endl;
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
  if (holds_alternative<Varchar>(cell) && charLength != -1 && get<Varchar>(cell).length != charLength) {
    cerr << "Char length constraint not met" << endl;
    exit(5);
  }

  if (holds_alternative<SQLChar>(cell) && charLength != -1 && get<SQLChar>(cell).length != charLength) {
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
  if (!unique) return;

  set<Types> uniqueCol(col.begin(), col.end());
  if (uniqueCol.size() != col.size()) {
    cerr << "Uniqueness constraint not met" << endl;
    exit(5);
  }
}

Types validNonNullDefaultValue(Datatypes type) {
  switch (type) {
    case (Datatypes::BIGINT): {
      return static_cast<int64_t>(0);
    } 
    case (Datatypes::BOOL): {
      return false;
    }
    case (Datatypes::CHAR): {
      return SQLChar();
    }
    case (Datatypes::DATE): {
      return Date("2000-01-01");
    }
    case (Datatypes::DATETIME): {
      return Datetime();
    }
    case (Datatypes::FLOAT): {
      return static_cast<float>(0);
    }
    case (Datatypes::INT): {
      return static_cast<int>(0);
    }
    case (Datatypes::SMALLINT): {
      return static_cast<int16_t>(0);
    }
    case (Datatypes::TEXT): {
      return "";
    }
    case (Datatypes::TIME): {
      return Time();
    }
    case (Datatypes::VARCHAR): {
      return Varchar();
    }
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

