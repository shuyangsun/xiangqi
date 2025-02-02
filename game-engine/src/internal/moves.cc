// file: moves.cc

#include "xiangqi/internal/moves.h"

#include <algorithm>
#include <cstdint>
#include <optional>

namespace xiangqi::internal::util {

namespace {

// Helper function to create an empty (all false) board.
Board<bool> makeEmptyBoard() {
  Board<bool> board;
  for (auto& row : board) {
    row.fill(false);
  }
  return board;
}

std::optional<Position> FindGeneral(const Board<Piece>& board, bool find_red) {
  if (find_red) {
    for (uint8_t row = 9; row >= 7; row--) {
      for (uint8_t col = 3; col <= 5; col++) {
        if (board[row][col] == R_GENERAL) {
          return {{row, col}};
        }
      }
    }
    for (uint8_t row = 0; row <= 2; row++) {
      for (uint8_t col = 3; col <= 5; col++) {
        if (board[row][col] == R_GENERAL) {
          return {{row, col}};
        }
      }
    }
    return std::nullopt;
  }

  for (uint8_t row = 0; row <= 2; row++) {
    for (uint8_t col = 3; col <= 5; col++) {
      if (board[row][col] == B_GENERAL) {
        return {{row, col}};
      }
    }
  }
  for (uint8_t row = 9; row >= 7; row--) {
    for (uint8_t col = 3; col <= 5; col++) {
      if (board[row][col] == B_GENERAL) {
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

Board<bool> PossibleMovesEmpty(const Board<Piece>& board, Position pos) {
  return makeEmptyBoard();
}

Board<bool> PossibleMovesGeneral(const Board<Piece>& board, Position pos) {
  const Piece piece = board[pos.row][pos.col];
  Board<bool> result{false};

  // Flying general check.
  const std::optional<Position> opponent_general =
      FindGeneral(board, piece == B_GENERAL);
  if (opponent_general.has_value() && opponent_general->col == pos.col) {
    const int8_t change = piece == B_GENERAL ? 1 : -1;
    bool is_blocked = false;
    // Start checking one square past the moving general.
    for (uint8_t row = pos.row + change; row != opponent_general->row;
         row += change) {
      if (board[row][pos.col] != EMPTY) {
        is_blocked = true;
        break;
      }
    }
    if (!is_blocked) {
      result[opponent_general->row][pos.col] = true;
    }
  }

  if (piece == R_GENERAL) {
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

Board<bool> PossibleMovesAdvisor(const Board<Piece>& board, Position pos) {
  Board<bool> result = makeEmptyBoard();
  const Piece piece = board[pos.row][pos.col];
  bool isRed = piece > 0;

  // Advisors move one square diagonally.
  int dr[4] = {-1, -1, 1, 1};
  int dc[4] = {-1, 1, -1, 1};

  // Palace boundaries.
  int minRow = isRed ? 7 : 0;
  int maxRow = isRed ? 9 : 2;
  int minCol = 3, maxCol = 5;

  for (int i = 0; i < 4; i++) {
    int newRow = pos.row + dr[i];
    int newCol = pos.col + dc[i];
    if (newRow >= minRow && newRow <= maxRow && newCol >= minCol &&
        newCol <= maxCol) {
      if (isRed) {
        if (board[newRow][newCol] <= 0) result[newRow][newCol] = true;
      } else {
        if (board[newRow][newCol] >= 0) result[newRow][newCol] = true;
      }
    }
  }
  return result;
}

Board<bool> PossibleMovesElephant(const Board<Piece>& board, Position pos) {
  Board<bool> result = makeEmptyBoard();
  const Piece piece = board[pos.row][pos.col];
  bool isRed = piece > 0;

  // Elephants move two squares diagonally.
  int dr[4] = {-2, -2, 2, 2};
  int dc[4] = {-2, 2, -2, 2};

  for (int i = 0; i < 4; i++) {
    int newRow = pos.row + dr[i];
    int newCol = pos.col + dc[i];
    // Check board bounds.
    if (newRow < 0 || newRow >= kTotalRow || newCol < 0 || newCol >= kTotalCol)
      continue;
    // Elephants cannot cross the river.
    if (isRed && newRow < 5) continue;
    if (!isRed && newRow > 4) continue;
    // Check the intermediate square ("the eye").
    int midRow = pos.row + dr[i] / 2;
    int midCol = pos.col + dc[i] / 2;
    if (board[midRow][midCol] != EMPTY) continue;
    // Allow move if destination is empty or holds an enemy.
    if (isRed) {
      if (board[newRow][newCol] <= 0) result[newRow][newCol] = true;
    } else {
      if (board[newRow][newCol] >= 0) result[newRow][newCol] = true;
    }
  }
  return result;
}

Board<bool> PossibleMovesHorse(const Board<Piece>& board, Position pos) {
  Board<bool> result = makeEmptyBoard();
  const Piece piece = board[pos.row][pos.col];
  bool isRed = piece > 0;

  // For each of the four orthogonal directions, check if the "leg" is free.
  // Upward:
  if (pos.row - 1 >= 0 && board[pos.row - 1][pos.col] == EMPTY) {
    int candRow = pos.row - 2;
    if (candRow >= 0) {
      int candCols[2] = {pos.col - 1, pos.col + 1};
      for (int i = 0; i < 2; i++) {
        int candCol = candCols[i];
        if (candCol >= 0 && candCol < kTotalCol) {
          if (isRed) {
            if (board[candRow][candCol] <= 0) result[candRow][candCol] = true;
          } else {
            if (board[candRow][candCol] >= 0) result[candRow][candCol] = true;
          }
        }
      }
    }
  }
  // Downward:
  if (pos.row + 1 < kTotalRow && board[pos.row + 1][pos.col] == EMPTY) {
    int candRow = pos.row + 2;
    if (candRow < kTotalRow) {
      int candCols[2] = {pos.col - 1, pos.col + 1};
      for (int i = 0; i < 2; i++) {
        int candCol = candCols[i];
        if (candCol >= 0 && candCol < kTotalCol) {
          if (isRed) {
            if (board[candRow][candCol] <= 0) result[candRow][candCol] = true;
          } else {
            if (board[candRow][candCol] >= 0) result[candRow][candCol] = true;
          }
        }
      }
    }
  }
  // Leftward:
  if (pos.col - 1 >= 0 && board[pos.row][pos.col - 1] == EMPTY) {
    int candCol = pos.col - 2;
    if (candCol >= 0) {
      int candRows[2] = {pos.row - 1, pos.row + 1};
      for (int i = 0; i < 2; i++) {
        int candRow = candRows[i];
        if (candRow >= 0 && candRow < kTotalRow) {
          if (isRed) {
            if (board[candRow][candCol] <= 0) result[candRow][candCol] = true;
          } else {
            if (board[candRow][candCol] >= 0) result[candRow][candCol] = true;
          }
        }
      }
    }
  }
  // Rightward:
  if (pos.col + 1 < kTotalCol && board[pos.row][pos.col + 1] == EMPTY) {
    int candCol = pos.col + 2;
    if (candCol < kTotalCol) {
      int candRows[2] = {pos.row - 1, pos.row + 1};
      for (int i = 0; i < 2; i++) {
        int candRow = candRows[i];
        if (candRow >= 0 && candRow < kTotalRow) {
          if (isRed) {
            if (board[candRow][candCol] <= 0) result[candRow][candCol] = true;
          } else {
            if (board[candRow][candCol] >= 0) result[candRow][candCol] = true;
          }
        }
      }
    }
  }
  return result;
}

Board<bool> PossibleMovesChariot(const Board<Piece>& board, Position pos) {
  Board<bool> result = makeEmptyBoard();
  const Piece piece = board[pos.row][pos.col];
  bool isRed = piece > 0;

  // Upwards.
  for (int r = pos.row - 1; r >= 0; r--) {
    if (board[r][pos.col] == EMPTY) {
      result[r][pos.col] = true;
    } else {
      if (isRed) {
        if (board[r][pos.col] < 0) result[r][pos.col] = true;
      } else {
        if (board[r][pos.col] > 0) result[r][pos.col] = true;
      }
      break;
    }
  }
  // Downwards.
  for (int r = pos.row + 1; r < kTotalRow; r++) {
    if (board[r][pos.col] == EMPTY) {
      result[r][pos.col] = true;
    } else {
      if (isRed) {
        if (board[r][pos.col] < 0) result[r][pos.col] = true;
      } else {
        if (board[r][pos.col] > 0) result[r][pos.col] = true;
      }
      break;
    }
  }
  // Leftwards.
  for (int c = pos.col - 1; c >= 0; c--) {
    if (board[pos.row][c] == EMPTY) {
      result[pos.row][c] = true;
    } else {
      if (isRed) {
        if (board[pos.row][c] < 0) result[pos.row][c] = true;
      } else {
        if (board[pos.row][c] > 0) result[pos.row][c] = true;
      }
      break;
    }
  }
  // Rightwards.
  for (int c = pos.col + 1; c < kTotalCol; c++) {
    if (board[pos.row][c] == EMPTY) {
      result[pos.row][c] = true;
    } else {
      if (isRed) {
        if (board[pos.row][c] < 0) result[pos.row][c] = true;
      } else {
        if (board[pos.row][c] > 0) result[pos.row][c] = true;
      }
      break;
    }
  }

  return result;
}

Board<bool> PossibleMovesCannon(const Board<Piece>& board, Position pos) {
  Board<bool> result = makeEmptyBoard();
  const Piece piece = board[pos.row][pos.col];
  bool isRed = piece > 0;

  // For each direction, first mark all empty squares.
  // Then, once a non-empty square (screen) is encountered,
  // continue until the first non-empty square after the screen which is
  // capturable.

  // Upwards.
  {
    bool screenFound = false;
    for (int r = pos.row - 1; r >= 0; r--) {
      if (!screenFound) {
        if (board[r][pos.col] == EMPTY) {
          result[r][pos.col] = true;
        } else {
          screenFound = true;
        }
      } else {
        if (board[r][pos.col] != EMPTY) {
          if (isRed) {
            if (board[r][pos.col] < 0) result[r][pos.col] = true;
          } else {
            if (board[r][pos.col] > 0) result[r][pos.col] = true;
          }
          break;
        }
      }
    }
  }

  // Downwards.
  {
    bool screenFound = false;
    for (int r = pos.row + 1; r < kTotalRow; r++) {
      if (!screenFound) {
        if (board[r][pos.col] == EMPTY) {
          result[r][pos.col] = true;
        } else {
          screenFound = true;
        }
      } else {
        if (board[r][pos.col] != EMPTY) {
          if (isRed) {
            if (board[r][pos.col] < 0) result[r][pos.col] = true;
          } else {
            if (board[r][pos.col] > 0) result[r][pos.col] = true;
          }
          break;
        }
      }
    }
  }

  // Leftwards.
  {
    bool screenFound = false;
    for (int c = pos.col - 1; c >= 0; c--) {
      if (!screenFound) {
        if (board[pos.row][c] == EMPTY) {
          result[pos.row][c] = true;
        } else {
          screenFound = true;
        }
      } else {
        if (board[pos.row][c] != EMPTY) {
          if (isRed) {
            if (board[pos.row][c] < 0) result[pos.row][c] = true;
          } else {
            if (board[pos.row][c] > 0) result[pos.row][c] = true;
          }
          break;
        }
      }
    }
  }

  // Rightwards.
  {
    bool screenFound = false;
    for (int c = pos.col + 1; c < kTotalCol; c++) {
      if (!screenFound) {
        if (board[pos.row][c] == EMPTY) {
          result[pos.row][c] = true;
        } else {
          screenFound = true;
        }
      } else {
        if (board[pos.row][c] != EMPTY) {
          if (isRed) {
            if (board[pos.row][c] < 0) result[pos.row][c] = true;
          } else {
            if (board[pos.row][c] > 0) result[pos.row][c] = true;
          }
          break;
        }
      }
    }
  }

  return result;
}

Board<bool> PossibleMovesSoldier(const Board<Piece>& board, Position pos) {
  Board<bool> result = makeEmptyBoard();
  const Piece piece = board[pos.row][pos.col];
  bool isRed = piece > 0;

  // Forward move.
  int forwardRow = isRed ? pos.row - 1 : pos.row + 1;
  if (forwardRow >= 0 && forwardRow < kTotalRow) {
    if (isRed) {
      if (board[forwardRow][pos.col] <= 0) result[forwardRow][pos.col] = true;
    } else {
      if (board[forwardRow][pos.col] >= 0) result[forwardRow][pos.col] = true;
    }
  }

  // Once soldier has crossed the river, add sideways moves.
  if (isRed) {
    // For red, crossing the river means row <= 4.
    if (pos.row <= 4) {
      if (pos.col - 1 >= 0) {
        if (board[pos.row][pos.col - 1] <= 0)
          result[pos.row][pos.col - 1] = true;
      }
      if (pos.col + 1 < kTotalCol) {
        if (board[pos.row][pos.col + 1] <= 0)
          result[pos.row][pos.col + 1] = true;
      }
    }
  } else {
    // For black, crossing the river means row >= 5.
    if (pos.row >= 5) {
      if (pos.col - 1 >= 0) {
        if (board[pos.row][pos.col - 1] >= 0)
          result[pos.row][pos.col - 1] = true;
      }
      if (pos.col + 1 < kTotalCol) {
        if (board[pos.row][pos.col + 1] >= 0)
          result[pos.row][pos.col + 1] = true;
      }
    }
  }

  return result;
}

}  // namespace xiangqi::internal::util
