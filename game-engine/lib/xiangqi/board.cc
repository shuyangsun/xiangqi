#include "xiangqi/board.h"

#include <algorithm>
#include <cstdint>
#include <vector>

#include "xiangqi/internal/moves.h"
#include "xiangqi/types.h"

namespace xq {

namespace {

bool isPathClear(const Board<Piece>& board, Position from, Position to) {
  // Assumes from and to are in the same row or same column.
  if (Row(from) == Row(to)) {
    int start = std::min((int)Col(from), (int)Col(to)) + 1;
    int end = std::max((int)Col(from), (int)Col(to));
    for (int c = start; c < end; ++c) {
      if (board[Row(from)][c] != Piece::EMPTY) return false;
    }
    return true;
  } else if (Col(from) == Col(to)) {
    int start = std::min((int)Row(from), (int)Row(to)) + 1;
    int end = std::max((int)Row(from), (int)Row(to));
    for (int r = start; r < end; ++r) {
      if (board[r][Col(from)] != Piece::EMPTY) return false;
    }
    return true;
  }
  return false;
}

// Soldiers move one step forward. Once they have “crossed the river” they can
// also move sideways. (For the purposes of check‐detection we test only the
// immediate “attack” squares.)
bool ThreatensBySoldier(Piece soldier, Position pos, Position target) {
  int val = static_cast<int>(soldier);
  if (val > 0) {  // red soldier moves upward (row decreases)
    if ((int)Row(pos) - 1 == (int)Row(target) && Col(pos) == Col(target))
      return true;
    // Allow sideways movement after crossing the river:
    if (Row(pos) <= 4 && Row(pos) == Row(target) &&
        std::abs((int)Col(pos) - (int)Col(target)) == 1)
      return true;
  } else {  // black soldier moves downward (row increases)
    if ((int)Row(pos) + 1 == (int)Row(target) && Col(pos) == Col(target))
      return true;
    // Allow sideways movement after crossing the river:
    if (Row(pos) >= 5 && Row(pos) == Row(target) &&
        std::abs((int)Col(pos) - (int)Col(target)) == 1)
      return true;
  }
  return false;
}

// The horse moves in an L-shape but its move is “blocked” if the adjacent
// square in the moving direction is occupied.
bool ThreatensByHorse(const Board<Piece>& board, Position horsePos,
                      Position target) {
  // Moves and corresponding “leg” (blocking square) offsets.
  const int moves[8][2] = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1},
                           {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};
  const int blocks[8][2] = {{1, 0}, {1, 0},  {-1, 0}, {-1, 0},
                            {0, 1}, {0, -1}, {0, 1},  {0, -1}};
  for (int i = 0; i < 8; ++i) {
    int new_r = Row(horsePos) + moves[i][0];
    int new_c = Col(horsePos) + moves[i][1];
    if (new_r == Row(target) && new_c == Col(target)) {
      int block_r = Row(horsePos) + blocks[i][0];
      int block_c = Col(horsePos) + blocks[i][1];
      if (block_r >= 0 && block_r < kTotalRow && block_c >= 0 &&
          block_c < kTotalCol && board[block_r][block_c] == Piece::EMPTY) {
        return true;
      }
    }
  }
  return false;
}

// The elephant moves two points diagonally. Its move is blocked if the midpoint
// is occupied. (For these tests we do not enforce the “river” restriction.)
bool ThreatensByElephant(const Board<Piece>& board, Piece elephant,
                         Position pos, Position target) {
  int dr = Row(target) - Row(pos);
  int dc = Col(target) - Col(pos);
  if (std::abs(dr) != 2 || std::abs(dc) != 2 ||
      board[static_cast<uint8_t>(Row(pos) + dr / 2)]
           [static_cast<uint8_t>(Col(pos) + dc / 2)] != Piece::EMPTY) {
    return false;
  }
  // (If you wish to enforce that elephants may not cross the river, add that
  // check here.)
  return true;
}

// The advisor moves one point diagonally but must remain within its palace.
bool ThreatensByAdvisor(Piece advisor, Position pos, Position target) {
  int dr = Row(target) - Row(pos);
  int dc = Col(target) - Col(pos);
  if (std::abs(dr) != 1 || std::abs(dc) != 1) return false;
  int val = static_cast<int>(advisor);
  if (val > 0) {  // red advisor: palace is rows 7–9 and cols 3–5
    if (Row(target) < 7 || Row(target) > 9 || Col(target) < 3 ||
        Col(target) > 5)
      return false;
  } else {  // black advisor: palace is rows 0–2 and cols 3–5
    if (Row(target) > 2 || Col(target) < 3 || Col(target) > 5) return false;
  }
  return true;
}

// The cannon moves like a chariot but can only capture if exactly one piece
// (the “screen”) lies between it and its target.
bool ThreatensByCannon(const Board<Piece>& board, Position cannonPos,
                       Position target) {
  if (Row(cannonPos) == Row(target)) {
    int start = std::min((int)Col(cannonPos), (int)Col(target)) + 1;
    int end = std::max((int)Col(cannonPos), (int)Col(target));
    int count = 0;
    for (int c = start; c < end; ++c) {
      if (board[Row(cannonPos)][c] != Piece::EMPTY) count++;
    }
    return (count == 1);
  } else if (Col(cannonPos) == Col(target)) {
    int start = std::min((int)Row(cannonPos), (int)Row(target)) + 1;
    int end = std::max((int)Row(cannonPos), (int)Row(target));
    int count = 0;
    for (int r = start; r < end; ++r) {
      if (board[r][Col(cannonPos)] != Piece::EMPTY) count++;
    }
    return (count == 1);
  }
  return false;
}

}  // namespace

