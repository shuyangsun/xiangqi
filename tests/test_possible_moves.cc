// file: test_possible_moves.cc

#include <gmock/gmock.h>
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

using ::testing::IsSupersetOf;

std::vector<Position> ToVec(const MovesPerPiece& moves) {
  std::vector<Position> result;
  for (const Position move : moves) {
    if (move == K_NO_POSITION) {
      break;
    }
    result.emplace_back(move);
  }
  std::sort(result.begin(), result.end());
  return result;
}

std::vector<Movement> ToVec(const std::vector<Movement>& moves) {
  std::vector<Movement> result{moves.begin(), moves.end()};
  std::sort(result.begin(), result.end());
  return result;
}

std::vector<Position> ToPos(const std::vector<std::string_view>& positions) {
  std::vector<Position> result;
  result.reserve(positions.size());
  for (const std::string_view& pos : positions) {
    result.emplace_back(PosStr(pos.data()));
  }
  std::sort(result.begin(), result.end());
  return result;
}

std::vector<Movement> ToMoves(const std::vector<std::string_view>& moves) {
  std::vector<Movement> result;
  result.reserve(moves.size());
  for (const std::string_view& move : moves) {
    const Position from = PosStr(move.data());
    const Position to = PosStr(move.data() + 3);
    result.emplace_back(NewMovement(from, to));
  }
  std::sort(result.begin(), result.end());
  return result;
}

}  // namespace

// ---------------------------------------------------------------------
// Test General
// ---------------------------------------------------------------------

