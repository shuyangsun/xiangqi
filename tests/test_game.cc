// file: test_game.cc

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "xiangqi/board.h"
#include "xiangqi/game.h"
#include "xiangqi/types.h"

namespace {

namespace {

using namespace ::xq;

}  // namespace

TEST(Game, InitialState) {
  Game game;
  EXPECT_EQ(game.PieceAt(Pos(9, 4)), R_GENERAL);
  EXPECT_EQ(game.PieceAt(Pos(0, 4)), B_GENERAL);
}

}  // namespace
