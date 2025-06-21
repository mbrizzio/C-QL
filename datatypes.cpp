#pragma once
#include "datatypes.h"
#include "table.h"

/////////////////////////////////// Varchar /////////////////////////////
Varchar::Varchar() {
  length = 100;
  value = "";
}

Varchar::Varchar(string Value) : value(Value){
  length = Value.size();
}

Varchar::Varchar(int Length) : length(Length) {
  value = "";

  enforceLengthInvariant();
};

Varchar::Varchar(int Length, string Value) : length(Length),
  value(Value) {
    enforceLengthInvariant();
  };

Varchar::Varchar(Table* table, std::string name) {
  length = table->getCharTypeLength(name);
  value = ""; 

  enforceLengthInvariant();
}

Varchar::Varchar(Table* table, string name, string Value) : value(Value) {
  length = table->getCharTypeLength(name);

  enforceLengthInvariant();
}

void Varchar::enforceLengthInvariant(){
  if (value.size() > length){
    cerr << "Length of VARCHAR exceeds the stipulated length" << endl;
    exit(3);
  }
}

string Varchar::getValue() const {
  return value;
}
/////////////////////////////// End Varchar //////////////////////////////



////////////////////////////// SQLChar //////////////////////////////////

SQLChar::SQLChar() : Varchar() {};

SQLChar::SQLChar(string Value) : Varchar(Value) {};

SQLChar::SQLChar(int Length) : Varchar(Length) {
  enforceLengthInvariant();
}

SQLChar::SQLChar(int Length, string Value) : Varchar(Length, Value) {
  enforceLengthInvariant();
}

SQLChar::SQLChar(Table* table, std::string name) : Varchar(table, name){
  enforceLengthInvariant();
}

SQLChar::SQLChar(Table* table, string name, string Value) : 
Varchar(table, name, Value) {
  enforceLengthInvariant();
}

void SQLChar::enforceLengthInvariant(){
  if (value.size() > length){
    cerr << "Length of VARCHAR exceeds the stipulated length" << endl;
    exit(3);
  }

  if (value.size() < length){
    int padLength = length - value.size();
    string padding(padLength, ' ');

    value += padding;
  }
}

string SQLChar::getUnpaddedValue() const{
  auto end = value.end();
  auto begin = value.begin();

  while (end != begin && *(--end) == ' ');

  return string(begin, ++end);
}
///////////////////////// End SQLChar //////////////////////////////////


/////////////////////////// Date ///////////////////////////////////////
Date::Date(const int Year, const int Month, const int Day) :
          year(Year), month(Month), day(Day) {
  enforceDateInvariants();
  dateToEpoch();
}

Date::Date(const int Epoch) : epoch(Epoch) {
  epochToDate();
}

// YYYY-MM-DD
Date::Date(string &YearMonthDay){
  if (YearMonthDay.size() != 10) {
    cerr << "Incorrect format for this constructor (should be YYYY-MM-DD)" << endl;
    exit(3);
  }

  year = stoi(YearMonthDay.substr(0, 4));
  month = stoi(YearMonthDay.substr(5, 2));
  day = stoi(YearMonthDay.substr(8, 2));

  enforceDateInvariants();
  dateToEpoch();
}


void Date::dateToEpoch(){
  int updatedEpoch = 0;

  // Additions due to years
  updatedEpoch += (year - 1) * 365;
  updatedEpoch += leapDaysUpToCurrentYear();

  // Additions due to months
  if (isLeapYear() && month > 2) ++updatedEpoch;
  int pastMonths = month;
  while (--pastMonths > 0){
    int daysToSubstract = daysPerMonth[pastMonths - 1];
    updatedEpoch += daysToSubstract;
  }

  // Additions due to days
  updatedEpoch += day;

  epoch = updatedEpoch;
}