TEST(PossiblePositions, General) {
  const MovesPerPiece moves_1 = PossiblePositions(kStartingBoard, PosStr("E0"));
  EXPECT_EQ(ToVec(moves_1), ToPos({"E1"}));

  const MovesPerPiece moves_2 = PossiblePositions(kStartingBoard, PosStr("E9"));
  EXPECT_EQ(ToVec(moves_2), ToPos({"E8"}));

  const Board board_1 = BoardFromString(
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

  const MovesPerPiece moves_3 = PossiblePositions(board_1, PosStr("F0"));
  EXPECT_EQ(ToVec(moves_3), ToPos({"F1", "E0"}));

  const MovesPerPiece moves_4 =
      PossiblePositions(board_1, PosStr("F0"), /*avoid_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_4), ToPos({"F1", "E0"}));

  const MovesPerPiece moves_5 = PossiblePositions(board_1, PosStr("E9"));
  EXPECT_EQ(ToVec(moves_5), ToPos({"E8", "D9", "F9"}));

  const MovesPerPiece moves_6 =
      PossiblePositions(board_1, PosStr("E9"), /*avoid_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_6), ToPos({"E8", "D9"}));

  const Board board_2 = BoardFromString(
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

  const MovesPerPiece moves_7 = PossiblePositions(board_2, PosStr("E1"));
  EXPECT_EQ(ToVec(moves_7), ToPos({"E0", "E2", "D1", "F1"}));

  const MovesPerPiece moves_8 =
      PossiblePositions(board_2, PosStr("E1"), /*avoid_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_8), ToPos({"E0", "E2", "D1", "F1"}));

  const MovesPerPiece moves_9 = PossiblePositions(board_2, PosStr("E8"));
  EXPECT_EQ(ToVec(moves_9), ToPos({"E7", "E9", "D8", "F8"}));

  const MovesPerPiece moves_10 =
      PossiblePositions(board_2, PosStr("E8"), /*avoid_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_10), ToPos({"E7", "E9", "D8", "F8"}));

  const Board board_3 = BoardFromString(
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

  const MovesPerPiece moves_11 = PossiblePositions(board_3, PosStr("E1"));
  EXPECT_EQ(ToVec(moves_11), ToPos({"E0", "E2", "D1", "F1"}));

  const MovesPerPiece moves_12 =
      PossiblePositions(board_3, PosStr("E1"), /*avoid_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_12), ToPos({"E0", "D1"}));

  const Board board_4 = BoardFromString(
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

  const MovesPerPiece moves_13 = PossiblePositions(board_4, PosStr("E2"));
  EXPECT_EQ(ToVec(moves_13), ToPos({"E1", "D2", "F2"}));

  const MovesPerPiece moves_14 =
      PossiblePositions(board_4, PosStr("E2"), /*avoid_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_14), ToPos({"E1", "D2", "F2"}));

  const MovesPerPiece moves_15 = PossiblePositions(board_4, PosStr("E7"));
  EXPECT_EQ(ToVec(moves_15), ToPos({"E8", "D7", "F7"}));

  const MovesPerPiece moves_16 =
      PossiblePositions(board_4, PosStr("E7"), /*avoid_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_16), ToPos({"E8", "D7", "F7"}));

  const Board board_5 = BoardFromString(
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

  const MovesPerPiece moves_17 = PossiblePositions(board_5, PosStr("D2"));
  EXPECT_EQ(ToVec(moves_17), ToPos({"E2", "D1"}));

  const MovesPerPiece moves_18 =
      PossiblePositions(board_5, PosStr("D2"), /*avoid_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_18), ToPos({"E2", "D1"}));

  const MovesPerPiece moves_19 = PossiblePositions(board_5, PosStr("D7"));
  EXPECT_EQ(ToVec(moves_19), ToPos({"E7", "D8"}));

  const MovesPerPiece moves_20 =
      PossiblePositions(board_5, PosStr("D7"), /*avoid_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_20), ToPos({"E7", "D8"}));

  const Board board_6 = BoardFromString(
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

  const MovesPerPiece moves_21 = PossiblePositions(board_6, PosStr("F2"));
  EXPECT_EQ(ToVec(moves_21), ToPos({"E2", "F1"}));

  const MovesPerPiece moves_22 =
      PossiblePositions(board_6, PosStr("F2"), /*avoid_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_22), ToPos({"E2", "F1"}));

  const MovesPerPiece moves_23 = PossiblePositions(board_6, PosStr("F7"));
  EXPECT_EQ(ToVec(moves_23), ToPos({"E7", "F8"}));

  const MovesPerPiece moves_24 =
      PossiblePositions(board_6, PosStr("F7"), /*avoid_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_24), ToPos({"E7", "F8"}));

  const Board board_7 = BoardFromString(
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

  const MovesPerPiece moves_25 = PossiblePositions(board_7, PosStr("F1"));
  EXPECT_EQ(ToVec(moves_25), ToPos({"F0", "F2", "E1"}));

  const MovesPerPiece moves_26 =
      PossiblePositions(board_7, PosStr("F1"), /*avoid_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_26), ToPos({"F0", "F2", "E1"}));

  const MovesPerPiece moves_27 = PossiblePositions(board_7, PosStr("F8"));
  EXPECT_EQ(ToVec(moves_27), ToPos({"F7", "F9", "E8"}));

  const MovesPerPiece moves_28 =
      PossiblePositions(board_7, PosStr("F8"), /*avoid_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_28), ToPos({"F7", "F9", "E8"}));

  const Board board_8 = BoardFromString(
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

  const MovesPerPiece moves_29 = PossiblePositions(board_8, PosStr("D1"));
  EXPECT_EQ(ToVec(moves_29), ToPos({"D0", "D2", "E1"}));

  const MovesPerPiece moves_30 =
      PossiblePositions(board_8, PosStr("D1"), /*avoid_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_30), ToPos({"D0", "D2", "E1"}));

  const MovesPerPiece moves_31 = PossiblePositions(board_8, PosStr("D8"));
  EXPECT_EQ(ToVec(moves_31), ToPos({"D7", "D9", "E8"}));

  const MovesPerPiece moves_32 =
      PossiblePositions(board_8, PosStr("D8"), /*avoid_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_32), ToPos({"D7", "D9", "E8"}));

  const Board board_9 = BoardFromString(
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

  const MovesPerPiece moves_33 = PossiblePositions(board_9, PosStr("D0"));
  EXPECT_EQ(ToVec(moves_33), ToPos({"D1", "E0"}));

  const MovesPerPiece moves_34 =
      PossiblePositions(board_9, PosStr("D0"), /*avoid_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_34), ToPos({"D1", "E0"}));

  const MovesPerPiece moves_35 = PossiblePositions(board_9, PosStr("D9"));
  EXPECT_EQ(ToVec(moves_35), ToPos({"D8", "E9"}));

  const MovesPerPiece moves_36 =
      PossiblePositions(board_9, PosStr("D9"), /*avoid_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_36), ToPos({"D8", "E9"}));

  const Board board_10 = BoardFromString(
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

  const MovesPerPiece moves_37 = PossiblePositions(board_10, PosStr("F0"));
  EXPECT_EQ(ToVec(moves_37), ToPos({"F1", "E0"}));

  const MovesPerPiece moves_38 =
      PossiblePositions(board_10, PosStr("F0"), /*avoid_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_38), ToPos({"F1", "E0"}));

  const MovesPerPiece moves_39 = PossiblePositions(board_10, PosStr("F9"));
  EXPECT_EQ(ToVec(moves_39), ToPos({"F8", "E9"}));

  const MovesPerPiece moves_40 =
      PossiblePositions(board_10, PosStr("F9"), /*avoid_checkmate=*/true);
  EXPECT_EQ(ToVec(moves_40), ToPos({"F8", "E9"}));

  const Board board_11 = BoardFromString(
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

  const MovesPerPiece moves_41 = PossiblePositions(board_11, PosStr("E9"));
  EXPECT_EQ(ToVec(moves_41), ToPos({"F9"}));

  const MovesPerPiece moves_42 =
      PossiblePositions(board_11, PosStr("E9"), /*avoid_checkmate=*/true);
  EXPECT_TRUE(ToVec(moves_42).empty());
}

// ---------------------------------------------------------------------
// Test Advisor
// ---------------------------------------------------------------------

TEST(PossiblePositions, Advisor) {
  const MovesPerPiece moves_1 = PossiblePositions(kStartingBoard, PosStr("D0"));
  EXPECT_EQ(ToVec(moves_1), ToPos({"E1"}));

  const MovesPerPiece moves_2 = PossiblePositions(kStartingBoard, PosStr("F0"));
  EXPECT_EQ(ToVec(moves_2), ToPos({"E1"}));

  const MovesPerPiece moves_3 = PossiblePositions(kStartingBoard, PosStr("D9"));
  EXPECT_EQ(ToVec(moves_3), ToPos({"E8"}));

  const MovesPerPiece moves_4 = PossiblePositions(kStartingBoard, PosStr("F9"));
  EXPECT_EQ(ToVec(moves_4), ToPos({"E8"}));

  const Board board_1 = BoardFromString(
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

  const MovesPerPiece moves_5 = PossiblePositions(board_1, PosStr("D0"));
  EXPECT_TRUE(ToVec(moves_5).empty());

  const MovesPerPiece moves_6 = PossiblePositions(board_1, PosStr("E1"));
  EXPECT_EQ(ToVec(moves_6), ToPos({"D2", "F2"}));

  const MovesPerPiece moves_7 = PossiblePositions(board_1, PosStr("E8"));
  EXPECT_EQ(ToVec(moves_7), ToPos({"F9", "D7", "F7"}));

  const MovesPerPiece moves_8 = PossiblePositions(board_1, PosStr("D9"));
  EXPECT_TRUE(ToVec(moves_8).empty());

  const Board board_2 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . a * a . . . \n"
      "1 . . . * * g . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . G * * . . . \n"
      "9 . . . A * A . . . \n");

  const MovesPerPiece moves_9 = PossiblePositions(board_2, PosStr("D0"));
  EXPECT_EQ(ToVec(moves_9), ToPos({"E1"}));

  const MovesPerPiece moves_10 = PossiblePositions(board_2, PosStr("F0"));
  EXPECT_EQ(ToVec(moves_10), ToPos({"E1"}));

  const MovesPerPiece moves_11 = PossiblePositions(board_2, PosStr("D9"));
  EXPECT_EQ(ToVec(moves_11), ToPos({"E8"}));

  const MovesPerPiece moves_12 = PossiblePositions(board_2, PosStr("F9"));
  EXPECT_EQ(ToVec(moves_12), ToPos({"E8"}));

  const Board board_3 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * * g . . . \n"
      "2 . . . a * a . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . A * A . . . \n"
      "8 . . . G * * . . . \n"
      "9 . . . * * * . . . \n");

  const MovesPerPiece moves_13 = PossiblePositions(board_3, PosStr("D2"));
  EXPECT_EQ(ToVec(moves_13), ToPos({"E1"}));

  const MovesPerPiece moves_14 = PossiblePositions(board_3, PosStr("F2"));
  EXPECT_EQ(ToVec(moves_14), ToPos({"E1"}));

  const MovesPerPiece moves_15 = PossiblePositions(board_3, PosStr("D7"));
  EXPECT_EQ(ToVec(moves_15), ToPos({"E8"}));

  const MovesPerPiece moves_16 = PossiblePositions(board_3, PosStr("F7"));
  EXPECT_EQ(ToVec(moves_16), ToPos({"E8"}));

  const Board board_4 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * a g . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . G A * . . . \n"
      "9 . . . * * * . . . \n");

  const MovesPerPiece moves_17 = PossiblePositions(board_4, PosStr("E1"));
  EXPECT_EQ(ToVec(moves_17), ToPos({"D0", "F0", "D2", "F2"}));

  const MovesPerPiece moves_18 = PossiblePositions(board_4, PosStr("E8"));
  EXPECT_EQ(ToVec(moves_18), ToPos({"D9", "F9", "D7", "F7"}));

  const Board board_5 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * g * . . . \n"
      "1 . . . * a * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * G * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * * * . . . \n");

  const MovesPerPiece moves_19 =
      PossiblePositions(board_5, PosStr("E1"), /*avoid_checkmate=*/true);
  EXPECT_TRUE(ToVec(moves_19).empty());
}

