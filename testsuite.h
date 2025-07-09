#pragma once
#include "datatypes.h"
#include "column.h"
#include "gtest/gtest.h" // Or your favorite C++ testing framework
#include <numeric>
#include <stdexcept>

#define C_GREEN   "\x1B[32m"
#define C_RED     "\x1B[31m"
#define C_YELLOW  "\x1B[33m"
#define C_BLUE    "\x1B[34m"
#define C_RESET   "\x1B[0m"

using namespace std;

void varcharComparatorCorrectnessTests() {
    Varchar a(10, "apple");
    Varchar b(20, "banana");
    std::string s1 = "apple";
    std::string s2 = "banana";

    cout << a << b << endl;

    // Varchar vs Varchar
    assert(a == Varchar(10, "apple"));
    assert(a != b);
    assert(a < b);
    assert(a <= b);
    assert(b > a);
    assert(b >= a);

    // std::string vs Varchar
    assert(s1 == a);
    assert(s1 != b);
    assert(s1 < b);
    assert(s1 <= b);
    assert(s2 > a);
    assert(s2 >= a);

    // Varchar vs std::string
    assert(a == s1);
    assert(a != s2);
    assert(a < s2);
    assert(a <= s2);
    assert(b > s1);
    assert(b >= s1);
}

void charComparatorCorrectnessTests() {
    SQLChar a(10, "apple");
    SQLChar b(20, "banana");
    Varchar c(10, "apple");
    Varchar d(20, "banana");
    std::string s1 = "apple";
    std::string s2 = "banana";

    cout << a << b << endl;

    // Char vs Char
    assert(a == SQLChar(10, "apple"));
    assert(a != b);
    assert(a < b);
    assert(a <= b);
    assert(b > a);
    assert(b >= a);

    // std::string vs Char
    assert(s1 == a);
    assert(s1 != b);
    assert(s1 < b);
    assert(s1 <= b);
    assert(s2 > a);
    assert(s2 >= a);

    // Char vs std::string
    assert(a == s1);
    assert(a != s2);
    assert(a < s2);
    assert(a <= s2);
    assert(b > s1);
    assert(b >= s1);

    // Varchar vs. Char
    assert(c == SQLChar(10, "apple"));
    assert(c != b);
    assert(c < b);
    assert(c <= b);
    assert(d > a);
    assert(d >= a);

    // Char vs. Varchar
    assert(a == Varchar(10, "apple"));
    assert(a != d);
    assert(a < d);
    assert(a <= d);
    assert(b > c);
    assert(b >= c);
}

void test_integer_and_float_cross_comparisons() {
    int i_val = 42;
    int16_t i16_val = 42;
    int64_t i64_val = 42;
    float f_val = 42.0f;

    Types t_int = i_val;
    Types t_int16 = i16_val;
    Types t_int64 = i64_val;
    Types t_float = f_val;

    // Helper lambda to test all 6 comparisons both ways
    auto test_all_ops = [](auto&& lhs, auto&& rhs) {
        // lhs == rhs and rhs == lhs
        assert((lhs == rhs) == true);
        assert((rhs == lhs) == true);

        // lhs != rhs and rhs != lhs
        assert((lhs != rhs) == false);
        assert((rhs != lhs) == false);

        // lhs < rhs and rhs > lhs
        assert((lhs < rhs) == false);
        assert((rhs > lhs) == false);

        // lhs <= rhs and rhs >= lhs
        assert((lhs <= rhs) == true);
        assert((rhs >= lhs) == true);
    };

    // Equal value comparisons
    test_all_ops(t_int, i_val);
    test_all_ops(t_int16, i16_val);
    test_all_ops(t_int64, i64_val);
    test_all_ops(t_float, f_val);

    test_all_ops(i_val, t_int);
    test_all_ops(i16_val, t_int16);
    test_all_ops(i64_val, t_int64);
    test_all_ops(f_val, t_float);

    // Cross type equality comparisons with same value (42)
    test_all_ops(t_int, i16_val);
    test_all_ops(i16_val, t_int);

    test_all_ops(t_int, i64_val);
    test_all_ops(i64_val, t_int);

    test_all_ops(t_int16, i64_val);
    test_all_ops(i64_val, t_int16);

    test_all_ops(t_int64, i_val);
    test_all_ops(i_val, t_int64);

    test_all_ops(t_int64, i16_val);
    test_all_ops(i16_val, t_int64);

    // Also test Types against Types with different integer types holding same value
    test_all_ops(t_int, t_int16);
    test_all_ops(t_int16, t_int64);
    test_all_ops(t_int64, t_int);

    // Now test ordering with different values
    Types t_int_10 = 10;
    Types t_int16_50 = int16_t(50);
    Types t_int64_100 = int64_t(100);
    Types t_float_75 = 75.0f;

    // 10 < 50
    assert((t_int_10 < t_int16_50) == true);
    assert((t_int16_50 > t_int_10) == true);

    // 50 <= 50
    assert((t_int16_50 <= int16_t(50)) == true);
    assert((int16_t(50) >= t_int16_50) == true);

    // 100 > 75
    assert((t_int64_100 > t_float_75) == true);
    assert((t_float_75 < t_int64_100) == true);

    // Mixed directions with primitives
    assert((int(10) < t_int16_50) == true);
    assert((t_int16_50 > int(10)) == true);

    assert((int64_t(100) > t_float_75) == true);
    assert((t_float_75 < int64_t(100)) == true);
}

void test_types_vs_types() {
    assert(Types(10) == Types(10));
    assert(Types(int16_t(5)) == Types(int16_t(5)));
    assert(Types(int64_t(123456789)) == Types(int64_t(123456789)));
    assert(Types(3.14f) == Types(3.14f));
    assert(Types(string("hello")) == Types(string("hello")));

    assert(Types(Varchar("abc")) == Types(Varchar("abc")));
    assert(Types(SQLChar("xyz")) == Types(SQLChar("xyz")));

    // Cross-type but logically equal
    assert((Types(10) == Types(int64_t(10))));
    assert((Types(10) == Types(float(10))));

    assert(Types(Varchar("abc")) == Types(SQLChar("abc")));
    assert(Types(Varchar("abc")) == Types(string("abc")));
    assert(Types(SQLChar("abc")) == Types(string("abc")));

    // Negative cases
    assert(!(Types(10) == Types(20)));
    assert(!(Types(Varchar("abc")) == Types(Varchar("xyz"))));
    assert(!(Types(SQLChar("abc")) == Types(Varchar("xyz"))));
    assert(!(Types(string("abc")) == Types(SQLChar("xyz"))));
}

void test_types_vs_any() {
    Types a = 10;
    Types b = string("abc");
    Types c = Varchar("abc");
    Types d = SQLChar("abc");
    Types null = monostate{};

    // int vs. Types
    assert(a == 10);
    assert(10 == a);
    assert(!(a == 20));
    assert(a != 20);
    assert(!(10 != a));

    // string vs. Types
    assert(b == string("abc"));
    assert(string("abc") == b);
    assert(b != string("xyz"));
    assert(string("xyz") != b);

    // Varchar, SQLChar, string (inter-compatible)
    assert(c == string("abc"));
    assert(string("abc") == c);
    assert(c == SQLChar("abc"));
    assert(SQLChar("abc") == c);

    assert(!(c == string("xyz")));
    assert(c != string("xyz"));

    // NULL
    assert(null != 10);
    assert(null != string("abc"));
    assert(null == null);
    assert(!(null == 10));
    assert((null == null));
}

void test_any_vs_types() {
    // This is symmetric with test_types_vs_any but from the "other side"

    Types i = 42;
    Types s = string("def");
    Types v = Varchar("123");
    Types ch = SQLChar("123");
    Types n = monostate{};

    // int vs. Types
    assert(42 == i);
    assert(!(50 == i));
    assert(50 != i);

    // string vs. Types
    assert(string("def") == s);
    assert(string("abc") != s);

    // Varchar and SQLChar
    assert(string("123") == v);
    assert(Varchar("123") == ch);
    assert(SQLChar("123") == v);
    assert(SQLChar("999") != v);

    // NULL
    assert(10 != n);
    assert(!(10 == n));
    assert(n != monostate{});
    assert(!(n == monostate{}));
}

void runTypesComparisonTests() {
    test_types_vs_types();
    test_types_vs_any();
    test_any_vs_types();
    test_integer_and_float_cross_comparisons();
}

// A structure to hold all the information for a single test case.
struct TestCase {
    int year;
    int month;
    int day;
    long expected_epoch;
    std::string description;
};

