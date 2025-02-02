#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

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

// Prints a 10x9 board with row and column indices.
// The column header is printed without spaces between the numbers.
void PrintBoard(const Board<Piece>& board) {
  // Print the column header without spaces between numbers.
  std::cout << "  ";
  for (uint8_t col = 0; col < kTotalCol; ++col) {
    std::cout << static_cast<int>(col) << ' ';
  }
  std::cout << "\n";

  for (uint8_t row = 0; row < kTotalRow; ++row) {
    std::cout << static_cast<int>(row) << ' ';
    for (uint8_t col = 0; col < kTotalCol; ++col) {
      std::cout << PieceToChar(board[row][col]) << ' ';
    }
    std::cout << '\n';
  }
}

}  // namespace

// Main function: displays the board and then waits for user input.
// For each entered coordinate (two digits representing row and col),
// it prints the board with possible moves (as returned by Game::PossibleMoves)
// marked by 'o'.
int main() {
  using namespace xiangqi;

  // Create a game instance with the default Xiangqi opening board.
  Game game;

  while (true) {
    // Retrieve and display the current board.
    Board<Piece> board = game.CurrentBoard();
    PrintBoard(board);

    // Ask the user to enter the coordinates of a piece.
    std::cout << "\nEnter the row and column of a piece (e.g., '34') or 'q' to "
                 "quit: ";
    std::string inputLine;
    if (!std::getline(std::cin, inputLine)) break;
    if (inputLine == "q" || inputLine == "Q") break;

    // Expect input in the format of two digits (with no space).
    if (inputLine.size() < 2) {
      std::cout << "Invalid input. Please enter two digits.\n";
      continue;
    }
    char rowChar = inputLine[0];
    char colChar = inputLine[1];
    if (!std::isdigit(rowChar) || !std::isdigit(colChar)) {
      std::cout << "Invalid input. Please enter two digits.\n";
      continue;
    }
    int rowInput = rowChar - '0';
    int colInput = colChar - '0';

    // Validate that the input coordinates are within board limits.
    if (rowInput < 0 || rowInput >= kTotalRow || colInput < 0 ||
        colInput >= kTotalCol) {
      std::cout << "Coordinates out of range. Please try again.\n";
      continue;
    }

    Position pos{static_cast<uint8_t>(rowInput),
                 static_cast<uint8_t>(colInput)};

    // Get the possible moves for the piece at the given position.
    Board<bool> moves = game.PossibleMoves(pos);

    // Print the board overlaying possible moves with 'o'.
    std::cout << "\nPossible moves from (" << rowInput << "," << colInput
              << "):\n";
    // Print the column header without spaces.
    std::cout << "   ";
    for (uint8_t col = 0; col < kTotalCol; ++col) {
      std::cout << static_cast<int>(col);
    }
    std::cout << "\n";

    for (uint8_t row = 0; row < kTotalRow; ++row) {
      std::cout << static_cast<int>(row) << " ";
      for (uint8_t col = 0; col < kTotalCol; ++col) {
        // If the move is possible, mark with 'o'; otherwise show the piece.
        if (moves[row][col]) {
          std::cout << "o ";
        } else {
          std::cout << PieceToChar(board[row][col]) << " ";
        }
      }
      std::cout << "\n";
    }
    std::cout << "\n";
  }

  std::cout << "Exiting game.\n";
  return 0;
}