// ---------------------------------------------------------------------
// Test Elephant
// ---------------------------------------------------------------------

TEST(PossiblePositions, Elephant) {
  const Board board_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . e a g * e . . \n"
      "1 . . . * a * . . . \n"
      "2 R . . * H * . . H \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 h . . * h * . . r \n"
      "8 . . . * A * . . . \n"
      "9 . . E * G A E . . \n");

  EXPECT_EQ(ToVec(PossiblePositions(board_1, PosStr("C0"))),
            ToPos({"A2", "E2"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_1, PosStr("G0"))),
            ToPos({"I2", "E2"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_1, PosStr("C9"))),
            ToPos({"A7", "E7"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_1, PosStr("G9"))),
            ToPos({"I7", "E7"}));

  const Board board_2 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . e a g * e . . \n"
      "1 . r . c a c . r . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . R . C A C . R . \n"
      "9 . . E * G A E . . \n");
  EXPECT_TRUE(ToVec(PossiblePositions(board_2, PosStr("C0"))).empty());
  EXPECT_TRUE(ToVec(PossiblePositions(board_2, PosStr("G0"))).empty());
  EXPECT_TRUE(ToVec(PossiblePositions(board_2, PosStr("C9"))).empty());
  EXPECT_TRUE(ToVec(PossiblePositions(board_2, PosStr("G9"))).empty());

  const Board board_3 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . a g * . . . \n"
      "1 . . . * a * . . . \n"
      "2 R . . * H * . . H \n"
      "3 . . . . . . . . . \n"
      "4 - - e - - - e - - \n"
      "5 - - E - - - E - - \n"
      "6 . . . . . . . . . \n"
      "7 h . . * h * . . r \n"
      "8 . . . * A * . . . \n"
      "9 . . . * G A . . . \n");

  EXPECT_EQ(ToVec(PossiblePositions(board_3, PosStr("C4"))),
            ToPos({"A2", "E2"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_3, PosStr("G4"))),
            ToPos({"I2", "E2"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_3, PosStr("C5"))),
            ToPos({"A7", "E7"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_3, PosStr("G5"))),
            ToPos({"I7", "E7"}));

  const Board board_4 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . a g * . . . \n"
      "1 . . . * a * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . r . c . c . r . \n"
      "4 - - e - - - e - - \n"
      "5 - - E - - - E - - \n"
      "6 . R . C . C . R . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * A * . . . \n"
      "9 . . . * G A . . . \n");
  EXPECT_TRUE(ToVec(PossiblePositions(board_4, PosStr("C4"))).empty());
  EXPECT_TRUE(ToVec(PossiblePositions(board_4, PosStr("G4"))).empty());
  EXPECT_TRUE(ToVec(PossiblePositions(board_4, PosStr("C5"))).empty());
  EXPECT_TRUE(ToVec(PossiblePositions(board_4, PosStr("G5"))).empty());

  const Board board_5 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . R a g * C . . \n"
      "1 . . . * a * . . . \n"
      "2 e . . * * * . . e \n"
      "3 . . . . . . . . . \n"
      "4 - - H - - - H - - \n"
      "5 - - h - - - h - - \n"
      "6 . . . . . . . . . \n"
      "7 E . . * h * . . E \n"
      "8 . . . * A * . . . \n"
      "9 . . c * G A r . . \n");

  EXPECT_EQ(ToVec(PossiblePositions(board_5, PosStr("A2"))),
            ToPos({"C0", "C4"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_5, PosStr("I2"))),
            ToPos({"G0", "G4"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_5, PosStr("A7"))),
            ToPos({"C5", "C9"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_5, PosStr("I7"))),
            ToPos({"G5", "G9"}));

  const Board board_6 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . a g * . . . \n"
      "1 . r . * a * . c . \n"
      "2 e . . * * * . . e \n"
      "3 . H . . . . . H . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . H . . . . . H . \n"
      "7 E . . * h * . . E \n"
      "8 . C . * A * . R . \n"
      "9 . . . * G A . . . \n");
  EXPECT_TRUE(ToVec(PossiblePositions(board_6, PosStr("A2"))).empty());
  EXPECT_TRUE(ToVec(PossiblePositions(board_6, PosStr("I2"))).empty());
  EXPECT_TRUE(ToVec(PossiblePositions(board_6, PosStr("A7"))).empty());
  EXPECT_TRUE(ToVec(PossiblePositions(board_6, PosStr("I7"))).empty());

  const Board board_7 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . R a g * C . . \n"
      "1 . . . * a * . . . \n"
      "2 . . . * e * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - H - - - H - - \n"
      "5 - - h - - - h - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * E * . . . \n"
      "8 . . . * A * . . . \n"
      "9 . . c * G A r . . \n");
  EXPECT_EQ(ToVec(PossiblePositions(board_7, PosStr("E2"))),
            ToPos({"C0", "G0", "C4", "G4"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_7, PosStr("E7"))),
            ToPos({"C9", "G9", "C5", "G5"}));

  const Board board_8 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . a g * . . . \n"
      "1 . . . H a H . . . \n"
      "2 . . . * e * . . . \n"
      "3 . . . r . r . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . R . R . . . \n"
      "7 . . . * E * . . . \n"
      "8 . . . h A h . . . \n"
      "9 . . . * G A . . . \n");
  EXPECT_TRUE(ToVec(PossiblePositions(board_8, PosStr("E2"))).empty());
  EXPECT_TRUE(ToVec(PossiblePositions(board_8, PosStr("E7"))).empty());
}

