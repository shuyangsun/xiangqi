// file: moves.cc

#include "xiangqi/internal/moves.h"

#include <algorithm>
#include <cstdint>
#include <optional>
#include <type_traits>

namespace xq::internal::util {

namespace {

bool IsRed(Piece piece) {
  return static_cast<std::underlying_type_t<Piece>>(piece) > 0;
}

bool IsRedOrEmpty(Piece piece) { return IsRed(piece) || piece == Piece::EMPTY; }

bool IsBlack(Piece piece) {
  return static_cast<std::underlying_type_t<Piece>>(piece) < 0;
}

bool IsBlackOrEmpty(Piece piece) {
  return IsBlack(piece) || piece == Piece::EMPTY;
}

// Helper function to create an empty (all false) board.
Board<bool> MakeEmptyBoard() {
  Board<bool> board;
  for (auto& row : board) {
    row.fill(false);
  }
  return board;
}

std::optional<Position> FindGeneral(const Board<Piece>& board, bool find_red) {
  if (find_red) {
    for (uint8_t row = kRedPalaceRowMax; row >= kRedPalaceRowMin; row--) {
      for (uint8_t col = kPalaceColMin; col <= kPalaceColMax; col++) {
        if (board[row][col] == Piece::R_GENERAL) {
          return {{row, col}};
        }
      }
    }
    for (uint8_t row = kBlackPalaceRowMin; row <= kBlackPalaceRowMax; row++) {
      for (uint8_t col = kPalaceColMin; col <= kPalaceColMax; col++) {
        if (board[row][col] == Piece::R_GENERAL) {
          return {{row, col}};
        }
      }
    }
    return std::nullopt;
  }

  for (uint8_t row = kBlackPalaceRowMin; row <= kBlackPalaceRowMax; row++) {
    for (uint8_t col = kPalaceColMin; col <= kPalaceColMax; col++) {
      if (board[row][col] == Piece::B_GENERAL) {
        return {{row, col}};
      }
    }
  }
  for (uint8_t row = kRedPalaceRowMax; row >= kRedPalaceRowMin; row--) {
    for (uint8_t col = kPalaceColMin; col <= kPalaceColMax; col++) {
      if (board[row][col] == Piece::B_GENERAL) {
        return {{row, col}};
      }
    }
  }
  return std::nullopt;
}

}  // namespace

Board<bool> PossibleMovesEmpty(const Board<Piece>& board, Position pos) {
  return MakeEmptyBoard();
}

Board<bool> PossibleMovesGeneral(const Board<Piece>& board, Position pos) {
  const Piece piece = board[pos.row][pos.col];
  Board<bool> result{false};

  // Flying general check.
  const std::optional<Position> opponent_general =
      FindGeneral(board, piece == Piece::B_GENERAL);
  if (opponent_general.has_value() && opponent_general->col == pos.col) {
    const int8_t change = piece == Piece::B_GENERAL ? 1 : -1;
    bool is_blocked = false;
    // Start checking one square past the moving general.
    for (uint8_t row = pos.row + change; row != opponent_general->row;
         row += change) {
      if (board[row][pos.col] != Piece::EMPTY) {
        is_blocked = true;
        break;
      }
    }
    if (!is_blocked) {
      result[opponent_general->row][pos.col] = true;
    }
  }

  int8_t row, col;
  if (piece == Piece::R_GENERAL) {
    // Move down:
    row = pos.row + 1;
    col = pos.col;
    if (row <= kRedPalaceRowMax && IsBlackOrEmpty(board[row][col])) {
      result[row][col] = true;
    }
    // Move up:
    row = pos.row - 1;
    col = pos.col;
    if (row >= kRedPalaceRowMin && IsBlackOrEmpty(board[row][col])) {
      result[row][col] = true;
    }
    // Move left:
    row = pos.row;
    col = pos.col - 1;
    if (col >= kPalaceColMin && IsBlackOrEmpty(board[row][col])) {
      result[row][col] = true;
    }
    // Move right:
    row = pos.row;
    col = pos.col + 1;
    if (col <= kPalaceColMax && IsBlackOrEmpty(board[row][col])) {
      result[row][col] = true;
    }
  } else {
    // Move down:
    row = pos.row + 1;
    col = pos.col;
    if (row <= kBlackPalaceRowMax && IsRedOrEmpty(board[row][col])) {
      result[row][col] = true;
    }
    // Move up:
    row = pos.row - 1;
    col = pos.col;
    if (row >= kBlackPalaceRowMin && IsRedOrEmpty(board[row][col])) {
      result[row][col] = true;
    }
    // Move left:
    row = pos.row;
    col = pos.col - 1;
    if (col >= kPalaceColMin && IsRedOrEmpty(board[row][col])) {
      result[row][col] = true;
    }
    // Move right:
    row = pos.row;
    col = pos.col + 1;
    if (col <= kPalaceColMax && IsRedOrEmpty(board[row][col])) {
      result[row][col] = true;
    }
  }
  return result;
}

Board<bool> PossibleMovesAdvisor(const Board<Piece>& board, Position pos) {
  Board<bool> result = MakeEmptyBoard();
  const Piece piece = board[pos.row][pos.col];
  bool isRed = IsRed(piece);

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
        if (IsBlackOrEmpty(board[newRow][newCol])) {
          result[newRow][newCol] = true;
        }
      } else {
        if (IsRedOrEmpty(board[newRow][newCol])) {
          result[newRow][newCol] = true;
        }
      }
    }
  }
  return result;
}

