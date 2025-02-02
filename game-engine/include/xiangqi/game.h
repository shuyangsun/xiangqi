#ifndef XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_GAME_H__
#define XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_GAME_H__

#include <vector>

#include "xiangqi/types.h"

namespace xiangqi {

class Game {
 public:
  Game();
  ~Game() = default;

  void Reset();
  void ChangeTurn();

  Board<Piece> CurrentBoard();
  Piece PieceAt(Position pos);

  Board<bool> PossibleMoves(Position pos);
  bool Move(Position from, Position to);
  bool Undo();

 private:
  bool is_red_turn_ = true;
  std::vector<Board<Piece>> history_;
  std::vector<MoveAction> moves_;
};

}  // namespace xiangqi

#endif  // XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_GAME_H__
