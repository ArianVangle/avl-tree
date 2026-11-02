#pragma once

#include <istream>
#include <ostream>
#include <string>

struct Person {
    int id = 0;
    std::string name;
};

struct Student : Person {};

struct Teacher : Person {};

bool operator<(const Person& a, const Person& b);
bool operator==(const Person& a, const Person& b);
std::ostream& operator<<(std::ostream& out, const Person& person);
std::istream& operator>>(std::istream& in, Person& person);
