#include "xiangqi/internal/agents/mcts.h"

#include <array>
#include <optional>
#include <random>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "xiangqi/board.h"
#include "xiangqi/types.h"

namespace xq::internal::agent {

namespace {

using BoardState = std::array<uint64_t, 4>;

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

template <typename T>
class StateLookup {
 public:
  StateLookup() = default;
  ~StateLookup() = default;

  bool Contains(const BoardState& state) const {
    auto it1 = cache_.find(state[0]);
    if (it1 == cache_.end()) {
      return false;
    }
    auto it2 = it1->find(state[1]);
    if (it2 == it1->end()) {
      return false;
    }
    auto it3 = it2->find(state[2]);
    if (it3 == it2->end()) {
      return false;
    }
    auto it4 = it3->find(state[3]);
    if (it4 == it3->end()) {
      return false;
    }
    return true;
  }

  const T& Get(const BoardState& state) const {
    return cache_[state[0]][state[1]][state[2]][state[3]];
  }

  void Set(const BoardState& state, T&& value) {
    if (!cache_.contains(state[0])) {
      cache_[state[0]] = std::unordered_map<
          uint64_t,
          std::unordered_map<uint64_t, std::unordered_map<uint64_t, T>>>{};
    }
    std::unordered_map<
        uint64_t,
        std::unordered_map<uint64_t, std::unordered_map<uint64_t, T>>>&
        cache_1 = cache_[state[0]];
    if (!cache_1.contains(state[1])) {
      cache_1[state[1]] =
          std::unordered_map<uint64_t, std::unordered_map<uint64_t, T>>{};
    }
    std::unordered_map<uint64_t, std::unordered_map<uint64_t, T>> cache_2 =
        cache_1[state[1]];
    if (!cache_2.contains(state[2])) {
      cache_2[state[2]] = std::unordered_map<uint64_t, T>{};
    }
    cache_2[state[2]][state[3]] = std::move(value);
  }

 private:
  std::unordered_map<
      uint64_t,
      std::unordered_map<
          uint64_t,
          std::unordered_map<uint64_t, std::unordered_map<uint64_t, T>>>>
      cache_{};
};

class Node {
 public:
  Node()
      : player_{Player::BLACK},
        is_leaf_{true},
        parent_{std::nullopt},
        children_{} {};

  Node(const Board<Piece>& board, Player player,
       std::optional<BoardState> parent)
      : player_{player},
        is_leaf_{IsGameOver(board)},
        parent_{parent},
        val_{0},
        vis_{0},
        uct_{0.0f} {
    if (!is_leaf_) {
      const std::vector<uint16_t> possible_moves =
          AllPossibleNextMoves(board, player);
      children_.reserve(possible_moves.size());
      for (const uint64_t move : possible_moves) {
        Board<Piece> next = board;
        Move(next, static_cast<uint8_t>((move & 0xFF00) >> 8),
             static_cast<uint8_t>(move & 0x00FF));
        children_.emplace_back(EncodeBoardState(next));
      }
    }
  }
  ~Node() = default;

 private:
  Player player_;
  bool is_leaf_;
  std::optional<BoardState> parent_;
  std::vector<BoardState> children_;

  int val_;
  size_t vis_;
  float uct_;
};

}  // namespace

MCTS::MCTS(size_t num_iter, size_t depth, float exploration_constant)
    : num_iter_{num_iter},
      depth_{depth},
      exploration_constant_{exploration_constant} {}

uint16_t MCTS::MakeMove(const Board<Piece>& board, Player player) const {
  StateLookup<Node> node_map{};
  node_map.Set(EncodeBoardState(board), Node{board, player, std::nullopt});
  // TODO: implementation

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
