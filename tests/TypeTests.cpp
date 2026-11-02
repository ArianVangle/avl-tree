#include <complex>
#include <functional>
#include <string>

#include "BinaryTree.hpp"
#include "Person.hpp"
#include "Tests.hpp"

namespace {

struct ByModulus {
    bool operator()(const std::complex<double>& a, const std::complex<double>& b) const {
        return std::abs(a) < std::abs(b);
    }
};

using Function = int (*)(int);

int AddOne(int x) { return x + 1; }
int Twice(int x) { return x * 2; }
int Square(int x) { return x * x; }

void TestDoubles() {
    BinaryTree<double> tree;
    for (double value : {2.5, -1.25, 3.75}) {
        tree.Insert(value);
    }
    assert(tree.Get(0) == -1.25);
    assert(BinaryTree<double>::FromString(tree.ToString()) == tree);
}

void TestComplex() {
    BinaryTree<std::complex<double>, ByModulus> tree;
    tree.Insert({3, 4});
    tree.Insert({1, 0});
    tree.Insert({0, 10});
    assert(tree.Get(0) == std::complex<double>(1, 0));
    assert(tree.Get(2) == std::complex<double>(0, 10));
    auto restored = BinaryTree<std::complex<double>, ByModulus>::FromString(tree.ToString());
    assert(restored == tree);
}

void TestStrings() {
    BinaryTree<std::string> tree;
    for (const char* word : {"дерево", "abc", "лес"}) {
        tree.Insert(word);
    }
    assert(tree.Get(0) == "abc");
    assert(tree.Map([](const std::string& s) { return s + "!"; }).Contains("лес!"));
    std::string joined = tree.Reduce(
        [](const std::string& s, std::string acc) { return acc + s + ";"; }, std::string());
    assert(joined == "abc;дерево;лес;");
}

void TestFunctions() {
    BinaryTree<Function, std::less<Function>> tree;
    for (Function f : {AddOne, Twice, Square}) {
        tree.Insert(f);
    }
    assert(tree.GetCount() == 3);
    assert(tree.Contains(Twice));
    assert(!tree.Insert(Square));
    int sum = tree.Reduce([](Function f, int acc) { return acc + f(5); }, 0);
    assert(sum == 6 + 10 + 25);
    auto chosen = tree.Where([](Function f) { return f(3) > 5; });
    assert(chosen.GetCount() == 2 && !chosen.Contains(AddOne));
}

void TestPeople() {
    BinaryTree<Student> students;
    students.Insert({{1003, "Иванов Иван"}});
    students.Insert({{1001, "Петров Пётр"}});
    students.Insert({{1002, "Сидорова Анна"}});
    assert(students.Get(0).name == "Петров Пётр");
    assert(students.Contains({{1002, ""}}));
    assert(BinaryTree<Student>::FromString(students.ToString()) == students);
    assert(Throws([] { BinaryTree<Student>::FromString("{1001}()[]"); }));

    BinaryTree<Teacher> teachers;
    teachers.Insert({{20, "Смирнов С. С."}});
    teachers.Insert({{10, "Кузнецова К. К."}});
    assert(teachers.Get(0).name == "Кузнецова К. К.");
    assert(BinaryTree<Teacher>::FromString(teachers.ToString()) == teachers);
}

void TestTrackedObjects() {
    {
        BinaryTree<Tracked> tree;
        for (int i = 0; i < 50; ++i) {
            tree.Insert(Tracked(i));
        }
        auto mapped = tree.Map([](const Tracked& t) { return Tracked(t.value * 3); });
        auto filtered = tree.Where([](const Tracked& t) { return t.value % 2 == 0; });
        auto restored = BinaryTree<Tracked>::FromString(tree.ToString());
        assert(mapped.GetCount() == 50);
        assert(filtered.GetCount() == 25);
        assert(restored == tree);
        assert(tree.ContainsSubtree(tree.Subtree(Tracked(10))));
    }
    assert(Tracked::alive == 0);
}

}

void RunTypeTests() {
    std::cout << "Разные типы элементов\n";
    RunTest("вещественные числа", TestDoubles);
    RunTest("комплексные числа со своим компаратором", TestComplex);
    RunTest("строки", TestStrings);
    RunTest("функции (указатели на функции)", TestFunctions);
    RunTest("студенты и преподаватели", TestPeople);
    RunTest("объекты со счётчиком: нет утечек", TestTrackedObjects);
}
