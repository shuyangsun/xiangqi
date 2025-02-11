#include "xiangqi/internal/agents/random.h"

#include <random>

#include "xiangqi/board.h"
#include "xiangqi/types.h"

namespace xq::internal::agent {

namespace {

using ::xq::Position;

}  // namespace

uint16_t Random::MakeMove(const Board<Piece>& board, Player player) const {
  const std::vector<uint16_t> possible_moves =
      AllPossibleNextMoves(board, player);
  std::uniform_int_distribution<> distrib(
      0, static_cast<int>(possible_moves.size()) - 1);
  std::random_device rd;
  std::mt19937 rand_gen(rd());
  int rand_idx = distrib(rand_gen);
  return possible_moves[rand_idx];
}

}  // namespace xq::internal::agent