// ---------------------------------------------------------------------
// Test Horse
// ---------------------------------------------------------------------

TEST(PossiblePositions, Horse) {
  EXPECT_EQ(ToVec(PossiblePositions(kStartingBoard, PosStr("B9"))),
            ToPos({"A7", "C7"}));
  EXPECT_EQ(ToVec(PossiblePositions(kStartingBoard, PosStr("H9"))),
            ToPos({"G7", "I7"}));
  EXPECT_EQ(ToVec(PossiblePositions(kStartingBoard, PosStr("B0"))),
            ToPos({"A2", "C2"}));
  EXPECT_EQ(ToVec(PossiblePositions(kStartingBoard, PosStr("H0"))),
            ToPos({"G2", "I2"}));

  const Board board_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 h . . a g * . . . \n"
      "1 . . C * a * . . . \n"
      "2 . R . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - h - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * H . . \n"
      "8 . . . * A * . . c \n"
      "9 H . . * G A . . . \n");
  EXPECT_EQ(ToVec(PossiblePositions(board_1, PosStr("G7"))),
            ToPos({"E6", "F5", "H5", "I6", "I8", "H9"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_1, PosStr("A9"))),
            ToPos({"B7", "C8"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_1, PosStr("D4"))),
            ToPos({"B5", "B3", "C2", "E2", "F3", "F5", "E6", "C6"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_1, PosStr("A0"))),
            ToPos({"C1", "B2"}));

  const Board board_2 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . a g * . . h \n"
      "1 . . C * a * . . . \n"
      "2 h . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 H - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * A * . . . \n"
      "9 . . . * G A . . H \n");

  EXPECT_EQ(ToVec(PossiblePositions(board_2, PosStr("I0"))),
            ToPos({"G1", "H2"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_2, PosStr("A2"))),
            ToPos({"B0", "B4", "C1", "C3"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_2, PosStr("A5"))),
            ToPos({"B3", "B7", "C4", "C6"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_2, PosStr("I9"))),
            ToPos({"H7", "G8"}));

  const Board board_3 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * g * . . . \n"
      "1 . . . * a * . . . \n"
      "2 . . . a h S . . . \n"
      "3 . . . . S . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . S . . . . \n"
      "7 . . . S H A . . . \n"
      "8 . . . * A * . . . \n"
      "9 . . . * G * . . . \n");
  EXPECT_TRUE(ToVec(PossiblePositions(board_3, PosStr("E2"))).empty());
  EXPECT_TRUE(ToVec(PossiblePositions(board_3, PosStr("E7"))).empty());

  const Board board_4 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * g . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * R . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - h - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * A * . . . \n"
      "9 . . . * G * . . . \n");
  EXPECT_EQ(
      ToVec(PossiblePositions(board_4, PosStr("E4"), /*avoid_checkmate=*/true)),
      ToPos({"F2"}));
}

