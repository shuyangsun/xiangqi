#include <string.h>

#include "xiangqi/board_c.h"
#include "xiangqi/types_c.h"

// --------------- Helper Function ---------------

#define CAN_CAPTURE(a, b) ((b) ? ((a) <= 0) : ((a) >= 0))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

static inline bool IsPathClear(const BoardC board, const Position from,
                               const Position to) {
  const uint8_t start = from < to ? from : to;
  const uint8_t end = from > to ? from : to;
  if (end - start < K_TOTAL_COL) {  // same row
    for (uint8_t pos = start + 1; pos < end; pos++) {
      if (board[pos] != PIECE_EMPTY) {
        return false;
      }
    }
    return true;
  } else if (Col(from) == Col(to)) {
    for (uint8_t pos = start + K_TOTAL_COL; pos < end; pos += K_TOTAL_COL) {
      if (board[pos] != PIECE_EMPTY) {
        return false;
      }
    }
    return true;
  }
  return false;
}

static inline bool ThreatensBySoldier(const enum Piece soldier,
                                      const Position pos,
                                      const Position target) {
  const uint8_t pos_col = Col(pos);
  return ((soldier == R_SOLDIER) &&
          ((pos == target + K_TOTAL_COL) ||                    // up
           (pos < K_RED_RIVER_START &&                         // crossed river
            ((pos_col < K_TOTAL_COL - 1 && pos + 1 == target)  // right
             || (pos_col > 0 && target + 1 == pos)             // left
             )))) ||
         ((soldier == B_SOLDIER) &&
          ((pos + K_TOTAL_COL == target) ||                    // down
           (pos >= K_RED_RIVER_START &&                        // crossed river
            ((pos_col < K_TOTAL_COL - 1 && pos + 1 == target)  // right
             || (pos_col > 0 && target + 1 == pos)             // left
             ))));
}

static inline bool ThreatensByHorse(const BoardC board, const Position pos,
                                    const Position target) {
  const uint8_t pos_row = Row(pos);
  const uint8_t pos_col = Col(pos);
  if ((target > pos + K_TOTAL_COL)    // target at least one row below
      && (pos_row < K_TOTAL_ROW - 2)  // can move down for 2 rows
      && board[pos + K_TOTAL_COL] == PIECE_EMPTY  // not blocked
  ) {
    const uint8_t down_2_row = pos + K_TOTAL_COL * 2;
    if ((pos_col > 0 && down_2_row - 1 == target)                   // left 1
        || (pos_col < K_TOTAL_COL - 1 && down_2_row + 1 == target)  // right 1
    ) {
      return true;
    }
  } else if ((target + K_TOTAL_COL < pos)  // target at least one row above
             && (pos_row > 1)              // can move up for 2 rows
             && (board[pos - K_TOTAL_COL] == PIECE_EMPTY)  // not blocked
  ) {
    const uint8_t up_2_row = pos - K_TOTAL_COL * 2;
    if ((pos_col > 0 && up_2_row == target + 1)                   // left 1
        || (pos_col < K_TOTAL_COL - 1 && up_2_row + 1 == target)  // right 1
    ) {
      return true;
    }
  }
  return (pos_col < K_TOTAL_COL - 2         // can move right for 2 columns
          && board[pos + 1] == PIECE_EMPTY  // not blocked
          && ((pos_row < K_TOTAL_ROW - 1 &&
               pos + K_TOTAL_COL + 2 == target)                    // down 1
              || (pos_row > 0 && target + K_TOTAL_COL - 2 == pos)  // up 1
              )) ||
         (pos_col > 1                       // can move left for 2 columns
          && board[pos - 1] == PIECE_EMPTY  // not blocked
          && ((pos_row < K_TOTAL_ROW - 1 &&
               pos + K_TOTAL_COL - 2 == target)                    // down 1
              || (pos_row > 0 && target + K_TOTAL_COL + 2 == pos)  // up 1
              ));
}

static inline bool ThreatensByCannon(const BoardC board, const Position pos,
                                     const Position target) {
  if (Row(pos) == Row(target)) {
    bool found_in_between = false;
    for (uint8_t p = (pos < target ? pos : target) + 1;
         p < (pos > target ? pos : target); p++) {
      if (board[p] != PIECE_EMPTY) {
        if (found_in_between) {
          return false;
        } else {
          found_in_between = true;
        }
      }
    }
    return found_in_between;
  } else if (Col(pos) == Col(target)) {
    bool found_in_between = false;
    for (uint8_t p = (pos < target ? pos : target) + K_TOTAL_COL;
         p < (pos > target ? pos : target); p += K_TOTAL_COL) {
      if (board[p] != PIECE_EMPTY) {
        if (found_in_between) {
          return false;
        } else {
          found_in_between = true;
        }
      }
    }
    return found_in_between;
  }
  return false;
}

