#include <string>

#include "Benchmark.hpp"
#include "Tests.hpp"
#include "Ui.hpp"

int main(int argc, char** argv) {
    std::string mode = argc > 1 ? argv[1] : "";
    if (mode == "tests") {
        RunAllTests();
        return 0;
    }
    if (mode == "benchmark") {
        std::string size = argc > 2 ? argv[2] : "";
        BenchmarkMode benchmarkMode = BenchmarkMode::Full;
        if (size == "quick") {
            benchmarkMode = BenchmarkMode::Quick;
        } else if (size == "huge") {
            benchmarkMode = BenchmarkMode::Huge;
        }
        return RunBenchmark(benchmarkMode) ? 0 : 1;
    }
    RunUi();
    return 0;
}
