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

  const MovesPerPiece moves_2 = PossibleMoves(kStartingBoard, PosStr("E9"));
  EXPECT_EQ(ToVec(moves_2), ToPos({"E8"}));

  const Board<Piece> board_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . a * g . . . \n"
      "1 . . . * H * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * G * . . . \n");

  const MovesPerPiece moves_3 = PossibleMoves(board_1, PosStr("F0"));
  EXPECT_EQ(ToVec(moves_3), ToPos({"F1", "E0"}));

  const MovesPerPiece moves_4 = PossibleMoves(board_1, PosStr("E9"));
  EXPECT_EQ(ToVec(moves_4), ToPos({"E8", "D9", "F9"}));

  const MovesPerPiece moves_5 = PossibleMoves(board_1, PosStr("E9"), true);
  EXPECT_EQ(ToVec(moves_5), ToPos({"E8", "D9"}));
}

}  // namespace