static inline uint8_t PossibleMovesGeneral(const BoardC board,
                                           const Position pos,
                                           const Position opponent_general,
                                           Position* out) {
  uint8_t res = 0;

  // Flying general check.
  const enum Piece piece = board[pos];
  bool is_blocked = false;
  if (opponent_general != K_NO_POSITION && Col(opponent_general) == Col(pos)) {
    for (Position cur = MIN(pos, opponent_general) + K_TOTAL_COL;
         cur < MAX(pos, opponent_general); cur += K_TOTAL_COL) {
      if (!IsEmpty(board[cur])) {
        is_blocked = true;
        break;
      }
    }
    if (!is_blocked) {
      *(out + res++) = opponent_general;
    }
  }

  if (piece == R_GENERAL) {
    if (((pos >= 66 && pos <= 68) || (pos >= 75 && pos <= 77)) &&
        !IsRed(board[pos + K_TOTAL_COL])) {
      // move down
      *(out + res++) = pos + K_TOTAL_COL;
    }
    if (((pos >= 75 && pos <= 77) || (pos >= 84 && pos <= 86)) &&
        !IsRed(board[pos - K_TOTAL_COL])) {
      // move up
      *(out + res++) = pos - K_TOTAL_COL;
    }
    if ((pos == 66 || pos == 67 || pos == 75 || pos == 76 || pos == 84 ||
         pos == 85) &&
        !IsRed(board[pos + 1])) {
      // move right
      *(out + res++) = pos + 1;
    }
    if ((pos == 67 || pos == 68 || pos == 76 || pos == 77 || pos == 85 ||
         pos == 86) &&
        !IsRed(board[pos - 1])) {
      // move left
      *(out + res++) = pos - 1;
    }
  } else if (piece == B_GENERAL) {
    if (((pos >= 3 && pos <= 5) || (pos >= 12 && pos <= 14)) &&
        !IsBlack(board[pos + K_TOTAL_COL])) {
      // move down
      *(out + res++) = pos + K_TOTAL_COL;
    }
    if (((pos >= 12 && pos <= 14) || (pos >= 21 && pos <= 23)) &&
        !IsBlack(board[pos - K_TOTAL_COL])) {
      // move up
      *(out + res++) = pos - K_TOTAL_COL;
    }
    if ((pos == 3 || pos == 4 || pos == 12 || pos == 13 || pos == 21 ||
         pos == 22) &&
        !IsBlack(board[pos + 1])) {
      // move right
      *(out + res++) = pos + 1;
    }
    if ((pos == 4 || pos == 5 || pos == 13 || pos == 14 || pos == 22 ||
         pos == 23) &&
        !IsBlack(board[pos - 1])) {
      // move left
      *(out + res++) = pos - 1;
    }
  }
  return res;
}

static inline uint8_t PossibleMovesAdvisor(const BoardC board,
                                           const Position pos, Position* out) {
  uint8_t res = 0;

  if (IsRed(board[pos])) {
    switch (pos) {
      case 84:  // D9
      case 86:  // F9
      case 66:  // D7
      case 68:  // F7
        if (!IsRed(board[76])) {
          *(out + res++) = 76;  // E8
        }
        break;
      case 76:  // E8
        if (!IsRed(board[84])) {
          *(out + res++) = 84;
        }
        if (!IsRed(board[86])) {
          *(out + res++) = 86;
        }
        if (!IsRed(board[66])) {
          *(out + res++) = 66;
        }
        if (!IsRed(board[68])) {
          *(out + res++) = 68;
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
          *(out + res++) = 13;  // E1
        }
        break;
      case 13:  // E1
        if (!IsBlack(board[3])) {
          *(out + res++) = 3;
        }
        if (!IsBlack(board[5])) {
          *(out + res++) = 5;
        }
        if (!IsBlack(board[21])) {
          *(out + res++) = 21;
        }
        if (!IsBlack(board[23])) {
          *(out + res++) = 23;
        }
        break;
    }
  }

  return res;
}

