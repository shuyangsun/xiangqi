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

namespace {

std::array<uint8_t, 32> UnpackEncoding(
    const std::array<uint64_t, 4>& encoding) {
  std::array<uint8_t, 32> bytes;
  for (size_t i = 0; i < 4; ++i) {
    uint64_t block = encoding[i];
    for (int j = 0; j < 8; ++j) {
      uint8_t byte = static_cast<uint8_t>(block >> (8 * (7 - j)));
      bytes[i * 8 + j] = byte;
    }
  }
  return bytes;
}

}  // namespace

// ---------------------------------------------------------------------
// Test that the default game board (constructed without a reset) is
// set up with the standard Xiangqi opening position.
// ---------------------------------------------------------------------
TEST(GameTest, InitialState) {
  Game game;
  EXPECT_EQ(game.PieceAt(Pos(9, 4)), R_GENERAL);
  EXPECT_EQ(game.PieceAt(Pos(0, 4)), B_GENERAL);
}

// ---------------------------------------------------------------------
// Test that the entire default board has the proper piece placements.
// (Standard Xiangqi layout: Black pieces on top, Red pieces at the bottom.)
// ---------------------------------------------------------------------
TEST(GameTest, DefaultBoardPieces) {
  Game game;

  // --- Black side ---
  // Row 0: major pieces.
  EXPECT_EQ(game.PieceAt(Pos(0, 0)), B_CHARIOT);
  EXPECT_EQ(game.PieceAt(Pos(0, 1)), B_HORSE);
  EXPECT_EQ(game.PieceAt(Pos(0, 2)), B_ELEPHANT);
  EXPECT_EQ(game.PieceAt(Pos(0, 3)), B_ADVISOR);
  EXPECT_EQ(game.PieceAt(Pos(0, 4)), B_GENERAL);
  EXPECT_EQ(game.PieceAt(Pos(0, 5)), B_ADVISOR);
  EXPECT_EQ(game.PieceAt(Pos(0, 6)), B_ELEPHANT);
  EXPECT_EQ(game.PieceAt(Pos(0, 7)), B_HORSE);
  EXPECT_EQ(game.PieceAt(Pos(0, 8)), B_CHARIOT);

  // Row 2: Cannons.
  EXPECT_EQ(game.PieceAt(Pos(2, 1)), B_CANNON);
  EXPECT_EQ(game.PieceAt(Pos(2, 7)), B_CANNON);

  // Row 3: Soldiers.
  EXPECT_EQ(game.PieceAt(Pos(3, 0)), B_SOLDIER);
  EXPECT_EQ(game.PieceAt(Pos(3, 2)), B_SOLDIER);
  EXPECT_EQ(game.PieceAt(Pos(3, 4)), B_SOLDIER);
  EXPECT_EQ(game.PieceAt(Pos(3, 6)), B_SOLDIER);
  EXPECT_EQ(game.PieceAt(Pos(3, 8)), B_SOLDIER);

  // --- Red side ---
  // Row 9: major pieces.
  EXPECT_EQ(game.PieceAt(Pos(9, 0)), R_CHARIOT);
  EXPECT_EQ(game.PieceAt(Pos(9, 1)), R_HORSE);
  EXPECT_EQ(game.PieceAt(Pos(9, 2)), R_ELEPHANT);
  EXPECT_EQ(game.PieceAt(Pos(9, 3)), R_ADVISOR);
  EXPECT_EQ(game.PieceAt(Pos(9, 4)), R_GENERAL);
  EXPECT_EQ(game.PieceAt(Pos(9, 5)), R_ADVISOR);
  EXPECT_EQ(game.PieceAt(Pos(9, 6)), R_ELEPHANT);
  EXPECT_EQ(game.PieceAt(Pos(9, 7)), R_HORSE);
  EXPECT_EQ(game.PieceAt(Pos(9, 8)), R_CHARIOT);

  // Row 7: Cannons.
  EXPECT_EQ(game.PieceAt(Pos(7, 1)), R_CANNON);
  EXPECT_EQ(game.PieceAt(Pos(7, 7)), R_CANNON);

  // Row 6: Soldiers.
  EXPECT_EQ(game.PieceAt(Pos(6, 0)), R_SOLDIER);
  EXPECT_EQ(game.PieceAt(Pos(6, 2)), R_SOLDIER);
  EXPECT_EQ(game.PieceAt(Pos(6, 4)), R_SOLDIER);
  EXPECT_EQ(game.PieceAt(Pos(6, 6)), R_SOLDIER);
  EXPECT_EQ(game.PieceAt(Pos(6, 8)), R_SOLDIER);
}

