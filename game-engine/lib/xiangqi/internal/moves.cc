// file: moves.cc

#include "xiangqi/internal/moves.h"

#include <algorithm>
#include <cstdint>
#include <optional>

namespace xq::internal::util {

std::array<Position, 5> PossibleMovesGeneral(const Board<Piece>& board,
                                             const Position pos,
                                             const Position opponent_general) {
  std::array<Position, 5> result;
  result.fill(kNoPosition);
  uint8_t res_idx = 0;

  // Flying general check.
  const Piece piece = board[pos];
  bool is_blocked = false;
  if (opponent_general != kNoPosition && Col(opponent_general) == Col(pos)) {
    for (Position cur = std::min(pos, opponent_general) + kTotalCol;
         cur < std::max(pos, opponent_general); cur += kTotalCol) {
      if (board[cur] != Piece::EMPTY) {
        is_blocked = true;
        break;
      }
    }
    if (!is_blocked) {
      result[res_idx++] = opponent_general;
    }
  }

  if (piece == Piece::R_GENERAL) {
    if (((pos >= 66 && pos <= 68) || (pos >= 75 && pos <= 77)) &&
        !IsRed(board[pos + kTotalCol])) {
      // move down
      result[res_idx++] = pos + kTotalCol;
    }
    if (((pos >= 75 && pos <= 77) || (pos >= 84 && pos <= 86)) &&
        !IsRed(board[pos - kTotalCol])) {
      // move up
      result[res_idx++] = pos - kTotalCol;
    }
    if ((pos == 66 || pos == 67 || pos == 75 || pos == 76 || pos == 84 ||
         pos == 85) &&
        !IsRed(board[pos + 1])) {
      // move right
      result[res_idx++] = pos + 1;
    }
    if ((pos == 67 || pos == 68 || pos == 76 || pos == 77 || pos == 85 ||
         pos == 86) &&
        !IsRed(board[pos - 1])) {
      // move left
      result[res_idx++] = pos - 1;
    }
  } else if (piece == Piece::B_GENERAL) {
    if (((pos >= 3 && pos <= 5) || (pos >= 12 && pos <= 14)) &&
        !IsBlack(board[pos + kTotalCol])) {
      // move down
      result[res_idx++] = pos + kTotalCol;
    }
    if (((pos >= 12 && pos <= 14) || (pos >= 21 && pos <= 23)) &&
        !IsBlack(board[pos - kTotalCol])) {
      // move up
      result[res_idx++] = pos - kTotalCol;
    }
    if ((pos == 3 || pos == 4 || pos == 12 || pos == 13 || pos == 21 ||
         pos == 22) &&
        !IsBlack(board[pos + 1])) {
      // move right
      result[res_idx++] = pos + 1;
    }
    if ((pos == 4 || pos == 5 || pos == 13 || pos == 14 || pos == 22 ||
         pos == 23) &&
        !IsBlack(board[pos - 1])) {
      // move left
      result[res_idx++] = pos - 1;
    }
  }
  return result;
}

std::array<Position, 2> PossibleMovesAdvisor(const Board<Piece>& board,
                                             const Position pos) {
  std::array<Position, 2> result;
  result.fill(kNoPosition);
  const Piece piece = board[pos];
  const bool isRed = IsRed(piece);

  // // Advisors move one square diagonally.
  // int dr[4] = {-1, -1, 1, 1};
  // int dc[4] = {-1, 1, -1, 1};

  // // Palace boundaries.
  // int minRow = isRed ? 7 : 0;
  // int maxRow = isRed ? 9 : 2;
  // int minCol = 3, maxCol = 5;

  // for (int i = 0; i < 4; i++) {
  //   int newRow = Row(pos) + dr[i];
  //   int newCol = Col(pos) + dc[i];
  //   if (newRow >= minRow && newRow <= maxRow && newCol >= minCol &&
  //       newCol <= maxCol) {
  //     if (isRed) {
  //       if (!IsRed(board[newRow][newCol])) {
  //         result[newRow][newCol] = true;
  //       }
  //     } else {
  //       if (!IsBlack(board[newRow][newCol])) {
  //         result[newRow][newCol] = true;
  //       }
  //     }
  //   }
  // }
  return result;
}

std::array<Position, 4> PossibleMovesElephant(const Board<Piece>& board,
                                              Position pos) {
  std::array<Position, 4> result;
  result.fill(kNoPosition);
  const Piece piece = board[pos];
  const bool isRed = IsRed(piece);

  // // Elephants move two squares diagonally.
  // int dr[4] = {-2, -2, 2, 2};
  // int dc[4] = {-2, 2, -2, 2};

  // for (int i = 0; i < 4; i++) {
  //   int newRow = Row(pos) + dr[i];
  //   int newCol = Col(pos) + dc[i];
  //   // Check board bounds.
  //   if (newRow < 0 || newRow >= kTotalRow || newCol < 0 || newCol >=
  //   kTotalCol)
  //     continue;
  //   // Elephants cannot cross the river.
  //   if (isRed && newRow < 5) continue;
  //   if (!isRed && newRow > 4) continue;
  //   // Check the intermediate square ("the eye").
  //   int midRow = Row(pos) + dr[i] / 2;
  //   int midCol = Col(pos) + dc[i] / 2;
  //   if (board[midRow][midCol] != Piece::EMPTY) continue;
  //   // Allow move if destination is empty or holds an enemy.
  //   if (isRed) {
  //     if (!IsRed(board[newRow][newCol])) {
  //       result[newRow][newCol] = true;
  //     }
  //   } else {
  //     if (!IsBlack(board[newRow][newCol])) {
  //       result[newRow][newCol] = true;
  //     }
  //   }
  // }
  return result;
}

std::array<Position, 8> PossibleMovesHorse(const Board<Piece>& board,
                                           Position pos) {
  std::array<Position, 8> result;
  result.fill(kNoPosition);
  const Piece piece = board[pos];
  const bool isRed = IsRed(piece);

  //   // For each of the four orthogonal directions, check if the "leg" is
  //   free.
  //   // Upward:
  //   if (Row(pos) - 1 >= 0 && board[Row(pos) - 1][Col(pos)] == Piece::EMPTY) {
  //     int candRow = Row(pos) - 2;
  //     if (candRow >= 0) {
  //       int candCols[2] = {Col(pos) - 1, Col(pos) + 1};
  //       for (int i = 0; i < 2; i++) {
  //         int candCol = candCols[i];
  //         if (candCol >= 0 && candCol < kTotalCol) {
  //           if (isRed) {
  //             if (!IsRed(board[candRow][candCol])) {
  //               result[candRow][candCol] = true;
  //             }
  //           } else {
  //             if (!IsBlack(board[candRow][candCol])) {
  //               result[candRow][candCol] = true;
  //             }
  //           }
  //         }
  //       }
  //     }
  //   }
  //   // Downward:
  //   if (Row(pos) + 1 < kTotalRow &&
  //       board[Row(pos) + 1][Col(pos)] == Piece::EMPTY) {
  //     int candRow = Row(pos) + 2;
  //     if (candRow < kTotalRow) {
  //       int candCols[2] = {Col(pos) - 1, Col(pos) + 1};
  //       for (int i = 0; i < 2; i++) {
  //         int candCol = candCols[i];
  //         if (candCol >= 0 && candCol < kTotalCol) {
  //           if (isRed) {
  //             if (!IsRed(board[candRow][candCol])) {
  //               result[candRow][candCol] = true;
  //             }
  //           } else {
  //             if (!IsBlack(board[candRow][candCol])) {
  //               result[candRow][candCol] = true;
  //             }
  //           }
  //         }
  //       }
  //     }
  //   }
  //   // Leftward:
  //   if (Col(pos) - 1 >= 0 && board[Row(pos)][Col(pos) - 1] == Piece::EMPTY) {
  //     int candCol = Col(pos) - 2;
  //     if (candCol >= 0) {
  //       int candRows[2] = {Row(pos) - 1, Row(pos) + 1};
  //       for (int i = 0; i < 2; i++) {
  //         int candRow = candRows[i];
  //         if (candRow >= 0 && candRow < kTotalRow) {
  //           if (isRed) {
  //             if (!IsRed(board[candRow][candCol])) {
  //               result[candRow][candCol] = true;
  //             }
  //           } else {
  //             if (!IsBlack(board[candRow][candCol])) {
  //               result[candRow][candCol] = true;
  //             }
  //           }
  //         }
  //       }
  //     }
  //   }
  //   // Rightward:
  //   if (Col(pos) + 1 < kTotalCol &&
  //       board[Row(pos)][Col(pos) + 1] == Piece::EMPTY) {
  //     int candCol = Col(pos) + 2;
  //     if (candCol < kTotalCol) {
  //       int candRows[2] = {Row(pos) - 1, Row(pos) + 1};
  //       for (int i = 0; i < 2; i++) {
  //         int candRow = candRows[i];
  //         if (candRow >= 0 && candRow < kTotalRow) {
  //           if (isRed) {
  //             if (!IsRed(board[candRow][candCol]))
  //               result[candRow][candCol] = true;
  //           } else {
  //             if (!IsBlack(board[candRow][candCol]))
  //               result[candRow][candCol] = true;
  //           }
  //         }
  //       }
  //     }
  //   }
  return result;
}

std::array<Position, 17> PossibleMovesChariot(const Board<Piece>& board,
                                              Position pos) {
  std::array<Position, 17> result;
  result.fill(kNoPosition);
  const Piece piece = board[pos];
  const bool isRed = IsRed(piece);

  //   Board<bool> result = MakeEmptyBoard();
  //   const Piece piece = board[pos];
  //   bool isRed = IsRed(piece);

  //   // Upwards.
  //   for (int r = Row(pos) - 1; r >= 0; r--) {
  //     if (board[r][Col(pos)] == Piece::EMPTY) {
  //       result[r][Col(pos)] = true;
  //     } else {
  //       if (isRed) {
  //         if (IsBlack(board[r][Col(pos)])) result[r][Col(pos)] = true;
  //       } else {
  //         if (IsRed(board[r][Col(pos)])) result[r][Col(pos)] = true;
  //       }
  //       break;
  //     }
  //   }
  //   // Downwards.
  //   for (int r = Row(pos) + 1; r < kTotalRow; r++) {
  //     if (board[r][Col(pos)] == Piece::EMPTY) {
  //       result[r][Col(pos)] = true;
  //     } else {
  //       if (isRed) {
  //         if (IsBlack(board[r][Col(pos)])) result[r][Col(pos)] = true;
  //       } else {
  //         if (IsRed(board[r][Col(pos)])) result[r][Col(pos)] = true;
  //       }
  //       break;
  //     }
  //   }
  //   // Leftwards.
  //   for (int c = Col(pos) - 1; c >= 0; c--) {
  //     if (board[Row(pos)][c] == Piece::EMPTY) {
  //       result[Row(pos)][c] = true;
  //     } else {
  //       if (isRed) {
  //         if (IsBlack(board[Row(pos)][c])) result[Row(pos)][c] = true;
  //       } else {
  //         if (IsRed(board[Row(pos)][c])) result[Row(pos)][c] = true;
  //       }
  //       break;
  //     }
  //   }
  //   // Rightwards.
  //   for (int c = Col(pos) + 1; c < kTotalCol; c++) {
  //     if (board[Row(pos)][c] == Piece::EMPTY) {
  //       result[Row(pos)][c] = true;
  //     } else {
  //       if (isRed) {
  //         if (IsBlack(board[Row(pos)][c])) result[Row(pos)][c] = true;
  //       } else {
  //         if (IsRed(board[Row(pos)][c])) result[Row(pos)][c] = true;
  //       }
  //       break;
  //     }
  //   }

  return result;
}

std::array<Position, 17> PossibleMovesCannon(const Board<Piece>& board,
                                             Position pos) {
  std::array<Position, 17> result;
  result.fill(kNoPosition);
  const Piece piece = board[pos];
  const bool isRed = IsRed(piece);

  // // For each direction, first mark all empty squares.
  // // Then, once a non-empty square (screen) is encountered,
  // // continue until the first non-empty square after the screen which is
  // // capturable.

  // // Upwards.
  // {
  //   bool screen_found = false;
  //   for (int r = Row(pos) - 1; r >= 0; r--) {
  //     if (!screen_found) {
  //       if (board[r][Col(pos)] == Piece::EMPTY) {
  //         result[r][Col(pos)] = true;
  //       } else {
  //         screen_found = true;
  //       }
  //     } else {
  //       if (board[r][Col(pos)] != Piece::EMPTY) {
  //         if (isRed) {
  //           if (IsBlack(board[r][Col(pos)])) result[r][Col(pos)] = true;
  //         } else {
  //           if (IsRed(board[r][Col(pos)])) result[r][Col(pos)] = true;
  //         }
  //         break;
  //       }
  //     }
  //   }
  // }

  // // Downwards.
  // {
  //   bool screen_found = false;
  //   for (int r = Row(pos) + 1; r < kTotalRow; r++) {
  //     if (!screen_found) {
  //       if (board[r][Col(pos)] == Piece::EMPTY) {
  //         result[r][Col(pos)] = true;
  //       } else {
  //         screen_found = true;
  //       }
  //     } else {
  //       if (board[r][Col(pos)] != Piece::EMPTY) {
  //         if (isRed) {
  //           if (IsBlack(board[r][Col(pos)])) result[r][Col(pos)] = true;
  //         } else {
  //           if (IsRed(board[r][Col(pos)])) result[r][Col(pos)] = true;
  //         }
  //         break;
  //       }
  //     }
  //   }
  // }

  // // Leftwards.
  // {
  //   bool screen_found = false;
  //   for (int c = Col(pos) - 1; c >= 0; c--) {
  //     if (!screen_found) {
  //       if (board[Row(pos)][c] == Piece::EMPTY) {
  //         result[Row(pos)][c] = true;
  //       } else {
  //         screen_found = true;
  //       }
  //     } else {
  //       if (board[Row(pos)][c] != Piece::EMPTY) {
  //         if (isRed) {
  //           if (IsBlack(board[Row(pos)][c])) result[Row(pos)][c] = true;
  //         } else {
  //           if (IsRed(board[Row(pos)][c])) result[Row(pos)][c] = true;
  //         }
  //         break;
  //       }
  //     }
  //   }
  // }

  // // Rightwards.
  // {
  //   bool screen_found = false;
  //   for (int c = Col(pos) + 1; c < kTotalCol; c++) {
  //     if (!screen_found) {
  //       if (board[Row(pos)][c] == Piece::EMPTY) {
  //         result[Row(pos)][c] = true;
  //       } else {
  //         screen_found = true;
  //       }
  //     } else {
  //       if (board[Row(pos)][c] != Piece::EMPTY) {
  //         if (isRed) {
  //           if (IsBlack(board[Row(pos)][c])) result[Row(pos)][c] = true;
  //         } else {
  //           if (IsRed(board[Row(pos)][c])) result[Row(pos)][c] = true;
  //         }
  //         break;
  //       }
  //     }
  //   }
  // }

  return result;
}

std::array<Position, 3> PossibleMovesSoldier(const Board<Piece>& board,
                                             Position pos) {
  std::array<Position, 3> result;
  result.fill(kNoPosition);
  const Piece piece = board[pos];
  const bool isRed = IsRed(piece);

  // // Forward move.
  // int forwardRow = isRed ? Row(pos) - 1 : Row(pos) + 1;
  // if (forwardRow >= 0 && forwardRow < kTotalRow) {
  //   if (isRed) {
  //     if (!IsRed(board[forwardRow][Col(pos)]))
  //       result[forwardRow][Col(pos)] = true;
  //   } else {
  //     if (!IsBlack(board[forwardRow][Col(pos)]))
  //       result[forwardRow][Col(pos)] = true;
  //   }
  // }

  // // Once soldier has crossed the river, add sideways moves.
  // if (isRed) {
  //   // For red, crossing the river means row <= 4.
  //   if (Row(pos) <= 4) {
  //     if (Col(pos) - 1 >= 0) {
  //       if (!IsRed(board[Row(pos)][Col(pos) - 1]))
  //         result[Row(pos)][Col(pos) - 1] = true;
  //     }
  //     if (Col(pos) + 1 < kTotalCol) {
  //       if (!IsRed(board[Row(pos)][Col(pos) + 1]))
  //         result[Row(pos)][Col(pos) + 1] = true;
  //     }
  //   }
  // } else {
  //   // For black, crossing the river means row >= 5.
  //   if (Row(pos) >= 5) {
  //     if (Col(pos) - 1 >= 0) {
  //       if (!IsBlack(board[Row(pos)][Col(pos) - 1]))
  //         result[Row(pos)][Col(pos) - 1] = true;
  //     }
  //     if (Col(pos) + 1 < kTotalCol) {
  //       if (!IsBlack(board[Row(pos)][Col(pos) + 1]))
  //         result[Row(pos)][Col(pos) + 1] = true;
  //     }
  //   }
  // }

  return result;
}

}  // namespace xq::internal::util
