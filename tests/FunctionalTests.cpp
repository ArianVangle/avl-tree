#include <string>

#include "BinaryTree.hpp"
#include "Tests.hpp"

namespace {

void TestMap() {
    IntTree tree = MakeTree({1, 2, 3});
    IntTree doubled = tree.Map([](int x) { return x * 2; });
    assert(doubled == MakeTree({2, 4, 6}));
    assert(tree == MakeTree({1, 2, 3}));
    assert(doubled.CheckInvariants());
}

void TestMapMergesEqualResults() {
    IntTree tree = MakeTree({-2, -1, 1, 2});
    IntTree squared = tree.Map([](int x) { return x * x; });
    assert(squared.GetCount() == 2);
    assert(squared == MakeTree({1, 4}));
}

void TestMapReordersTree() {
    IntTree tree = MakeTree({1, 2, 3, 4, 5});
    IntTree negated = tree.Map([](int x) { return -x; });
    assert(negated == MakeTree({-5, -4, -3, -2, -1}));
    assert(negated.CheckInvariants());
}

void TestWhere() {
    IntTree tree = MakeTree({1, 2, 3, 4, 5, 6});
    IntTree even = tree.Where([](int x) { return x % 2 == 0; });
    assert(even == MakeTree({2, 4, 6}));
    IntTree none = tree.Where([](int x) { return x > 100; });
    assert(none.IsEmpty());
}

void TestReduceSum() {
    IntTree tree = MakeTree({4, 1, 3, 2});
    assert(tree.Reduce([](int x, int acc) { return x + acc; }, 0) == 10);
    assert(IntTree().Reduce([](int x, int acc) { return x + acc; }, 7) == 7);
}

void TestReduceFormulaFromTask() {
    IntTree tree = MakeTree({1, 2, 3});
    int result = tree.Reduce([](int x1, int x2) { return 2 * x1 + 3 * x2; }, 4);
    assert(result == 144);
}

void TestReduceOtherType() {
    IntTree tree = MakeTree({3, 1, 2});
    std::string digits = tree.Reduce(
        [](int x, std::string acc) { return acc + std::to_string(x); }, std::string());
    assert(digits == "123");
}

void TestOnEmptyTree() {
    IntTree empty;
    assert(empty.Map([](int x) { return x + 1; }).IsEmpty());
    assert(empty.Where([](int) { return true; }).IsEmpty());
}

}

void RunFunctionalTests() {
    std::cout << "map, where, reduce\n";
    RunTest("map", TestMap);
    RunTest("map склеивает одинаковые результаты", TestMapMergesEqualResults);
    RunTest("map меняет порядок, дерево остаётся правильным", TestMapReordersTree);
    RunTest("where", TestWhere);
    RunTest("reduce: сумма", TestReduceSum);
    RunTest("reduce: пример из задания (верный ответ 144)", TestReduceFormulaFromTask);
    RunTest("reduce с аккумулятором другого типа", TestReduceOtherType);
    RunTest("map и where на пустом дереве", TestOnEmptyTree);
}
