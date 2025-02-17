#include <benchmark/benchmark.h>

namespace {

static void BM_PossibleBoards(benchmark::State& state) {
  for (auto _ : state) {
    // TODO
  }
}

static void BM_PossibleBoards_C(benchmark::State& state) {
  for (auto _ : state) {
    // TODO
  }
}

BENCHMARK(BM_PossibleBoards);
BENCHMARK(BM_PossibleBoards_C);

}  // namespace

BENCHMARK_MAIN();
