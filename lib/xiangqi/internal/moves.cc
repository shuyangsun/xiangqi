// file: moves.cc

#include "xiangqi/internal/moves.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <optional>

namespace xq::internal::util {

namespace {

inline bool CanCapture(const Piece piece, const bool is_red) {
  return is_red ? !IsRed(piece) : !IsBlack(piece);
}

}  // namespace

std::array<Position, 5> PossibleMovesGeneral(const Board& board,
                                             const Position pos,
                                             const Position opponent_general) {
  std::array<Position, 5> result;
  result.fill(K_NO_POSITION);
  uint8_t res_idx = 0;

  // Flying general check.
  const Piece piece = board[pos];
  bool is_blocked = false;
  if (opponent_general != K_NO_POSITION && Col(opponent_general) == Col(pos)) {
    for (Position cur = std::min(pos, opponent_general) + K_TOTAL_COL;
         cur < std::max(pos, opponent_general); cur += K_TOTAL_COL) {
      if (!IsEmpty(board[cur])) {
        is_blocked = true;
        break;
      }
    }
    if (!is_blocked) {
      result[res_idx++] = opponent_general;
    }
  }

  if (piece == R_GENERAL) {
    if (((pos >= 66 && pos <= 68) || (pos >= 75 && pos <= 77)) &&
        !IsRed(board[pos + K_TOTAL_COL])) {
      // move down
      result[res_idx++] = pos + K_TOTAL_COL;
    }
    if (((pos >= 75 && pos <= 77) || (pos >= 84 && pos <= 86)) &&
        !IsRed(board[pos - K_TOTAL_COL])) {
      // move up
      result[res_idx++] = pos - K_TOTAL_COL;
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
  } else if (piece == B_GENERAL) {
    if (((pos >= 3 && pos <= 5) || (pos >= 12 && pos <= 14)) &&
        !IsBlack(board[pos + K_TOTAL_COL])) {
      // move down
      result[res_idx++] = pos + K_TOTAL_COL;
    }
    if (((pos >= 12 && pos <= 14) || (pos >= 21 && pos <= 23)) &&
        !IsBlack(board[pos - K_TOTAL_COL])) {
      // move up
      result[res_idx++] = pos - K_TOTAL_COL;
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

std::array<Position, 4> PossibleMovesAdvisor(const Board& board,
                                             const Position pos) {
  std::array<Position, 4> result;
  result.fill(K_NO_POSITION);
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

std::array<Position, 4> PossibleMovesElephant(const Board& board,
                                              const Position pos) {
  std::array<Position, 4> result;
  result.fill(K_NO_POSITION);

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

std::array<Position, 8> PossibleMovesHorse(const Board& board,
                                           const Position pos) {
  std::array<Position, 8> result;
  result.fill(K_NO_POSITION);
  const bool is_red = IsRed(board[pos]);
  const uint8_t col = Col(pos);
  uint8_t res_idx = 0;

  if (pos > 17) {                             // can move up 2
    if (IsEmpty(board[pos - K_TOTAL_COL])) {  // not blocked
      const Position up = pos - 2 * K_TOTAL_COL;
      if (col > 0 && CanCapture(board[up - 1], is_red)) {
        result[res_idx++] = up - 1;
      }
      if (col < K_TOTAL_COL - 1 && CanCapture(board[up + 1], is_red)) {
        result[res_idx++] = up + 1;
      }
    }
  }
  if (pos < 72) {                             // can move down 2
    if (IsEmpty(board[pos + K_TOTAL_COL])) {  // not blocked
      const Position down = pos + 2 * K_TOTAL_COL;
      if (col > 0 && CanCapture(board[down - 1], is_red)) {
        result[res_idx++] = down - 1;
      }
      if (col < K_TOTAL_COL - 1 && CanCapture(board[down + 1], is_red)) {
        result[res_idx++] = down + 1;
      }
    }
  }
  if (col > 1) {                    // can move left 2
    if (IsEmpty(board[pos - 1])) {  // not blocked
      const Position left = pos - 2;
      if (pos > 8 && CanCapture(board[left - K_TOTAL_COL], is_red)) {
        result[res_idx++] = left - K_TOTAL_COL;
      }
      if (pos < 81 && CanCapture(board[left + K_TOTAL_COL], is_red)) {
        result[res_idx++] = left + K_TOTAL_COL;
      }
    }
  }
  if (col < K_TOTAL_COL - 2) {      // can move right 2
    if (IsEmpty(board[pos + 1])) {  // not blocked
      const Position right = pos + 2;
      if (pos > 8 && CanCapture(board[right - K_TOTAL_COL], is_red)) {
        result[res_idx++] = right - K_TOTAL_COL;
      }
      if (pos < 81 && CanCapture(board[right + K_TOTAL_COL], is_red)) {
        result[res_idx++] = right + K_TOTAL_COL;
      }
    }
  }

  return result;
}

std::array<Position, 17> PossibleMovesChariot(const Board& board,
                                              const Position pos) {
  std::array<Position, 17> result;
  result.fill(K_NO_POSITION);
  const bool is_red = IsRed(board[pos]);
  uint8_t res_idx = 0;

  // Up
  for (int8_t cur = static_cast<int8_t>(pos) - K_TOTAL_COL; cur >= 0;
       cur -= K_TOTAL_COL) {
    if (CanCapture(board[cur], is_red)) {
      result[res_idx++] = static_cast<Position>(cur);
      if (!IsEmpty(board[cur])) {
        break;
      }
    } else {
      break;
    }
  }

  // Down
  for (int8_t cur = static_cast<int8_t>(pos) + K_TOTAL_COL; cur < K_BOARD_SIZE;
       cur += K_TOTAL_COL) {
    if (CanCapture(board[cur], is_red)) {
      result[res_idx++] = static_cast<Position>(cur);
      if (!IsEmpty(board[cur])) {
        break;
      }
    } else {
      break;
    }
  }

  // Left
  for (int8_t cur = static_cast<int8_t>(pos) - 1;
       cur >= (pos - pos % K_TOTAL_COL); cur--) {
    if (CanCapture(board[cur], is_red)) {
      result[res_idx++] = static_cast<Position>(cur);
      if (!IsEmpty(board[cur])) {
        break;
      }
    } else {
      break;
    }
  }

  // Right
  for (int8_t cur = static_cast<int8_t>(pos) + 1;
       cur < ((pos / K_TOTAL_COL + 1) * K_TOTAL_COL); cur++) {
    if (CanCapture(board[cur], is_red)) {
      result[res_idx++] = static_cast<Position>(cur);
      if (!IsEmpty(board[cur])) {
        break;
      }
    } else {
      break;
    }
  }

  return result;
}

std::array<Position, 17> PossibleMovesCannon(const Board& board,
                                             const Position pos) {
  std::array<Position, 17> result;
  result.fill(K_NO_POSITION);
  const bool is_red = IsRed(board[pos]);
  uint8_t res_idx = 0;

  // Up
  bool blocked = false;
  for (int8_t cur = static_cast<int8_t>(pos) - K_TOTAL_COL; cur >= 0;
       cur -= K_TOTAL_COL) {
    if (IsEmpty(board[cur])) {
      if (!blocked) {
        result[res_idx++] = static_cast<Position>(cur);
      }
    } else {
      if (blocked) {
        if (CanCapture(board[cur], is_red)) {
          result[res_idx++] = static_cast<Position>(cur);
        }
        break;
      } else {
        blocked = true;
      }
    }
  }

  // Down
  blocked = false;
  for (int8_t cur = static_cast<int8_t>(pos) + K_TOTAL_COL; cur < K_BOARD_SIZE;
       cur += K_TOTAL_COL) {
    if (IsEmpty(board[cur])) {
      if (!blocked) {
        result[res_idx++] = static_cast<Position>(cur);
      }
    } else {
      if (blocked) {
        if (CanCapture(board[cur], is_red)) {
          result[res_idx++] = static_cast<Position>(cur);
        }
        break;
      } else {
        blocked = true;
      }
    }
  }

  // Left
  blocked = false;
  for (int8_t cur = static_cast<int8_t>(pos) - 1;
       cur >= (pos - pos % K_TOTAL_COL); cur--) {
    if (IsEmpty(board[cur])) {
      if (!blocked) {
        result[res_idx++] = static_cast<Position>(cur);
      }
    } else {
      if (blocked) {
        if (CanCapture(board[cur], is_red)) {
          result[res_idx++] = static_cast<Position>(cur);
        }
        break;
      } else {
        blocked = true;
      }
    }
  }

  // Right
  blocked = false;
  for (int8_t cur = static_cast<int8_t>(pos) + 1;
       cur < ((pos / K_TOTAL_COL + 1) * K_TOTAL_COL); cur++) {
    if (IsEmpty(board[cur])) {
      if (!blocked) {
        result[res_idx++] = static_cast<Position>(cur);
      }
    } else {
      if (blocked) {
        if (CanCapture(board[cur], is_red)) {
          result[res_idx++] = static_cast<Position>(cur);
        }
        break;
      } else {
        blocked = true;
      }
    }
  }

  return result;
}

std::array<Position, 3> PossibleMovesSoldier(const Board& board,
                                             const Position pos) {
  std::array<Position, 3> result;
  result.fill(K_NO_POSITION);
  const bool is_red = IsRed(board[pos]);
  size_t res_idx = 0;

  if (is_red) {
    if (pos >= K_RED_RIVER_START &&
        CanCapture(board[pos - K_TOTAL_COL], is_red)) {
      result[0] = pos - K_TOTAL_COL;
      return result;
    } else {
      if (pos >= K_TOTAL_COL && CanCapture(board[pos - K_TOTAL_COL], is_red)) {
        result[res_idx++] = pos - K_TOTAL_COL;
      }
      const uint8_t col = Col(pos);
      if (col > 0 && CanCapture(board[pos - 1], is_red)) {
        result[res_idx++] = pos - 1;
      }
      if (col < K_TOTAL_COL - 1 && CanCapture(board[pos + 1], is_red)) {
        result[res_idx++] = pos + 1;
      }
      return result;
    }
  } else {
    if (pos < K_RED_RIVER_START &&
        CanCapture(board[pos + K_TOTAL_COL], is_red)) {
      result[0] = pos + K_TOTAL_COL;
      return result;
    } else {
      if (pos < 81 && CanCapture(board[pos + K_TOTAL_COL], is_red)) {
        result[res_idx++] = pos + K_TOTAL_COL;
      }
      const uint8_t col = Col(pos);
      if (col > 0 && CanCapture(board[pos - 1], is_red)) {
        result[res_idx++] = pos - 1;
      }
      if (col < K_TOTAL_COL - 1 && CanCapture(board[pos + 1], is_red)) {
        result[res_idx++] = pos + 1;
      }
      return result;
    }
  }
}

}  // namespace xq::internal::util
