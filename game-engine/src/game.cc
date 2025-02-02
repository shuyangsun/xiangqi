#include "xiangqi/game.h"

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

}  // namespace

Game::Game() : history_{kInitState} {}

void Game::Reset() {
  moves_.clear();
  history_.clear();
  history_.emplace_back(kInitState);
  is_red_turn_ = true;
}

void Game::ChangeTurn() { is_red_turn_ = !is_red_turn_; }

Board<Piece> Game::CurrentBoard() { return {history_.back()}; }

Piece Game::PieceAt(Position pos) { return history_.back()[pos.row][pos.col]; }

bool Game::Move(Position from, Position to) {
  Board<Piece> next = history_.back();
  const Piece piece = PieceAt(from);
  next[to.row][to.col] = PieceAt(from);
  next[from.row][from.col] = EMPTY;
  moves_.emplace_back(piece, from, to);
  history_.emplace_back(next);
  return false;  // TODO: return check or not.
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
      return {false};  // TODO: remove this default switch statement.
  }
  return {false};
}

}  // namespace xiangqi