static inline uint8_t PossibleMovesElephant(const BoardC board,
                                            const Position pos, Position* out) {
  uint8_t res = 0;

  if (IsRed(board[pos])) {
    switch (pos) {
      case 83:  // C9
        if (!IsRed(board[63]) && IsEmpty(board[73])) {
          *(out + res++) = 63;
        }
        if (!IsRed(board[67]) && IsEmpty(board[75])) {
          *(out + res++) = 67;
        }
        break;
      case 87:  // G9
        if (!IsRed(board[71]) && IsEmpty(board[79])) {
          *(out + res++) = 71;
        }
        if (!IsRed(board[67]) && IsEmpty(board[77])) {
          *(out + res++) = 67;
        }
        break;
      case 63:  // A7
        if (!IsRed(board[47]) && IsEmpty(board[55])) {
          *(out + res++) = 47;
        }
        if (!IsRed(board[83]) && IsEmpty(board[73])) {
          *(out + res++) = 83;
        }
        break;
      case 71:  // I7
        if (!IsRed(board[51]) && IsEmpty(board[61])) {
          *(out + res++) = 51;
        }
        if (!IsRed(board[87]) && IsEmpty(board[79])) {
          *(out + res++) = 87;
        }
        break;
      case 47:  // C5
        if (!IsRed(board[63]) && IsEmpty(board[55])) {
          *(out + res++) = 63;
        }
        if (!IsRed(board[67]) && IsEmpty(board[57])) {
          *(out + res++) = 67;
        }
        break;
      case 51:  // G5
        if (!IsRed(board[71]) && IsEmpty(board[61])) {
          *(out + res++) = 71;
        }
        if (!IsRed(board[67]) && IsEmpty(board[59])) {
          *(out + res++) = 67;
        }
        break;
      case 67:  // E7
        if (!IsRed(board[47]) && IsEmpty(board[57])) {
          *(out + res++) = 47;
        }
        if (!IsRed(board[83]) && IsEmpty(board[75])) {
          *(out + res++) = 83;
        }
        if (!IsRed(board[51]) && IsEmpty(board[59])) {
          *(out + res++) = 51;
        }
        if (!IsRed(board[87]) && IsEmpty(board[77])) {
          *(out + res++) = 87;
        }
        break;
    }
  } else {
    switch (pos) {
      case 2:  // C0
        if (!IsBlack(board[18]) && IsEmpty(board[10])) {
          *(out + res++) = 18;
        }
        if (!IsBlack(board[22]) && IsEmpty(board[12])) {
          *(out + res++) = 22;
        }
        break;
      case 6:  // G0
        if (!IsBlack(board[26]) && IsEmpty(board[16])) {
          *(out + res++) = 26;
        }
        if (!IsBlack(board[22]) && IsEmpty(board[14])) {
          *(out + res++) = 22;
        }
        break;
      case 18:  // A2
        if (!IsBlack(board[2]) && IsEmpty(board[10])) {
          *(out + res++) = 2;
        }
        if (!IsBlack(board[38]) && IsEmpty(board[28])) {
          *(out + res++) = 38;
        }
        break;
      case 26:  // I2
        if (!IsBlack(board[6]) && IsEmpty(board[16])) {
          *(out + res++) = 6;
        }
        if (!IsBlack(board[42]) && IsEmpty(board[34])) {
          *(out + res++) = 42;
        }
        break;
      case 38:  // C4
        if (!IsBlack(board[18]) && IsEmpty(board[28])) {
          *(out + res++) = 18;
        }
        if (!IsBlack(board[22]) && IsEmpty(board[30])) {
          *(out + res++) = 22;
        }
        break;
      case 42:  // G4
        if (!IsBlack(board[26]) && IsEmpty(board[34])) {
          *(out + res++) = 26;
        }
        if (!IsBlack(board[22]) && IsEmpty(board[32])) {
          *(out + res++) = 22;
        }
        break;
      case 22:  // E2
        if (!IsBlack(board[2]) && IsEmpty(board[12])) {
          *(out + res++) = 2;
        }
        if (!IsBlack(board[6]) && IsEmpty(board[14])) {
          *(out + res++) = 6;
        }
        if (!IsBlack(board[38]) && IsEmpty(board[30])) {
          *(out + res++) = 38;
        }
        if (!IsBlack(board[42]) && IsEmpty(board[32])) {
          *(out + res++) = 42;
        }
        break;
    }
  }
  return res;
}

static inline uint8_t PossibleMovesHorse(const BoardC board, const Position pos,
                                         Position* out) {
  const bool is_red = IsRed(board[pos]);
  const uint8_t col = Col(pos);
  uint8_t res = 0;

  if (pos > 17) {                             // can move up 2
    if (IsEmpty(board[pos - K_TOTAL_COL])) {  // not blocked
      const Position up = pos - 2 * K_TOTAL_COL;
      if (col > 0 && CAN_CAPTURE(board[up - 1], is_red)) {
        *(out + res++) = up - 1;
      }
      if (col < K_TOTAL_COL - 1 && CAN_CAPTURE(board[up + 1], is_red)) {
        *(out + res++) = up + 1;
      }
    }
  }
  if (pos < 72) {                             // can move down 2
    if (IsEmpty(board[pos + K_TOTAL_COL])) {  // not blocked
      const Position down = pos + 2 * K_TOTAL_COL;
      if (col > 0 && CAN_CAPTURE(board[down - 1], is_red)) {
        *(out + res++) = down - 1;
      }
      if (col < K_TOTAL_COL - 1 && CAN_CAPTURE(board[down + 1], is_red)) {
        *(out + res++) = down + 1;
      }
    }
  }
  if (col > 1) {                    // can move left 2
    if (IsEmpty(board[pos - 1])) {  // not blocked
      const Position left = pos - 2;
      if (pos > 8 && CAN_CAPTURE(board[left - K_TOTAL_COL], is_red)) {
        *(out + res++) = left - K_TOTAL_COL;
      }
      if (pos < 81 && CAN_CAPTURE(board[left + K_TOTAL_COL], is_red)) {
        *(out + res++) = left + K_TOTAL_COL;
      }
    }
  }
  if (col < K_TOTAL_COL - 2) {      // can move right 2
    if (IsEmpty(board[pos + 1])) {  // not blocked
      const Position right = pos + 2;
      if (pos > 8 && CAN_CAPTURE(board[right - K_TOTAL_COL], is_red)) {
        *(out + res++) = right - K_TOTAL_COL;
      }
      if (pos < 81 && CAN_CAPTURE(board[right + K_TOTAL_COL], is_red)) {
        *(out + res++) = right + K_TOTAL_COL;
      }
    }
  }

  return res;
}

