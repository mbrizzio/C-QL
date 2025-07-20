#include "column.h"
#include <set>
#include <functional>

using namespace std;

// First value will always be the original name, subsequents will be aliases
class Alias {
  public:
    Alias(const string &name);
    
    string getOriginalName() const;
    bool containsName(const string &name) const;

    int currentAlias = 0;
    vector<string> names;
    static const int ORIGINAL_NAME_LOCATION;
};

class Aliases {
  public:
    Aliases();

    void push(const Alias &alias);
    void push(const string &trueName, const string &newName);

    // If passing in an alias, gives the real name
    // If passing in a name, reflects the name back
    // If neither, errors out
    string operator[] (const string &name) const;

  private:
    vector<Alias> names;
};

// I will need to create a class to enforce CHECKs, need to look into how they work
class Table{
  public:
    ////// Methods required for table manipulation
    
    // Default constructor, to be used when creating a table from scratch
    Table();

    // Takes in a .csv file, and converts it into a table
    // .csv have the format of "name (TYPE), val, val, ...;"
    Table(const string csvName);

    void addColumn(const string &name, Datatypes type, string &unprocessedValues);
    void addColumn(const Column formedColumn, const string &name);
    void deleteColumn(const string &name);
    void renameColumn(const string &oldName, const string &newName);

    void insertRows(const vector<string> &colNames, const vector<Types> &values);
    void insertRows(const vector<Types> &values);
    void updateRows(); // Will need to look into how this works fully
    void deleteRows(); // Same
    
  private:
    ////// Methods required for table manipulation 
    unordered_map<string, Column> table = {};
    Aliases aliases = Aliases();

    vector<int> indices = {};
    vector<string> primaryColumns = {};

    int length = 0;

    static const string filepathPrefix;

    ////// Helper methods for data processing

    // Takes in a full CSV line (past the first line) and creates a column from that
    Column commaSeparatedToColumn(const Datatypes type, string &values); 
};