Board<bool> PossibleMovesElephant(const Board<Piece>& board, Position pos) {
  Board<bool> result = MakeEmptyBoard();
  const Piece piece = board[pos.row][pos.col];
  bool isRed = IsRed(piece);

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
    if (board[midRow][midCol] != Piece::EMPTY) continue;
    // Allow move if destination is empty or holds an enemy.
    if (isRed) {
      if (IsBlackOrEmpty(board[newRow][newCol])) {
        result[newRow][newCol] = true;
      }
    } else {
      if (IsRedOrEmpty(board[newRow][newCol])) {
        result[newRow][newCol] = true;
      }
    }
  }
  return result;
}

Board<bool> PossibleMovesHorse(const Board<Piece>& board, Position pos) {
  Board<bool> result = MakeEmptyBoard();
  const Piece piece = board[pos.row][pos.col];
  bool isRed = IsRed(piece);

  // For each of the four orthogonal directions, check if the "leg" is free.
  // Upward:
  if (pos.row - 1 >= 0 && board[pos.row - 1][pos.col] == Piece::EMPTY) {
    int candRow = pos.row - 2;
    if (candRow >= 0) {
      int candCols[2] = {pos.col - 1, pos.col + 1};
      for (int i = 0; i < 2; i++) {
        int candCol = candCols[i];
        if (candCol >= 0 && candCol < kTotalCol) {
          if (isRed) {
            if (IsBlackOrEmpty(board[candRow][candCol])) {
              result[candRow][candCol] = true;
            }
          } else {
            if (IsRedOrEmpty(board[candRow][candCol])) {
              result[candRow][candCol] = true;
            }
          }
        }
      }
    }
  }
  // Downward:
  if (pos.row + 1 < kTotalRow && board[pos.row + 1][pos.col] == Piece::EMPTY) {
    int candRow = pos.row + 2;
    if (candRow < kTotalRow) {
      int candCols[2] = {pos.col - 1, pos.col + 1};
      for (int i = 0; i < 2; i++) {
        int candCol = candCols[i];
        if (candCol >= 0 && candCol < kTotalCol) {
          if (isRed) {
            if (IsBlackOrEmpty(board[candRow][candCol])) {
              result[candRow][candCol] = true;
            }
          } else {
            if (IsRedOrEmpty(board[candRow][candCol])) {
              result[candRow][candCol] = true;
            }
          }
        }
      }
    }
  }
  // Leftward:
  if (pos.col - 1 >= 0 && board[pos.row][pos.col - 1] == Piece::EMPTY) {
    int candCol = pos.col - 2;
    if (candCol >= 0) {
      int candRows[2] = {pos.row - 1, pos.row + 1};
      for (int i = 0; i < 2; i++) {
        int candRow = candRows[i];
        if (candRow >= 0 && candRow < kTotalRow) {
          if (isRed) {
            if (IsBlackOrEmpty(board[candRow][candCol])) {
              result[candRow][candCol] = true;
            }
          } else {
            if (IsRedOrEmpty(board[candRow][candCol])) {
              result[candRow][candCol] = true;
            }
          }
        }
      }
    }
  }
  // Rightward:
  if (pos.col + 1 < kTotalCol && board[pos.row][pos.col + 1] == Piece::EMPTY) {
    int candCol = pos.col + 2;
    if (candCol < kTotalCol) {
      int candRows[2] = {pos.row - 1, pos.row + 1};
      for (int i = 0; i < 2; i++) {
        int candRow = candRows[i];
        if (candRow >= 0 && candRow < kTotalRow) {
          if (isRed) {
            if (IsBlackOrEmpty(board[candRow][candCol]))
              result[candRow][candCol] = true;
          } else {
            if (IsRedOrEmpty(board[candRow][candCol]))
              result[candRow][candCol] = true;
          }
        }
      }
    }
  }
  return result;
}

