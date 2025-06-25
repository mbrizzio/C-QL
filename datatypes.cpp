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

// Fixed :)
void Date::epochToDate(){
  // Make a shitty loop and call it a day (for now)
  year = 1;
  int remainingDays = epoch;

  while (remainingDays > 366){
    remainingDays -= 365;
    remainingDays -= isLeapYear();

    ++year;
  }

  // Hardcode a solution right on the border of current year and previous
  // in case of a leap year.
  if (remainingDays == 366){
    if (isLeapYear()){
      month = 12;
      day = 31;
    }
    else{
      ++year;
      month = 1;
      day = 1;
    }

    return;
  }

  // Hardcode solution when the remaining days are 0, which indicates
  // the final day of the year
  if (remainingDays == 0){
    --year;
    month = 12;
    day = 31;

    return;
  }

  // Normal case, go through the months 
  month = 0;
  while (remainingDays > daysPerMonth[month]){
    remainingDays -= daysPerMonth[month];
    remainingDays -= (isLeapYear() && (month == 1)); // 1 becuase 0 indexed for now
    ++month;
  }

  if (remainingDays == 0) { //this will only happen if we are in february on a leap year
    day = 29;
    month = 2;
    return;
  }

  ++month;
  day = remainingDays;

}

bool Date::isLeapYear(int const currentYear) const {
  // The rule is: evey 4 years there is a leap years, multiples of 100 are skipped
  // UNLESS they are multiples of 400
  return (currentYear % 400 == 0 || 
         (currentYear % 4 == 0 && currentYear % 100 != 0));
}

bool Date::isLeapYear() const {
  return isLeapYear(year);
}

int Date::leapDaysUpToCurrentYear(int const currentYear) const {
  int leapYears = (currentYear - 1) / (int)4;

  // Remove multiples of 100
  leapYears -= (currentYear - 1) / (int)100;

  // Add multiples of 400
  leapYears += (currentYear - 1) / (int)400;

  return leapYears; 
}

