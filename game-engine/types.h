#ifndef XIANGQI_GAME_ENGINE_TYPES_H__
#define XIANGQI_GAME_ENGINE_TYPES_H__

#include <array>
#include <cstdint>

namespace xiangqi {

constexpr uint8_t kTotalRow = 10;
constexpr uint8_t kTotalCol = 9;
constexpr uint8_t kTotalPieces = 32;

template <typename T>
using Board = std::array<std::array<T, kTotalCol>, kTotalRow>;

enum Piece : int8_t {
  EMPTY = 0,
  R_SHUAI = 1,
  B_SHUAI = -1,
  R_SHI_L = 2,
  B_SHI_L = -2,
  R_SHI_R = 3,
  B_SHI_R = -3,
  R_XIANG_L = 4,
  B_XIANG_L = -4,
  R_XIANG_R = 5,
  B_XIANG_R = -5,
  R_MA_L = 6,
  B_MA_L = -6,
  R_MA_R = 7,
  B_MA_R = -7,
  R_JU_L = 8,
  B_JU_L = -8,
  R_JU_R = 9,
  B_JU_R = -9,
  R_PAO_L = 10,
  B_PAO_L = -10,
  R_PAO_R = 11,
  B_PAO_R = -11,
  R_ZU_1 = 12,
  B_ZU_1 = -12,
  R_ZU_2 = 13,
  B_ZU_2 = -13,
  R_ZU_3 = 14,
  B_ZU_3 = -14,
  R_ZU_4 = 15,
  B_ZU_4 = -15,
  R_ZU_5 = 16,
  B_ZU_5 = -16,
};

struct Position {
  uint8_t row;
  uint8_t col;
};

struct MoveAction {
  Piece piece;
  Position from;
  Position to;
};

}  // namespace xiangqi

#endif  // XIANGQI_GAME_ENGINE_TYPES_H__
