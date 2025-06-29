#pragma once
#include <algorithm>
#include "datatypes.h"

// Container for columns; will likely be pretty barebones, housing only stuff that is self-contained to a column
struct ColumnConstraints {
  bool Unique = false;
  bool TakesNulls = true;
  Types DefaultValue = Null;

  int TimePrecision = 6;
  int CharLength = 255; 
};

// Will need to create metaprogram type traits or just regular functions to
// Check if a cast is possible. Or just let it crash normally lol

class Column {
  public:
    Column();

    Column(const Datatypes Type);
    Column(const Datatypes Type, ColumnConstraints Constraints);

    Column(const vector<Types> Column, const Datatypes Type);
    Column(const vector<Types> Column, const Datatypes Type, ColumnConstraints Constraints);

    int size() const;

    void push();
    void push(Types value);

    void pop();   
    void erase(int index);

    template <typename Comparator>
    bool meetsCondition(const int index, const Types &rhs, const Comparator &comp) const;

    template <typename Comparator>
    vector<int> indicesMeetingCondition(const Types &rhs, const Comparator &comp) const;

    ////// Temporary column creation functions
    Column round(const vector<int> &indices, int decimals) const;
    Column ceiling(const vector<int> &indices) const;
    Column floor(const vector<int> &indices) const;
    Column absolute(const vector<int> &indices) const;

    Column length(const vector<int> &indices) const;
    Column concat(const vector<int> &indices, string toConcatenate) const;
    Column upper(const vector<int> &indices) const;
    Column lower(const vector<int> &indices) const;
    Column initCap(const vector<int> &indices) const;
    Column substring(const vector<int> &indices, int startPos, int length);
    Column trim(const vector<int> &indices);
    Column replace(const vector<int> &indices, 
                   const string &substr, const string &newVal);
    Column left(const vector<int> &indices, int cutoff) const;
    Column right(const vector<int> &indices, int start) const;

    template <typename Component>
    std::enable_if<is_same_v<decay_t<Component>, DateComponents> 
                  || is_same_v<decay_t<Component>, TimeComponents>
                  , Column> 
           extract(const vector<int> &indices, const Component &mode) const;
    
    template <typename Comparison, typename Lhs, typename Rhs>
    Column caseWhen(const vector<int> &indices, const Comparison &comp, 
                    const Lhs &whenTrue, const Rhs &whenFalse) const;
    template <typename T>
    Column nullIf(const vector<int> &indices, const T &rhs);
    template <typename T>
    Column coalesce(const vector<int> &indices, const T &rhs);

    Column cast(const vector<int> &indices, const Datatypes &type);
                
    ////// Aggregate functions
    // Ignore NULL
    double sum(const vector<int> &indices) const; 
    double sumDistinct(const vector<int> &indices) const;

    // Ignore NULL
    double count(const vector<int> &indices) const;
    double countDistinct(const vector<int> &indices) const;

    // Ignore NULL
    double avg(const vector<int> &indices) const;
    double avgDistinct(const vector<int> &indices) const;

    // Ignore NULL
    double max(const vector<int> &indices) const;
    double min(const vector<int> &indices) const;

    double standardDeviation(const vector<int> &indices) const;

    string stringAggregate(const vector<int> &indices, string separator) const;

    operator vector<Types> () const;
  
    vector<Types> col;
    Datatypes type;

    bool unique = false;
    bool takesNulls = true;
    Types defaultValue = Null;

    int timePrecision = 6;
    int charLength = 255;

    // Look into how CHECK works in SQL to inform its design

  private:
    void enforceColumnConstraints();
};

// WILL BE FULLY REDESIGNED LATER
class Table {
  public:
    Table();

    void insertColumn(const string name, const string dataType);
    void insertRow(Row row);
    
    // Overload for char/varchar
    void insertColumn(const string name, const string dataType, const int length);
    int getCharTypeLength(const string name) const;
    
    void print();

  private:
    unordered_map<string, ColumnOld> columns;
    unordered_map<string, int> charTypeColumnLengths;
    vector<string> columnNames;
    vector<int> order;

    vector<string> supportedDatatypes = {
        "INTEGER", "SMALLINT", "BIGINT", "FLOAT", 
        "TEXT", "CHAR", "VARCHAR", "NULL"
    };

    int numRows = 0;
    int numCols = 0;

    // Validity helpers
    bool validDatatype(const string datatype) const;
    bool columnNameExists(const string name) const;
    void columnNameValidityCheck();  
    void tableValidityCheck();
};