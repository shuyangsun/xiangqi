#ifndef XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_GAME_H__
#define XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_GAME_H__

#include <unordered_map>
#include <vector>

#include "xiangqi/types.h"

namespace xq {
class Game {
 public:
  // Initialize an empty game with the default Xiangqi opening board.
  // Red represents self and black represents the opponent.
  Game();
  ~Game() = default;

  // Reset the board with default Xiangqi openning board.
  void Reset();
  // Reset the game with a given board position.
  void ResetFromBoard(Board<Piece>&& board);
  // Reset the game with a map of pieces to their positions.
  void ResetFromPos(std::unordered_map<Piece, Position>&& piece_pos);

  // Returns true if it is red's turn, false if black's.
  Player Turn() const;

  // Returns the number of moves has been made in this game.
  size_t MovesCount() const;

  // Change the starting turn to Black.
  void MakeBlackMoveFirst();

  // Get a copy of the current board.
  Board<Piece> CurrentBoard() const;

  // Get the piece at a speific position of the current board.
  Piece PieceAt(Position pos) const;

  // Get all possible moves of a position. Returns a 10x9 boolean board, with
  // possible locations marked as true. If there is no piece at the position,
  // returns a board with all false values.
  Board<bool> PossibleMoves(Position pos) const;

  // Move a piece from a position to another position, returns the captured
  // piece.
  Piece Move(Position from, Position to);

  // Whether or not the player can perform undo action.
  bool CanUndo() const;

  // Undo last move. Returns the move action that was reversed. Cannot undo
  // if the board is already at its original state when initialized or reset.
  MoveAction Undo();

  // Returns true if it's checkmade for the current player. I.e., if the
  // current player does not take certain actions, the opponent can capture
  // the general with their next move.
  bool IsCheckMade() const;

  // Returns true if the game is over. For example, one of the generals has
  // been captured, or if there is no possible move for the current player's
  // general to not be captured.
  bool IsGameOver() const;

  // Returns the winner if the game is over. Returns NONE if the game is not
  // over.
  Winner GetWinner() const;

  // Export moves as vector of uint16_t. The integer represents four 4-bit
  // integers as from.row, from.col, to.row and to.col.
  std::vector<uint16_t> ExportMoves() const;

  // Restores game state from exported moves.
  void RestoreMoves(const std::vector<uint16_t>& moves);

 private:
  Player player_ = Player::RED;
  std::vector<Board<Piece>> history_;
  std::vector<MoveAction> moves_;
};

// Rotate the board so that it's from the opponent's perspective.
Board<Piece> FlipBoard(const Board<Piece>& board);

// Encode the board state using a small number of bytes, mainly used for the
// game AI to identify a unique board state. The board state does not
// distinguish the same piece for the same player, i.e., R_ADVISOR_1 is the same
// as R_ADVISOR_2.
//
// There are a total of 32 pieces on the board, so the encoded board state is
// 32 bytes, with each byte representing the row and column of that piece
// (4 bits for each). If the piece is not present, use 0xF to represent it.
// To make sure the same piece for the same player is treated in the same way,
// all byte representations of a group of piece is sorted.
std::array<uint64_t, 4> EncodeBoardState(const Board<Piece>& board);

}  // namespace xq

#endif  // XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_GAME_H__
