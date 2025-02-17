#include <benchmark/benchmark.h>

#include <array>

#include "xiangqi/board.h"
#include "xiangqi/board_c.h"

namespace {

namespace {

using ::xq::Board;
using ::xq::kStartingBoard;
using ::xq::PossibleBoards;

}  // namespace

static void BM_PossibleBoards(benchmark::State& state) {
  for (auto _ : state) {
    PossibleBoards(kStartingBoard, PLAYER_RED, false);
  }
}

static void BM_PossibleBoards_C(benchmark::State& state) {
  std::array<Piece, K_BOARD_SIZE * K_MAX_MOVE_PER_PLAYER> out;
  for (auto _ : state) {
    PossibleBoards_C(K_STARTING_BOARD, PLAYER_RED, false, out.data());
  }
}

BENCHMARK(BM_PossibleBoards);
BENCHMARK(BM_PossibleBoards_C);

}  // namespace

BENCHMARK_MAIN();
