#include <gtest/gtest.h>

#include <string>
#include <string_view>

#include "xiangqi/board.h"
#include "xiangqi/types.h"

namespace {

namespace {

using namespace ::xq;

constexpr std::string_view kStartingBoardStr =
    "  A B C D E F G H I \n"
    "0 r h e a g a e h r \n"
    "1 . . . * * * . . . \n"
    "2 . c . * * * . c . \n"
    "3 s . s . s . s . s \n"
    "4 - - - - - - - - - \n"
    "5 - - - - - - - - - \n"
    "6 S . S . S . S . S \n"
    "7 . C . * * * . C . \n"
    "8 . . . * * * . . . \n"
    "9 R H E A G A E H R \n";

}  // namespace

TEST(Board, PosStr) {
  EXPECT_EQ(PosStr("a0"), Pos(0, 0));
  EXPECT_EQ(PosStr("A2"), Pos(2, 0));
  EXPECT_EQ(PosStr("I5"), Pos(5, 8));
  EXPECT_EQ(PosStr("F9"), Pos(9, 5));
}

TEST(Board, BoardEqual) {
  const Board a = kStartingBoard;
  const Board b = kStartingBoard;
  EXPECT_EQ(a, b);
}

TEST(Board, BoardNotEqual) {
  const Board a = kStartingBoard;
  Board b = kStartingBoard;
  b[Pos(0, 0)] = PIECE_EMPTY;
  EXPECT_NE(a, b);
}

TEST(Board, FromString) {
  const Board board = BoardFromString(kStartingBoardStr);
  EXPECT_EQ(board, kStartingBoard) << "Expected boards to equal:\n"
                                   << BoardToString(board) << "\n"
                                   << BoardToString(kStartingBoard);
}

TEST(Board, ToString) {
  const std::string str = BoardToString(kStartingBoard);
  EXPECT_EQ(str, kStartingBoardStr) << "Expected board strings to equal:\n"
                                    << str << "\n"
                                    << kStartingBoardStr;
}

// ---------------------------------------------------------------------
// Test FlipPosition and FlipBoard
// ---------------------------------------------------------------------

TEST(Board, FlipPosition) {
  EXPECT_EQ(FlipPosition(PosStr("A0")), PosStr("I9"));
  EXPECT_EQ(FlipPosition(PosStr("B2")), PosStr("H7"));
  EXPECT_EQ(FlipPosition(PosStr("H2")), PosStr("B7"));
  EXPECT_EQ(FlipPosition(PosStr("A3")), PosStr("I6"));
  EXPECT_EQ(FlipPosition(PosStr("E0")), PosStr("E9"));
  EXPECT_EQ(FlipPosition(PosStr("G4")), PosStr("C5"));
}

TEST(Board, FlipBoard) {
  EXPECT_EQ(kStartingBoard, FlipBoard(kStartingBoard));

  const Board board_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 r h e a g a e h r \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . c . \n"
      "3 s . s . s . s . s \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 S c S . S . S . S \n"
      "7 . C . * * * . C . \n"
      "8 . . . * * * . . . \n"
      "9 R H E A G A E H R \n");
  const Board flipped_1 = FlipBoard(board_1);
  const Board expected_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 r h e a g a e h r \n"
      "1 . . . * * * . . . \n"
      "2 . c . * * * . c . \n"
      "3 s . s . s . s C s \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 S . S . S . S . S \n"
      "7 . C . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 R H E A G A E H R \n");
  EXPECT_NE(board_1, flipped_1);
  EXPECT_EQ(flipped_1, expected_1);
}

// ---------------------------------------------------------------------
// Test MirrorPositionHorizontal and MirrorBoardHorizontal
// ---------------------------------------------------------------------

TEST(Board, MirrorPositionHorizontal) {
  EXPECT_EQ(MirrorPositionHorizontal(PosStr("C0")), PosStr("G0"));
  EXPECT_EQ(MirrorPositionHorizontal(PosStr("E2")), PosStr("E2"));
  EXPECT_EQ(MirrorPositionHorizontal(PosStr("A9")), PosStr("I9"));
  EXPECT_EQ(MirrorPositionHorizontal(PosStr("D5")), PosStr("F5"));
}

