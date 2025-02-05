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

using namespace ::xq;
using enum ::xq::Piece;

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
// Test that calling MakeBlackMoveFirst() does not alter the board.
// ---------------------------------------------------------------------
TEST(GameTest, MakeBlackMoveFirstDoesNotAffectBoard) {
  Game game;
  Board<Piece> board_before = game.CurrentBoard();
  game.MakeBlackMoveFirst();
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
  Piece captured = game.Move({5, 5}, {4, 5});
  EXPECT_EQ(captured, EMPTY);
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
  Piece captured = game.Move({5, 5}, {5, 6});
  EXPECT_EQ(captured, B_SOLDIER_1);
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
  Piece captured = game.Move({4, 4}, {3, 4});
  EXPECT_EQ(captured, EMPTY);
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
  EXPECT_FALSE(game.CanUndo());
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

  game.Move({5, 5}, {4, 5});
  EXPECT_EQ(game.PieceAt({5, 5}), EMPTY);
  EXPECT_EQ(game.PieceAt({4, 5}), R_SOLDIER_1);

  // Undo the move.
  const MoveAction undo_action = game.Undo();
  const MoveAction expected{.piece = R_SOLDIER_1,
                            .from = Position{5, 5},
                            .to = Position{4, 5},
                            .captured = EMPTY};
  EXPECT_EQ(undo_action.piece, expected.piece);
  EXPECT_EQ(undo_action.from.row, expected.from.row);
  EXPECT_EQ(undo_action.from.col, expected.from.col);
  EXPECT_EQ(undo_action.to.row, expected.to.row);
  EXPECT_EQ(undo_action.to.col, expected.to.col);
  EXPECT_EQ(undo_action.captured, expected.captured);
  Board<Piece> board_after = game.CurrentBoard();
  EXPECT_EQ(board_before, board_after);
}

// ---------------------------------------------------------------------
// Test Undo() after multiple moves using a piece map reset.
// ---------------------------------------------------------------------
TEST(GameTest, UndoMultipleMoves) {
  Game game;
  std::unordered_map<Piece, Position> piece_map;
  piece_map[R_SOLDIER_3] = {6, 4};
  piece_map[B_SOLDIER_3] = {3, 4};

  game.Reset(std::move(piece_map));

  Board<Piece> initial_board = game.CurrentBoard();

  game.Move({6, 4}, {5, 4});
  game.Move({3, 4}, {4, 4});

  // Undo the second move.
  EXPECT_TRUE(game.CanUndo());
  game.Undo();
  EXPECT_EQ(game.PieceAt({5, 4}), R_SOLDIER_3);
  EXPECT_EQ(game.PieceAt({4, 4}), EMPTY);
  EXPECT_EQ(game.PieceAt({3, 4}), B_SOLDIER_3);

  EXPECT_TRUE(game.CanUndo());
  game.Undo();
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
  EXPECT_FALSE(game.CanUndo());
}

// ---------------------------------------------------------------------
// Basic scenarios with only generals on board.
TEST(IsCheckMadeTest, RedNotInCheck) {
  // Only both generals are on board and they are not in a “facing”
  // configuration.
  std::unordered_map<Piece, Position> board_setup = {
      {Piece::R_GENERAL, {9, 4}},
      {Piece::B_GENERAL, {0, 3}},
  };

  Game game;
  game.Reset(std::move(board_setup));
  EXPECT_FALSE(game.IsCheckMade());
}

TEST(IsCheckMadeTest, GeneralsFacingEachOther) {
  // In Xiangqi, having the two generals directly facing each other with no
  // intervening piece is illegal. Here we assume the engine flags that as
  // check.
  std::unordered_map<Piece, Position> board_setup = {
      {Piece::R_GENERAL, {9, 4}},
      {Piece::B_GENERAL, {0, 4}},
  };

  Game game;
  game.Reset(std::move(board_setup));
  EXPECT_TRUE(game.IsCheckMade());
}

