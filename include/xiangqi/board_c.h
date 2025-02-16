#ifndef XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_BOARD_C_H_
#define XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_BOARD_C_H_

#include "xiangqi/types_c.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef Position MovesPerPieceC[17];

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

#ifdef __cplusplus
}
#endif

#endif  // XIANGQI_GAME_ENGINE_INCLUDE_XIANGQI_BOARD_C_H_
