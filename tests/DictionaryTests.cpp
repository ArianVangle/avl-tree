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


Dictionary<int, std::string> Numbers() {
    Dictionary<int, std::string> dictionary;
    const char* names[] = {"ноль", "один", "два", "три", "четыре", "пять", "шесть"};
    for (int key : {3, 1, 5, 0, 2, 4, 6}) {
        dictionary.Add(key, names[key]);
    }
    return dictionary;
}

void TestDictionaryIsTree() {
    Dictionary<int, std::string> dictionary = Numbers();
    const ICollection<int>& keys = dictionary;
    assert(keys.Get(0) == 0 && keys.Get(6) == 6);
    assert(dictionary.CheckInvariants());
    assert(dictionary.GetHeight() == 3);
    assert(dictionary.ToString() == "{3}({1}({0}()[])[{2}()[]])[{5}({4}()[])[{6}()[]]]");
}

void TestEnumeratorWithValues() {
    Dictionary<int, std::string> dictionary = Numbers();
    for (const char* format : kAllFormats) {
        auto enumerator = dictionary.GetEnumerator(format);
        size_t count = 0;
        while (enumerator->MoveNext()) {
            assert(enumerator->GetValue() == dictionary.Get(enumerator->GetCurrent()));
            ++count;
        }
        assert(count == dictionary.GetCount());
    }
}

void TestKeys() {
    Set<int> keys = Numbers().Keys();
    assert(keys == MakeTree({0, 1, 2, 3, 4, 5, 6}));
    assert(keys.CheckInvariants());
    assert((Dictionary<int, int>().Keys().IsEmpty()));
}

void TestSetToDictionary() {
    Set<int> set = MakeTree({4, 2, 6, 1, 3, 5, 7});
    Dictionary<int, int> squares(set, [](int x) { return x * x; });
    assert(squares.GetCount() == 7);
    assert(squares.Get(3) == 9 && squares.Get(7) == 49);
    assert(squares.CheckInvariants());
    assert(squares.Keys() == set);

    Set<std::string> words;
    for (const char* word : {"лес", "дерево", "ab"}) {
        words.Insert(word);
    }
    Dictionary<std::string, size_t> lengths(words, [](const std::string& w) { return w.size(); });
    assert(lengths.Get("ab") == 2 && lengths.Get("лес") == 6);
    assert((Dictionary<int, int>(Set<int>(), [](int x) { return x; }).GetCount() == 0));
}

void TestWhereKeepsValues() {
    auto even = Numbers().Where([](int key) { return key % 2 == 0; });
    assert(even.GetCount() == 4 && !even.Contains(3));
    assert(*even.Find(4) == "четыре" && even.Find(3) == nullptr);
}

void TestRemoveWithTwoChildren() {
    Dictionary<int, std::string> dictionary = Numbers();
    dictionary.Remove(3);
    assert(dictionary.CheckInvariants());
    assert(dictionary.GetCount() == 6 && !dictionary.ContainsKey(3));
    assert(dictionary.Get(4) == "четыре" && dictionary.Get(2) == "два");
}
}

void RunDictionaryTests() {
    std::cout << "Словарь на дереве BinaryTree<TKey, TValue>\n";
    RunTest("добавление и получение по ключу", TestAddAndGet);
    RunTest("повторный ключ заменяет значение", TestAddReplacesValue);
    RunTest("удаление по ключу", TestRemove);
    RunTest("отсутствующий ключ — исключение", TestMissingKey);
    RunTest("тысяча ключей", TestManyKeys);
    RunTest("работа через IDictionary", TestThroughInterface);
    RunTest("значения со счётчиком: нет утечек", TestTrackedValues);
    RunTest("словарь — это дерево: k-й ключ, строка, баланс", TestDictionaryIsTree);
    RunTest("перечислитель словаря: ключ и значение", TestEnumeratorWithValues);
    RunTest("словарь → множество ключей", TestKeys);
    RunTest("множество → словарь и обратно", TestSetToDictionary);
    RunTest("where сохраняет значения", TestWhereKeepsValues);
    RunTest("удаление узла с двумя потомками", TestRemoveWithTwoChildren);
}
