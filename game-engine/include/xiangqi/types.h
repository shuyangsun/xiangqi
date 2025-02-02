#ifndef XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_TYPES_H__
#define XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_TYPES_H__

#include <array>
#include <cstdint>

namespace xiangqi {

constexpr uint8_t kTotalRow = 10;
constexpr uint8_t kTotalCol = 9;
constexpr uint8_t kTotalPieces = 32;

template <typename T>
using Board = std::array<std::array<T, kTotalCol>, kTotalRow>;

enum class Player : bool { RED = true, BLACK = false };
enum class Winner : int8_t { DRAW = 0, RED = 1, BLACK = -1 };

enum class Piece : int8_t {
  EMPTY = 0,
  R_GENERAL = 1,
  B_GENERAL = -1,
  R_ADVISOR_L = 2,
  B_ADVISOR_L = -2,
  R_ADVISOR_R = 3,
  B_ADVISOR_R = -3,
  R_ELEPHANT_L = 4,
  B_ELEPHANT_L = -4,
  R_ELEPHANT_R = 5,
  B_ELEPHANT_R = -5,
  R_HORSE_L = 6,
  B_HORSE_L = -6,
  R_HORSE_R = 7,
  B_HORSE_R = -7,
  R_CHARIOT_L = 8,
  B_CHARIOT_L = -8,
  R_CHARIOT_R = 9,
  B_CHARIOT_R = -9,
  R_CANNON_L = 10,
  B_CANNON_L = -10,
  R_CANNON_R = 11,
  B_CANNON_R = -11,
  R_SOLDIER_1 = 12,
  B_SOLDIER_1 = -12,
  R_SOLDIER_2 = 13,
  B_SOLDIER_2 = -13,
  R_SOLDIER_3 = 14,
  B_SOLDIER_3 = -14,
  R_SOLDIER_4 = 15,
  B_SOLDIER_4 = -15,
  R_SOLDIER_5 = 16,
  B_SOLDIER_5 = -16,
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

#endif  // XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_TYPES_H__
