#include "xiangqi/board.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>

#include "xiangqi/board_c.h"
#include "xiangqi/types.h"

namespace xq {

namespace {

char PieceToCh(const Piece piece, const uint8_t row, const uint8_t col) {
  switch (piece) {
    case PIECE_EMPTY:
      if (row == 4 || row == 5) {
        return '-';
      } else if (col >= 3 && col <= 5 &&
                 ((row >= 0 && row <= 2) || (row >= 7 && row <= 9))) {
        return '*';
      } else {
        return '.';
      }
    case R_GENERAL:
      return 'G';
    case R_ADVISOR:
      return 'A';
    case R_ELEPHANT:
      return 'E';
    case R_HORSE:
      return 'H';
    case R_CHARIOT:
      return 'R';
    case R_CANNON:
      return 'C';
    case R_SOLDIER:
      return 'S';
    case B_GENERAL:
      return 'g';
    case B_ADVISOR:
      return 'a';
    case B_ELEPHANT:
      return 'e';
    case B_HORSE:
      return 'h';
    case B_CHARIOT:
      return 'r';
    case B_CANNON:
      return 'c';
    case B_SOLDIER:
      return 's';
    default:
      return '?';
  }
}

Piece ChToPiece(const char ch) {
  switch (ch) {
    case '.':
      return PIECE_EMPTY;
    case 'G':
      return R_GENERAL;
    case 'A':
      return R_ADVISOR;
    case 'E':
      return R_ELEPHANT;
    case 'H':
      return R_HORSE;
    case 'R':
      return R_CHARIOT;
    case 'C':
      return R_CANNON;
    case 'S':
      return R_SOLDIER;
    case 'g':
      return B_GENERAL;
    case 'a':
      return B_ADVISOR;
    case 'e':
      return B_ELEPHANT;
    case 'h':
      return B_HORSE;
    case 'r':
      return B_CHARIOT;
    case 'c':
      return B_CANNON;
    case 's':
      return B_SOLDIER;
    default:
      return PIECE_EMPTY;
  }
}

}  // namespace

Board BoardFromString(const std::string_view str) {
  Board result;
  result.fill(PIECE_EMPTY);
  size_t idx = 23;
  for (uint8_t row = 0; row < K_TOTAL_ROW; row++) {
    for (uint8_t col = 0; col < K_TOTAL_COL; col++) {
      result[Pos(row, col)] = ChToPiece(str[idx]);
      idx += 2;
    }
    idx += 3;
  }
  return result;
}

std::string BoardToString(const Board& board) {
  std::string result;
  result.reserve(242);
  result.append("  A B C D E F G H I \n");
  for (uint8_t row = 0; row < K_TOTAL_ROW; row++) {
    result.append(std::to_string(row));
    result.append(" ");
    for (uint8_t col = 0; col < K_TOTAL_COL; col++) {
      result += PieceToCh(board[Pos(row, col)], row, col);
      result.append(" ");
    }
    result.append("\n");
  }
  return result;
}

bool BoardEq(const Board& a, const Board& b) {
  for (Position pos = 0; pos < K_BOARD_SIZE; pos++) {
    if (a[pos] != b[pos]) {
      return false;
    }
  }
  return true;
}

bool operator==(const Board& lhs, const Board& rhs) {
  return BoardEq(lhs, rhs);
}

bool IsBeingCheckmate(const Board& board, Player player) {
  return IsBeingCheckmate_C(board.data(), player);
}

Winner GetWinner(const Board& board) { return GetWinner_C(board.data()); }

bool DidPlayerLose(const Board& board, const Player player) {
  return DidPlayerLose_C(board.data(), player);
}

Board FlipBoard(const Board& board) {
  Board result;
  FlipBoard_C(result.data(), board.data());
  return result;
}

Board MirrorBoardHorizontal(const Board& board) {
  Board result;
  MirrorBoardHorizontal_C(result.data(), board.data());
  return result;
}

Board MirrorBoardVertical(const Board& board) {
  Board result;
  MirrorBoardVertical_C(result.data(), board.data());
  return result;
}

BoardState EncodeBoardState(const Board& board) {
  BoardState result;
  EncodeBoardState_C(board.data(), result.data());
  return result;
}

Board DecodeBoardState(const BoardState& state) {
  // Initialize an empty board.
  Board result;
  DecodeBoardState_C(state.data(), result.data());
  return result;
}

MovesPerPiece PossiblePositions(const Board& board, const Position pos,
                                const bool avoid_checkmate) {
  MovesPerPiece result;
  PossiblePositions_C(board.data(), pos, avoid_checkmate, result.data());
  return result;
}

Piece Move(Board& board, const Movement movement) {
  return Move_C(board.data(), movement);
}

// Returns a vector of all possible moves for player.
std::vector<Movement> PossibleMoves(const Board& board, const Player player,
                                    const bool avoid_checkmate) {
  MaxMovesPerPlayerC buff;
  const uint8_t num_moves =
      PossibleMoves_C(board.data(), player, avoid_checkmate, buff);
  return std::vector<Movement>{buff, buff + num_moves};
}

std::vector<Board> AllPossibleNextBoards(const Board& board,
                                         const Player player,
                                         const bool avoid_checkmate) {
  const std::vector<uint16_t> possible_moves =
      PossibleMoves(board, player, avoid_checkmate);
  std::vector<Board> result;
  result.reserve(possible_moves.size());
  for (const uint16_t move : possible_moves) {
    const Position from = static_cast<Position>((move & 0xFF00) >> 8);
    const Position to = static_cast<Position>(move & 0x00FF);
    Board next = board;
    Move(next, move);
    result.emplace_back(std::move(next));
  }
  return result;
}

Position FindGeneral(const Board& board, const Player player) {
  return FindGeneral_C(board.data(), player);
}

}  // namespace xq
