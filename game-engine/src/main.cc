#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

#include "xiangqi/game.h"
#include "xiangqi/types.h"

namespace {

using ::xq::Board;
using ::xq::Game;
using ::xq::kTotalCol;
using ::xq::kTotalRow;
using ::xq::Piece;
using ::xq::Player;
using ::xq::Pos;
using ::xq::Position;
using ::xq::PossibleMoves;
using ::xq::Winner;

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
    case Piece::R_ADVISOR:
      return 'A';
    case Piece::B_ADVISOR:
      return 'a';

    // Elephants
    case Piece::R_ELEPHANT:
      return 'E';
    case Piece::B_ELEPHANT:
      return 'e';

    // Horses
    case Piece::R_HORSE:
      return 'H';
    case Piece::B_HORSE:
      return 'h';

    // Chariots
    case Piece::R_CHARIOT:
      return 'R';
    case Piece::B_CHARIOT:
      return 'r';

    // Cannons
    case Piece::R_CANNON:
      return 'C';
    case Piece::B_CANNON:
      return 'c';

    // Soldiers
    case Piece::R_SOLDIER:
      return 'S';
    case Piece::B_SOLDIER:
      return 's';

    default:
      return '?';
  }
}

// Prints a 10x9 board with row and column indices.
// The column header is printed without spaces between the numbers.
void PrintGame(const Game& game) {
  // Print the column header without spaces between numbers.
  std::cout << "  ";
  for (uint8_t col = 0; col < kTotalCol; ++col) {
    std::cout << static_cast<int>(col) << ' ';
  }
  std::cout << "\n";

  const Board<Piece> board = game.CurrentBoard();
  for (uint8_t row = 0; row < kTotalRow; ++row) {
    std::cout << static_cast<int>(row) << ' ';
    for (uint8_t col = 0; col < kTotalCol; ++col) {
      std::cout << PieceToChar(board[row][col]) << ' ';
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
  std::cout << "Turn: " << (game.Turn() == Player::RED ? "Red" : "Black")
            << std::endl;
  std::cout << "Check made: " << (game.IsCheckMade() ? "true" : "false")
            << std::endl;
  const std::optional<Winner> winner = game.GetWinner();
  std::cout << "Winner: "
            << (winner.has_value() ? (*winner == Winner::RED ? "Red" : "Black")
                                   : "None")
            << std::endl;
}

}  // namespace

// Main function: displays the board and then waits for user input.
// For each entered coordinate (two digits representing row and col),
// it prints the board with possible moves (as returned by Game::PossibleMoves)
// marked by 'o'.
int main() {
  // Create a game instance with the default Xiangqi opening board.
  Game game;
  PrintGame(game);

  while (true) {
    // Ask the user to enter the coordinates of a piece.
    std::cout << "\n1. Enter the row and column of a piece (e.g., '34') to "
                 "view possible moves.\n"
                 "2. Enter two positions separated by comma (e.g, '34,54') to "
                 "move.\n"
                 "3. Enter or 'q' to quit.\n: ";
    std::string input_line;
    if (!std::getline(std::cin, input_line)) break;
    if (input_line == "q" || input_line == "Q") break;

    // Expect input in the format of two digits (with no space).
    const size_t input_size = input_line.size();
    const bool input_size_valid =
        input_size == 1 || input_size == 2 || input_size == 5;
    if (!input_size_valid) {
      std::cout << "Invalid input.\n";
      continue;
    }

    // Get first two characters.
    char row_char = input_line[0];
    char col_char = input_line[1];
    if (!std::isdigit(row_char) || !std::isdigit(col_char)) {
      std::cout << "Invalid input. Please enter two digits.\n";
      continue;
    }
    int row = row_char - '0';
    int col = col_char - '0';

    // Validate that the input coordinates are within board limits.
    if (row < 0 || row >= kTotalRow || col < 0 || col >= kTotalCol) {
      std::cout << "Coordinates out of range. Please try again.\n";
      continue;
    }

    if (input_size == 2) {
      Position pos = Pos(row, col);

      // Get the possible moves for the piece at the given position.
      Board<bool> moves = PossibleMoves(game.CurrentBoard(), pos);

      // Print the board overlaying possible moves with 'o'.
      std::cout << "\nPossible moves from (" << row << "," << col << "):\n";
      // Print the column header without spaces.
      std::cout << "  ";
      for (uint8_t col = 0; col < kTotalCol; ++col) {
        std::cout << static_cast<int>(col) << ' ';
      }
      std::cout << '\n';

      const Board<Piece> board = game.CurrentBoard();

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
    } else {
      // Get last two characters.
      char row_char_2 = input_line[3];
      char col_char_2 = input_line[4];
      if (!std::isdigit(row_char_2) || !std::isdigit(col_char_2)) {
        std::cout << "Invalid input. Please enter two digits.\n";
        continue;
      }
      int row_2 = row_char_2 - '0';
      int col_2 = col_char_2 - '0';

      // Validate that the input coordinates are within board limits.
      if (row_2 < 0 || row_2 >= kTotalRow || col_2 < 0 || col_2 >= kTotalCol) {
        std::cout << "Coordinates out of range. Please try again.\n";
        continue;
      }

      game.Move(Pos(row, col), Pos(row_2, col_2));

      PrintGame(game);
    }
  }

  std::cout << "Exiting game.\n";
  return 0;
}
