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
      "9 . . . G . A . . . \n");
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

  const Board<Piece> board_3 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . a . g . . . \n"
      "1 . . . . H . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . . . G . . . \n");
  EXPECT_EQ(FindGeneral(board_3, Player::RED), PosStr("F9"));
  EXPECT_EQ(FindGeneral(board_3, Player::BLACK), PosStr("F0"));

  const Board<Piece> board_4 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . . . . . . \n"
      "1 . . . g . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . G . . . . . \n"
      "9 . . . . . . . . . \n");
  EXPECT_EQ(FindGeneral(board_4, Player::RED), PosStr("D8"));
  EXPECT_EQ(FindGeneral(board_4, Player::BLACK), PosStr("D1"));

  const Board<Piece> board_5 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . . . . . . \n"
      "1 . . . . g . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . G . . . . \n"
      "9 . . . . . . . . . \n");
  EXPECT_EQ(FindGeneral(board_5, Player::RED), PosStr("E8"));
  EXPECT_EQ(FindGeneral(board_5, Player::BLACK), PosStr("E1"));

  const Board<Piece> board_6 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . . . . . . \n"
      "1 . . . . . g . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . . G . . . \n"
      "9 . . . . . . . . . \n");
  EXPECT_EQ(FindGeneral(board_6, Player::RED), PosStr("F8"));
  EXPECT_EQ(FindGeneral(board_6, Player::BLACK), PosStr("F1"));

  const Board<Piece> board_7 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . . . . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . g . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . G . . . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . . . . . . . \n");
  EXPECT_EQ(FindGeneral(board_7, Player::RED), PosStr("D7"));
  EXPECT_EQ(FindGeneral(board_7, Player::BLACK), PosStr("D2"));

  const Board<Piece> board_8 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . . . . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . . g . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . G . . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . . . . . . . \n");
  EXPECT_EQ(FindGeneral(board_8, Player::RED), PosStr("E7"));
  EXPECT_EQ(FindGeneral(board_8, Player::BLACK), PosStr("E2"));

  const Board<Piece> board_9 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . . . . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . . . g . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . G . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . . . . . . . \n");
  EXPECT_EQ(FindGeneral(board_9, Player::RED), PosStr("F7"));
  EXPECT_EQ(FindGeneral(board_9, Player::BLACK), PosStr("F2"));
}

TEST(BoardTest, FindGeneralOpponentPalace) {
  const Board<Piece> board_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . G . A . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . g . A . . . \n");
  EXPECT_EQ(FindGeneral(board_1, Player::RED), PosStr("D0"));
  EXPECT_EQ(FindGeneral(board_1, Player::BLACK), PosStr("D9"));

  const Board<Piece> board_2 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . a G a . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . A g A . . . \n");
  EXPECT_EQ(FindGeneral(board_2, Player::RED), PosStr("E0"));
  EXPECT_EQ(FindGeneral(board_2, Player::BLACK), PosStr("E9"));

  const Board<Piece> board_3 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . a . G . . . \n"
      "1 . . . . H . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . . . g . . . \n");
  EXPECT_EQ(FindGeneral(board_3, Player::RED), PosStr("F0"));
  EXPECT_EQ(FindGeneral(board_3, Player::BLACK), PosStr("F9"));

  const Board<Piece> board_4 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . . . . . . \n"
      "1 . . . G . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . g . . . . . \n"
      "9 . . . . . . . . . \n");
  EXPECT_EQ(FindGeneral(board_4, Player::RED), PosStr("D1"));
  EXPECT_EQ(FindGeneral(board_4, Player::BLACK), PosStr("D8"));

  const Board<Piece> board_5 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . . . . . . \n"
      "1 . . . . G . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . g . . . . \n"
      "9 . . . . . . . . . \n");
  EXPECT_EQ(FindGeneral(board_5, Player::RED), PosStr("E1"));
  EXPECT_EQ(FindGeneral(board_5, Player::BLACK), PosStr("E8"));

  const Board<Piece> board_6 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . . . . . . \n"
      "1 . . . . . G . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . . g . . . \n"
      "9 . . . . . . . . . \n");
  EXPECT_EQ(FindGeneral(board_6, Player::RED), PosStr("F1"));
  EXPECT_EQ(FindGeneral(board_6, Player::BLACK), PosStr("F8"));

  const Board<Piece> board_7 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . . . . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . G . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . g . . . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . . . . . . . \n");
  EXPECT_EQ(FindGeneral(board_7, Player::RED), PosStr("D2"));
  EXPECT_EQ(FindGeneral(board_7, Player::BLACK), PosStr("D7"));

  const Board<Piece> board_8 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . . . . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . . G . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . g . . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . . . . . . . \n");
  EXPECT_EQ(FindGeneral(board_8, Player::RED), PosStr("E2"));
  EXPECT_EQ(FindGeneral(board_8, Player::BLACK), PosStr("E7"));

  const Board<Piece> board_9 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . . . . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . . . G . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . g . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . . . . . . . \n");
  EXPECT_EQ(FindGeneral(board_9, Player::RED), PosStr("F2"));
  EXPECT_EQ(FindGeneral(board_9, Player::BLACK), PosStr("F7"));
}