bool IsCheckMade(const Board<Piece>& board, Player player) {
  using enum Piece;
  Position general_pos = kNoPosition;
  bool found = false;
  // Determine our own general based on whose turn it is.
  Piece my_general =
      (player == Player::RED) ? Piece::R_GENERAL : Piece::B_GENERAL;
  for (uint8_t r = 0; r < kTotalRow; ++r) {
    for (uint8_t c = 0; c < kTotalCol; ++c) {
      if (board[r][c] == my_general) {
        general_pos = Pos(r, c);
        found = true;
        break;
      }
    }
    if (found) break;
  }
  // (If the general is missing, you might want to signal game over. Here we
  // simply return true.)
  if (!found) return true;

  // Now scan the board for enemy pieces that might be threatening our general.
  for (uint8_t r = 0; r < kTotalRow; ++r) {
    for (uint8_t c = 0; c < kTotalCol; ++c) {
      Piece piece = board[r][c];
      if (piece == Piece::EMPTY) continue;
      int piece_val = static_cast<int>(piece);
      const bool piece_is_red = piece_val > 0;
      const bool player_is_red = player == Player::RED;
      if (piece_is_red == player_is_red) continue;  // skip our own pieces

      Position piece_pos = Pos(r, c);
      switch (std::abs(piece_val)) {
        case static_cast<uint8_t>(R_GENERAL):
          // Enemy general: by the flying-general rule, if the two generals are
          // on the same file with no piece between.
          if (Col(piece_pos) == Col(general_pos) &&
              isPathClear(board, piece_pos, general_pos))
            return true;
          break;
        case static_cast<uint8_t>(R_CHARIOT):
          if ((Row(piece_pos) == Row(general_pos) ||
               Col(piece_pos) == Col(general_pos)) &&
              isPathClear(board, piece_pos, general_pos))
            return true;
          break;
        case static_cast<uint8_t>(R_SOLDIER):
          if (ThreatensBySoldier(piece, piece_pos, general_pos)) return true;
          break;
        case static_cast<uint8_t>(R_HORSE):
          if (ThreatensByHorse(board, piece_pos, general_pos)) return true;
        case static_cast<uint8_t>(R_ELEPHANT):
          if (ThreatensByElephant(board, piece, piece_pos, general_pos))
            return true;
          break;
        case static_cast<uint8_t>(R_ADVISOR):
          if (ThreatensByAdvisor(piece, piece_pos, general_pos)) return true;
          break;
        case static_cast<uint8_t>(R_CANNON):
          if (ThreatensByCannon(board, piece_pos, general_pos)) return true;
          break;
        default:
          break;
      }
    }
  }
  return false;
}