// ---------------------------------------------------------------------
// Test Chariot
// ---------------------------------------------------------------------

TEST(PossiblePositions, Chariot) {
  EXPECT_EQ(ToVec(PossiblePositions(kStartingBoard, PosStr("A0"))),
            ToPos({"A1", "A2"}));
  EXPECT_EQ(ToVec(PossiblePositions(kStartingBoard, PosStr("I0"))),
            ToPos({"I1", "I2"}));
  EXPECT_EQ(ToVec(PossiblePositions(kStartingBoard, PosStr("A9"))),
            ToPos({"A8", "A7"}));
  EXPECT_EQ(ToVec(PossiblePositions(kStartingBoard, PosStr("I9"))),
            ToPos({"I8", "I7"}));

  const Board board_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 R . . a g * . . . \n"
      "1 . . . * a * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - r - - \n"
      "5 - - R - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * A * . . . \n"
      "9 . . . * G A . . r \n");
  EXPECT_EQ(ToVec(PossiblePositions(board_1, PosStr("A0"))),
            ToPos({"A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "B0",
                   "C0", "D0"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_1, PosStr("C5"))),
            ToPos({"C0", "C1", "C2", "C3", "C4", "C6", "C7", "C8", "C9", "A5",
                   "B5", "D5", "E5", "F5", "G5", "H5", "I5"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_1, PosStr("I9"))),
            ToPos({"I0", "I1", "I2", "I3", "I4", "I5", "I6", "I7", "I8", "H9",
                   "G9", "F9"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_1, PosStr("G4"))),
            ToPos({"G0", "G1", "G2", "G3", "G5", "G6", "G7", "G8", "G9", "A4",
                   "B4", "C4", "D4", "E4", "F4", "H4", "I4"}));

  const Board board_2 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 R c . a g * . . . \n"
      "1 c . . * a * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . H . . \n"
      "4 - - s - - H r S - \n"
      "5 - s R h - - S - - \n"
      "6 . . h . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * A * . . C \n"
      "9 . . . * G A . C r \n");
  EXPECT_EQ(ToVec(PossiblePositions(board_2, PosStr("A0"))),
            ToPos({"A1", "B0"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_2, PosStr("I9"))),
            ToPos({"I8", "H9"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_2, PosStr("C5"))),
            ToPos({"C4", "C6", "B5", "D5"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_2, PosStr("G4"))),
            ToPos({"G3", "G5", "F4", "H4"}));

  const Board board_3 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 r C . a g * . . . \n"
      "1 C . . * a * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . H . . \n"
      "4 - - s - - H r S - \n"
      "5 - s R h - - S - - \n"
      "6 . . h . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * A * . . c \n"
      "9 . . . * G A . c R \n");
  EXPECT_EQ(
      ToVec(PossiblePositions(board_3, PosStr("A0"), /*avoid_checkmate=*/true)),
      ToPos({"B0"}));
  EXPECT_EQ(
      ToVec(PossiblePositions(board_3, PosStr("I9"), /*avoid_checkmate=*/true)),
      ToPos({"H9"}));
  EXPECT_TRUE(
      ToVec(PossiblePositions(board_3, PosStr("C5"), /*avoid_checkmate=*/true))
          .empty());
  EXPECT_TRUE(
      ToVec(PossiblePositions(board_3, PosStr("G4"), /*avoid_checkmate=*/true))
          .empty());
}