// ---------------------------------------------------------------------
// Test ResetFromPos() using an unordered_map (custom configuration) by
// providing a piece map and verifying that only those pieces appear in
// the expected positions.
// ---------------------------------------------------------------------
TEST(GameTest, ResetWithCustomPieceMap) {
  Game game;
  game.ResetFromPos({
      {Pos(5, 5), R_GENERAL},
      {Pos(4, 4), B_GENERAL},
      {Pos(0, 0), R_SOLDIER},
  });
  Board<Piece> current_board = game.CurrentBoard();

  EXPECT_EQ(current_board[5][5], R_GENERAL);
  EXPECT_EQ(current_board[4][4], B_GENERAL);
  EXPECT_EQ(current_board[0][0], R_SOLDIER);
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
// Test ResetFromPos() with another piece map configuration.
// ---------------------------------------------------------------------
TEST(GameTest, ResetWithPieceMap) {
  Game game;
  game.ResetFromPos({
      {Pos(5, 5), R_GENERAL},
      {Pos(4, 4), B_GENERAL},
      {Pos(8, 0), R_CHARIOT},
  });
  Board<Piece> current_board = game.CurrentBoard();

  EXPECT_EQ(current_board[5][5], R_GENERAL);
  EXPECT_EQ(current_board[4][4], B_GENERAL);
  EXPECT_EQ(current_board[8][0], R_CHARIOT);
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
  game.ResetFromPos({{Pos(2, 3), B_GENERAL}});
  EXPECT_EQ(game.PieceAt(Pos(2, 3)), B_GENERAL);
  EXPECT_EQ(game.PieceAt(Pos(0, 0)), EMPTY);
}

// ---------------------------------------------------------------------
// Test Move() for a non-capturing move using a piece map to set the board.
// ---------------------------------------------------------------------
TEST(GameTest, MoveNonCapture) {
  Game game;
  game.ResetFromPos({{Pos(5, 5), R_SOLDIER}});

  // Move red soldier from (5,5) to an empty square (4,5).
  Piece captured = game.Move(Pos(5, 5), Pos(4, 5));
  EXPECT_EQ(captured, EMPTY);
  EXPECT_EQ(game.PieceAt(Pos(5, 5)), EMPTY);
  EXPECT_EQ(game.PieceAt(Pos(4, 5)), R_SOLDIER);
}

// ---------------------------------------------------------------------
// Test Move() for a capturing move using a piece map to set the board.
// ---------------------------------------------------------------------
TEST(GameTest, MoveCapture) {
  Game game;
  game.ResetFromPos({
      {Pos(5, 5), R_SOLDIER},
      {Pos(5, 6), B_SOLDIER},
  });

  // Move red soldier from (5,5) to (5,6), capturing the black soldier.
  Piece captured = game.Move(Pos(5, 5), Pos(5, 6));
  EXPECT_EQ(captured, B_SOLDIER);
  EXPECT_EQ(game.PieceAt(Pos(5, 5)), EMPTY);
  EXPECT_EQ(game.PieceAt(Pos(5, 6)), R_SOLDIER);
}

// ---------------------------------------------------------------------
// Test Move() when attempting to move from an empty square. We set the
// board to be completely empty via an empty piece map.
// ---------------------------------------------------------------------
TEST(GameTest, MoveFromEmpty) {
  Game game;
  std::unordered_map<Position, Piece> empty_map;  // No pieces set.
  game.ResetFromPos(std::move(empty_map));

  // Attempt to move from an empty square.
  Piece captured = game.Move(Pos(4, 4), Pos(3, 4));
  EXPECT_EQ(captured, EMPTY);
  EXPECT_EQ(game.PieceAt(Pos(4, 4)), EMPTY);
  EXPECT_EQ(game.PieceAt(Pos(3, 4)), EMPTY);
}

// ---------------------------------------------------------------------
// Test that CurrentBoard() returns a copy of the board rather than a
// reference to the internal state.
// ---------------------------------------------------------------------
TEST(GameTest, CurrentBoardReturnsCopy) {
  Game game;
  Board<Piece> board_copy = game.CurrentBoard();
  // Make a move in the game. (Using the default board configuration.)
  game.Move(Pos(9, 4), Pos(8, 4));  // Move the red general.
  // The copy should not reflect the move.
  EXPECT_EQ(board_copy[9][4], R_GENERAL);
  EXPECT_EQ(board_copy[8][4], EMPTY);
  // The current board should show the change.
  EXPECT_EQ(game.PieceAt(Pos(9, 4)), EMPTY);
  EXPECT_EQ(game.PieceAt(Pos(8, 4)), R_GENERAL);
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
  game.ResetFromPos({{Pos(5, 5), R_SOLDIER}});

  Board<Piece> board_before = game.CurrentBoard();

  game.Move(Pos(5, 5), Pos(4, 5));
  EXPECT_EQ(game.PieceAt(Pos(5, 5)), EMPTY);
  EXPECT_EQ(game.PieceAt(Pos(4, 5)), R_SOLDIER);

  // Undo the move.
  const MoveAction undo_action = game.Undo();
  const MoveAction expected{.piece = R_SOLDIER,
                            .from = Pos(5, 5),
                            .to = Pos(4, 5),
                            .captured = EMPTY};
  EXPECT_EQ(undo_action.piece, expected.piece);
  EXPECT_EQ(undo_action.from, expected.from);
  EXPECT_EQ(undo_action.to, expected.to);
  EXPECT_EQ(undo_action.captured, expected.captured);
  Board<Piece> board_after = game.CurrentBoard();
  EXPECT_EQ(board_before, board_after);
}

// ---------------------------------------------------------------------
// Test Undo() after multiple moves using a piece map reset.
// ---------------------------------------------------------------------
TEST(GameTest, UndoMultipleMoves) {
  Game game;
  game.ResetFromPos({
      {Pos(6, 4), R_SOLDIER},
      {Pos(3, 4), B_SOLDIER},
  });

  Board<Piece> initial_board = game.CurrentBoard();

  game.Move(Pos(6, 4), Pos(5, 4));
  game.Move(Pos(3, 4), Pos(4, 4));

  // Undo the second move.
  EXPECT_TRUE(game.CanUndo());
  game.Undo();
  EXPECT_EQ(game.PieceAt(Pos(5, 4)), R_SOLDIER);
  EXPECT_EQ(game.PieceAt(Pos(4, 4)), EMPTY);
  EXPECT_EQ(game.PieceAt(Pos(3, 4)), B_SOLDIER);

  EXPECT_TRUE(game.CanUndo());
  game.Undo();
  Board<Piece> board_after = game.CurrentBoard();
  EXPECT_EQ(initial_board, board_after);
}

// ---------------------------------------------------------------------
// Test that calling ResetFromPos() clears the move history.
// After a Reset, Undo() should not succeed.
// ---------------------------------------------------------------------
TEST(GameTest, ResetClearsHistory) {
  Game game;
  game.ResetFromPos({{Pos(5, 5), R_SOLDIER}});

  // Make a move.
  game.Move(Pos(5, 5), Pos(4, 5));
  // Now reset to the default board.
  game.Reset();
  // Undo should now fail because history has been cleared.
  EXPECT_FALSE(game.CanUndo());
}

// ---------------------------------------------------------------------
// Basic scenarios with only generals on board.
TEST(IsCheckMadeTest, RedNotInCheck) {
  Game game;
  game.ResetFromPos({
      {Pos(9, 4), R_GENERAL},
      {Pos(0, 3), B_GENERAL},
  });
  EXPECT_FALSE(game.IsCheckMade());
}

TEST(IsCheckMadeTest, GeneralsFacingEachOther) {
  // In Xiangqi, having the two generals directly facing each other with no
  // intervening piece is illegal. Here we assume the engine flags that as
  // check.
  std::unordered_map<Piece, Position> board_setup = {};

  Game game;
  game.ResetFromPos({
      {Pos(9, 4), R_GENERAL},
      {Pos(0, 4), B_GENERAL},
  });
  EXPECT_TRUE(game.IsCheckMade());
}

// ---------------------------------------------------------------------
// Tests involving the Chariot.
TEST(IsCheckMadeTest, RedInCheckByChariot) {
  Game game;
  // Black chariot is vertically aligned with the red general with a clear
  game.ResetFromPos({
      // path.
      {Pos(9, 4), R_GENERAL},
      {Pos(0, 3), B_GENERAL},  // placed away from column 4 to avoid interfering
                               // with other rules
      {Pos(5, 4), B_CHARIOT},

  });
  EXPECT_TRUE(game.IsCheckMade());
}

TEST(IsCheckMadeTest, RedNotInCheckByChariotBlocked) {
  Game game;
  // Even though the black chariot is aligned, a friendly piece blocks its line
  // of sight.
  game.ResetFromPos({
      {Pos(9, 4), R_GENERAL},
      {Pos(0, 3), B_GENERAL},
      {Pos(5, 4), B_CHARIOT},
      {Pos(7, 4), R_SOLDIER},  // blocks the chariot's path to the general

  });
  EXPECT_FALSE(game.IsCheckMade());
}

// ---------------------------------------------------------------------
// Tests involving the Soldier.
TEST(IsCheckMadeTest, RedInCheckBySoldier) {
  // For red, an enemy (black) soldier directly in front (i.e. one row above)
  // can capture the general. (Black soldier moves downward.)
  Game game;
  game.ResetFromPos({
      {Pos(9, 4), R_GENERAL},
      {Pos(0, 3), B_GENERAL},
      {Pos(8, 4), B_SOLDIER},
  });
  EXPECT_TRUE(game.IsCheckMade());
}

// ---------------------------------------------------------------------
// Tests involving the Horse.
TEST(IsCheckMadeTest, RedInCheckByHorse) {
  // A black horse positioned so that its legal L-shaped move can capture the
  // red general. For example, from (7,3) the horse can move to (9,4) provided
  // the intermediate square (8,3) is empty.
  Game game;
  game.ResetFromPos({
      {Pos(9, 4), R_GENERAL},
      {Pos(0, 3), B_GENERAL},
      {Pos(7, 3), B_HORSE},
  });
  EXPECT_TRUE(game.IsCheckMade());
}

TEST(IsCheckMadeTest, RedNotInCheckByHorseWhenBlocked) {
  // The same configuration as above, but now a piece is placed in the horse's
  // "leg" at (8,3), which should block its move.
  Game game;
  game.ResetFromPos({
      {Pos(9, 4), R_GENERAL},
      {Pos(0, 3), B_GENERAL},
      {Pos(7, 3), B_HORSE},
      {Pos(8, 3), R_SOLDIER},  // blocking the horse’s move
  });
  EXPECT_FALSE(game.IsCheckMade());
}

// ---------------------------------------------------------------------
// Tests involving the Elephant.
TEST(IsCheckMadeTest, RedInCheckByElephant) {
  // In Xiangqi, the elephant moves exactly two points diagonally.
  // Here, a black elephant from (7,2) can reach the red general at (9,4) if the
  // intermediate square (8,3) is empty.
  Game game;
  game.ResetFromPos({
      {Pos(9, 4), R_GENERAL},
      {Pos(0, 3), B_GENERAL},
      {Pos(7, 2), B_ELEPHANT},
  });
  EXPECT_TRUE(game.IsCheckMade());
}

TEST(IsCheckMadeTest, RedNotInCheckByElephantBlocked) {
  // The same as above, but the intermediate square (8,3) is blocked.
  Game game;
  game.ResetFromPos({
      {Pos(9, 4), R_GENERAL},
      {Pos(0, 3), B_GENERAL},
      {Pos(7, 2), B_ELEPHANT},
      {Pos(8, 3), R_SOLDIER},  // blocks the elephant’s diagonal move
  });
  EXPECT_FALSE(game.IsCheckMade());
}

// ---------------------------------------------------------------------
// Tests involving the Cannon.
TEST(IsCheckMadeTest, RedInCheckByCannon) {
  // A cannon captures like a chariot but requires a single intervening piece (a
  // screen). Here, the black cannon at (7,4) can capture the red general at
  // (9,4) because a red soldier at (8,4) acts as a screen.
  Game game;
  game.ResetFromPos({
      {Pos(9, 4), R_GENERAL},
      {Pos(0, 3), B_GENERAL},
      {Pos(7, 4), B_CANNON},
      {Pos(8, 4), R_SOLDIER},  // screen piece for the cannon
  });
  EXPECT_TRUE(game.IsCheckMade());
}

TEST(IsCheckMadeTest, RedNotInCheckByCannonMissingScreen) {
  // Without any screen piece, the cannon cannot capture.
  Game game;
  game.ResetFromPos({
      {Pos(9, 4), R_GENERAL}, {Pos(0, 3), B_GENERAL}, {Pos(7, 4), B_CANNON}
      // No piece at (8,4)
  });
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
  Game game;
  game.ResetFromPos({
      {Pos(9, 4), R_GENERAL},
      {Pos(0, 3), B_GENERAL},
      {Pos(8, 4), B_SOLDIER},
      {Pos(7, 3), B_HORSE},
      {Pos(7, 4), B_CANNON},

  });
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
  Game game;
  game.ResetFromPos({
      {Pos(0, 4), B_GENERAL},
      {Pos(9, 4), R_GENERAL},
      {Pos(1, 4), R_SOLDIER},
      {Pos(2, 3), R_HORSE},
      {Pos(2, 4), R_CANNON},
  });
  game.MakeBlackMoveFirst();
  EXPECT_TRUE(game.IsCheckMade());
}

TEST(GameHistory, ExportAndRestoreMoves) {
  Game game;
  game.Move(Pos(7, 1), Pos(0, 1));
  game.Move(Pos(0, 0), Pos(0, 1));
  game.Move(Pos(7, 7), Pos(0, 7));
  game.Move(Pos(0, 8), Pos(0, 7));

  Game game2;
  game2.RestoreMoves(game.ExportMoves());
  for (uint8_t row = 0; row < kTotalRow; row++) {
    for (uint8_t col = 0; col < kTotalCol; col++) {
      EXPECT_EQ(game.PieceAt(Pos(row, col)), game2.PieceAt(Pos(row, col)));
    }
  }
}

// ---------------------------------------------------------------------
// Test that flipping an empty board results in an empty board.
// ---------------------------------------------------------------------
TEST(FlipBoardTest, EmptyBoard) {
  Board<Piece> board;
  // Initialize board to be completely EMPTY.
  for (auto& row : board) {
    row.fill(EMPTY);
  }
  Board<Piece> flipped = FlipBoard(board);

  for (uint8_t r = 0; r < kTotalRow; ++r) {
    for (uint8_t c = 0; c < kTotalCol; ++c) {
      EXPECT_EQ(flipped[r][c], EMPTY) << "Expected cell (" << unsigned(r)
                                      << ", " << unsigned(c) << ") to be EMPTY";
    }
  }
}

// ---------------------------------------------------------------------
// Test that a board with a single piece is correctly flipped.
// The piece’s position should be rotated 180° and its sign flipped.
// For example, placing R_GENERAL (value +1) should become B_GENERAL (-1).
// ---------------------------------------------------------------------
TEST(FlipBoardTest, SinglePiece) {
  Board<Piece> board;
  for (auto& row : board) {
    row.fill(EMPTY);
  }
  // Place a red general at position (2,3)
  board[2][3] = R_GENERAL;
  Board<Piece> flipped = FlipBoard(board);

  // New row = kTotalRow - 1 - 2, new col = kTotalCol - 1 - 3.
  uint8_t newRow = kTotalRow - 1 - 2;  // 10 - 1 - 2 = 7
  uint8_t newCol = kTotalCol - 1 - 3;  // 9 - 1 - 3 = 5

  // Since R_GENERAL is positive, after flipping it should become -R_GENERAL,
  // i.e. B_GENERAL.
  EXPECT_EQ(flipped[newRow][newCol], B_GENERAL);
  // Also, ensure that no other cell accidentally contains a piece.
  EXPECT_EQ(flipped[2][3], EMPTY);
}

// ---------------------------------------------------------------------
// Test that flipping a board twice returns the original board.
// ---------------------------------------------------------------------
TEST(FlipBoardTest, DoubleFlipReturnsOriginal) {
  Board<Piece> board;
  for (auto& row : board) {
    row.fill(EMPTY);
  }
  // Set up a few pieces.
  board[1][2] = R_HORSE;    // R_HORSE has a positive value.
  board[8][3] = B_CANNON;   // B_CANNON is negative.
  board[4][4] = R_SOLDIER;  // R_SOLDIER is positive.

  Board<Piece> flipped = FlipBoard(board);
  Board<Piece> doubleFlipped = FlipBoard(flipped);

  // Flipping twice should return the board to its original configuration.
  EXPECT_EQ(doubleFlipped, board);
}

// ---------------------------------------------------------------------
// Test a board with multiple pieces placed in known locations.
// Verify that each piece is moved to the rotated position and its value
// (if non-zero) is negated.
// ---------------------------------------------------------------------
TEST(FlipBoardTest, MultiplePieces) {
  Board<Piece> board;
  for (auto& row : board) {
    row.fill(EMPTY);
  }
  // Place several pieces.
  // 1. Place a red soldier at (0,0). R_SOLDIER is defined as 12.
  board[0][0] = R_SOLDIER;
  // 2. Place a black horse at (5,4). B_HORSE is defined as -6.
  board[5][4] = B_HORSE;
  // 3. Place a black chariot at (9,8). B_CHARIOT is defined as -9.
  board[9][8] = B_CHARIOT;

  Board<Piece> flipped = FlipBoard(board);

  // For (0,0) = R_SOLDIER, the new location is:
  // row: kTotalRow - 1 - 0 = 9, col: kTotalCol - 1 - 0 = 8.
  // And R_SOLDIER (12) should become -12, i.e. B_SOLDIER.
  EXPECT_EQ(flipped[9][8], B_SOLDIER);

  // For (5,4) = B_HORSE, the new location is:
  // row: 10 - 1 - 5 = 4, col: 9 - 1 - 4 = 4.
  // And B_HORSE (-6) should become 6, i.e. R_HORSE.
  EXPECT_EQ(flipped[4][4], R_HORSE);

  // For (9,8) = B_CHARIOT, the new location is:
  // row: 10 - 1 - 9 = 0, col: 9 - 1 - 8 = 0.
  // And B_CHARIOT (-9) should become 9, i.e. R_CHARIOT.
  EXPECT_EQ(flipped[0][0], R_CHARIOT);
}

// ---------------------------------------------------------------------
// Test flipping the default board from a Game instance.
// Verify that pieces originally at the bottom appear at the top with
// their sign reversed, and vice versa.
// ---------------------------------------------------------------------
TEST(FlipBoardTest, FlipDefaultBoard) {
  Game game;
  Board<Piece> defaultBoard = game.CurrentBoard();
  Board<Piece> flipped = FlipBoard(defaultBoard);

  // In the default board (kInitState), the red general (R_GENERAL) is at (9,4)
  // and the black general (B_GENERAL) is at (0,4).
  // After flipping:
  //   - The red general should move to (0,4) and become B_GENERAL.
  //   - The black general should move to (9,4) and become R_GENERAL.
  EXPECT_EQ(flipped[0][4], B_GENERAL);
  EXPECT_EQ(flipped[9][4], R_GENERAL);

  // Similarly, check one more pair:
  // The default board has B_CHARIOT at (0,0), so after flipping it should
  // appear at (9,8) as R_CHARIOT.
  EXPECT_EQ(flipped[9][8], R_CHARIOT);
}

// ---------------------------------------------------------------------
// EncodeBoardStateTest: Tests for encoding and decoding board states.
// ---------------------------------------------------------------------

// Test 1: When the board is completely empty, every piece should be missing,
// so every byte in the 32-byte encoding is 0xFF. Also, decoding should yield an
// empty board.
TEST(EncodeBoardStateTest, EmptyBoard) {
  Board<Piece> board;
  for (auto& row : board) {
    row.fill(Piece::EMPTY);
  }
  auto encoding = EncodeBoardState(board);
  auto bytes = UnpackEncoding(encoding);
  ASSERT_EQ(bytes.size(), 32u);
  for (size_t i = 0; i < bytes.size(); ++i) {
    EXPECT_EQ(bytes[i], 0xFF) << "Byte at index " << i << " should be 0xFF.";
  }
  // Test decoding: the decoded board should be completely empty.
  Board<Piece> decoded = DecodeBoardState(encoding);
  for (uint8_t r = 0; r < kTotalRow; ++r) {
    for (uint8_t c = 0; c < kTotalCol; ++c) {
      EXPECT_EQ(decoded[r][c], Piece::EMPTY)
          << "Decoded board cell (" << unsigned(r) << ", " << unsigned(c)
          << ") should be EMPTY.";
    }
  }
}

// Test 2: A board with a single red general at (3,4) should yield an encoding
// where the red general group (group 0) contains its encoded position and every
// other byte is 0xFF. Also, decoding should yield a board with only R_GENERAL
// at (3,4).
TEST(EncodeBoardStateTest, OnlyRedGeneral) {
  Board<Piece> board;
  for (auto& row : board) {
    row.fill(Piece::EMPTY);
  }
  // Place red general at row=3, col=4.
  board[3][4] = Piece::R_GENERAL;
  auto encoding = EncodeBoardState(board);
  auto bytes = UnpackEncoding(encoding);
  ASSERT_EQ(bytes.size(), 32u);
  // Group 0 (red general) is expected at byte index 0.
  EXPECT_EQ(bytes[0], 0x34)
      << "Expected red general encoded as (3<<4)|4 = 0x34 in group 0.";
  // All other 31 bytes should be 0xFF.
  for (size_t i = 1; i < bytes.size(); ++i) {
    EXPECT_EQ(bytes[i], 0xFF) << "Byte at index " << i << " should be 0xFF.";
  }
  // Test decoding: the decoded board should have R_GENERAL at (3,4) and nothing
  // else.
  Board<Piece> decoded = DecodeBoardState(encoding);
  for (uint8_t r = 0; r < kTotalRow; ++r) {
    for (uint8_t c = 0; c < kTotalCol; ++c) {
      if (r == 3 && c == 4) {
        EXPECT_EQ(decoded[r][c], Piece::R_GENERAL);
      } else {
        EXPECT_EQ(decoded[r][c], Piece::EMPTY);
      }
    }
  }
}

// Test 3: Test that pieces in the same group are sorted canonically.
// For example, if we place two red advisors at (5,5) and (2,3) (in unsorted
// order), then the advisor group (group 1) should be sorted by their encoded
// value. Decoding should yield both advisors as R_ADVISOR.
TEST(EncodeBoardStateTest, OnlyRedAdvisorsSorted) {
  Board<Piece> board;
  for (auto& row : board) {
    row.fill(Piece::EMPTY);
  }
  // For red advisors (group 1, expected 2 bytes), place:
  // - R_ADVISOR at (5,5): encoded as (5<<4)|5 = 0x55.
  // - R_ADVISOR at (2,3): encoded as (2<<4)|3 = 0x23.
  board[5][5] = Piece::R_ADVISOR;
  board[2][3] = Piece::R_ADVISOR;
  const std::array<uint64_t, 4> encoding = EncodeBoardState(board);
  auto bytes = UnpackEncoding(encoding);
  ASSERT_EQ(bytes.size(), 32u);
  // The groups appear in the following order:
  // Group 0: red general (1 byte) → index 0.
  // Group 1: red advisors (2 bytes) → indices 1-2.
  // All other groups (indices 3 to 31) are missing and padded with 0xFF.
  EXPECT_EQ(bytes[0], 0xFF)
      << "Red general missing; group 0 should be padded with 0xFF.";
  // The two advisor bytes should be sorted: 0x23 comes before 0x55.
  EXPECT_EQ(bytes[1], 0x23);
  EXPECT_EQ(bytes[2], 0x55);
  for (size_t i = 3; i < bytes.size(); ++i) {
    EXPECT_EQ(bytes[i], 0xFF) << "Byte at index " << i << " should be 0xFF.";
  }
  // Test decoding: both advisors should appear as R_ADVISOR in their
  // positions.
  Board<Piece> decoded = DecodeBoardState(encoding);
  for (uint8_t r = 0; r < kTotalRow; ++r) {
    for (uint8_t c = 0; c < kTotalCol; ++c) {
      if ((r == 2 && c == 3) || (r == 5 && c == 5)) {
        EXPECT_EQ(decoded[r][c], Piece::R_ADVISOR);
      } else {
        EXPECT_EQ(decoded[r][c], Piece::EMPTY);
      }
    }
  }
}

// Test 4: Repeated calls to EncodeBoardState with the same board should produce
// identical encodings. Also, decoding the encoding and then re-encoding the
// decoded board should yield the same canonical encoding.
TEST(EncodeBoardStateTest, ConsistencyMultipleCalls) {
  Board<Piece> board;
  for (auto& row : board) {
    row.fill(Piece::EMPTY);
  }
  // Place a few pieces.
  board[0][0] = Piece::R_CHARIOT;
  board[9][8] = Piece::R_CHARIOT;
  board[0][4] = Piece::B_GENERAL;
  board[9][4] = Piece::R_GENERAL;
  auto encoding1 = EncodeBoardState(board);
  auto encoding2 = EncodeBoardState(board);
  EXPECT_EQ(encoding1, encoding2)
      << "Multiple calls to EncodeBoardState on the same board should produce "
         "the same encoding.";

  // Test decoding consistency: decode encoding1 and re-encode the decoded
  // board.
  Board<Piece> decoded = DecodeBoardState(encoding1);
  auto reencoded = EncodeBoardState(decoded);
  EXPECT_EQ(encoding1, reencoded) << "Re-encoding the decoded board should "
                                     "yield the same canonical encoding.";
}

// Test 5: Use the Game’s default opening board and verify that the encoded
// state reflects that both generals are present (i.e. not padded as missing).
// Also, decoding should yield a board with the generals in their proper
// positions.
TEST(EncodeBoardStateTest, DefaultBoardState) {
  Game game;
  Board<Piece> board = game.CurrentBoard();
  auto encoding = EncodeBoardState(board);
  auto bytes = UnpackEncoding(encoding);
  ASSERT_EQ(bytes.size(), 32u);
  EXPECT_NE(bytes[0], 0xFF) << "Red general should be present (group 0).";
  EXPECT_NE(bytes[16], 0xFF) << "Black general should be present (group 7).";

  // Test decoding: verify that the decoded board has R_GENERAL and B_GENERAL in
  // the expected positions.
  Board<Piece> decoded = DecodeBoardState(encoding);
  for (uint8_t row = 0; row < kTotalRow; row++) {
    for (uint8_t col = 0; row < kTotalCol; row++) {
      ASSERT_EQ(decoded[row][col], board[row][col])
          << "Decoded default board at (" << static_cast<int>(row) << ", "
          << static_cast<int>(col) << ") is not equal.";
    }
  }
}

TEST(TestFlipPosition, Cannon1) {
  const Position position = 0x71;
  const Position flipped = FlipPosition(position);
  const Position flipped_twice = FlipPosition(flipped);

  EXPECT_EQ(flipped, 0x27);
  EXPECT_EQ(flipped_twice, position);
}

}  // namespace