static inline uint8_t PossibleMovesChariot(const BoardC board,
                                           const Position pos, Position* out) {
  const bool is_red = IsRed(board[pos]);
  uint8_t res = 0;

  // Up
  for (int8_t cur = ((int8_t)pos) - K_TOTAL_COL; cur >= 0; cur -= K_TOTAL_COL) {
    if (CAN_CAPTURE(board[cur], is_red)) {
      *(out + res++) = cur;
      if (!IsEmpty(board[cur])) {
        break;
      }
    } else {
      break;
    }
  }

  // Down
  for (int8_t cur = pos + K_TOTAL_COL; cur < K_BOARD_SIZE; cur += K_TOTAL_COL) {
    if (CAN_CAPTURE(board[cur], is_red)) {
      *(out + res++) = cur;
      if (!IsEmpty(board[cur])) {
        break;
      }
    } else {
      break;
    }
  }

  // Left
  for (int8_t cur = ((int8_t)pos) - 1; cur >= (pos - pos % K_TOTAL_COL);
       cur--) {
    if (CAN_CAPTURE(board[cur], is_red)) {
      *(out + res++) = cur;
      if (!IsEmpty(board[cur])) {
        break;
      }
    } else {
      break;
    }
  }

  // Right
  for (int8_t cur = pos + 1; cur < ((pos / K_TOTAL_COL + 1) * K_TOTAL_COL);
       cur++) {
    if (CAN_CAPTURE(board[cur], is_red)) {
      *(out + res++) = cur;
      if (!IsEmpty(board[cur])) {
        break;
      }
    } else {
      break;
    }
  }

  return res;
}

static inline uint8_t PossibleMovesCannon(const BoardC board,
                                          const Position pos, Position* out) {
  const bool is_red = IsRed(board[pos]);
  uint8_t res = 0;

  // Up
  bool blocked = false;
  for (int8_t cur = ((int8_t)pos) - K_TOTAL_COL; cur >= 0; cur -= K_TOTAL_COL) {
    if (IsEmpty(board[cur])) {
      if (!blocked) {
        *(out + res++) = cur;
      }
    } else {
      if (blocked) {
        if (CAN_CAPTURE(board[cur], is_red)) {
          *(out + res++) = cur;
        }
        break;
      } else {
        blocked = true;
      }
    }
  }

  // Down
  blocked = false;
  for (int8_t cur = pos + K_TOTAL_COL; cur < K_BOARD_SIZE; cur += K_TOTAL_COL) {
    if (IsEmpty(board[cur])) {
      if (!blocked) {
        *(out + res++) = cur;
      }
    } else {
      if (blocked) {
        if (CAN_CAPTURE(board[cur], is_red)) {
          *(out + res++) = cur;
        }
        break;
      } else {
        blocked = true;
      }
    }
  }

  // Left
  blocked = false;
  for (int8_t cur = ((int8_t)pos) - 1; cur >= (pos - pos % K_TOTAL_COL);
       cur--) {
    if (IsEmpty(board[cur])) {
      if (!blocked) {
        *(out + res++) = cur;
      }
    } else {
      if (blocked) {
        if (CAN_CAPTURE(board[cur], is_red)) {
          *(out + res++) = cur;
        }
        break;
      } else {
        blocked = true;
      }
    }
  }

  // Right
  blocked = false;
  for (int8_t cur = pos + 1; cur < ((pos / K_TOTAL_COL + 1) * K_TOTAL_COL);
       cur++) {
    if (IsEmpty(board[cur])) {
      if (!blocked) {
        *(out + res++) = cur;
      }
    } else {
      if (blocked) {
        if (CAN_CAPTURE(board[cur], is_red)) {
          *(out + res++) = cur;
        }
        break;
      } else {
        blocked = true;
      }
    }
  }

  return res;
}

static inline uint8_t PossibleMovesSoldier(const BoardC board,
                                           const Position pos, Position* out) {
  const bool is_red = IsRed(board[pos]);
  uint8_t res = 0;

  if (is_red) {
    if (pos >= K_RED_RIVER_START &&
        CAN_CAPTURE(board[pos - K_TOTAL_COL], is_red)) {
      *out = pos - K_TOTAL_COL;
      return 1;
    } else {
      if (pos >= K_TOTAL_COL && CAN_CAPTURE(board[pos - K_TOTAL_COL], is_red)) {
        *(out + res++) = pos - K_TOTAL_COL;
      }
      const uint8_t col = Col(pos);
      if (col > 0 && CAN_CAPTURE(board[pos - 1], is_red)) {
        *(out + res++) = pos - 1;
      }
      if (col < K_TOTAL_COL - 1 && CAN_CAPTURE(board[pos + 1], is_red)) {
        *(out + res++) = pos + 1;
      }
      return res;
    }
  } else {
    if (pos < K_RED_RIVER_START &&
        CAN_CAPTURE(board[pos + K_TOTAL_COL], is_red)) {
      *out = pos + K_TOTAL_COL;
      return 1;
    } else {
      if (pos < 81 && CAN_CAPTURE(board[pos + K_TOTAL_COL], is_red)) {
        *(out + res++) = pos + K_TOTAL_COL;
      }
      const uint8_t col = Col(pos);
      if (col > 0 && CAN_CAPTURE(board[pos - 1], is_red)) {
        *(out + res++) = pos - 1;
      }
      if (col < K_TOTAL_COL - 1 && CAN_CAPTURE(board[pos + 1], is_red)) {
        *(out + res++) = pos + 1;
      }
      return res;
    }
  }
}

// --------------- Public Function ---------------

void ClearBoard_C(BoardC board) { memset(board, 0, K_BOARD_SIZE); }

void ResetBoard_C(BoardC board) {
  memcpy(board, K_STARTING_BOARD, K_BOARD_SIZE);
}

void CopyBoard_C(BoardC dest, const BoardC src) {
  memcpy(dest, src, K_BOARD_SIZE);
}

