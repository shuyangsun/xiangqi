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

}  // namespace xq

#endif  // XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_GAME_H__
