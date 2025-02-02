#include <iostream>

#include "xiangqi/game.h"
#include "xiangqi/types.h"

namespace {

using ::xiangqi::Board;
using ::xiangqi::kTotalCol;
using ::xiangqi::kTotalRow;
using ::xiangqi::Piece;

// Converts a Piece value to a one-character representation.
// Red pieces are uppercase, Black pieces are lowercase, and empty squares are
// printed as '.'
char PieceToChar(Piece piece) {
  switch (piece) {
    case Piece::EMPTY:
      return '.';

    // General
    case Piece::R_GENERAL:
      return 'G';
    case Piece::B_GENERAL:
      return 'g';

    // Advisors
    case Piece::R_ADVISOR_L:
    case Piece::R_ADVISOR_R:
      return 'A';
    case Piece::B_ADVISOR_L:
    case Piece::B_ADVISOR_R:
      return 'a';

    // Elephants
    case Piece::R_ELEPHANT_L:
    case Piece::R_ELEPHANT_R:
      return 'E';
    case Piece::B_ELEPHANT_L:
    case Piece::B_ELEPHANT_R:
      return 'e';

    // Horses
    case Piece::R_HORSE_L:
    case Piece::R_HORSE_R:
      return 'H';
    case Piece::B_HORSE_L:
    case Piece::B_HORSE_R:
      return 'h';

    // Chariots
    case Piece::R_CHARIOT_L:
    case Piece::R_CHARIOT_R:
      return 'R';
    case Piece::B_CHARIOT_L:
    case Piece::B_CHARIOT_R:
      return 'r';

    // Cannons
    case Piece::R_CANNON_L:
    case Piece::R_CANNON_R:
      return 'C';
    case Piece::B_CANNON_L:
    case Piece::B_CANNON_R:
      return 'c';

    // Soldiers
    case Piece::R_SOLDIER_1:
    case Piece::R_SOLDIER_2:
    case Piece::R_SOLDIER_3:
    case Piece::R_SOLDIER_4:
    case Piece::R_SOLDIER_5:
      return 'S';
    case Piece::B_SOLDIER_1:
    case Piece::B_SOLDIER_2:
    case Piece::B_SOLDIER_3:
    case Piece::B_SOLDIER_4:
    case Piece::B_SOLDIER_5:
      return 's';

    default:
      return '?';
  }
}

// Prints the board as ASCII art with row and column indices.
void PrintBoard(const Board<Piece>& board) {
  // Print the column header.
  std::cout << "   ";  // Offset for row indices
  for (uint8_t col = 0; col < kTotalCol; ++col) {
    std::cout << static_cast<int>(col) << " ";
  }
  std::cout << "\n";

  // Print each row with its index.
  for (uint8_t row = 0; row < kTotalRow; ++row) {
    std::cout << static_cast<int>(row) << "  ";  // Print row index
    for (uint8_t col = 0; col < kTotalCol; ++col) {
      std::cout << PieceToChar(board[row][col]) << " ";
    }
    std::cout << "\n";
  }
}

}  // namespace

int main(int argc, char** argv) {
  xiangqi::Game game;
  PrintBoard(game.CurrentBoard());
  return 0;
}