Position FindGeneral_C(const BoardC board, const enum Player player) {
  const bool find_red = player == PLAYER_RED;
  const enum Piece general = find_red ? R_GENERAL : B_GENERAL;
  if (find_red) {
    // Find from bottom if find red.
    if (board[66] == general) {
      return 66;
    } else if (board[67] == general) {
      return 67;
    } else if (board[68] == general) {
      return 68;
    } else if (board[75] == general) {
      return 75;
    } else if (board[76] == general) {
      return 76;
    } else if (board[77] == general) {
      return 77;
    } else if (board[84] == general) {
      return 84;
    } else if (board[85] == general) {
      return 85;
    } else if (board[86] == general) {
      return 86;
    } else if (board[3] == general) {
      return 3;
    } else if (board[4] == general) {
      return 4;
    } else if (board[5] == general) {
      return 5;
    } else if (board[12] == general) {
      return 12;
    } else if (board[13] == general) {
      return 13;
    } else if (board[14] == general) {
      return 14;
    } else if (board[21] == general) {
      return 21;
    } else if (board[22] == general) {
      return 22;
    } else if (board[23] == general) {
      return 23;
    }
  } else {
    // Find from top if find black.
    if (board[3] == general) {
      return 3;
    } else if (board[4] == general) {
      return 4;
    } else if (board[5] == general) {
      return 5;
    } else if (board[12] == general) {
      return 12;
    } else if (board[13] == general) {
      return 13;
    } else if (board[14] == general) {
      return 14;
    } else if (board[21] == general) {
      return 21;
    } else if (board[22] == general) {
      return 22;
    } else if (board[23] == general) {
      return 23;
    } else if (board[66] == general) {
      return 66;
    } else if (board[67] == general) {
      return 67;
    } else if (board[68] == general) {
      return 68;
    } else if (board[75] == general) {
      return 75;
    } else if (board[76] == general) {
      return 76;
    } else if (board[77] == general) {
      return 77;
    } else if (board[84] == general) {
      return 84;
    } else if (board[85] == general) {
      return 85;
    } else if (board[86] == general) {
      return 86;
    }
  }
  return K_NO_POSITION;
}

bool IsBeingCheckmate_C(const BoardC board, const enum Player player) {
  const enum Piece general = (player == PLAYER_RED) ? R_GENERAL : B_GENERAL;
  const Position general_pos = FindGeneral_C(board, player);
  if (general_pos == K_NO_POSITION) {
    return true;
  }

  // Now scan the board for enemy pieces that might be threatening our
  // general.
  for (uint8_t pos = 0; pos < K_BOARD_SIZE; pos++) {
    const enum Piece piece = board[pos];
    if (piece == PIECE_EMPTY) {
      continue;
    }
    const bool piece_is_red = IsRed(piece);
    const bool player_is_red = player == PLAYER_RED;
    if (piece_is_red == player_is_red) {
      continue;  // Skip own piece
    }

    switch (piece > 0 ? piece : -piece) {
      case R_GENERAL:
      case R_CHARIOT:
        if (IsPathClear(board, pos, general_pos)) {
          return true;
        }
        break;
      case R_SOLDIER:
        if (ThreatensBySoldier(piece, pos, general_pos)) {
          return true;
        }
        break;
      case R_HORSE:
        if (ThreatensByHorse(board, pos, general_pos)) {
          return true;
        }
        break;
      case R_CANNON:
        if (ThreatensByCannon(board, pos, general_pos)) {
          return true;
        }
        break;
      default:
        continue;
    }
  }
  return false;
}

enum Winner GetWinner_C(const BoardC board) {
  const Position black_general_pos = FindGeneral_C(board, PLAYER_BLACK);
  if (black_general_pos == K_NO_POSITION) {
    return WINNER_RED;
  }
  const Position red_general_pos = FindGeneral_C(board, PLAYER_RED);
  if (red_general_pos == K_NO_POSITION) {
    return WINNER_BLACK;
  }
  return WINNER_NONE;
}

enum Piece Move_C(BoardC board, const Movement movement) {
  const Position from = Orig(movement);
  const Position to = Dest(movement);
  if (from == to) {
    return PIECE_EMPTY;
  }
  const enum Piece piece = board[from];
  if (piece == PIECE_EMPTY) {
    return PIECE_EMPTY;
  }
  const enum Piece captured = board[to];
  board[to] = piece;
  board[from] = PIECE_EMPTY;
  return captured;
}

void FlipBoard_C(BoardC dest, const BoardC src) {
  for (uint8_t pos = 0; pos < K_BOARD_SIZE / 2; pos++) {
    const uint8_t pos_mirror = K_BOARD_SIZE - 1 - pos;
    const enum Piece left_flipped = -src[pos];
    dest[pos] = -src[pos_mirror];
    dest[pos_mirror] = left_flipped;
  }
}

void MirrorBoardHorizontal_C(BoardC dest, const BoardC src) {
  for (uint8_t row = 0; row < K_TOTAL_ROW; row++) {
    const Position row_start = row * K_TOTAL_COL;
    for (uint8_t col = 0; col < K_TOTAL_COL / 2; col++) {
      const Position left_pos = row_start + col;
      const Position right_pos = row_start + K_TOTAL_COL - 1 - col;
      dest[left_pos] = src[right_pos];
      dest[right_pos] = src[left_pos];
    }
    dest[row_start + K_TOTAL_COL / 2] = src[row_start + K_TOTAL_COL / 2];
  }
}