bool IsGameOver(const Board<Piece>& board) {
  // The game is over if one or both generals have been captured.
  bool redFound = false;
  bool blackFound = false;
  for (uint8_t r = 0; r < kTotalRow; ++r) {
    for (uint8_t c = 0; c < kTotalCol; ++c) {
      if (board[r][c] == Piece::R_GENERAL) {
        redFound = true;
      } else if (board[r][c] == Piece::B_GENERAL) {
        blackFound = true;
      }
    }
  }
  return (!redFound || !blackFound);
}

Winner GetWinner(const Board<Piece>& board) {
  if (!IsGameOver(board)) {
    return Winner::NONE;
  }

  bool redFound = false;
  bool blackFound = false;
  for (uint8_t r = 0; r < kTotalRow; ++r) {
    for (uint8_t c = 0; c < kTotalCol; ++c) {
      if (board[r][c] == Piece::R_GENERAL) {
        redFound = true;
      } else if (board[r][c] == Piece::B_GENERAL) {
        blackFound = true;
      }
    }
  }

  if (redFound && !blackFound) {
    return Winner::RED;
  } else if (!redFound && blackFound) {
    return Winner::BLACK;
  } else if (!redFound && !blackFound) {
    return Winner::DRAW;
  }
  return Winner::DRAW;
}

Position FlipPosition(Position position) {
  return Pos(kTotalRow - 1 - Row(position), kTotalCol - 1 - Col(position));
}