TEST(BoardTest, FindGeneralNone) {
  const Board<Piece> board_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . R . a . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . G . A . . . \n");
  EXPECT_EQ(FindGeneral(board_1, Player::BLACK), kNoPosition);

  const Board<Piece> board_2 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . G . a . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . . . A . . . \n");
  EXPECT_EQ(FindGeneral(board_1, Player::BLACK), kNoPosition);

  const Board<Piece> board_3 = BoardFromString(
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
      "9 . . . r . A . . . \n");
  EXPECT_EQ(FindGeneral(board_3, Player::RED), kNoPosition);

  const Board<Piece> board_4 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . . a . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . G . A . . . \n");
  EXPECT_EQ(FindGeneral(board_3, Player::RED), kNoPosition);
}

// ---------------------------------------------------------------------
// Test IsBeingCheckmate
// ---------------------------------------------------------------------

TEST(BoardTest, IsBeingCheckmateFlyingGeneral) {
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
      "9 . . . G . A . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_1, Player::RED));
  EXPECT_TRUE(IsBeingCheckmate(board_1, Player::BLACK));

  const Board<Piece> board_2 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . g . a . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . h . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . G . A . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_2, Player::RED));
  EXPECT_FALSE(IsBeingCheckmate(board_2, Player::BLACK));
}

TEST(BoardTest, IsBeingCheckmateChariot) {
  const Board<Piece> board_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . g . a . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . R . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . . G A . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_1, Player::RED));
  EXPECT_TRUE(IsBeingCheckmate(board_1, Player::BLACK));

  const Board<Piece> board_2 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . g . a . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . . R . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . . G A . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_2, Player::RED));
  EXPECT_FALSE(IsBeingCheckmate(board_2, Player::BLACK));

  const Board<Piece> board_3 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . g . a . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . r . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . . G A . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_3, Player::RED));
  EXPECT_FALSE(IsBeingCheckmate(board_3, Player::BLACK));

  const Board<Piece> board_4 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . g . a . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . r . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . . G A . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_4, Player::RED));
  EXPECT_FALSE(IsBeingCheckmate(board_4, Player::BLACK));

  const Board<Piece> board_5 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . R . g . a . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . . G A . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_5, Player::BLACK));

  const Board<Piece> board_6 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . g . a . R . \n"
      "1 . . . . . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . . G A . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_6, Player::BLACK));

  const Board<Piece> board_7 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . g . a . R . \n"
      "1 . . . . . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . r G A . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_7, Player::RED));

  const Board<Piece> board_8 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . g . a . R . \n"
      "1 . . . . . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . r . . . . \n"
      "9 . . . . G A . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_8, Player::RED));

  const Board<Piece> board_9 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . g r a . R . \n"
      "1 . . . . . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . . G A . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_8, Player::RED));

  const Board<Piece> board_10 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . . . . . . \n"
      "1 R . . g . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . r G . . . . \n"
      "9 . . . . . . . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_10, Player::RED));
  EXPECT_TRUE(IsBeingCheckmate(board_10, Player::BLACK));

  const Board<Piece> board_11 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . . . . . . \n"
      "1 . . . g R . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . G . . . r \n"
      "9 . . . . . . . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_11, Player::RED));
  EXPECT_TRUE(IsBeingCheckmate(board_11, Player::BLACK));

  const Board<Piece> board_12 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . R r . . . . \n"
      "1 . . . g . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . G . . . . \n"
      "9 . . . . . . . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_12, Player::RED));
  EXPECT_TRUE(IsBeingCheckmate(board_12, Player::BLACK));

  const Board<Piece> board_13 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . . . . . . \n"
      "1 . . . g . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . R . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . G . . . r \n"
      "9 . . . . r . . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_13, Player::RED));
  EXPECT_TRUE(IsBeingCheckmate(board_13, Player::BLACK));
}

