#include <gtest/gtest.h>

#include <string>
#include <string_view>

#include "xiangqi/board.h"
#include "xiangqi/types.h"

namespace {

namespace {

using namespace ::xq;
using enum ::xq::Piece;

constexpr std::string_view kStartingBoardStr =
    "  A B C D E F G H I \n"
    "0 r h e a g a e h r \n"
    "1 . . . . . . . . . \n"
    "2 . c . . . . . c . \n"
    "3 s . s . s . s . s \n"
    "4 . . . . . . . . . \n"
    "5 . . . . . . . . . \n"
    "6 S . S . S . S . S \n"
    "7 . C . . . . . C . \n"
    "8 . . . . . . . . . \n"
    "9 R H E A G A E H R \n";

}  // namespace

TEST(BoardTest, PosStr) {
  EXPECT_EQ(PosStr("a0"), Pos(0, 0));
  EXPECT_EQ(PosStr("A2"), Pos(2, 0));
  EXPECT_EQ(PosStr("I5"), Pos(5, 8));
  EXPECT_EQ(PosStr("F9"), Pos(9, 5));
}

TEST(BoardTest, BoardEqual) {
  const Board<Piece> a = kStartingBoard;
  const Board<Piece> b = kStartingBoard;
  EXPECT_EQ(a, b);
}

TEST(BoardTest, BoardNotEqual) {
  const Board<Piece> a = kStartingBoard;
  Board<Piece> b = kStartingBoard;
  b[Pos(0, 0)] = EMPTY;
  EXPECT_NE(a, b);
}

TEST(BoardTest, FromString) {
  const Board<Piece> board = BoardFromString(kStartingBoardStr);
  EXPECT_EQ(board, kStartingBoard) << "Expected boards to equal:\n"
                                   << BoardToString(board) << "\n"
                                   << BoardToString(kStartingBoard);
}

TEST(BoardTest, ToString) {
  const std::string str = BoardToString(kStartingBoard);
  EXPECT_EQ(str, kStartingBoardStr) << "Expected board strings to equal:\n"
                                    << str << "\n"
                                    << kStartingBoardStr;
}

// ---------------------------------------------------------------------
// Test FindGeneral
// ---------------------------------------------------------------------

TEST(BoardTest, FindGeneralOwnPalace) {
  const Board<Piece> board_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . g . a . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . G . a . . . \n");
  EXPECT_EQ(FindGeneral(board_1, Player::RED), PosStr("D9"));
  EXPECT_EQ(FindGeneral(board_1, Player::BLACK), PosStr("D0"));

  const Board<Piece> board_2 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . a g a . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . A G A . . . \n");
  EXPECT_EQ(FindGeneral(board_2, Player::RED), PosStr("E9"));
  EXPECT_EQ(FindGeneral(board_2, Player::BLACK), PosStr("E0"));
}

}  // namespace
