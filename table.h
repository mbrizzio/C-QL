#include "column.h"
#include <set>

using namespace std;

// First value will always be the original name, subsequents will be aliases
class Alias {
  public:
    Alias(const string &name);
    
    string getOriginalName() const;
    bool containsName(const string &name) const;

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

class Table{
  public:
    ////// Methods required for table manipulation
    
    // Default constructor, to be used when creating a table from scratch
    Table();

    // Takes in a .csv file, and converts it into a table
    // .csv have the format of "name (TYPE), val, val, ...;"
    Table(const string csvPath);

  private:
    ////// Methods required for table manipulation 
    unordered_map<string, Column> table;
    Aliases aliases;

    vector<int> indices;

    static const string filepathPrefix;
};