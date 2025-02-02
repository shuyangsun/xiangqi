#ifndef XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_GAME_H__
#define XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_GAME_H__

#include <unordered_map>
#include <vector>

#include "xiangqi/types.h"

namespace xiangqi {

class Game {
 public:
  // Initialize an empty game with the default Xiangqi opening board.
  // Red represents self and black represents the opponent.
  Game();
  ~Game() = default;

  // Disable copy and move constructors.
  Game(const Game& game) = delete;
  Game(Game&& game) = delete;

  // Reset the board with default Xiangqi openning board.
  void Reset();
  // Reset the game with a given board position.
  void Reset(Board<Piece>&& board);
  // Reset the game with a map of pieces to their positions.
  void Reset(std::unordered_map<Piece, Position>&& piece_pos);

  // Change the current turn. Should only be called when the game starts.
  void ChangeTurn();

  // Get a copy of the current board.
  Board<Piece> CurrentBoard();

  // Get the piece at a speific position of the current board.
  Piece PieceAt(Position pos);

  // Get all possible moves of a position. Returns a 10x9 boolean board, with
  // possible locations marked as true. If there is no piece at the position,
  // returns a board with all false values.
  Board<bool> PossibleMoves(Position pos);

  // Move a piece from a position to another position. Returns true if the new
  // move caused a piece to be taken.
  bool Move(Position from, Position to);

  // Undo last move. Returns true if succeeded, false otherwise. Cannot undo
  // if the board is already at its original state when initialized or reset.
  bool Undo();

 private:
  bool is_red_turn_ = true;
  std::vector<Board<Piece>> history_;
  std::vector<MoveAction> moves_;
};

}  // namespace xiangqi

#endif  // XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_GAME_H__
