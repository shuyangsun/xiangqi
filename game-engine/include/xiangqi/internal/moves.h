#ifndef XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_MOVES_H__
#define XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_MOVES_H__

#include "xiangqi/types.h"

namespace xq::internal::util {

// Return board full of false.
Board<bool> PossibleMovesEmpty(const Board<Piece>& board, Position pos);

// Returns a board (10×9) with "true" set for squares that the General can
// legally move to. The General is assumed to move one square orthogonally
// within the palace.
Board<bool> PossibleMovesGeneral(const Board<Piece>& board, Position pos);

// Returns a board with "true" set for squares that an Advisor can move to.
// Advisors move one square diagonally inside the palace.
Board<bool> PossibleMovesAdvisor(const Board<Piece>& board, Position pos);

// Returns a board with "true" set for squares that an Elephant can move to.
// Elephants move exactly two squares diagonally, provided that the intermediate
// square ("the eye") is empty, and they cannot cross the river.
Board<bool> PossibleMovesElephant(const Board<Piece>& board, Position pos);

// Returns a board with "true" set for squares that a Horse can move to.
// Horses move one square orthogonally then one square diagonally outward,
// but the orthogonal “leg” must be unblocked.
Board<bool> PossibleMovesHorse(const Board<Piece>& board, Position pos);

// Returns a board with "true" set for squares that a Chariot can move to.
// Chariots move in straight lines (horizontally or vertically) until an
// intervening piece is encountered; they may capture an opposing piece on the
// first occupied square.
Board<bool> PossibleMovesChariot(const Board<Piece>& board, Position pos);

// Returns a board with "true" set for squares that a Cannon can move to.
// Cannons move like chariots when not capturing. To capture, a cannon must have
// exactly one intervening piece (a screen), and then may capture the first
// enemy piece encountered after the screen.
Board<bool> PossibleMovesCannon(const Board<Piece>& board, Position pos);

// Returns a board with "true" set for squares that a Soldier can move to.
// Soldiers always move forward (red moves upward, black moves downward).
// Once a Soldier has crossed the river, it may also move horizontally.
Board<bool> PossibleMovesSoldier(const Board<Piece>& board, Position pos);

}  // namespace xq::internal::util

#endif  // XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_MOVES_H__
