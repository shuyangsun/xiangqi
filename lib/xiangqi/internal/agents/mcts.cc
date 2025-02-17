#include "xiangqi/internal/agents/mcts.h"

#include <array>
#include <optional>
#include <random>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "xiangqi/board.h"
#include "xiangqi/internal/agents/util.h"
#include "xiangqi/types.h"

namespace xq::internal::agent {

namespace {

using BoardState = std::array<uint64_t, 4>;

constexpr size_t kDefaultNumSimulations = 1000;

Winner MakeRandomMoveUntilGameOver(const Board& board, Player player) {
  Board cur_board = board;
  Player cur_player = player;

  std::random_device rd;
  std::mt19937 rand_gen(util::GetRNG());

  while (!IsGameOver(cur_board)) {
    const std::vector<uint16_t> possible_moves =
        PossibleMoves(cur_board, cur_player);
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
      : board_{},
        player_{Player::BLACK},
        winner_{Winner::NONE},
        produced_by_move_{0xFFFF},
        parent_{std::shared_ptr<Node>(nullptr)},
        children_{},
        untried_moves_{},
        wins_{0.0f},
        visits_{0} {};

  Node(const Board& board, Player player, std::shared_ptr<Node> parent,
       uint16_t produced_by_move)
      : board_{board},
        player_{player},
        winner_{GetWinner(board)},
        produced_by_move_{produced_by_move},
        parent_{std::move(parent)},
        children_{},
        wins_{0.0f},
        visits_{0} {
    if (winner_ == Winner::NONE) {
      untried_moves_ = PossibleMoves(board, player);
      children_.reserve(untried_moves_.size());
    }
  }
  ~Node() = default;

  inline const Board& GetBoard() const { return board_; }
  inline Player GetPlayer() const { return player_; }
  inline std::weak_ptr<Node> Parent() const { return parent_; }
  inline bool HasUntriedMoves() const { return !untried_moves_.empty(); }
  inline size_t Wins() const { return wins_; }
  inline size_t Visits() const { return visits_; }
  uint16_t ProducedByMove() const { return produced_by_move_; }
  inline std::vector<std::shared_ptr<Node>> Children() const {
    return children_;
  }

  inline void Reward(float reward) { wins_ += reward; }
  inline void RecordVisit() { visits_++; }

  inline void AddChild(std::shared_ptr<Node> child) {
    children_.emplace_back(child);
  }

  uint16_t ChooseRandomUntriedMove() {
    std::mt19937& rng = util::GetRNG();
    std::uniform_int_distribution<size_t> dist(0, untried_moves_.size() - 1);
    size_t idx = dist(rng);
    uint16_t result = untried_moves_[idx];
    untried_moves_.erase(untried_moves_.begin() + idx);
    return result;
  }

 private:
  Board board_;
  Player player_;
  Winner winner_;
  uint16_t produced_by_move_;
  std::weak_ptr<Node> parent_;
  std::vector<std::shared_ptr<Node>> children_;
  std::vector<uint16_t> untried_moves_;

  float wins_;
  size_t visits_;
};

std::shared_ptr<Node> TreePolicy(std::shared_ptr<Node> node,
                                 float exploration_constant) {
  while (!IsGameOver(node->GetBoard())) {
    if (node->HasUntriedMoves()) {
      // -- Expansion --
      const uint16_t move = node->ChooseRandomUntriedMove();
      const Position from = move >> 8, to = move & 0xFF;
      Board next = node->GetBoard();
      Move(next, from, to);
      const Player next_player = ChangePlayer(node->GetPlayer());
      auto child = std::make_shared<Node>(next, next_player, node, move);
      node->AddChild(child);
      return child;
    } else {
      // -- Selection: choose the best UCT child.
      float best_uct = -std::numeric_limits<float>::infinity();
      std::shared_ptr<Node> best_child = nullptr;
      for (std::shared_ptr<Node> child : node->Children()) {
        // UCT = (child wins / child visits) + kUCTConstant * sqrt( log(parent
        // visits) / child visits )
        float winRate = child->Wins() == 0
                            ? 0
                            : double(child->Wins()) / double(child->Visits());
        float uct = winRate + exploration_constant *
                                  std::sqrt(std::log(node->Visits() + 1) /
                                            (child->Visits() + 1e-4));
        if (uct > best_uct) {
          best_uct = uct;
          best_child = child;
        }
      }
      if (best_child == nullptr) {
        // Should not happen, but if no child is available return the current
        // node.
        return node;
      }
      node = best_child;
    }
  }
  return node;
}

Winner DefaultPolicy(const Board& board, Player player) {
  // Simulate a random playout from the current board.
  constexpr size_t kMaxPlayoutSteps = 10000;
  size_t steps = 0;
  Board next = board;
  while (!IsGameOver(next) && steps < kMaxPlayoutSteps) {
    std::vector<uint16_t> moves = PossibleMoves(next, player);
    if (moves.empty()) {
      break;
    }
    std::mt19937& rng = util::GetRNG();
    std::uniform_int_distribution<size_t> dist(0, moves.size() - 1);
    const uint16_t move = moves[dist(rng)];
    const Position from = move >> 8, to = move & 0xFF;
    Move(next, from, to);
    player = ChangePlayer(player);
    steps++;
  }
  return GetWinner(next);
}

void Backup(std::shared_ptr<Node> node, Winner winner) {
  // Propagate the simulation result back up the tree.
  // We use the “playerJustMoved” stored in each node to determine if the move
  // leading to that node was a win. (For draws we add 0.5.)
  while (node != nullptr) {
    node->RecordVisit();
    float reward = 0.0f;
    if (winner == Winner::DRAW || winner == Winner::NONE) {
      reward = 0.5f;
    } else {
      // Convert the Winner to a Player.
      const Player winner_player =
          (winner == Winner::RED) ? Player::RED : Player::BLACK;
      // Node player is not the one played this move, but the current player,
      // so we reward if winning player is NOT the same as current player.
      reward = (node->GetPlayer() == winner_player) ? 0.0f : 1.0f;
    }
    node->Reward(reward);
    node = node->Parent().lock();
  }
}

}  // namespace

MCTS::MCTS(size_t num_iter, size_t depth, float exploration_constant)
    : num_iter_{num_iter},
      depth_{depth},
      exploration_constant_{exploration_constant} {}

uint16_t MCTS::MakeMove(const Board& board, Player player) const {
  auto root = std::make_shared<Node>(board, player,
                                     std::shared_ptr<Node>(nullptr), 0xFFFF);
  for (size_t i = 0; i < num_iter_; i++) {
    // Selection and expansion.
    std::shared_ptr<Node> node = TreePolicy(root, exploration_constant_);
    // Simulation
    const Winner winner =
        DefaultPolicy(node->GetBoard(), ChangePlayer(node->GetPlayer()));
    Backup(node, winner);
  }

  // Select the move that was explored the most (or with the best win rate).
  std::shared_ptr<Node> best_child = nullptr;
  size_t best_visits = 0;
  for (std::shared_ptr<Node> child : root->Children()) {
    if (child->Visits() > best_visits) {
      best_visits = child->Visits();
      best_child = child;
    }
  }

  if (best_child == nullptr) {
    // No moves were found (should not happen in a valid position).
    return 0xFFFF;
  }
  return best_child->ProducedByMove();
}

}  // namespace xq::internal::agent
