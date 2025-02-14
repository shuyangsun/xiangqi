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

  const MovesPerPiece moves_4 =
      PossibleMoves(board_1, PosStr("F0"), /*prevent_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_4), ToPos({"F1", "E0"}));

  const MovesPerPiece moves_5 = PossibleMoves(board_1, PosStr("E9"));
  EXPECT_EQ(ToVec(moves_5), ToPos({"E8", "D9", "F9"}));

  const MovesPerPiece moves_6 =
      PossibleMoves(board_1, PosStr("E9"), /*prevent_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_6), ToPos({"E8", "D9"}));

  const Board<Piece> board_2 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * g * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - s - - - - \n"
      "5 - - - - S - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * G * . . . \n"
      "9 . . . * * * . . . \n");

  const MovesPerPiece moves_7 = PossibleMoves(board_2, PosStr("E1"));
  EXPECT_EQ(ToVec(moves_7), ToPos({"E0", "E2", "D1", "F1"}));

  const MovesPerPiece moves_8 =
      PossibleMoves(board_2, PosStr("E1"), /*prevent_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_8), ToPos({"E0", "E2", "D1", "F1"}));

  const MovesPerPiece moves_9 = PossibleMoves(board_2, PosStr("E8"));
  EXPECT_EQ(ToVec(moves_9), ToPos({"E7", "E9", "D8", "F8"}));

  const MovesPerPiece moves_10 =
      PossibleMoves(board_2, PosStr("E8"), /*prevent_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_10), ToPos({"E7", "E9", "D8", "F8"}));

  const Board<Piece> board_3 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * g * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . H . . \n"
      "4 - - - - s - - - - \n"
      "5 - - - - S - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * G * . . . \n"
      "9 . . . * * * . . . \n");

  const MovesPerPiece moves_11 = PossibleMoves(board_3, PosStr("E1"));
  EXPECT_EQ(ToVec(moves_11), ToPos({"E0", "E2", "D1", "F1"}));

  const MovesPerPiece moves_12 =
      PossibleMoves(board_3, PosStr("E1"), /*prevent_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_12), ToPos({"E0", "D1"}));

  const Board<Piece> board_4 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * g * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - s - - - - \n"
      "5 - - - - S - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * G * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * * * . . . \n");

  const MovesPerPiece moves_13 = PossibleMoves(board_4, PosStr("E2"));
  EXPECT_EQ(ToVec(moves_13), ToPos({"E1", "D2", "F2"}));

  const MovesPerPiece moves_14 =
      PossibleMoves(board_4, PosStr("E2"), /*prevent_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_14), ToPos({"E1", "D2", "F2"}));

  const MovesPerPiece moves_15 = PossibleMoves(board_4, PosStr("E7"));
  EXPECT_EQ(ToVec(moves_15), ToPos({"E8", "D7", "F7"}));

  const MovesPerPiece moves_16 =
      PossibleMoves(board_4, PosStr("E7"), /*prevent_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_16), ToPos({"E8", "D7", "F7"}));

  const Board<Piece> board_5 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . g * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - S - - - - - \n"
      "5 - - - s - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . G * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * * * . . . \n");

  const MovesPerPiece moves_17 = PossibleMoves(board_5, PosStr("D2"));
  EXPECT_EQ(ToVec(moves_17), ToPos({"E2", "D1"}));

  const MovesPerPiece moves_18 =
      PossibleMoves(board_5, PosStr("D2"), /*prevent_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_18), ToPos({"E2", "D1"}));

  const MovesPerPiece moves_19 = PossibleMoves(board_5, PosStr("D7"));
  EXPECT_EQ(ToVec(moves_19), ToPos({"E7", "D8"}));

  const MovesPerPiece moves_20 =
      PossibleMoves(board_5, PosStr("D7"), /*prevent_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_20), ToPos({"E7", "D8"}));

  const Board<Piece> board_6 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * g . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - S - - - \n"
      "5 - - - - - s - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * G . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * * * . . . \n");

  const MovesPerPiece moves_21 = PossibleMoves(board_6, PosStr("F2"));
  EXPECT_EQ(ToVec(moves_21), ToPos({"E2", "F1"}));

  const MovesPerPiece moves_22 =
      PossibleMoves(board_6, PosStr("F2"), /*prevent_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_22), ToPos({"E2", "F1"}));

  const MovesPerPiece moves_23 = PossibleMoves(board_6, PosStr("F7"));
  EXPECT_EQ(ToVec(moves_23), ToPos({"E7", "F8"}));

  const MovesPerPiece moves_24 =
      PossibleMoves(board_6, PosStr("F7"), /*prevent_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_24), ToPos({"E7", "F8"}));

  const Board<Piece> board_7 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * * g . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - S - - - \n"
      "5 - - - - - s - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * G . . . \n"
      "9 . . . * * * . . . \n");

  const MovesPerPiece moves_25 = PossibleMoves(board_7, PosStr("F1"));
  EXPECT_EQ(ToVec(moves_25), ToPos({"F0", "F2", "E1"}));

  const MovesPerPiece moves_26 =
      PossibleMoves(board_7, PosStr("F1"), /*prevent_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_26), ToPos({"F0", "F2", "E1"}));

  const MovesPerPiece moves_27 = PossibleMoves(board_7, PosStr("F8"));
  EXPECT_EQ(ToVec(moves_27), ToPos({"F7", "F9", "E8"}));

  const MovesPerPiece moves_28 =
      PossibleMoves(board_7, PosStr("F8"), /*prevent_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_28), ToPos({"F7", "F9", "E8"}));

  const Board<Piece> board_8 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . g * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - S - - - - - \n"
      "5 - - - s - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . G * * . . . \n"
      "9 . . . * * * . . . \n");

  const MovesPerPiece moves_29 = PossibleMoves(board_8, PosStr("D1"));
  EXPECT_EQ(ToVec(moves_29), ToPos({"D0", "D2", "E1"}));

  const MovesPerPiece moves_30 =
      PossibleMoves(board_8, PosStr("D1"), /*prevent_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_30), ToPos({"D0", "D2", "E1"}));

  const MovesPerPiece moves_31 = PossibleMoves(board_8, PosStr("D8"));
  EXPECT_EQ(ToVec(moves_31), ToPos({"D7", "D9", "E8"}));

  const MovesPerPiece moves_32 =
      PossibleMoves(board_8, PosStr("D8"), /*prevent_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_32), ToPos({"D7", "D9", "E8"}));

  const Board<Piece> board_9 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . g * * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - S - - - - - \n"
      "5 - - - s - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . G * * . . . \n");

  const MovesPerPiece moves_33 = PossibleMoves(board_9, PosStr("D0"));
  EXPECT_EQ(ToVec(moves_33), ToPos({"D1", "E0"}));

  const MovesPerPiece moves_34 =
      PossibleMoves(board_9, PosStr("D0"), /*prevent_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_34), ToPos({"D1", "E0"}));

  const MovesPerPiece moves_35 = PossibleMoves(board_9, PosStr("D9"));
  EXPECT_EQ(ToVec(moves_35), ToPos({"D8", "E9"}));

  const MovesPerPiece moves_36 =
      PossibleMoves(board_9, PosStr("D9"), /*prevent_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_36), ToPos({"D8", "E9"}));

  const Board<Piece> board_10 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * g . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - S - - - \n"
      "5 - - - - - s - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * * G . . . \n");

  const MovesPerPiece moves_37 = PossibleMoves(board_10, PosStr("F0"));
  EXPECT_EQ(ToVec(moves_37), ToPos({"F1", "E0"}));

  const MovesPerPiece moves_38 =
      PossibleMoves(board_10, PosStr("F0"), /*prevent_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_38), ToPos({"F1", "E0"}));

  const MovesPerPiece moves_39 = PossibleMoves(board_10, PosStr("F9"));
  EXPECT_EQ(ToVec(moves_39), ToPos({"F8", "E9"}));

  const MovesPerPiece moves_40 =
      PossibleMoves(board_10, PosStr("F9"), /*prevent_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_40), ToPos({"F8", "E9"}));

  const Board<Piece> board_11 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . a * g . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * A * . . . \n"
      "9 . . . A G h . . . \n");

  const MovesPerPiece moves_41 = PossibleMoves(board_11, PosStr("E9"));
  EXPECT_EQ(ToVec(moves_41), ToPos({"F9"}));

  const MovesPerPiece moves_42 =
      PossibleMoves(board_11, PosStr("E9"), /*prevent_checkmate=*/true);
  EXPECT_TRUE(ToVec(moves_42).empty());
}

