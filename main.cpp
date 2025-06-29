#include "datatypes.h"
#include "table.h"
#include "testsuite.h"

using namespace std;

int main(){
  // Table table;

  // table.insertColumn("test", "INTEGER");
  // table.insertColumn("Name", "TEXT");
  
  // Row toAdd;
  // toAdd["test"] = 1;
  // table.insertRow(toAdd);

  // toAdd["Name"] = "heh";
  // table.insertRow(toAdd);

  // table.insertColumn("variableCharacter", "VARCHAR", 10);
  // Row empty;
  // table.insertRow(empty);

  // Varchar v(&table, "variableCharacter", "exactlyTen");
  // empty["variableCharacter"] = v;
  // table.insertRow(empty);

  // Varchar d(10, "lesser");
  // empty["variableCharacter"] = d;
  // table.insertRow(empty);

  // table.insertColumn("setchar", "CHAR", 10);

  // SQLChar c(10, "lesser");

  // toAdd["setchar"] = c;
  // table.insertRow(toAdd);
  
  // table.print();

  // varcharComparatorCorrectnessTests();
  // charComparatorCorrectnessTests();
  // runTypesComparisonTests();
  // dateUnitTests();
  // dateComparisonTestSuite();
  // dateArithmeticTestSuite();
  // timeTestCases();

  datetimeUnitTests();
}