void Date::epochToDate(){
  // First we generate the epoch from years
  int naiveYears = (epoch / (int)365) + 1;
  Date naiveYearAsDate = Date(naiveYears, 1, 1);
  int naiveRemainder = epoch - naiveYears*365 - 
                       naiveYearAsDate.leapDaysUpToCurrentYear() -
                       naiveYearAsDate.isLeapYear(); // the final substraction could be buggy, think through!
  
  // Set year right away so we can make use of helpers.
  year = naiveYears;
  if (naiveRemainder <= 0) --year;

  int updatedRemainder = epoch - year*365 - leapDaysUpToCurrentYear();

  // Create the month
  month = 1;
  while (updatedRemainder > 0){
    if (isLeapYear() && month == 2) --updatedRemainder;
    updatedRemainder -= daysPerMonth[month];
    ++month;
  }
  --month;
  updatedRemainder += daysPerMonth[month];
  if (isLeapYear() && month == 2) ++updatedRemainder;

  // The remainder represents the days of the month, so nothing left to do
  day = updatedRemainder;
}

bool Date::isLeapYear() const {
  // The rule is: evey 4 years there is a leap years, multiples of 100 are skipped
  // UNLESS they are multiples of 400
  return (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0));
}

int Date::leapDaysUpToCurrentYear() const {
  int leapYears = (year - 1) / (int)4;

  // Remove multiples of 100
  leapYears -= (year - 1) / (int)100;

  // Add multiples of 400
  leapYears += (year - 1) / (int)400;

  return leapYears;
}

void Date::enforceDateInvariants() const {
  bool yearCheck = (year >= 0 && year <= 9999);
  if (!yearCheck) {
    cerr << "The year is not in the supported range" << endl;
    exit(3);
  }
  
  bool monthCheck = (month >= 0 && month <= 12);
  if (!monthCheck){
    cerr << "The month is not in the supported range" << endl;
    exit(3);
  }

  bool dayCheck = (day <= daysPerMonth[month - 1] + (month == 2) * isLeapYear());
  if (!dayCheck){
    cerr << "The day is too large for the specified month" << endl;
    exit(3);
  }

  dayCheck = (day >= 0);
  if (!dayCheck){
    cerr << "The day cannot be negative" << endl;
    exit(3);
  }
}

//////////////////////// End Date ///////////////////////////////////////



/////////////////////// misc operators (all classes) //////////////////////


////// OS operators 
ostream& operator<<(ostream& os, const Varchar& self){
  os << self.value;
  return os;
}

ostream& operator<<(ostream& os, const SQLChar& self){
  os << self.value;
  return os;
}

ostream& operator<<(ostream& os, const Date& self){
  os << setfill('0') << setw(4) << self.year << '-';
  os << setfill('0') << setw(2) << self.month << '-';
  os << setfill('0') << setw(2) << self.day;

  return os;
}

ostream& operator<<(ostream& os, const Types& self){
  if (holds_alternative<monostate>(self)) os << "NULL";
  else if (holds_alternative<string>(self)) os << get<string>(self);
  else if (holds_alternative<SQLChar>(self)) os << get<SQLChar>(self);
  else if (holds_alternative<Varchar>(self)) os << get<Varchar>(self);
  else if (holds_alternative<float>(self)){
    os << fixed << setprecision(4) << get<float>(self);
  }
  else if (holds_alternative<int>(self)) os << to_string(get<int>(self));
  else if (holds_alternative<int16_t>(self)) os << to_string(get<int16_t>(self));
  else if (holds_alternative<int64_t>(self)) os << to_string(get<int64_t>(self));
  else {
    cerr << "Datatype of column currently unsupported by this function" << endl;
    exit(2);
  }

  return os;

}

/////////////////////// Comparator hell (all classes) ///////////////////

////// Varchar vs. Varchar
bool Varchar::operator==(const Varchar &rhs) const {
  return comparatorHelper(value, rhs.value, std::equal_to<>{});
}