// ---------------------------------------------------------------------
// Test Cannon
// ---------------------------------------------------------------------

TEST(PossiblePositions, Cannon) {
  EXPECT_EQ(ToVec(PossiblePositions(kStartingBoard, PosStr("B2"))),
            ToPos({"B1", "B3", "B4", "B5", "B6", "B9", "A2", "C2", "D2", "E2",
                   "F2", "G2"}));
  EXPECT_EQ(ToVec(PossiblePositions(kStartingBoard, PosStr("H2"))),
            ToPos({"H1", "H3", "H4", "H5", "H6", "H9", "I2", "G2", "F2", "E2",
                   "D2", "C2"}));
  EXPECT_EQ(ToVec(PossiblePositions(kStartingBoard, PosStr("B7"))),
            ToPos({"B8", "B6", "B5", "B4", "B3", "B0", "A7", "C7", "D7", "E7",
                   "F7", "G7"}));
  EXPECT_EQ(ToVec(PossiblePositions(kStartingBoard, PosStr("H7"))),
            ToPos({"H8", "H6", "H5", "H4", "H3", "H0", "I7", "G7", "F7", "E7",
                   "D7", "C7"}));

  const Board board_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . a g * . . . \n"
      "1 . . . * a * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . S . . . \n"
      "4 - - - - S - - - - \n"
      "5 R - R H c c h - - \n"
      "6 . . . . h . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * A H . . . \n"
      "9 . . . * G A . . . \n");
  EXPECT_EQ(ToVec(PossiblePositions(board_1, PosStr("E5"))),
            ToPos({"C5", "E8"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_1, PosStr("F5"))),
            ToPos({"D5", "F4", "F6", "F7", "F9"}));
}

