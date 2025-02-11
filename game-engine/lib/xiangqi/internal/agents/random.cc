#include "xiangqi/internal/agents/random.h"

#include "xiangqi/types.h"

namespace xq::internal::agent {

namespace {

using ::xq::Position;

}  // namespace

Position Random::MakeMove(const Board<Piece>& board, Player player) const {
  return xq::kNoPosition;  // TODO: implementation
}

}  // namespace xq::internal::agent