Board<bool> PossibleMovesChariot(const Board<Piece>& board, Position pos) {
  Board<bool> result = MakeEmptyBoard();
  const Piece piece = board[pos.row][pos.col];
  bool isRed = IsRed(piece);

  // Upwards.
  for (int r = pos.row - 1; r >= 0; r--) {
    if (board[r][pos.col] == Piece::EMPTY) {
      result[r][pos.col] = true;
    } else {
      if (isRed) {
        if (IsBlack(board[r][pos.col])) result[r][pos.col] = true;
      } else {
        if (IsRed(board[r][pos.col])) result[r][pos.col] = true;
      }
      break;
    }
  }
  // Downwards.
  for (int r = pos.row + 1; r < kTotalRow; r++) {
    if (board[r][pos.col] == Piece::EMPTY) {
      result[r][pos.col] = true;
    } else {
      if (isRed) {
        if (IsBlack(board[r][pos.col])) result[r][pos.col] = true;
      } else {
        if (IsRed(board[r][pos.col])) result[r][pos.col] = true;
      }
      break;
    }
  }
  // Leftwards.
  for (int c = pos.col - 1; c >= 0; c--) {
    if (board[pos.row][c] == Piece::EMPTY) {
      result[pos.row][c] = true;
    } else {
      if (isRed) {
        if (IsBlack(board[pos.row][c])) result[pos.row][c] = true;
      } else {
        if (IsRed(board[pos.row][c])) result[pos.row][c] = true;
      }
      break;
    }
  }
  // Rightwards.
  for (int c = pos.col + 1; c < kTotalCol; c++) {
    if (board[pos.row][c] == Piece::EMPTY) {
      result[pos.row][c] = true;
    } else {
      if (isRed) {
        if (IsBlack(board[pos.row][c])) result[pos.row][c] = true;
      } else {
        if (IsRed(board[pos.row][c])) result[pos.row][c] = true;
      }
      break;
    }
  }

  return result;
}

