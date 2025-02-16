#ifndef XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_TYPES_H__
#define XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_TYPES_H__

#include <array>

#include "xiangqi/types_c.h"

namespace xq {

// Row-major representation of the Xiangqi board in a 1-d array.
// The first element is the top-left of the board, and the last element is the
// bottom-right of the board.
using Board = std::array<Piece, K_BOARD_SIZE>;

using BoardState = std::array<uint64_t, 4>;

}  // namespace xq

#endif  // XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_TYPES_H__
