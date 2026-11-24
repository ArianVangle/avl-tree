#include "BinaryTree.hpp"
#include "Interfaces.hpp"
#include "Tests.hpp"

namespace {

void TestCollectionInterface() {
    IntTree tree = MakeTree({30, 10, 20});
    const ICollection<int>& collection = tree;
    assert(collection.GetCount() == 3);
    assert(collection.Contains(20));
    assert(!collection.Contains(40));
    assert(collection.Get(0) == 10);
    assert(collection.Get(1) == 20);
    assert(collection.Get(2) == 30);
    assert(Throws([&] { collection.Get(3); }));
    assert(Throws([] { IntTree().Get(0); }));
}

void TestGetByIndexLarge() {
    IntTree tree;
    for (int i = 999; i >= 0; --i) {
        tree.Insert(i * 2);
    }
    for (size_t i = 0; i < 1000; ++i) {
        assert(tree.Get(i) == static_cast<int>(i) * 2);
    }
}

void TestEnumerator() {
    IntTree tree = MakeTree({3, 1, 2});
    const IEnumerable<int>& enumerable = tree;
    auto enumerator = enumerable.GetEnumerator();
    assert(Throws([&] { enumerator->GetCurrent(); }));
    int expected = 1;
    while (enumerator->MoveNext()) {
        assert(enumerator->GetCurrent() == expected);
        ++expected;
    }
    assert(expected == 4);
    assert(!enumerator->MoveNext());
    assert(Throws([&] { enumerator->GetCurrent(); }));
    enumerator->Reset();
    assert(enumerator->MoveNext());
    assert(enumerator->GetCurrent() == 1);
}

void TestEnumeratorOnEmpty() {
    IntTree tree;
    auto enumerator = tree.GetEnumerator();
    assert(!enumerator->MoveNext());
    assert(Throws([&] { enumerator->GetCurrent(); }));
}

void TestEnumeratorAllOrders() {
    IntTree tree;
    for (int i = 0; i < 40; ++i) {
        tree.Insert((i * 17) % 40);
    }
    for (const char* format : kAllFormats) {
        DynamicArray<int> expected = tree.Traverse(format);
        auto enumerator = tree.GetEnumerator(format);
        for (int round = 0; round < 2; ++round) {
            size_t i = 0;
            while (enumerator->MoveNext()) {
                assert(enumerator->GetCurrent() == expected[i]);
                ++i;
            }
            assert(i == expected.GetLength());
            enumerator->Reset();
        }
    }
}

void TestEnumeratorPreorder() {
    IntTree tree = MakeTree({2, 1, 3});
    auto enumerator = tree.GetEnumerator("{К}(Л)[П]");
    int expected[] = {2, 1, 3};
    for (int value : expected) {
        assert(enumerator->MoveNext() && enumerator->GetCurrent() == value);
    }
    assert(!enumerator->MoveNext());
    assert(Throws([&] { enumerator->GetCurrent(); }));
    assert(!IntTree().GetEnumerator("[П](Л){К}")->MoveNext());
    assert(Throws([&] { tree.GetEnumerator("{К}(К)[П]"); }));
}

void TestEquality() {
    IntTree a = MakeTree({1, 2, 3});
    IntTree b = MakeTree({3, 2, 1});
    assert(a == b);
    assert(!(a != b));
    assert(a != MakeTree({1, 2}));
    assert(a != MakeTree({1, 2, 4}));
    assert(IntTree() == IntTree());
}

void TestMergeOperator() {
    IntTree merged = MakeTree({1, 3, 5}) + MakeTree({2, 3, 4});
    assert(merged == MakeTree({1, 2, 3, 4, 5}));
    assert(merged.CheckInvariants());
    assert((IntTree() + IntTree()).IsEmpty());
}

void TestTraversals() {
    IntTree tree = MakeTree({2, 1, 3});
    auto check = [&](const char* format, int a, int b, int c) {
        DynamicArray<int> values = tree.Traverse(format);
        return values.GetLength() == 3 && values[0] == a && values[1] == b && values[2] == c;
    };
    assert(check("{К}(Л)[П]", 2, 1, 3));
    assert(check("{К}[П](Л)", 2, 3, 1));
    assert(check("(Л){К}[П]", 1, 2, 3));
    assert(check("(Л)[П]{К}", 1, 3, 2));
    assert(check("[П]{К}(Л)", 3, 2, 1));
    assert(check("[П](Л){К}", 3, 1, 2));
    assert(TraversalFormat("(Л) {К} [П]").Order() == "ЛКП");
    assert(Throws([&] { tree.Traverse("{К}(К)[П]"); }));
    assert(Throws([&] { tree.Traverse("{К}(Л)"); }));
}

}

void RunInterfaceTests() {
    std::cout << "Интерфейсы, перечислитель и операторы\n";
    RunTest("ICollection через ссылку на интерфейс", TestCollectionInterface);
    RunTest("Get(k) на 1000 элементах", TestGetByIndexLarge);
    RunTest("IEnumerator: обход, конец, Reset", TestEnumerator);
    RunTest("IEnumerator на пустом дереве", TestEnumeratorOnEmpty);
    RunTest("перечислитель во всех шести порядках", TestEnumeratorAllOrders);
    RunTest("перечислитель: прямой обход, неверный формат", TestEnumeratorPreorder);
    RunTest("операторы == и !=", TestEquality);
    RunTest("оператор + (слияние)", TestMergeOperator);
    RunTest("шесть обходов", TestTraversals);
}
