#include "xiangqi/internal/agents/random.h"

#include <random>

#include "xiangqi/board.h"
#include "xiangqi/internal/agents/util.h"
#include "xiangqi/types.h"

namespace xq::internal::agent {

uint16_t Random::MakeMove(const Board<Piece>& board, Player player) const {
  const std::vector<uint16_t> possible_moves =
      AllPossibleNextMoves(board, player);
  std::uniform_int_distribution<> distrib(
      0, static_cast<int>(possible_moves.size()) - 1);
  int rand_idx = distrib(util::GetRNG());
  return possible_moves[rand_idx];
}

}  // namespace xq::internal::agent