TEST(Board, MirrorBoardHorizontal) {
  EXPECT_EQ(kStartingBoard, MirrorBoardHorizontal(kStartingBoard))
      << "Expected board\n"
      << BoardToString(MirrorBoardHorizontal(kStartingBoard))
      << "to be the same as starting board.";

  const Board board_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 r h e a g a e h r \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . c . \n"
      "3 s . s . s . s . s \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 S c S . S . S . S \n"
      "7 . C . * * * . C . \n"
      "8 . . . * * * . . . \n"
      "9 R H E A G A E H R \n");
  const Board mirrored_1 = MirrorBoardHorizontal(board_1);
  const Board expected_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 r h e a g a e h r \n"
      "1 . . . * * * . . . \n"
      "2 . c . * * * . . . \n"
      "3 s . s . s . s . s \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 S . S . S . S c S \n"
      "7 . C . * * * . C . \n"
      "8 . . . * * * . . . \n"
      "9 R H E A G A E H R \n");
  EXPECT_NE(board_1, mirrored_1);
  EXPECT_EQ(mirrored_1, expected_1);
}

// ---------------------------------------------------------------------
// Test MirrorPositionVertical and MirrorBoardVertical
// ---------------------------------------------------------------------

TEST(Board, MirrorPositionVertical) {
  EXPECT_EQ(MirrorPositionVertical(PosStr("C0")), PosStr("C9"));
  EXPECT_EQ(MirrorPositionVertical(PosStr("E2")), PosStr("E7"));
  EXPECT_EQ(MirrorPositionVertical(PosStr("A9")), PosStr("A0"));
  EXPECT_EQ(MirrorPositionVertical(PosStr("D5")), PosStr("D4"));
}

TEST(Board, MirrorBoardVertical) {
  EXPECT_EQ(kStartingBoard, MirrorBoardVertical(kStartingBoard));

  const Board board_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 r h e a g a e h r \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . c . \n"
      "3 s . s . s . s . s \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 S c S . S . S . S \n"
      "7 . C . * * * . C . \n"
      "8 . . . * * * . . . \n"
      "9 R H E A G A E H R \n");
  const Board mirrored_1 = MirrorBoardVertical(board_1);
  const Board expected_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 r h e a g a e h r \n"
      "1 . . . * * * . . . \n"
      "2 . c . * * * . c . \n"
      "3 s C s . s . s . s \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 S . S . S . S . S \n"
      "7 . . . * * * . C . \n"
      "8 . . . * * * . . . \n"
      "9 R H E A G A E H R \n");
  EXPECT_NE(board_1, mirrored_1);
  EXPECT_EQ(mirrored_1, expected_1);
}

// ---------------------------------------------------------------------
// Test Move
// ---------------------------------------------------------------------

TEST(Board, Move) {
  Board board = kStartingBoard;

  Piece capture = Move(board, NewMovement(PosStr("B7"), PosStr("B0")));
  Board expected_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 r C e a g a e h r \n"
      "1 . . . * * * . . . \n"
      "2 . c . * * * . c . \n"
      "3 s . s . s . s . s \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 S . S . S . S . S \n"
      "7 . . . * * * . C . \n"
      "8 . . . * * * . . . \n"
      "9 R H E A G A E H R \n");
  EXPECT_EQ(board, expected_1);
  EXPECT_EQ(capture, Piece::B_HORSE);

  capture = Move(board, NewMovement(PosStr("E0"), PosStr("E1")));
  Board expected_2 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 r C e a . a e h r \n"
      "1 . . . * g * . . . \n"
      "2 . c . * * * . c . \n"
      "3 s . s . s . s . s \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 S . S . S . S . S \n"
      "7 . . . * * * . C . \n"
      "8 . . . * * * . . . \n"
      "9 R H E A G A E H R \n");
  EXPECT_EQ(board, expected_2);
  EXPECT_EQ(capture, PIECE_EMPTY);
}