Board<Piece> FlipBoard(const Board<Piece>& board) {
  Board<Piece> flipped;
  // Iterate through every cell in the board.
  for (uint8_t r = 0; r < kTotalRow; ++r) {
    for (uint8_t c = 0; c < kTotalCol; ++c) {
      // Get the original piece.
      Piece orig = board[r][c];
      int8_t orig_val = static_cast<int8_t>(orig);
      // If not empty, flip its sign. EMPTY remains unchanged.
      int8_t new_val = (orig_val == 0) ? 0 : -orig_val;
      // Place the new piece into the rotated position.
      flipped[kTotalRow - 1 - r][kTotalCol - 1 - c] =
          static_cast<Piece>(new_val);
    }
  }
  return flipped;
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

  for (uint8_t row = 0; row < kTotalRow; row++) {
    for (uint8_t col = 0; col < kTotalCol; col++) {
      const Position pos = Pos(row, col);
      switch (board[row][col]) {
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
  for (auto& row : board) {
    row.fill(Piece::EMPTY);
  }

  // For red pieces, the encoded state is split between state[0] (res1) and
  // state[1] (res2).
  uint64_t res1 = state[0];
  uint64_t res2 = state[1];

  // --- Red Pieces ---

  // Red General: bits 56-63 of res1.
  uint8_t r_general = (res1 >> 56) & 0xFF;
  if (r_general != 0xFF) {
    board[Row(r_general)][Col(r_general)] = Piece::R_GENERAL;
  }

  // Red Advisors: bits 48-55 and 40-47 of res1.
  uint8_t r_adv1 = (res1 >> 48) & 0xFF;
  if (r_adv1 != 0xFF) {
    board[Row(r_adv1)][Col(r_adv1)] = Piece::R_ADVISOR;
  }
  uint8_t r_adv2 = (res1 >> 40) & 0xFF;
  if (r_adv2 != 0xFF) {
    board[Row(r_adv2)][Col(r_adv2)] = Piece::R_ADVISOR;
  }

  // Red Elephants: bits 32-39 and 24-31 of res1.
  uint8_t r_ele1 = (res1 >> 32) & 0xFF;
  if (r_ele1 != 0xFF) {
    board[Row(r_ele1)][Col(r_ele1)] = Piece::R_ELEPHANT;
  }
  uint8_t r_ele2 = (res1 >> 24) & 0xFF;
  if (r_ele2 != 0xFF) {
    board[Row(r_ele2)][Col(r_ele2)] = Piece::R_ELEPHANT;
  }

  // Red Horses: bits 16-23 and 8-15 of res1.
  uint8_t r_horse1 = (res1 >> 16) & 0xFF;
  if (r_horse1 != 0xFF) {
    board[Row(r_horse1)][Col(r_horse1)] = Piece::R_HORSE;
  }
  uint8_t r_horse2 = (res1 >> 8) & 0xFF;
  if (r_horse2 != 0xFF) {
    board[Row(r_horse2)][Col(r_horse2)] = Piece::R_HORSE;
  }

  // Red Chariots:
  //   - One chariot: bits 0-7 of res1.
  uint8_t r_chariot1 = res1 & 0xFF;
  if (r_chariot1 != 0xFF) {
    board[Row(r_chariot1)][Col(r_chariot1)] = Piece::R_CHARIOT;
  }
  //   - The other chariot: bits 56-63 of res2.
  uint8_t r_chariot2 = (res2 >> 56) & 0xFF;
  if (r_chariot2 != 0xFF) {
    board[Row(r_chariot2)][Col(r_chariot2)] = Piece::R_CHARIOT;
  }

  // Red Cannons: bits 48-55 and 40-47 of res2.
  uint8_t r_cannon1 = (res2 >> 48) & 0xFF;
  if (r_cannon1 != 0xFF) {
    board[Row(r_cannon1)][Col(r_cannon1)] = Piece::R_CANNON;
  }
  uint8_t r_cannon2 = (res2 >> 40) & 0xFF;
  if (r_cannon2 != 0xFF) {
    board[Row(r_cannon2)][Col(r_cannon2)] = Piece::R_CANNON;
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
      board[Row(pos)][Col(pos)] = Piece::R_SOLDIER;
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
    board[Row(b_general)][Col(b_general)] = Piece::B_GENERAL;
  }

  // Black Advisors: bits 48-55 and 40-47 of res3.
  uint8_t b_adv1 = (res3 >> 48) & 0xFF;
  if (b_adv1 != 0xFF) {
    board[Row(b_adv1)][Col(b_adv1)] = Piece::B_ADVISOR;
  }
  uint8_t b_adv2 = (res3 >> 40) & 0xFF;
  if (b_adv2 != 0xFF) {
    board[Row(b_adv2)][Col(b_adv2)] = Piece::B_ADVISOR;
  }

  // Black Elephants: bits 32-39 and 24-31 of res3.
  uint8_t b_ele1 = (res3 >> 32) & 0xFF;
  if (b_ele1 != 0xFF) {
    board[Row(b_ele1)][Col(b_ele1)] = Piece::B_ELEPHANT;
  }
  uint8_t b_ele2 = (res3 >> 24) & 0xFF;
  if (b_ele2 != 0xFF) {
    board[Row(b_ele2)][Col(b_ele2)] = Piece::B_ELEPHANT;
  }

  // Black Horses: bits 16-23 and 8-15 of res3.
  uint8_t b_horse1 = (res3 >> 16) & 0xFF;
  if (b_horse1 != 0xFF) {
    board[Row(b_horse1)][Col(b_horse1)] = Piece::B_HORSE;
  }
  uint8_t b_horse2 = (res3 >> 8) & 0xFF;
  if (b_horse2 != 0xFF) {
    board[Row(b_horse2)][Col(b_horse2)] = Piece::B_HORSE;
  }

  // Black Chariots:
  //   - One from res3: bits 0-7.
  uint8_t b_chariot1 = res3 & 0xFF;
  if (b_chariot1 != 0xFF) {
    board[Row(b_chariot1)][Col(b_chariot1)] = Piece::B_CHARIOT;
  }
  //   - The other from res4: bits 56-63.
  uint8_t b_chariot2 = (res4 >> 56) & 0xFF;
  if (b_chariot2 != 0xFF) {
    board[Row(b_chariot2)][Col(b_chariot2)] = Piece::B_CHARIOT;
  }

  // Black Cannons: bits 48-55 and 40-47 of res4.
  uint8_t b_cannon1 = (res4 >> 48) & 0xFF;
  if (b_cannon1 != 0xFF) {
    board[Row(b_cannon1)][Col(b_cannon1)] = Piece::B_CANNON;
  }
  uint8_t b_cannon2 = (res4 >> 40) & 0xFF;
  if (b_cannon2 != 0xFF) {
    board[Row(b_cannon2)][Col(b_cannon2)] = Piece::B_CANNON;
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
      board[Row(pos)][Col(pos)] = Piece::B_SOLDIER;
    }
  }

  return board;
}

Board<bool> PossibleMoves(const Board<Piece>& board, Position pos) {
  using namespace xq::internal::util;
  using enum Piece;
  const Piece piece = board[Row(pos)][Col(pos)];
  switch (piece) {
    case EMPTY:
      return PossibleMovesEmpty(board, pos);
    case R_GENERAL:
    case B_GENERAL:
      return PossibleMovesGeneral(board, pos);
    case R_ADVISOR:
    case B_ADVISOR:
      return PossibleMovesAdvisor(board, pos);
    case R_ELEPHANT:
    case B_ELEPHANT:
      return PossibleMovesElephant(board, pos);
    case R_HORSE:
    case B_HORSE:
      return PossibleMovesHorse(board, pos);
    case R_CHARIOT:
    case B_CHARIOT:
      return PossibleMovesChariot(board, pos);
    case R_CANNON:
    case B_CANNON:
      return PossibleMovesCannon(board, pos);
    case R_SOLDIER:
    case B_SOLDIER:
      return PossibleMovesSoldier(board, pos);
    default:
      return {false};
  }
}

Piece Move(Board<Piece>& board, Position from, Position to) {
  using enum Player;
  if (from == to) {
    return Piece::EMPTY;
  }
  const Piece piece = board[Row(from)][Col(from)];
  if (piece == Piece::EMPTY) {
    return Piece::EMPTY;
  }
  const Piece captured = board[Row(to)][Col(to)];
  board[Row(to)][Col(to)] = piece;
  board[Row(from)][Col(from)] = Piece::EMPTY;
  return captured;
}

// Returns a vector of all possible moves for player.
std::vector<uint16_t> AllPossibleNextMoves(const Board<Piece>& board,
                                           Player player) {
  std::vector<uint16_t> result;
  for (uint8_t row = 0; row < kTotalRow; row++) {
    for (uint8_t col = 0; col < kTotalCol; col++) {
      const Piece piece = board[row][col];
      if (piece == Piece::EMPTY || IsRed(piece) != (player == Player::RED)) {
        continue;
      }
      const Board<bool> possible_moves = PossibleMoves(board, Pos(row, col));
      for (uint8_t mrow = 0; mrow < kTotalRow; mrow++) {
        for (uint8_t mcol = 0; mcol < kTotalCol; mcol++) {
          if (!possible_moves[mrow][mcol]) {
            continue;
          }
          result.emplace_back(static_cast<uint16_t>(Pos(row, col) << 8) |
                              static_cast<uint16_t>(Pos(mrow, mcol)));
        }
      }
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

}  // namespace xq
