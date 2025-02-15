// file: moves.cc

#include "xiangqi/internal/moves.h"

#include <algorithm>
#include <cstdint>
#include <optional>

namespace xq::internal::util {

namespace {

inline bool CanCapture(const Piece piece, const bool is_red) {
  return is_red ? !IsRed(piece) : !IsBlack(piece);
}

}  // namespace

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

std::array<Position, 4> PossibleMovesAdvisor(const Board<Piece>& board,
                                             const Position pos) {
  std::array<Position, 4> result;
  result.fill(kNoPosition);
  uint8_t res_idx = 0;

  if (IsRed(board[pos])) {
    switch (pos) {
      case 84:  // D9
      case 86:  // F9
      case 66:  // D7
      case 68:  // F7
        if (!IsRed(board[76])) {
          result[res_idx++] = 76;  // E8
        }
        break;
      case 76:  // E8
        if (!IsRed(board[84])) {
          result[res_idx++] = 84;
        }
        if (!IsRed(board[86])) {
          result[res_idx++] = 86;
        }
        if (!IsRed(board[66])) {
          result[res_idx++] = 66;
        }
        if (!IsRed(board[68])) {
          result[res_idx++] = 68;
        }
        break;
    }
  } else {
    switch (pos) {
      case 3:   // D0
      case 5:   // F0
      case 21:  // D2
      case 23:  // F2
        if (!IsBlack(board[13])) {
          result[res_idx++] = 13;  // E1
        }
        break;
      case 13:  // E1
        if (!IsBlack(board[3])) {
          result[res_idx++] = 3;
        }
        if (!IsBlack(board[5])) {
          result[res_idx++] = 5;
        }
        if (!IsBlack(board[21])) {
          result[res_idx++] = 21;
        }
        if (!IsBlack(board[23])) {
          result[res_idx++] = 23;
        }
        break;
    }
  }

  return result;
}

std::array<Position, 4> PossibleMovesElephant(const Board<Piece>& board,
                                              const Position pos) {
  std::array<Position, 4> result;
  result.fill(kNoPosition);

  uint8_t res_idx = 0;

  if (IsRed(board[pos])) {
    switch (pos) {
      case 83:  // C9
        if (!IsRed(board[63]) && IsEmpty(board[73])) {
          result[res_idx++] = 63;
        }
        if (!IsRed(board[67]) && IsEmpty(board[75])) {
          result[res_idx++] = 67;
        }
        break;
      case 87:  // G9
        if (!IsRed(board[71]) && IsEmpty(board[79])) {
          result[res_idx++] = 71;
        }
        if (!IsRed(board[67]) && IsEmpty(board[77])) {
          result[res_idx++] = 67;
        }
        break;
      case 63:  // A7
        if (!IsRed(board[47]) && IsEmpty(board[55])) {
          result[res_idx++] = 47;
        }
        if (!IsRed(board[83]) && IsEmpty(board[73])) {
          result[res_idx++] = 83;
        }
        break;
      case 71:  // I7
        if (!IsRed(board[51]) && IsEmpty(board[61])) {
          result[res_idx++] = 51;
        }
        if (!IsRed(board[87]) && IsEmpty(board[79])) {
          result[res_idx++] = 87;
        }
        break;
      case 47:  // C5
        if (!IsRed(board[63]) && IsEmpty(board[55])) {
          result[res_idx++] = 63;
        }
        if (!IsRed(board[67]) && IsEmpty(board[57])) {
          result[res_idx++] = 67;
        }
        break;
      case 51:  // G5
        if (!IsRed(board[71]) && IsEmpty(board[61])) {
          result[res_idx++] = 71;
        }
        if (!IsRed(board[67]) && IsEmpty(board[59])) {
          result[res_idx++] = 67;
        }
        break;
      case 67:  // E7
        if (!IsRed(board[47]) && IsEmpty(board[57])) {
          result[res_idx++] = 47;
        }
        if (!IsRed(board[83]) && IsEmpty(board[75])) {
          result[res_idx++] = 83;
        }
        if (!IsRed(board[51]) && IsEmpty(board[59])) {
          result[res_idx++] = 51;
        }
        if (!IsRed(board[87]) && IsEmpty(board[77])) {
          result[res_idx++] = 87;
        }
        break;
    }
  } else {
    switch (pos) {
      case 2:  // C0
        if (!IsBlack(board[18]) && IsEmpty(board[10])) {
          result[res_idx++] = 18;
        }
        if (!IsBlack(board[22]) && IsEmpty(board[12])) {
          result[res_idx++] = 22;
        }
        break;
      case 6:  // G0
        if (!IsBlack(board[26]) && IsEmpty(board[16])) {
          result[res_idx++] = 26;
        }
        if (!IsBlack(board[22]) && IsEmpty(board[14])) {
          result[res_idx++] = 22;
        }
        break;
      case 18:  // A2
        if (!IsBlack(board[2]) && IsEmpty(board[10])) {
          result[res_idx++] = 2;
        }
        if (!IsBlack(board[38]) && IsEmpty(board[28])) {
          result[res_idx++] = 38;
        }
        break;
      case 26:  // I2
        if (!IsBlack(board[6]) && IsEmpty(board[16])) {
          result[res_idx++] = 6;
        }
        if (!IsBlack(board[42]) && IsEmpty(board[34])) {
          result[res_idx++] = 42;
        }
        break;
      case 38:  // C4
        if (!IsBlack(board[18]) && IsEmpty(board[28])) {
          result[res_idx++] = 18;
        }
        if (!IsBlack(board[22]) && IsEmpty(board[30])) {
          result[res_idx++] = 22;
        }
        break;
      case 42:  // G4
        if (!IsBlack(board[26]) && IsEmpty(board[34])) {
          result[res_idx++] = 26;
        }
        if (!IsBlack(board[22]) && IsEmpty(board[32])) {
          result[res_idx++] = 22;
        }
        break;
      case 22:  // E2
        if (!IsBlack(board[2]) && IsEmpty(board[12])) {
          result[res_idx++] = 2;
        }
        if (!IsBlack(board[6]) && IsEmpty(board[14])) {
          result[res_idx++] = 6;
        }
        if (!IsBlack(board[38]) && IsEmpty(board[30])) {
          result[res_idx++] = 38;
        }
        if (!IsBlack(board[42]) && IsEmpty(board[32])) {
          result[res_idx++] = 42;
        }
        break;
    }
  }
  return result;
}

std::array<Position, 8> PossibleMovesHorse(const Board<Piece>& board,
                                           const Position pos) {
  std::array<Position, 8> result;
  result.fill(kNoPosition);
  const bool is_red = IsRed(board[pos]);
  const uint8_t col = Col(pos);
  uint8_t res_idx = 0;

  if (pos > 17) {                           // can move up 2
    if (IsEmpty(board[pos - kTotalCol])) {  // non-blocking
      const Position up = pos - 2 * kTotalCol;
      if (col > 0 && CanCapture(board[up - 1], is_red)) {
        result[res_idx++] = up - 1;
      }
      if (col < kTotalCol - 1 && CanCapture(board[up + 1], is_red)) {
        result[res_idx++] = up + 1;
      }
    }
  }
  if (pos < 72) {                           // can move down 2
    if (IsEmpty(board[pos + kTotalCol])) {  // non-blocking
      const Position down = pos + 2 * kTotalCol;
      if (col > 0 && CanCapture(board[down - 1], is_red)) {
        result[res_idx++] = down - 1;
      }
      if (col < kTotalCol - 1 && CanCapture(board[down + 1], is_red)) {
        result[res_idx++] = down + 1;
      }
    }
  }
  if (col > 1) {                    // can move left 2
    if (IsEmpty(board[pos - 1])) {  // non-blocking
      const Position left = pos - 2;
      if (pos > 8 && CanCapture(board[left - kTotalCol], is_red)) {
        result[res_idx++] = left - kTotalCol;
      }
      if (pos < 81 && CanCapture(board[left + kTotalCol], is_red)) {
        result[res_idx++] = left + kTotalCol;
      }
    }
  }
  if (col < kTotalCol - 2) {        // can move right 2
    if (IsEmpty(board[pos + 1])) {  // non-blocking
      const Position right = pos + 2;
      if (pos > 8 && CanCapture(board[right - kTotalCol], is_red)) {
        result[res_idx++] = right - kTotalCol;
      }
      if (pos < 81 && CanCapture(board[right + kTotalCol], is_red)) {
        result[res_idx++] = right + kTotalCol;
      }
    }
  }

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
