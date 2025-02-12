#include <gtest/gtest.h>

#include "xiangqi/board.h"
#include "xiangqi/types.h"

namespace {

namespace {

using namespace ::xq;
using enum ::xq::Piece;

}  // namespace

TEST(BoardTest, Equal) {
  const Board<Piece> a = kStartingBoard;
  const Board<Piece> b = kStartingBoard;
  EXPECT_TRUE(BoardEq(a, b));
}

TEST(BoardTest, NotEqual) {
  const Board<Piece> a = kStartingBoard;
  Board<Piece> b = kStartingBoard;
  b[Pos(0, 0)] = EMPTY;
  EXPECT_FALSE(BoardEq(a, b));
}

TEST(BoardTest, FromString) {
  const Board<Piece> board = BoardFromString(
      "  A B C D E F G H I \n"
      "0 r h e a g a e h r \n"
      "1 . . . . . . . . . \n"
      "2 . c . . . . . c . \n"
      "3 s . s . s . s . s \n"
      "4 . . . . . . . . . \n"
      "5 . . . . . . . . . \n"
      "7 S . S . S . S . S \n"
      "7 . C . . . . . C . \n"
      "8 . . . . . . . . . \n"
      "9 R H E A G A E H R \n");
}

}  // namespace
