#include <string>

#include "BinaryTree.hpp"
#include <random>
#include "Tests.hpp"

namespace {

void TestFixedFormat() {
    IntTree tree = MakeTree({2, 1, 3});
    assert(tree.ToString() == "{2}({1}()[])[{3}()[]]");
    assert(IntTree().ToString().empty());
}

void TestCustomFormat() {
    IntTree tree = MakeTree({2, 1, 3});
    assert(tree.ToString("(Л)<К>[П]") == "(()<1>[])<2>[()<3>[]]");
    assert(tree.ToString("[П]{Л}(К)") == "[[]{}(3)]{[]{}(1)}(2)");
}

void TestDifferentBrackets() {
    IntTree tree = MakeTree({2, 1, 3});
    assert(tree.ToString("{К](Л)[П}") == "{2]({1]()[})[{3]()[}}");
    assert(IntTree::FromString(tree.ToString("{К](Л)[П}"), "{К](Л)[П}") == tree);
    assert(tree.ToString("(К)(Л)(П)") == "(2)((1)()())((3)()())");
}

void TestRoundTripAllOrders() {
    IntTree tree;
    std::mt19937_64 random(7);
    std::uniform_int_distribution<int> pick(-1000, 1000);
    for (int i = 0; i < 200; ++i) {
        tree.Insert(pick(random));
    }
    for (const char* format : kAllFormats) {
        IntTree restored = IntTree::FromString(tree.ToString(format), format);
        assert(restored == tree);
        assert(restored.ToString() == tree.ToString());
        assert(restored.CheckInvariants());
    }
}

void TestReadEmptyAndSpaces() {
    assert(IntTree::FromString("").IsEmpty());
    assert(IntTree::FromString("   ").IsEmpty());
    IntTree tree = IntTree::FromString(" { 2 } ( {1} ( ) [ ] ) [ {3}()[] ] ");
    assert(tree == MakeTree({1, 2, 3}));
}

void TestInvalidFormats() {
    IntTree tree = MakeTree({1});
    assert(Throws([&] { tree.ToString(""); }));
    assert(Throws([&] { tree.ToString("{К}(Л)"); }));
    assert(Throws([&] { tree.ToString("{К}(Л)[Л]"); }));
    assert(Throws([&] { tree.ToString("{К}(Л)[П"); }));
    assert(Throws([&] { tree.ToString("|К|(Л)[П]"); }));
    assert(Throws([&] { tree.ToString("{X}(Л)[П]"); }));
    assert(Throws([&] { tree.ToString("{К}(Л)[П]x"); }));
}

void TestInvalidStrings() {
    assert(Throws([] { IntTree::FromString("{1}("); }));
    assert(Throws([] { IntTree::FromString("{1}()[]]"); }));
    assert(Throws([] { IntTree::FromString("{abc}()[]"); }));
    assert(Throws([] { IntTree::FromString("{}()[]"); }));
    assert(Throws([] { IntTree::FromString("{1}[](){}"); }));
    assert(Throws([] { IntTree::FromString("{1"); }));
}

void TestOrderAndBalanceChecked() {
    assert(Throws([] { IntTree::FromString("{1}({5}()[])[]"); }));
    assert(Throws([] { IntTree::FromString("{1}({1}()[])[]"); }));
    assert(Throws([] { IntTree::FromString("{3}({2}({1}()[])[])[]"); }));
}

void TestStringValuesWithSpaces() {
    BinaryTree<std::string> tree;
    tree.Insert("hello world");
    tree.Insert("abc");
    BinaryTree<std::string> restored = BinaryTree<std::string>::FromString(tree.ToString());
    assert(restored == tree);
    assert(restored.Contains("hello world"));
}

void TestValueWithBracketRejected() {
    BinaryTree<std::string> tree;
    tree.Insert("a}b");
    assert(Throws([&] { tree.ToString(); }));
    assert(tree.ToString("<К>(Л)[П]") == "<a}b>()[]");
}

void TestDrawTopDown() {
    IntTree tree = MakeTree({2, 1, 3});
    assert(tree.Draw(100) == "   2\n ┌─┴─┐\n 1   3\n");
    assert(tree.Draw(5).empty());
    assert(IntTree().Draw(100).empty());
}

void TestStreamOperator() {
    std::ostringstream out;
    out << MakeTree({2, 1});
    assert(out.str() == "{2}({1}()[])[]");
}

}

void RunStringTests() {
    std::cout << "Сохранение в строку и чтение из строки\n";
    RunTest("фиксированный обход КЛП", TestFixedFormat);
    RunTest("строка форматирования", TestCustomFormat);
    RunTest("разные скобки слева и справа, как в задании", TestDifferentBrackets);
    RunTest("туда и обратно для всех 6 обходов", TestRoundTripAllOrders);
    RunTest("пустая строка и пробелы", TestReadEmptyAndSpaces);
    RunTest("некорректный формат", TestInvalidFormats);
    RunTest("некорректная строка дерева", TestInvalidStrings);
    RunTest("проверка порядка и баланса при чтении", TestOrderAndBalanceChecked);
    RunTest("строки с пробелами", TestStringValuesWithSpaces);
    RunTest("значение со скобкой формата", TestValueWithBracketRejected);
    RunTest("рисунок дерева сверху вниз", TestDrawTopDown);
    RunTest("оператор <<", TestStreamOperator);
}
