#ifndef XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_BOARD_H_
#define XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_BOARD_H_

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "xiangqi/types.h"

namespace xq {

using MovesPerPiece = std::array<Position, 17>;

constexpr Board kStartingBoard = {
    B_CHARIOT,   B_HORSE,     B_ELEPHANT,  B_ADVISOR,   B_GENERAL,
    B_ADVISOR,   B_ELEPHANT,  B_HORSE,     B_CHARIOT,  // Row 0
    PIECE_EMPTY, PIECE_EMPTY, PIECE_EMPTY, PIECE_EMPTY, PIECE_EMPTY,
    PIECE_EMPTY, PIECE_EMPTY, PIECE_EMPTY, PIECE_EMPTY,  // Row 1
    PIECE_EMPTY, B_CANNON,    PIECE_EMPTY, PIECE_EMPTY, PIECE_EMPTY,
    PIECE_EMPTY, PIECE_EMPTY, B_CANNON,    PIECE_EMPTY,  // Row 2
    B_SOLDIER,   PIECE_EMPTY, B_SOLDIER,   PIECE_EMPTY, B_SOLDIER,
    PIECE_EMPTY, B_SOLDIER,   PIECE_EMPTY, B_SOLDIER,  // Row 3
    PIECE_EMPTY, PIECE_EMPTY, PIECE_EMPTY, PIECE_EMPTY, PIECE_EMPTY,
    PIECE_EMPTY, PIECE_EMPTY, PIECE_EMPTY, PIECE_EMPTY,  // Row 4
    PIECE_EMPTY, PIECE_EMPTY, PIECE_EMPTY, PIECE_EMPTY, PIECE_EMPTY,
    PIECE_EMPTY, PIECE_EMPTY, PIECE_EMPTY, PIECE_EMPTY,  // Row 5
    R_SOLDIER,   PIECE_EMPTY, R_SOLDIER,   PIECE_EMPTY, R_SOLDIER,
    PIECE_EMPTY, R_SOLDIER,   PIECE_EMPTY, R_SOLDIER,  // Row 6
    PIECE_EMPTY, R_CANNON,    PIECE_EMPTY, PIECE_EMPTY, PIECE_EMPTY,
    PIECE_EMPTY, PIECE_EMPTY, R_CANNON,    PIECE_EMPTY,  // Row 7
    PIECE_EMPTY, PIECE_EMPTY, PIECE_EMPTY, PIECE_EMPTY, PIECE_EMPTY,
    PIECE_EMPTY, PIECE_EMPTY, PIECE_EMPTY, PIECE_EMPTY,  // Row 8
    R_CHARIOT,   R_HORSE,     R_ELEPHANT,  R_ADVISOR,   R_GENERAL,
    R_ADVISOR,   R_ELEPHANT,  R_HORSE,     R_CHARIOT,  // Row 9
};

// Construct board from human-readable string.
// Example:
// "  A B C D E F G H I \n"
// "0 r h e a g a e h r \n"
// "1 . . . * * * . . . \n"
// "2 . c . * * * . c . \n"
// "3 s . s . s . s . s \n"
// "4 - - - - - - - - - \n"
// "5 - - - - - - - - - \n"
// "7 S . S . S . S . S \n"
// "7 . C . * * * . C . \n"
// "8 . . . * * * . . . \n"
// "9 R H E A G A E H R \n";
Board BoardFromString(std::string_view str);

// Convert board to human-readable string.
std::string BoardToString(const Board& board);

// Check if two boards are identical.
bool BoardEq(const Board& a, const Board& b);

bool operator==(const Board& lhs, const Board& rhs);

// C++ wrapper of FindGeneral_C.
Position FindGeneral(const Board& board, Player player);

// C++ wrapper of IsBeingCheckmate_C.
bool IsBeingCheckmate(const Board& board, Player player);

// C++ wrapper of GetWinner_C.
Winner GetWinner(const Board& board);

// Returns true if all possible moves of the given player still result in the
// player being checkmate.
bool DidPlayerLose(const Board& board, Player player);

// C++ wrapper of FlipBoard_C.
Board FlipBoard(const Board& board);

// C++ wrapper of MirrorBoardHorizontal_C.
Board MirrorBoardHorizontal(const Board& board);

// C++ wrapper of MirrorBoardVertical_C.
Board MirrorBoardVertical(const Board& board);

// C++ wrapper of Move_C.
Piece Move(Board& board, Movement movement);

// Returns all possible moves for the player with piece at position. Impossible
// moves are filled with kNoPosition.
// If avoid_checkmate is set to true, moves that result in being checkmade
// will not be included.
MovesPerPiece PossibleMoves(const Board& board, Position pos,
                            bool avoid_checkmate = false);

// Returns a vector of all possible moves for player. Each move is a 16-bit
// integer, representing "from" and "to" positions, each being 8 bits
// (Position).
std::vector<Movement> AllPossibleNextMoves(const Board& board, Player player,
                                           bool avoid_checkmate = false);

// Returns a vector of all possible boards for the given player after any valid
// move.
// If avoid_checkmate is set to true, moves that result in being checkmade
// will not be included.
std::vector<Board> AllPossibleNextBoards(const Board& board, Player player,
                                         bool avoid_checkmate = false);

// C++ wrapper of EncodeBoardState_C.
BoardState EncodeBoardState(const Board& board);

// Decode the encoded board state back to its original state.
Board DecodeBoardState(const BoardState& state);

}  // namespace xq

#endif  // XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_BOARD_H_
