#include "BinaryTree.hpp"
#include "Tests.hpp"

namespace {

IntTree Sample() { return MakeTree({5, 3, 8, 1, 4, 7, 9}); }

void TestSubtreeOfInnerNode() {
    IntTree subtree = Sample().Subtree(3);
    assert(subtree == MakeTree({1, 3, 4}));
    assert(subtree.ToString() == "{3}({1}()[])[{4}()[]]");
    assert(subtree.CheckInvariants());
}

void TestSubtreeOfRootAndLeaf() {
    IntTree tree = Sample();
    assert(tree.Subtree(5) == tree);
    IntTree leaf = tree.Subtree(9);
    assert(leaf.GetCount() == 1);
    assert(leaf.GetHeight() == 1);
}

void TestSubtreeIsCopy() {
    IntTree tree = Sample();
    IntTree subtree = tree.Subtree(8);
    subtree.Insert(100);
    subtree.Remove(7);
    assert(tree.Contains(7));
    assert(!tree.Contains(100));
}

void TestSubtreeMissing() {
    IntTree tree = Sample();
    assert(Throws([&] { tree.Subtree(6); }));
    assert(Throws([] { IntTree().Subtree(1); }));
}

void TestContainsSubtree() {
    IntTree tree = Sample();
    assert(tree.ContainsSubtree(tree.Subtree(3)));
    assert(tree.ContainsSubtree(tree));
    assert(tree.ContainsSubtree(IntTree()));
    assert(tree.ContainsSubtree(IntTree::FromString("{9}()[]")));
    assert(!tree.ContainsSubtree(IntTree::FromString("{8}({7}()[])[]")));
    assert(!tree.ContainsSubtree(IntTree::FromString("{6}()[]")));
    assert(!IntTree().ContainsSubtree(MakeTree({1})));
}

void TestContainsSubtreeShapeMatters() {
    IntTree tree = MakeTree({2, 1, 3, 4});
    IntTree sameValues = IntTree::FromString("{3}()[{4}()[]]");
    IntTree otherShape = IntTree::FromString("{4}({3}()[])[]");
    assert(tree.ContainsSubtree(sameValues));
    assert(!tree.ContainsSubtree(otherShape));
}

}

void RunSubtreeTests() {
    std::cout << "Поддеревья\n";
    RunTest("извлечение поддерева внутреннего узла", TestSubtreeOfInnerNode);
    RunTest("поддерево корня и листа", TestSubtreeOfRootAndLeaf);
    RunTest("поддерево — независимая копия", TestSubtreeIsCopy);
    RunTest("нет такого элемента", TestSubtreeMissing);
    RunTest("поиск на вхождение поддерева", TestContainsSubtree);
    RunTest("вхождение учитывает форму", TestContainsSubtreeShapeMatters);
}
