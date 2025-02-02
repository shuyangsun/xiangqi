#include "xiangqi/game.h"

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
  return false;  // TODO: placeholder implementation, please implement.
}

bool Game::IsGameOver() const {
  return false;  // TODO: placeholder implementation, please implement.
}

std::optional<Winner> Game::GetWinner() const {
  return std::nullopt;  // TODO: placeholder implementation, please implement.
}

}  // namespace xiangqi
