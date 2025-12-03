#include "tictactoe.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

int main() {
    std::cout << "==============================================\n";
    std::cout << "  Functional Tic-Tac-Toe Demo\n";
    std::cout << "==============================================\n\n";

    // ========================================================================
    // Demo 1: Immutability - making moves creates NEW boards
    // ========================================================================
    std::cout << "DEMO 1: Immutability\n";
    std::cout << "--------------------\n\n";

    Board empty = emptyBoard();
    std::cout << "Empty board:\n" << boardToString(empty) << "\n";

    // Make a move - returns NEW board, doesn't modify empty!
    // Using Position type for all board positions
    std::optional<Board> afterX = makeMove(empty, Position{0, 0}, Cell::X);
    if (afterX) {
        std::cout << "After X plays (0,0):\n" << boardToString(*afterX) << "\n";
    }

    // empty is STILL empty!
    std::cout << "Original 'empty' board is unchanged:\n" << boardToString(empty) << "\n";

    // Chain more moves - each returns a new optional<Board>
    std::optional<Board> afterO = makeMove(*afterX, Position{1, 1}, Cell::O);
    std::optional<Board> afterX2 = makeMove(*afterO, Position{0, 1}, Cell::X);
    std::optional<Board> afterO2 = makeMove(*afterX2, Position{2, 2}, Cell::O);
    std::optional<Board> afterX3 = makeMove(*afterO2, Position{0, 2}, Cell::X);  // X wins!

    std::cout << "Final board (X wins!):\n" << boardToString(*afterX3) << "\n";
    std::cout << "Winner: " << cellToChar(checkWinner(*afterX3)) << "\n\n";

    // ========================================================================
    // Demo 2: Easy Undo - just use a previous board
    // ========================================================================
    std::cout << "DEMO 2: Easy Undo\n";
    std::cout << "-----------------\n\n";

    std::cout << "All previous boards still exist!\n";
    std::cout << "We can 'undo' by using any previous state:\n\n";

    std::cout << "After X's first move:\n" << boardToString(*afterX) << "\n";
    std::cout << "After O's first move:\n" << boardToString(*afterO) << "\n";
    std::cout << "After X's second move:\n" << boardToString(*afterX2) << "\n";

    // ========================================================================
    // Demo 3: Branching - explore different game paths
    // ========================================================================
    std::cout << "DEMO 3: Branching (What-If Analysis)\n";
    std::cout << "------------------------------------\n\n";

    std::cout << "Starting from this position:\n" << boardToString(*afterX) << "\n";
    std::cout << "O can play in different positions. Let's explore:\n\n";

    // Branch 1: O plays center
    std::optional<Board> branch1 = makeMove(*afterX, Position{1, 1}, Cell::O);
    std::cout << "Branch 1 - O plays center (1,1):\n" << boardToString(*branch1) << "\n";

    // Branch 2: O plays corner
    std::optional<Board> branch2 = makeMove(*afterX, Position{2, 2}, Cell::O);
    std::cout << "Branch 2 - O plays corner (2,2):\n" << boardToString(*branch2) << "\n";

    // Branch 3: O plays edge
    std::optional<Board> branch3 = makeMove(*afterX, Position{0, 1}, Cell::O);
    std::cout << "Branch 3 - O plays edge (0,1):\n" << boardToString(*branch3) << "\n";

    std::cout << "All branches exist simultaneously!\n";
    std::cout << "This is perfect for AI game tree search.\n\n";

    // ========================================================================
    // Demo 4: Higher-Order Functions - pass strategies as arguments
    // ========================================================================
    std::cout << "DEMO 4: Higher-Order Functions\n";
    std::cout << "------------------------------\n\n";

    srand(time(nullptr));

    std::cout << "Playing 3 games with different strategy combinations:\n\n";

    // Game 1: Random vs Random
    std::pair<Board, Cell> game1 = playGame(randomStrategy, randomStrategy);
    std::cout << "Game 1 (Random vs Random):\n" << boardToString(game1.first);
    std::cout << "Winner: " << (game1.second == Cell::Empty ? "Draw" : std::string(1, cellToChar(game1.second))) << "\n\n";

    // Game 2: First-Available vs Random
    std::pair<Board, Cell> game2 = playGame(firstAvailableStrategy, randomStrategy);
    std::cout << "Game 2 (First-Available vs Random):\n" << boardToString(game2.first);
    std::cout << "Winner: " << (game2.second == Cell::Empty ? "Draw" : std::string(1, cellToChar(game2.second))) << "\n\n";

    // Game 3: First-Available vs First-Available
    std::pair<Board, Cell> game3 = playGame(firstAvailableStrategy, firstAvailableStrategy);
    std::cout << "Game 3 (First-Available vs First-Available):\n" << boardToString(game3.first);
    std::cout << "Winner: " << (game3.second == Cell::Empty ? "Draw" : std::string(1, cellToChar(game3.second))) << "\n\n";

    // ========================================================================
    // Summary
    // ========================================================================
    std::cout << "==============================================\n";
    std::cout << "  Key Functional Programming Concepts\n";
    std::cout << "==============================================\n\n";
    std::cout << "1. PURE FUNCTIONS: checkWinner(), isFull() - no side effects\n";
    std::cout << "2. IMMUTABILITY: makeMove() returns NEW board\n";
    std::cout << "3. EASY UNDO: Just use a previous board variable\n";
    std::cout << "4. BRANCHING: Explore multiple game states simultaneously\n";
    std::cout << "5. HIGHER-ORDER: Pass strategy functions as arguments\n\n";

    std::cout << "Compare with OOP version:\n";
    std::cout << "  https://github.com/CIS-25-F25-BCC/tic-tac-toe\n\n";

    return 0;
}
