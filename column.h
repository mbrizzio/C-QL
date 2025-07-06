#pragma once
#include <algorithm>
#include <iostream>
#include <sstream>
#include <set>
#include <cmath>
#include <limits>
#include "datatypes.h"

enum class TrimModes {
  LEADING,
  TRAILING,
  BOTH
};

// Container for columns; will likely be pretty barebones, housing only stuff that is self-contained to a column
struct ColumnConstraints {
  bool Unique = false;
  bool TakesNulls = true;
  bool IsPrimaryKey = false;
  bool IsForeignKey = false;
  Types DefaultValue = Null;

  int TimePrecision = -1; // Indicates it can be anything
  int CharLength = -1; // Indicates it can be anything
};

// Will need to create metaprogram type traits or just regular functions to
// Check if a cast is possible. Or just let it crash normally lol

class Column {
  public:
    Column(const Datatypes Type);
    Column(const Datatypes Type, ColumnConstraints Constraints);

    Column(const vector<Types> Column, const Datatypes Type);
    Column(const vector<Types> Column, const Datatypes Type, ColumnConstraints Constraints);

    int size() const;

    void push();
    void push(const Types value);
    void update(int index, const Types newValue);

    void pop();   
    void erase(int index);

    void bulkErase(vector<int> &indices);
    void bulkUpdate(vector<int> &indices, const Types newValue);

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
    Column substring(const vector<int> &indices, int startPos, int length) const;
    Column trim(const vector<int> &indices, const TrimModes mode, char toRemove=' ') const;
    Column replace(const vector<int> &indices, 
                   const string &substr, const string &newVal) const;
    Column left(const vector<int> &indices, int cutoff) const;
    Column right(const vector<int> &indices, int start) const;

    template <typename Component>
    std::enable_if_t<is_same_v<decay_t<Component>, DateComponents> 
                  || is_same_v<decay_t<Component>, TimeComponents>
                  , Column> 
           extract(const vector<int> &indices, const Component &mode) const;
    
    // Should this be removed and movs to a Table class? I am sure it should. keeping it here for now just in case
    // template <typename Comparison>
    // Column caseWhen(const vector<int> &indices, const Comparison &comp, 
    //                 const Types &whenTrue, const Types &whenFalse) const;
    Column nullIf(const vector<int> &indices, const Types &rhs) const;
    Column coalesce(const vector<int> &indices, const Types &rhs) const;
    

    // Don't wanna deal with this one now, it will involve writing a lot of boilerplate for explicit casts
    Column cast(const vector<int> &indices, const Datatypes &type);
                
    ////// Aggregate functions
    // Ignore NULL
    double sum(const vector<int> &indices) const; 
    double sumDistinct(const vector<int> &indices) const;

    // Ignore NULL
    int count(const vector<int> &indices) const;
    int countDistinct(const vector<int> &indices) const;

    // Ignore NULL
    double avg(const vector<int> &indices) const;
    double avgDistinct(const vector<int> &indices) const;

    // Ignore NULL
    double max(const vector<int> &indices) const;
    double min(const vector<int> &indices) const;

    double standardDeviation(const vector<int> &indices) const;

    string stringAggregate(const vector<int> &indices, string separator) const;

    explicit operator vector<Types> () const;
    const Types& operator[] (int index) const;
    Types& operator[] (int index);
  
    Datatypes type;

    int timePrecision = 6;
    int charLength = 255;

  private:
    void enforceWholeColumnConstraints() const;
    void enforceCellContraint(const Types &cell, const bool comesFromBulk=false) const; 

    bool unique = false;
    bool takesNulls = true;
    bool isPrimaryKey = false;
    bool isForeignKey = false;
    Types defaultValue = Null;
    vector<Types> col;
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