// ---------------------------------------------------------------------
// Test Soldier
// ---------------------------------------------------------------------

TEST(PossiblePositions, Soldier) {
  EXPECT_EQ(ToVec(PossiblePositions(kStartingBoard, PosStr("A3"))),
            ToPos({"A4"}));
  EXPECT_EQ(ToVec(PossiblePositions(kStartingBoard, PosStr("C3"))),
            ToPos({"C4"}));
  EXPECT_EQ(ToVec(PossiblePositions(kStartingBoard, PosStr("E3"))),
            ToPos({"E4"}));
  EXPECT_EQ(ToVec(PossiblePositions(kStartingBoard, PosStr("G3"))),
            ToPos({"G4"}));
  EXPECT_EQ(ToVec(PossiblePositions(kStartingBoard, PosStr("I3"))),
            ToPos({"I4"}));
  EXPECT_EQ(ToVec(PossiblePositions(kStartingBoard, PosStr("A6"))),
            ToPos({"A5"}));
  EXPECT_EQ(ToVec(PossiblePositions(kStartingBoard, PosStr("C6"))),
            ToPos({"C5"}));
  EXPECT_EQ(ToVec(PossiblePositions(kStartingBoard, PosStr("E6"))),
            ToPos({"E5"}));
  EXPECT_EQ(ToVec(PossiblePositions(kStartingBoard, PosStr("G6"))),
            ToPos({"G5"}));
  EXPECT_EQ(ToVec(PossiblePositions(kStartingBoard, PosStr("I6"))),
            ToPos({"I5"}));

  const Board board_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . a g * . . . \n"
      "1 . . . * a * . . . \n"
      "2 . H . * * * . . . \n"
      "3 R S R r S c h . . \n"
      "4 - - - - - - S - S \n"
      "5 S - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * A * . . . \n"
      "9 . . . G * A . . . \n");
  EXPECT_EQ(ToVec(PossiblePositions(board_1, PosStr("A5"))), ToPos({"A4"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_1, PosStr("I4"))),
            ToPos({"I3", "H4"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_1, PosStr("G4"))),
            ToPos({"G3", "F4", "H4"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_1, PosStr("E3"))),
            ToPos({"E2", "D3", "F3"}));
  EXPECT_EQ(
      ToVec(PossiblePositions(board_1, PosStr("E3"), /*avoid_checkmate=*/true)),
      ToPos({"D3"}));
  EXPECT_TRUE(ToVec(PossiblePositions(board_1, PosStr("B3"))).empty());

  const Board board_2 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . a * g . . . \n"
      "1 . . . * a * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - s \n"
      "5 s C s H - - - - - \n"
      "6 . . . C s R r s r \n"
      "7 . . . * * * . h . \n"
      "8 . . . * A * . . . \n"
      "9 . . . G * A . . . \n");
  EXPECT_EQ(ToVec(PossiblePositions(board_2, PosStr("I4"))), ToPos({"I5"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_2, PosStr("A5"))),
            ToPos({"A6", "B5"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_2, PosStr("C5"))),
            ToPos({"C6", "B5", "D5"}));
  EXPECT_EQ(ToVec(PossiblePositions(board_2, PosStr("E6"))),
            ToPos({"E7", "D6", "F6"}));
  EXPECT_EQ(
      ToVec(PossiblePositions(board_2, PosStr("E6"), /*avoid_checkmate=*/true)),
      ToPos({"F6"}));
  EXPECT_TRUE(ToVec(PossiblePositions(board_2, PosStr("H6"))).empty());
}

