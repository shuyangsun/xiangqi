#include "xiangqi/game.h"

#include <algorithm>
#include <optional>
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

void Game::ChangeTurn() {
  using enum Player;
  player_ = player_ == RED ? BLACK : RED;
}

Board<Piece> Game::CurrentBoard() const { return {history_.back()}; }

Piece Game::PieceAt(Position pos) const {
  return history_.back()[pos.row][pos.col];
}

bool Game::Move(Position from, Position to) {
  if (from.row == to.row && from.col == to.col) {
    return false;
  }

  Board<Piece> next = history_.back();
  const Piece piece = PieceAt(from);
  const bool taken = next[to.row][to.col] != EMPTY;
  next[to.row][to.col] = PieceAt(from);
  next[from.row][from.col] = EMPTY;
  moves_.emplace_back(piece, from, to);
  history_.emplace_back(next);
  return taken;
}

bool Game::Undo() {
  if (history_.size() <= 1) {
    return false;
  }

  moves_.pop_back();
  history_.pop_back();
  ChangeTurn();
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
  // Use the current board.
  const Board<Piece>& board = history_.back();

  // Find the positions of the red and black generals.
  std::optional<Position> redGeneralPos;
  std::optional<Position> blackGeneralPos;
  for (uint8_t r = 0; r < kTotalRow; ++r) {
    for (uint8_t c = 0; c < kTotalCol; ++c) {
      if (board[r][c] == R_GENERAL)
        redGeneralPos = Position{r, c};
      else if (board[r][c] == B_GENERAL)
        blackGeneralPos = Position{r, c};
    }
  }
  // If one (or both) general is missing, then a check condition exists.
  if (!redGeneralPos.has_value() || !blackGeneralPos.has_value()) {
    return true;
  }

  // Check for the flying general condition:
  // If the two generals are in the same column and there are no pieces between
  // them, then the flying general check is triggered.
  if (redGeneralPos->col == blackGeneralPos->col) {
    int col = redGeneralPos->col;
    int startRow = std::min(redGeneralPos->row, blackGeneralPos->row) + 1;
    int endRow = std::max(redGeneralPos->row, blackGeneralPos->row);
    bool blocked = false;
    for (int r = startRow; r < endRow; ++r) {
      if (board[r][col] != EMPTY) {
        blocked = true;
        break;
      }
    }
    if (!blocked) {
      return true;
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

std::optional<Winner> Game::GetWinner() const {
  // If the game is not over, there is no winner.
  if (!IsGameOver()) {
    return std::nullopt;
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
  // This point should never be reached if IsGameOver() is true.
  return std::nullopt;
}

}  // namespace xiangqi
