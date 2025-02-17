#include "xiangqi/board.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>

#include "xiangqi/board_c.h"
#include "xiangqi/internal/moves.h"
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

bool DidPlayerLose(const Board& board, Player player) {
  using namespace xq::internal::util;

  const Winner opponent = player == PLAYER_RED ? WINNER_BLACK : WINNER_RED;
  const Piece opponent_general = player == PLAYER_RED ? B_GENERAL : R_GENERAL;
  if (GetWinner(board) == opponent) {
    return true;
  }

  for (Position pos = 0; pos < K_BOARD_SIZE; pos++) {
    const Piece piece = board[pos];
    if (piece == PIECE_EMPTY || IsRed(piece) != (player == PLAYER_RED)) {
      continue;
    }
    switch (piece) {
      case R_GENERAL:
        for (const Position to : PossibleMovesGeneral(
                 board, pos, FindGeneral(board, PLAYER_BLACK))) {
          if (to == K_NO_POSITION) {
            break;
          }
          Board next = board;
          const Piece capture = Move(next, NewMovement(pos, to));
          if (capture == opponent_general || !IsBeingCheckmate(next, player)) {
            return false;
          }
        }
        break;
      case B_GENERAL:
        for (const Position to :
             PossibleMovesGeneral(board, pos, FindGeneral(board, PLAYER_RED))) {
          if (to == K_NO_POSITION) {
            break;
          }
          Board next = board;
          const Piece capture = Move(next, NewMovement(pos, to));
          if (capture == opponent_general || !IsBeingCheckmate(next, player)) {
            return false;
          }
        }
        break;
      case R_ADVISOR:
      case B_ADVISOR:
        for (const Position to : PossibleMovesAdvisor(board, pos)) {
          if (to == K_NO_POSITION) {
            break;
          }
          Board next = board;
          const Piece capture = Move(next, NewMovement(pos, to));
          if (capture == opponent_general || !IsBeingCheckmate(next, player)) {
            return false;
          }
        }
        break;
      case R_ELEPHANT:
      case B_ELEPHANT:
        for (const Position to : PossibleMovesElephant(board, pos)) {
          if (to == K_NO_POSITION) {
            break;
          }
          Board next = board;
          const Piece capture = Move(next, NewMovement(pos, to));
          if (capture == opponent_general || !IsBeingCheckmate(next, player)) {
            return false;
          }
        }
        break;
      case R_HORSE:
      case B_HORSE:
        for (const Position to : PossibleMovesHorse(board, pos)) {
          if (to == K_NO_POSITION) {
            break;
          }
          Board next = board;
          const Piece capture = Move(next, NewMovement(pos, to));
          if (capture == opponent_general || !IsBeingCheckmate(next, player)) {
            return false;
          }
        }
        break;
      case R_CHARIOT:
      case B_CHARIOT:
        for (const Position to : PossibleMovesChariot(board, pos)) {
          if (to == K_NO_POSITION) {
            break;
          }
          Board next = board;
          const Piece capture = Move(next, NewMovement(pos, to));
          if (capture == opponent_general || !IsBeingCheckmate(next, player)) {
            return false;
          }
        }
        break;
      case R_CANNON:
      case B_CANNON:
        for (const Position to : PossibleMovesCannon(board, pos)) {
          if (to == K_NO_POSITION) {
            break;
          }
          Board next = board;
          const Piece capture = Move(next, NewMovement(pos, to));
          if (capture == opponent_general || !IsBeingCheckmate(next, player)) {
            return false;
          }
        }
        break;
      case R_SOLDIER:
      case B_SOLDIER:
        for (const Position to : PossibleMovesSoldier(board, pos)) {
          if (to == K_NO_POSITION) {
            break;
          }
          Board next = board;
          const Piece capture = Move(next, NewMovement(pos, to));
          if (capture == opponent_general || !IsBeingCheckmate(next, player)) {
            return false;
          }
        }
        break;
      default:
        continue;
    }
  }
  return true;
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

MovesPerPiece PossibleMoves(const Board& board, const Position pos,
                            const bool avoid_checkmate) {
  using namespace xq::internal::util;

  const Piece piece = board[pos];
  MovesPerPiece result;
  result.fill(K_NO_POSITION);
  switch (piece) {
    case PIECE_EMPTY:
      return result;
    case R_GENERAL:
      memcpy(result.data(),
             PossibleMovesGeneral(board, pos, FindGeneral(board, PLAYER_BLACK))
                 .data(),
             5);
      break;
    case B_GENERAL:
      memcpy(result.data(),
             PossibleMovesGeneral(board, pos, FindGeneral(board, PLAYER_RED))
                 .data(),
             5);
      break;
    case R_ADVISOR:
    case B_ADVISOR:
      memcpy(result.data(), PossibleMovesAdvisor(board, pos).data(), 4);
      break;
    case R_ELEPHANT:
    case B_ELEPHANT:
      memcpy(result.data(), PossibleMovesElephant(board, pos).data(), 4);
      break;
    case R_HORSE:
    case B_HORSE:
      memcpy(result.data(), PossibleMovesHorse(board, pos).data(), 8);
      break;
    case R_CHARIOT:
    case B_CHARIOT:
      memcpy(result.data(), PossibleMovesChariot(board, pos).data(), 17);
      break;
    case R_CANNON:
    case B_CANNON:
      memcpy(result.data(), PossibleMovesCannon(board, pos).data(), 17);
      break;
    case R_SOLDIER:
    case B_SOLDIER:
      memcpy(result.data(), PossibleMovesSoldier(board, pos).data(), 3);
      break;
    default:
      return result;
  }
  if (avoid_checkmate) {
    const Player player = IsRed(piece) ? PLAYER_RED : PLAYER_BLACK;
    for (size_t i = 0; i < result.size() && result[i] != K_NO_POSITION; i++) {
      Board next = board;
      const Piece captured = Move(next, NewMovement(pos, result[i]));
      if (captured == R_GENERAL || captured == B_GENERAL) {
        continue;
      }
      if (IsBeingCheckmate(next, player)) {
        result[i] = K_NO_POSITION;
      }
    }
    std::sort(result.begin(), result.end());
  }
  return result;
}

Piece Move(Board& board, const Movement movement) {
  return Move_C(board.data(), movement);
}

// Returns a vector of all possible moves for player.
std::vector<uint16_t> AllPossibleNextMoves(const Board& board,
                                           const Player player,
                                           const bool avoid_checkmate) {
  using namespace xq::internal::util;

  std::vector<uint16_t> result;
  for (Position pos = 0; pos < K_BOARD_SIZE; pos++) {
    const Piece piece = board[pos];
    if (piece == PIECE_EMPTY || IsRed(piece) != (player == PLAYER_RED)) {
      continue;
    }
    const auto from_16bit = static_cast<uint16_t>(pos << 8);
    switch (piece) {
      case R_GENERAL:
        for (const Position to : PossibleMovesGeneral(
                 board, pos, FindGeneral(board, PLAYER_BLACK))) {
          if (to == K_NO_POSITION) {
            break;
          }
          result.emplace_back(from_16bit | static_cast<uint16_t>(to));
        }
        break;
      case B_GENERAL:
        for (const Position to :
             PossibleMovesGeneral(board, pos, FindGeneral(board, PLAYER_RED))) {
          if (to == K_NO_POSITION) {
            break;
          }
          result.emplace_back(from_16bit | static_cast<uint16_t>(to));
        }
        break;
      case R_ADVISOR:
      case B_ADVISOR:
        for (const Position to : PossibleMovesAdvisor(board, pos)) {
          if (to == K_NO_POSITION) {
            break;
          }
          result.emplace_back(from_16bit | static_cast<uint16_t>(to));
        }
        break;
      case R_ELEPHANT:
      case B_ELEPHANT:
        for (const Position to : PossibleMovesElephant(board, pos)) {
          if (to == K_NO_POSITION) {
            break;
          }
          result.emplace_back(from_16bit | static_cast<uint16_t>(to));
        }
        break;
      case R_HORSE:
      case B_HORSE:
        for (const Position to : PossibleMovesHorse(board, pos)) {
          if (to == K_NO_POSITION) {
            break;
          }
          result.emplace_back(from_16bit | static_cast<uint16_t>(to));
        }
        break;
      case R_CHARIOT:
      case B_CHARIOT:
        for (const Position to : PossibleMovesChariot(board, pos)) {
          if (to == K_NO_POSITION) {
            break;
          }
          result.emplace_back(from_16bit | static_cast<uint16_t>(to));
        }
        break;
      case R_CANNON:
      case B_CANNON:
        for (const Position to : PossibleMovesCannon(board, pos)) {
          if (to == K_NO_POSITION) {
            break;
          }
          result.emplace_back(from_16bit | static_cast<uint16_t>(to));
        }
        break;
      case R_SOLDIER:
      case B_SOLDIER:
        for (const Position to : PossibleMovesSoldier(board, pos)) {
          if (to == K_NO_POSITION) {
            break;
          }
          result.emplace_back(from_16bit | static_cast<uint16_t>(to));
        }
        break;
      default:
        continue;
    }
  }
  if (avoid_checkmate) {
    std::vector<Movement> final_result;
    for (const Movement move : result) {
      const Player player =
          IsRed(board[Orig(move)]) ? PLAYER_RED : PLAYER_BLACK;
      Board next = board;
      const Piece captured = Move(next, move);
      if (captured == R_GENERAL || captured == B_GENERAL ||
          !IsBeingCheckmate(next, player)) {
        final_result.emplace_back(move);
      }
    }
  }
  return result;
}

std::vector<Board> AllPossibleNextBoards(const Board& board,
                                         const Player player,
                                         const bool avoid_checkmate) {
  const std::vector<uint16_t> possible_moves =
      AllPossibleNextMoves(board, player, avoid_checkmate);
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
