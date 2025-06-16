#include <optional>
#include <map>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <variant>
#include <cassert>
#include <tuple>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

using Types = variant<
  int,        //INTEGER
  int16_t,    //SMALLINT
  int64_t,    //BIGINT
  float,      //FLOAT
  string,     //TEXT
  monostate   //NULL
>;

using Row = unordered_map<string, Types>;
using Column = vector<Types>;

monostate null = monostate{};

class Table{
  public:

    Table() {}

    // Add a new column to the table
    // Ensures the datatype is supported
    void insertColumn(const string name, const string dataType){    
      assert(validDatatype(dataType));

      columns[name] = {};
      columnNames.push_back(name);

      ++numCols;

      // Populate the column with nulls for the number of rows in table
      for (int i = 0; i < numRows; ++i){
        columns[name].push_back(null);
      }
    }

    // Add a new row to the table
    // Skips incorrectly names/non-existent columns
    // Columns not specified get initialized as NULL
    void insertRow(Row row){
      for (string column : columnNames){
        if (row.find(column) == row.end()) row[column] = null;;

        columns[column].push_back(row[column]);
      }

      order.push_back(numRows);
      ++numRows;
    }

    // Prints out the existing SQL table
    void print() {
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
      string separator(totalSize*1.5, '-'); // Why is - not monospace...
      cout << separator << "\n";

      // Rows, in order
      for (int i : order){
        int length = numHeader.size() - to_string(i).size();
        string whitespace(length, ' ');
        cout << i << whitespace;

        for (string name : columnNames) {
          Types value = columns[name][i];
          string toPrint = "";

          // Extract the value from the cell
          if (holds_alternative<monostate>(value)) toPrint = "NULL";
          else if (holds_alternative<string>(value)) toPrint = get<string>(value);
          else if (holds_alternative<float>(value)){
            ostringstream out;
            out << fixed << setprecision(4) << get<float>(value);
            toPrint = out.str();
          }
          else if (holds_alternative<int>(value)) toPrint = to_string(get<int>(value));
          else if (holds_alternative<int16_t>(value)) toPrint = to_string(get<int16_t>(value));
          else if (holds_alternative<int64_t>(value)) toPrint = to_string(get<int64_t>(value));
          else {
            cerr << "Datatype of column currently unsupported by this function" << endl;
            exit(2);
          }

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
  

  private: 
    unordered_map<string, Column> columns;
    vector<string> columnNames; //maybe should be a set?
    vector<int> order;

    vector<string> supportedDatatypes = {
      "INTEGER", "SMALLINT", "BIGINT", "FLOAT", "TEXT", "NULL"
    };

    bool validDatatype(const string datatype) const {
      for (string valid : supportedDatatypes){
        if (datatype == valid) return true;
      }

      return false;
    }

    void tableValidityCheck() {
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

    int numRows = 0;
    int numCols = 0;
};


int main(){
  Table table;

  table.insertColumn("test", "INTEGER");
  table.insertColumn("Name", "TEXT");
  Row toAdd;
  toAdd["test"] = 1;

  table.insertRow(toAdd);

  Row toAdd2;
  toAdd2["Name"] = "hehehe";

  table.insertRow(toAdd2);

  table.print();
}