Board<bool> PossibleMovesCannon(const Board<Piece>& board, Position pos) {
  Board<bool> result = MakeEmptyBoard();
  const Piece piece = board[pos.row][pos.col];
  bool isRed = IsRed(piece);

  // For each direction, first mark all empty squares.
  // Then, once a non-empty square (screen) is encountered,
  // continue until the first non-empty square after the screen which is
  // capturable.

  // Upwards.
  {
    bool screen_found = false;
    for (int r = pos.row - 1; r >= 0; r--) {
      if (!screen_found) {
        if (board[r][pos.col] == Piece::EMPTY) {
          result[r][pos.col] = true;
        } else {
          screen_found = true;
        }
      } else {
        if (board[r][pos.col] != Piece::EMPTY) {
          if (isRed) {
            if (IsBlack(board[r][pos.col])) result[r][pos.col] = true;
          } else {
            if (IsRed(board[r][pos.col])) result[r][pos.col] = true;
          }
          break;
        }
      }
    }
  }

  // Downwards.
  {
    bool screen_found = false;
    for (int r = pos.row + 1; r < kTotalRow; r++) {
      if (!screen_found) {
        if (board[r][pos.col] == Piece::EMPTY) {
          result[r][pos.col] = true;
        } else {
          screen_found = true;
        }
      } else {
        if (board[r][pos.col] != Piece::EMPTY) {
          if (isRed) {
            if (IsBlack(board[r][pos.col])) result[r][pos.col] = true;
          } else {
            if (IsRed(board[r][pos.col])) result[r][pos.col] = true;
          }
          break;
        }
      }
    }
  }

  // Leftwards.
  {
    bool screen_found = false;
    for (int c = pos.col - 1; c >= 0; c--) {
      if (!screen_found) {
        if (board[pos.row][c] == Piece::EMPTY) {
          result[pos.row][c] = true;
        } else {
          screen_found = true;
        }
      } else {
        if (board[pos.row][c] != Piece::EMPTY) {
          if (isRed) {
            if (IsBlack(board[pos.row][c])) result[pos.row][c] = true;
          } else {
            if (IsRed(board[pos.row][c])) result[pos.row][c] = true;
          }
          break;
        }
      }
    }
  }

  // Rightwards.
  {
    bool screen_found = false;
    for (int c = pos.col + 1; c < kTotalCol; c++) {
      if (!screen_found) {
        if (board[pos.row][c] == Piece::EMPTY) {
          result[pos.row][c] = true;
        } else {
          screen_found = true;
        }
      } else {
        if (board[pos.row][c] != Piece::EMPTY) {
          if (isRed) {
            if (IsBlack(board[pos.row][c])) result[pos.row][c] = true;
          } else {
            if (IsRed(board[pos.row][c])) result[pos.row][c] = true;
          }
          break;
        }
      }
    }
  }

  return result;
}

Board<bool> PossibleMovesSoldier(const Board<Piece>& board, Position pos) {
  Board<bool> result = MakeEmptyBoard();
  const Piece piece = board[pos.row][pos.col];
  bool isRed = IsRed(piece);

  // Forward move.
  int forwardRow = isRed ? pos.row - 1 : pos.row + 1;
  if (forwardRow >= 0 && forwardRow < kTotalRow) {
    if (isRed) {
      if (IsBlackOrEmpty(board[forwardRow][pos.col]))
        result[forwardRow][pos.col] = true;
    } else {
      if (IsRedOrEmpty(board[forwardRow][pos.col]))
        result[forwardRow][pos.col] = true;
    }
  }

  // Once soldier has crossed the river, add sideways moves.
  if (isRed) {
    // For red, crossing the river means row <= 4.
    if (pos.row <= 4) {
      if (pos.col - 1 >= 0) {
        if (IsBlackOrEmpty(board[pos.row][pos.col - 1]))
          result[pos.row][pos.col - 1] = true;
      }
      if (pos.col + 1 < kTotalCol) {
        if (IsBlackOrEmpty(board[pos.row][pos.col + 1]))
          result[pos.row][pos.col + 1] = true;
      }
    }
  } else {
    // For black, crossing the river means row >= 5.
    if (pos.row >= 5) {
      if (pos.col - 1 >= 0) {
        if (IsRedOrEmpty(board[pos.row][pos.col - 1]))
          result[pos.row][pos.col - 1] = true;
      }
      if (pos.col + 1 < kTotalCol) {
        if (IsRedOrEmpty(board[pos.row][pos.col + 1]))
          result[pos.row][pos.col + 1] = true;
      }
    }
  }

  return result;
}

}  // namespace xq::internal::util
