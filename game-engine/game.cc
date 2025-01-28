#include "game.h"

#include "moves.h"
#include "types.h"

namespace xiangqi {

namespace {

constexpr Board<Piece> kInitState = {
    B_JU_L,  B_MA_L,    B_XIANG_L, B_SHI_L, B_SHUAI,
    B_SHI_R, B_XIANG_R, B_MA_R,    B_JU_R,  // Row 0
    EMPTY,   EMPTY,     EMPTY,     EMPTY,   EMPTY,
    EMPTY,   EMPTY,     EMPTY,     EMPTY,  // Row 1
    EMPTY,   B_PAO_L,   EMPTY,     EMPTY,   EMPTY,
    EMPTY,   EMPTY,     B_PAO_R,   EMPTY,  // Row 2
    B_ZU_1,  EMPTY,     B_ZU_2,    EMPTY,   B_ZU_3,
    EMPTY,   B_ZU_4,    EMPTY,     B_ZU_5,  // Row 3
    EMPTY,   EMPTY,     EMPTY,     EMPTY,   EMPTY,
    EMPTY,   EMPTY,     EMPTY,     EMPTY,  // Row 4
    EMPTY,   EMPTY,     EMPTY,     EMPTY,   EMPTY,
    EMPTY,   EMPTY,     EMPTY,     EMPTY,  // Row 5
    R_ZU_1,  EMPTY,     R_ZU_2,    EMPTY,   R_ZU_3,
    EMPTY,   R_ZU_4,    EMPTY,     R_ZU_5,  // Row 6
    EMPTY,   R_PAO_L,   EMPTY,     EMPTY,   EMPTY,
    EMPTY,   EMPTY,     R_PAO_R,   EMPTY,  // Row 7
    EMPTY,   EMPTY,     EMPTY,     EMPTY,   EMPTY,
    EMPTY,   EMPTY,     EMPTY,     EMPTY,  // Row 8
    R_JU_L,  R_MA_L,    R_XIANG_L, R_SHI_L, R_SHUAI,
    R_SHI_R, R_XIANG_R, R_MA_R,    R_JU_R,  // Row 9
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
  Board next = history_.back();
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
    case R_SHUAI:
    case B_SHUAI:
      return PossibleMovesGeneral(board, pos);
      // TODO
  }
  return {false};
}

}  // namespace xiangqi
