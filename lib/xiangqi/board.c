#include <string.h>

#include "xiangqi/board_c.h"
#include "xiangqi/types_c.h"

// --------------- Helper Function ---------------

bool IsPathClear(const BoardC board, const Position from, const Position to) {
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

bool ThreatensBySoldier(const enum Piece soldier, const Position pos,
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

bool ThreatensByHorse(const BoardC board, const Position pos,
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

bool ThreatensByCannon(const BoardC board, const Position pos,
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

// --------------- Public Function ---------------

void ClearBoard_C(BoardC board) { memset(board, 0, K_BOARD_SIZE); }

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