bool Varchar::operator!=(const Varchar &rhs) const {
  return comparatorHelper(value, rhs.value, std::not_equal_to<>{});
}

bool Varchar::operator<(const Varchar &rhs) const {
  return comparatorHelper(value, rhs.value, std::less<>{});
}

bool Varchar::operator>(const Varchar &rhs) const {
  return comparatorHelper(value, rhs.value, std::greater<>{});
}

bool Varchar::operator<=(const Varchar &rhs) const {
  return comparatorHelper(value, rhs.value, std::less_equal<>{});
}

bool Varchar::operator>=(const Varchar &rhs) const {
  return comparatorHelper(value, rhs.value, std::greater_equal<>{});
}


////// Varchar vs. String
bool Varchar::operator==(const string &rhs) const {
  return comparatorHelper(value, rhs, std::equal_to<>{});
} 

bool Varchar::operator!=(const string &rhs) const {
  return comparatorHelper(value, rhs, std::not_equal_to<>{});
}

bool Varchar::operator<(const string &rhs) const {
  return comparatorHelper(value, rhs, std::less<>{});
}

bool Varchar::operator>(const string &rhs) const {
  return comparatorHelper(value, rhs, std::greater<>{});
}

bool Varchar::operator<=(const string &rhs) const {
  return comparatorHelper(value, rhs, std::less_equal<>{});
}

bool Varchar::operator>=(const string &rhs) const {
  return comparatorHelper(value, rhs, std::greater_equal<>{});
} 


///// String vs. Varchar
bool operator==(const string &lhs, const Varchar &rhs) {
  return rhs.comparatorHelper(lhs, rhs.value, std::equal_to<>{});
}

bool operator!=(const string &lhs, const Varchar &rhs) {
  return rhs.comparatorHelper(lhs, rhs.value, std::not_equal_to<>{});
}

bool operator<(const string &lhs, const Varchar &rhs) {
  return rhs.comparatorHelper(lhs, rhs.value, std::less<>{});
}

bool operator>(const string &lhs, const Varchar &rhs) {
  return rhs.comparatorHelper(lhs, rhs.value, std::greater<>{});
}

bool operator<=(const string &lhs, const Varchar &rhs) {
  return rhs.comparatorHelper(lhs, rhs.value, std::less_equal<>{});
}

bool operator>=(const string &lhs, const Varchar &rhs) {
  return rhs.comparatorHelper(lhs, rhs.value, std::greater_equal<>{});
}


///// SQLChar vs. SQLChar
bool SQLChar::operator==(const SQLChar &rhs) const {
  return comparatorHelper(value, rhs.value, std::equal_to<>{});
}

bool SQLChar::operator!=(const SQLChar &rhs) const {
  return comparatorHelper(value, rhs.value, std::not_equal_to<>{});
}

bool SQLChar::operator<(const SQLChar &rhs) const {
  return comparatorHelper(value, rhs.value, std::less<>{});
}

bool SQLChar::operator>(const SQLChar &rhs) const {
  return comparatorHelper(value, rhs.value, std::greater<>{});
}

bool SQLChar::operator<=(const SQLChar &rhs) const {
  return comparatorHelper(value, rhs.value, std::less_equal<>{});
}

bool SQLChar::operator>=(const SQLChar &rhs) const {
  return comparatorHelper(value, rhs.value, std::greater_equal<>{});
}


///// String vs. SQLChar
bool operator==(const string &lhs, const SQLChar &rhs) {
  return rhs.comparatorHelper(lhs, rhs.getUnpaddedValue(), std::equal_to<>{});
}

bool operator!=(const string &lhs, const SQLChar &rhs) {
  return rhs.comparatorHelper(lhs, rhs.getUnpaddedValue(), std::not_equal_to<>{});
}

bool operator<(const string &lhs, const SQLChar &rhs) {
  return rhs.comparatorHelper(lhs, rhs.getUnpaddedValue(), std::less<>{});
}

