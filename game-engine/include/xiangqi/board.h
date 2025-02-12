#ifndef XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_BOARD_H_
#define XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_BOARD_H_

#include <cstdint>
#include <vector>

#include "xiangqi/types.h"

namespace xq {

constexpr Board<Piece> kStartingBoard = {
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

// Returns true if it's checkmade for the given player. I.e., if the
// current player does not take certain actions, the opponent can capture
// the general with their next move.
bool IsCheckMade(const Board<Piece>& board, Player player);

// Returns the winner if the game is over. Returns NONE if the game is not
// over.
Winner GetWinner(const Board<Piece>& board);

// Returns all possible moves for the player with piece at position. Impossible
// moves are filled with kNoPosition.
std::array<Position, 17> PossibleMoves(const Board<Piece>& board, Position pos);

// Move a piece from a position to another position, returns the captured
// piece. If no piece was captured, return EMPTY.
Piece Move(Board<Piece>& board, Position from, Position to);

// Returns a vector of all possible moves for player. Each move is a 16-bit
// integer, representing "from" and "to" positions, each being 8 bits
// (Position).
std::vector<uint16_t> AllPossibleNextMoves(const Board<Piece>& board,
                                           Player player);

// Returns a vector of all possible boards for the given player after any valid
// move.
std::vector<Board<Piece>> AllPossibleNextBoards(const Board<Piece>& board,
                                                Player player);

// Rotate the position so that it's from the opponent's perspective.
Position FlipPosition(Position position);

// Rotate the board so that it's from the opponent's perspective.
Board<Piece> FlipBoard(const Board<Piece>& board);

// Encode the board state using a small number of bytes, mainly used for the
// game AI to identify a unique board state.
//
// There are a total of 32 pieces on the board, so the encoded board state is
// 32 bytes, with each byte representing the row and column of that piece
// (4 bits for each). If the piece is not present, use 0xF to represent it.
// To make sure the same piece for the same player is treated in the same way,
// all byte representations of a group of piece is sorted.
std::array<uint64_t, 4> EncodeBoardState(const Board<Piece>& board);

// Decode the encoded board state back to its original state.
Board<Piece> DecodeBoardState(const std::array<uint64_t, 4> state);

}  // namespace xq

#endif  // XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_BOARD_H_