// Make sure even invalid moves can be performed.
//
// The Move function should not check if the movement is valid, as that is
// a much more expensive calculation. Possible moves should be explicitly
// calculated beforehand and be checked against before calling Move.
TEST(Board, CanDoInvalidMove) {
  Board board = kStartingBoard;

  // Move cannon to capture general at the beginning.
  Piece capture = Move(board, NewMovement(PosStr("B7"), PosStr("E0")));
  Board expected_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 r h e a C a e h r \n"
      "1 . . . * * * . . . \n"
      "2 . c . * * * . c . \n"
      "3 s . s . s . s . s \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 S . S . S . S . S \n"
      "7 . . . * * * . C . \n"
      "8 . . . * * * . . . \n"
      "9 R H E A G A E H R \n");
  EXPECT_EQ(board, expected_1);
  EXPECT_EQ(capture, Piece::B_GENERAL);

  // Move red again
  capture = Move(board, NewMovement(PosStr("G6"), PosStr("I0")));
  Board expected_2 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 r h e a C a e h S \n"
      "1 . . . * * * . . . \n"
      "2 . c . * * * . c . \n"
      "3 s . s . s . s . s \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 S . S . S . . . S \n"
      "7 . . . * * * . C . \n"
      "8 . . . * * * . . . \n"
      "9 R H E A G A E H R \n");
  EXPECT_EQ(board, expected_2);
  EXPECT_EQ(capture, Piece::B_CHARIOT);

  // Move black same position
  capture = Move(board, NewMovement(PosStr("H0"), PosStr("H0")));
  EXPECT_EQ(board, expected_2);  // nothing should change
  EXPECT_EQ(capture, PIECE_EMPTY);
}

// ---------------------------------------------------------------------
// Test FindGeneral
// ---------------------------------------------------------------------

TEST(Board, FindGeneralOwnPalace) {
  const Board board_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . g * a . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . G * A . . . \n");
  EXPECT_EQ(FindGeneral(board_1, PLAYER_RED), PosStr("D9"));
  EXPECT_EQ(FindGeneral(board_1, PLAYER_BLACK), PosStr("D0"));

  const Board board_2 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . a g a . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . A G A . . . \n");
  EXPECT_EQ(FindGeneral(board_2, PLAYER_RED), PosStr("E9"));
  EXPECT_EQ(FindGeneral(board_2, PLAYER_BLACK), PosStr("E0"));

  const Board board_3 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . a * g . . . \n"
      "1 . . . * H * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * * G . . . \n");
  EXPECT_EQ(FindGeneral(board_3, PLAYER_RED), PosStr("F9"));
  EXPECT_EQ(FindGeneral(board_3, PLAYER_BLACK), PosStr("F0"));

  const Board board_4 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . g * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . G * * . . . \n"
      "9 . . . * * * . . . \n");
  EXPECT_EQ(FindGeneral(board_4, PLAYER_RED), PosStr("D8"));
  EXPECT_EQ(FindGeneral(board_4, PLAYER_BLACK), PosStr("D1"));

  const Board board_5 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * g * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * G * . . . \n"
      "9 . . . * * * . . . \n");
  EXPECT_EQ(FindGeneral(board_5, PLAYER_RED), PosStr("E8"));
  EXPECT_EQ(FindGeneral(board_5, PLAYER_BLACK), PosStr("E1"));

  const Board board_6 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * * g . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * G . . . \n"
      "9 . . . * * * . . . \n");
  EXPECT_EQ(FindGeneral(board_6, PLAYER_RED), PosStr("F8"));
  EXPECT_EQ(FindGeneral(board_6, PLAYER_BLACK), PosStr("F1"));

  const Board board_7 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . g * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . G * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * * * . . . \n");
  EXPECT_EQ(FindGeneral(board_7, PLAYER_RED), PosStr("D7"));
  EXPECT_EQ(FindGeneral(board_7, PLAYER_BLACK), PosStr("D2"));

  const Board board_8 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * g * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * G * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * * * . . . \n");
  EXPECT_EQ(FindGeneral(board_8, PLAYER_RED), PosStr("E7"));
  EXPECT_EQ(FindGeneral(board_8, PLAYER_BLACK), PosStr("E2"));

  const Board board_9 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * g . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * G . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * * * . . . \n");
  EXPECT_EQ(FindGeneral(board_9, PLAYER_RED), PosStr("F7"));
  EXPECT_EQ(FindGeneral(board_9, PLAYER_BLACK), PosStr("F2"));
}