// ---------------------------------------------------------------------
// Tests involving the Chariot.
TEST(IsCheckMadeTest, RedInCheckByChariot) {
  // Black chariot is vertically aligned with the red general with a clear path.
  std::unordered_map<Piece, Position> board_setup = {
      {Piece::R_GENERAL, {9, 4}},
      {Piece::B_GENERAL,
       {0,
        3}},  // placed away from column 4 to avoid interfering with other rules
      {Piece::B_CHARIOT_L, {5, 4}},
  };

  Game game;
  game.Reset(std::move(board_setup));
  EXPECT_TRUE(game.IsCheckMade());
}

TEST(IsCheckMadeTest, RedNotInCheckByChariotBlocked) {
  // Even though the black chariot is aligned, a friendly piece blocks its line
  // of sight.
  std::unordered_map<Piece, Position> board_setup = {
      {Piece::R_GENERAL, {9, 4}},
      {Piece::B_GENERAL, {0, 3}},
      {Piece::B_CHARIOT_L, {5, 4}},
      {Piece::R_SOLDIER_1, {7, 4}},  // blocks the chariot's path to the general
  };

  Game game;
  game.Reset(std::move(board_setup));
  EXPECT_FALSE(game.IsCheckMade());
}

// ---------------------------------------------------------------------
// Tests involving the Soldier.
TEST(IsCheckMadeTest, RedInCheckBySoldier) {
  // For red, an enemy (black) soldier directly in front (i.e. one row above)
  // can capture the general. (Black soldier moves downward.)
  std::unordered_map<Piece, Position> board_setup = {
      {Piece::R_GENERAL, {9, 4}},
      {Piece::B_GENERAL, {0, 3}},
      {Piece::B_SOLDIER_1, {8, 4}},
  };

  Game game;
  game.Reset(std::move(board_setup));
  EXPECT_TRUE(game.IsCheckMade());
}

// ---------------------------------------------------------------------
// Tests involving the Horse.
TEST(IsCheckMadeTest, RedInCheckByHorse) {
  // A black horse positioned so that its legal L-shaped move can capture the
  // red general. For example, from (7,3) the horse can move to (9,4) provided
  // the intermediate square (8,3) is empty.
  std::unordered_map<Piece, Position> board_setup = {
      {Piece::R_GENERAL, {9, 4}},
      {Piece::B_GENERAL, {0, 3}},
      {Piece::B_HORSE_L, {7, 3}},
  };

  Game game;
  game.Reset(std::move(board_setup));
  EXPECT_TRUE(game.IsCheckMade());
}

TEST(IsCheckMadeTest, RedNotInCheckByHorseWhenBlocked) {
  // The same configuration as above, but now a piece is placed in the horse's
  // "leg" at (8,3), which should block its move.
  std::unordered_map<Piece, Position> board_setup = {
      {Piece::R_GENERAL, {9, 4}},
      {Piece::B_GENERAL, {0, 3}},
      {Piece::B_HORSE_L, {7, 3}},
      {Piece::R_SOLDIER_1, {8, 3}},  // blocking the horse’s move
  };

  Game game;
  game.Reset(std::move(board_setup));
  EXPECT_FALSE(game.IsCheckMade());
}

// ---------------------------------------------------------------------
// Tests involving the Elephant.
TEST(IsCheckMadeTest, RedInCheckByElephant) {
  // In Xiangqi, the elephant moves exactly two points diagonally.
  // Here, a black elephant from (7,2) can reach the red general at (9,4) if the
  // intermediate square (8,3) is empty.
  std::unordered_map<Piece, Position> board_setup = {
      {Piece::R_GENERAL, {9, 4}},
      {Piece::B_GENERAL, {0, 3}},
      {Piece::B_ELEPHANT_L, {7, 2}},
  };

  Game game;
  game.Reset(std::move(board_setup));
  EXPECT_TRUE(game.IsCheckMade());
}

TEST(IsCheckMadeTest, RedNotInCheckByElephantBlocked) {
  // The same as above, but the intermediate square (8,3) is blocked.
  std::unordered_map<Piece, Position> board_setup = {
      {Piece::R_GENERAL, {9, 4}},
      {Piece::B_GENERAL, {0, 3}},
      {Piece::B_ELEPHANT_L, {7, 2}},
      {Piece::R_SOLDIER_1, {8, 3}},  // blocks the elephant’s diagonal move
  };

  Game game;
  game.Reset(std::move(board_setup));
  EXPECT_FALSE(game.IsCheckMade());
}