TEST(BoardTest, IsBeingCheckmateHorse) {
  // Down left
  const Board<Piece> board_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . H . . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . g . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . h . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . . G . . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_1, Player::RED));
  EXPECT_TRUE(IsBeingCheckmate(board_1, Player::BLACK));

  // Down left blocked
  const Board<Piece> board_2 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . H . . . . \n"
      "1 . . . . c . . . . \n"
      "2 . . . g . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . h . . . \n"
      "8 . . . . . C . . . \n"
      "9 . . . . G . . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_2, Player::RED));
  EXPECT_FALSE(IsBeingCheckmate(board_2, Player::BLACK));

  // Down right
  const Board<Piece> board_3 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . H . . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . . . g . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . h . . . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . . G . . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_3, Player::RED));
  EXPECT_TRUE(IsBeingCheckmate(board_3, Player::BLACK));

  // Down right blocked
  const Board<Piece> board_4 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . H . . . . \n"
      "1 . . . . c . . . . \n"
      "2 . . . . . g . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . h . . . . . \n"
      "8 . . . C . . . . . \n"
      "9 . . . . G . . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_4, Player::RED));
  EXPECT_FALSE(IsBeingCheckmate(board_4, Player::BLACK));

  // Up left
  const Board<Piece> board_5 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . . . . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . g . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . H . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . G . . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . . . h . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_5, Player::RED));
  EXPECT_TRUE(IsBeingCheckmate(board_5, Player::BLACK));

  // Up left blocked
  const Board<Piece> board_6 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . . . . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . g . . . . . \n"
      "3 . . . . r . . . . \n"
      "4 . . . . H . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . G . . . . \n"
      "8 . . . . . R . . . \n"
      "9 . . . . . h . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_6, Player::RED));
  EXPECT_FALSE(IsBeingCheckmate(board_6, Player::BLACK));

  // Up right
  const Board<Piece> board_7 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . . . . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . . . g . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . H . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . G . . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . h . . . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_7, Player::RED));
  EXPECT_TRUE(IsBeingCheckmate(board_7, Player::BLACK));

  // Up right blocked
  const Board<Piece> board_8 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . . . . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . . . g . . . \n"
      "3 . . . . r . . . . \n"
      "4 . . . . H . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . G . . . . \n"
      "8 . . . R . . . . . \n"
      "9 . . . h . . . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_8, Player::RED));
  EXPECT_FALSE(IsBeingCheckmate(board_8, Player::BLACK));

  // Left up
  const Board<Piece> board_9 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . . . . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . g . . . . . \n"
      "3 . . . . . H . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . G . . . . \n"
      "9 . . . . . . h . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_9, Player::RED));
  EXPECT_TRUE(IsBeingCheckmate(board_9, Player::BLACK));

  // Left up blocked
  const Board<Piece> board_10 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . . . . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . g . . . . . \n"
      "3 . . . . h H . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . G . . . . \n"
      "9 . . . . . H h . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_10, Player::RED));
  EXPECT_FALSE(IsBeingCheckmate(board_10, Player::BLACK));

  // Left down
  const Board<Piece> board_11 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . . H . . . \n"
      "1 . . . g h . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . h . . \n"
      "8 . . . . G H . . . \n"
      "9 . . . . . . . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_11, Player::RED));
  EXPECT_TRUE(IsBeingCheckmate(board_11, Player::BLACK));

  // Left down blocked
  const Board<Piece> board_12 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . h H . . . \n"
      "1 . . . g . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . H h . . \n"
      "8 . . . . G . . . . \n"
      "9 . . . . . . . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_12, Player::RED));
  EXPECT_FALSE(IsBeingCheckmate(board_12, Player::BLACK));

  // Right up
  const Board<Piece> board_13 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . . . . . . \n"
      "1 . . h g . . . . . \n"
      "2 . H . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . H G . . . . \n"
      "9 . . h . . . . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_13, Player::RED));
  EXPECT_TRUE(IsBeingCheckmate(board_13, Player::BLACK));

  // Right up blocked
  const Board<Piece> board_14 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . . . . . . . \n"
      "1 . . . g . . . . . \n"
      "2 . H h . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . G . . . . \n"
      "9 . . h H . . . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_14, Player::RED));
  EXPECT_FALSE(IsBeingCheckmate(board_14, Player::BLACK));

  // Right down
  const Board<Piece> board_15 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . H . . . . . . . \n"
      "1 . . h g . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . h . . . . . . \n"
      "8 . . . H G . . . . \n"
      "9 . . . . . . . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_15, Player::RED));
  EXPECT_TRUE(IsBeingCheckmate(board_15, Player::BLACK));

  // Right down blocked
  const Board<Piece> board_16 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . H h . . . . . . \n"
      "1 . . . g . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . h H . . . . . \n"
      "8 . . . . G . . . . \n"
      "9 . . . . . . . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_16, Player::RED));
  EXPECT_FALSE(IsBeingCheckmate(board_16, Player::BLACK));

  // Too far
  const Board<Piece> board_17 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 H . . . . . . . H \n"
      "1 . . . g . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . G . . . . \n"
      "9 h . . . . . . . h \n");
  EXPECT_FALSE(IsBeingCheckmate(board_17, Player::RED));
  EXPECT_FALSE(IsBeingCheckmate(board_17, Player::BLACK));
}

TEST(BoardTest, IsBeingCheckmateCannon) {
  const Board<Piece> board_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . g . a . C . \n"
      "1 . . . . . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . . . . . . \n"
      "9 . . . . G c h . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_1, Player::RED));
  EXPECT_TRUE(IsBeingCheckmate(board_1, Player::BLACK));

  const Board<Piece> board_2 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . h C g . a . . . \n"
      "1 . . . . . . . . . \n"
      "2 . . . . . . . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "6 . . . . . . . . . \n"
      "7 . . . . . . . . . \n"
      "8 . . . . . . . . . \n"
      "9 c h . . G . . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_2, Player::RED));
  EXPECT_FALSE(IsBeingCheckmate(board_2, Player::BLACK));
}

}  // namespace
