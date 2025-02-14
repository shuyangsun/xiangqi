// file: test_possible_moves.cc

#include <gtest/gtest.h>

#include <algorithm>
#include <string_view>
#include <vector>

#include "xiangqi/board.h"
#include "xiangqi/game.h"
#include "xiangqi/types.h"

using namespace xq;

namespace {

namespace {

using namespace ::xq;

std::vector<Position> ToVec(const MovesPerPiece& moves) {
  std::vector<Position> result;
  for (const Position move : moves) {
    if (move == kNoPosition) {
      break;
    }
    result.emplace_back(move);
  }
  std::sort(result.begin(), result.end());
  return result;
}

std::vector<Position> ToPos(const std::vector<std::string_view>& positions) {
  std::vector<Position> result;
  result.reserve(positions.size());
  for (const std::string_view& pos : positions) {
    result.emplace_back(PosStr(pos));
  }
  std::sort(result.begin(), result.end());
  return result;
}

}  // namespace

// ---------------------------------------------------------------------
// Test General
// ---------------------------------------------------------------------

TEST(TestPossibleMoves, General) {
  const MovesPerPiece moves_1 = PossibleMoves(kStartingBoard, PosStr("E0"));
  EXPECT_EQ(ToVec(moves_1), ToPos({"E1"}));
}

}  // namespace
