#include "xiangqi/board.h"

#include <algorithm>
#include <cstdint>
#include <vector>

#include "xiangqi/internal/moves.h"
#include "xiangqi/types.h"

namespace xq {

namespace {

bool IsPathClear(const Board<Piece>& board, const Position from,
                 const Position to) {
  const uint8_t start = std::min(from, to);
  const uint8_t end = std::max(from, to);
  if (end - start < kTotalCol) {  // same row
    for (uint8_t pos = start + 1; pos < end; pos++) {
      if (board[pos] != Piece::EMPTY) return false;
    }
    return true;
  } else if (Col(from) == Col(to)) {
    for (uint8_t pos = start + kTotalCol; pos < end; pos += kTotalCol) {
      if (board[pos] != Piece::EMPTY) return false;
    }
    return true;
  }
  return false;
}

bool ThreatensBySoldier(const Piece soldier, const Position pos,
                        const Position target) {
  const uint8_t pos_col = Col(pos);
  return ((soldier == Piece::R_SOLDIER) &&
          ((pos == target + kTotalCol) ||                    // up
           (pos < kRedRiverStart &&                          // crossed river
            ((pos_col < kTotalCol - 1 && pos + 1 == target)  // right
             || (pos_col > 0 && target + 1 == pos)           // left
             )))) ||
         ((soldier == Piece::B_SOLDIER) &&
          ((pos + kTotalCol == target) ||                    // down
           (pos >= kRedRiverStart &&                         // crossed river
            ((pos_col < kTotalCol - 1 && pos + 1 == target)  // right
             || (pos_col > 0 && target + 1 == pos)           // left
             ))));
}

bool ThreatensByHorse(const Board<Piece>& board, const Position pos,
                      const Position target) {
  const uint8_t pos_row = Row(pos);
  const uint8_t pos_col = Col(pos);
  if ((target > pos + kTotalCol)    // target at least one row below
      && (pos_row < kTotalRow - 2)  // can move down for 2 rows
      && board[pos + kTotalCol] == Piece::EMPTY  // not blocked
  ) {
    const uint8_t down_2_row = target + kTotalCol * 2;
    if ((pos_col > 0 && down_2_row - 1 == target)                 // left 1
        || (pos_col < kTotalCol - 1 && down_2_row + 1 == target)  // right 1
    ) {
      return true;  // down right
    }
  } else if ((target + kTotalCol < pos)  // target at least one row above
             && (pos_row > 1)            // can move up for 2 rows
             && (board[pos - kTotalCol] == Piece::EMPTY)  // not blocked
  ) {
    const uint8_t up_2_row = target - kTotalCol * 2;
    if ((pos_col > 0 && up_2_row == target + 1)                 // left 1
        || (pos_col < kTotalCol - 1 && up_2_row + 1 == target)  // right 1
    ) {
      return true;
    }
  }
  return ((pos_col < kTotalCol - 2)            // can move right for 2 columns
          && (board[pos + 1] == Piece::EMPTY)  // not blocked
          &&
          ((pos_row < kTotalRow - 1 && pos + kTotalCol + 2 == target)  // down 1
           || (pos_row > 0 && target + kTotalCol - 2 == pos)           // up 1
           )) ||
         ((pos_col > 1)                        // can move left for 2 columns
          && (board[pos - 1] == Piece::EMPTY)  // not blocked
          &&
          ((pos_row < kTotalRow - 1 && pos + kTotalCol - 2 == target)  // down 1
           || (pos_row > 0 && target + kTotalCol + 2 == pos)           // up 1
           ));
}

bool ThreatensByCannon(const Board<Piece>& board, const Position pos,
                       const Position target) {
  if (Row(pos) == Row(target)) {
    bool found_in_between = false;
    for (uint8_t p = std::min(pos, target) + 1; p < std::max(pos, target);
         p++) {
      if (board[p] != Piece::EMPTY) {
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
    for (uint8_t p = std::min(pos, target) + kTotalCol;
         p < std::max(pos, target); p += kTotalCol) {
      if (board[p] != Piece::EMPTY) {
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

}  // namespace

bool IsCheckMade(const Board<Piece>& board, Player player) {
  using enum Piece;
  Position general_pos = kNoPosition;
  const Piece general =
      (player == Player::RED) ? Piece::R_GENERAL : Piece::B_GENERAL;
  if (player == Player::RED) {
    if (board[66] == general) {
      general_pos = 66;
    } else if (board[67] == general) {
      general_pos = 67;
    } else if (board[68] == general) {
      general_pos = 68;
    } else if (board[75] == general) {
      general_pos = 75;
    } else if (board[76] == general) {
      general_pos = 76;
    } else if (board[77] == general) {
      general_pos = 77;
    } else if (board[84] == general) {
      general_pos = 84;
    } else if (board[85] == general) {
      general_pos = 85;
    } else if (board[86] == general) {
      general_pos = 86;
    }
  } else if (player == Player::BLACK) {
    if (board[3] == general) {
      general_pos = 3;
    } else if (board[4] == general) {
      general_pos = 4;
    } else if (board[5] == general) {
      general_pos = 5;
    } else if (board[12] == general) {
      general_pos = 12;
    } else if (board[13] == general) {
      general_pos = 13;
    } else if (board[14] == general) {
      general_pos = 14;
    } else if (board[21] == general) {
      general_pos = 21;
    } else if (board[22] == general) {
      general_pos = 22;
    } else if (board[23] == general) {
      general_pos = 23;
    }
  }
  if (general_pos == kNoPosition) {
    return true;
  }

  // Now scan the board for enemy pieces that might be threatening our
  // general.
  for (uint8_t pos = 0; pos < kBoardSize; ++pos) {
    const Piece piece = board[pos];
    if (piece == Piece::EMPTY) {
      continue;
    }
    const bool piece_is_red = IsRed(piece);
    const bool player_is_red = player == Player::RED;
    if (piece_is_red == player_is_red) {
      continue;  // Skip own piece
    }

    switch (std::abs(static_cast<std::underlying_type_t<Piece>>(piece))) {
      case static_cast<std::underlying_type_t<Piece>>(R_GENERAL):
      case static_cast<std::underlying_type_t<Piece>>(R_CHARIOT):
        if (IsPathClear(board, pos, general_pos)) {
          return true;
        }
        break;
      case static_cast<std::underlying_type_t<Piece>>(R_SOLDIER):
        if (ThreatensBySoldier(piece, pos, general_pos)) {
          return true;
        }
        break;
      case static_cast<std::underlying_type_t<Piece>>(R_HORSE):
        if (ThreatensByHorse(board, pos, general_pos)) {
          return true;
        }
        break;
      case static_cast<std::underlying_type_t<Piece>>(R_CANNON):
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

Winner GetWinner(const Board<Piece>& board) {
  using enum Piece;
  if (!(board[66] == R_GENERAL || board[67] == R_GENERAL ||
        board[68] == R_GENERAL || board[75] == R_GENERAL ||
        board[76] == R_GENERAL || board[77] == R_GENERAL ||
        board[84] == R_GENERAL || board[85] == R_GENERAL ||
        board[86] == R_GENERAL)) {
    return Winner::BLACK;
  }
  if (!(board[3] == B_GENERAL || board[4] == B_GENERAL ||
        board[5] == B_GENERAL || board[12] == B_GENERAL ||
        board[13] == B_GENERAL || board[14] == B_GENERAL ||
        board[21] == B_GENERAL || board[22] == B_GENERAL ||
        board[23] == B_GENERAL)) {
    return Winner::RED;
  }
  return Winner::NONE;
}

Position FlipPosition(const Position position) {
  if (position == kNoPosition) {
    return kNoPosition;
  }
  return kBoardSize - position - 1;
}

Board<Piece> FlipBoard(const Board<Piece>& board) {
  Board<Piece> result = board;
  for (uint8_t pos = 0; pos < kBoardSize / 2; pos++) {
    const uint8_t pos_mirror = kBoardSize - 1 - pos;
    const Piece left_flipped = static_cast<Piece>(
        -static_cast<std::underlying_type_t<Piece>>(board[pos]));
    const Piece right_flipped = static_cast<Piece>(
        -static_cast<std::underlying_type_t<Piece>>(board[pos_mirror]));
    result[pos] = right_flipped;
    result[pos_mirror] = left_flipped;
  }
  return result;
}

std::array<uint64_t, 4> EncodeBoardState(const Board<Piece>& board) {
  using enum Piece;

  uint64_t res1 = 0, res2 = 0, res3 = 0, res4 = 0;

  // Initialize all pieces as missing.
  uint8_t r_general_pos = 0xFF, b_general_pos = 0xFF;
  uint16_t r_advisor_poses = 0xFFFF, r_elephant_poses = 0xFFFF,
           r_horse_poses = 0xFFFF, r_chariot_poses = 0xFFFF,
           r_cannon_poses = 0xFFFF, b_advisor_poses = 0xFFFF,
           b_elephant_poses = 0xFFFF, b_horse_poses = 0xFFFF,
           b_chariot_poses = 0xFFFF, b_cannon_poses = 0xFFFF;

  std::array<uint8_t, 5> r_soldier_poses{};
  r_soldier_poses.fill(0xFF);
  std::array<uint8_t, 5> b_soldier_poses{};
  b_soldier_poses.fill(0xFF);
  uint8_t r_soldier_idx = 0, b_soldier_idx = 0;

  for (Position pos = 0; pos < kBoardSize; pos++) {
    switch (board[pos]) {
      case EMPTY: {
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
        r_advisor_poses = (pos < cur_min)
                              ? (static_cast<uint16_t>(pos << 8) | cur_min)
                              : (left_byte | pos);
        break;
      }
      case B_ADVISOR: {
        const uint16_t left_byte = b_advisor_poses & 0xFF00;
        const uint16_t cur_min = left_byte >> 8;
        b_advisor_poses = (pos < cur_min)
                              ? (static_cast<uint16_t>(pos << 8) | cur_min)
                              : (left_byte | pos);
        break;
      }
      case R_ELEPHANT: {
        const uint16_t left_byte = r_elephant_poses & 0xFF00;
        const uint16_t cur_min = left_byte >> 8;
        r_elephant_poses = (pos < cur_min)
                               ? (static_cast<uint16_t>(pos << 8) | cur_min)
                               : (left_byte | pos);
        break;
      }
      case B_ELEPHANT: {
        const uint16_t left_byte = b_elephant_poses & 0xFF00;
        const uint16_t cur_min = left_byte >> 8;
        b_elephant_poses = (pos < cur_min)
                               ? (static_cast<uint16_t>(pos << 8) | cur_min)
                               : (left_byte | pos);
        break;
      }
      case R_HORSE: {
        const uint16_t left_byte = r_horse_poses & 0xFF00;
        const uint16_t cur_min = left_byte >> 8;
        r_horse_poses = (pos < cur_min)
                            ? (static_cast<uint16_t>(pos << 8) | cur_min)
                            : (left_byte | pos);
        break;
      }
      case B_HORSE: {
        const uint16_t left_byte = b_horse_poses & 0xFF00;
        const uint16_t cur_min = left_byte >> 8;
        b_horse_poses = (pos < cur_min)
                            ? (static_cast<uint16_t>(pos << 8) | cur_min)
                            : (left_byte | pos);
        break;
      }
      case R_CHARIOT: {
        const uint16_t left_byte = r_chariot_poses & 0xFF00;
        const uint16_t cur_min = left_byte >> 8;
        r_chariot_poses = (pos < cur_min)
                              ? (static_cast<uint16_t>(pos << 8) | cur_min)
                              : (left_byte | pos);
        break;
      }
      case B_CHARIOT: {
        const uint16_t left_byte = b_chariot_poses & 0xFF00;
        const uint16_t cur_min = left_byte >> 8;
        b_chariot_poses = (pos < cur_min)
                              ? (static_cast<uint16_t>(pos << 8) | cur_min)
                              : (left_byte | pos);
        break;
      }
      case R_CANNON: {
        const uint16_t left_byte = r_cannon_poses & 0xFF00;
        const uint16_t cur_min = left_byte >> 8;
        r_cannon_poses = (pos < cur_min)
                             ? (static_cast<uint16_t>(pos << 8) | cur_min)
                             : (left_byte | pos);
        break;
      }
      case B_CANNON: {
        const uint16_t left_byte = b_cannon_poses & 0xFF00;
        const uint16_t cur_min = left_byte >> 8;
        b_cannon_poses = (pos < cur_min)
                             ? (static_cast<uint16_t>(pos << 8) | cur_min)
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

  std::sort(r_soldier_poses.begin(), r_soldier_poses.end());
  std::sort(b_soldier_poses.begin(), b_soldier_poses.end());

  res1 |= static_cast<uint64_t>(r_general_pos) << (7 * 8);
  res3 |= static_cast<uint64_t>(b_general_pos) << (7 * 8);
  res1 |= static_cast<uint64_t>(r_advisor_poses) << (5 * 8);
  res3 |= static_cast<uint64_t>(b_advisor_poses) << (5 * 8);
  res1 |= static_cast<uint64_t>(r_elephant_poses) << (3 * 8);
  res3 |= static_cast<uint64_t>(b_elephant_poses) << (3 * 8);
  res1 |= static_cast<uint64_t>(r_horse_poses) << 8;
  res3 |= static_cast<uint64_t>(b_horse_poses) << 8;
  res1 |= static_cast<uint64_t>(r_chariot_poses >> 8);
  res3 |= static_cast<uint64_t>(b_chariot_poses >> 8);

  res2 |= static_cast<uint64_t>(r_chariot_poses & 0x00FF) << (7 * 8);
  res4 |= static_cast<uint64_t>(b_chariot_poses & 0x00FF) << (7 * 8);

  res2 |= static_cast<uint64_t>(r_cannon_poses) << (5 * 8);
  res4 |= static_cast<uint64_t>(b_cannon_poses) << (5 * 8);

  res2 |= (static_cast<uint64_t>(r_soldier_poses[0]) << (4 * 8)) |
          (static_cast<uint64_t>(r_soldier_poses[1]) << (3 * 8)) |
          (static_cast<uint64_t>(r_soldier_poses[2]) << (2 * 8)) |
          (static_cast<uint64_t>(r_soldier_poses[3]) << 8) |
          static_cast<uint64_t>(r_soldier_poses[4]);
  res4 |= (static_cast<uint64_t>(b_soldier_poses[0]) << (4 * 8)) |
          (static_cast<uint64_t>(b_soldier_poses[1]) << (3 * 8)) |
          (static_cast<uint64_t>(b_soldier_poses[2]) << (2 * 8)) |
          (static_cast<uint64_t>(b_soldier_poses[3]) << 8) |
          static_cast<uint64_t>(b_soldier_poses[4]);

  return {res1, res2, res3, res4};
}

Board<Piece> DecodeBoardState(const std::array<uint64_t, 4> state) {
  // Initialize an empty board.
  Board<Piece> board;
  board.fill(Piece::EMPTY);

  // For red pieces, the encoded state is split between state[0] (res1) and
  // state[1] (res2).
  uint64_t res1 = state[0];
  uint64_t res2 = state[1];

  // --- Red Pieces ---

  // Red General: bits 56-63 of res1.
  uint8_t r_general = (res1 >> 56) & 0xFF;
  if (r_general != 0xFF) {
    board[r_general] = Piece::R_GENERAL;
  }

  // Red Advisors: bits 48-55 and 40-47 of res1.
  uint8_t r_adv1 = (res1 >> 48) & 0xFF;
  if (r_adv1 != 0xFF) {
    board[r_adv1] = Piece::R_ADVISOR;
  }
  uint8_t r_adv2 = (res1 >> 40) & 0xFF;
  if (r_adv2 != 0xFF) {
    board[r_adv2] = Piece::R_ADVISOR;
  }

  // Red Elephants: bits 32-39 and 24-31 of res1.
  uint8_t r_ele1 = (res1 >> 32) & 0xFF;
  if (r_ele1 != 0xFF) {
    board[r_ele1] = Piece::R_ELEPHANT;
  }
  uint8_t r_ele2 = (res1 >> 24) & 0xFF;
  if (r_ele2 != 0xFF) {
    board[r_ele2] = Piece::R_ELEPHANT;
  }

  // Red Horses: bits 16-23 and 8-15 of res1.
  uint8_t r_horse1 = (res1 >> 16) & 0xFF;
  if (r_horse1 != 0xFF) {
    board[r_horse1] = Piece::R_HORSE;
  }
  uint8_t r_horse2 = (res1 >> 8) & 0xFF;
  if (r_horse2 != 0xFF) {
    board[r_horse2] = Piece::R_HORSE;
  }

  // Red Chariots:
  //   - One chariot: bits 0-7 of res1.
  uint8_t r_chariot1 = res1 & 0xFF;
  if (r_chariot1 != 0xFF) {
    board[r_chariot1] = Piece::R_CHARIOT;
  }
  //   - The other chariot: bits 56-63 of res2.
  uint8_t r_chariot2 = (res2 >> 56) & 0xFF;
  if (r_chariot2 != 0xFF) {
    board[r_chariot2] = Piece::R_CHARIOT;
  }

  // Red Cannons: bits 48-55 and 40-47 of res2.
  uint8_t r_cannon1 = (res2 >> 48) & 0xFF;
  if (r_cannon1 != 0xFF) {
    board[r_cannon1] = Piece::R_CANNON;
  }
  uint8_t r_cannon2 = (res2 >> 40) & 0xFF;
  if (r_cannon2 != 0xFF) {
    board[r_cannon2] = Piece::R_CANNON;
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
      board[pos] = Piece::R_SOLDIER;
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
    board[b_general] = Piece::B_GENERAL;
  }

  // Black Advisors: bits 48-55 and 40-47 of res3.
  uint8_t b_adv1 = (res3 >> 48) & 0xFF;
  if (b_adv1 != 0xFF) {
    board[b_adv1] = Piece::B_ADVISOR;
  }
  uint8_t b_adv2 = (res3 >> 40) & 0xFF;
  if (b_adv2 != 0xFF) {
    board[b_adv2] = Piece::B_ADVISOR;
  }

  // Black Elephants: bits 32-39 and 24-31 of res3.
  uint8_t b_ele1 = (res3 >> 32) & 0xFF;
  if (b_ele1 != 0xFF) {
    board[b_ele1] = Piece::B_ELEPHANT;
  }
  uint8_t b_ele2 = (res3 >> 24) & 0xFF;
  if (b_ele2 != 0xFF) {
    board[b_ele2] = Piece::B_ELEPHANT;
  }

  // Black Horses: bits 16-23 and 8-15 of res3.
  uint8_t b_horse1 = (res3 >> 16) & 0xFF;
  if (b_horse1 != 0xFF) {
    board[b_horse1] = Piece::B_HORSE;
  }
  uint8_t b_horse2 = (res3 >> 8) & 0xFF;
  if (b_horse2 != 0xFF) {
    board[b_horse2] = Piece::B_HORSE;
  }

  // Black Chariots:
  //   - One from res3: bits 0-7.
  uint8_t b_chariot1 = res3 & 0xFF;
  if (b_chariot1 != 0xFF) {
    board[b_chariot1] = Piece::B_CHARIOT;
  }
  //   - The other from res4: bits 56-63.
  uint8_t b_chariot2 = (res4 >> 56) & 0xFF;
  if (b_chariot2 != 0xFF) {
    board[b_chariot2] = Piece::B_CHARIOT;
  }

  // Black Cannons: bits 48-55 and 40-47 of res4.
  uint8_t b_cannon1 = (res4 >> 48) & 0xFF;
  if (b_cannon1 != 0xFF) {
    board[b_cannon1] = Piece::B_CANNON;
  }
  uint8_t b_cannon2 = (res4 >> 40) & 0xFF;
  if (b_cannon2 != 0xFF) {
    board[b_cannon2] = Piece::B_CANNON;
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
      board[pos] = Piece::B_SOLDIER;
    }
  }

  return board;
}

std::array<Position, 17> PossibleMoves(const Board<Piece>& board,
                                       Position pos) {
  using namespace xq::internal::util;
  using enum Piece;

  const Piece piece = board[pos];
  std::array<Position, 17> result;
  result.fill(kNoPosition);
  switch (piece) {
    case EMPTY:
      return result;
    case R_GENERAL:
      memcpy(PossibleMovesGeneral(board, pos, FindGeneral(board, Player::BLACK))
                 .data(),
             result.data(), 5);
      break;
    case B_GENERAL:
      memcpy(PossibleMovesGeneral(board, pos, FindGeneral(board, Player::RED))
                 .data(),
             result.data(), 5);
      break;
    case R_ADVISOR:
    case B_ADVISOR:
      memcpy(PossibleMovesAdvisor(board, pos).data(), result.data(), 2);
      break;
    case R_ELEPHANT:
    case B_ELEPHANT:
      memcpy(PossibleMovesElephant(board, pos).data(), result.data(), 4);
      break;
    case R_HORSE:
    case B_HORSE:
      memcpy(PossibleMovesHorse(board, pos).data(), result.data(), 8);
      break;
    case R_CHARIOT:
    case B_CHARIOT:
      memcpy(PossibleMovesChariot(board, pos).data(), result.data(), 17);
      break;
    case R_CANNON:
    case B_CANNON:
      memcpy(PossibleMovesCannon(board, pos).data(), result.data(), 17);
      break;
    case R_SOLDIER:
    case B_SOLDIER:
      memcpy(PossibleMovesSoldier(board, pos).data(), result.data(), 3);
      break;
    default:
      return result;
  }
  return result;
}

Piece Move(Board<Piece>& board, Position from, Position to) {
  using enum Player;
  if (from == to) {
    return Piece::EMPTY;
  }
  const Piece piece = board[from];
  if (piece == Piece::EMPTY) {
    return Piece::EMPTY;
  }
  const Piece captured = board[to];
  board[to] = piece;
  board[from] = Piece::EMPTY;
  return captured;
}

// Returns a vector of all possible moves for player.
std::vector<uint16_t> AllPossibleNextMoves(const Board<Piece>& board,
                                           Player player) {
  using namespace xq::internal::util;
  using enum Piece;

  std::vector<uint16_t> result;
  for (Position pos = 0; pos < kBoardSize; pos++) {
    const Piece piece = board[pos];
    if (piece == EMPTY || IsRed(piece) != (player == Player::RED)) {
      continue;
    }
    const auto from_16bit = static_cast<uint16_t>(pos << 8);
    switch (piece) {
      case R_GENERAL:
      case B_GENERAL:
        for (const Position to : PossibleMovesGeneral(board, pos)) {
          if (to == kNoPosition) {
            break;
          }
          result.emplace_back(from_16bit | static_cast<uint16_t>(to));
        }
        break;
      case R_ADVISOR:
      case B_ADVISOR:
        for (const Position to : PossibleMovesAdvisor(board, pos)) {
          if (to == kNoPosition) {
            break;
          }
          result.emplace_back(from_16bit | static_cast<uint16_t>(to));
        }
        break;
      case R_ELEPHANT:
      case B_ELEPHANT:
        for (const Position to : PossibleMovesElephant(board, pos)) {
          if (to == kNoPosition) {
            break;
          }
          result.emplace_back(from_16bit | static_cast<uint16_t>(to));
        }
        break;
      case R_HORSE:
      case B_HORSE:
        for (const Position to : PossibleMovesHorse(board, pos)) {
          if (to == kNoPosition) {
            break;
          }
          result.emplace_back(from_16bit | static_cast<uint16_t>(to));
        }
        break;
      case R_CHARIOT:
      case B_CHARIOT:
        for (const Position to : PossibleMovesChariot(board, pos)) {
          if (to == kNoPosition) {
            break;
          }
          result.emplace_back(from_16bit | static_cast<uint16_t>(to));
        }
        break;
      case R_CANNON:
      case B_CANNON:
        for (const Position to : PossibleMovesCannon(board, pos)) {
          if (to == kNoPosition) {
            break;
          }
          result.emplace_back(from_16bit | static_cast<uint16_t>(to));
        }
        break;
      case R_SOLDIER:
      case B_SOLDIER:
        for (const Position to : PossibleMovesSoldier(board, pos)) {
          if (to == kNoPosition) {
            break;
          }
          result.emplace_back(from_16bit | static_cast<uint16_t>(to));
        }
        break;
      default:
        continue;
    }
  }
  return result;
}

std::vector<Board<Piece>> AllPossibleNextBoards(const Board<Piece>& board,
                                                Player player) {
  const std::vector<uint16_t> possible_moves =
      AllPossibleNextMoves(board, player);
  std::vector<Board<Piece>> result;
  result.reserve(possible_moves.size());
  for (const uint16_t move : possible_moves) {
    const Position from = static_cast<Position>((move & 0xFF00) >> 8);
    const Position to = static_cast<Position>(move & 0x00FF);
    Board<Piece> next = board;
    Move(next, from, to);
    result.emplace_back(std::move(next));
  }
  return result;
}

Position FindGeneral(const Board<Piece>& board, const Player player) {
  const bool find_red = player == Player::RED;
  const Piece general = find_red ? Piece::R_GENERAL : Piece::B_GENERAL;
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
    } else if (board[3] == general) {
    }
    return kNoPosition;
  }
}

}  // namespace xq
