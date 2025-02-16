#ifndef XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_TYPES_H__
#define XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_TYPES_H__

#include <array>
#include <cctype>
#include <cstdint>
#include <string_view>
#include <type_traits>

namespace xq {

// Position on the board, range is [0, 90).
using Position = uint8_t;

// Two Positions representing a movement.
using Movement = uint16_t;

constexpr Position kNoPosition = 0xFF;
constexpr Movement kNoMovement = 0xFFFF;

constexpr uint8_t kTotalRow = 10;
constexpr uint8_t kTotalCol = 9;
constexpr uint8_t kBoardSize = kTotalRow * kTotalCol;
constexpr uint8_t kTotalPieces = 32;

constexpr uint8_t kRedPalaceRowMin = 7;
constexpr uint8_t kRedPalaceRowMax = kTotalRow - 1;
constexpr uint8_t kBlackPalaceRowMin = 0;
constexpr uint8_t kBlackPalaceRowMax = 3;
constexpr uint8_t kPalaceColMin = 3;
constexpr uint8_t kPalaceColMax = 5;

// Starting position of red river.
constexpr Position kRedRiverStart = 5 * kTotalCol;

// Row-major board in flat 1-D array.
template <typename T>
using Board = std::array<T, kBoardSize>;

using BoardState = std::array<uint64_t, 4>;

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

// Rotate the position so that it's from the opponent's perspective.
inline Position FlipPosition(Position position) {
  return kBoardSize - 1 - position;
}

inline bool IsEmpty(const Piece piece) { return piece == Piece::EMPTY; }
inline bool IsRed(const Piece piece) {
  return static_cast<std::underlying_type_t<Piece>>(piece) > 0;
}
inline bool IsBlack(const Piece piece) {
  return static_cast<std::underlying_type_t<Piece>>(piece) < 0;
}
inline Player ChangePlayer(const Player player) {
  return (player == Player::RED) ? Player::BLACK : Player::RED;
}

inline Position Pos(const uint8_t row, const uint8_t col) {
  return row * kTotalCol + col;
}
inline uint8_t Row(const Position position) { return position / kTotalCol; }
inline uint8_t Col(const Position position) { return position % kTotalCol; }
inline Position PosStr(const std::string_view str) {
  if (str.size() != 2) {
    return kNoPosition;
  }
  return Pos(str[1] - '0', std::tolower(str[0]) - 'a');
}

inline Movement NewMovement(const Position orig, const Position dest) {
  return static_cast<Movement>(orig) << 8 | static_cast<Movement>(dest);
}
inline Position Orig(const Movement movement) { return movement >> 8; }
inline Position Dest(const Movement movement) { return movement & 0x00FF; }

}  // namespace xq

#endif  // XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_TYPES_H__
