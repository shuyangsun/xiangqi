// file: test_game.cc

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "xiangqi/game.h"
#include "xiangqi/types.h"

using namespace xiangqi;

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
