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

bool IsBlack(Piece piece) {
  return static_cast<std::underlying_type_t<Piece>>(piece) < 0;
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
          return {Pos(row, col)};
        }
      }
    }
    for (uint8_t row = kBlackPalaceRowMin; row <= kBlackPalaceRowMax; row++) {
      for (uint8_t col = kPalaceColMin; col <= kPalaceColMax; col++) {
        if (board[row][col] == Piece::R_GENERAL) {
          return {Pos(row, col)};
        }
      }
    }
    return std::nullopt;
  }

  for (uint8_t row = kBlackPalaceRowMin; row <= kBlackPalaceRowMax; row++) {
    for (uint8_t col = kPalaceColMin; col <= kPalaceColMax; col++) {
      if (board[row][col] == Piece::B_GENERAL) {
        return {Pos(row, col)};
      }
    }
  }
  for (uint8_t row = kRedPalaceRowMax; row >= kRedPalaceRowMin; row--) {
    for (uint8_t col = kPalaceColMin; col <= kPalaceColMax; col++) {
      if (board[row][col] == Piece::B_GENERAL) {
        return {Pos(row, col)};
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
  const Piece piece = board[Row(pos)][Col(pos)];
  Board<bool> result{false};

  // Flying general check.
  const std::optional<Position> opponent_general =
      FindGeneral(board, piece == Piece::B_GENERAL);
  const uint8_t self_row = Row(pos);
  const uint8_t self_col = Col(pos);
  if (opponent_general.has_value() && Col(*opponent_general) == self_col) {
    const uint8_t op_row = Row(*opponent_general);
    const uint8_t op_col = Col(*opponent_general);
    const int8_t change = piece == Piece::B_GENERAL ? 1 : -1;
    bool is_blocked = false;
    // Start checking one square past the moving general.
    for (uint8_t row = self_row + change; row != op_row; row += change) {
      if (board[row][self_col] != Piece::EMPTY) {
        is_blocked = true;
        break;
      }
    }
    if (!is_blocked) {
      result[op_row][self_col] = true;
    }
  }

  int8_t row, col;
  if (piece == Piece::R_GENERAL) {
    // Move down:
    row = Row(pos) + 1;
    col = Col(pos);
    if (row <= kRedPalaceRowMax && !IsRed(board[row][col])) {
      result[row][col] = true;
    }
    // Move up:
    row = Row(pos) - 1;
    col = Col(pos);
    if (row >= kRedPalaceRowMin && !IsRed(board[row][col])) {
      result[row][col] = true;
    }
    // Move left:
    row = Row(pos);
    col = Col(pos) - 1;
    if (col >= kPalaceColMin && !IsRed(board[row][col])) {
      result[row][col] = true;
    }
    // Move right:
    row = Row(pos);
    col = Col(pos) + 1;
    if (col <= kPalaceColMax && !IsRed(board[row][col])) {
      result[row][col] = true;
    }
  } else {
    // Move down:
    row = Row(pos) + 1;
    col = Col(pos);
    if (row <= kBlackPalaceRowMax && !IsBlack(board[row][col])) {
      result[row][col] = true;
    }
    // Move up:
    row = Row(pos) - 1;
    col = Col(pos);
    if (row >= kBlackPalaceRowMin && !IsBlack(board[row][col])) {
      result[row][col] = true;
    }
    // Move left:
    row = Row(pos);
    col = Col(pos) - 1;
    if (col >= kPalaceColMin && !IsBlack(board[row][col])) {
      result[row][col] = true;
    }
    // Move right:
    row = Row(pos);
    col = Col(pos) + 1;
    if (col <= kPalaceColMax && !IsBlack(board[row][col])) {
      result[row][col] = true;
    }
  }
  return result;
}

Board<bool> PossibleMovesAdvisor(const Board<Piece>& board, Position pos) {
  Board<bool> result = MakeEmptyBoard();
  const Piece piece = board[Row(pos)][Col(pos)];
  bool isRed = IsRed(piece);

  // Advisors move one square diagonally.
  int dr[4] = {-1, -1, 1, 1};
  int dc[4] = {-1, 1, -1, 1};

  // Palace boundaries.
  int minRow = isRed ? 7 : 0;
  int maxRow = isRed ? 9 : 2;
  int minCol = 3, maxCol = 5;

  for (int i = 0; i < 4; i++) {
    int newRow = Row(pos) + dr[i];
    int newCol = Col(pos) + dc[i];
    if (newRow >= minRow && newRow <= maxRow && newCol >= minCol &&
        newCol <= maxCol) {
      if (isRed) {
        if (!IsRed(board[newRow][newCol])) {
          result[newRow][newCol] = true;
        }
      } else {
        if (!IsBlack(board[newRow][newCol])) {
          result[newRow][newCol] = true;
        }
      }
    }
  }
  return result;
}

Board<bool> PossibleMovesElephant(const Board<Piece>& board, Position pos) {
  Board<bool> result = MakeEmptyBoard();
  const Piece piece = board[Row(pos)][Col(pos)];
  bool isRed = IsRed(piece);

  // Elephants move two squares diagonally.
  int dr[4] = {-2, -2, 2, 2};
  int dc[4] = {-2, 2, -2, 2};

  for (int i = 0; i < 4; i++) {
    int newRow = Row(pos) + dr[i];
    int newCol = Col(pos) + dc[i];
    // Check board bounds.
    if (newRow < 0 || newRow >= kTotalRow || newCol < 0 || newCol >= kTotalCol)
      continue;
    // Elephants cannot cross the river.
    if (isRed && newRow < 5) continue;
    if (!isRed && newRow > 4) continue;
    // Check the intermediate square ("the eye").
    int midRow = Row(pos) + dr[i] / 2;
    int midCol = Col(pos) + dc[i] / 2;
    if (board[midRow][midCol] != Piece::EMPTY) continue;
    // Allow move if destination is empty or holds an enemy.
    if (isRed) {
      if (!IsRed(board[newRow][newCol])) {
        result[newRow][newCol] = true;
      }
    } else {
      if (!IsBlack(board[newRow][newCol])) {
        result[newRow][newCol] = true;
      }
    }
  }
  return result;
}

Board<bool> PossibleMovesHorse(const Board<Piece>& board, Position pos) {
  Board<bool> result = MakeEmptyBoard();
  const Piece piece = board[Row(pos)][Col(pos)];
  bool isRed = IsRed(piece);

  // For each of the four orthogonal directions, check if the "leg" is free.
  // Upward:
  if (Row(pos) - 1 >= 0 && board[Row(pos) - 1][Col(pos)] == Piece::EMPTY) {
    int candRow = Row(pos) - 2;
    if (candRow >= 0) {
      int candCols[2] = {Col(pos) - 1, Col(pos) + 1};
      for (int i = 0; i < 2; i++) {
        int candCol = candCols[i];
        if (candCol >= 0 && candCol < kTotalCol) {
          if (isRed) {
            if (!IsRed(board[candRow][candCol])) {
              result[candRow][candCol] = true;
            }
          } else {
            if (!IsBlack(board[candRow][candCol])) {
              result[candRow][candCol] = true;
            }
          }
        }
      }
    }
  }
  // Downward:
  if (Row(pos) + 1 < kTotalRow &&
      board[Row(pos) + 1][Col(pos)] == Piece::EMPTY) {
    int candRow = Row(pos) + 2;
    if (candRow < kTotalRow) {
      int candCols[2] = {Col(pos) - 1, Col(pos) + 1};
      for (int i = 0; i < 2; i++) {
        int candCol = candCols[i];
        if (candCol >= 0 && candCol < kTotalCol) {
          if (isRed) {
            if (!IsRed(board[candRow][candCol])) {
              result[candRow][candCol] = true;
            }
          } else {
            if (!IsBlack(board[candRow][candCol])) {
              result[candRow][candCol] = true;
            }
          }
        }
      }
    }
  }
  // Leftward:
  if (Col(pos) - 1 >= 0 && board[Row(pos)][Col(pos) - 1] == Piece::EMPTY) {
    int candCol = Col(pos) - 2;
    if (candCol >= 0) {
      int candRows[2] = {Row(pos) - 1, Row(pos) + 1};
      for (int i = 0; i < 2; i++) {
        int candRow = candRows[i];
        if (candRow >= 0 && candRow < kTotalRow) {
          if (isRed) {
            if (!IsRed(board[candRow][candCol])) {
              result[candRow][candCol] = true;
            }
          } else {
            if (!IsBlack(board[candRow][candCol])) {
              result[candRow][candCol] = true;
            }
          }
        }
      }
    }
  }
  // Rightward:
  if (Col(pos) + 1 < kTotalCol &&
      board[Row(pos)][Col(pos) + 1] == Piece::EMPTY) {
    int candCol = Col(pos) + 2;
    if (candCol < kTotalCol) {
      int candRows[2] = {Row(pos) - 1, Row(pos) + 1};
      for (int i = 0; i < 2; i++) {
        int candRow = candRows[i];
        if (candRow >= 0 && candRow < kTotalRow) {
          if (isRed) {
            if (!IsRed(board[candRow][candCol]))
              result[candRow][candCol] = true;
          } else {
            if (!IsBlack(board[candRow][candCol]))
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
  const Piece piece = board[Row(pos)][Col(pos)];
  bool isRed = IsRed(piece);

  // Upwards.
  for (int r = Row(pos) - 1; r >= 0; r--) {
    if (board[r][Col(pos)] == Piece::EMPTY) {
      result[r][Col(pos)] = true;
    } else {
      if (isRed) {
        if (IsBlack(board[r][Col(pos)])) result[r][Col(pos)] = true;
      } else {
        if (IsRed(board[r][Col(pos)])) result[r][Col(pos)] = true;
      }
      break;
    }
  }
  // Downwards.
  for (int r = Row(pos) + 1; r < kTotalRow; r++) {
    if (board[r][Col(pos)] == Piece::EMPTY) {
      result[r][Col(pos)] = true;
    } else {
      if (isRed) {
        if (IsBlack(board[r][Col(pos)])) result[r][Col(pos)] = true;
      } else {
        if (IsRed(board[r][Col(pos)])) result[r][Col(pos)] = true;
      }
      break;
    }
  }
  // Leftwards.
  for (int c = Col(pos) - 1; c >= 0; c--) {
    if (board[Row(pos)][c] == Piece::EMPTY) {
      result[Row(pos)][c] = true;
    } else {
      if (isRed) {
        if (IsBlack(board[Row(pos)][c])) result[Row(pos)][c] = true;
      } else {
        if (IsRed(board[Row(pos)][c])) result[Row(pos)][c] = true;
      }
      break;
    }
  }
  // Rightwards.
  for (int c = Col(pos) + 1; c < kTotalCol; c++) {
    if (board[Row(pos)][c] == Piece::EMPTY) {
      result[Row(pos)][c] = true;
    } else {
      if (isRed) {
        if (IsBlack(board[Row(pos)][c])) result[Row(pos)][c] = true;
      } else {
        if (IsRed(board[Row(pos)][c])) result[Row(pos)][c] = true;
      }
      break;
    }
  }

  return result;
}

Board<bool> PossibleMovesCannon(const Board<Piece>& board, Position pos) {
  Board<bool> result = MakeEmptyBoard();
  const Piece piece = board[Row(pos)][Col(pos)];
  bool isRed = IsRed(piece);

  // For each direction, first mark all empty squares.
  // Then, once a non-empty square (screen) is encountered,
  // continue until the first non-empty square after the screen which is
  // capturable.

  // Upwards.
  {
    bool screen_found = false;
    for (int r = Row(pos) - 1; r >= 0; r--) {
      if (!screen_found) {
        if (board[r][Col(pos)] == Piece::EMPTY) {
          result[r][Col(pos)] = true;
        } else {
          screen_found = true;
        }
      } else {
        if (board[r][Col(pos)] != Piece::EMPTY) {
          if (isRed) {
            if (IsBlack(board[r][Col(pos)])) result[r][Col(pos)] = true;
          } else {
            if (IsRed(board[r][Col(pos)])) result[r][Col(pos)] = true;
          }
          break;
        }
      }
    }
  }

  // Downwards.
  {
    bool screen_found = false;
    for (int r = Row(pos) + 1; r < kTotalRow; r++) {
      if (!screen_found) {
        if (board[r][Col(pos)] == Piece::EMPTY) {
          result[r][Col(pos)] = true;
        } else {
          screen_found = true;
        }
      } else {
        if (board[r][Col(pos)] != Piece::EMPTY) {
          if (isRed) {
            if (IsBlack(board[r][Col(pos)])) result[r][Col(pos)] = true;
          } else {
            if (IsRed(board[r][Col(pos)])) result[r][Col(pos)] = true;
          }
          break;
        }
      }
    }
  }

  // Leftwards.
  {
    bool screen_found = false;
    for (int c = Col(pos) - 1; c >= 0; c--) {
      if (!screen_found) {
        if (board[Row(pos)][c] == Piece::EMPTY) {
          result[Row(pos)][c] = true;
        } else {
          screen_found = true;
        }
      } else {
        if (board[Row(pos)][c] != Piece::EMPTY) {
          if (isRed) {
            if (IsBlack(board[Row(pos)][c])) result[Row(pos)][c] = true;
          } else {
            if (IsRed(board[Row(pos)][c])) result[Row(pos)][c] = true;
          }
          break;
        }
      }
    }
  }

  // Rightwards.
  {
    bool screen_found = false;
    for (int c = Col(pos) + 1; c < kTotalCol; c++) {
      if (!screen_found) {
        if (board[Row(pos)][c] == Piece::EMPTY) {
          result[Row(pos)][c] = true;
        } else {
          screen_found = true;
        }
      } else {
        if (board[Row(pos)][c] != Piece::EMPTY) {
          if (isRed) {
            if (IsBlack(board[Row(pos)][c])) result[Row(pos)][c] = true;
          } else {
            if (IsRed(board[Row(pos)][c])) result[Row(pos)][c] = true;
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
  const Piece piece = board[Row(pos)][Col(pos)];
  bool isRed = IsRed(piece);

  // Forward move.
  int forwardRow = isRed ? Row(pos) - 1 : Row(pos) + 1;
  if (forwardRow >= 0 && forwardRow < kTotalRow) {
    if (isRed) {
      if (!IsRed(board[forwardRow][Col(pos)]))
        result[forwardRow][Col(pos)] = true;
    } else {
      if (!IsBlack(board[forwardRow][Col(pos)]))
        result[forwardRow][Col(pos)] = true;
    }
  }

  // Once soldier has crossed the river, add sideways moves.
  if (isRed) {
    // For red, crossing the river means row <= 4.
    if (Row(pos) <= 4) {
      if (Col(pos) - 1 >= 0) {
        if (!IsRed(board[Row(pos)][Col(pos) - 1]))
          result[Row(pos)][Col(pos) - 1] = true;
      }
      if (Col(pos) + 1 < kTotalCol) {
        if (!IsRed(board[Row(pos)][Col(pos) + 1]))
          result[Row(pos)][Col(pos) + 1] = true;
      }
    }
  } else {
    // For black, crossing the river means row >= 5.
    if (Row(pos) >= 5) {
      if (Col(pos) - 1 >= 0) {
        if (!IsBlack(board[Row(pos)][Col(pos) - 1]))
          result[Row(pos)][Col(pos) - 1] = true;
      }
      if (Col(pos) + 1 < kTotalCol) {
        if (!IsBlack(board[Row(pos)][Col(pos) + 1]))
          result[Row(pos)][Col(pos) + 1] = true;
      }
    }
  }

  return result;
}

}  // namespace xq::internal::util
