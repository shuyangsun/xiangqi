#include <benchmark/benchmark.h>

#include <array>

#include "xiangqi/board.h"
#include "xiangqi/board_c.h"

namespace {

namespace {

using ::xq::Board;
using ::xq::kStartingBoard;
using ::xq::PossibleBoards;
using ::xq::PossibleMoves;

}  // namespace

static void BM_PossibleMoves_C(benchmark::State& state) {
  MaxMovesPerPlayerC out;
  for (auto _ : state) {
    PossibleMoves_C(K_STARTING_BOARD, PLAYER_RED, false, out);
  }
}

static void BM_PossibleMoves_C_AvoidCheckmate(benchmark::State& state) {
  MaxMovesPerPlayerC out;
  for (auto _ : state) {
    PossibleMoves_C(K_STARTING_BOARD, PLAYER_RED, true, out);
  }
}

static void BM_PossibleMoves(benchmark::State& state) {
  for (auto _ : state) {
    PossibleMoves(kStartingBoard, PLAYER_RED, false);
  }
}

static void BM_PossibleMoves_AvoidCheckmate(benchmark::State& state) {
  for (auto _ : state) {
    PossibleMoves(kStartingBoard, PLAYER_RED, true);
  }
}

static void BM_PossibleBoards_C(benchmark::State& state) {
  std::array<Piece, K_BOARD_SIZE * K_MAX_MOVE_PER_PLAYER> out;
  for (auto _ : state) {
    PossibleBoards_C(K_STARTING_BOARD, PLAYER_RED, false, out.data());
  }
}

static void BM_PossibleBoards_C_AvoidCheckmate(benchmark::State& state) {
  std::array<Piece, K_BOARD_SIZE * K_MAX_MOVE_PER_PLAYER> out;
  for (auto _ : state) {
    PossibleBoards_C(K_STARTING_BOARD, PLAYER_RED, true, out.data());
  }
}

static void BM_PossibleBoards(benchmark::State& state) {
  for (auto _ : state) {
    PossibleBoards(kStartingBoard, PLAYER_RED, false);
  }
}

static void BM_PossibleBoards_AvoidCheckmate(benchmark::State& state) {
  for (auto _ : state) {
    PossibleBoards(kStartingBoard, PLAYER_RED, true);
  }
}

BENCHMARK(BM_PossibleMoves_C);
BENCHMARK(BM_PossibleMoves_C_AvoidCheckmate);
BENCHMARK(BM_PossibleMoves);
BENCHMARK(BM_PossibleMoves_AvoidCheckmate);

BENCHMARK(BM_PossibleBoards_C);
BENCHMARK(BM_PossibleBoards_C_AvoidCheckmate);
BENCHMARK(BM_PossibleBoards);
BENCHMARK(BM_PossibleBoards_AvoidCheckmate);

}  // namespace

BENCHMARK_MAIN();
