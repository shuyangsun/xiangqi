#ifndef XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_GAME_H__
#define XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_GAME_H__

#include <optional>
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

  // Returns the current player.
  Player CurrentPlayer() const;

  // Returns the number of moves has been made in this game.
  size_t MovesCount() const;

  // Change the starting turn to Black.
  void MakeBlackMoveFirst();

  // Get the piece at a speific position of the current board.
  Piece PieceAt(Position pos) const;

  // Return reference of current board.
  const Board& CurrentBoard() const;

  // Move a piece from a position to another position, returns the captured
  // piece. If no piece was captured, return EMPTY.
  Piece Move(Movement move);

  // Whether or not the player can perform undo action.
  bool CanUndo() const;

  // Undo last move. Returns the move action that was reversed. Cannot undo
  // if the board is already at its original state when initialized or reset.
  Movement Undo();

  // Get initial encoded board state.
  BoardState InitialBoardState() const;

  std::vector<Movement> ExportMoves() const;

  // Reset the board with default Xiangqi openning board.
  void Restart();

  // Restores game state from inital state.
  void RestoreBoard(const BoardState& state);

  // Restores game state from exported moves.
  void RestoreMoves(const std::vector<Movement>& moves);

 private:
  Player player_ = Player::RED;
  std::optional<BoardState> initial_board_state_ = std::nullopt;
  Board board_;
  std::vector<Piece> captured_;
  std::vector<Movement> moves_;
};

}  // namespace xq

#endif  // XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_GAME_H__
