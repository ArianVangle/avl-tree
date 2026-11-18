#include "Tests.hpp"

namespace {

int testsPassed = 0;

}

void RunTest(const char* name, void (*test)()) {
    Tracked::alive = 0;
    test();
    assert(Tracked::alive == 0);
    ++testsPassed;
    std::cout << "  ok  " << name << "\n";
}

void RunAllTests() {
    testsPassed = 0;
    RunBasicTests();
    RunFunctionalTests();
    RunStringTests();
    RunSubtreeTests();
    RunInterfaceTests();
    RunTypeTests();
    RunDictionaryTests();
    std::cout << "\nВсе тесты пройдены: " << testsPassed << std::endl;
}
