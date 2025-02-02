#include <gtest/gtest.h>

#include "xiangqi/game.h"

using namespace xiangqi;

TEST(PossibleMoveTest, General) {
  Game game;
  game.Reset();
  // TODO
  EXPECT_EQ(game.PieceAt({9, 4}), R_GENERAL);
  EXPECT_EQ(game.PieceAt({0, 4}), B_GENERAL);
}