TEST(Board, FindGeneralOpponentPalace) {
  const Board board_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . G * A . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . g * A . . . \n");
  EXPECT_EQ(FindGeneral(board_1, PLAYER_RED), PosStr("D0"));
  EXPECT_EQ(FindGeneral(board_1, PLAYER_BLACK), PosStr("D9"));

  const Board board_2 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . a G a . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . A g A . . . \n");
  EXPECT_EQ(FindGeneral(board_2, PLAYER_RED), PosStr("E0"));
  EXPECT_EQ(FindGeneral(board_2, PLAYER_BLACK), PosStr("E9"));

  const Board board_3 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . a * G . . . \n"
      "1 . . . * H * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * * g . . . \n");
  EXPECT_EQ(FindGeneral(board_3, PLAYER_RED), PosStr("F0"));
  EXPECT_EQ(FindGeneral(board_3, PLAYER_BLACK), PosStr("F9"));

  const Board board_4 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . G * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . g * * . . . \n"
      "9 . . . * * * . . . \n");
  EXPECT_EQ(FindGeneral(board_4, PLAYER_RED), PosStr("D1"));
  EXPECT_EQ(FindGeneral(board_4, PLAYER_BLACK), PosStr("D8"));

  const Board board_5 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * G * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * g * . . . \n"
      "9 . . . * * * . . . \n");
  EXPECT_EQ(FindGeneral(board_5, PLAYER_RED), PosStr("E1"));
  EXPECT_EQ(FindGeneral(board_5, PLAYER_BLACK), PosStr("E8"));

  const Board board_6 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * * G . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * g . . . \n"
      "9 . . . * * * . . . \n");
  EXPECT_EQ(FindGeneral(board_6, PLAYER_RED), PosStr("F1"));
  EXPECT_EQ(FindGeneral(board_6, PLAYER_BLACK), PosStr("F8"));

  const Board board_7 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . G * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . g * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * * * . . . \n");
  EXPECT_EQ(FindGeneral(board_7, PLAYER_RED), PosStr("D2"));
  EXPECT_EQ(FindGeneral(board_7, PLAYER_BLACK), PosStr("D7"));

  const Board board_8 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * G * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * g * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * * * . . . \n");
  EXPECT_EQ(FindGeneral(board_8, PLAYER_RED), PosStr("E2"));
  EXPECT_EQ(FindGeneral(board_8, PLAYER_BLACK), PosStr("E7"));

  const Board board_9 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * G . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * g . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * * * . . . \n");
  EXPECT_EQ(FindGeneral(board_9, PLAYER_RED), PosStr("F2"));
  EXPECT_EQ(FindGeneral(board_9, PLAYER_BLACK), PosStr("F7"));
}

TEST(Board, FindGeneralNone) {
  const Board board_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . R * a . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . G * A . . . \n");
  EXPECT_EQ(FindGeneral(board_1, PLAYER_BLACK), K_NO_POSITION);

  const Board board_2 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . G * a . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * * A . . . \n");
  EXPECT_EQ(FindGeneral(board_2, PLAYER_BLACK), K_NO_POSITION);

  const Board board_3 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . g * a . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . r * A . . . \n");
  EXPECT_EQ(FindGeneral(board_3, PLAYER_RED), K_NO_POSITION);

  const Board board_4 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * a . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . g * A . . . \n");
  EXPECT_EQ(FindGeneral(board_4, PLAYER_RED), K_NO_POSITION);
}