bool operator>(const string &lhs, const SQLChar &rhs) {
  return rhs.comparatorHelper(lhs, rhs.getUnpaddedValue(), std::greater<>{});
}

bool operator<=(const string &lhs, const SQLChar &rhs) {
  return rhs.comparatorHelper(lhs, rhs.getUnpaddedValue(), std::less_equal<>{});
}

bool operator>=(const string &lhs, const SQLChar &rhs) {
  return rhs.comparatorHelper(lhs, rhs.getUnpaddedValue(), std::greater_equal<>{});
}


////// SQLChar vs. String
bool SQLChar::operator==(const string &rhs) const {
  return comparatorHelper(getUnpaddedValue(), rhs, std::equal_to<>{});
} 

bool SQLChar::operator!=(const string &rhs) const {
  return comparatorHelper(getUnpaddedValue(), rhs, std::not_equal_to<>{});
}

bool SQLChar::operator<(const string &rhs) const {
  return comparatorHelper(getUnpaddedValue(), rhs, std::less<>{});
}

bool SQLChar::operator>(const string &rhs) const {
  return comparatorHelper(getUnpaddedValue(), rhs, std::greater<>{});
}

bool SQLChar::operator<=(const string &rhs) const {
  return comparatorHelper(getUnpaddedValue(), rhs, std::less_equal<>{});
}

bool SQLChar::operator>=(const string &rhs) const {
  return comparatorHelper(getUnpaddedValue(), rhs, std::greater_equal<>{});
} 



////// Varchar vs. SQLChar
bool operator==(const Varchar &lhs, const SQLChar &rhs) {
  return rhs.comparatorHelper(lhs.value, rhs.getUnpaddedValue(), std::equal_to<>{});
}

bool operator!=(const Varchar &lhs, const SQLChar &rhs) {
  return rhs.comparatorHelper(lhs.value, rhs.getUnpaddedValue(), std::not_equal_to<>{});
}

bool operator<(const Varchar &lhs, const SQLChar &rhs) {
  return rhs.comparatorHelper(lhs.value, rhs.getUnpaddedValue(), std::less<>{});
}

bool operator>(const Varchar &lhs, const SQLChar &rhs) {
  return rhs.comparatorHelper(lhs.value, rhs.getUnpaddedValue(), std::greater<>{});
}

bool operator<=(const Varchar &lhs, const SQLChar &rhs) {
  return rhs.comparatorHelper(lhs.value, rhs.getUnpaddedValue(), std::less_equal<>{});
}

bool operator>=(const Varchar &lhs, const SQLChar &rhs) {
  return rhs.comparatorHelper(lhs.value, rhs.getUnpaddedValue(), std::greater_equal<>{});
}

////// SQLChar vs. Varchar
bool operator==(const SQLChar &lhs, const Varchar &rhs) {
  return rhs.comparatorHelper(lhs.getUnpaddedValue(), rhs.value, std::equal_to<>{});
}

bool operator!=(const SQLChar &lhs, const Varchar &rhs) {
  return rhs.comparatorHelper(lhs.getUnpaddedValue(), rhs.value, std::not_equal_to<>{});
}

bool operator<(const SQLChar &lhs, const Varchar &rhs) {
  return rhs.comparatorHelper(lhs.getUnpaddedValue(), rhs.value, std::less<>{});
}

bool operator>(const SQLChar &lhs, const Varchar &rhs) {
  return rhs.comparatorHelper(lhs.getUnpaddedValue(), rhs.value, std::greater<>{});
}

bool operator<=(const SQLChar &lhs, const Varchar &rhs) {
  return rhs.comparatorHelper(lhs.getUnpaddedValue(), rhs.value, std::less_equal<>{});
}

bool operator>=(const SQLChar &lhs, const Varchar &rhs) {
  return rhs.comparatorHelper(lhs.getUnpaddedValue(), rhs.value, std::greater_equal<>{});
}

/////////////////////////// End comparator hell ////////////////////////