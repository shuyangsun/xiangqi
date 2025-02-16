#include "xiangqi/game.h"

#include <cctype>
#include <unordered_map>
#include <utility>
#include <vector>

#include "xiangqi/board.h"
#include "xiangqi/types.h"

namespace xq {

Game::Game() : board_{kStartingBoard} {}

void Game::Restart() {
  board_ = kStartingBoard;
  initial_board_state_ = std::nullopt;
  moves_.clear();
  captured_.clear();
}

Player Game::CurrentPlayer() const { return player_; }

size_t Game::MovesCount() const { return moves_.size(); }

void Game::MakeBlackMoveFirst() {
  if (moves_.size() > 1) {
    return;
  }
  player_ = Player::BLACK;
}

BoardState Game::InitialBoardState() const {
  if (initial_board_state_.has_value()) {
    return *initial_board_state_;
  }
  return EncodeBoardState(kStartingBoard);
}

Piece Game::PieceAt(Position pos) const { return board_[pos]; }

Piece Game::Move(const Movement move) {
  moves_.emplace_back(move);
  player_ = ChangePlayer(player_);
  const Piece captured = xq::Move(board_, move);
  captured_.emplace_back(captured);
  return captured;
}

bool Game::CanUndo() const { return moves_.size() > 1; }

Movement Game::Undo() {
  using enum Player;

  if (!CanUndo()) {
    return kNoMovement;
  }
  Movement result = moves_.back();
  const Piece captured = captured_.back();
  moves_.pop_back();
  captured_.pop_back();
  player_ = ChangePlayer(player_);

  xq::Move(board_, NewMovement(Dest(result), Orig(result)));
  if (!IsEmpty(captured)) {
    board_[Dest(result)] = captured;
  }
  return result;
}

std::vector<Movement> Game::ExportMoves() const { return moves_; }

// Restores game state from inital state.
void Game::RestoreBoard(const BoardState& state) {
  initial_board_state_ = state;
  board_ = DecodeBoardState(state);
  moves_.clear();
  captured_.clear();
  player_ = Player::RED;
}

void Game::RestoreMoves(const std::vector<Movement>& moves) {
  if (moves.empty()) {
    return;
  }
  player_ = IsRed(board_[Orig(moves.back())]) ? Player::BLACK : Player::RED;
  moves_ = moves;
  captured_.clear();
  captured_.reserve(moves.size());
  for (const Movement move : moves) {
    captured_.emplace_back(xq::Move(board_, move));
  }
}

}  // namespace xq
