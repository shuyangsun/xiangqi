#include "xiangqi/game.h"

#include <cctype>
#include <unordered_map>
#include <utility>
#include <vector>

#include "xiangqi/board.h"
#include "xiangqi/types.h"

namespace xq {

namespace {

using enum Piece;

constexpr Board<Piece> kInitState = {
    B_CHARIOT, B_HORSE,    B_ELEPHANT, B_ADVISOR, B_GENERAL,
    B_ADVISOR, B_ELEPHANT, B_HORSE,    B_CHARIOT,  // Row 0
    EMPTY,     EMPTY,      EMPTY,      EMPTY,     EMPTY,
    EMPTY,     EMPTY,      EMPTY,      EMPTY,  // Row 1
    EMPTY,     B_CANNON,   EMPTY,      EMPTY,     EMPTY,
    EMPTY,     EMPTY,      B_CANNON,   EMPTY,  // Row 2
    B_SOLDIER, EMPTY,      B_SOLDIER,  EMPTY,     B_SOLDIER,
    EMPTY,     B_SOLDIER,  EMPTY,      B_SOLDIER,  // Row 3
    EMPTY,     EMPTY,      EMPTY,      EMPTY,     EMPTY,
    EMPTY,     EMPTY,      EMPTY,      EMPTY,  // Row 4
    EMPTY,     EMPTY,      EMPTY,      EMPTY,     EMPTY,
    EMPTY,     EMPTY,      EMPTY,      EMPTY,  // Row 5
    R_SOLDIER, EMPTY,      R_SOLDIER,  EMPTY,     R_SOLDIER,
    EMPTY,     R_SOLDIER,  EMPTY,      R_SOLDIER,  // Row 6
    EMPTY,     R_CANNON,   EMPTY,      EMPTY,     EMPTY,
    EMPTY,     EMPTY,      R_CANNON,   EMPTY,  // Row 7
    EMPTY,     EMPTY,      EMPTY,      EMPTY,     EMPTY,
    EMPTY,     EMPTY,      EMPTY,      EMPTY,  // Row 8
    R_CHARIOT, R_HORSE,    R_ELEPHANT, R_ADVISOR, R_GENERAL,
    R_ADVISOR, R_ELEPHANT, R_HORSE,    R_CHARIOT,  // Row 9
};

// Converts a piece map to a board. The resulting board will be completely empty
// (all cells set to EMPTY) except for the positions specified in the map.
Board<Piece> pieceMapToBoard(
    const std::unordered_map<Position, Piece>& pos_piece) {
  Board<Piece> board;
  for (auto& row : board) {
    row.fill(EMPTY);
  }
  for (const auto& entry : pos_piece) {
    Position pos = entry.first;
    const Piece piece = entry.second;
    if (Row(pos) < kTotalRow && Col(pos) < kTotalCol) {
      board[pos] = piece;
    }
  }
  return board;
}

}  // namespace

Game::Game() : history_{kInitState} {}

void Game::ResetFromBoard(const Board<Piece>& board) {
  using enum Player;
  moves_.clear();
  history_.clear();
  history_.emplace_back(std::move(board));
  player_ = RED;
}

void Game::Reset() { ResetFromBoard(kInitState); }

void Game::ResetFromPos(const std::unordered_map<Position, Piece>& pos_piece) {
  ResetFromBoard(pieceMapToBoard(pos_piece));
}

Player Game::CurrentPlayer() const { return player_; }

size_t Game::MovesCount() const { return moves_.size(); }

void Game::MakeBlackMoveFirst() {
  if (history_.size() > 1) {
    return;
  }
  player_ = Player::BLACK;
}

Board<Piece> Game::StartingBoard() const { return {history_.front()}; }

Board<Piece> Game::CurrentBoard() const { return {history_.back()}; }

Piece Game::PieceAt(Position pos) const {
  return history_.back()[Row(pos)][Col(pos)];
}

Piece Game::Move(Position from, Position to) {
  Board<Piece> next = history_.back();
  const Piece piece = next[Row(from)][Col(from)];
  const Piece captured = xq::Move(next, from, to);
  moves_.emplace_back(piece, from, to, captured);
  history_.emplace_back(next);
  player_ = player_ == Player::RED ? Player::BLACK : Player::RED;
  return captured;
}

bool Game::CanUndo() const { return history_.size() > 1; }

MoveAction Game::Undo() {
  using enum Player;

  if (!CanUndo()) {
    // Dummy move action.
    return {Piece::EMPTY, kNoPosition, kNoPosition, Piece::EMPTY};
  }
  MoveAction result = moves_.back();
  moves_.pop_back();
  history_.pop_back();
  player_ = player_ == RED ? BLACK : RED;
  return result;
}

std::vector<uint16_t> Game::ExportMoves() const {
  std::vector<uint16_t> result;
  result.reserve(moves_.size());
  for (const MoveAction& move : moves_) {
    result.emplace_back((static_cast<uint16_t>(move.from) << 8) |
                        (static_cast<uint16_t>(move.to)));
  }
  return result;
}

void Game::RestoreMoves(const std::vector<uint16_t>& moves) {
  for (const uint16_t move : moves) {
    Move(static_cast<Position>((move & 0xFF00) >> 8),
         static_cast<Position>(move & 0x00FF));
  }
}

}  // namespace xq
