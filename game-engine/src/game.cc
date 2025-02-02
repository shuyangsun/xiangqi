#include "xiangqi/game.h"

#include <unordered_map>
#include <utility>

#include "xiangqi/moves.h"
#include "xiangqi/types.h"

namespace xiangqi {

namespace {

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
  moves_.clear();
  history_.clear();
  history_.emplace_back(std::move(board));
  is_red_turn_ = true;
}

void Game::Reset() {
  Board<Piece> board = kInitState;
  Reset(std::move(board));
}

void Game::Reset(std::unordered_map<Piece, Position>&& piece_pos) {
  Reset(pieceMapToBoard(std::move(piece_pos)));
}

void Game::ChangeTurn() { is_red_turn_ = !is_red_turn_; }

Board<Piece> Game::CurrentBoard() { return {history_.back()}; }

Piece Game::PieceAt(Position pos) { return history_.back()[pos.row][pos.col]; }

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

Board<bool> Game::PossibleMoves(Position pos) {
  using namespace xiangqi::internal::util;
  const Piece piece = PieceAt(pos);
  const Board<Piece>& board = history_.back();
  switch (piece) {
    case EMPTY:
      return {false};
    case R_GENERAL:
    case B_GENERAL:
      return PossibleMovesGeneral(board, pos);
    default:
      return {false};  // TODO: please implement other scenarios.
  }
  return {false};
}

}  // namespace xiangqi