// ---------------------------------------------------------------------
// Tests involving the Cannon.
TEST(IsCheckMadeTest, RedInCheckByCannon) {
  // A cannon captures like a chariot but requires a single intervening piece (a
  // screen). Here, the black cannon at (7,4) can capture the red general at
  // (9,4) because a red soldier at (8,4) acts as a screen.
  std::unordered_map<Piece, Position> board_setup = {
      {Piece::R_GENERAL, {9, 4}},
      {Piece::B_GENERAL, {0, 3}},
      {Piece::B_CANNON_L, {7, 4}},
      {Piece::R_SOLDIER_1, {8, 4}},  // screen piece for the cannon
  };

  Game game;
  game.Reset(std::move(board_setup));
  EXPECT_TRUE(game.IsCheckMade());
}

TEST(IsCheckMadeTest, RedNotInCheckByCannonMissingScreen) {
  // Without any screen piece, the cannon cannot capture.
  std::unordered_map<Piece, Position> board_setup = {
      {Piece::R_GENERAL, {9, 4}},
      {Piece::B_GENERAL, {0, 3}},
      {Piece::B_CANNON_L, {7, 4}},
      // No piece at (8,4)
  };

  Game game;
  game.Reset(std::move(board_setup));
  EXPECT_FALSE(game.IsCheckMade());
}

// ---------------------------------------------------------------------
// Tests combining multiple enemy threats.
TEST(IsCheckMadeTest, RedInCheckByMultipleThreats) {
  // Several enemy pieces simultaneously threaten the red general.
  // - A black soldier directly in front at (8,4).
  // - A black horse from (7,3) (with its leg at (8,3) clear).
  // - A black cannon from (7,4) can also capture thanks to the soldier at (8,4)
  // acting as a screen.
  std::unordered_map<Piece, Position> board_setup = {
      {Piece::R_GENERAL, {9, 4}},   {Piece::B_GENERAL, {0, 3}},
      {Piece::B_SOLDIER_1, {8, 4}}, {Piece::B_HORSE_L, {7, 3}},
      {Piece::B_CANNON_L, {7, 4}},
  };

  Game game;
  game.Reset(std::move(board_setup));
  EXPECT_TRUE(game.IsCheckMade());
}

TEST(IsCheckMadeTest, BlackInCheckByMultipleThreats) {
  // Now test a scenario for black.
  // For black’s turn, the enemy red pieces are threatening the black general.
  // - Black general is at (0,4).
  // - A red soldier at (1,4) (assuming red soldier moves upward, from (1,4) to
  // (0,4)).
  // - A red horse from (2,3) can capture if its blocking square (1,3) is clear.
  // - A red cannon from (2,4) also threatens black general using the soldier at
  // (1,4) as a screen.
  std::unordered_map<Piece, Position> board_setup = {
      {Piece::B_GENERAL, {0, 4}},   {Piece::R_GENERAL, {9, 4}},
      {Piece::R_SOLDIER_1, {1, 4}}, {Piece::R_HORSE_L, {2, 3}},
      {Piece::R_CANNON_L, {2, 4}},
  };

  Game game;
  game.Reset(std::move(board_setup));
  game.MakeBlackMoveFirst();
  EXPECT_TRUE(game.IsCheckMade());
}

TEST(GameHistory, ExportAndRestoreMoves) {
  Game game;
  game.Move({7, 1}, {0, 1});
  game.Move({0, 0}, {0, 1});
  game.Move({7, 7}, {0, 7});
  game.Move({0, 8}, {0, 7});

  Game game2;
  game2.RestoreMoves(game.ExportMoves());
  for (uint8_t row = 0; row < kTotalRow; row++) {
    for (uint8_t col = 0; col < kTotalCol; col++) {
      EXPECT_EQ(game.PieceAt({row, col}), game2.PieceAt({row, col}));
    }
  }
}

}  // namespace
