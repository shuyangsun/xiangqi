#include "xiangqi/game.h"

#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <utility>
#include <vector>

#include "xiangqi/internal/moves.h"
#include "xiangqi/types.h"

namespace xq {

namespace {

using enum Piece;

constexpr Board<Piece> kInitState = {
    B_CHARIOT_2, B_HORSE_2,    B_ELEPHANT_2, B_ADVISOR_2, B_GENERAL,
    B_ADVISOR_1, B_ELEPHANT_1, B_HORSE_1,    B_CHARIOT_1,  // Row 0
    EMPTY,       EMPTY,        EMPTY,        EMPTY,       EMPTY,
    EMPTY,       EMPTY,        EMPTY,        EMPTY,  // Row 1
    EMPTY,       B_CANNON_2,   EMPTY,        EMPTY,       EMPTY,
    EMPTY,       EMPTY,        B_CANNON_1,   EMPTY,  // Row 2
    B_SOLDIER_5, EMPTY,        B_SOLDIER_4,  EMPTY,       B_SOLDIER_3,
    EMPTY,       B_SOLDIER_2,  EMPTY,        B_SOLDIER_1,  // Row 3
    EMPTY,       EMPTY,        EMPTY,        EMPTY,       EMPTY,
    EMPTY,       EMPTY,        EMPTY,        EMPTY,  // Row 4
    EMPTY,       EMPTY,        EMPTY,        EMPTY,       EMPTY,
    EMPTY,       EMPTY,        EMPTY,        EMPTY,  // Row 5
    R_SOLDIER_1, EMPTY,        R_SOLDIER_2,  EMPTY,       R_SOLDIER_3,
    EMPTY,       R_SOLDIER_4,  EMPTY,        R_SOLDIER_5,  // Row 6
    EMPTY,       R_CANNON_1,   EMPTY,        EMPTY,       EMPTY,
    EMPTY,       EMPTY,        R_CANNON_2,   EMPTY,  // Row 7
    EMPTY,       EMPTY,        EMPTY,        EMPTY,       EMPTY,
    EMPTY,       EMPTY,        EMPTY,        EMPTY,  // Row 8
    R_CHARIOT_1, R_HORSE_1,    R_ELEPHANT_1, R_ADVISOR_1, R_GENERAL,
    R_ADVISOR_2, R_ELEPHANT_2, R_HORSE_2,    R_CHARIOT_2,  // Row 9
};

// Converts a piece map to a board. The resulting board will be completely empty
// (all cells set to EMPTY) except for the positions specified in the map.
Board<Piece> pieceMapToBoard(std::unordered_map<Piece, Position>&& piece_pos) {
  Board<Piece> board;
  for (auto& row : board) {
    row.fill(EMPTY);
  }
  for (const auto& entry : piece_pos) {
    Piece piece = entry.first;
    const Position& pos = entry.second;
    if (pos.row < kTotalRow && pos.col < kTotalCol) {
      board[pos.row][pos.col] = piece;
    }
  }
  return board;
}

bool isPathClear(const Board<Piece>& board, const Position& from,
                 const Position& to) {
  // Assumes from and to are in the same row or same column.
  if (from.row == to.row) {
    int start = std::min((int)from.col, (int)to.col) + 1;
    int end = std::max((int)from.col, (int)to.col);
    for (int c = start; c < end; ++c) {
      if (board[from.row][c] != Piece::EMPTY) return false;
    }
    return true;
  } else if (from.col == to.col) {
    int start = std::min((int)from.row, (int)to.row) + 1;
    int end = std::max((int)from.row, (int)to.row);
    for (int r = start; r < end; ++r) {
      if (board[r][from.col] != Piece::EMPTY) return false;
    }
    return true;
  }
  return false;
}

// Soldiers move one step forward. Once they have “crossed the river” they can
// also move sideways. (For the purposes of check‐detection we test only the
// immediate “attack” squares.)
bool threatensBySoldier(Piece soldier, const Position& pos,
                        const Position& target) {
  int val = static_cast<int>(soldier);
  if (val > 0) {  // red soldier moves upward (row decreases)
    if ((int)pos.row - 1 == (int)target.row && pos.col == target.col)
      return true;
    // Allow sideways movement after crossing the river:
    if (pos.row <= 4 && pos.row == target.row &&
        std::abs((int)pos.col - (int)target.col) == 1)
      return true;
  } else {  // black soldier moves downward (row increases)
    if ((int)pos.row + 1 == (int)target.row && pos.col == target.col)
      return true;
    // Allow sideways movement after crossing the river:
    if (pos.row >= 5 && pos.row == target.row &&
        std::abs((int)pos.col - (int)target.col) == 1)
      return true;
  }
  return false;
}

// The horse moves in an L-shape but its move is “blocked” if the adjacent
// square in the moving direction is occupied.
bool threatensByHorse(const Board<Piece>& board, const Position& horsePos,
                      const Position& target) {
  // Moves and corresponding “leg” (blocking square) offsets.
  const int moves[8][2] = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1},
                           {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};
  const int blocks[8][2] = {{1, 0}, {1, 0},  {-1, 0}, {-1, 0},
                            {0, 1}, {0, -1}, {0, 1},  {0, -1}};
  for (int i = 0; i < 8; ++i) {
    int new_r = horsePos.row + moves[i][0];
    int new_c = horsePos.col + moves[i][1];
    if (new_r == target.row && new_c == target.col) {
      int block_r = horsePos.row + blocks[i][0];
      int block_c = horsePos.col + blocks[i][1];
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
bool threatensByElephant(const Board<Piece>& board, Piece elephant,
                         const Position& pos, const Position& target) {
  int dr = target.row - pos.row;
  int dc = target.col - pos.col;
  if (std::abs(dr) != 2 || std::abs(dc) != 2) return false;
  Position mid{static_cast<uint8_t>(pos.row + dr / 2),
               static_cast<uint8_t>(pos.col + dc / 2)};
  if (board[mid.row][mid.col] != Piece::EMPTY) return false;
  // (If you wish to enforce that elephants may not cross the river, add that
  // check here.)
  return true;
}

// The advisor moves one point diagonally but must remain within its palace.
bool threatensByAdvisor(Piece advisor, const Position& pos,
                        const Position& target) {
  int dr = target.row - pos.row;
  int dc = target.col - pos.col;
  if (std::abs(dr) != 1 || std::abs(dc) != 1) return false;
  int val = static_cast<int>(advisor);
  if (val > 0) {  // red advisor: palace is rows 7–9 and cols 3–5
    if (target.row < 7 || target.row > 9 || target.col < 3 || target.col > 5)
      return false;
  } else {  // black advisor: palace is rows 0–2 and cols 3–5
    if (target.row > 2 || target.col < 3 || target.col > 5) return false;
  }
  return true;
}

// The cannon moves like a chariot but can only capture if exactly one piece
// (the “screen”) lies between it and its target.
bool threatensByCannon(const Board<Piece>& board, const Position& cannonPos,
                       const Position& target) {
  if (cannonPos.row == target.row) {
    int start = std::min((int)cannonPos.col, (int)target.col) + 1;
    int end = std::max((int)cannonPos.col, (int)target.col);
    int count = 0;
    for (int c = start; c < end; ++c) {
      if (board[cannonPos.row][c] != Piece::EMPTY) count++;
    }
    return (count == 1);
  } else if (cannonPos.col == target.col) {
    int start = std::min((int)cannonPos.row, (int)target.row) + 1;
    int end = std::max((int)cannonPos.row, (int)target.row);
    int count = 0;
    for (int r = start; r < end; ++r) {
      if (board[r][cannonPos.col] != Piece::EMPTY) count++;
    }
    return (count == 1);
  }
  return false;
}

}  // namespace

Game::Game() : history_{kInitState} {}

void Game::ResetFromBoard(Board<Piece>&& board) {
  using enum Player;
  moves_.clear();
  history_.clear();
  history_.emplace_back(std::move(board));
  player_ = RED;
}

void Game::Reset() {
  Board<Piece> board = kInitState;
  ResetFromBoard(std::move(board));
}

void Game::ResetFromPos(std::unordered_map<Piece, Position>&& piece_pos) {
  ResetFromBoard(pieceMapToBoard(std::move(piece_pos)));
}

Player Game::Turn() const { return player_; }

size_t Game::MovesCount() const { return moves_.size(); }

void Game::MakeBlackMoveFirst() {
  if (history_.size() > 1) {
    return;
  }
  player_ = Player::BLACK;
}

Board<Piece> Game::CurrentBoard() const { return {history_.back()}; }

Piece Game::PieceAt(Position pos) const {
  return history_.back()[pos.row][pos.col];
}

Piece Game::Move(Position from, Position to) {
  using enum Player;
  if (from.row == to.row && from.col == to.col) {
    return Piece::EMPTY;
  }
  const Piece piece = PieceAt(from);
  const auto piece_value = static_cast<std::underlying_type_t<Piece>>(piece);
  if (piece == Piece::EMPTY || (player_ == RED && piece_value < 0) ||
      (player_ == BLACK && piece_value > 0)) {
    return Piece::EMPTY;
  }

  Board<Piece> next = history_.back();
  Piece captured = next[to.row][to.col];
  next[to.row][to.col] = PieceAt(from);
  next[from.row][from.col] = Piece::EMPTY;
  moves_.emplace_back(piece, from, to, captured);
  history_.emplace_back(next);
  player_ = player_ == RED ? BLACK : RED;
  return captured;
}

bool Game::CanUndo() const { return history_.size() > 1; }

MoveAction Game::Undo() {
  using enum Player;

  if (!CanUndo()) {
    // Dummy move action.
    return {Piece::EMPTY, {1, 0}, {1, 0}, Piece::EMPTY};
  }
  MoveAction result = moves_.back();
  moves_.pop_back();
  history_.pop_back();
  player_ = player_ == RED ? BLACK : RED;
  return result;
}

Board<bool> Game::PossibleMoves(Position pos) const {
  using namespace xq::internal::util;
  using enum Piece;
  const Piece piece = PieceAt(pos);
  const Board<Piece>& board = history_.back();
  switch (piece) {
    case EMPTY:
      return PossibleMovesEmpty(board, pos);
    case R_GENERAL:
    case B_GENERAL:
      return PossibleMovesGeneral(board, pos);
    case R_ADVISOR_1:
    case R_ADVISOR_2:
    case B_ADVISOR_1:
    case B_ADVISOR_2:
      return PossibleMovesAdvisor(board, pos);
    case R_ELEPHANT_1:
    case R_ELEPHANT_2:
    case B_ELEPHANT_1:
    case B_ELEPHANT_2:
      return PossibleMovesElephant(board, pos);
    case R_HORSE_1:
    case R_HORSE_2:
    case B_HORSE_1:
    case B_HORSE_2:
      return PossibleMovesHorse(board, pos);
    case R_CHARIOT_1:
    case R_CHARIOT_2:
    case B_CHARIOT_1:
    case B_CHARIOT_2:
      return PossibleMovesChariot(board, pos);
    case R_CANNON_1:
    case R_CANNON_2:
    case B_CANNON_1:
    case B_CANNON_2:
      return PossibleMovesCannon(board, pos);
    case R_SOLDIER_1:
    case R_SOLDIER_2:
    case R_SOLDIER_3:
    case R_SOLDIER_4:
    case R_SOLDIER_5:
    case B_SOLDIER_1:
    case B_SOLDIER_2:
    case B_SOLDIER_3:
    case B_SOLDIER_4:
    case B_SOLDIER_5:
      return PossibleMovesSoldier(board, pos);
    default:
      return {false};
  }
}

bool Game::IsCheckMade() const {
  Board<Piece> board = CurrentBoard();
  Position generalPos{0, 0};
  bool found = false;
  // Determine our own general based on whose turn it is.
  Piece myGeneral =
      (player_ == Player::RED) ? Piece::R_GENERAL : Piece::B_GENERAL;
  for (uint8_t r = 0; r < kTotalRow; ++r) {
    for (uint8_t c = 0; c < kTotalCol; ++c) {
      if (board[r][c] == myGeneral) {
        generalPos = {r, c};
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
      int pieceVal = static_cast<int>(piece);
      bool pieceIsRed = (pieceVal > 0);
      bool myIsRed = (player_ == Player::RED);
      if (pieceIsRed == myIsRed) continue;  // skip our own pieces

      Position piecePos{r, c};
      int absVal = std::abs(pieceVal);
      switch (absVal) {
        case 1:
          // Enemy general: by the flying-general rule, if the two generals are
          // on the same file with no piece between.
          if (piecePos.col == generalPos.col &&
              isPathClear(board, piecePos, generalPos))
            return true;
          break;
        case 8:  // chariot (first variant)
        case 9:  // chariot (second variant)
          if ((piecePos.row == generalPos.row ||
               piecePos.col == generalPos.col) &&
              isPathClear(board, piecePos, generalPos))
            return true;
          break;
        case 12:  // soldier (all soldier values: 12–16 for red, -12 to -16 for
                  // black)
        case 13:
        case 14:
        case 15:
        case 16:
          if (threatensBySoldier(piece, piecePos, generalPos)) return true;
          break;
        case 6:  // horse
        case 7:
          if (threatensByHorse(board, piecePos, generalPos)) return true;
          break;
        case 4:  // elephant
        case 5:
          if (threatensByElephant(board, piece, piecePos, generalPos))
            return true;
          break;
        case 2:  // advisor
        case 3:
          if (threatensByAdvisor(piece, piecePos, generalPos)) return true;
          break;
        case 10:  // cannon
        case 11:
          if (threatensByCannon(board, piecePos, generalPos)) return true;
          break;
        default:
          break;
      }
    }
  }
  return false;
}

bool Game::IsGameOver() const {
  // The game is over if one or both generals have been captured.
  const Board<Piece>& board = history_.back();
  bool redFound = false;
  bool blackFound = false;
  for (uint8_t r = 0; r < kTotalRow; ++r) {
    for (uint8_t c = 0; c < kTotalCol; ++c) {
      if (board[r][c] == R_GENERAL) {
        redFound = true;
      } else if (board[r][c] == B_GENERAL) {
        blackFound = true;
      }
    }
  }
  return (!redFound || !blackFound);
}

Winner Game::GetWinner() const {
  if (!IsGameOver()) {
    return Winner::NONE;
  }

  const Board<Piece>& board = history_.back();
  bool redFound = false;
  bool blackFound = false;
  for (uint8_t r = 0; r < kTotalRow; ++r) {
    for (uint8_t c = 0; c < kTotalCol; ++c) {
      if (board[r][c] == R_GENERAL) {
        redFound = true;
      } else if (board[r][c] == B_GENERAL) {
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

std::vector<uint16_t> Game::ExportMoves() const {
  std::vector<uint16_t> result;
  result.reserve(moves_.size());
  for (const MoveAction& move : moves_) {
    result.emplace_back((static_cast<uint16_t>(move.from.row) << 12) |
                        (static_cast<uint16_t>(move.from.col) << 8) |
                        (static_cast<uint16_t>(move.to.row) << 4) |
                        move.to.col);
  }
  return result;
}

void Game::RestoreMoves(const std::vector<uint16_t>& moves) {
  for (const uint16_t move : moves) {
    Move(
        Position{
            .row = static_cast<uint8_t>((move & 0xF000) >> 12),
            .col = static_cast<uint8_t>((move & 0x0F00) >> 8),
        },
        Position{
            .row = static_cast<uint8_t>((move & 0x00F0) >> 4),
            .col = static_cast<uint8_t>((move & 0x000F)),
        });
  }
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
  uint64_t res1 = 0, res2 = 0, res3 = 0, res4 = 0;

  // Initialize all pieces as missing.
  uint8_t r_general_pos = 0xFF, r_advisor_1_pos = 0xFF, r_advisor_2_pos = 0xFF,
          r_elephant_1_pos = 0xFF, r_elephant_2_pos = 0xFF,
          r_horse_1_pos = 0xFF, r_horse_2_pos = 0xFF, r_chariot_1_pos = 0xFF,
          r_chariot_2_pos = 0xFF, r_cannon_1_pos = 0xFF, r_cannon_2_pos = 0xFF,
          r_soldier_1_pos = 0xFF, r_soldier_2_pos = 0xFF,
          r_soldier_3_pos = 0xFF, r_soldier_4_pos = 0xFF,
          r_soldier_5_pos = 0xFF, b_advisor_1_pos = 0xFF, b_general_pos = 0xFF,
          b_advisor_2_pos = 0xFF, b_elephant_1_pos = 0xFF,
          b_elephant_2_pos = 0xFF, b_horse_1_pos = 0xFF, b_horse_2_pos = 0xFF,
          b_chariot_1_pos = 0xFF, b_chariot_2_pos = 0xFF, b_cannon_1_pos = 0xFF,
          b_cannon_2_pos = 0xFF, b_soldier_1_pos = 0xFF, b_soldier_2_pos = 0xFF,
          b_soldier_3_pos = 0xFF, b_soldier_4_pos = 0xFF,
          b_soldier_5_pos = 0xFF;

  // Loop over the board and record positions.
  for (uint8_t r = 0; r < kTotalRow; ++r) {
    for (uint8_t c = 0; c < kTotalCol; ++c) {
      const Piece p = board[r][c];
      if (p == Piece::EMPTY) {
        continue;
      }
      const uint8_t pos = (r << 4) | c;
      switch (p) {
        case R_GENERAL:
          r_general_pos = pos;
          break;
        case R_ADVISOR_1:
          r_advisor_1_pos = pos;
          break;
        case R_ADVISOR_2:
          r_advisor_2_pos = pos;
          break;
        case R_ELEPHANT_1:
          r_elephant_1_pos = pos;
          break;
        case R_ELEPHANT_2:
          r_elephant_2_pos = pos;
          break;
        case R_HORSE_1:
          r_horse_1_pos = pos;
          break;
        case R_HORSE_2:
          r_horse_2_pos = pos;
          break;
        case R_CHARIOT_1:
          r_chariot_1_pos = pos;
          break;
        case R_CHARIOT_2:
          r_chariot_2_pos = pos;
          break;
        case R_CANNON_1:
          r_cannon_1_pos = pos;
          break;
        case R_CANNON_2:
          r_cannon_2_pos = pos;
          break;
        case R_SOLDIER_1:
          r_soldier_1_pos = pos;
          break;
        case R_SOLDIER_2:
          r_soldier_2_pos = pos;
          break;
        case R_SOLDIER_3:
          r_soldier_3_pos = pos;
          break;
        case R_SOLDIER_4:
          r_soldier_4_pos = pos;
          break;
        case R_SOLDIER_5:
          r_soldier_5_pos = pos;
          break;
        case B_GENERAL:
          b_general_pos = pos;
          break;
        case B_ADVISOR_1:
          b_advisor_1_pos = pos;
          break;
        case B_ADVISOR_2:
          b_advisor_2_pos = pos;
          break;
        case B_ELEPHANT_1:
          b_elephant_1_pos = pos;
          break;
        case B_ELEPHANT_2:
          b_elephant_2_pos = pos;
          break;
        case B_HORSE_1:
          b_horse_1_pos = pos;
          break;
        case B_HORSE_2:
          b_horse_2_pos = pos;
          break;
        case B_CHARIOT_1:
          b_chariot_1_pos = pos;
          break;
        case B_CHARIOT_2:
          b_chariot_2_pos = pos;
          break;
        case B_CANNON_1:
          b_cannon_1_pos = pos;
          break;
        case B_CANNON_2:
          b_cannon_2_pos = pos;
          break;
        case B_SOLDIER_1:
          b_soldier_1_pos = pos;
          break;
        case B_SOLDIER_2:
          b_soldier_2_pos = pos;
          break;
        case B_SOLDIER_3:
          b_soldier_3_pos = pos;
          break;
        case B_SOLDIER_4:
          b_soldier_4_pos = pos;
          break;
        case B_SOLDIER_5:
          b_soldier_5_pos = pos;
          break;
      }
    }
  }

  // generals
  res1 |= (static_cast<uint64_t>(r_general_pos) << 56);
  res3 |= (static_cast<uint64_t>(b_general_pos) << 56);

  // Advisors
  if (r_advisor_1_pos < r_advisor_2_pos) {
    res1 |= ((static_cast<uint64_t>(r_advisor_1_pos) << 48) |
             (static_cast<uint64_t>(r_advisor_2_pos) << 40));
  } else {
    res1 |= ((static_cast<uint64_t>(r_advisor_2_pos) << 48) |
             (static_cast<uint64_t>(r_advisor_1_pos) << 40));
  }
  if (b_advisor_1_pos < b_advisor_2_pos) {
    res3 |= ((static_cast<uint64_t>(b_advisor_1_pos) << 48) |
             (static_cast<uint64_t>(b_advisor_2_pos) << 40));
  } else {
    res3 |= ((static_cast<uint64_t>(b_advisor_2_pos) << 48) |
             (static_cast<uint64_t>(b_advisor_1_pos) << 40));
  }
  // Elephants
  if (r_elephant_1_pos < r_elephant_2_pos) {
    res1 |= ((static_cast<uint64_t>(r_elephant_1_pos) << 32) |
             (static_cast<uint64_t>(r_elephant_2_pos) << 24));
  } else {
    res1 |= ((static_cast<uint64_t>(r_elephant_2_pos) << 32) |
             (static_cast<uint64_t>(r_elephant_1_pos) << 24));
  }
  if (b_elephant_1_pos < b_elephant_2_pos) {
    res3 |= ((static_cast<uint64_t>(b_elephant_1_pos) << 32) |
             (static_cast<uint64_t>(b_elephant_2_pos) << 24));
  } else {
    res3 |= ((static_cast<uint64_t>(b_elephant_2_pos) << 32) |
             (static_cast<uint64_t>(b_elephant_1_pos) << 24));
  }
  // Horses
  if (r_horse_1_pos < r_horse_2_pos) {
    res1 |= ((static_cast<uint64_t>(r_horse_1_pos) << 16) |
             (static_cast<uint64_t>(r_horse_2_pos) << 8));
  } else {
    res1 |= ((static_cast<uint64_t>(r_horse_2_pos) << 16) |
             (static_cast<uint64_t>(r_horse_1_pos) << 8));
  }
  if (b_horse_1_pos < b_horse_2_pos) {
    res3 |= ((static_cast<uint64_t>(b_horse_1_pos) << 16) |
             (static_cast<uint64_t>(b_horse_2_pos) << 8));
  } else {
    res3 |= ((static_cast<uint64_t>(b_horse_2_pos) << 16) |
             (static_cast<uint64_t>(b_horse_1_pos) << 8));
  }
  // Chariots
  if (r_chariot_1_pos < r_chariot_2_pos) {
    res1 |= static_cast<uint64_t>(r_chariot_1_pos);
    res2 |= (static_cast<uint64_t>(r_chariot_2_pos) << 56);
  } else {
    res1 |= static_cast<uint64_t>(r_chariot_2_pos);
    res2 |= (static_cast<uint64_t>(r_chariot_1_pos) << 56);
  }
  if (b_chariot_1_pos < b_chariot_2_pos) {
    res3 |= static_cast<uint64_t>(b_chariot_1_pos);
    res4 |= (static_cast<uint64_t>(b_chariot_2_pos) << 56);
  } else {
    res3 |= static_cast<uint64_t>(b_chariot_2_pos);
    res4 |= (static_cast<uint64_t>(b_chariot_1_pos) << 56);
  }
  // Cannons
  if (r_cannon_1_pos < r_cannon_2_pos) {
    res2 |= ((static_cast<uint64_t>(r_cannon_1_pos) << 48) |
             (static_cast<uint64_t>(r_cannon_2_pos) << 40));
  } else {
    res2 |= ((static_cast<uint64_t>(r_cannon_2_pos) << 48) |
             (static_cast<uint64_t>(r_cannon_1_pos) << 40));
  }
  if (b_cannon_1_pos < b_cannon_2_pos) {
    res4 |= ((static_cast<uint64_t>(b_cannon_1_pos) << 48) |
             (static_cast<uint64_t>(b_cannon_2_pos) << 40));
  } else {
    res4 |= ((static_cast<uint64_t>(b_cannon_2_pos) << 48) |
             (static_cast<uint64_t>(b_cannon_1_pos) << 40));
  }
  // Soldiers
  std::array<uint8_t, 5> red_soldier_pos{r_soldier_1_pos, r_soldier_2_pos,
                                         r_soldier_3_pos, r_soldier_4_pos,
                                         r_soldier_5_pos};
  std::sort(red_soldier_pos.begin(), red_soldier_pos.end());
  std::array<uint8_t, 5> black_soldier_pos{b_soldier_1_pos, b_soldier_2_pos,
                                           b_soldier_3_pos, b_soldier_4_pos,
                                           b_soldier_5_pos};
  std::sort(black_soldier_pos.begin(), black_soldier_pos.end());
  res2 |= (static_cast<uint64_t>(red_soldier_pos[0]) << 32) |
          (static_cast<uint64_t>(red_soldier_pos[1]) << 24) |
          (static_cast<uint64_t>(red_soldier_pos[2]) << 16) |
          (static_cast<uint64_t>(red_soldier_pos[3]) << 8) |
          (static_cast<uint64_t>(red_soldier_pos[4]));
  res4 |= (static_cast<uint64_t>(black_soldier_pos[0]) << 32) |
          (static_cast<uint64_t>(black_soldier_pos[1]) << 24) |
          (static_cast<uint64_t>(black_soldier_pos[2]) << 16) |
          (static_cast<uint64_t>(black_soldier_pos[3]) << 8) |
          (static_cast<uint64_t>(black_soldier_pos[4]));
  return {res1, res2, res3, res4};
}

}  // namespace xq