void MirrorBoardVertical_C(BoardC dest, const BoardC src) {
  for (uint8_t row = 0; row < K_TOTAL_ROW / 2; row++) {
    const size_t top_start = row * K_TOTAL_COL;
    const size_t bottom_start = (K_TOTAL_ROW - 1 - row) * K_TOTAL_COL;
    memcpy((void*)dest + top_start, (void*)src + bottom_start, K_TOTAL_COL);
    memcpy((void*)dest + bottom_start, (void*)src + top_start, K_TOTAL_COL);
    for (uint8_t col = 0; col < K_TOTAL_COL; col++) {
      dest[top_start + col] = -dest[top_start + col];
      dest[bottom_start + col] = -dest[bottom_start + col];
    }
  }
}

void EncodeBoardState_C(const BoardC board, BoardStateC out) {
  uint64_t res1 = 0, res2 = 0, res3 = 0, res4 = 0;

  // Initialize all pieces as missing.
  uint8_t r_general_pos = 0xFF, b_general_pos = 0xFF;
  uint16_t r_advisor_poses = 0xFFFF, r_elephant_poses = 0xFFFF,
           r_horse_poses = 0xFFFF, r_chariot_poses = 0xFFFF,
           r_cannon_poses = 0xFFFF, b_advisor_poses = 0xFFFF,
           b_elephant_poses = 0xFFFF, b_horse_poses = 0xFFFF,
           b_chariot_poses = 0xFFFF, b_cannon_poses = 0xFFFF;

  uint8_t r_soldier_poses[5];
  memset(r_soldier_poses, K_NO_POSITION, 5);
  uint8_t b_soldier_poses[5];
  memset(b_soldier_poses, K_NO_POSITION, 5);
  uint8_t r_soldier_idx = 0, b_soldier_idx = 0;

  for (Position pos = 0; pos < K_BOARD_SIZE; pos++) {
    switch (board[pos]) {
      case PIECE_EMPTY: {
        continue;
      }
      case R_GENERAL: {
        r_general_pos = pos;
        break;
      }
      case B_GENERAL: {
        b_general_pos = pos;
        break;
      }
      case R_ADVISOR: {
        const uint16_t left_byte = r_advisor_poses & 0xFF00;
        const uint16_t cur_min = left_byte >> 8;
        r_advisor_poses = (pos < cur_min) ? ((((uint16_t)pos) << 8) | cur_min)
                                          : (left_byte | pos);
        break;
      }
      case B_ADVISOR: {
        const uint16_t left_byte = b_advisor_poses & 0xFF00;
        const uint16_t cur_min = left_byte >> 8;
        b_advisor_poses = (pos < cur_min) ? ((((uint16_t)pos) << 8) | cur_min)
                                          : (left_byte | pos);
        break;
      }
      case R_ELEPHANT: {
        const uint16_t left_byte = r_elephant_poses & 0xFF00;
        const uint16_t cur_min = left_byte >> 8;
        r_elephant_poses = (pos < cur_min) ? ((((uint16_t)pos) << 8) | cur_min)
                                           : (left_byte | pos);
        break;
      }
      case B_ELEPHANT: {
        const uint16_t left_byte = b_elephant_poses & 0xFF00;
        const uint16_t cur_min = left_byte >> 8;
        b_elephant_poses = (pos < cur_min) ? ((((uint16_t)pos) << 8) | cur_min)
                                           : (left_byte | pos);
        break;
      }
      case R_HORSE: {
        const uint16_t left_byte = r_horse_poses & 0xFF00;
        const uint16_t cur_min = left_byte >> 8;
        r_horse_poses = (pos < cur_min) ? ((((uint16_t)pos) << 8) | cur_min)
                                        : (left_byte | pos);
        break;
      }
      case B_HORSE: {
        const uint16_t left_byte = b_horse_poses & 0xFF00;
        const uint16_t cur_min = left_byte >> 8;
        b_horse_poses = (pos < cur_min) ? ((((uint16_t)pos) << 8) | cur_min)
                                        : (left_byte | pos);
        break;
      }
      case R_CHARIOT: {
        const uint16_t left_byte = r_chariot_poses & 0xFF00;
        const uint16_t cur_min = left_byte >> 8;
        r_chariot_poses = (pos < cur_min) ? ((((uint16_t)pos) << 8) | cur_min)
                                          : (left_byte | pos);
        break;
      }
      case B_CHARIOT: {
        const uint16_t left_byte = b_chariot_poses & 0xFF00;
        const uint16_t cur_min = left_byte >> 8;
        b_chariot_poses = (pos < cur_min) ? ((((uint16_t)pos) << 8) | cur_min)
                                          : (left_byte | pos);
        break;
      }
      case R_CANNON: {
        const uint16_t left_byte = r_cannon_poses & 0xFF00;
        const uint16_t cur_min = left_byte >> 8;
        r_cannon_poses = (pos < cur_min) ? ((((uint16_t)pos) << 8) | cur_min)
                                         : (left_byte | pos);
        break;
      }
      case B_CANNON: {
        const uint16_t left_byte = b_cannon_poses & 0xFF00;
        const uint16_t cur_min = left_byte >> 8;
        b_cannon_poses = (pos < cur_min) ? ((((uint16_t)pos) << 8) | cur_min)
                                         : (left_byte | pos);
        break;
      }
      case R_SOLDIER:
        r_soldier_poses[r_soldier_idx++] = pos;
        break;
      case B_SOLDIER:
        b_soldier_poses[b_soldier_idx++] = pos;
        break;
    }
  }

  res1 |= ((uint64_t)r_general_pos) << (7 * 8);
  res3 |= ((uint64_t)b_general_pos) << (7 * 8);
  res1 |= ((uint64_t)r_advisor_poses) << (5 * 8);
  res3 |= ((uint64_t)b_advisor_poses) << (5 * 8);
  res1 |= ((uint64_t)r_elephant_poses) << (3 * 8);
  res3 |= ((uint64_t)b_elephant_poses) << (3 * 8);
  res1 |= ((uint64_t)r_horse_poses) << 8;
  res3 |= ((uint64_t)b_horse_poses) << 8;
  res1 |= (uint64_t)(r_chariot_poses >> 8);
  res3 |= (uint64_t)(b_chariot_poses >> 8);

  res2 |= (uint64_t)(r_chariot_poses & 0x00FF) << (7 * 8);
  res4 |= (uint64_t)(b_chariot_poses & 0x00FF) << (7 * 8);

  res2 |= ((uint64_t)r_cannon_poses) << (5 * 8);
  res4 |= ((uint64_t)b_cannon_poses) << (5 * 8);

  res2 |= ((uint64_t)(r_soldier_poses[0]) << (4 * 8)) |
          ((uint64_t)(r_soldier_poses[1]) << (3 * 8)) |
          ((uint64_t)(r_soldier_poses[2]) << (2 * 8)) |
          ((uint64_t)(r_soldier_poses[3]) << 8) |
          (uint64_t)(r_soldier_poses[4]);
  res4 |= ((uint64_t)(b_soldier_poses[0]) << (4 * 8)) |
          ((uint64_t)(b_soldier_poses[1]) << (3 * 8)) |
          ((uint64_t)(b_soldier_poses[2]) << (2 * 8)) |
          ((uint64_t)(b_soldier_poses[3]) << 8) |
          (uint64_t)(b_soldier_poses[4]);

  out[0] = res1;
  out[1] = res2;
  out[2] = res3;
  out[3] = res4;
}

