#include "Benchmark.hpp"

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <set>
#include <string>

#include "BinaryTree.hpp"
#include "DynamicArray.hpp"

namespace {

using Clock = std::chrono::steady_clock;

struct Result {
    double insertMs = 0;
    double searchMs = 0;
    double removeMs = 0;
    int height = 0;

    double TotalMs() const { return insertMs + searchMs + removeMs; }
};

double Ms(Clock::time_point from, Clock::time_point to) {
    return std::chrono::duration<double, std::milli>(to - from).count();
}

DynamicArray<long long> MakeKeys(size_t n, uint64_t seed) {
    std::mt19937_64 random(seed);
    DynamicArray<long long> keys;
    for (size_t i = 0; i < n; ++i) {
        keys.Append(static_cast<long long>(random() >> 1));
    }
    return keys;
}

Result MeasureTree(const DynamicArray<long long>& keys) {
    Result result;
    BinaryTree<long long> tree;
    size_t n = keys.GetLength();
    auto start = Clock::now();
    for (size_t i = 0; i < n; ++i) {
        tree.Insert(keys[i]);
    }
    auto inserted = Clock::now();
    size_t found = 0;
    for (size_t i = 0; i < n; ++i) {
        found += tree.Contains(keys[i]) ? 1 : 0;
    }
    auto searched = Clock::now();
    result.height = tree.GetHeight();
    for (size_t i = 0; i < n; ++i) {
        tree.Remove(keys[i]);
    }
    auto removed = Clock::now();
    if (found != n || !tree.IsEmpty()) {
        std::cout << "ошибка: найдено " << found << " из " << n << "\n";
    }
    result.insertMs = Ms(start, inserted);
    result.searchMs = Ms(inserted, searched);
    result.removeMs = Ms(searched, removed);
    return result;
}

Result MeasureStdSet(const DynamicArray<long long>& keys) {
    Result result;
    std::set<long long> set;
    size_t n = keys.GetLength();
    auto start = Clock::now();
    for (size_t i = 0; i < n; ++i) {
        set.insert(keys[i]);
    }
    auto inserted = Clock::now();
    size_t found = 0;
    for (size_t i = 0; i < n; ++i) {
        found += set.count(keys[i]);
    }
    auto searched = Clock::now();
    for (size_t i = 0; i < n; ++i) {
        set.erase(keys[i]);
    }
    auto removed = Clock::now();
    if (found != n || !set.empty()) {
        std::cout << "ошибка: найдено " << found << " из " << n << "\n";
    }
    result.insertMs = Ms(start, inserted);
    result.searchMs = Ms(inserted, searched);
    result.removeMs = Ms(searched, removed);
    return result;
}

DynamicArray<size_t> MakeSizes(BenchmarkMode mode) {
    size_t last = 100000;
    if (mode == BenchmarkMode::Full) {
        last = 10000000;
    } else if (mode == BenchmarkMode::Huge) {
        last = 100000000;
    }
    DynamicArray<size_t> sizes;
    for (size_t base = 10000; base <= last; base *= 10) {
        for (size_t factor : {1, 2, 5}) {
            if (base * factor <= last) {
                sizes.Append(base * factor);
            }
        }
    }
    return sizes;
}

int RoundsFor(size_t n) { return n <= 1000000 ? 3 : 1; }

void Keep(Result& best, const Result& current, int round) {
    if (round == 0 || current.TotalMs() < best.TotalMs()) {
        best = current;
    }
}

void PrintRow(const std::string& name, size_t n, const Result& r) {
    std::cout << std::left << std::setw(10) << name << std::setw(11) << n << std::fixed
              << std::setprecision(2) << std::setw(12) << r.insertMs << std::setw(12)
              << r.searchMs << std::setw(12) << r.removeMs << std::setprecision(1)
              << std::setw(10) << r.TotalMs() * 1e6 / static_cast<double>(n) / 3.0;
    if (r.height > 0) {
        std::cout << r.height;
    }
    std::cout << "\n";
}

void WriteRow(std::ofstream& csv, const std::string& name, size_t n, const Result& r) {
    csv << name << "," << n << "," << r.insertMs << "," << r.searchMs << "," << r.removeMs
        << "," << r.height << "\n";
}

}

bool RunBenchmark(BenchmarkMode mode) {
    std::ofstream csv("results/results.csv");
    if (!csv) {
        std::cout << "не удалось открыть results/results.csv: запускайте программу из корня "
                     "проекта\n";
        return false;
    }
    csv << std::setprecision(10);
    csv << "structure,n,insert_ms,search_ms,remove_ms,height\n";
    if (mode == BenchmarkMode::Huge) {
        std::cout << "режим до 10^8: нужно около 6 ГБ свободной памяти\n\n";
    }
    std::cout << std::left << std::setw(10) << "structure" << std::setw(11) << "n"
              << std::setw(12) << "insert_ms" << std::setw(12) << "search_ms" << std::setw(12)
              << "remove_ms" << std::setw(10) << "ns/op" << "height\n";
    DynamicArray<size_t> sizes = MakeSizes(mode);
    for (size_t k = 0; k < sizes.GetLength(); ++k) {
        size_t n = sizes[k];
        DynamicArray<long long> keys = MakeKeys(n, 12345 + n);
        Result tree;
        Result standard;
        for (int round = 0; round < RoundsFor(n); ++round) {
            Keep(tree, MeasureTree(keys), round);
            Keep(standard, MeasureStdSet(keys), round);
        }
        PrintRow("AVL", n, tree);
        PrintRow("std::set", n, standard);
        WriteRow(csv, "AVL", n, tree);
        WriteRow(csv, "std_set", n, standard);
        std::cout.flush();
    }
    std::cout << std::defaultfloat << std::setprecision(6);
    std::cout << "\nрезультаты сохранены в results/results.csv" << std::endl;
    return true;
}
