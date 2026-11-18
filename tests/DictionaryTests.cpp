#include <string>

#include "Dictionary.hpp"
#include "Tests.hpp"

namespace {

void TestAddAndGet() {
    Dictionary<int, std::string> dictionary;
    dictionary.Add(2, "два");
    dictionary.Add(1, "один");
    dictionary.Add(3, "три");
    assert(dictionary.GetCount() == 3);
    assert(dictionary.Get(1) == "один");
    assert(dictionary.Get(3) == "три");
    assert(dictionary.ContainsKey(2));
    assert(!dictionary.ContainsKey(4));
}

void TestAddReplacesValue() {
    Dictionary<int, std::string> dictionary;
    dictionary.Add(1, "старое");
    dictionary.Add(1, "новое");
    assert(dictionary.GetCount() == 1);
    assert(dictionary.Get(1) == "новое");
}

void TestRemove() {
    Dictionary<int, std::string> dictionary;
    dictionary.Add(1, "один");
    dictionary.Add(2, "два");
    dictionary.Remove(1);
    assert(dictionary.GetCount() == 1);
    assert(!dictionary.ContainsKey(1));
    assert(dictionary.Get(2) == "два");
}

void TestMissingKey() {
    Dictionary<int, std::string> dictionary;
    assert(Throws([&] { dictionary.Get(1); }));
    assert(Throws([&] { dictionary.Remove(1); }));
    dictionary.Add(1, "один");
    assert(Throws([&] { dictionary.Get(2); }));
    assert(dictionary.GetCount() == 1);
}

void TestManyKeys() {
    Dictionary<int, int> dictionary;
    for (int key = 0; key < 1000; ++key) {
        dictionary.Add((key * 37) % 1000, key);
    }
    assert(dictionary.GetCount() == 1000);
    for (int key = 0; key < 1000; ++key) {
        assert(dictionary.Get((key * 37) % 1000) == key);
    }
    for (int key = 0; key < 1000; key += 2) {
        dictionary.Remove(key);
    }
    assert(dictionary.GetCount() == 500);
    assert(!dictionary.ContainsKey(10) && dictionary.ContainsKey(11));
}

void TestThroughInterface() {
    Dictionary<std::string, int> dictionary;
    IDictionary<std::string, int>& base = dictionary;
    base.Add("a", 1);
    base.Add("b", 2);
    assert(base.GetCount() == 2);
    assert(base.Get("b") == 2);
}

void TestTrackedValues() {
    {
        Dictionary<int, Tracked> dictionary;
        for (int key = 0; key < 100; ++key) {
            dictionary.Add(key, Tracked(key));
        }
        dictionary.Add(5, Tracked(500));
        dictionary.Remove(7);
        assert(dictionary.Get(5).value == 500);
        assert(dictionary.GetCount() == 99);
    }
    assert(Tracked::alive == 0);
}

}

void RunDictionaryTests() {
    std::cout << "Словарь поверх дерева\n";
    RunTest("добавление и получение по ключу", TestAddAndGet);
    RunTest("повторный ключ заменяет значение", TestAddReplacesValue);
    RunTest("удаление по ключу", TestRemove);
    RunTest("отсутствующий ключ — исключение", TestMissingKey);
    RunTest("тысяча ключей", TestManyKeys);
    RunTest("работа через IDictionary", TestThroughInterface);
    RunTest("значения со счётчиком: нет утечек", TestTrackedValues);
}
