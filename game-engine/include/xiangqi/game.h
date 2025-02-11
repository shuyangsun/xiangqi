#ifndef XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_GAME_H__
#define XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_GAME_H__

#include <unordered_map>
#include <utility>
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
  void ResetFromBoard(const Board<Piece>& board);
  // Reset the game with a map of pieces to their positions.
  void ResetFromPos(const std::unordered_map<Position, Piece>& pos_piece);

  // Returns the current player.
  Player CurrentPlayer() const;

  // Returns the number of moves has been made in this game.
  size_t MovesCount() const;

  // Change the starting turn to Black.
  void MakeBlackMoveFirst();

  // Get a copy of the starting board.
  Board<Piece> StartingBoard() const;

  // Get a copy of the current board.
  Board<Piece> CurrentBoard() const;

  // Get the piece at a speific position of the current board.
  Piece PieceAt(Position pos) const;

  // Move a piece from a position to another position, returns the captured
  // piece.
  Piece Move(Position from, Position to);

  // Whether or not the player can perform undo action.
  bool CanUndo() const;

  // Undo last move. Returns the move action that was reversed. Cannot undo
  // if the board is already at its original state when initialized or reset.
  MoveAction Undo();

  // Export moves as vector of uint16_t. The integer represents four 4-bit
  // integers as Row(from), Col(from), Row(to) and Col(to).
  std::vector<uint16_t> ExportMoves() const;

  // Restores game state from exported moves.
  void RestoreMoves(const std::vector<uint16_t>& moves);

 private:
  Player player_ = Player::RED;
  std::vector<Board<Piece>> history_;
  std::vector<MoveAction> moves_;
};

// Returns true if it's checkmade for the given player. I.e., if the
// current player does not take certain actions, the opponent can capture
// the general with their next move.
bool IsCheckMade(const Board<Piece>& board, Player player);

// Returns true if the game is over. For example, one of the generals has
// been captured, or if there is no possible move for the current player's
// general to not be captured.
bool IsGameOver(const Board<Piece>& board);

// Returns the winner if the game is over. Returns NONE if the game is not
// over.
Winner GetWinner(const Board<Piece>& board);

// Returns all possible moves for the player with piece at position.
Board<bool> PossibleMoves(const Board<Piece>& board, Position pos);

// Move a piece from a position to another position, returns the captured
// piece. If no piece was captured, return EMPTY.
Piece Move(Board<Piece>& board, Position from, Position to);

// Returns a vector of all possible moves for player.
std::vector<uint16_t> PossibleMoves(const Board<Piece>& board, Player player);

// Returns a vector of all possible boards for the given player after any valid
// move.
std::vector<Board<Piece>> PossibleNextBoards(const Board<Piece>& board,
                                             Player player);

// Rotate the position so that it's from the opponent's perspective.
Position FlipPosition(Position position);

// Rotate the board so that it's from the opponent's perspective.
Board<Piece> FlipBoard(const Board<Piece>& board);

// Encode the board state using a small number of bytes, mainly used for the
// game AI to identify a unique board state.
//
// There are a total of 32 pieces on the board, so the encoded board state is
// 32 bytes, with each byte representing the row and column of that piece
// (4 bits for each). If the piece is not present, use 0xF to represent it.
// To make sure the same piece for the same player is treated in the same way,
// all byte representations of a group of piece is sorted.
std::array<uint64_t, 4> EncodeBoardState(const Board<Piece>& board);

// Decode the encoded board state back to its original state.
Board<Piece> DecodeBoardState(const std::array<uint64_t, 4> state);

}  // namespace xq

#endif  // XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_GAME_H__
