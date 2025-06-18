#pragma once
#include "datatypes.h"
#include "table.h"

using namespace std;

void test_equality() {
    Varchar a(10, "apple");
    Varchar b(10, "banana");
    Varchar c(10, "apple");
    Varchar d(10, Null);
    Varchar e(10, monostate{});
    Varchar empty;

    // == operator
    assert((a == c) == true);
    assert((a == b) == false);
    assert((a == empty) == false);
    assert((empty == a) == false);
    assert((empty == empty) == false);

    // != operator
    assert((a != b) == true);
    assert((a != c) == false);
    assert((a != empty) == false);
    assert((empty != a) == false);
    assert((empty != empty) == false);
}

void test_less_than() {
    Varchar a(10, "apple");
    Varchar b(10, "banana");
    Varchar empty;

    assert((a < b) == true);
    assert((b < a) == false);
    assert((a < a) == false);
    assert((a < empty) == false);
    assert((empty < a) == false);
    assert((empty < empty) == false);
}

void test_less_than_equal() {
    Varchar a(10, "apple");
    Varchar b(10, "banana");
    Varchar c(10, "apple");
    Varchar empty;

    assert((a <= b) == true);
    assert((a <= c) == true);
    assert((b <= a) == false);
    assert((a <= empty) == false);
    assert((empty <= a) == false);
    assert((empty <= empty) == false);
}

void test_greater_than() {
    Varchar a(10, "apple");
    Varchar b(10, "banana");
    Varchar empty;

    assert((b > a) == true);
    assert((a > b) == false);
    assert((a > a) == false);
    assert((a > empty) == false);
    assert((empty > a) == false);
    assert((empty > empty) == false);
}

void test_greater_than_equal() {
    Varchar a(10, "apple");
    Varchar b(10, "banana");
    Varchar c(10, "apple");
    Varchar empty;

    assert((b >= a) == true);
    assert((a >= b) == false);
    assert((a >= c) == true);
    assert((a >= empty) == false);
    assert((empty >= a) == false);
    assert((empty >= empty) == false);
}