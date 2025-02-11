#ifndef XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_TYPES_H__
#define XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_TYPES_H__

#include <array>
#include <cstdint>
#include <type_traits>

namespace xq {

// row, col, 4 bytes each.
using Position = uint8_t;

constexpr Position kNoPosition = 0xFF;

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
  R_ADVISOR = 2,
  B_ADVISOR = -2,
  R_ELEPHANT = 3,
  B_ELEPHANT = -3,
  R_HORSE = 4,
  B_HORSE = -4,
  R_CHARIOT = 5,
  B_CHARIOT = -5,
  R_CANNON = 6,
  B_CANNON = -6,
  R_SOLDIER = 7,
  B_SOLDIER = -7
};

struct MoveAction {
  Piece piece;
  Position from;
  Position to;
  Piece captured;
};

inline bool IsRed(Piece piece) {
  return static_cast<std::underlying_type_t<Piece>>(piece) > 0;
}
inline bool IsBlack(Piece piece) {
  return static_cast<std::underlying_type_t<Piece>>(piece) < 0;
}
inline Player ChangePlayer(Player player) {
  return (player == Player::RED) ? Player::BLACK : Player::RED;
}

inline Position Pos(uint8_t row, uint8_t col) { return row << 4 | col; }
inline Position Row(Position position) { return (position & 0xF0) >> 4; }
inline Position Col(Position position) { return (position & 0x0F); }

}  // namespace xq

#endif  // XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_TYPES_H__