void dateUnitTests() {
    std::cout << C_BLUE << "Starting Date class test suite..." << C_RESET << std::endl;
    std::cout << "-----------------------------------" << std::endl;

    // A collection of test cases, including simple ones and tricky edge cases.
    const std::vector<TestCase> testCases = {
        // --- Basic Cases ---
        {1, 1, 1, 1, "The first day of the epoch"},
        {1, 3, 5, 64, "Simple date in the first year"},
        {2, 1, 1, 366, "First day of the second year"},
        
        // --- Standard Leap Year (divisible by 4) ---
        {2024, 2, 28, 738944, "Day before a standard leap day"},
        {2024, 2, 29, 738945, "A standard leap day"},
        {2024, 3, 1, 738946, "Day after a standard leap day"},
        {2024, 12, 31, 739251, "End of a standard leap year"},
        {2025, 1, 1, 739252, "Start of year after leap year"},

        // --- Century Non-Leap Year (divisible by 100 but not 400) ---
        {1900, 2, 28, 693654, "Day before a 'skipped' leap day"},
        {1900, 3, 1, 693655, "Day after a 'skipped' leap day"}, // Epoch must be consecutive
        {1900, 12, 31, 693960, "End of a century non-leap year"},
        
        // --- Quadricentennial Leap Year (divisible by 400) ---
        {2000, 2, 28, 730178, "Day before a quadricentennial leap day"},
        {2000, 2, 29, 730179, "A quadricentennial leap day"},
        {2000, 3, 1, 730180, "Day after a quadricentennial leap day"},
        
        // --- Boundary and Recent Dates ---
        {2023, 12, 31, 738885, "End of a recent common year"},
        {2024, 1, 1, 738886, "Start of a recent leap year"},
        {2025, 6, 21, 739423, "A random recent date"},

        // --- Far Future Date ---
        {9999, 12, 31, 3652059, "Maximum date in many systems"}
    };

    int passed_count = 0;
    int total_tests = testCases.size() * 2; // Each case has two tests (YMD->E and E->YMD)

    for (const auto& tc : testCases) {
        bool test1_passed = false;
        bool test2_passed = false;

        std::cout << C_YELLOW << "\nTesting Case: " << tc.description << " ("
                  << std::setfill('0') << std::setw(4) << tc.year << "-" 
                  << std::setw(2) << tc.month << "-" << std::setw(2) << tc.day << ")" << C_RESET << std::endl;

        // --- Test 1: Instantiate with Y-M-D and check epoch ---
        try {
            Date d_from_ymd(tc.year, tc.month, tc.day);
            long actual_epoch = d_from_ymd.epoch;
            if (actual_epoch == tc.expected_epoch) {
                std::cout << C_GREEN << "  [PASS]" << C_RESET << " YMD -> Epoch. Correct epoch calculated: " << actual_epoch << std::endl;
                test1_passed = true;
            } else {
                std::cout << C_RED << "  [FAIL]" << C_RESET << " YMD -> Epoch. Expected " << tc.expected_epoch << " but got " << actual_epoch << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << C_RED << "  [FAIL]" << C_RESET << " YMD -> Epoch. Threw an unexpected exception: " << e.what() << std::endl;
        }

        // --- Test 2: Instantiate with epoch and check Y-M-D string ---
        try {
            Date d_from_epoch(tc.expected_epoch);
            std::stringstream ss;
            ss << d_from_epoch;
            std::string actual_ymd = ss.str();

            std::stringstream expected_ss;
            expected_ss << std::setfill('0') << std::setw(4) << tc.year << '-'
                        << std::setfill('0') << std::setw(2) << tc.month << '-'
                        << std::setfill('0') << std::setw(2) << tc.day;
            std::string expected_ymd = expected_ss.str();

            if (actual_ymd == expected_ymd) {
                std::cout << C_GREEN << "  [PASS]" << C_RESET << " Epoch -> YMD. Correct date generated: " << actual_ymd << std::endl;
                test2_passed = true;
            } else {
                std::cout << C_RED << "  [FAIL]" << C_RESET << " Epoch -> YMD. Expected " << expected_ymd << " but got " << actual_ymd << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << C_RED << "  [FAIL]" << C_RESET << " Epoch -> YMD. Threw an unexpected exception: " << e.what() << std::endl;
        }
        
        if (test1_passed) passed_count++;
        if (test2_passed) passed_count++;
    }

    // --- Final Summary ---
    std::cout << "\n-----------------------------------" << std::endl;
    std::cout << C_BLUE << "Test Suite Finished." << C_RESET << std::endl;
    if (passed_count == total_tests) {
        std::cout << C_GREEN << "Summary: All " << total_tests << " tests passed!" << C_RESET << std::endl;
    } else {
        std::cout << C_RED << "Summary: " << passed_count << " / " << total_tests << " tests passed." << C_RESET << std::endl;
    }
}

// Enum to define the expected relationship between two dates in a test case.
enum class Relationship { EQUAL, LESS_THAN, GREATER_THAN };

// A structure to hold all the information for a single comparison test case.
struct ComparisonTestCase {
    Date date1;
    Date date2;
    Relationship expected_relationship;
    std::string description;
};

// --- Test Runner ---

void dateComparisonTestSuite() {
    std::cout << C_BLUE << "Starting Date comparator test suite..." << C_RESET << std::endl;
    std::cout << "---------------------------------------" << std::endl;

    const std::vector<ComparisonTestCase> testCases = {
        { {2025, 6, 21}, {2025, 6, 21}, Relationship::EQUAL, "Two identical dates" },
        { {2025, 6, 20}, {2025, 6, 21}, Relationship::LESS_THAN, "One day apart" },
        { {2025, 6, 22}, {2025, 6, 21}, Relationship::GREATER_THAN, "One day apart (reversed)" },
        { {2025, 2, 28}, {2025, 3, 1}, Relationship::LESS_THAN, "Crossing a month boundary" },
        { {2024, 12, 31}, {2025, 1, 1}, Relationship::LESS_THAN, "Crossing a year boundary" },
        { {2024, 2, 28}, {2024, 2, 29}, Relationship::LESS_THAN, "Crossing a leap day" },
        { {2024, 2, 29}, {2024, 3, 1}, Relationship::LESS_THAN, "Leap day vs next day" },
        { {1900, 2, 28}, {1900, 3, 1}, Relationship::LESS_THAN, "Century non-leap boundary" },
        { {2000, 2, 29}, {2000, 3, 1}, Relationship::LESS_THAN, "Century leap boundary" },
        { {1, 1, 1}, {9999, 12, 31}, Relationship::LESS_THAN, "Distant past vs distant future" }
    };

    int tests_run = 0;
    int passed_count = 0;

    for (const auto& tc : testCases) {
        std::cout << C_YELLOW << "\nTesting Case: " << tc.description << C_RESET << std::endl;
        
        bool expected_eq, expected_ne, expected_lt, expected_gt, expected_le, expected_ge;

        if (tc.expected_relationship == Relationship::EQUAL) {
            expected_eq = true; expected_ne = false; expected_lt = false;
            expected_gt = false; expected_le = true; expected_ge = true;
        } else if (tc.expected_relationship == Relationship::LESS_THAN) {
            expected_eq = false; expected_ne = true; expected_lt = true;
            expected_gt = false; expected_le = true; expected_ge = false;
        } else { // GREATER_THAN
            expected_eq = false; expected_ne = true; expected_lt = false;
            expected_gt = true; expected_le = false; expected_ge = true;
        }

        const auto run_check = [&](const std::string& op, bool actual, bool expected) {
            tests_run++;
            std::cout << "  " << std::setw(25) << std::left << (tc.description + " (" + op + ") ");
            if (actual == expected) {
                std::cout << C_GREEN << "[PASS]" << C_RESET << std::endl;
                passed_count++;
            } else {
                std::cout << C_RED << "[FAIL]" << C_RESET << " -> Expected " << std::boolalpha << expected << ", got " << actual << std::endl;
            }
        };

        run_check("==", tc.date1 == tc.date2, expected_eq);
        run_check("!=", tc.date1 != tc.date2, expected_ne);
        run_check("<",  tc.date1 <  tc.date2, expected_lt);
        run_check(">",  tc.date1 >  tc.date2, expected_gt);
        run_check("<=", tc.date1 <= tc.date2, expected_le);
        run_check(">=", tc.date1 >= tc.date2, expected_ge);
    }
    
    // --- Final Summary ---
    std::cout << "\n---------------------------------------" << std::endl;
    std::cout << C_BLUE << "Comparator Test Suite Finished." << C_RESET << std::endl;
    if (passed_count == tests_run) {
        std::cout << C_GREEN << "Summary: All " << tests_run << " comparator tests passed!" << C_RESET << std::endl;
    } else {
        std::cout << C_RED << "Summary: " << passed_count << " / " << tests_run << " tests passed." << C_RESET << std::endl;
    }
}

struct ArithmeticTestCase {
    std::string description;
    std::function<Date()> test_function;
    Date expected_date;
};

void dateArithmeticTestSuite() {
    std::cout << C_BLUE << "Starting Date arithmetic test suite..." << C_RESET << std::endl;
    std::cout << "--------------------------------------" << std::endl;

    const std::vector<ArithmeticTestCase> testCases = {
        // --- DAY Arithmetic ---
        {"DAY: Add 10 days, no boundary", []{ return Date(2025, 6, 10).dateAdd(10, DateComponents::DAYS); }, Date(2025, 6, 20)},
        {"DAY: Subtract 10 days, cross year", []{ return Date(2025, 1, 5).dateSub(10, DateComponents::DAYS); }, Date(2024, 12, 26)},
        
        // --- WEEK Arithmetic ---
        {"WEEK: Add 2 weeks", []{ return Date(2025, 3, 1).dateAdd(2, DateComponents::WEEKS); }, Date(2025, 3, 15)},
        {"WEEK: Subtract 1 week across month", []{ return Date(2025, 3, 5).dateSub(1, DateComponents::WEEKS); }, Date(2025, 2, 26)},

        // --- MONTH Arithmetic ---
        {"MONTH: Add 1 month, simple", []{ return Date(2025, 1, 15).dateAdd(1, DateComponents::MONTHS); }, Date(2025, 2, 15)},
        {"MONTH: Subtract 1 month, cross year", []{ return Date(2025, 1, 15).dateSub(1, DateComponents::MONTHS); }, Date(2024, 12, 15)},
        {"MONTH: Add 12 months (same as 1 year)", []{ return Date(2024, 2, 29).dateAdd(12, DateComponents::MONTHS); }, Date(2025, 2, 28)},
        {"MONTH: [EDGE] Add 1 month to Jan 31", []{ return Date(2025, 1, 31).dateAdd(1, DateComponents::MONTHS); }, Date(2025, 2, 28)},
        {"MONTH: [EDGE] Add 1 month to Jan 31 (leap)", []{ return Date(2024, 1, 31).dateAdd(1, DateComponents::MONTHS); }, Date(2024, 2, 29)},
        {"MONTH: [EDGE] Subtract 1 month from Mar 31", []{ return Date(2025, 3, 31).dateSub(1, DateComponents::MONTHS); }, Date(2025, 2, 28)},
        
        // --- QUARTER Arithmetic ---
        {"QUARTER: Add 1 quarter (3 months)", []{ return Date(2025, 1, 31).dateAdd(1, DateComponents::QUARTERS); }, Date(2025, 4, 30)},
        {"QUARTER: Subtract 1 quarter, cross year", []{ return Date(2025, 2, 28).dateSub(1, DateComponents::QUARTERS); }, Date(2024, 11, 28)},
        {"QUARTER: [EDGE] Add 1 Q to Oct 31 (leap)", []{ return Date(2023, 10, 31).dateAdd(1, DateComponents::QUARTERS); }, Date(2024, 1, 31)},
        {"QUARTER: [EDGE] Add 1 Q to Nov 30 (leap)", []{ return Date(2023, 11, 30).dateAdd(1, DateComponents::QUARTERS); }, Date(2024, 2, 29)},

        // --- YEAR Arithmetic ---
        {"YEAR: Add 5 years, simple", []{ return Date(2025, 6, 21).dateAdd(5, DateComponents::YEARS); }, Date(2030, 6, 21)},
        {"YEAR: [EDGE] Add 1 year to leap day", []{ return Date(2024, 2, 29).dateAdd(1, DateComponents::YEARS); }, Date(2025, 2, 28)},
        {"YEAR: [EDGE] Add 4 years to leap day", []{ return Date(2024, 2, 29).dateAdd(4, DateComponents::YEARS); }, Date(2028, 2, 29)},
        {"YEAR: [EDGE] Non-leap century boundary", []{ return Date(1899, 5, 5).dateAdd(1, DateComponents::YEARS); }, Date(1900, 5, 5)},
        {"YEAR: [EDGE] Leap century boundary", []{ return Date(1999, 2, 1).dateAdd(1, DateComponents::YEARS); }, Date(2000, 2, 1)},
    };

    int passed_count = 0;
    for (const auto& tc : testCases) {
        std::cout << C_YELLOW << "Testing: " << std::left << std::setw(40) << tc.description << C_RESET;
        try {
            Date result = tc.test_function();
            if (result == tc.expected_date) {
                std::cout << C_GREEN << "[PASS]" << C_RESET << " -> Got " << result << std::endl;
                passed_count++;
            } else {
                std::cout << C_RED << "[FAIL]" << C_RESET << " -> Expected " << tc.expected_date << ", but got " << result << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << C_RED << "[FAIL]" << C_RESET << " -> Threw an unexpected exception: " << e.what() << std::endl;
        }
    }
    
    // --- Final Summary ---
    std::cout << "\n--------------------------------------" << std::endl;
    std::cout << C_BLUE << "Arithmetic Test Suite Finished." << C_RESET << std::endl;
    if (passed_count == testCases.size()) {
        std::cout << C_GREEN << "Summary: All " << testCases.size() << " tests passed!" << C_RESET << std::endl;
    } else {
        std::cout << C_RED << "Summary: " << passed_count << " / " << testCases.size() << " tests passed." << C_RESET << std::endl;
    }
}

void testConstructors() {
    cout << "Running Constructor Tests..." << endl;

    // Test 1: Default constructor
    Time t1;
    assert(t1.hour == 0 && t1.minute == 0 && t1.second == 0 && t1.precision == 0);

    // Test 2: Precision constructor
    Time t2(4);
    assert(t2.hour == 0 && t2.minute == 0 && t2.second == 0 && t2.precision == 4);

    // Test 3: Component constructor (basic)
    Time t3(14, 30, 5, 123, 3);
    assert(t3.hour == 14 && t3.minute == 30 && t3.second == 5);
    assert(t3.fraction == 123 && t3.precision == 3); // Logic in your constructor seems to have a bug here

    // Test 4: Component constructor (edge case: midnight)
    Time t4(0, 0, 0);
    assert(t4.hour == 0 && t4.minute == 0 && t4.second == 0);

    // Test 5: String constructor (basic)
    Time t5("16:05:23");
    assert(t5.hour == 16 && t5.minute == 5 && t5.second == 23 && t5.precision == 6);

    // Test 6: String constructor (with fractions)
    Time t6("09:01:02.123456", 6);
    assert(t6.hour == 9 && t6.minute == 1 && t6.second == 2);
    assert(t6.fraction == 123456); // Logic in your string ctor seems to have a bug here

    // Test 7: Duration constructor
    double dur = 10 * 3600 + 20 * 60 + 30.5; // 10:20:30.5
    Time t7(dur, 1);
    assert(t7.hour == 10 && t7.minute == 20 && t7.second == 30);
    assert(t7.fraction == 5); // Logic in durationToTime seems to have bugs

    cout << "Constructor Tests Passed!" << endl;
}

void testComparisonOperators() {
    cout << "Running Comparison Operator Tests..." << endl;
    Time t1(10, 20, 30, 500, 3);
    Time t2(10, 20, 30, 500, 3);
    Time t3(11, 0, 0);
    Time t4(10, 21, 0);
    Time t5(10, 20, 31);
    Time t6(10, 20, 30, 501, 3);

    // Equality
    assert(t1 == t2);
    assert(!(t1 == t3));
    assert(t1 != t3);

    // Less than
    assert(t1 < t3); // by hour
    assert(t1 < t4); // by minute
    assert(t1 < t5); // by second
    assert(t1 < t6); // by fraction

    // Greater than
    assert(t3 > t1);
    assert(t4 > t1);
    assert(t5 > t1);
    assert(t6 > t1);

    // Less than or equal
    assert(t1 <= t2);
    assert(t1 <= t3);

    // Greater than or equal
    assert(t1 >= t2);
    assert(t3 >= t1);

    cout << "Comparison Operator Tests Passed!" << endl;
}

void testExtractMethod() {
    cout << "Running extract() Method Tests..." << endl;
    Time t1(15, 45, 12, 9876, 4);

    assert(t1.extract(TimeComponents::HOURS) == 15);
    assert(t1.extract(TimeComponents::MINUTES) == 45);
    assert(t1.extract(TimeComponents::SECONDS) == 12);
    // assert(t1.extract(TimeComponents::FRACTIONS) == 987600); // 9876 * 10^(6-4)
    
    Time t_zero;
    assert(t_zero.extract(TimeComponents::HOURS) == 0);
    assert(t_zero.extract(TimeComponents::MINUTES) == 0);
    assert(t_zero.extract(TimeComponents::SECONDS) == 0);
    assert(t_zero.extract(TimeComponents::FRACTIONS) == 0);

    cout << "extract() Method Tests Passed!" << endl;
}

void testArithmeticMethods() {
    cout << "Running Arithmetic Method Tests..." << endl;

    // timeAdd
    Time base(10, 59, 58);
    Time res1 = base.timeAdd(3, TimeComponents::SECONDS); // Should roll over minute and second
    assert(res1.hour == 11 && res1.minute == 0 && res1.second == 1);

    Time res2 = base.timeAdd(1, TimeComponents::MINUTES); // Should roll over hour
    assert(res2.hour == 11 && res2.minute == 0 && res2.second == 58);

    Time res3 = base.timeAdd(14, TimeComponents::HOURS); // Should wrap around 24 hours
    assert(res3.hour == 24 && res3.minute == 59 && res3.second == 58);

    // timeSub
    // Time base2(1, 0, 1);
    // Time res4 = base2.timeSub(3, TimeComponents::SECONDS); // Should borrow from minute and hour
    // assert(res4.hour == 0 && res4.minute == 59 && res4.second == 58);

    // Time midnight(0,0,0);
    // Time res5 = midnight.timeSub(1, TimeComponents::SECONDS); // Should wrap to previous day
    // assert(res5.hour == 23 && res5.minute == 59 && res5.second == 59);

    // timeDiff
    Time t_a(10, 30, 0);
    Time t_b(10, 30, 15);
    Time diff1 = t_a.timeDiff(t_b);
    assert(diff1.duration == 15.0);
    
    Time t_c(12, 0, 0);
    Time t_d(11, 0, 0);
    Time diff2 = t_c.timeDiff(t_d);
    assert(diff2.duration == 3600.0);

    Time diff3 = t_d.timeDiff(t_c); // Should be absolute
    assert(diff3.duration == 3600.0);

    cout << "Arithmetic Method Tests Passed!" << endl;
}

// void testOutputStream() {
//     cout << "Running ostream<< Operator Tests..." << endl;
    
//     Time t1(8, 5, 3, 0, 0);
//     string sVer = t1.toString();
//     assert(sVer == "08:05:03");

//     Time t2(23, 59, 59, 0, 0);
//     assert(t2.toString() == "23:59:59");
    
//     Time t3(14, 30, 5, 123, 3);
//     // assert(t3.toString() == "14:30:05.123"); // Depends on correct fraction implementation

//     Time t4(10, 20, 30, 50, 6);
//     // assert(t4.toString() == "10:20:30.000050"); // Test padding

//     cout << "ostream<< Operator Tests Passed!" << endl;
// }

// NOTE: Tests for invalid inputs that call exit() are hard to write in a simple
// test suite. A more advanced framework like Google Test can catch such exits.
// For now, these tests would need to be run manually to confirm they cause an exit.
void testInvalidInputs() {
    cout << "Running Invalid Input Tests (manual check required)..." << endl;
    // Example cases that should cause your program to exit(3)
    // To test, uncomment one line at a time and run.
    
    // Time invalid_s("10:30:99");
    // Time invalid_m("10:99:30");
    // Time invalid_h("25:30:30");
    // Time invalid_comp(10, 70, 20);

    cout << "Manual check for invalid inputs complete." << endl;
}


int timeTestCases() {
    cout << "--- Starting Time Class Test Suite ---" << endl;
    try {
        // Run all test suites
        testConstructors();
        testComparisonOperators();
        testExtractMethod();
        testArithmeticMethods();
        //testOutputStream();
        testInvalidInputs();

        cout << "\n--- All Tests Successfully Completed ---" << endl;
    }
    catch (const exception& e) {
        cerr << "\n--- A test failed with an exception: " << e.what() << " ---" << endl;
        return 1;
    }
    catch (...) {
        cerr << "\n--- A test failed with an unknown exception ---" << endl;
        return 1;
    }
    
    return 0;
}

template<typename T>
void run_test(const string& desc, T actual, T expected) {
    cout << C_YELLOW << "Testing: " << left << setw(50) << desc << C_RESET;
    if (actual == expected) {
        cout << C_GREEN << "[PASS]" << C_RESET << " -> Got " << actual << endl;
    } else {
        cout << C_RED << "[FAIL]" << C_RESET << " -> Expected " << expected << ", but got " << actual << endl;
    }
}

int datetimeUnitTests() {
    cout << C_BLUE << "Starting Datetime test suite (validating your implementation)..." << C_RESET << endl;

    cout << C_YELLOW << "\n--- Section: Construction ---" << C_RESET << endl;
    run_test("Constructor from string", Datetime("2024-07-29 10:30:00"), Datetime(Date(2024, 7, 29), Time(10, 30, 0)));
    run_test("Constructor from Date/Time parts", Datetime(Date(2025, 1, 1), Time(0, 0, 0)), Datetime("2025-01-01 00:00:00"));

    cout << C_YELLOW << "\n--- Section: DATEADD Behavior ---" << C_RESET << endl;
    run_test("datetimeAdd: 2 hours (no rollover)", Datetime("2025-06-21 10:00:00").datetimeAdd(2, TimeComponents::HOURS), Datetime("2025-06-21 12:00:00"));
    //run_test("datetimeAdd: 25 hours (day rollover)", Datetime("2025-06-21 10:00:00").datetimeAdd(25, TimeComponents::HOURS), Datetime("2025-06-22 11:00:00"));
    run_test("datetimeSub: 12 hours (day rollover)", Datetime("2025-06-21 10:00:00").datetimeSub(12, TimeComponents::HOURS), Datetime("2025-06-20 22:00:00"));
    run_test("datetimeAdd: 1 month (end-of-month)", Datetime("2025-01-31 12:00:00").datetimeAdd(1, DateComponents::MONTHS), Datetime("2025-02-28 12:00:00"));
    run_test("datetimeSub: 1 month (end-of-month)", Datetime("2025-03-31 12:00:00").datetimeSub(1, DateComponents::MONTHS), Datetime("2025-02-28 12:00:00"));
    run_test("datetimeAdd: 1 month (leap target)", Datetime("2024-01-31 12:00:00").datetimeAdd(1, DateComponents::MONTHS), Datetime("2024-02-29 12:00:00"));
    run_test("datetimeAdd: 1 year (from leap day)", Datetime("2024-02-29 10:00:00").datetimeAdd(1, DateComponents::YEARS), Datetime("2025-02-28 10:00:00"));
    run_test("datetimeSub: 1 year (to leap day)", Datetime("2025-02-28 10:00:00").datetimeSub(1, DateComponents::YEARS), Datetime("2024-02-28 10:00:00"));
    run_test("datetimeAdd: 4 years (leap to leap)", Datetime("2024-02-29 10:00:00").datetimeAdd(4, DateComponents::YEARS), Datetime("2028-02-29 10:00:00"));
    run_test("datetimeAdd: 1 Quarter to Oct 31", Datetime("2023-10-31 00:00:00").datetimeAdd(1, DateComponents::QUARTERS), Datetime("2024-01-31 00:00:00"));

    cout << C_YELLOW << "\n--- Section: DATEDIFF Behavior ---" << C_RESET << endl;
    run_test("DATEDIFF(YEAR) across boundary", dateDiff(Datetime("2024-12-31 23:59:59"), Datetime("2025-01-01 00:00:00"), DateComponents::YEARS), 1);
    run_test("DATEDIFF(YEAR) almost 2 years", dateDiff(Datetime("2024-01-01 00:00:00"), Datetime("2025-12-31 23:59:59"), DateComponents::YEARS), 1);
    run_test("DATEDIFF(MONTH) across boundary", dateDiff(Datetime("2025-01-31 23:59:59"), Datetime("2025-02-01 00:00:00"), DateComponents::MONTHS), 1);
    run_test("DATEDIFF(DAY) across boundary", dateDiff(Datetime("2025-02-10 23:59:59"), Datetime("2025-02-11 00:00:00"), DateComponents::DAYS), 1);
    run_test("DATEDIFF(HOUR) across boundary", dateDiff(Datetime("2025-06-21 10:59:59"), Datetime("2025-06-21 11:00:00"), TimeComponents::HOURS), 1);
    run_test("DATEDIFF(HOUR) over full year", dateDiff(Datetime("2020-01-01 00:00:00"), Datetime("2021-01-01 00:00:00"), TimeComponents::HOURS), 8784);
    run_test("DATEDIFF(MINUTE) across boundary", dateDiff(Datetime("2025-01-01 10:00:59"), Datetime("2025-01-01 10:01:00"), TimeComponents::MINUTES), 1);
    
    cout << C_YELLOW << "\n--- Section: EXTRACT Behavior ---" << C_RESET << endl;
    Datetime dt_extract("2024-02-29 15:45:30");
    run_test("EXTRACT(YEAR)", dt_extract.extract(DateComponents::YEARS), 2024);
    run_test("EXTRACT(MONTH)", dt_extract.extract(DateComponents::MONTHS), 2);
    run_test("EXTRACT(DAY)", dt_extract.extract(DateComponents::DAYS), 29);
    run_test("EXTRACT(HOUR)", dt_extract.extract(TimeComponents::HOURS), 15.0);
    run_test("EXTRACT(MINUTE)", dt_extract.extract(TimeComponents::MINUTES), 45.0);
    run_test("EXTRACT(SECOND)", dt_extract.extract(TimeComponents::SECONDS), 30.0);

    cout << "\n------------------------------------------------------------------" << endl;
    cout << C_BLUE << "Datetime Test Suite Finished." << C_RESET << endl;

    return 0;
}


// // Mock or stub implementations for Varchar, SQLChar, Date, Time, Datetime if they are complex classes
// // For this test suite, we'll assume they have appropriate constructors and operators.

// // Helper to create a vector of all indices for a column
// std::vector<int> all_indices(const Column& col) {
//     std::vector<int> indices(col.size());
//     std::iota(indices.begin(), indices.end(), 0);
//     return indices;
// }

// //=================================================================================================
// // 1. CONSTRUCTOR AND BASIC SETUP TESTS
// //=================================================================================================

// TEST(ColumnTest, Constructor_EmptyWithType) {
//     // Tests creating an empty column of a specific data type.
//     Column int_col(Datatypes::INT);
//     ASSERT_EQ(int_col.size(), 0);
//     ASSERT_EQ(int_col.type, Datatypes::INT);

//     Column text_col(Datatypes::TEXT);
//     ASSERT_EQ(text_col.size(), 0);
//     ASSERT_EQ(text_col.type, Datatypes::TEXT);
// }

// TEST(ColumnTest, Constructor_FromVector) {
//     // Tests creating a column pre-populated with data.
//     std::vector<Types> data = {10, 20, 30};
//     Column col(data, Datatypes::INT);
//     ASSERT_EQ(col.size(), 3);
//     ASSERT_EQ(col[1], Types(20));
// }

// TEST(ColumnTest, Constructor_FromVectorWithConstraints) {
//     // Tests that constraints are checked upon creation.
//     // This should fail because the `Unique` constraint is violated.
//     std::vector<Types> data = {10, 20, 10}; // Duplicate value
//     ColumnConstraints constraints;
//     constraints.Unique = true;
//     ASSERT_THROW(Column(data, Datatypes::INT, constraints), std::runtime_error);
// }

// TEST(ColumnTest, Constructor_NotNullConstraint) {
//     // Tests that the NOT NULL constraint is checked upon creation.
//     std::vector<Types> data = {10, monostate{}, 30}; // Contains NULL
//     ColumnConstraints constraints;
//     constraints.TakesNulls = false;
//     ASSERT_THROW(Column(data, Datatypes::INT, constraints), std::runtime_error);
// }


// //=================================================================================================
// // 2. DATA MANIPULATION TESTS (push, update, pop, erase)
// //=================================================================================================

// TEST(ColumnTest, Push_WithValue) {
//     // Tests adding values to the column.
//     Column col(Datatypes::TEXT);
//     col.push("hello");
//     col.push("world");
//     ASSERT_EQ(col.size(), 2);
//     ASSERT_EQ(getString(col[0]), "hello");
// }

// TEST(ColumnTest, Push_DefaultValue) {
//     // Tests pushing a default value (NULL in this case) when no argument is provided.
//     Column col(Datatypes::INT);
//     col.push();
//     ASSERT_EQ(col.size(), 1);
//     ASSERT_TRUE(std::holds_alternative<monostate>(col[0]));
// }

// TEST(ColumnTest, Push_WithCustomDefaultValue) {
//     // Tests pushing a user-defined default value.
//     ColumnConstraints constraints;
//     constraints.DefaultValue = Types(0);
//     Column col(Datatypes::INT, constraints);
//     col.push();
//     ASSERT_EQ(col.size(), 1);
//     ASSERT_EQ(std::get<int>(col[0]), 0);
// }

// TEST(ColumnTest, Push_ViolatesUniqueness) {
//     // Tests that pushing a duplicate value into a unique column throws an error.
//     ColumnConstraints constraints;
//     constraints.Unique = true;
//     Column col(Datatypes::INT, constraints);
//     col.push(100);
//     ASSERT_THROW(col.push(100), std::runtime_error);
// }

// TEST(ColumnTest, Update_ValidIndex) {
//     // Tests updating an existing value.
//     std::vector<Types> data = {1, 2, 3};
//     Column col(data, Datatypes::INT);
//     col.update(1, 99);
//     ASSERT_EQ(std::get<int>(col[1]), 99);
// }

// TEST(ColumnTest, Update_ViolatesNotNull) {
//     // Tests that updating a value to NULL in a NOT NULL column fails.
//     std::vector<Types> data = {1, 2, 3};
//     ColumnConstraints constraints;
//     constraints.TakesNulls = false;
//     Column col(data, Datatypes::INT, constraints);
//     ASSERT_THROW(col.update(1, monostate{}), std::runtime_error);
// }

// TEST(ColumnTest, Pop_OnNonEmptyColumn) {
//     // Tests removing the last element.
//     std::vector<Types> data = {"a", "b"};
//     Column col(data, Datatypes::TEXT);
//     col.pop();
//     ASSERT_EQ(col.size(), 1);
//     ASSERT_EQ(getString(col[0]), "a");
// }

// TEST(ColumnTest, Pop_OnEmptyColumn) {
//     // Tests that calling pop on an empty column is safe (e.g., does nothing or throws).
//     // Assuming a safe no-op.
//     Column col(Datatypes::INT);
//     ASSERT_NO_THROW(col.pop());
//     ASSERT_EQ(col.size(), 0);
// }

// TEST(ColumnTest, BulkUpdate_And_BulkErase) {
//     // Tests modifying multiple records at once.
//     std::vector<Types> data = {10, 20, 30, 40, 50};
//     Column col(data, Datatypes::INT);
    
//     // Bulk Update
//     std::vector<int> update_indices = {0, 2, 4};
//     col.bulkUpdate(update_indices, 0);
//     ASSERT_EQ(std::get<int>(col[0]), 0);
//     ASSERT_EQ(std::get<int>(col[1]), 20);
//     ASSERT_EQ(std::get<int>(col[2]), 0);
//     ASSERT_EQ(std::get<int>(col[3]), 40);
//     ASSERT_EQ(std::get<int>(col[4]), 0);

//     // Bulk Erase
//     std::vector<int> erase_indices = {0, 3}; // Erase 0 and 40
//     col.bulkErase(erase_indices);
//     ASSERT_EQ(col.size(), 3);
//     ASSERT_EQ(std::get<int>(col[0]), 20);
//     ASSERT_EQ(std::get<int>(col[1]), 0);
//     ASSERT_EQ(std::get<int>(col[2]), 0);
// }

// //=================================================================================================
// // 3. TEMPORARY COLUMN CREATION (SCALAR FUNCTIONS)
// //=================================================================================================

// // --- Numeric Functions ---

// TEST(ColumnTest, Round_DecimalPlaces) {
//     // Tests rounding numbers to a specified number of decimal places.
//     std::vector<Types> data = {1.234f, 5.678f, 9.0f, monostate{}};
//     Column col(data, Datatypes::FLOAT);
//     Column rounded_col = col.round(all_indices(col), 2);
    
//     ASSERT_EQ(rounded_col.size(), 4);
//     ASSERT_FLOAT_EQ(std::get<float>(rounded_col[0]), 1.23f);
//     ASSERT_FLOAT_EQ(std::get<float>(rounded_col[1]), 5.68f);
//     ASSERT_FLOAT_EQ(std::get<float>(rounded_col[2]), 9.00f);
//     ASSERT_TRUE(std::holds_alternative<monostate>(rounded_col[3])); // NULLs should propagate
// }

// TEST(ColumnTest, FloorAndCeiling) {
//     // Tests floor and ceiling functions.
//     std::vector<Types> data = {1.1f, -2.8f, 5.0f};
//     Column col(data, Datatypes::FLOAT);
    
//     Column floor_col = col.floor(all_indices(col));
//     ASSERT_FLOAT_EQ(std::get<float>(floor_col[0]), 1.0f);
//     ASSERT_FLOAT_EQ(std::get<float>(floor_col[1]), -3.0f);
//     ASSERT_FLOAT_EQ(std::get<float>(floor_col[2]), 5.0f);

//     Column ceil_col = col.ceiling(all_indices(col));
//     ASSERT_FLOAT_EQ(std::get<float>(ceil_col[0]), 2.0f);
//     ASSERT_FLOAT_EQ(std::get<float>(ceil_col[1]), -2.0f);
//     ASSERT_FLOAT_EQ(std::get<float>(ceil_col[2]), 5.0f);
// }

// // --- String Functions ---

// TEST(ColumnTest, Concat_Basic) {
//     // Tests string concatenation.
//     std::vector<Types> data = {"apple", "orange", monostate{}, "plum"};
//     Column col(data, Datatypes::TEXT);
//     Column new_col = col.concat(all_indices(col), "-fruit");
    
//     ASSERT_EQ(getString(new_col[0]), "apple-fruit");
//     ASSERT_EQ(getString(new_col[1]), "orange-fruit");
//     ASSERT_TRUE(std::holds_alternative<monostate>(new_col[2]));
//     ASSERT_EQ(getString(new_col[3]), "plum-fruit");
// }

// TEST(ColumnTest, UpperAndLower) {
//     // Tests converting string case.
//     std::vector<Types> data = {"Case", "MIXED", "lower"};
//     Column col(data, Datatypes::TEXT);

//     Column upper_col = col.upper(all_indices(col));
//     ASSERT_EQ(getString(upper_col[0]), "CASE");
//     ASSERT_EQ(getString(upper_col[1]), "MIXED");
//     ASSERT_EQ(getString(upper_col[2]), "LOWER");

//     Column lower_col = col.lower(all_indices(col));
//     ASSERT_EQ(getString(lower_col[0]), "case");
//     ASSERT_EQ(getString(lower_col[1]), "mixed");
//     ASSERT_EQ(getString(lower_col[2]), "lower");
// }

// TEST(ColumnTest, Trim_BothModes) {
//     // Tests trimming whitespace from both sides.
//     std::vector<Types> data = {"  hello  ", "  world", "no-spaces"};
//     Column col(data, Datatypes::TEXT);
//     Column trimmed_col = col.trim(all_indices(col), TrimModes::BOTH);

//     ASSERT_EQ(getString(trimmed_col[0]), "hello");
//     ASSERT_EQ(getString(trimmed_col[1]), "world");
//     ASSERT_EQ(getString(trimmed_col[2]), "no-spaces");
// }

// TEST(ColumnTest, Trim_LeadingWithChar) {
//     // Tests trimming a specific character from the beginning of strings.
//     std::vector<Types> data = {"$$$100", "$200", "300"};
//     Column col(data, Datatypes::TEXT);
//     Column trimmed_col = col.trim(all_indices(col), TrimModes::LEADING, '$');
    
//     ASSERT_EQ(getString(trimmed_col[0]), "100");
//     ASSERT_EQ(getString(trimmed_col[1]), "200");
//     ASSERT_EQ(getString(trimmed_col[2]), "300");
// }

// TEST(ColumnTest, Substring_Valid) {
//     // Tests extracting a substring.
//     std::vector<Types> data = {"abcdef", "ghijkl"};
//     Column col(data, Datatypes::TEXT);
//     // SQL SUBSTRING is often 1-indexed, let's assume this implementation is 0-indexed.
//     Column sub_col = col.substring(all_indices(col), 1, 3); // Extracts "bcd" and "hij"
    
//     ASSERT_EQ(getString(sub_col[0]), "bcd");
//     ASSERT_EQ(getString(sub_col[1]), "hij");
// }

// TEST(ColumnTest, Substring_ExceedsLength) {
//     // Tests that asking for more characters than available works correctly.
//     std::vector<Types> data = {"short"};
//     Column col(data, Datatypes::TEXT);
//     Column sub_col = col.substring(all_indices(col), 2, 100); // from 'o' to the end
    
//     ASSERT_EQ(getString(sub_col[0]), "ort");
// }


// // --- Conditional Functions ---

// TEST(ColumnTest, NullIf_ConditionMet) {
//     // Tests NULLIF where the condition is met.
//     std::vector<Types> data = {10, 20, 30, 20};
//     Column col(data, Datatypes::INT);
//     Column nullif_col = col.nullIf(all_indices(col), 20);

//     ASSERT_EQ(std::get<int>(nullif_col[0]), 10);
//     ASSERT_TRUE(std::holds_alternative<monostate>(nullif_col[1]));
//     ASSERT_EQ(std::get<int>(nullif_col[2]), 30);
//     ASSERT_TRUE(std::holds_alternative<monostate>(nullif_col[3]));
// }

// TEST(ColumnTest, Coalesce_WithNulls) {
//     // Tests COALESCE, which should replace NULLs with a specified value.
//     std::vector<Types> data = {100, monostate{}, 300, monostate{}};
//     Column col(data, Datatypes::INT);
//     Column coalesced_col = col.coalesce(all_indices(col), 0);
    
//     ASSERT_EQ(std::get<int>(coalesced_col[0]), 100);
//     ASSERT_EQ(std::get<int>(coalesced_col[1]), 0);
//     ASSERT_EQ(std::get<int>(coalesced_col[2]), 300);
//     ASSERT_EQ(std::get<int>(coalesced_col[3]), 0);
// }


// //=================================================================================================
// // 4. AGGREGATE FUNCTION TESTS
// //=================================================================================================

// TEST(ColumnTest, Sum_And_SumDistinct) {
//     // Tests sum and distinct sum, ensuring NULLs are ignored.
//     std::vector<Types> data = {10, 20, monostate{}, 10, 30.5f};
//     Column col(data, Datatypes::FLOAT);
    
//     double total_sum = col.sum(all_indices(col));
//     ASSERT_DOUBLE_EQ(total_sum, 70.5);

//     double distinct_sum = col.sumDistinct(all_indices(col));
//     ASSERT_DOUBLE_EQ(distinct_sum, 60.5); // 10 + 20 + 30.5
// }

// TEST(ColumnTest, Count_And_CountDistinct) {
//     // Tests count and distinct count for all non-null values.
//     std::vector<Types> data = {"A", "B", monostate{}, "A", "C"};
//     Column col(data, Datatypes::TEXT);

//     int total_count = col.count(all_indices(col));
//     ASSERT_EQ(total_count, 4); // Counts non-null values

//     int distinct_count = col.countDistinct(all_indices(col));
//     ASSERT_EQ(distinct_count, 3); // A, B, C
// }

// TEST(ColumnTest, Avg_And_AvgDistinct) {
//     // Tests average and distinct average.
//     std::vector<Types> data = {10, 20, monostate{}, 10, 20}; // Non-nulls are {10, 20, 10, 20}
//     Column col(data, Datatypes::INT);
    
//     // Avg = (10+20+10+20) / 4 = 15
//     double total_avg = col.avg(all_indices(col));
//     ASSERT_DOUBLE_EQ(total_avg, 15.0);
    
//     // Avg Distinct = (10+20) / 2 = 15
//     double distinct_avg = col.avgDistinct(all_indices(col));
//     ASSERT_DOUBLE_EQ(distinct_avg, 15.0);
// }

// TEST(ColumnTest, Avg_AllNulls) {
//     // The average of an empty set (e.g., all NULLs) is NULL in SQL.
//     // The function returns double, so we expect a specific behavior, like returning 0 or NaN, or throwing.
//     // Let's assume throwing is the desired behavior to signal an undefined result.
//     std::vector<Types> data = {monostate{}, monostate{}};
//     Column col(data, Datatypes::INT);
//     ASSERT_THROW(col.avg(all_indices(col)), std::runtime_error);
// }

// TEST(ColumnTest, Max_And_Min_Numeric) {
//     // Tests max and min on a numeric column.
//     std::vector<Types> data = {10, -5, monostate{}, 100, 25};
//     Column col(data, Datatypes::INT);

//     ASSERT_DOUBLE_EQ(col.max(all_indices(col)), 100);
//     ASSERT_DOUBLE_EQ(col.min(all_indices(col)), -5);
// }

// TEST(ColumnTest, Max_And_Min_String) {
//     // Tests max and min on a string column (lexicographical comparison).
//     std::vector<Types> data = {"apple", "banana", monostate{}, "zebra", "cherry"};
//     Column col(data, Datatypes::TEXT);
    
//     // Assuming max/min on strings are not directly supported by the double-returning function.
//     // This is a design question. If they should be, the return type should be `Types`.
//     // Let's assume these functions throw for non-numeric types.
//     ASSERT_THROW(col.max(all_indices(col)), std::runtime_error);
//     ASSERT_THROW(col.min(all_indices(col)), std::runtime_error);
// }

// TEST(ColumnTest, StringAggregate) {
//     // Tests aggregating strings with a separator.
//     std::vector<Types> data = {"first", "second", monostate{}, "third"};
//     Column col(data, Datatypes::TEXT);
    
//     string result = col.stringAggregate(all_indices(col), ", ");
//     ASSERT_EQ(result, "first, second, third");
// }

// TEST(ColumnTest, StringAggregate_EmptySeparator) {
//     // Tests aggregating strings with an empty separator.
//     std::vector<Types> data = {"a", "b", "c"};
//     Column col(data, Datatypes::TEXT);
    
//     string result = col.stringAggregate(all_indices(col), "");
//     ASSERT_EQ(result, "abc");
// }

// Helper to create a vector of indices from 0 to n-1
std::vector<int> create_indices(int n) {
    std::vector<int> indices(n);
    std::iota(indices.begin(), indices.end(), 0);
    return indices;
}

// Test Fixture for the Column class
class ColumnTest : public ::testing::Test {
protected:
    // You can add setup and teardown logic here if needed
    // For example, creating common Column objects for tests
    Column int_col{Datatypes::INT};
    Column string_col{Datatypes::TEXT};
    Column float_col{Datatypes::FLOAT};
    Column date_col{Datatypes::DATETIME};
};

//##############################################################################
// CONSTRUCTOR AND BASIC OPERATIONS TESTS
//##############################################################################

TEST_F(ColumnTest, ConstructorAndSize) {
    EXPECT_EQ(int_col.size(), 0);
    EXPECT_EQ(int_col.type, Datatypes::INT);

    std::vector<Types> initial_data = {1, 2, 3, 4, 5};
    Column populated_col(initial_data, Datatypes::INT);
    EXPECT_EQ(populated_col.size(), 5);
    EXPECT_EQ(populated_col[2], Types(3));
}

TEST_F(ColumnTest, PushAndPop) {
    int_col.push(10);
    int_col.push(20);
    EXPECT_EQ(int_col.size(), 2);
    EXPECT_EQ(int_col[1], Types(20));
}

TEST_F(ColumnTest, UpdateAndErase) {
    std::vector<Types> initial_data = {1, 2, 3, 4, 5};
    Column col(initial_data, Datatypes::INT);

    col.update(2, 99);
    EXPECT_EQ(col[2], Types(99));

    col.erase(1); // Erase '2'
    EXPECT_EQ(col.size(), 4);
    EXPECT_EQ(col[0], Types(1));
    EXPECT_EQ(col[1], Types(99)); // Formerly index 2
    EXPECT_EQ(col[2], Types(4));
    EXPECT_EQ(col[3], Types(5));
}

TEST_F(ColumnTest, BulkOperations) {
    std::vector<Types> initial_data = {10, 20, 30, 40, 50, 60};
    Column col(initial_data, Datatypes::INT);

    std::vector<int> update_indices = {1, 3, 5};
    col.bulkUpdate(update_indices, 0);
    EXPECT_EQ(col[0], Types(10));
    EXPECT_EQ(col[1], Types(0));
    EXPECT_EQ(col[2], Types(30));
    EXPECT_EQ(col[3], Types(0));
    EXPECT_EQ(col[4], Types(50));
    EXPECT_EQ(col[5], Types(0));

    std::vector<int> erase_indices = {0, 2, 4}; // Erase 10, 30, 50
    col.bulkErase(erase_indices);
    EXPECT_EQ(col.size(), 3);
    EXPECT_EQ(col[0], Types(0));
    EXPECT_EQ(col[1], Types(0));
    EXPECT_EQ(col[2], Types(0));
}

//##############################################################################
// CONDITIONAL LOGIC TESTS
//##############################################################################

TEST_F(ColumnTest, IndicesMeetingCondition) {
    std::vector<Types> data = {5, 10, 15, 10, 20, 5};
    Column col(data, Datatypes::INT);

    auto greater_than_10 = col.indicesMeetingCondition(Types(10), std::greater<Types>());
    std::vector<int> expected_gt = {2, 4};
    EXPECT_EQ(greater_than_10, expected_gt);

    auto equal_to_10 = col.indicesMeetingCondition(Types(10), std::equal_to<Types>());
    std::vector<int> expected_eq = {1, 3};
    EXPECT_EQ(equal_to_10, expected_eq);

    auto not_equal_to_5 = col.indicesMeetingCondition(Types(5), std::not_equal_to<Types>());
    std::vector<int> expected_neq = {1, 2, 3, 4};
    EXPECT_EQ(not_equal_to_5, expected_neq);
}


//##############################################################################
// NUMERIC FUNCTION TESTS
//##############################################################################

TEST_F(ColumnTest, Round) {
    std::vector<Types> data = {1.234f, 5.678f, 9.0f, -3.14159f};
    Column col(data, Datatypes::FLOAT);
    auto indices = create_indices(col.size());

    Column rounded_2 = col.round(indices, 2);
    EXPECT_FLOAT_EQ(std::get<float>(rounded_2[0]), 1.23f);
    EXPECT_FLOAT_EQ(std::get<float>(rounded_2[1]), 5.68f);
    EXPECT_FLOAT_EQ(std::get<float>(rounded_2[2]), 9.00f);
    EXPECT_FLOAT_EQ(std::get<float>(rounded_2[3]), -3.14f);

    Column rounded_0 = col.round(indices, 0);
    EXPECT_FLOAT_EQ(std::get<float>(rounded_0[0]), 1.0f);
    EXPECT_FLOAT_EQ(std::get<float>(rounded_0[1]), 6.0f);
}

TEST_F(ColumnTest, FloorAndCeiling) {
    std::vector<Types> data = {1.1f, 1.9f, -1.1f, -1.9f, 5.0f};
    Column col(data, Datatypes::FLOAT);
    auto indices = create_indices(col.size());

    Column floored = col.floor(indices);
    EXPECT_EQ(getNumeric<int>(floored[0]), 1);
    EXPECT_EQ(getNumeric<int>(floored[1]), 1);
    EXPECT_EQ(getNumeric<int>(floored[2]), -2);
    EXPECT_EQ(getNumeric<int>(floored[3]), -2);
    EXPECT_EQ(getNumeric<int>(floored[4]), 5);

    Column ceiled = col.ceiling(indices);
    EXPECT_EQ(getNumeric<int>(ceiled[0]), 2);
    EXPECT_EQ(getNumeric<int>(ceiled[1]), 2);
    EXPECT_EQ(getNumeric<int>(ceiled[2]), -1);
    EXPECT_EQ(getNumeric<int>(ceiled[3]), -1);
    EXPECT_EQ(getNumeric<int>(ceiled[4]), 5);
}

TEST_F(ColumnTest, Absolute) {
    std::vector<Types> data = {(float)10, (float)-20, (float)0, 3.14f, -9.9f};
    Column col(data, Datatypes::FLOAT);
    auto indices = create_indices(col.size());

    Column abs_col = col.absolute(indices);
    EXPECT_EQ(abs_col[0], Types(10));
    EXPECT_EQ(abs_col[1], Types(20));
    EXPECT_EQ(abs_col[2], Types(0));
    EXPECT_EQ(abs_col[3], Types(3.14f));
    EXPECT_EQ(abs_col[4], Types(9.9f));
}

//##############################################################################
// STRING FUNCTION TESTS
//##############################################################################

TEST_F(ColumnTest, Length) {
    std::vector<Types> data = {std::string("hello"), std::string(""), std::string("world!")};
    Column col(data, Datatypes::TEXT);
    auto indices = create_indices(col.size());

    Column len_col = col.length(indices);
    EXPECT_EQ(std::get<int>(len_col[0]), 5);
    EXPECT_EQ(std::get<int>(len_col[1]), 0);
    EXPECT_EQ(std::get<int>(len_col[2]), 6);
}

TEST_F(ColumnTest, Concat) {
    std::vector<Types> data = {std::string("first"), std::string("second")};
    Column col(data, Datatypes::TEXT);
    auto indices = create_indices(col.size());

    Column concat_col = col.concat(indices, "-place");
    EXPECT_EQ(std::get<std::string>(concat_col[0]), "first-place");
    EXPECT_EQ(std::get<std::string>(concat_col[1]), "second-place");
}

TEST_F(ColumnTest, UpperLowerInitCap) {
    std::vector<Types> data = {std::string("HelLo WorLd"), std::string("all lower"), std::string("ALL UPPER")};
    Column col(data, Datatypes::TEXT);
    auto indices = create_indices(col.size());

    Column upper_col = col.upper(indices);
    EXPECT_EQ(std::get<std::string>(upper_col[0]), "HELLO WORLD");
    EXPECT_EQ(std::get<std::string>(upper_col[1]), "ALL LOWER");
    EXPECT_EQ(std::get<std::string>(upper_col[2]), "ALL UPPER");

    Column lower_col = col.lower(indices);
    EXPECT_EQ(std::get<std::string>(lower_col[0]), "hello world");
    EXPECT_EQ(std::get<std::string>(lower_col[1]), "all lower");
    EXPECT_EQ(std::get<std::string>(lower_col[2]), "all upper");

    // Assuming InitCap capitalizes the first letter of each word
    Column initcap_col = col.initCap(indices);
    EXPECT_EQ(std::get<std::string>(initcap_col[0]), "Hello World");
    EXPECT_EQ(std::get<std::string>(initcap_col[1]), "All Lower");
    EXPECT_EQ(std::get<std::string>(initcap_col[2]), "All Upper");
}

TEST_F(ColumnTest, Trim) {
    std::vector<Types> data = {std::string("  hello  "), std::string("xxxyyyxxx"), std::string("  both sides  ")};
    Column col(data, Datatypes::TEXT);
    auto indices = create_indices(col.size());

    Column trim_leading = col.trim(indices, TrimModes::LEADING);
    EXPECT_EQ(std::get<std::string>(trim_leading[0]), "hello  ");
    Column trim_trailing = col.trim(indices, TrimModes::TRAILING);
    EXPECT_EQ(std::get<std::string>(trim_trailing[0]), "  hello");
    Column trim_both = col.trim(indices, TrimModes::BOTH);
    EXPECT_EQ(std::get<std::string>(trim_both[0]), "hello");

    Column trim_custom = col.trim({1}, TrimModes::BOTH, 'x');
    EXPECT_EQ(std::get<std::string>(trim_custom[0]), "yyy");
}

TEST_F(ColumnTest, Substring) {
    std::vector<Types> data = {std::string("abcdefg")};
    Column col(data, Datatypes::TEXT);
    auto indices = create_indices(col.size());

    // SQL substring is often 1-indexed
    Column sub_col = col.substring(indices, 2, 3);
    EXPECT_EQ(std::get<std::string>(sub_col[0]), "bcd");

    Column sub_to_end = col.substring(indices, 4, 99); // Length past end
    EXPECT_EQ(std::get<std::string>(sub_to_end[0]), "defg");
    
    Column sub_invalid_start = col.substring(indices, 0, 3); // Invalid start
    EXPECT_EQ(std::get<std::string>(sub_invalid_start[0]), "abc"); // Common SQL behavior
}

//##############################################################################
// AGGREGATE FUNCTION TESTS
//##############################################################################

TEST_F(ColumnTest, Sum) {
    std::vector<Types> data = {10, 20, std::monostate{}, 30, -5};
    Column col(data, Datatypes::INT);
    auto indices = create_indices(col.size());

    EXPECT_DOUBLE_EQ(col.sum(indices), 55.0);
}

TEST_F(ColumnTest, SumDistinct) {
    std::vector<Types> data = {10, 20, 10, std::monostate{}, 30, 20};
    Column col(data, Datatypes::INT);
    auto indices = create_indices(col.size());

    EXPECT_DOUBLE_EQ(col.sumDistinct(indices), 60.0); // 10 + 20 + 30
}

TEST_F(ColumnTest, Count) {
    std::vector<Types> data = {1, std::monostate{}, 2, 4, std::monostate{}};
    Column col(data, Datatypes::INT); // Type doesn't matter for count
    auto indices = create_indices(col.size());

    EXPECT_EQ(col.count(indices), 3); // Should ignore NULLs
}

TEST_F(ColumnTest, CountDistinct) {
    std::vector<Types> data = {"A", "B", "A", std::monostate{}, "C", "B", "D"};
    Column col(data, Datatypes::TEXT);
    auto indices = create_indices(col.size());

    EXPECT_EQ(col.countDistinct(indices), 4); // A, B, C, D
}

TEST_F(ColumnTest, Avg) {
    std::vector<Types> data = {10, 15, 20, std::monostate{}, 25};
    Column col(data, Datatypes::INT);
    auto indices = create_indices(col.size());

    EXPECT_DOUBLE_EQ(col.avg(indices), 17.5); // (10+15+20+25) / 4
}

TEST_F(ColumnTest, AvgDistinct) {
    std::vector<Types> data = {10, 20, 10, std::monostate{}, 30, 20};
    Column col(data, Datatypes::INT);
    auto indices = create_indices(col.size());

    EXPECT_DOUBLE_EQ(col.avgDistinct(indices), 20.0); // (10 + 20 + 30) / 3
}

TEST_F(ColumnTest, MinMax) {
    std::vector<Types> data_num = {100, -50, 200, std::monostate{}, 0, 99};
    Column col_num(data_num, Datatypes::INT);
    auto indices_num = create_indices(col_num.size());

    EXPECT_DOUBLE_EQ(col_num.min(indices_num), -50.0);
    EXPECT_DOUBLE_EQ(col_num.max(indices_num), 200.0);

    // std::vector<Types> data_str = {"apple", "zebra", "banana", std::monostate{}, "grape"};
    // Column col_str(data_str, Datatypes::TEXT);
    // auto indices_str = create_indices(col_str.size());

    // // Assuming min/max on strings is lexicographical
    // EXPECT_EQ(std::get<std::string>(col_str.min(indices_str)), "apple");
    // EXPECT_EQ(std::get<std::string>(col_str.max(indices_str)), "zebra");
}

TEST_F(ColumnTest, StringAggregate) {
    std::vector<Types> data = {"A", "B", std::monostate{}, "C", "D"};
    Column col(data, Datatypes::TEXT);
    auto indices = create_indices(col.size());

    EXPECT_EQ(col.stringAggregate(indices, ","), "A,B,C,D");
    EXPECT_EQ(col.stringAggregate(indices, "->"), "A->B->C->D");
}

//##############################################################################
// NULL-RELATED FUNCTION TESTS
//##############################################################################

TEST_F(ColumnTest, NullIf) {
    std::vector<Types> data = {10, 20, 30, 40};
    Column col(data, Datatypes::INT);
    auto indices = create_indices(col.size());

    Column nullif_col = col.nullIf(indices, 20);
    EXPECT_EQ(nullif_col[0], Types(10));
    EXPECT_TRUE(std::holds_alternative<std::monostate>(nullif_col[1]));
    EXPECT_EQ(nullif_col[2], Types(30));
    EXPECT_EQ(nullif_col[3], Types(40));
}

TEST_F(ColumnTest, Coalesce) {
    std::vector<Types> data = {10, std::monostate{}, 30, std::monostate{}};
    Column col(data, Datatypes::INT);
    auto indices = create_indices(col.size());

    Column coalesce_col = col.coalesce(indices, 99);
    EXPECT_EQ(coalesce_col[0], Types(10));
    EXPECT_EQ(coalesce_col[1], Types(99));
    EXPECT_EQ(coalesce_col[2], Types(30));
    EXPECT_EQ(coalesce_col[3], Types(99));
}

//##############################################################################
// CONSTRAINT ENFORCEMENT TESTS
//##############################################################################

// TEST_F(ColumnTest, NotNullConstraint) {
//     ColumnConstraints constraints;
//     constraints.TakesNulls = false;
//     Column col(Datatypes::INT, constraints);

//     col.push(10); // OK
//     EXPECT_THROW(col.push(std::monostate{}), std::runtime_error);

//     std::vector<Types> bad_data = {1, 2, std::monostate{}};
//     EXPECT_THROW(Column(bad_data, Datatypes::INT, constraints), std::runtime_error);
// }

// TEST_F(ColumnTest, UniqueConstraint) {
//     ColumnConstraints constraints;
//     constraints.Unique = true;
//     Column col(Datatypes::INT, constraints);

//     col.push(10);
//     col.push(20); // OK
//     EXPECT_THROW(col.push(10), std::runtime_error);

//     col.update(0, 30); // OK
//     EXPECT_THROW(col.update(0, 20), std::runtime_error);

//     std::vector<Types> bad_data = {1, 2, 3, 2, 4};
//     EXPECT_THROW(Column(bad_data, Datatypes::INT, constraints), std::runtime_error);
// }

// TEST_F(ColumnTest, CharLengthConstraint) {
//     ColumnConstraints constraints;
//     constraints.CharLength = 5;
//     Column col(Datatypes::VARCHAR, constraints);

//     col.push(Varchar("abcde")); // OK
//     EXPECT_THROW(col.push(Varchar("abcdef")), std::runtime_error);

//     col.update(0, std::string("12345")); // OK
//     EXPECT_THROW(col.update(0, std::string("123456")), std::runtime_error);
// }
