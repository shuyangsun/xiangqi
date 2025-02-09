#ifndef XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_TYPES_H__
#define XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_TYPES_H__

#include <array>
#include <cstdint>

namespace xq {

constexpr uint8_t kTotalRow = 10;
constexpr uint8_t kTotalCol = 9;

constexpr uint8_t kRedPalaceRowMin = 7;
constexpr uint8_t kRedPalaceRowMax = kTotalRow - 1;
constexpr uint8_t kBlackPalaceRowMin = 0;
constexpr uint8_t kBlackPalaceRowMax = 3;
constexpr uint8_t kPalaceColMin = 3;
constexpr uint8_t kPalaceColMax = 5;

constexpr uint8_t kTotalPieces = 32;

template <typename T>
using Board = std::array<std::array<T, kTotalCol>, kTotalRow>;

enum class Player : bool { RED = true, BLACK = false };
enum class Winner : int8_t { DRAW = 0, RED = 1, BLACK = -1, NONE = -2 };

enum class Piece : int8_t {
  EMPTY = 0,
  R_GENERAL = 1,
  B_GENERAL = -1,
  R_ADVISOR_1 = 2,
  B_ADVISOR_1 = -2,
  R_ADVISOR_2 = 3,
  B_ADVISOR_2 = -3,
  R_ELEPHANT_1 = 4,
  B_ELEPHANT_1 = -4,
  R_ELEPHANT_2 = 5,
  B_ELEPHANT_2 = -5,
  R_HORSE_1 = 6,
  B_HORSE_1 = -6,
  R_HORSE_2 = 7,
  B_HORSE_2 = -7,
  R_CHARIOT_1 = 8,
  B_CHARIOT_1 = -8,
  R_CHARIOT_2 = 9,
  B_CHARIOT_2 = -9,
  R_CANNON_1 = 10,
  B_CANNON_1 = -10,
  R_CANNON_2 = 11,
  B_CANNON_2 = -11,
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
  Piece captured;
};

}  // namespace xq

#endif  // XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_TYPES_H__
