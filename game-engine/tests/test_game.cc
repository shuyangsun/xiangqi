// file: test_game.cc

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "xiangqi/game.h"
#include "xiangqi/types.h"

namespace {

using namespace ::xiangqi;
using enum ::xiangqi::Piece;

// ---------------------------------------------------------------------
// Verify at compile time that Game is neither copyable nor movable.
static_assert(!std::is_copy_constructible<Game>::value,
              "Game should not be copyable");
static_assert(!std::is_move_constructible<Game>::value,
              "Game should not be movable");

// ---------------------------------------------------------------------
// Test that the default game board (constructed without a reset) is
// set up with the standard Xiangqi opening position.
// ---------------------------------------------------------------------
TEST(GameTest, InitialState) {
  Game game;
  EXPECT_EQ(game.PieceAt({9, 4}), R_GENERAL);
  EXPECT_EQ(game.PieceAt({0, 4}), B_GENERAL);
}

// ---------------------------------------------------------------------
// Test that the entire default board has the proper piece placements.
// (Standard Xiangqi layout: Black pieces on top, Red pieces at the bottom.)
// ---------------------------------------------------------------------
TEST(GameTest, DefaultBoardPieces) {
  Game game;

  // --- Black side ---
  // Row 0: major pieces.
  EXPECT_EQ(game.PieceAt({0, 0}), B_CHARIOT_L);
  EXPECT_EQ(game.PieceAt({0, 1}), B_HORSE_L);
  EXPECT_EQ(game.PieceAt({0, 2}), B_ELEPHANT_L);
  EXPECT_EQ(game.PieceAt({0, 3}), B_ADVISOR_L);
  EXPECT_EQ(game.PieceAt({0, 4}), B_GENERAL);
  EXPECT_EQ(game.PieceAt({0, 5}), B_ADVISOR_R);
  EXPECT_EQ(game.PieceAt({0, 6}), B_ELEPHANT_R);
  EXPECT_EQ(game.PieceAt({0, 7}), B_HORSE_R);
  EXPECT_EQ(game.PieceAt({0, 8}), B_CHARIOT_R);

  // Row 2: Cannons.
  EXPECT_EQ(game.PieceAt({2, 1}), B_CANNON_L);
  EXPECT_EQ(game.PieceAt({2, 7}), B_CANNON_R);

  // Row 3: Soldiers.
  EXPECT_EQ(game.PieceAt({3, 0}), B_SOLDIER_1);
  EXPECT_EQ(game.PieceAt({3, 2}), B_SOLDIER_2);
  EXPECT_EQ(game.PieceAt({3, 4}), B_SOLDIER_3);
  EXPECT_EQ(game.PieceAt({3, 6}), B_SOLDIER_4);
  EXPECT_EQ(game.PieceAt({3, 8}), B_SOLDIER_5);

  // --- Red side ---
  // Row 9: major pieces.
  EXPECT_EQ(game.PieceAt({9, 0}), R_CHARIOT_L);
  EXPECT_EQ(game.PieceAt({9, 1}), R_HORSE_L);
  EXPECT_EQ(game.PieceAt({9, 2}), R_ELEPHANT_L);
  EXPECT_EQ(game.PieceAt({9, 3}), R_ADVISOR_L);
  EXPECT_EQ(game.PieceAt({9, 4}), R_GENERAL);
  EXPECT_EQ(game.PieceAt({9, 5}), R_ADVISOR_R);
  EXPECT_EQ(game.PieceAt({9, 6}), R_ELEPHANT_R);
  EXPECT_EQ(game.PieceAt({9, 7}), R_HORSE_R);
  EXPECT_EQ(game.PieceAt({9, 8}), R_CHARIOT_R);

  // Row 7: Cannons.
  EXPECT_EQ(game.PieceAt({7, 1}), R_CANNON_L);
  EXPECT_EQ(game.PieceAt({7, 7}), R_CANNON_R);

  // Row 6: Soldiers.
  EXPECT_EQ(game.PieceAt({6, 0}), R_SOLDIER_1);
  EXPECT_EQ(game.PieceAt({6, 2}), R_SOLDIER_2);
  EXPECT_EQ(game.PieceAt({6, 4}), R_SOLDIER_3);
  EXPECT_EQ(game.PieceAt({6, 6}), R_SOLDIER_4);
  EXPECT_EQ(game.PieceAt({6, 8}), R_SOLDIER_5);
}

// ---------------------------------------------------------------------
// Test Reset() using an unordered_map (custom configuration) by
// providing a piece map and verifying that only those pieces appear in
// the expected positions.
// ---------------------------------------------------------------------
TEST(GameTest, ResetWithCustomPieceMap) {
  Game game;
  std::unordered_map<Piece, Position> piece_map;
  piece_map[R_GENERAL] = {5, 5};
  piece_map[B_GENERAL] = {4, 4};
  piece_map[R_SOLDIER_1] = {0, 0};

  game.Reset(std::move(piece_map));
  Board<Piece> current_board = game.CurrentBoard();

  EXPECT_EQ(current_board[5][5], R_GENERAL);
  EXPECT_EQ(current_board[4][4], B_GENERAL);
  EXPECT_EQ(current_board[0][0], R_SOLDIER_1);
  // Verify that all other positions are EMPTY.
  for (uint8_t i = 0; i < kTotalRow; ++i) {
    for (uint8_t j = 0; j < kTotalCol; ++j) {
      if ((i == 5 && j == 5) || (i == 4 && j == 4) || (i == 0 && j == 0))
        continue;
      EXPECT_EQ(current_board[i][j], EMPTY);
    }
  }
}

// ---------------------------------------------------------------------
// Test Reset() with another piece map configuration.
// ---------------------------------------------------------------------
TEST(GameTest, ResetWithPieceMap) {
  Game game;
  std::unordered_map<Piece, Position> piece_map;
  piece_map[R_GENERAL] = {5, 5};
  piece_map[B_GENERAL] = {4, 4};
  piece_map[R_CHARIOT_L] = {8, 0};

  game.Reset(std::move(piece_map));
  Board<Piece> current_board = game.CurrentBoard();

  EXPECT_EQ(current_board[5][5], R_GENERAL);
  EXPECT_EQ(current_board[4][4], B_GENERAL);
  EXPECT_EQ(current_board[8][0], R_CHARIOT_L);
  // Ensure all other positions are EMPTY.
  for (uint8_t i = 0; i < kTotalRow; ++i) {
    for (uint8_t j = 0; j < kTotalCol; ++j) {
      if ((i == 5 && j == 5) || (i == 4 && j == 4) || (i == 8 && j == 0))
        continue;
      EXPECT_EQ(current_board[i][j], EMPTY);
    }
  }
}

// ---------------------------------------------------------------------
// Test that calling ChangeTurn() does not alter the board.
// ---------------------------------------------------------------------
TEST(GameTest, ChangeTurnDoesNotAffectBoard) {
  Game game;
  Board<Piece> board_before = game.CurrentBoard();
  game.ChangeTurn();
  Board<Piece> board_after = game.CurrentBoard();
  EXPECT_EQ(board_before, board_after);
}

// ---------------------------------------------------------------------
// Test that PieceAt() returns the correct piece for a given position.
// Set a custom configuration with only one piece using a piece map.
// ---------------------------------------------------------------------
TEST(GameTest, PieceAtCustomPosition) {
  Game game;
  std::unordered_map<Piece, Position> piece_map;
  piece_map[B_GENERAL] = {2, 3};

  game.Reset(std::move(piece_map));
  EXPECT_EQ(game.PieceAt({2, 3}), B_GENERAL);
  EXPECT_EQ(game.PieceAt({0, 0}), EMPTY);
}

// ---------------------------------------------------------------------
// Test Move() for a non-capturing move using a piece map to set the board.
// ---------------------------------------------------------------------
TEST(GameTest, MoveNonCapture) {
  Game game;
  std::unordered_map<Piece, Position> piece_map;
  piece_map[R_SOLDIER_1] = {5, 5};

  game.Reset(std::move(piece_map));

  // Move red soldier from (5,5) to an empty square (4,5).
  bool captured = game.Move({5, 5}, {4, 5});
  EXPECT_FALSE(captured);
  EXPECT_EQ(game.PieceAt({5, 5}), EMPTY);
  EXPECT_EQ(game.PieceAt({4, 5}), R_SOLDIER_1);
}

// ---------------------------------------------------------------------
// Test Move() for a capturing move using a piece map to set the board.
// ---------------------------------------------------------------------
TEST(GameTest, MoveCapture) {
  Game game;
  std::unordered_map<Piece, Position> piece_map;
  piece_map[R_SOLDIER_1] = {5, 5};
  piece_map[B_SOLDIER_1] = {5, 6};

  game.Reset(std::move(piece_map));

  // Move red soldier from (5,5) to (5,6), capturing the black soldier.
  bool captured = game.Move({5, 5}, {5, 6});
  EXPECT_TRUE(captured);
  EXPECT_EQ(game.PieceAt({5, 5}), EMPTY);
  EXPECT_EQ(game.PieceAt({5, 6}), R_SOLDIER_1);
}

// ---------------------------------------------------------------------
// Test Move() when attempting to move from an empty square. We set the
// board to be completely empty via an empty piece map.
// ---------------------------------------------------------------------
TEST(GameTest, MoveFromEmpty) {
  Game game;
  std::unordered_map<Piece, Position> empty_map;  // No pieces set.
  game.Reset(std::move(empty_map));

  // Attempt to move from an empty square.
  bool captured = game.Move({4, 4}, {3, 4});
  EXPECT_FALSE(captured);
  EXPECT_EQ(game.PieceAt({4, 4}), EMPTY);
  EXPECT_EQ(game.PieceAt({3, 4}), EMPTY);
}

// ---------------------------------------------------------------------
// Test that CurrentBoard() returns a copy of the board rather than a
// reference to the internal state.
// ---------------------------------------------------------------------
TEST(GameTest, CurrentBoardReturnsCopy) {
  Game game;
  Board<Piece> board_copy = game.CurrentBoard();
  // Make a move in the game. (Using the default board configuration.)
  game.Move({9, 4}, {8, 4});  // Move the red general.
  // The copy should not reflect the move.
  EXPECT_EQ(board_copy[9][4], R_GENERAL);
  EXPECT_EQ(board_copy[8][4], EMPTY);
  // The current board should show the change.
  EXPECT_EQ(game.PieceAt({9, 4}), EMPTY);
  EXPECT_EQ(game.PieceAt({8, 4}), R_GENERAL);
}

// ---------------------------------------------------------------------
// Test Undo() when no moves have been made.
// ---------------------------------------------------------------------
TEST(GameTest, UndoWithoutMove) {
  Game game;
  EXPECT_FALSE(game.Undo());
}

// ---------------------------------------------------------------------
// Test Undo() after a single move using a piece map reset.
// ---------------------------------------------------------------------
TEST(GameTest, UndoAfterOneMove) {
  Game game;
  std::unordered_map<Piece, Position> piece_map;
  piece_map[R_SOLDIER_1] = {5, 5};

  game.Reset(std::move(piece_map));

  Board<Piece> board_before = game.CurrentBoard();

  // Make a move.
  game.Move({5, 5}, {4, 5});
  EXPECT_EQ(game.PieceAt({5, 5}), EMPTY);
  EXPECT_EQ(game.PieceAt({4, 5}), R_SOLDIER_1);

  // Undo the move.
  EXPECT_TRUE(game.Undo());
  Board<Piece> board_after = game.CurrentBoard();
  EXPECT_EQ(board_before, board_after);
}

// ---------------------------------------------------------------------
// Test Undo() after multiple moves using a piece map reset.
// ---------------------------------------------------------------------
TEST(GameTest, UndoMultipleMoves) {
  Game game;
  std::unordered_map<Piece, Position> piece_map;
  piece_map[R_SOLDIER_1] = {5, 5};
  piece_map[B_SOLDIER_1] = {4, 5};

  game.Reset(std::move(piece_map));

  Board<Piece> initial_board = game.CurrentBoard();

  // First move: red soldier moves from (5,5) to (4,5) and captures black
  // soldier.
  game.Move({5, 5}, {4, 5});
  // Second move: red soldier moves forward from (4,5) to (3,5).
  game.Move({4, 5}, {3, 5});

  // Undo the second move.
  EXPECT_TRUE(game.Undo());
  EXPECT_EQ(game.PieceAt({4, 5}), R_SOLDIER_1);
  EXPECT_EQ(game.PieceAt({3, 5}), EMPTY);

  // Undo the first move.
  EXPECT_TRUE(game.Undo());
  Board<Piece> board_after = game.CurrentBoard();
  EXPECT_EQ(initial_board, board_after);
}

// ---------------------------------------------------------------------
// Test that calling Reset() clears the move history.
// After a Reset, Undo() should not succeed.
// ---------------------------------------------------------------------
TEST(GameTest, ResetClearsHistory) {
  Game game;
  std::unordered_map<Piece, Position> piece_map;
  piece_map[R_SOLDIER_1] = {5, 5};

  game.Reset(std::move(piece_map));

  // Make a move.
  game.Move({5, 5}, {4, 5});
  // Now reset to the default board.
  game.Reset();
  // Undo should now fail because history has been cleared.
  EXPECT_FALSE(game.Undo());
}

//
// Note: These tests assume the following semantics:
//
// - When both generals are present and arranged normally (not facing each
// other),
//   the game is not over and no check is flagged.
// - If the generals are directly facing each other (with no intervening
// pieces),
//   then IsCheckMade() returns true (a flying general check), but the game is
//   not yet over.
// - If one general is missing (i.e. has been captured), then both IsCheckMade()
// and
//   IsGameOver() return true, and GetWinner() returns the surviving side.
//

//
// Test: Both generals are on board and not in a check configuration.
// Expected: No check, game not over, and no winner.
//
TEST(GameStatusTest, NoCheckNoGameOver) {
  Game game;
  // Place red general at (8,4) in the red palace and black general at (1,3) in
  // the black palace.
  game.Reset({{R_GENERAL, {8, 4}}, {B_GENERAL, {1, 3}}});
  EXPECT_FALSE(game.IsCheckMade());
  EXPECT_FALSE(game.IsGameOver());
  EXPECT_EQ(game.GetWinner(), std::nullopt);
}

//
// Test: Flying General check.
// Arrange the generals so they face each other in the same column with no
// blocking pieces. Expected: A check is detected, but the game is not over
// because both generals are still on board.
//
TEST(GameStatusTest, FlyingGeneralCheck) {
  Game game;
  // Place red general at (8,4) and black general at (1,4) so that they are in
  // the same column.
  game.Reset({{R_GENERAL, {8, 4}}, {B_GENERAL, {1, 4}}});
  EXPECT_TRUE(game.IsCheckMade());
  EXPECT_FALSE(game.IsGameOver());
  EXPECT_EQ(game.GetWinner(), std::nullopt);
}

//
// Test: Game over with red as the winner.
// Here, black's general is missing (captured), so the game should be over.
// Expected: Both IsCheckMade() and IsGameOver() return true and GetWinner()
// returns Winner::RED.
//
TEST(GameStatusTest, RedWinsGameOver) {
  Game game;
  // Only the red general is present in the red palace.
  game.Reset({{R_GENERAL, {8, 4}}});
  EXPECT_TRUE(game.IsCheckMade());
  EXPECT_TRUE(game.IsGameOver());
  std::optional<Winner> winner = game.GetWinner();
  ASSERT_TRUE(winner.has_value());
  EXPECT_EQ(winner.value(), Winner::RED);
}

//
// Test: Game over with black as the winner.
// Here, red's general is missing (captured), so the game should be over.
// Expected: Both IsCheckMade() and IsGameOver() return true and GetWinner()
// returns Winner::BLACK.
//
TEST(GameStatusTest, BlackWinsGameOver) {
  Game game;
  // Only the black general is present in the black palace.
  game.Reset({{B_GENERAL, {1, 4}}});
  EXPECT_TRUE(game.IsCheckMade());
  EXPECT_TRUE(game.IsGameOver());
  std::optional<Winner> winner = game.GetWinner();
  ASSERT_TRUE(winner.has_value());
  EXPECT_EQ(winner.value(), Winner::BLACK);
}

//
// Test 1: Standard full board setup (using many pieces) should not be in check
// or game over. This simulates a near-starting position that includes generals
// and all other piece types.
//
TEST(GameStatusComplexTest, FullBoardNormalPosition) {
  Game game;
  game.Reset({// Black pieces (top of the board)
              {B_CHARIOT_L, {0, 0}},
              {B_HORSE_L, {0, 1}},
              {B_ELEPHANT_L, {0, 2}},
              {B_ADVISOR_L, {0, 3}},
              {B_GENERAL, {0, 4}},
              {B_ADVISOR_R, {0, 5}},
              {B_ELEPHANT_R, {0, 6}},
              {B_HORSE_R, {0, 7}},
              {B_CHARIOT_R, {0, 8}},
              {B_CANNON_L, {2, 1}},
              {B_CANNON_R, {2, 7}},
              {B_SOLDIER_1, {3, 0}},
              {B_SOLDIER_2, {3, 2}},
              {B_SOLDIER_3, {3, 4}},
              {B_SOLDIER_4, {3, 6}},
              {B_SOLDIER_5, {3, 8}},
              // Red pieces (bottom of the board)
              {R_CHARIOT_L, {9, 0}},
              {R_HORSE_L, {9, 1}},
              {R_ELEPHANT_L, {9, 2}},
              {R_ADVISOR_L, {9, 3}},
              {R_GENERAL, {9, 4}},
              {R_ADVISOR_R, {9, 5}},
              {R_ELEPHANT_R, {9, 6}},
              {R_HORSE_R, {9, 7}},
              {R_CHARIOT_R, {9, 8}},
              {R_CANNON_L, {7, 1}},
              {R_CANNON_R, {7, 7}},
              {R_SOLDIER_1, {6, 0}},
              {R_SOLDIER_2, {6, 2}},
              {R_SOLDIER_3, {6, 4}},
              {R_SOLDIER_4, {6, 6}},
              {R_SOLDIER_5, {6, 8}}});
  EXPECT_FALSE(game.IsCheckMade());
  EXPECT_FALSE(game.IsGameOver());
  EXPECT_EQ(game.GetWinner(), std::nullopt);
}

//
// Test 2: Complex position with extra pieces where the generals are aligned
// (flying general check) even though many other pieces are present off the
// critical column. The extra pieces do not interfere with the flying check if
// they are off the generals’ column.
//
TEST(GameStatusComplexTest, FlyingGeneralCheckWithExtraPieces) {
  Game game;
  game.Reset({// Generals aligned in the same column (col 4)
              {R_GENERAL, {8, 4}},
              {B_GENERAL, {1, 4}},
              // Extra red pieces (placed off column 4)
              {R_ADVISOR_L, {8, 3}},
              {R_ELEPHANT_L, {8, 2}},
              {R_HORSE_L, {9, 1}},
              {R_CHARIOT_L, {9, 0}},
              {R_CANNON_L, {7, 1}},
              {R_SOLDIER_1, {6, 0}},
              // Extra black pieces (placed off column 4)
              {B_ADVISOR_L, {0, 3}},
              {B_ELEPHANT_L, {0, 2}},
              {B_HORSE_L, {0, 1}},
              {B_CHARIOT_L, {0, 0}},
              {B_CANNON_L, {2, 1}},
              {B_SOLDIER_1, {3, 0}}});
  // With no blocking piece in column 4 between row 1 and row 8,
  // the flying general check should be triggered.
  EXPECT_TRUE(game.IsCheckMade());
  EXPECT_FALSE(game.IsGameOver());
  EXPECT_EQ(game.GetWinner(), std::nullopt);
}

//
// Test 3: Complex position where extra pieces are present but a blocking piece
// prevents the flying general check from occurring.
// For example, place a red soldier directly in the path (col 4) between the
// generals.
//
TEST(GameStatusComplexTest, FlyingGeneralBlockedWithExtraPieces) {
  Game game;
  game.Reset({{R_GENERAL, {8, 4}},
              {B_GENERAL, {1, 4}},
              // Extra pieces off the main column.
              {R_ADVISOR_R, {8, 5}},
              {B_ADVISOR_R, {0, 5}},
              // Blocking piece on column 4 between the generals.
              {R_SOLDIER_1, {5, 4}},
              // Other extra pieces.
              {R_HORSE_R, {9, 7}},
              {B_HORSE_R, {0, 7}}});
  EXPECT_FALSE(game.IsCheckMade());
  EXPECT_FALSE(game.IsGameOver());
  EXPECT_EQ(game.GetWinner(), std::nullopt);
}

//
// Test 4: Complex board where black’s general is missing amid a field of
// pieces, so red wins. Extra pieces of various types are present on both sides.
//
TEST(GameStatusComplexTest, ComplexPositionGameOverRedWins) {
  Game game;
  game.Reset({// Red pieces
              {R_GENERAL, {9, 4}},
              {R_ADVISOR_L, {9, 3}},
              {R_ADVISOR_R, {9, 5}},
              {R_ELEPHANT_L, {9, 2}},
              {R_ELEPHANT_R, {9, 6}},
              {R_HORSE_L, {9, 1}},
              {R_HORSE_R, {9, 7}},
              {R_CHARIOT_L, {9, 0}},
              {R_CHARIOT_R, {9, 8}},
              {R_CANNON_L, {7, 1}},
              {R_CANNON_R, {7, 7}},
              {R_SOLDIER_1, {6, 0}},
              {R_SOLDIER_2, {6, 2}},
              {R_SOLDIER_3, {6, 4}},
              {R_SOLDIER_4, {6, 6}},
              {R_SOLDIER_5, {6, 8}},
              // Black pieces but NO general.
              {B_ADVISOR_L, {0, 3}},
              {B_ADVISOR_R, {0, 5}},
              {B_ELEPHANT_L, {0, 2}},
              {B_ELEPHANT_R, {0, 6}},
              {B_HORSE_L, {0, 1}},
              {B_HORSE_R, {0, 7}},
              {B_CHARIOT_L, {0, 0}},
              {B_CHARIOT_R, {0, 8}},
              {B_CANNON_L, {2, 1}},
              {B_CANNON_R, {2, 7}},
              {B_SOLDIER_1, {3, 0}},
              {B_SOLDIER_2, {3, 2}},
              {B_SOLDIER_3, {3, 4}},
              {B_SOLDIER_4, {3, 6}},
              {B_SOLDIER_5, {3, 8}}});
  EXPECT_TRUE(game.IsGameOver());
  EXPECT_TRUE(game.IsCheckMade());
  std::optional<Winner> winner = game.GetWinner();
  ASSERT_TRUE(winner.has_value());
  EXPECT_EQ(winner.value(), Winner::RED);
}

//
// Test 5: Complex board where red’s general is missing amid extra pieces,
// so black wins.
//
TEST(GameStatusComplexTest, ComplexPositionGameOverBlackWins) {
  Game game;
  game.Reset({// Black pieces
              {B_GENERAL, {0, 4}},
              {B_ADVISOR_L, {0, 3}},
              {B_ADVISOR_R, {0, 5}},
              {B_ELEPHANT_L, {0, 2}},
              {B_ELEPHANT_R, {0, 6}},
              {B_HORSE_L, {0, 1}},
              {B_HORSE_R, {0, 7}},
              {B_CHARIOT_L, {0, 0}},
              {B_CHARIOT_R, {0, 8}},
              {B_CANNON_L, {2, 1}},
              {B_CANNON_R, {2, 7}},
              {B_SOLDIER_1, {3, 0}},
              {B_SOLDIER_2, {3, 2}},
              {B_SOLDIER_3, {3, 4}},
              {B_SOLDIER_4, {3, 6}},
              {B_SOLDIER_5, {3, 8}},
              // Red pieces but NO general.
              {R_ADVISOR_L, {9, 3}},
              {R_ADVISOR_R, {9, 5}},
              {R_ELEPHANT_L, {9, 2}},
              {R_ELEPHANT_R, {9, 6}},
              {R_HORSE_L, {9, 1}},
              {R_HORSE_R, {9, 7}},
              {R_CHARIOT_L, {9, 0}},
              {R_CHARIOT_R, {9, 8}},
              {R_CANNON_L, {7, 1}},
              {R_CANNON_R, {7, 7}},
              {R_SOLDIER_1, {6, 0}},
              {R_SOLDIER_2, {6, 2}},
              {R_SOLDIER_3, {6, 4}},
              {R_SOLDIER_4, {6, 6}},
              {R_SOLDIER_5, {6, 8}}});
  EXPECT_TRUE(game.IsGameOver());
  EXPECT_TRUE(game.IsCheckMade());
  std::optional<Winner> winner = game.GetWinner();
  ASSERT_TRUE(winner.has_value());
  EXPECT_EQ(winner.value(), Winner::BLACK);
}

//
// Test 6: Complex board with extra pieces from both sides but with both
// generals missing, resulting in a draw.
//
TEST(GameStatusComplexTest, ComplexPositionBothGeneralsMissingDraw) {
  Game game;
  game.Reset({// Red extra pieces
              {R_ADVISOR_L, {9, 3}},
              {R_ADVISOR_R, {9, 5}},
              {R_ELEPHANT_L, {9, 2}},
              {R_ELEPHANT_R, {9, 6}},
              {R_HORSE_L, {9, 1}},
              {R_HORSE_R, {9, 7}},
              {R_CHARIOT_L, {9, 0}},
              {R_CHARIOT_R, {9, 8}},
              {R_CANNON_L, {7, 1}},
              {R_CANNON_R, {7, 7}},
              {R_SOLDIER_1, {6, 0}},
              {R_SOLDIER_2, {6, 2}},
              {R_SOLDIER_3, {6, 4}},
              {R_SOLDIER_4, {6, 6}},
              {R_SOLDIER_5, {6, 8}},
              // Black extra pieces
              {B_ADVISOR_L, {0, 3}},
              {B_ADVISOR_R, {0, 5}},
              {B_ELEPHANT_L, {0, 2}},
              {B_ELEPHANT_R, {0, 6}},
              {B_HORSE_L, {0, 1}},
              {B_HORSE_R, {0, 7}},
              {B_CHARIOT_L, {0, 0}},
              {B_CHARIOT_R, {0, 8}},
              {B_CANNON_L, {2, 1}},
              {B_CANNON_R, {2, 7}},
              {B_SOLDIER_1, {3, 0}},
              {B_SOLDIER_2, {3, 2}},
              {B_SOLDIER_3, {3, 4}},
              {B_SOLDIER_4, {3, 6}},
              {B_SOLDIER_5, {3, 8}}});
  EXPECT_TRUE(game.IsGameOver());
  EXPECT_TRUE(game.IsCheckMade());
  std::optional<Winner> winner = game.GetWinner();
  ASSERT_TRUE(winner.has_value());
  EXPECT_EQ(winner.value(), Winner::DRAW);
}

}  // namespace
