#ifndef TICTACTOE_FUNCTIONAL_H
#define TICTACTOE_FUNCTIONAL_H

#include <array>
#include <string>
#include <vector>
#include <optional>

// ============================================================================
// FUNCTIONAL PROGRAMMING CONCEPTS
//
// This code demonstrates functional programming principles in C++:
//
// 1. IMMUTABILITY: Data is never modified - we create new copies instead
//    - Board is passed as "const Board&" (read-only reference)
//    - makeMove() returns a NEW board, doesn't modify the input
//
// 2. PURE FUNCTIONS: Same input always gives same output, no side effects
//    - checkWinner(board) always returns the same result for same board
//    - No global state, no I/O inside functions
//
// 3. HIGHER-ORDER FUNCTIONS: Functions that take/return other functions
//    - Strategy is a function type that can be passed around
//    - playGame() takes two Strategy functions as parameters
//
// 4. DECLARATIVE STYLE: Describe WHAT to compute, not HOW
//    - Instead of: for loop with index, check condition, build result
//    - We use: std::all_of, std::find_if, std::copy_if
// ============================================================================

// ============================================================================
// ALGEBRAIC DATA TYPES
//
// In functional programming, we define custom types to represent our domain.
// An "enum class" in C++ is similar to a "sum type" or "algebraic data type".
//
// Instead of using 'char' and remembering that 'X' means player X, ' ' means
// empty, etc., we define a Cell type with exactly three possible values.
//
// Benefits:
//   - The compiler ensures we only use valid values
//   - Code is self-documenting (Cell::X vs 'X')
//   - Pattern matching becomes more natural
//   - No "magic values" like ' ' or '\0'
//
// This is similar to how you'd define a type in other functional languages:
//   data Cell = Empty | X | O
// ============================================================================
enum class Cell { Empty, X, O };

// ============================================================================
// TYPE ALIASES WITH 'using'
//
// The 'using' keyword creates a new name for an existing type.
// It's like giving a nickname to a complex type to make code more readable.
//
// Syntax: using NewName = ExistingType;
//
// Examples:
//   using Board = std::array<std::array<Cell, 3>, 3>;
//   // Now "Board" means "3x3 array of Cell"
//
//   using Strategy = Position(*)(const Board&, Cell);
//   // Now "Strategy" means "pointer to function taking Board and Cell, returning Position"
//
// Benefits:
//   - More readable code (Board vs std::array<std::array<Cell, 3>, 3>)
//   - Single place to change if the underlying type changes
//   - Documents intent (what the type represents, not just what it is)
//
// This is similar to type aliases in functional languages:
//   type Board = [[Cell]]  -- Haskell
//   type Board = Cell[][]  -- TypeScript
// ============================================================================

// Board is just data - a 3x3 array of cells
using Board = std::array<std::array<Cell, 3>, 3>;

// Position on the board
struct Position {
    int row;
    int col;
};

// ============================================================================
// COMPILE-TIME CONSTANTS
//
// constexpr = "compile-time constant"
// These arrays are computed at compile time, not runtime.
// The compiler calculates these values and embeds them directly in the binary.
// ============================================================================

// All positions on the board - precomputed list of all 9 positions
constexpr std::array<Position, 9> allPositions = {{
    {0, 0}, {0, 1}, {0, 2},
    {1, 0}, {1, 1}, {1, 2},
    {2, 0}, {2, 1}, {2, 2}
}};

// All winning lines (rows, columns, diagonals) - 8 ways to win
constexpr std::array<std::array<Position, 3>, 8> winningLines = {{
    // Rows
    {{{0, 0}, {0, 1}, {0, 2}}},
    {{{1, 0}, {1, 1}, {1, 2}}},
    {{{2, 0}, {2, 1}, {2, 2}}},
    // Columns
    {{{0, 0}, {1, 0}, {2, 0}}},
    {{{0, 1}, {1, 1}, {2, 1}}},
    {{{0, 2}, {1, 2}, {2, 2}}},
    // Diagonals
    {{{0, 0}, {1, 1}, {2, 2}}},
    {{{0, 2}, {1, 1}, {2, 0}}}
}};

// Type alias for iterator over winning lines (used by checkWinner helper)
using WinningLinesIterator = std::array<std::array<Position, 3>, 8>::const_iterator;

// ============================================================================
// Pure Functions - no side effects, same input = same output
// ============================================================================

// Create an empty board
Board emptyBoard();

// Get the cell at a position (using Position type)
Cell getCell(const Board& board, Position pos);

// Check if a position is valid (within board bounds)
bool isValidPosition(Position pos);

// Check if a position is empty on the board
bool isEmpty(const Board& board, Position pos);

// Make a move - returns NEW board (doesn't modify input!)
// Returns nullopt if the move is invalid
std::optional<Board> makeMove(const Board& board, Position pos, Cell player);

// Check for winner - returns Cell::X, Cell::O, or Cell::Empty (no winner)
Cell checkWinner(const Board& board);

// Check if board is full
bool isFull(const Board& board);

// Check if game is over
bool isGameOver(const Board& board);

// Get next player
Cell nextPlayer(Cell current);

// Count moves made
int countMoves(const Board& board);

// Get all valid moves from current position
std::vector<Position> getValidMoves(const Board& board);

// Convert board to string for display
std::string boardToString(const Board& board);

// Convert Cell to display character (for output only)
char cellToChar(Cell c);

// Convert character input to Cell (for input only)
Cell charToCell(char c);

// ============================================================================
// Helper Functions
//
// These are small functions that do ONE thing well.
// They can be composed together to build larger operations.
// ============================================================================

// Check if a line is a winning line, return winner (Cell::X/Cell::O) or Cell::Empty
Cell lineWinner(const Board& board, const std::array<Position, 3>& line);

// Check if a line is a winning line (returns bool)
bool isWinningLine(const Board& board, const std::array<Position, 3>& line);

// ============================================================================
// Helper Functions for Expression-Based Code
//
// These small functions help us avoid intermediate variables.
// Each takes data and returns a result - no side effects.
// ============================================================================

// Select random position from moves, or return invalid position if empty
Position randomFromMoves(const std::vector<Position>& moves);

// Select first position from moves, or return invalid position if empty
Position firstFromMoves(const std::vector<Position>& moves);

// Convert winning lines iterator to winner Cell (helper for checkWinner)
Cell iteratorToWinner(const Board& board, WinningLinesIterator it, WinningLinesIterator end);


// ============================================================================
// Higher-Order Functions - functions that take/return functions
// ============================================================================

// Type for AI strategy functions
using Strategy = Position(*)(const Board& board, Cell player);

// Select a strategy based on current player (pure function for strategy dispatch)
Strategy selectStrategy(Cell player, Strategy xStrategy, Strategy oStrategy);

// Continue game from optional board result (helper for playGameStep)
std::pair<Board, Cell> continueFromMove(
    std::optional<Board> maybeBoard,
    Cell player,
    Strategy xStrategy,
    Strategy oStrategy,
    std::pair<Board, Cell> fallback);

// Play a complete game with two strategies
// Returns pair of (final board, winner)
std::pair<Board, Cell> playGame(Strategy xStrategy, Strategy oStrategy);

// ============================================================================
// Example Strategies (for demonstration)
// ============================================================================

// Random valid move
Position randomStrategy(const Board& board, Cell player);

// First available move (top-left to bottom-right)
Position firstAvailableStrategy(const Board& board, Cell player);

#endif // TICTACTOE_FUNCTIONAL_H
