#ifndef XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_TYPES_C_H__
#define XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_TYPES_C_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Position on the board, range is [0, 90).
typedef uint8_t Position;

// Two Positions representing a movement.
typedef uint16_t Movement;

#define K_NO_POSITION 0xFF
#define K_NO_MOVEMENT 0xFFFF

#define K_TOTAL_ROW 10
#define K_TOTAL_COL 9
#define K_BOARD_SIZE 90  // K_TOTAL_ROW * K_TOTAL_COL
#define K_TOTAL_PIECES 32

#define K_RED_RIVER_START 45  // 5 * K_TOTAL_COL

#define K_MAX_MOVE_PER_PIECE 17

// Maximum number of possible moves per player is 112.
// Chariot: 2 * 17 = 34
// Cannon: 2 * 17 = 34
// Horse: 2 * 8 = 16
// Elephant: 4 + 2 = 6
// Solider: 5 * 3 = 15
// Advisor + General: 7
#define K_MAX_MOVE_PER_PLAYER 112

enum Player : bool { PLAYER_RED = true, PLAYER_BLACK = false };
enum Winner : int8_t {
  WINNER_DRAW = 0,
  WINNER_RED = 1,
  WINNER_BLACK = -1,
  WINNER_NONE = -2
};

enum Piece : int8_t {
  PIECE_EMPTY = 0,
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

// Row-major representation of the Xiangqi board in a 1-d array.
// The first element is the top-left of the board, and the last element is the
// bottom-right of the board.
typedef enum Piece BoardC[90];
typedef uint64_t BoardStateC[4];

// Rotate the position so that it's from the opponent's perspective.
static inline Position FlipPosition(const Position position) {
  return K_BOARD_SIZE - 1 - position;
}

static inline Position MirrorPositionHorizontal(const Position pos) {
  const uint8_t remainder = pos % K_TOTAL_COL;
  return pos - remainder + K_TOTAL_COL - 1 - remainder;
}

static inline Position MirrorPositionVertical(const Position pos) {
  const uint8_t remainder = pos % K_TOTAL_COL;
  return (K_TOTAL_ROW - 1 - pos / K_TOTAL_COL) * K_TOTAL_COL + remainder;
}

static inline bool IsEmpty(const enum Piece piece) {
  return piece == PIECE_EMPTY;
}
static inline bool IsRed(const enum Piece piece) { return piece > 0; }
static inline bool IsBlack(const enum Piece piece) { return piece < 0; }
static inline enum Player ChangePlayer(const enum Player player) {
  return (player == PLAYER_RED) ? PLAYER_BLACK : PLAYER_RED;
}

static inline Position Pos(const uint8_t row, const uint8_t col) {
  return row * K_TOTAL_COL + col;
}
static inline uint8_t Row(const Position position) {
  return position / K_TOTAL_COL;
}
static inline uint8_t Col(const Position position) {
  return position % K_TOTAL_COL;
}
static inline Position PosStr(const char* str) {
  return Pos(str[1] - '0', str[0] - (str[0] >= 'a' ? 'a' : 'A'));
}

static inline Movement NewMovement(const Position orig, const Position dest) {
  return ((Movement)orig) << 8 | ((Movement)dest);
}
static inline Position Orig(const Movement movement) { return movement >> 8; }
static inline Position Dest(const Movement movement) {
  return movement & 0x00FF;
}

#ifdef __cplusplus
}
#endif

#endif  // XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_TYPES_C_H__
