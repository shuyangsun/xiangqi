#include "xiangqi/internal/agents/mcts.h"

#include "xiangqi/types.h"

namespace xq::internal::agent {

namespace {

using ::xq::Position;

}  // namespace

uint16_t MCTS::MakeMove(const Board<Piece>& board, Player player) const {
  return xq::kNoPosition;  // TODO: implementation
}

}  // namespace xq::internal::agent
