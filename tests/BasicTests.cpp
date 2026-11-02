#include <random>
#include <type_traits>
#include <utility>

#include "BinaryTree.hpp"
#include "Tests.hpp"

static_assert(std::is_copy_constructible_v<BinaryTree<int>>);
static_assert(std::is_nothrow_move_constructible_v<BinaryTree<int>>);

namespace {

void TestEmpty() {
    IntTree tree;
    assert(tree.IsEmpty());
    assert(tree.GetCount() == 0);
    assert(tree.GetHeight() == 0);
    assert(!tree.Contains(1));
    assert(!tree.Remove(1));
    assert(Throws([&] { tree.Get(0); }));
    assert(tree.CheckInvariants());
}

void TestInsertAndContains() {
    IntTree tree = MakeTree({5, 3, 8, 1, 4});
    assert(tree.GetCount() == 5);
    for (int value : {1, 3, 4, 5, 8}) {
        assert(tree.Contains(value));
    }
    assert(!tree.Contains(2));
    assert(!tree.Contains(100));
    assert(tree.Get(0) == 1);
    assert(tree.Get(4) == 8);
    assert(tree.CheckInvariants());
}

void TestDuplicates() {
    IntTree tree;
    assert(tree.Insert(7));
    assert(!tree.Insert(7));
    assert(tree.GetCount() == 1);
}

void TestRemoveLeafAndOneChild() {
    IntTree tree = MakeTree({5, 3, 8, 1, 9});
    assert(tree.Remove(1));
    assert(!tree.Contains(1));
    assert(tree.Remove(8));
    assert(!tree.Contains(8));
    assert(tree.Contains(9));
    assert(tree.GetCount() == 3);
    assert(tree.CheckInvariants());
}

void TestRemoveTwoChildrenAndRoot() {
    IntTree tree = MakeTree({5, 3, 8, 1, 4, 7, 9});
    assert(tree.Remove(3));
    assert(tree.Remove(5));
    assert(!tree.Contains(3));
    assert(!tree.Contains(5));
    assert(tree.GetCount() == 5);
    assert(tree.CheckInvariants());
    assert(!tree.Remove(5));
}

void TestRemoveAll() {
    IntTree tree = MakeTree({4, 2, 6, 1, 3, 5, 7});
    for (int value = 1; value <= 7; ++value) {
        assert(tree.Remove(value));
        assert(tree.CheckInvariants());
    }
    assert(tree.IsEmpty());
}

void TestBalanceOnSortedInput() {
    IntTree tree;
    const int count = 10000;
    for (int i = 0; i < count; ++i) {
        tree.Insert(i);
    }
    assert(tree.CheckInvariants());
    assert(tree.GetHeight() <= static_cast<int>(1.45 * std::log2(count + 2)));
    for (int i = count - 1; i >= 0; i -= 2) {
        tree.Remove(i);
    }
    assert(tree.CheckInvariants());
    assert(tree.GetCount() == count / 2);
}

void TestRandomAgainstReference() {
    const int range = 500;
    bool present[range] = {};
    size_t count = 0;
    IntTree tree;
    std::mt19937_64 random(42);
    std::uniform_int_distribution<int> pick(0, range - 1);
    for (int step = 0; step < 20000; ++step) {
        int value = pick(random);
        if (random() % 3 == 0) {
            assert(tree.Remove(value) == present[value]);
            count -= present[value] ? 1 : 0;
            present[value] = false;
        } else {
            assert(tree.Insert(value) == !present[value]);
            count += present[value] ? 0 : 1;
            present[value] = true;
        }
        assert(tree.GetCount() == count);
    }
    for (int value = 0; value < range; ++value) {
        assert(tree.Contains(value) == present[value]);
    }
    assert(tree.CheckInvariants());
}

void TestClear() {
    IntTree tree = MakeTree({1, 2, 3});
    tree.Clear();
    assert(tree.IsEmpty());
    tree.Insert(10);
    assert(tree.GetCount() == 1);
}

void TestCopyIsDeep() {
    IntTree original = MakeTree({2, 1, 3});
    IntTree copy = original;
    copy.Insert(4);
    copy.Remove(1);
    assert(original.GetCount() == 3);
    assert(original.Contains(1));
    assert(!original.Contains(4));
    IntTree assigned;
    assigned = original;
    assert(assigned == original);
    assigned = assigned;
    assert(assigned.GetCount() == 3);
}

void TestMove() {
    IntTree source = MakeTree({2, 1, 3});
    IntTree moved(std::move(source));
    assert(moved.GetCount() == 3);
    assert(source.IsEmpty());
    IntTree target = MakeTree({9});
    target = std::move(moved);
    assert(target.GetCount() == 3);
    assert(!target.Contains(9));
}

void TestCustomComparator() {
    struct Greater {
        bool operator()(int a, int b) const { return a > b; }
    };
    BinaryTree<int, Greater> tree;
    for (int value : {1, 5, 3}) {
        tree.Insert(value);
    }
    assert(tree.Get(0) == 5);
    assert(tree.Get(2) == 1);
    assert(tree.CheckInvariants());
}

void TestNoLeaks() {
    {
        BinaryTree<Tracked> tree;
        for (int i = 0; i < 300; ++i) {
            tree.Insert(Tracked(i));
        }
        for (int i = 0; i < 300; i += 3) {
            tree.Remove(Tracked(i));
        }
        BinaryTree<Tracked> copy = tree;
        assert(copy.GetCount() == 200);
    }
    assert(Tracked::alive == 0);
}

}

void RunBasicTests() {
    std::cout << "Базовые операции и балансировка\n";
    RunTest("пустое дерево", TestEmpty);
    RunTest("вставка и поиск", TestInsertAndContains);
    RunTest("дубликаты не вставляются", TestDuplicates);
    RunTest("удаление листа и узла с одним ребёнком", TestRemoveLeafAndOneChild);
    RunTest("удаление узла с двумя детьми и корня", TestRemoveTwoChildrenAndRoot);
    RunTest("удаление всех элементов", TestRemoveAll);
    RunTest("балансировка на отсортированных данных", TestBalanceOnSortedInput);
    RunTest("20 000 случайных операций против эталона", TestRandomAgainstReference);
    RunTest("очистка", TestClear);
    RunTest("копия независима", TestCopyIsDeep);
    RunTest("перемещение", TestMove);
    RunTest("свой компаратор", TestCustomComparator);
    RunTest("нет утечек", TestNoLeaks);
}
