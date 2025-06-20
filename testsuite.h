#pragma once
#include "datatypes.h"
#include "table.h"

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

#define C_GREEN   "\x1B[32m"
#define C_RED     "\x1B[31m"
#define C_YELLOW  "\x1B[33m"
#define C_BLUE    "\x1B[34m"
#define C_RESET   "\x1B[0m"

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