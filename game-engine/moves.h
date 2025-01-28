#ifndef XIANGQI_GAME_ENGINE_MOVES_H__
#define XIANGQI_GAME_ENGINE_MOVES_H__

#include "types.h"

namespace xiangqi::internal::util {

Board<bool> PossibleMovesGeneral(const Board<Piece>& board, Position pos);

}  // namespace xiangqi::internal::util

#endif  // XIANGQI_GAME_ENGINE_MOVES_H__