// ---------------------------------------------------------------------
// Test IsBeingCheckmate
// ---------------------------------------------------------------------

TEST(Board, IsBeingCheckmateFlyingGeneral) {
  const Board board_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . g * a . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . G * A . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_1, PLAYER_RED));
  EXPECT_TRUE(IsBeingCheckmate(board_1, PLAYER_BLACK));

  const Board board_2 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . g * a . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . h . . . . . \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . G * A . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_2, PLAYER_RED));
  EXPECT_FALSE(IsBeingCheckmate(board_2, PLAYER_BLACK));
}

TEST(Board, IsBeingCheckmateChariot) {
  const Board board_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . g * a . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . R . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * G A . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_1, PLAYER_RED));
  EXPECT_TRUE(IsBeingCheckmate(board_1, PLAYER_BLACK));

  const Board board_2 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . g * a . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * R * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * G A . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_2, PLAYER_RED));
  EXPECT_FALSE(IsBeingCheckmate(board_2, PLAYER_BLACK));

  const Board board_3 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . g * a . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . r . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * G A . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_3, PLAYER_RED));
  EXPECT_FALSE(IsBeingCheckmate(board_3, PLAYER_BLACK));

  const Board board_4 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . g * a . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 . . . r . . . . . \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * G A . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_4, PLAYER_RED));
  EXPECT_FALSE(IsBeingCheckmate(board_4, PLAYER_BLACK));

  const Board board_5 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . R . g * a . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * G A . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_5, PLAYER_BLACK));

  const Board board_6 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . g * a . R . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * G A . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_6, PLAYER_BLACK));

  const Board board_7 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . g * a . R . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . r G A . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_7, PLAYER_RED));

  const Board board_8 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . g * a . R . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * r * . . . \n"
      "9 . . . * G A . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_8, PLAYER_RED));

  const Board board_9 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . g r a . R . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * G A . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_9, PLAYER_RED));

  const Board board_10 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 R . . g * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . r G * . . . \n"
      "9 . . . * * * . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_10, PLAYER_RED));
  EXPECT_TRUE(IsBeingCheckmate(board_10, PLAYER_BLACK));

  const Board board_11 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . g R * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * G * . . r \n"
      "9 . . . * * * . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_11, PLAYER_RED));
  EXPECT_TRUE(IsBeingCheckmate(board_11, PLAYER_BLACK));

  const Board board_12 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . R r * . . . \n"
      "1 . . . g * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * G * . . . \n"
      "9 . . . * * * . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_12, PLAYER_RED));
  EXPECT_TRUE(IsBeingCheckmate(board_12, PLAYER_BLACK));

  const Board board_13 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . g * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - R - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * G * . . r \n"
      "9 . . . * r * . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_13, PLAYER_RED));
  EXPECT_TRUE(IsBeingCheckmate(board_13, PLAYER_BLACK));
}