// ---------------------------------------------------------------------
// Test Advisor
// ---------------------------------------------------------------------

TEST(TestPossibleMoves, Advisor) {
  const MovesPerPiece moves_1 = PossibleMoves(kStartingBoard, PosStr("D0"));
  EXPECT_EQ(ToVec(moves_1), ToPos({"E1"}));

  const MovesPerPiece moves_2 = PossibleMoves(kStartingBoard, PosStr("F0"));
  EXPECT_EQ(ToVec(moves_2), ToPos({"E1"}));

  const MovesPerPiece moves_3 = PossibleMoves(kStartingBoard, PosStr("D9"));
  EXPECT_EQ(ToVec(moves_3), ToPos({"E8"}));

  const MovesPerPiece moves_4 = PossibleMoves(kStartingBoard, PosStr("F9"));
  EXPECT_EQ(ToVec(moves_4), ToPos({"E8"}));

  const Board<Piece> board_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . a * g . . . \n"
      "1 . . . * a * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * G * . . . \n"
      "8 . . . * A * . . . \n"
      "9 . . . A * * . . . \n");

  const MovesPerPiece moves_5 = PossibleMoves(board_1, PosStr("D0"));
  EXPECT_TRUE(ToVec(moves_5).empty());

  const MovesPerPiece moves_6 = PossibleMoves(board_1, PosStr("E1"));
  EXPECT_EQ(ToVec(moves_6), ToPos({"D2", "F2"}));

  const MovesPerPiece moves_7 = PossibleMoves(board_1, PosStr("E8"));
  EXPECT_EQ(ToVec(moves_7), ToPos({"F9", "D7", "F7"}));

  const MovesPerPiece moves_8 = PossibleMoves(board_1, PosStr("D9"));
  EXPECT_TRUE(ToVec(moves_8).empty());
}

}  // namespace
