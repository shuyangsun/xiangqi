#ifndef XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_MOVES_H__
#define XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_MOVES_H__

#include "xiangqi/types.h"

namespace xq::internal::util {

// General can move to a maximum of 5 positions: up, down, left, right and
// flying general. Impossible positions are filled with kNoPosition.
std::array<Position, 5> PossibleMovesGeneral(const Board<Piece>& board,
                                             Position pos,
                                             Position opponent_general);

// Advisor can move to at most 2 positions. Impossible positions are filled with
// kNoPosition.
std::array<Position, 2> PossibleMovesAdvisor(const Board<Piece>& board,
                                             Position pos);

// Elephant can move to at most 4 positions. Impossible positions are filled
// with kNoPosition.
std::array<Position, 4> PossibleMovesElephant(const Board<Piece>& board,
                                              Position pos);

// Horse can move to at most 8 positions. Impossible positions are filled with
// kNoPosition.
std::array<Position, 8> PossibleMovesHorse(const Board<Piece>& board,
                                           Position pos);

// Chariot can move to at most 17 positions (8 + 9). Impossible positions are
// filled with kNoPosition.
std::array<Position, 17> PossibleMovesChariot(const Board<Piece>& board,
                                              Position pos);

// Cannon can move to at most 17 positions (8 + 9). Impossible positions are
// filled with kNoPosition.
std::array<Position, 17> PossibleMovesCannon(const Board<Piece>& board,
                                             Position pos);

// Soldier can move to at most 3 positions. Impossible positions are filled with
// kNoPosition.
std::array<Position, 3> PossibleMovesSoldier(const Board<Piece>& board,
                                             Position pos);

}  // namespace xq::internal::util

#endif  // XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_MOVES_H__
