#include "xiangqi/game.h"

#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <utility>

#include "xiangqi/internal/moves.h"
#include "xiangqi/types.h"

namespace xiangqi {

namespace {

using enum Piece;

constexpr Board<Piece> kInitState = {
    B_CHARIOT_L, B_HORSE_L,    B_ELEPHANT_L, B_ADVISOR_L, B_GENERAL,
    B_ADVISOR_R, B_ELEPHANT_R, B_HORSE_R,    B_CHARIOT_R,  // Row 0
    EMPTY,       EMPTY,        EMPTY,        EMPTY,       EMPTY,
    EMPTY,       EMPTY,        EMPTY,        EMPTY,  // Row 1
    EMPTY,       B_CANNON_L,   EMPTY,        EMPTY,       EMPTY,
    EMPTY,       EMPTY,        B_CANNON_R,   EMPTY,  // Row 2
    B_SOLDIER_1, EMPTY,        B_SOLDIER_2,  EMPTY,       B_SOLDIER_3,
    EMPTY,       B_SOLDIER_4,  EMPTY,        B_SOLDIER_5,  // Row 3
    EMPTY,       EMPTY,        EMPTY,        EMPTY,       EMPTY,
    EMPTY,       EMPTY,        EMPTY,        EMPTY,  // Row 4
    EMPTY,       EMPTY,        EMPTY,        EMPTY,       EMPTY,
    EMPTY,       EMPTY,        EMPTY,        EMPTY,  // Row 5
    R_SOLDIER_1, EMPTY,        R_SOLDIER_2,  EMPTY,       R_SOLDIER_3,
    EMPTY,       R_SOLDIER_4,  EMPTY,        R_SOLDIER_5,  // Row 6
    EMPTY,       R_CANNON_L,   EMPTY,        EMPTY,       EMPTY,
    EMPTY,       EMPTY,        R_CANNON_R,   EMPTY,  // Row 7
    EMPTY,       EMPTY,        EMPTY,        EMPTY,       EMPTY,
    EMPTY,       EMPTY,        EMPTY,        EMPTY,  // Row 8
    R_CHARIOT_L, R_HORSE_L,    R_ELEPHANT_L, R_ADVISOR_L, R_GENERAL,
    R_ADVISOR_R, R_ELEPHANT_R, R_HORSE_R,    R_CHARIOT_R,  // Row 9
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

void Game::Reset(Board<Piece>&& board) {
  using enum Player;
  moves_.clear();
  history_.clear();
  history_.emplace_back(std::move(board));
  player_ = RED;
}

void Game::Reset() {
  Board<Piece> board = kInitState;
  Reset(std::move(board));
}

void Game::Reset(std::unordered_map<Piece, Position>&& piece_pos) {
  Reset(pieceMapToBoard(std::move(piece_pos)));
}

Player Game::Turn() const { return player_; }

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

bool Game::Move(Position from, Position to) {
  using enum Player;
  if (from.row == to.row && from.col == to.col) {
    return false;
  }
  const Piece piece = PieceAt(from);
  const auto piece_value = static_cast<std::underlying_type_t<Piece>>(piece);
  if (piece == Piece::EMPTY || player_ == RED && piece_value < 0 ||
      player_ == BLACK && piece_value > 0) {
    return false;
  }

  Board<Piece> next = history_.back();
  const bool taken = next[to.row][to.col] != EMPTY;
  next[to.row][to.col] = PieceAt(from);
  next[from.row][from.col] = EMPTY;
  moves_.emplace_back(piece, from, to);
  history_.emplace_back(next);
  player_ = player_ == RED ? BLACK : RED;
  return taken;
}

bool Game::Undo() {
  using enum Player;

  if (history_.size() <= 1) {
    return false;
  }

  moves_.pop_back();
  history_.pop_back();
  player_ = player_ == RED ? BLACK : RED;
  return true;
}

Board<bool> Game::PossibleMoves(Position pos) const {
  using namespace xiangqi::internal::util;
  using enum Piece;
  const Piece piece = PieceAt(pos);
  const Board<Piece>& board = history_.back();
  switch (piece) {
    case EMPTY:
      return PossibleMovesEmpty(board, pos);
    case R_GENERAL:
    case B_GENERAL:
      return PossibleMovesGeneral(board, pos);
    case R_ADVISOR_L:
    case R_ADVISOR_R:
    case B_ADVISOR_L:
    case B_ADVISOR_R:
      return PossibleMovesAdvisor(board, pos);
    case R_ELEPHANT_L:
    case R_ELEPHANT_R:
    case B_ELEPHANT_L:
    case B_ELEPHANT_R:
      return PossibleMovesElephant(board, pos);
    case R_HORSE_L:
    case R_HORSE_R:
    case B_HORSE_L:
    case B_HORSE_R:
      return PossibleMovesHorse(board, pos);
    case R_CHARIOT_L:
    case R_CHARIOT_R:
    case B_CHARIOT_L:
    case B_CHARIOT_R:
      return PossibleMovesChariot(board, pos);
    case R_CANNON_L:
    case R_CANNON_R:
    case B_CANNON_L:
    case B_CANNON_R:
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
    return Winner::DRAW;  // Default when game is not over.
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

}  // namespace xiangqi