TEST(Board, IsBeingCheckmateHorse) {
  // Down left
  const Board board_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * H * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . g * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * h . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * G * . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_1, PLAYER_RED));
  EXPECT_TRUE(IsBeingCheckmate(board_1, PLAYER_BLACK));

  // Down left blocked
  const Board board_2 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * H * . . . \n"
      "1 . . . * c * . . . \n"
      "2 . . . g * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * h . . . \n"
      "8 . . . * * C . . . \n"
      "9 . . . * G * . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_2, PLAYER_RED));
  EXPECT_FALSE(IsBeingCheckmate(board_2, PLAYER_BLACK));

  // Down right
  const Board board_3 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * H * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * g . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . h * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * G * . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_3, PLAYER_RED));
  EXPECT_TRUE(IsBeingCheckmate(board_3, PLAYER_BLACK));

  // Down right blocked
  const Board board_4 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * H * . . . \n"
      "1 . . . * c * . . . \n"
      "2 . . . * * g . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . h * * . . . \n"
      "8 . . . C * * . . . \n"
      "9 . . . * G * . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_4, PLAYER_RED));
  EXPECT_FALSE(IsBeingCheckmate(board_4, PLAYER_BLACK));

  // Up left
  const Board board_5 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . g * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - H - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * G * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * * h . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_5, PLAYER_RED));
  EXPECT_TRUE(IsBeingCheckmate(board_5, PLAYER_BLACK));

  // Up left blocked
  const Board board_6 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . g * * . . . \n"
      "3 . . . . r . . . . \n"
      "4 - - - - H - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * G * . . . \n"
      "8 . . . * * R . . . \n"
      "9 . . . * * h . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_6, PLAYER_RED));
  EXPECT_FALSE(IsBeingCheckmate(board_6, PLAYER_BLACK));

  // Up right
  const Board board_7 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * g . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - H - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * G * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . h * * . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_7, PLAYER_RED));
  EXPECT_TRUE(IsBeingCheckmate(board_7, PLAYER_BLACK));

  // Up right blocked
  const Board board_8 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * g . . . \n"
      "3 . . . . r . . . . \n"
      "4 - - - - H - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * G * . . . \n"
      "8 . . . R * * . . . \n"
      "9 . . . h * * . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_8, PLAYER_RED));
  EXPECT_FALSE(IsBeingCheckmate(board_8, PLAYER_BLACK));

  // Left up
  const Board board_9 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . g * * . . . \n"
      "3 . . . . . H . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * G * . . . \n"
      "9 . . . * * * h . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_9, PLAYER_RED));
  EXPECT_TRUE(IsBeingCheckmate(board_9, PLAYER_BLACK));

  // Left up blocked
  const Board board_10 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . g * * . . . \n"
      "3 . . . . h H . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * G * . . . \n"
      "9 . . . * * H h . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_10, PLAYER_RED));
  EXPECT_FALSE(IsBeingCheckmate(board_10, PLAYER_BLACK));

  // Left down
  const Board board_11 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * H . . . \n"
      "1 . . . g h * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * h . . \n"
      "8 . . . * G H . . . \n"
      "9 . . . * * * . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_11, PLAYER_RED));
  EXPECT_TRUE(IsBeingCheckmate(board_11, PLAYER_BLACK));

  // Left down blocked
  const Board board_12 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * h H . . . \n"
      "1 . . . g * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * H h . . \n"
      "8 . . . * G * . . . \n"
      "9 . . . * * * . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_12, PLAYER_RED));
  EXPECT_FALSE(IsBeingCheckmate(board_12, PLAYER_BLACK));

  // Right up
  const Board board_13 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . h g * * . . . \n"
      "2 . H . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . H G * . . . \n"
      "9 . . h * * * . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_13, PLAYER_RED));
  EXPECT_TRUE(IsBeingCheckmate(board_13, PLAYER_BLACK));

  // Right up blocked
  const Board board_14 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . g * * . . . \n"
      "2 . H h * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * G * . . . \n"
      "9 . . h H * * . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_14, PLAYER_RED));
  EXPECT_FALSE(IsBeingCheckmate(board_14, PLAYER_BLACK));

  // Right down
  const Board board_15 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . H . * * * . . . \n"
      "1 . . h g * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . h * * * . . . \n"
      "8 . . . H G * . . . \n"
      "9 . . . * * * . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_15, PLAYER_RED));
  EXPECT_TRUE(IsBeingCheckmate(board_15, PLAYER_BLACK));

  // Right down blocked
  const Board board_16 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . H h * * * . . . \n"
      "1 . . . g * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . h H * * . . . \n"
      "8 . . . * G * . . . \n"
      "9 . . . * * * . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_16, PLAYER_RED));
  EXPECT_FALSE(IsBeingCheckmate(board_16, PLAYER_BLACK));

  // Too far
  const Board board_17 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 H . . * * * . . H \n"
      "1 . . . g * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * G * . . . \n"
      "9 h . . * * * . . h \n");
  EXPECT_FALSE(IsBeingCheckmate(board_17, PLAYER_RED));
  EXPECT_FALSE(IsBeingCheckmate(board_17, PLAYER_BLACK));
}

