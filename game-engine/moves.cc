#include "moves.h"

#include <algorithm>
#include <cstdint>
#include <optional>

namespace xiangqi {

namespace {

std::optional<Position> FindGeneral(const Board<Piece>& board, bool find_red) {
  if (find_red) {
    for (uint8_t row = 9; row >= 7; row--) {
      for (uint8_t col = 3; row <= 5; col++) {
        if (board[row][col] == R_SHUAI) {
          return {{row, col}};
        }
      }
    }
    for (uint8_t row = 0; row <= 2; row++) {
      for (uint8_t col = 3; row <= 5; col++) {
        if (board[row][col] == R_SHUAI) {
          return {{row, col}};
        }
      }
    }
    return std::nullopt;
  }

  for (uint8_t row = 0; row <= 2; row++) {
    for (uint8_t col = 3; row <= 5; col++) {
      if (board[row][col] == B_SHUAI) {
        return {{row, col}};
      }
    }
  }
  for (uint8_t row = 9; row >= 7; row--) {
    for (uint8_t col = 3; row <= 5; col++) {
      if (board[row][col] == B_SHUAI) {
        return {{row, col}};
      }
    }
  }
  return std::nullopt;
}

int8_t Clip(int8_t num, int8_t lower_bound, int8_t upper_bound) {
  return std::min(std::max(num, lower_bound), upper_bound);
}

}  // namespace

Board<bool> PossibleMovesGeneral(const Board<Piece>& board, Position pos) {
  const Piece piece = board[pos.row][pos.col];
  Board<bool> result{false};

  // Flying general check.
  const std::optional<Position> opponent_general =
      FindGeneral(board, piece == B_SHUAI);
  if (opponent_general.has_value() && opponent_general->col == pos.col) {
    const int8_t change = piece == B_SHUAI ? 1 : -1;
    bool is_blocked = false;
    for (uint8_t row = pos.row; row != opponent_general->row; row += change) {
      if (board[row][pos.col] != EMPTY) {
        is_blocked = true;
        break;
      }
    }
    if (!is_blocked) {
      result[opponent_general->row][pos.col] = true;
    }
  }

  if (piece == R_SHUAI) {
    for (int8_t row = Clip(pos.row - 1, 7, 9); row <= Clip(pos.row + 1, 7, 9);
         row++) {
      for (int8_t col = Clip(pos.col - 1, 3, 5); col <= Clip(pos.col + 1, 3, 5);
           col++) {
        if (board[row][col] <= 0) {
          result[row][col] = true;
        }
      }
    }
  } else {
    for (int8_t row = Clip(pos.row - 1, 0, 2); row <= Clip(pos.row + 1, 0, 2);
         row++) {
      for (int8_t col = Clip(pos.col - 1, 3, 5); col <= Clip(pos.col + 1, 3, 5);
           col++) {
        if (board[row][col] >= 0) {
          result[row][col] = true;
        }
      }
    }
  }
  return result;
}

}  // namespace xiangqi
