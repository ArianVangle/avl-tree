#pragma once

#include <cassert>
#include <exception>
#include <initializer_list>
#include <iostream>
#include <istream>
#include <ostream>

#include "BinaryTree.hpp"

struct Tracked {
    inline static int alive = 0;
    int value;
    explicit Tracked(int v = 0) : value(v) { ++alive; }
    Tracked(const Tracked& other) : value(other.value) { ++alive; }
    Tracked(Tracked&& other) noexcept : value(other.value) { ++alive; }
    Tracked& operator=(const Tracked&) = default;
    Tracked& operator=(Tracked&&) noexcept = default;
    ~Tracked() { --alive; }
};

inline bool operator<(const Tracked& a, const Tracked& b) { return a.value < b.value; }
inline std::ostream& operator<<(std::ostream& out, const Tracked& t) { return out << t.value; }
inline std::istream& operator>>(std::istream& in, Tracked& t) { return in >> t.value; }

using IntTree = BinaryTree<int>;

inline IntTree MakeTree(std::initializer_list<int> values) {
    IntTree tree;
    for (int value : values) {
        tree.Insert(value);
    }
    return tree;
}

void RunTest(const char* name, void (*test)());

template <typename Func>
bool Throws(Func func) {
    try {
        func();
    } catch (const std::exception&) {
        return true;
    }
    return false;
}

void RunBasicTests();
void RunFunctionalTests();
void RunStringTests();
void RunSubtreeTests();
void RunInterfaceTests();
void RunTypeTests();

void RunAllTests();