TEST(Board, IsBeingCheckmateCannon) {
  // Right
  const Board board_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . g * a . C . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * G c h . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_1, PLAYER_RED));
  EXPECT_TRUE(IsBeingCheckmate(board_1, PLAYER_BLACK));

  // Left
  const Board board_2 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . h C g * a . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 c h . * G * . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_2, PLAYER_RED));
  EXPECT_FALSE(IsBeingCheckmate(board_2, PLAYER_BLACK));

  // Top
  const Board board_3 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . C * a . . . \n"
      "1 . . . h * * . . . \n"
      "2 . . . g c * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * G * . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_3, PLAYER_RED));
  EXPECT_TRUE(IsBeingCheckmate(board_3, PLAYER_BLACK));

  // Bottom
  const Board board_4 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . g * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - h - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * G * . . . \n"
      "8 . . . * h * . . . \n"
      "9 . . . C c * . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_4, PLAYER_RED));
  EXPECT_TRUE(IsBeingCheckmate(board_4, PLAYER_BLACK));

  // Not aligned
  const Board board_5 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . C h * * * . . . \n"
      "2 . h . g * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - H c - \n"
      "6 . . . . . . . H . \n"
      "7 . . . * G * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * * * . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_5, PLAYER_RED));
  EXPECT_FALSE(IsBeingCheckmate(board_5, PLAYER_BLACK));
}

TEST(Board, IsBeingCheckmateSoldier) {
  const Board board_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . g * * . . . \n"
      "3 . . . S . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * G * . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_1, PLAYER_BLACK));

  const Board board_2 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * g S . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . G * * . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_2, PLAYER_BLACK));

  const Board board_3 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * S g . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * G * . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_3, PLAYER_BLACK));

  const Board board_4 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * S . . . \n"
      "1 . . . * * g . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * G * . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_4, PLAYER_BLACK));

  const Board board_5 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * g * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . s . . . . . \n"
      "7 . . . G * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * * * . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_5, PLAYER_RED));

  const Board board_6 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * g * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . G s * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * * * . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_6, PLAYER_RED));

  const Board board_7 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * g * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * s G . . . \n"
      "9 . . . * * * . . . \n");
  EXPECT_TRUE(IsBeingCheckmate(board_7, PLAYER_RED));

  const Board board_8 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * g * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * G . . . \n"
      "9 . . . * * s . . . \n");
  EXPECT_FALSE(IsBeingCheckmate(board_8, PLAYER_RED));
}

// ---------------------------------------------------------------------
// Test GetWinner
// ---------------------------------------------------------------------

TEST(Board, GetWinner) {
  const Board board_1 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * g * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * s . . . \n"
      "8 . . . * * G . . . \n"
      "9 . . . * * * . . . \n");
  EXPECT_EQ(GetWinner(board_1), WINNER_NONE);

  const Board board_2 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * R * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * G * . . . \n");
  EXPECT_EQ(GetWinner(board_2), WINNER_RED);

  const Board board_3 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * * * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * g * . . . \n");
  EXPECT_EQ(GetWinner(board_3), WINNER_BLACK);

  const Board board_4 = BoardFromString(
      "  A B C D E F G H I \n"
      "0 . . . * * * . . . \n"
      "1 . . . * G * . . . \n"
      "2 . . . * * * . . . \n"
      "3 . . . . . . . . . \n"
      "4 - - - - - - - - - \n"
      "5 - - - - - - - - - \n"
      "6 . . . . . . . . . \n"
      "7 . . . * * * . . . \n"
      "8 . . . * * * . . . \n"
      "9 . . . * * * . . . \n");
  EXPECT_EQ(GetWinner(board_4), WINNER_RED);
}

// ---------------------------------------------------------------------
// Test EncodeBoardState and DecodeBoardState
// ---------------------------------------------------------------------

TEST(Board, EncodeBoardState) {
  EXPECT_EQ(DecodeBoardState(EncodeBoardState(kStartingBoard)), kStartingBoard);
}

}  // namespace