TEST(PossibleMoves, StartingBoard) {
  const std::vector<Movement> red_possible_moves =
      PossibleMoves(kStartingBoard, PLAYER_RED, false);
  const std::vector<Movement> black_possible_moves =
      PossibleMoves(kStartingBoard, PLAYER_BLACK, false);

  EXPECT_EQ(red_possible_moves.size(), 44);
  EXPECT_EQ(black_possible_moves.size(), 44);

  EXPECT_EQ(ToVec(red_possible_moves),
            ToMoves({// Red soldiers
                     "A6,A5", "C6,C5", "E6,E5", "G6,G5", "I6,I5",
                     // Red cannon B7
                     "B7,A7", "B7,C7", "B7,D7", "B7,E7", "B7,F7", "B7,G7",
                     "B7,B8", "B7,B6", "B7,B5", "B7,B4", "B7,B3", "B7,B0",
                     // Red cannon H7
                     "H7,I7", "H7,G7", "H7,F7", "H7,E7", "H7,D7", "H7,C7",
                     "H7,H8", "H7,H6", "H7,H5", "H7,H4", "H7,H3", "H7,H0",
                     // Red chariot
                     "A9,A8", "A9,A7", "I9,I8", "I9,I7",
                     // Red horse
                     "B9,A7", "B9,C7", "H9,G7", "H9,I7",
                     // Red elephant
                     "C9,A7", "C9,E7", "G9,I7", "G9,E7",
                     // Red advisor
                     "D9,E8", "F9,E8",
                     // Red general
                     "E9,E8"}));

  EXPECT_EQ(ToVec(black_possible_moves),
            ToMoves({// Black soldiers
                     "A3,A4", "C3,C4", "E3,E4", "G3,G4", "I3,I4",
                     // Black cannon B2
                     "B2,A2", "B2,C2", "B2,D2", "B2,E2", "B2,F2", "B2,G2",
                     "B2,B1", "B2,B3", "B2,B4", "B2,B5", "B2,B6", "B2,B9",
                     // Black cannon H2
                     "H2,I2", "H2,G2", "H2,F2", "H2,E2", "H2,D2", "H2,C2",
                     "H2,H1", "H2,H3", "H2,H4", "H2,H5", "H2,H6", "H2,H9",
                     // Black chariot
                     "A0,A1", "A0,A2", "I0,I1", "I0,I2",
                     // Black horse
                     "B0,A2", "B0,C2", "H0,G2", "H0,I2",
                     // Black elephant
                     "C0,A2", "C0,E2", "G0,I2", "G0,E2",
                     // Black advisor
                     "D0,E1", "F0,E1",
                     // Black general
                     "E0,E1"}));
}

TEST(PossibleBoards, StartingBoard) {
  const std::vector<Board> red_possible_boards =
      PossibleBoards(kStartingBoard, PLAYER_RED, false);
  const std::vector<Board> black_possible_boards =
      PossibleBoards(kStartingBoard, PLAYER_BLACK, false);

  EXPECT_EQ(red_possible_boards.size(), 44);
  EXPECT_EQ(black_possible_boards.size(), 44);

  EXPECT_THAT(red_possible_boards,
              IsSupersetOf({BoardFromString("  A B C D E F G H I \n"
                                            "0 r C e a g a e h r \n"
                                            "1 . . . * * * . . . \n"
                                            "2 . c . * * * . c . \n"
                                            "3 s . s . s . s . s \n"
                                            "4 - - - - - - - - - \n"
                                            "5 - - - - - - - - - \n"
                                            "6 S . S . S . S . S \n"
                                            "7 . . . * * * . C . \n"
                                            "8 . . . * * * . . . \n"
                                            "9 R H E A G A E H R \n"),
                            BoardFromString("  A B C D E F G H I \n"
                                            "0 r h e a g a e h r \n"
                                            "1 . . . * * * . . . \n"
                                            "2 . c . * * * . c . \n"
                                            "3 s . s . s . s . s \n"
                                            "4 - - - - - - - - - \n"
                                            "5 - - - - - - - - - \n"
                                            "6 S . S . S . S . S \n"
                                            "7 . C H * * * . C . \n"
                                            "8 . . . * * * . . . \n"
                                            "9 R . E A G A E H R \n")}));

  EXPECT_THAT(black_possible_boards,
              IsSupersetOf({BoardFromString("  A B C D E F G H I \n"
                                            "0 r h e a g a e h r \n"
                                            "1 . . . * * * . . . \n"
                                            "2 . . . * * * . c . \n"
                                            "3 s . s . s . s . s \n"
                                            "4 - - - - - - - - - \n"
                                            "5 - - - - - - - - - \n"
                                            "6 S . S . S . S . S \n"
                                            "7 . C . * * * . C . \n"
                                            "8 . . . * * * . . . \n"
                                            "9 R c E A G A E H R \n"),
                            BoardFromString("  A B C D E F G H I \n"
                                            "0 r h . a g a e h r \n"
                                            "1 . . . * * * . . . \n"
                                            "2 . c . * e * . c . \n"
                                            "3 s . s . s . s . s \n"
                                            "4 - - - - - - - - - \n"
                                            "5 - - - - - - - - - \n"
                                            "6 S . S . S . S . S \n"
                                            "7 . C . * * * . C . \n"
                                            "8 . . . * * * . . . \n"
                                            "9 R H E A G A E H R \n")}));
}

}  // namespace
