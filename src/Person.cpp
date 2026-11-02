#include "Person.hpp"

bool operator<(const Person& a, const Person& b) { return a.id < b.id; }

bool operator==(const Person& a, const Person& b) { return a.id == b.id && a.name == b.name; }

std::ostream& operator<<(std::ostream& out, const Person& person) {
    return out << person.id << ':' << person.name;
}

std::istream& operator>>(std::istream& in, Person& person) {
    char colon = '\0';
    if (in >> person.id >> colon && colon == ':' && std::getline(in, person.name) &&
        !person.name.empty()) {
        return in;
    }
    in.setstate(std::ios::failbit);
    return in;
}