// Decode the encoded board state back to its original state.
void DecodeBoardState_C(const BoardStateC state, BoardC out) {
  ClearBoard_C(out);

  // For red pieces, the encoded state is split between state[0] (res1) and
  // state[1] (res2).
  uint64_t res1 = state[0];
  uint64_t res2 = state[1];

  // --- Red Pieces ---

  // Red General: bits 56-63 of res1.
  uint8_t r_general = (res1 >> 56) & 0xFF;
  if (r_general != 0xFF) {
    out[r_general] = R_GENERAL;
  }

  // Red Advisors: bits 48-55 and 40-47 of res1.
  uint8_t r_adv1 = (res1 >> 48) & 0xFF;
  if (r_adv1 != 0xFF) {
    out[r_adv1] = R_ADVISOR;
  }
  uint8_t r_adv2 = (res1 >> 40) & 0xFF;
  if (r_adv2 != 0xFF) {
    out[r_adv2] = R_ADVISOR;
  }

  // Red Elephants: bits 32-39 and 24-31 of res1.
  uint8_t r_ele1 = (res1 >> 32) & 0xFF;
  if (r_ele1 != 0xFF) {
    out[r_ele1] = R_ELEPHANT;
  }
  uint8_t r_ele2 = (res1 >> 24) & 0xFF;
  if (r_ele2 != 0xFF) {
    out[r_ele2] = R_ELEPHANT;
  }

  // Red Horses: bits 16-23 and 8-15 of res1.
  uint8_t r_horse1 = (res1 >> 16) & 0xFF;
  if (r_horse1 != 0xFF) {
    out[r_horse1] = R_HORSE;
  }
  uint8_t r_horse2 = (res1 >> 8) & 0xFF;
  if (r_horse2 != 0xFF) {
    out[r_horse2] = R_HORSE;
  }

  // Red Chariots:
  //   - One chariot: bits 0-7 of res1.
  uint8_t r_chariot1 = res1 & 0xFF;
  if (r_chariot1 != 0xFF) {
    out[r_chariot1] = R_CHARIOT;
  }
  //   - The other chariot: bits 56-63 of res2.
  uint8_t r_chariot2 = (res2 >> 56) & 0xFF;
  if (r_chariot2 != 0xFF) {
    out[r_chariot2] = R_CHARIOT;
  }

  // Red Cannons: bits 48-55 and 40-47 of res2.
  uint8_t r_cannon1 = (res2 >> 48) & 0xFF;
  if (r_cannon1 != 0xFF) {
    out[r_cannon1] = R_CANNON;
  }
  uint8_t r_cannon2 = (res2 >> 40) & 0xFF;
  if (r_cannon2 != 0xFF) {
    out[r_cannon2] = R_CANNON;
  }

  // Red Soldiers: bits 32-39, 24-31, 16-23, 8-15, and 0-7 of res2.
  uint8_t r_soldier[5];
  r_soldier[0] = (res2 >> 32) & 0xFF;
  r_soldier[1] = (res2 >> 24) & 0xFF;
  r_soldier[2] = (res2 >> 16) & 0xFF;
  r_soldier[3] = (res2 >> 8) & 0xFF;
  r_soldier[4] = res2 & 0xFF;
  for (int i = 0; i < 5; ++i) {
    if (r_soldier[i] != 0xFF) {
      const Position pos = r_soldier[i];
      out[pos] = R_SOLDIER;
    }
  }

  // For black pieces, the encoded state is in state[2] (res3) and state[3]
  // (res4).
  uint64_t res3 = state[2];
  uint64_t res4 = state[3];

  // --- Black Pieces ---

  // Black General: bits 56-63 of res3.
  uint8_t b_general = (res3 >> 56) & 0xFF;
  if (b_general != 0xFF) {
    out[b_general] = B_GENERAL;
  }

  // Black Advisors: bits 48-55 and 40-47 of res3.
  uint8_t b_adv1 = (res3 >> 48) & 0xFF;
  if (b_adv1 != 0xFF) {
    out[b_adv1] = B_ADVISOR;
  }
  uint8_t b_adv2 = (res3 >> 40) & 0xFF;
  if (b_adv2 != 0xFF) {
    out[b_adv2] = B_ADVISOR;
  }

  // Black Elephants: bits 32-39 and 24-31 of res3.
  uint8_t b_ele1 = (res3 >> 32) & 0xFF;
  if (b_ele1 != 0xFF) {
    out[b_ele1] = B_ELEPHANT;
  }
  uint8_t b_ele2 = (res3 >> 24) & 0xFF;
  if (b_ele2 != 0xFF) {
    out[b_ele2] = B_ELEPHANT;
  }

  // Black Horses: bits 16-23 and 8-15 of res3.
  uint8_t b_horse1 = (res3 >> 16) & 0xFF;
  if (b_horse1 != 0xFF) {
    out[b_horse1] = B_HORSE;
  }
  uint8_t b_horse2 = (res3 >> 8) & 0xFF;
  if (b_horse2 != 0xFF) {
    out[b_horse2] = B_HORSE;
  }

  // Black Chariots:
  //   - One from res3: bits 0-7.
  uint8_t b_chariot1 = res3 & 0xFF;
  if (b_chariot1 != 0xFF) {
    out[b_chariot1] = B_CHARIOT;
  }
  //   - The other from res4: bits 56-63.
  uint8_t b_chariot2 = (res4 >> 56) & 0xFF;
  if (b_chariot2 != 0xFF) {
    out[b_chariot2] = B_CHARIOT;
  }

  // Black Cannons: bits 48-55 and 40-47 of res4.
  uint8_t b_cannon1 = (res4 >> 48) & 0xFF;
  if (b_cannon1 != 0xFF) {
    out[b_cannon1] = B_CANNON;
  }
  uint8_t b_cannon2 = (res4 >> 40) & 0xFF;
  if (b_cannon2 != 0xFF) {
    out[b_cannon2] = B_CANNON;
  }

  // Black Soldiers: bits 32-39, 24-31, 16-23, 8-15, and 0-7 of res4.
  uint8_t b_soldier[5];
  b_soldier[0] = (res4 >> 32) & 0xFF;
  b_soldier[1] = (res4 >> 24) & 0xFF;
  b_soldier[2] = (res4 >> 16) & 0xFF;
  b_soldier[3] = (res4 >> 8) & 0xFF;
  b_soldier[4] = res4 & 0xFF;
  for (int i = 0; i < 5; ++i) {
    if (b_soldier[i] != 0xFF) {
      const Position pos = b_soldier[i];
      out[pos] = B_SOLDIER;
    }
  }
}

