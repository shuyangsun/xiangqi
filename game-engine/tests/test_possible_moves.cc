// file: test_possible_moves.cc

#include <gtest/gtest.h>

#include "xiangqi/game.h"
#include "xiangqi/types.h"

using namespace xiangqi;

namespace {

// Helper: Check that every cell in a Board<bool> is false.
void ExpectAllFalse(const Board<bool>& moves) {
  for (uint8_t r = 0; r < kTotalRow; ++r) {
    for (uint8_t c = 0; c < kTotalCol; ++c) {
      EXPECT_FALSE(moves[r][c])
          << "Expected false at (" << unsigned(r) << "," << unsigned(c) << ")";
    }
  }
}

//--------------------------------------------------------------
// Test when the selected square is empty.
//--------------------------------------------------------------
TEST(PossibleMovesTest, EmptySquareReturnsAllFalse) {
  Game game;
  // Initialize board with one piece (red general) at (8,4); every other square
  // is empty.
  game.Reset({{R_GENERAL, {8, 4}}});
  // Query a position with no piece.
  Board<bool> moves = game.PossibleMoves({0, 0});
  ExpectAllFalse(moves);
}

//--------------------------------------------------------------
// Standard General Moves
//--------------------------------------------------------------

// Red General in the center of its palace.
TEST(PossibleMovesTest, RedGeneralCenter) {
  Game game;
  // Red palace: rows 7 to 9, cols 3 to 5.
  game.Reset({{R_GENERAL, {8, 4}}});
  Board<bool> moves = game.PossibleMoves({8, 4});
  // Expected normal moves: up (7,4), down (9,4), left (8,3), right (8,5)
  EXPECT_TRUE(moves[7][4]);
  EXPECT_TRUE(moves[9][4]);
  EXPECT_TRUE(moves[8][3]);
  EXPECT_TRUE(moves[8][5]);
  // The current square should not be marked.
  EXPECT_FALSE(moves[8][4]);
}

// Black General in the center of its palace.
TEST(PossibleMovesTest, BlackGeneralCenter) {
  Game game;
  // Black palace: rows 0 to 2, cols 3 to 5.
  game.Reset({{B_GENERAL, {1, 4}}});
  Board<bool> moves = game.PossibleMoves({1, 4});
  // Expected normal moves: up (0,4), down (2,4), left (1,3), right (1,5)
  EXPECT_TRUE(moves[0][4]);
  EXPECT_TRUE(moves[2][4]);
  EXPECT_TRUE(moves[1][3]);
  EXPECT_TRUE(moves[1][5]);
}

//--------------------------------------------------------------
// Flying General Check Tests
//--------------------------------------------------------------

// Flying General check for Red: Unblocked flying move.
// Place the red general at (8,4) in its palace and the black general at (1,4)
// in its palace. There are no pieces between them, so the red general's moves
// should include the black general's square (1,4) as a "flying" move.
TEST(PossibleMovesTest, RedGeneralFlyingUnblocked) {
  Game game;
  game.Reset({{R_GENERAL, {8, 4}}, {B_GENERAL, {1, 4}}});
  Board<bool> moves = game.PossibleMoves({8, 4});
  // Flying move: red general should be allowed to move to (1,4).
  EXPECT_TRUE(moves[1][4]);
}

// Flying General check for Red: Blocked flying move.
// Insert a blocking piece between the generals so that the flying move is not
// available.
TEST(PossibleMovesTest, RedGeneralFlyingBlocked) {
  Game game;
  // Place red general at (8,4), black general at (1,4), and a blocking piece at
  // (5,4).
  game.Reset({{R_GENERAL, {8, 4}}, {B_GENERAL, {1, 4}}, {R_SOLDIER_1, {5, 4}}});
  Board<bool> moves = game.PossibleMoves({8, 4});
  // With a block in between, the flying move should not be available.
  EXPECT_FALSE(moves[1][4]);
}

// Flying General check for Black: Unblocked flying move.
// Place the black general at (1,4) and the red general at (8,4). With no block
// between them, the black general's move set should mark (8,4) as available.
TEST(PossibleMovesTest, BlackGeneralFlyingUnblocked) {
  Game game;
  game.Reset({{B_GENERAL, {1, 4}}, {R_GENERAL, {8, 4}}});
  Board<bool> moves = game.PossibleMoves({1, 4});
  EXPECT_TRUE(moves[8][4]);
}

// Flying General check for Black: Blocked flying move.
// Place a blocking piece between the black general and the red general.
TEST(PossibleMovesTest, BlackGeneralFlyingBlocked) {
  Game game;
  // Place black general at (1,4), red general at (8,4), and a blocking piece at
  // (3,4).
  game.Reset({{B_GENERAL, {1, 4}}, {R_GENERAL, {8, 4}}, {B_SOLDIER_1, {3, 4}}});
  Board<bool> moves = game.PossibleMoves({1, 4});
  EXPECT_FALSE(moves[8][4]);
}

// Flying General not triggered when generals are not in the same column.
// Here, we place the red general at (8,4) and the black general at (1,5). Since
// they are not aligned on the same column, the flying general move should not
// be marked.
TEST(PossibleMovesTest, FlyingGeneralNotTriggeredDifferentColumn) {
  Game game;
  game.Reset({{R_GENERAL, {8, 4}}, {B_GENERAL, {1, 5}}});
  Board<bool> moves = game.PossibleMoves({8, 4});
  // Ensure that the square (1,5) is not marked due to flying general.
  EXPECT_FALSE(moves[1][5]);
}

//--------------------------------------------------------------
// Advisor Tests
//--------------------------------------------------------------

// Red Advisor at a palace edge.
TEST(PossibleMovesTest, RedAdvisorEdge) {
  Game game;
  // Place a red advisor at (9,3) (red palace: rows 7-9, cols 3-5).
  game.Reset({{R_ADVISOR_L, {9, 3}}});
  Board<bool> moves = game.PossibleMoves({9, 3});
  // Advisors move one square diagonally inside the palace.
  // For (9,3), only (8,4) is valid (since (8,2) is outside the palace and moves
  // below row 9 are off-board).
  EXPECT_TRUE(moves[8][4]);
  EXPECT_FALSE(moves[8][2]);
}

// Black Advisor in the center.
TEST(PossibleMovesTest, BlackAdvisorCenter) {
  Game game;
  // Place a black advisor at (0,4) (black palace: rows 0-2, cols 3-5).
  game.Reset({{B_ADVISOR_R, {0, 4}}});
  Board<bool> moves = game.PossibleMoves({0, 4});
  // For (0,4), the only valid diagonal moves are downward: (1,3) and (1,5).
  EXPECT_TRUE(moves[1][3]);
  EXPECT_TRUE(moves[1][5]);
  EXPECT_FALSE(moves[0][3]);
  EXPECT_FALSE(moves[0][5]);
}

//--------------------------------------------------------------
// Elephant Tests
//--------------------------------------------------------------

// Red Elephant with all intermediate "eyes" clear.
TEST(PossibleMovesTest, RedElephantNormal) {
  Game game;
  // For red elephants, allowed rows: 5 to 9.
  // Place a red elephant at (7,2).
  game.Reset({{R_ELEPHANT_L, {7, 2}}});
  Board<bool> moves = game.PossibleMoves({7, 2});
  // Expected moves from (7,2):
  // Up-right: (5,4), Up-left: (5,0), Down-right: (9,4), Down-left: (9,0)
  EXPECT_TRUE(moves[5][4]);
  EXPECT_TRUE(moves[5][0]);
  EXPECT_TRUE(moves[9][4]);
  EXPECT_TRUE(moves[9][0]);
}

// Red Elephant with one move blocked.
TEST(PossibleMovesTest, RedElephantBlocked) {
  Game game;
  // Block the "up-right" move by placing a red soldier at the intermediate
  // square (6,3).
  game.Reset({{R_ELEPHANT_L, {7, 2}}, {R_SOLDIER_1, {6, 3}}});
  Board<bool> moves = game.PossibleMoves({7, 2});
  // The up-right move to (5,4) should be disallowed.
  EXPECT_FALSE(moves[5][4]);
  // Other moves remain available.
  EXPECT_TRUE(moves[5][0]);
  EXPECT_TRUE(moves[9][4]);
  EXPECT_TRUE(moves[9][0]);
}

//--------------------------------------------------------------
// Horse Tests
//--------------------------------------------------------------

// Red Horse with no blocking ("legs" clear).
TEST(PossibleMovesTest, RedHorseNormal) {
  Game game;
  // Place a red horse at (5,5).
  game.Reset({{R_HORSE_L, {5, 5}}});
  Board<bool> moves = game.PossibleMoves({5, 5});
  // A horse moves one square orthogonally and then one square diagonally
  // outward. Expected moves (if all legs are clear):
  //   Upward leg: (3,4) and (3,6).
  //   Downward leg: (7,4) and (7,6).
  //   Leftward leg: (4,3) and (6,3).
  //   Rightward leg: (4,7) and (6,7).
  EXPECT_TRUE(moves[3][4]);
  EXPECT_TRUE(moves[3][6]);
  EXPECT_TRUE(moves[7][4]);
  EXPECT_TRUE(moves[7][6]);
  EXPECT_TRUE(moves[4][3]);
  EXPECT_TRUE(moves[6][3]);
  EXPECT_TRUE(moves[4][7]);
  EXPECT_TRUE(moves[6][7]);
}

// Red Horse with the upward leg blocked.
TEST(PossibleMovesTest, RedHorseBlockedUp) {
  Game game;
  // Block the upward leg by placing a red soldier at (4,5).
  game.Reset({{R_HORSE_L, {5, 5}}, {R_SOLDIER_1, {4, 5}}});
  Board<bool> moves = game.PossibleMoves({5, 5});
  // With the upward leg blocked, moves that depend on it ((3,4) and (3,6)) must
  // not be allowed.
  EXPECT_FALSE(moves[3][4]);
  EXPECT_FALSE(moves[3][6]);
  // Other moves (down, left, right) remain available.
  EXPECT_TRUE(moves[7][4]);
  EXPECT_TRUE(moves[7][6]);
  EXPECT_TRUE(moves[4][3]);
  EXPECT_TRUE(moves[6][3]);
  EXPECT_TRUE(moves[4][7]);
  EXPECT_TRUE(moves[6][7]);
}

//--------------------------------------------------------------
// Chariot Tests
//--------------------------------------------------------------

// Red Chariot on an empty board.
TEST(PossibleMovesTest, RedChariotEmptyBoard) {
  Game game;
  // Place a red chariot at (5,5).
  game.Reset({{R_CHARIOT_L, {5, 5}}});
  Board<bool> moves = game.PossibleMoves({5, 5});
  // The chariot moves straight along rows and columns until an obstruction.
  // Upward (row 4 to 0, col 5):
  for (int r = 4; r >= 0; --r) {
    EXPECT_TRUE(moves[r][5]) << "Expected move at (" << r << ",5)";
  }
  // Downward (row 6 to 9, col 5):
  for (int r = 6; r < kTotalRow; ++r) {
    EXPECT_TRUE(moves[r][5]) << "Expected move at (" << r << ",5)";
  }
  // Leftward (row 5, cols 4 to 0):
  for (int c = 4; c >= 0; --c) {
    EXPECT_TRUE(moves[5][c]) << "Expected move at (5," << c << ")";
  }
  // Rightward (row 5, cols 6 to 8):
  for (int c = 6; c < kTotalCol; ++c) {
    EXPECT_TRUE(moves[5][c]) << "Expected move at (5," << c << ")";
  }
}

// Red Chariot blocked by an allied piece.
TEST(PossibleMovesTest, RedChariotBlockedByAlly) {
  Game game;
  // Place a red chariot at (5,5) and a red soldier at (3,5) to block upward
  // movement.
  game.Reset({{R_CHARIOT_L, {5, 5}}, {R_SOLDIER_1, {3, 5}}});
  Board<bool> moves = game.PossibleMoves({5, 5});
  // Upward: (4,5) should be available (empty), but (3,5) should not be (blocked
  // by ally), and no moves beyond.
  EXPECT_TRUE(moves[4][5]);
  EXPECT_FALSE(moves[3][5]);
}

// Red Chariot capturing an enemy piece.
TEST(PossibleMovesTest, RedChariotCapturingEnemy) {
  Game game;
  // Place a red chariot at (5,5) and an enemy (black) soldier at (3,5).
  game.Reset({{R_CHARIOT_L, {5, 5}}, {B_SOLDIER_1, {3, 5}}});
  Board<bool> moves = game.PossibleMoves({5, 5});
  // Upward: It should mark (4,5) as a normal move and (3,5) as a capturing
  // move.
  EXPECT_TRUE(moves[4][5]);
  EXPECT_TRUE(moves[3][5]);
  // No move beyond (3,5) should be allowed.
  if (3 > 0) EXPECT_FALSE(moves[2][5]);
}

//--------------------------------------------------------------
// Cannon Tests
//--------------------------------------------------------------

// Red Cannon on an empty board (non-capturing moves).
TEST(PossibleMovesTest, RedCannonEmptyBoard) {
  Game game;
  // Place a red cannon at (5,5).
  game.Reset({{R_CANNON_L, {5, 5}}});
  Board<bool> moves = game.PossibleMoves({5, 5});
  // With no intervening pieces, a cannon moves like a chariot.
  // Upward:
  for (int r = 4; r >= 0; --r) {
    EXPECT_TRUE(moves[r][5]) << "Expected move at (" << r << ",5)";
  }
  // Downward:
  for (int r = 6; r < kTotalRow; ++r) {
    EXPECT_TRUE(moves[r][5]) << "Expected move at (" << r << ",5)";
  }
  // Leftward:
  for (int c = 4; c >= 0; --c) {
    EXPECT_TRUE(moves[5][c]) << "Expected move at (5," << c << ")";
  }
  // Rightward:
  for (int c = 6; c < kTotalCol; ++c) {
    EXPECT_TRUE(moves[5][c]) << "Expected move at (5," << c << ")";
  }
}

// Red Cannon capturing enemy with a proper screen.
TEST(PossibleMovesTest, RedCannonCapturing) {
  Game game;
  // Place a red cannon at (5,5), a blocking (screen) piece at (5,4), and an
  // enemy piece at (5,3).
  game.Reset(
      {{R_CANNON_L, {5, 5}}, {R_SOLDIER_1, {5, 4}}, {B_SOLDIER_1, {5, 3}}});
  Board<bool> moves = game.PossibleMoves({5, 5});
  // In the leftward direction:
  // The square at (5,4) becomes the screen and is not a destination.
  // (5,3) is an enemy piece immediately after the screen.
  EXPECT_TRUE(moves[5][3]);
  // Further left (j = 2 and beyond) should not be marked.
  EXPECT_FALSE(moves[5][2]);
}

//--------------------------------------------------------------
// Soldier Tests
//--------------------------------------------------------------

// Red Soldier not yet crossed the river.
TEST(PossibleMovesTest, RedSoldierNotCrossed) {
  Game game;
  // For red, not crossed if row > 4. Place a red soldier at (6,5).
  game.Reset({{R_SOLDIER_1, {6, 5}}});
  Board<bool> moves = game.PossibleMoves({6, 5});
  // Expected: only forward move. For red, forward is upward (row - 1).
  EXPECT_TRUE(moves[5][5]);
  // No sideways moves because the soldier has not crossed the river.
  EXPECT_FALSE(moves[6][4]);
  EXPECT_FALSE(moves[6][6]);
}

// Red Soldier after crossing the river.
TEST(PossibleMovesTest, RedSoldierCrossed) {
  Game game;
  // For red, consider row 4 as having crossed.
  game.Reset({{R_SOLDIER_1, {4, 5}}});
  Board<bool> moves = game.PossibleMoves({4, 5});
  // Expected: forward (3,5) plus sideways (4,4) and (4,6).
  EXPECT_TRUE(moves[3][5]);
  EXPECT_TRUE(moves[4][4]);
  EXPECT_TRUE(moves[4][6]);
}

// Black Soldier not yet crossed.
TEST(PossibleMovesTest, BlackSoldierNotCrossed) {
  Game game;
  // For black, not crossed if row < 5. Place a black soldier at (3,5).
  game.Reset({{B_SOLDIER_1, {3, 5}}});
  Board<bool> moves = game.PossibleMoves({3, 5});
  // For black, forward is downward (row + 1).
  EXPECT_TRUE(moves[4][5]);
  // No sideways moves yet.
  EXPECT_FALSE(moves[3][4]);
  EXPECT_FALSE(moves[3][6]);
}

// Black Soldier after crossing.
TEST(PossibleMovesTest, BlackSoldierCrossed) {
  Game game;
  // For black, crossed if row >= 5. Place a black soldier at (6,5).
  game.Reset({{B_SOLDIER_1, {6, 5}}});
  Board<bool> moves = game.PossibleMoves({6, 5});
  // Expected: forward (7,5) and sideways (6,4) and (6,6).
  EXPECT_TRUE(moves[7][5]);
  EXPECT_TRUE(moves[6][4]);
  EXPECT_TRUE(moves[6][6]);
}

}  // namespace
