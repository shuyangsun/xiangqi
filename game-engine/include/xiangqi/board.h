#ifndef XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_BOARD_H_
#define XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_BOARD_H_

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "xiangqi/types.h"

namespace xq {

constexpr Board<Piece> kStartingBoard = {
    Piece::B_CHARIOT,  Piece::B_HORSE,   Piece::B_ELEPHANT,
    Piece::B_ADVISOR,  Piece::B_GENERAL, Piece::B_ADVISOR,
    Piece::B_ELEPHANT, Piece::B_HORSE,   Piece::B_CHARIOT,  // Row 0
    Piece::EMPTY,      Piece::EMPTY,     Piece::EMPTY,
    Piece::EMPTY,      Piece::EMPTY,     Piece::EMPTY,
    Piece::EMPTY,      Piece::EMPTY,     Piece::EMPTY,  // Row 1
    Piece::EMPTY,      Piece::B_CANNON,  Piece::EMPTY,
    Piece::EMPTY,      Piece::EMPTY,     Piece::EMPTY,
    Piece::EMPTY,      Piece::B_CANNON,  Piece::EMPTY,  // Row 2
    Piece::B_SOLDIER,  Piece::EMPTY,     Piece::B_SOLDIER,
    Piece::EMPTY,      Piece::B_SOLDIER, Piece::EMPTY,
    Piece::B_SOLDIER,  Piece::EMPTY,     Piece::B_SOLDIER,  // Row 3
    Piece::EMPTY,      Piece::EMPTY,     Piece::EMPTY,
    Piece::EMPTY,      Piece::EMPTY,     Piece::EMPTY,
    Piece::EMPTY,      Piece::EMPTY,     Piece::EMPTY,  // Row 4
    Piece::EMPTY,      Piece::EMPTY,     Piece::EMPTY,
    Piece::EMPTY,      Piece::EMPTY,     Piece::EMPTY,
    Piece::EMPTY,      Piece::EMPTY,     Piece::EMPTY,  // Row 5
    Piece::R_SOLDIER,  Piece::EMPTY,     Piece::R_SOLDIER,
    Piece::EMPTY,      Piece::R_SOLDIER, Piece::EMPTY,
    Piece::R_SOLDIER,  Piece::EMPTY,     Piece::R_SOLDIER,  // Row 6
    Piece::EMPTY,      Piece::R_CANNON,  Piece::EMPTY,
    Piece::EMPTY,      Piece::EMPTY,     Piece::EMPTY,
    Piece::EMPTY,      Piece::R_CANNON,  Piece::EMPTY,  // Row 7
    Piece::EMPTY,      Piece::EMPTY,     Piece::EMPTY,
    Piece::EMPTY,      Piece::EMPTY,     Piece::EMPTY,
    Piece::EMPTY,      Piece::EMPTY,     Piece::EMPTY,  // Row 8
    Piece::R_CHARIOT,  Piece::R_HORSE,   Piece::R_ELEPHANT,
    Piece::R_ADVISOR,  Piece::R_GENERAL, Piece::R_ADVISOR,
    Piece::R_ELEPHANT, Piece::R_HORSE,   Piece::R_CHARIOT,  // Row 9
};

// Construct board from human-readable string.
// Example:
// "  A B C D E F G H I \n"
// "0 r h e a g a e h r \n"
// "1 . . . . . . . . . \n"
// "2 . c . . . . . c . \n"
// "3 s . s . s . s . s \n"
// "4 - - - - - - - - - \n"
// "5 - - - - - - - - - \n"
// "7 S . S . S . S . S \n"
// "7 . C . . . . . C . \n"
// "8 . . . . . . . . . \n"
// "9 R H E A G A E H R \n";
Board<Piece> BoardFromString(std::string_view str);

// Convert board to human-readable string.
std::string BoardToString(const Board<Piece>& board);

// Check if two boards are identical.
bool BoardEq(const Board<Piece>& a, const Board<Piece>& b);

bool operator==(const Board<Piece>& lhs, const Board<Piece>& rhs);

// Returns the position of a player's general. If the player's general was
// captured, return kNoPosition.
Position FindGeneral(const Board<Piece>& board, Player player);

// Returns true if it's checkmade for the given player. I.e., if the
// current player does not take certain actions, the opponent can capture
// the general with their next move.
bool IsBeingCheckmate(const Board<Piece>& board, Player player);

// Returns the winner if one of the player's general is captured, returns NONE
// if both generals are on the board.
// Note that this function does not perform a future-looking search to check
// if there is no possible move for one player to not be checkmate. To perform
// future-looking winning evaluation, call the DidPlayerLose function.
Winner GetWinner(const Board<Piece>& board);

// Returns true if all possible moves of the given player still result in the
// player being checkmate.
bool DidPlayerLose(const Board<Piece>& board, Player player);

// Rotate the board 180 degrees so that it's from the opponent's perspective.
// Red and black pieces are also flipped.
Board<Piece> FlipBoard(const Board<Piece>& board);

// Move a piece from a position to another position, returns the captured
// piece. If no piece was captured, return EMPTY.
Piece Move(Board<Piece>& board, Movement movement);

// Returns all possible moves for the player with piece at position. Impossible
// moves are filled with kNoPosition.
std::array<Position, 17> PossibleMoves(const Board<Piece>& board, Position pos);

// Returns a vector of all possible moves for player. Each move is a 16-bit
// integer, representing "from" and "to" positions, each being 8 bits
// (Position).
std::vector<Movement> AllPossibleNextMoves(const Board<Piece>& board,
                                           Player player);

// Returns a vector of all possible boards for the given player after any valid
// move.
std::vector<Board<Piece>> AllPossibleNextBoards(const Board<Piece>& board,
                                                Player player);

// Encode the board state using a small number of bytes, mainly used for the
// game AI to identify a unique board state.
//
// There are a total of 32 pieces on the board, so the encoded board state is
// 32 bytes, with each byte representing the row and column of that piece
// (4 bits for each). If the piece is not present, use 0xF to represent it.
// To make sure the same piece for the same player is treated in the same way,
// all byte representations of a group of piece is sorted.
BoardState EncodeBoardState(const Board<Piece>& board);

// Decode the encoded board state back to its original state.
Board<Piece> DecodeBoardState(const BoardState& state);

}  // namespace xq

#endif  // XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_BOARD_H_
