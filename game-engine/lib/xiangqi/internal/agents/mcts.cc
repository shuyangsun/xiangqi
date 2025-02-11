#include "xiangqi/internal/agents/mcts.h"

#include <random>
#include <thread>
#include <vector>

#include "xiangqi/board.h"
#include "xiangqi/types.h"

namespace xq::internal::agent {

namespace {

constexpr size_t kDefaultNumSimulations = 1000;

Winner MakeRandomMoveUntilGameOver(const Board<Piece>& board, Player player) {
  Board<Piece> cur_board = board;
  Player cur_player = player;

  std::random_device rd;
  std::mt19937 rand_gen(rd());

  while (!IsGameOver(cur_board)) {
    const std::vector<uint16_t> possible_moves =
        AllPossibleNextMoves(cur_board, cur_player);
    std::uniform_int_distribution<> distrib(
        0, static_cast<int>(possible_moves.size()) - 1);
    int rand_idx = distrib(rand_gen);
    const uint16_t move = possible_moves[rand_idx];
    Move(cur_board, static_cast<uint8_t>((move & 0xFF00) >> 8),
         static_cast<uint8_t>(move & 0x00FF));
    cur_player = ChangePlayer(cur_player);
  }
  return GetWinner(cur_board);
}

}  // namespace

MCTS::MCTS(size_t num_iter, size_t depth, float exploration_constant)
    : num_iter_{num_iter},
      depth_{depth},
      exploration_constant_{exploration_constant} {}

uint16_t MCTS::MakeMove(const Board<Piece>& board, Player player) const {
  const std::vector<uint16_t> possible_moves =
      AllPossibleNextMoves(board, player);
  uint16_t best_move = 0xFFFF;
  size_t best_move_wins = 0;
  size_t best_move_draws = 0;
  for (const uint16_t move : possible_moves) {
    Board<Piece> next = board;
    Move(next, static_cast<uint8_t>((move & 0xFF00) >> 8),
         static_cast<uint8_t>(move & 0x00FF));
    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) {
      num_threads = 8;
    }

    std::vector<size_t> wins(num_threads, 0);
    std::vector<size_t> draws(num_threads, 0);
    std::vector<std::thread> threads;

    size_t simulations_per_thread = num_iter_ / num_threads;
    size_t remainder = num_iter_ % num_threads;
    size_t start = 0;

    for (unsigned int t = 0; t < num_threads; t++) {
      // Distribute any remainder among the first few threads.
      size_t end = start + simulations_per_thread + (t < remainder ? 1 : 0);

      threads.emplace_back(
          [next, player, start, end, t, &wins, &draws, this]() {
            for (size_t i = start; i < end; i++) {
              const Winner winner =
                  MakeRandomMoveUntilGameOver(next, ChangePlayer(player));
              if ((winner == Winner::BLACK && player == Player::BLACK) ||
                  (winner == Winner::RED && player == Player::RED)) {
                wins[t]++;
              } else if (winner == Winner::DRAW) {
                draws[t]++;
              }
            }
          });
      start = end;
    }

    for (auto& th : threads) {
      th.join();
    }

    size_t cur_wins = 0, cur_draws = 0;
    for (const size_t win : wins) {
      cur_wins += win;
    }
    for (const size_t draw : draws) {
      cur_draws += draw;
    }

    if (cur_wins > best_move_wins ||
        (cur_wins == best_move_wins && cur_draws > best_move_draws)) {
      best_move = move;
      best_move_wins = cur_wins;
      best_move_draws = cur_draws;
    }
  }

  return best_move;
}

}  // namespace xq::internal::agent
