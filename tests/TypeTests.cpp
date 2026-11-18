#include <complex>
#include <string>

#include "BinaryTree.hpp"
#include "Dictionary.hpp"
#include "Person.hpp"
#include "Tests.hpp"

namespace {

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
    Dictionary<int, std::complex<double>> numbers;
    numbers.Add(1, {3, 4});
    numbers.Add(2, {1, 0});
    assert(numbers.Get(1) == std::complex<double>(3, 4));
    assert(numbers.GetCount() == 2);
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
    Dictionary<std::string, Function> functions;
    functions.Add("x+1", AddOne);
    functions.Add("2x", Twice);
    functions.Add("x²", Square);
    assert(functions.GetCount() == 3);
    assert(functions.Get("2x")(5) == 10);
    assert(functions.Get("x²")(5) == 25);
    assert(!functions.ContainsKey("x³"));
}

void TestPeople() {
    Dictionary<int, Student> students;
    students.Add(1003, {{1003, "Иванов Иван"}});
    students.Add(1001, {{1001, "Петров Пётр"}});
    assert(students.Get(1001).name == "Петров Пётр");
    assert(students.ContainsKey(1003) && !students.ContainsKey(1002));

    Dictionary<int, Teacher> teachers;
    teachers.Add(20, {{20, "Смирнов С. С."}});
    teachers.Add(10, {{10, "Кузнецова К. К."}});
    assert(teachers.Get(10).name == "Кузнецова К. К.");
    assert(teachers.GetCount() == 2);
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
    RunTest("комплексные числа в словаре", TestComplex);
    RunTest("строки", TestStrings);
    RunTest("функции в словаре", TestFunctions);
    RunTest("студенты и преподаватели в словаре", TestPeople);
    RunTest("объекты со счётчиком: нет утечек", TestTrackedObjects);
}
