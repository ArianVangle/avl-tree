#include "Ui.hpp"

#include <iostream>
#include <random>
#include <sstream>
#include <string>

#include "Benchmark.hpp"
#include "BinaryTree.hpp"
#include "Console.hpp"
#include "Demo.hpp"
#include "DynamicArray.hpp"
#include "Tests.hpp"

namespace {

using Tree = BinaryTree<long long>;

const size_t kDrawWidth = 96;
bool inputClosed = false;

bool ReadLine(const std::string& prompt, std::string& line) {
    std::cout << "  " << Paint("▸ " + prompt, Color::kCyan);
    if (!std::getline(std::cin, line)) {
        inputClosed = true;
        line.clear();
        return false;
    }
    return true;
}

bool ParseNumber(const std::string& text, long long& value) {
    std::istringstream in(text);
    if (!(in >> value)) {
        return false;
    }
    in >> std::ws;
    return in.eof();
}

bool ReadNumber(const std::string& prompt, long long& value) {
    std::string line;
    if (!ReadLine(prompt, line)) {
        return false;
    }
    if (!ParseNumber(line, value)) {
        SayError("ожидалось целое число, а введено «" + line + "»");
        return false;
    }
    return true;
}

bool ReadNumberOr(const std::string& prompt, long long fallback, long long& value) {
    std::string line;
    if (!ReadLine(prompt + " [" + std::to_string(fallback) + "]: ", line)) {
        return false;
    }
    if (line.empty()) {
        value = fallback;
        return true;
    }
    if (!ParseNumber(line, value)) {
        SayError("ожидалось целое число, а введено «" + line + "»");
        return false;
    }
    return true;
}

std::string Join(const DynamicArray<long long>& values, size_t limit) {
    std::string result;
    for (size_t i = 0; i < values.GetLength() && i < limit; ++i) {
        result += (i == 0 ? "" : " ") + std::to_string(values[i]);
    }
    if (values.GetLength() > limit) {
        result += " …";
    }
    return result;
}

void ShowTree(const Tree& tree, const std::string& title) {
    SayHeader(title + ": " + std::to_string(tree.GetCount()) + " эл., высота " +
              std::to_string(tree.GetHeight()));
    if (tree.IsEmpty()) {
        SayNote("дерево пустое");
        return;
    }
    std::string picture = tree.Draw(kDrawWidth);
    if (picture.empty()) {
        SayNote("дерево не помещается на экран, рисунок не выводится");
        Say("по возрастанию: " + Join(tree.Traverse(), 20));
        return;
    }
    std::istringstream lines(picture);
    std::string line;
    while (std::getline(lines, line)) {
        Say(line);
    }
}

void OfferReplace(Tree& tree, Tree result) {
    std::string answer;
    if (ReadLine("заменить текущее дерево результатом? (д/н): ", answer) &&
        (answer == "д" || answer == "y" || answer == "да")) {
        tree = std::move(result);
        SayGood("дерево заменено");
    }
}

std::string Section(const std::string& name) { return "  " + Paint(name, Color::kGreen); }

std::string Item(const std::string& key, const std::string& name,
                 const std::string& description) {
    return "   " + Paint(PadRight(key, 2), Color::kYellow) + "  " +
           Paint(PadRight(name, 16), Color::kBold) + Paint(description, Color::kDim);
}

DynamicArray<std::string> MenuLines(const Tree& tree) {
    DynamicArray<std::string> lines;
    std::string state = "текущее дерево: " + std::to_string(tree.GetCount()) + " эл., высота " +
                        std::to_string(tree.GetHeight());
    lines.Append("");
    lines.Append("  " + Paint(state, Color::kDim));
    lines.Append("");
    lines.Append(Section("ДЕРЕВО ЦЕЛЫХ ЧИСЕЛ"));
    lines.Append(Item("1", "Показать", "рисунок дерева и обходы"));
    lines.Append(Item("2", "Вставить", "одно или несколько чисел"));
    lines.Append(Item("3", "Удалить", "элемент"));
    lines.Append(Item("4", "Найти", "элемент или k-й по порядку"));
    lines.Append(Item("5", "Заполнить", "случайно: N, диапазон, seed"));
    lines.Append(Item("6", "Очистить", ""));
    lines.Append("");
    lines.Append(Section("ОПЕРАЦИИ"));
    lines.Append(Item("7", "Функции", "map, where, reduce с выбором функции"));
    lines.Append(Item("8", "В строку", "по формату, например {К}(Л)[П]"));
    lines.Append(Item("9", "Из строки", "прочитать дерево"));
    lines.Append(Item("10", "Поддерево", "извлечь и найти вхождение"));
    lines.Append("");
    lines.Append(Section("ПРОВЕРКИ"));
    lines.Append(Item("11", "Другие типы", "вещественные, комплексные, строки, функции, люди"));
    lines.Append(Item("12", "Тесты", "все блоки, 52 теста"));
    lines.Append(Item("13", "Нагрузка", "быстрый тест, от 10 000 до 100 000 элементов"));
    lines.Append(Item("14", "Нагрузка", "полный тест, от 10 000 до 10 000 000"));
    lines.Append(Item("15", "Графики", "построить по results/results.csv"));
    lines.Append("");
    lines.Append(Item("0", "Выход", ""));
    lines.Append("");
    return lines;
}

void ScreenShow(const Tree& tree) {
    DrawWindow("Дерево");
    ShowTree(tree, "Дерево");
    if (!tree.IsEmpty()) {
        SayHeader("Обходы");
        for (const char* format : kAllFormats) {
            Say(TraversalFormat(format).Order() + ": " + Join(tree.Traverse(format), 20));
        }
    }
    Pause();
}

void ScreenInsert(Tree& tree) {
    DrawWindow("Вставка");
    std::string line;
    if (ReadLine("числа через пробел: ", line)) {
        std::istringstream in(line);
        std::string token;
        while (in >> token) {
            long long value;
            if (!ParseNumber(token, value)) {
                SayError("«" + token + "» — не число, пропущено");
            } else if (tree.Insert(value)) {
                SayGood("вставлено " + token);
            } else {
                SayNote(token + " уже есть в дереве");
            }
        }
        ShowTree(tree, "Дерево");
    }
    Pause();
}

void ScreenRemove(Tree& tree) {
    DrawWindow("Удаление");
    long long value;
    if (ReadNumber("удалить элемент: ", value)) {
        if (tree.Remove(value)) {
            SayGood("удалено");
        } else {
            SayError("такого элемента нет");
        }
        ShowTree(tree, "Дерево");
    }
    Pause();
}

void ScreenFind(Tree& tree) {
    DrawWindow("Поиск");
    long long value;
    if (ReadNumber("найти элемент: ", value)) {
        if (tree.Contains(value)) {
            SayGood("элемент есть");
        } else {
            SayError("элемента нет");
        }
    }
    if (!inputClosed && ReadNumber("индекс k (с нуля) для k-го по порядку: ", value)) {
        try {
            if (value < 0) {
                throw std::out_of_range("индекс не может быть отрицательным");
            }
            Say("элемент №" + std::to_string(value) + ": " +
                std::to_string(tree.Get(static_cast<size_t>(value))));
        } catch (const std::exception& error) {
            SayError(error.what());
        }
    }
    Pause();
}

void ScreenFill(Tree& tree) {
    DrawWindow("Автоматическое заполнение");
    long long count;
    long long low;
    long long high;
    long long seed;
    if (ReadNumberOr("количество N", 20, count) && ReadNumberOr("минимум", 0, low) &&
        ReadNumberOr("максимум", 99, high) && ReadNumberOr("seed", 1, seed)) {
        try {
            if (count < 0) {
                throw std::invalid_argument("N не может быть отрицательным");
            }
            if (low > high) {
                throw std::invalid_argument("минимум больше максимума");
            }
            std::mt19937_64 random(static_cast<uint64_t>(seed));
            std::uniform_int_distribution<long long> pick(low, high);
            tree.Clear();
            for (long long i = 0; i < count; ++i) {
                tree.Insert(pick(random));
            }
            SayGood("вставлено попыток: " + std::to_string(count) +
                    ", уникальных: " + std::to_string(tree.GetCount()));
            ShowTree(tree, "Дерево");
        } catch (const std::exception& error) {
            SayError(error.what());
        }
    }
    Pause();
}

void ScreenFunctions(Tree& tree) {
    DrawWindow("map, where, reduce");
    ShowTree(tree, "Исходное дерево");
    Say("");
    Say("map:    1) x*2   2) x+10   3) x*x   4) -x");
    Say("where:  5) чётные   6) больше k   7) меньше k");
    Say("reduce: 8) сумма   9) максимум   10) f(x, acc) = 2x + 3acc, как в задании");
    long long choice;
    if (!ReadNumber("выбор: ", choice)) {
        Pause();
        return;
    }
    try {
        if (choice >= 1 && choice <= 4) {
            Tree result = tree.Map([choice](long long x) {
                return choice == 1 ? x * 2 : choice == 2 ? x + 10 : choice == 3 ? x * x : -x;
            });
            ShowTree(result, "Результат map");
            OfferReplace(tree, std::move(result));
        } else if (choice >= 5 && choice <= 7) {
            long long k = 0;
            if (choice != 5 && !ReadNumber("k: ", k)) {
                Pause();
                return;
            }
            Tree result = tree.Where([choice, k](long long x) {
                return choice == 5 ? x % 2 == 0 : choice == 6 ? x > k : x < k;
            });
            ShowTree(result, "Результат where");
            OfferReplace(tree, std::move(result));
        } else if (choice == 8) {
            Say("сумма = " + std::to_string(tree.Reduce(
                                 [](long long x, long long acc) { return x + acc; }, 0LL)));
        } else if (choice == 9) {
            long long max = tree.Reduce([](long long x, long long acc) { return x > acc ? x : acc; },
                                        tree.Get(0));
            Say("максимум = " + std::to_string(max));
        } else if (choice == 10) {
            long long start;
            if (ReadNumberOr("стартовое значение c", 4, start)) {
                long long result = tree.Reduce(
                    [](long long x, long long acc) { return 2 * x + 3 * acc; }, start);
                Say("reduce = " + std::to_string(result));
            }
        } else {
            SayError("нет такого пункта");
        }
    } catch (const std::exception& error) {
        SayError(error.what());
    }
    Pause();
}

void ScreenSave(const Tree& tree) {
    DrawWindow("Сохранение в строку");
    Say("формат: буквы К, Л, П в любом порядке, каждая в скобках: слева ( [ { <, справа ) ] } >");
    std::string format;
    if (ReadLine(std::string("формат [") + kDefaultFormat + "]: ", format)) {
        if (format.empty()) {
            format = kDefaultFormat;
        }
        try {
            SayHeader("Строка");
            Say(tree.ToString(format));
        } catch (const std::exception& error) {
            SayError(error.what());
        }
    }
    Pause();
}

void ScreenLoad(Tree& tree) {
    DrawWindow("Чтение из строки");
    SayNote(std::string("пример в формате ") + kDefaultFormat + ": {2}({1}()[])[{3}()[]]");
    std::string text;
    std::string format;
    if (ReadLine("строка: ", text) &&
        ReadLine(std::string("формат [") + kDefaultFormat + "]: ", format)) {
        if (format.empty()) {
            format = kDefaultFormat;
        }
        try {
            Tree result = Tree::FromString(text, format);
            SayGood("прочитано");
            ShowTree(result, "Прочитанное дерево");
            OfferReplace(tree, std::move(result));
        } catch (const std::exception& error) {
            SayError(error.what());
        }
    }
    Pause();
}

void ScreenSubtree(const Tree& tree) {
    DrawWindow("Поддерево");
    ShowTree(tree, "Дерево");
    long long value;
    if (ReadNumber("корень поддерева: ", value)) {
        try {
            Tree subtree = tree.Subtree(value);
            ShowTree(subtree, "Поддерево");
            Say("в строку: " + subtree.ToString());
        } catch (const std::exception& error) {
            SayError(error.what());
        }
    }
    std::string text;
    if (!inputClosed &&
        ReadLine(std::string("проверить вхождение, дерево в формате ") + kDefaultFormat + ": ",
                 text)) {
        try {
            Tree pattern = Tree::FromString(text);
            if (tree.ContainsSubtree(pattern)) {
                SayGood("поддерево входит в дерево");
            } else {
                SayError("такого поддерева нет");
            }
        } catch (const std::exception& error) {
            SayError(error.what());
        }
    }
    Pause();
}

void PrintMenu(const std::string& hint, const Tree& tree) {
    DrawBox("Дерево поиска (AVL) · лабораторная работа №1", MenuLines(tree));
    if (!hint.empty()) {
        std::cout << "  " << Paint(hint, Color::kRed) << "\n";
    }
    std::cout << "\n  " << Paint("▸ Ваш выбор: ", Color::kCyan);
    std::cout.flush();
}

void ShowTests() {
    DrawWindow("Тесты: операции, функции, строки, поддеревья, типы");
    RunAllTests();
    std::cout << "\n";
    SayGood("Готово");
    Pause();
}

void ShowBenchmark(BenchmarkMode mode) {
    DrawWindow(mode == BenchmarkMode::Quick ? "Нагрузочный тест (быстрый)"
                                            : "Нагрузочный тест (полный, около минуты)");
    bool saved = RunBenchmark(mode);
    std::cout << "\n";
    if (saved) {
        SayGood("Готово");
    } else {
        SayError("Результаты не сохранены");
    }
    Pause();
}

bool HandleChoice(const std::string& choice, Tree& tree) {
    if (choice == "1") {
        ScreenShow(tree);
    } else if (choice == "2") {
        ScreenInsert(tree);
    } else if (choice == "3") {
        ScreenRemove(tree);
    } else if (choice == "4") {
        ScreenFind(tree);
    } else if (choice == "5") {
        ScreenFill(tree);
    } else if (choice == "6") {
        tree.Clear();
    } else if (choice == "7") {
        ScreenFunctions(tree);
    } else if (choice == "8") {
        ScreenSave(tree);
    } else if (choice == "9") {
        ScreenLoad(tree);
    } else if (choice == "10") {
        ScreenSubtree(tree);
    } else if (choice == "11") {
        DemoTypes();
    } else if (choice == "12") {
        ShowTests();
    } else if (choice == "13") {
        ShowBenchmark(BenchmarkMode::Quick);
    } else if (choice == "14") {
        ShowBenchmark(BenchmarkMode::Full);
    } else if (choice == "15") {
        RunCommand("Графики", "python3 scripts/plot.py");
    } else {
        return false;
    }
    return true;
}

}

void RunUi() {
    Tree tree;
    for (long long value : {50, 30, 70, 20, 40, 60, 80}) {
        tree.Insert(value);
    }
    std::string choice;
    std::string hint;
    while (true) {
        PrintMenu(hint, tree);
        if (!std::getline(std::cin, choice) || choice == "0") {
            break;
        }
        bool known = HandleChoice(choice, tree);
        if (inputClosed) {
            break;
        }
        hint = known ? "" : "Нет пункта «" + choice + "», введите число от 0 до 15";
    }
    ClearScreen();
    std::cout.flush();
}