uint8_t PossibleMoves_C(const BoardC board, const Position pos,
                        const bool avoid_checkmate, MovesPerPieceC out) {
  memset(out, 0xFF, 17);
  uint8_t res = 0;

  const enum Piece piece = board[pos];
  switch (piece) {
    case PIECE_EMPTY:
      return res;
    case R_GENERAL:
      res = PossibleMovesGeneral(board, pos, FindGeneral_C(board, PLAYER_BLACK),
                                 out);
      break;
    case B_GENERAL:
      res = PossibleMovesGeneral(board, pos, FindGeneral_C(board, PLAYER_RED),
                                 out);
      break;
    case R_ADVISOR:
    case B_ADVISOR:
      res = PossibleMovesAdvisor(board, pos, out);
      break;
    case R_ELEPHANT:
    case B_ELEPHANT:
      res = PossibleMovesElephant(board, pos, out);
      break;
    case R_HORSE:
    case B_HORSE:
      res = PossibleMovesHorse(board, pos, out);
      break;
    case R_CHARIOT:
    case B_CHARIOT:
      res = PossibleMovesChariot(board, pos, out);
      break;
    case R_CANNON:
    case B_CANNON:
      res = PossibleMovesCannon(board, pos, out);
      break;
    case R_SOLDIER:
    case B_SOLDIER:
      res = PossibleMovesSoldier(board, pos, out);
      break;
    default:
      return res;
  }
  if (avoid_checkmate) {
    const enum Player player = IsRed(piece) ? PLAYER_RED : PLAYER_BLACK;
    for (size_t i = 0; i < res && *(out + i) != K_NO_POSITION; i++) {
      BoardC next;
      CopyBoard_C(next, board);
      const enum Piece captured = Move_C(next, NewMovement(pos, *(out + i)));
      if (captured == R_GENERAL || captured == B_GENERAL) {
        continue;
      }
      if (IsBeingCheckmate_C(next, player)) {
        // Replace the current result with the last one, and set the last one
        // to no position.
        *(out + i) = *(out + res - 1);
        *(out + res - 1) = K_NO_POSITION;
        res--;
        i--;
      }
    }
  }
  return res;
}

#undef CAN_CAPTURE
#undef MIN
#undef MAX
