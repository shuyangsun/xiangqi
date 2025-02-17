#ifndef XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_BOARD_C_H_
#define XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_BOARD_C_H_

#include "xiangqi/types_c.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef Position MovesPerPieceC[K_MAX_MOVE_PER_PIECE];

typedef Movement MaxMovesPerPlayerC[K_MAX_MOVE_PER_PLAYER];

const BoardC K_STARTING_BOARD = {
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

// Remove all pieces from board.
void ClearBoard_C(BoardC board);

// Reset board to starting point.
void ResetBoard_C(BoardC board);

// Copies destination board from source board.
void CopyBoard_C(BoardC dest, const BoardC src);

// Copies destination board from source board.
void CopyBoard_C(BoardC dest, const BoardC src);

// Returns the position of a player's general. If the player's general was
// captured, return kNoPosition.
Position FindGeneral_C(const BoardC board, enum Player player);

// Returns true if it's checkmade for the given player. I.e., if the
// current player does not take certain actions, the opponent can capture
// the general with their next move.
bool IsBeingCheckmate_C(const BoardC board, enum Player player);

// Returns the winner if one of the player's general is captured, returns NONE
// if both generals are on the board.
// Note that this function does not perform a future-looking search to check
// if there is no possible move for one player to not be checkmate. To perform
// future-looking winning evaluation, call the DidPlayerLose function.
enum Winner GetWinner_C(const BoardC board);

// Move a piece from a position to another position, returns the captured
// piece. If no piece was captured, return EMPTY.
enum Piece Move_C(BoardC board, Movement movement);

// Rotate the board 180 degrees so that it's from the opponent's perspective.
// Red and black pieces are also flipped.
void FlipBoard_C(BoardC dest, const BoardC src);

// Flip the board left and right.
void MirrorBoardHorizontal_C(BoardC dest, const BoardC src);

// Flip the board up and down.
void MirrorBoardVertical_C(BoardC dest, const BoardC src);

// Encode the board state using a small number of bytes, mainly used for the
// game AI to identify a unique board state.
//
// There are a total of 32 pieces on the board, so the encoded board state is
// 32 bytes, with each byte representing the row and column of that piece
// (4 bits for each). If the piece is not present, use 0xF to represent it.
// To make sure the same piece for the same player is treated in the same way,
// all byte representations of a group of piece is sorted.
void EncodeBoardState_C(const BoardC board, BoardStateC out);

// Decode the encoded board state back to its original state.
void DecodeBoardState_C(const BoardStateC state, BoardC out);

// Get all possible moves for the player with piece at position. Impossible
// moves are filled with kNoPosition. Returns number of possible moves.
// If avoid_checkmate is set to true, moves that result in being checkmade
// will not be included.
uint8_t PossiblePositions_C(const BoardC board, Position pos,
                            bool avoid_checkmate, MovesPerPieceC out);

// Get all possible moves for player. Each move is a 16-bit unsigned integer,
// representing "from" and "to" positions, each being 8 bits (Position).
uint8_t PossibleMoves_C(const BoardC board, enum Player player,
                        bool avoid_checkmate, MaxMovesPerPlayerC out);

// Returns true if all possible moves of the given player still result in the
// player being checkmate.
bool DidPlayerLose_C(const BoardC board, enum Player player);

#ifdef __cplusplus
}
#endif

#endif  // XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_BOARD_C_H_
