#include "Person.hpp"

bool operator==(const Person& a, const Person& b) { return a.id == b.id && a.name == b.name; }

std::ostream& operator<<(std::ostream& out, const Person& person) {
    return out << person.id << ':' << person.name;
}