int Date::leapDaysUpToCurrentYear() const {
  return leapDaysUpToCurrentYear(year);
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

Date Date::dateAdd(int difference, const DateComponents mode) const {
  Date newDate(1);
  switch (mode) {
    // Looking for it to fall through to days since the logic is the same
    // But we have 7 days instead of 1
    case DateComponents::WEEKS:
      difference *= 7;

    case DateComponents::DAYS: {
      int newEpoch = epoch + difference;

      Date newDateDays(newEpoch);

      newDate = newDateDays; 

      break;
    }
    
    // Looking for it to fall through to months since the logic is the same
    // but we have 3 months instead of 1
    case DateComponents::QUARTERS:
      difference *= 3;

    case DateComponents::MONTHS: {
      Date newDateMonths(year, month, day);
      // Step 1: deal with years and months
      // Deal with this tomorrow
      int leftover = difference % 12;
      if (leftover + month > 12){
        ++newDateMonths.year;
        newDateMonths.month = ((leftover + month) % 12);
      }
      else if (leftover + month <= 0) {
        --newDateMonths.year;
        newDateMonths.month = 12 + (month + leftover);
      }
      else {
        newDateMonths.month += leftover;
      }

      newDateMonths.year += difference / (int)12;

      // Step 2: deal with the day
      int maxDay = daysPerMonth[newDateMonths.month - 1];
      maxDay += ((newDateMonths.month == 2) && newDateMonths.isLeapYear());

      newDateMonths.day = min(day, maxDay);

      newDate = newDateMonths;
      newDate.dateToEpoch();

      break;
    }
    
    case DateComponents::YEARS: {
      Date newDateYears(year, month, day);

      newDateYears.year += difference;

      if ( (month == 2 && day == 29) && !newDateYears.isLeapYear() ) {
        newDateYears.day = 28;
      }
      
      newDate = newDateYears;
      newDate.dateToEpoch();

      break;
    }
  }  

  return newDate;
}

Date Date::dateSub(const int rhs, const DateComponents mode) const {
  return dateAdd(-rhs, mode);
}

int Date::extract(const DateComponents mode) const {
  int value = 0;

  switch (mode){
    case DateComponents::DAYS: {
      value = day;
      break;
    }

    case DateComponents::MONTHS: {
      value = month;
      break;
    }

    case DateComponents::QUARTERS: {
      value = ((month - 1) / (int)3) + 1;
      break;
    }

    case DateComponents::YEARS: {
      value = year;
      break;
    }

    case DateComponents::DAYOFYEAR: {
      for (int i = 0; i < month; ++i){
        value += daysPerMonth[i];
      }
      value += ((month > 2) && isLeapYear());

      value += day;
      break;
    }

  }

  return value;
}

//////////////////////// End Date ///////////////////////////////////////



//////////////////////////// Time ////////////////////////////////////////

Time::Time(int Precision) : second(0), minute(0), hour(0), duration(0), 
                            precision(Precision) {}

Time::Time() : Time(0) {}

Time::Time(double Duration, int Precision=0) : duration(Duration), precision(Precision) {
  durationToTime();
  enforceTimeInvariants();
}


// Only guaranteed to work if string format is H[H]:M[M]:S[S][.DDDDDD]
Time::Time(const string &HHMMSS, int Precision) : precision(Precision) {
  auto firstColon = find(HHMMSS.begin(), HHMMSS.end(), ':');
  hour = stoi(string(HHMMSS.begin(), firstColon));

  auto secondColon = find(firstColon+1, HHMMSS.end(), ':');
  minute = stoi(string(firstColon+1, secondColon));

  double fullSecond = stod(string(secondColon+1, HHMMSS.end()));
  second = (int)fullSecond;

  fraction = (int)((fullSecond - (double)second) * pow(10, precision)) % 1;
    
  timeToDuration();
  enforceTimeInvariants();
}

Time::Time(int Hour, int Minute, int Second, int Fraction=0, int Precision=0) :
           hour(Hour), minute(Minute), second(Second), precision(Precision) {
  fraction = second % (int)pow(10, Precision);

  timeToDuration();
  enforceTimeInvariants();
}

double Time::extract(const TimeComponents mode) const {
  double element = 0;

  switch (mode){
    case TimeComponents::FRACTIONS: {
      // 6 decimal precision is supported. This scheme lets us compare the
      // fractions of 2 numbers with different decimal precision easily
      element = fraction * (int)pow(10, 6 - precision); 
      break;
    }
    case TimeComponents::SECONDS: {
      element = second;
      break;
    }
    case TimeComponents::MINUTES: {
      element = minute;
      break;
    }
    case TimeComponents::HOURS: {
      element = hour;
      break;
    }
  }

  return element;
}

Time Time::timeAdd(int difference, const TimeComponents mode) const {
  double updatedDuration = duration;

  switch (mode) {
    // Assume the user is responsibly adding the decimal component
    case TimeComponents::FRACTIONS: {
      updatedDuration += (difference / pow(10, precision));
      break;
    }
    case TimeComponents::SECONDS: {
      updatedDuration += difference;
      break;
    }
    case TimeComponents::MINUTES: {
      updatedDuration += difference * 60;
    } 
    case TimeComponents::HOURS: {
      updatedDuration += difference * 3600;
    }
  }

  return Time((double)updatedDuration, precision);
}

Time Time::timeSub(int difference, const TimeComponents mode) const {
  return timeAdd(-difference, mode);
}

Time Time::timeDiff(const Time &rhs) {
  return Time((double)abs(duration - rhs.duration));
}

void Time::timeToDuration() {
  duration = hour * 3600 + minute * 60 + second;

  if (precision > 0) duration += fraction / (int)pow(10, precision);
}

void Time::durationToTime() {
  double witheredDuration = duration;

  hour = (u_int)(witheredDuration / 3600);
  witheredDuration -= hour * 3600;

  minute = (u_int)(witheredDuration / 60);
  witheredDuration -= hour * 60;

  second = (u_int)(witheredDuration / 1);
  witheredDuration -= minute * 1;

  fraction = witheredDuration;
}

void Time::enforceTimeInvariants() {
  if (second > 60){
    cerr << "Invalid second inputted to Time" << endl;
    exit(3);
  }
  if (minute > 60){
    cerr << "Invalid minute inputted to Time" << endl;
    exit(3);
  }
  if (hour > 24){
    cerr << "Invalid hour inputted to Time" << endl;
    exit(3);
  }
  if (fraction > (u_int)pow(10, precision)) {
    cerr << "Fraction is wrong somehow" << endl;
    exit(3);
  }
  if (precision > 6) {
    cerr << "Only up to 6 digits of decimal precision supported" << endl;
  }
}

////////////////////////// End Time ///////////////////////////////////////



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

// BUGGY, zeroes appear in the wrong side. how to fix?
ostream& operator<<(ostream& os, const Date& self){
  char oldFill = os.fill();

  os << setfill('0') << setw(4) << self.year << '-';
  os << setfill('0') << setw(2) << self.month << '-';
  os << setfill('0') << setw(2) << self.day;

  os.fill(oldFill);
  
  return os;
}

ostream& operator<<(ostream& os, const Time &self){
  char oldFill = os.fill();

  os << setfill('0') << setw(2) << self.hour << ':';
  os << setfill('0') << setw(2) << self.minute << ':';
  os << setfill('0') << setw(2) << self.second;
  if (self.precision > 0) os << setfill('0') << setw(self.precision) << self.fraction;
  
  os.fill(oldFill);

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

////// Arithmetic operators (all classes)

Date Date::operator+(const Date &rhs) const {
  return Date(epoch + rhs.epoch);
}

Date Date::operator-(const Date &rhs) const {
  return Date(epoch - rhs.epoch);
}

void Date::operator-=(const Date &rhs) {
  epoch -= rhs.epoch;
  epochToDate();
  enforceDateInvariants();
}

void Date::operator+=(const Date &rhs) {
  epoch += rhs.epoch;
  epochToDate();
  enforceDateInvariants();
}

////// Explicit class conversion (all classes)

Date::operator int() const {
  return epoch;
}

/////////////////////// Misc operators end //////////////////////////////



/////////////////////// Comparator hell (all classes) ///////////////////

////// Time vs. Time
bool Time::operator==(const Time &rhs) const {
  return duration == rhs.duration;
}

bool Time::operator!=(const Time &rhs) const {
  return duration != rhs.duration;
}

bool Time::operator<(const Time &rhs) const {
  return duration < rhs.duration;
}

bool Time::operator>(const Time &rhs) const {
  return duration > rhs.duration;
}

bool Time::operator<=(const Time &rhs) const {
  return duration <= rhs.duration;
}

bool Time::operator>=(const Time &rhs) const {
  return duration >= rhs.duration;
}

////// Date vs. Date
bool Date::operator==(const Date &rhs) const {
  return epoch == rhs.epoch;
}

bool Date::operator!=(const Date &rhs) const {
  return epoch != rhs.epoch;
}

bool Date::operator<(const Date &rhs) const {
  return epoch < rhs.epoch;
}

bool Date::operator>(const Date &rhs) const {
  return epoch > rhs.epoch;
}

bool Date::operator<=(const Date &rhs) const {
  return epoch <= rhs.epoch;
}

bool Date::operator>=(const Date &rhs) const {
  return epoch >= rhs.epoch;
}


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