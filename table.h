#pragma once
#include <algorithm>

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
    unordered_map<string, Column> columns;
    unordered_map<string, int> charTypeColumnLengths;
    vector<string> columnNames;
    vector<int> order;

    vector<string> supportedDatatypes = {
        "INTEGER", "SMALLINT", "BIGINT", "FLOAT", 
        "TEXT", "CHAR", "VARCHAR","NULL"
    };

    int numRows = 0;
    int numCols = 0;

    // Validity helpers
    bool validDatatype(const string datatype) const;
    bool columnNameExists(const string name) const;
    void columnNameValidityCheck();  
    void tableValidityCheck();
};