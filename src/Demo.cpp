#include "Demo.hpp"

#include <complex>
#include <string>

#include "BinaryTree.hpp"
#include "Console.hpp"
#include "Dictionary.hpp"
#include "Person.hpp"

namespace {

using Function = int (*)(int);

int AddOne(int x) { return x + 1; }
int Twice(int x) { return x * 2; }
int Square(int x) { return x * x; }

}

void DemoTypes() {
    DrawWindow("Другие типы элементов");

    SayHeader("дерево вещественных чисел");
    BinaryTree<double> doubles;
    for (double x : {3.14, -2.5, 10.0, 0.5}) {
        doubles.Insert(x);
    }
    Say("дерево: " + doubles.ToString());
    Say("map x/2: " + doubles.Map([](double x) { return x / 2; }).ToString());

    SayHeader("дерево строк");
    BinaryTree<std::string> words;
    for (const char* word : {"липа", "берёза", "сосна", "ель"}) {
        words.Insert(word);
    }
    Say("дерево: " + words.ToString());

    SayHeader("словарь комплексных чисел (ключ — номер)");
    
    Dictionary<int, std::complex<double>> numbers;
    numbers.Add(2, {3, 4});
    numbers.Add(1, {1, 1});
    numbers.Add(3, {0, 10});

    for (int key = 1; key <= 3; ++key) {
        Say(std::to_string(key) + " → " + ValueToText(numbers.Get(key)));
    }

    SayHeader("словарь функций (ключ — название)");

    Dictionary<std::string, Function> functions;
    functions.Add("x+1", AddOne);
    functions.Add("2x", Twice);
    functions.Add("x²", Square);

    for (const char* name : {"x+1", "2x", "x²"}) {
        Say(std::string(name) + " при x = 5: " + std::to_string(functions.Get(name)(5)));
    }

    SayHeader("словари студентов и преподавателей (ключ — номер)");

    Dictionary<int, Student> students;
    students.Add(1003, {{1003, "Иванов Иван"}});
    students.Add(1001, {{1001, "Петрова Анна"}});

    Say("студент 1001: " + ValueToText(students.Get(1001)));

    Dictionary<int, Teacher> teachers;
    teachers.Add(20, {{20, "Смирнов С. С."}});

    Say("преподаватель 20: " + ValueToText(teachers.Get(20)));
    Say("всего студентов: " + std::to_string(students.GetCount()) +
        ", есть ли 1002: " + (students.ContainsKey(1002) ? "да" : "нет"));

    SayHeader("множество → словарь → множество");

    Set<int> set;
    for (int x : {4, 2, 6, 1, 3}) {
        set.Insert(x);
    }
    Dictionary<int, int> squares(set, [](int x) { return x * x; });
    Say("множество: " + set.ToString());
    auto enumerator = squares.GetEnumerator("(Л){К}[П]");
    std::string pairs;
    while (enumerator->MoveNext()) {
        pairs += std::to_string(enumerator->GetCurrent()) + "→" +
                 std::to_string(enumerator->GetValue()) + " ";
    }
    Say("словарь x → x²: " + pairs);
    Say(std::string("ключи словаря равны множеству: ") + (squares.Keys() == set ? "да" : "нет"));
    Pause();
}
