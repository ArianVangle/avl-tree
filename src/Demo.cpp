#include "Demo.hpp"

#include <complex>
#include <functional>
#include <string>

#include "BinaryTree.hpp"
#include "Console.hpp"
#include "Person.hpp"

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

}

void DemoTypes() {
    DrawWindow("Другие типы элементов");

    SayHeader("вещественные числа");
    BinaryTree<double> doubles;
    for (double x : {3.14, -2.5, 10.0, 0.5}) {
        doubles.Insert(x);
    }
    Say("дерево: " + doubles.ToString());
    Say("map x/2: " + doubles.Map([](double x) { return x / 2; }).ToString());

    SayHeader("комплексные числа (сравнение по модулю)");
    BinaryTree<std::complex<double>, ByModulus> complexes;
    for (auto z : {std::complex<double>(3, 4), std::complex<double>(1, 1),
                   std::complex<double>(0, 10)}) {
        complexes.Insert(z);
    }
    Say("дерево: " + complexes.ToString("<К>(Л)[П]"));

    SayHeader("строки");
    BinaryTree<std::string> words;
    for (const char* word : {"липа", "берёза", "сосна", "ель"}) {
        words.Insert(word);
    }
    Say("дерево: " + words.ToString());

    SayHeader("функции (указатели на функции)");
    BinaryTree<Function, std::less<Function>> functions;
    for (Function f : {AddOne, Twice, Square}) {
        functions.Insert(f);
    }
    int sum = functions.Reduce([](Function f, int acc) { return acc + f(5); }, 0);
    Say("в дереве функций: " + std::to_string(functions.GetCount()) +
        " (x+1, 2x, x²), есть ли 2x: " + (functions.Contains(Twice) ? "да" : "нет"));
    Say("reduce: сумма f(5) по всем функциям = " + std::to_string(sum));

    SayHeader("студенты и преподаватели (порядок по номеру)");
    BinaryTree<Student> students;
    students.Insert({{1003, "Иванов Иван"}});
    students.Insert({{1001, "Петрова Анна"}});
    students.Insert({{1002, "Сидоров Пётр"}});
    Say("студенты: " + students.ToString());
    BinaryTree<Teacher> teachers;
    teachers.Insert({{20, "Смирнов С. С."}});
    teachers.Insert({{10, "Кузнецова К. К."}});
    Say("преподаватели: " + teachers.ToString());
    Pause();
}
