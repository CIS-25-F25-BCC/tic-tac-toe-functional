#include "tictactoe.h"
#include <algorithm>  // std::all_of, std::find_if, std::copy_if, std::count_if
#include <iterator>   // std::back_inserter
#include <cstdlib>
#include <ctime>

// ============================================================================
// std::optional - A VALUE THAT MIGHT NOT EXIST
//
// In functional programming, we avoid "null" or "invalid" values. Instead,
// we use a type that explicitly represents "maybe there's a value, maybe not."
//
// std::optional<T> is a container that either:
//   - Contains exactly one value of type T, OR
//   - Contains nothing (is "empty")
//
// Think of it like a box:
//   - The box might have something inside (a T value)
//   - Or the box might be empty
//
// Why use optional instead of special values like -1 or nullptr?
//   - The TYPE itself tells you the value might be missing
//   - The compiler enforces that you handle the "missing" case
//   - No "magic numbers" like -1 to remember
//   - Clearer intent: "this function might not return a result"
//
// Key operations:
//   std::nullopt           - Create an empty optional (the "nothing" value)
//   std::optional<T>{val}  - Create an optional containing val
//   opt.has_value()        - Check if it contains a value (returns bool)
//   *opt                   - Get the value inside (ONLY if has_value() is true!)
//   opt.value_or(default)  - Get value if present, otherwise return default
//
// The ternary operator works with optional (converts to bool):
//   opt ? expr_if_present : expr_if_empty
//
// Example:
//   std::optional<int> findAge(std::string name);  // might not find the person
//
//   std::optional<int> maybeAge = findAge("Alice");
//   // maybeAge either contains an int, or is empty
//
//   int age = maybeAge.value_or(0);  // Use 0 if not found
//   // OR
//   if (maybeAge) {
//       std::cout << "Age: " << *maybeAge << std::endl;
//   }
//
// In this codebase, makeMove() returns std::optional<Board>:
//   - If the move is valid, returns the new board state
//   - If the move is invalid (out of bounds, cell occupied), returns nothing
// ============================================================================

// ============================================================================
// THE STANDARD TEMPLATE LIBRARY (STL)
//
// The STL is a collection of pre-built tools that come with C++.
// It saves you from reinventing common data structures and algorithms.
//
// STL has three main parts:
//
// 1. CONTAINERS - Data structures that hold collections of elements
//      std::vector<T>  - Dynamic array (grows as needed)
//      std::array<T,N> - Fixed-size array (size known at compile time)
//      std::string     - Text (sequence of characters)
//      std::map<K,V>   - Key-value pairs (like a dictionary)
//      std::set<T>     - Unique elements in sorted order
//
// 2. ALGORITHMS - Functions that operate on containers
//      std::sort       - Sort elements
//      std::find       - Find an element
//      std::all_of     - Check if all elements match a condition
//      std::copy_if    - Copy elements that match a condition
//      ... and many more
//
// 3. ITERATORS - "Pointers" that let algorithms work with any container
//      Algorithms don't know about specific containers
//      They just know how to walk through elements using iterators
//
// Why use STL?
//   - Tested and optimized by experts
//   - Works with any data type (that's the "Template" part)
//   - Consistent interface across all containers
//   - Less code for you to write and debug
// ============================================================================

// ============================================================================
// ITERATORS AND RANGES
//
// An ITERATOR is like a pointer that walks through a container (vector, array, etc.)
//
// Every STL container has two special methods:
//   .begin() - returns an iterator pointing to the FIRST element
//   .end()   - returns an iterator pointing PAST the LAST element (not to it!)
//
// Example: If you have a vector {10, 20, 30}
//   vec.begin() points to 10
//   vec.end()   points to... nothing (one past 30)
//
//   [10] [20] [30] [???]
//    ^              ^
//  begin()        end()
//
// STL algorithms work on RANGES defined by two iterators: [begin, end)
// This means "start at begin, stop BEFORE reaching end"
// ============================================================================

// ============================================================================
// LAMBDA EXPRESSIONS (Anonymous Functions)
//
// A lambda is a small function you define right where you need it.
//
// Syntax: [capture](parameters) { body }
//
// Parts:
//   [capture]    - What outside variables the lambda can access
//   (parameters) - Input parameters, just like a regular function
//   { body }     - The code to run
//
// Capture options:
//   []  = nothing - lambda only uses its parameters
//   [&] = by reference - lambda can read/modify outside variables
//   [=] = by value - lambda gets copies of outside variables
//
// Example:
//   auto add = [](int a, int b) { return a + b; };
//   int result = add(3, 4);  // result = 7
//
// Example with capture:
//   int multiplier = 10;
//   auto times = [&](int x) { return x * multiplier; };  // uses 'multiplier'
//   int result = times(5);  // result = 50
// ============================================================================

// ============================================================================
// PREDICATE FUNCTIONS
//
// A PREDICATE is a function that takes one element and returns true/false.
// STL algorithms use predicates to decide what to do with each element.
//
// Example predicates:
//   [](int x) { return x > 0; }      // "Is x positive?"
//   [](int x) { return x % 2 == 0; } // "Is x even?"
//   [](char c) { return c != ' '; }  // "Is c not a space?"
// ============================================================================

// ============================================================================
// Pure Functions Implementation
// ============================================================================

Board emptyBoard() {
    return {{
        {{Cell::Empty, Cell::Empty, Cell::Empty}},
        {{Cell::Empty, Cell::Empty, Cell::Empty}},
        {{Cell::Empty, Cell::Empty, Cell::Empty}}
    }};
}

// ============================================================================
// Cell Conversion Functions
//
// These convert between our Cell type and characters for display/input.
// Notice these are pure expressions - no if statements, just ternary chains.
// ============================================================================

char cellToChar(Cell c) {
    return (c == Cell::X) ? 'X'
         : (c == Cell::O) ? 'O'
         : ' ';
}

Cell charToCell(char c) {
    return (c == 'X') ? Cell::X
         : (c == 'O') ? Cell::O
         : Cell::Empty;
}

// Get cell at position - single expression, no if statements
Cell getCell(const Board& board, int row, int col) {
    return (row >= 0 && row < 3 && col >= 0 && col < 3)
        ? board[row][col]
        : Cell::Empty;
}

// ============================================================================
// IMMEDIATELY INVOKED FUNCTION EXPRESSION (IIFE)
//
// Sometimes we need to compute a value that requires multiple steps, but we
// want to stay expression-based. An IIFE lets us do this:
//
//   [&]() { /* statements */ return value; }()
//
// The [&]() { ... } creates a lambda, and the () at the end calls it immediately.
// This pattern lets us use local variables inside an expression context.
// ============================================================================

std::optional<Board> makeMove(const Board& board, int row, int col, Cell player) {
    // Single expression: validate position AND cell is empty, then create new board
    return (row >= 0 && row < 3 && col >= 0 && col < 3 && board[row][col] == Cell::Empty)
        ? std::optional{[&]() {
            Board newBoard = board;  // Copy the board
            newBoard[row][col] = player;  // Place the piece
            return newBoard;
          }()}  // IIFE: immediately invoke the lambda to get the new board
        : std::nullopt;
}

// ============================================================================
// std::all_of - Check if ALL elements satisfy a condition
//
// Signature: std::all_of(begin_iterator, end_iterator, predicate)
//
// What it does:
//   1. Starts at begin_iterator
//   2. For each element, calls the predicate function
//   3. If ANY predicate returns false, immediately returns false
//   4. If ALL predicates return true, returns true
//
// It's like this for-loop, but more concise:
//   for (auto it = begin; it != end; ++it) {
//       if (!predicate(*it)) return false;
//   }
//   return true;
// ============================================================================

// Helper function: Check if all cells in a line have the same (non-empty) value
// Takes a board and a line (3 positions), returns the winner (Cell::X/Cell::O) or Cell::Empty
Cell lineWinner(const Board& board, const std::array<Position, 3>& line) {
    Cell first = board[line[0].row][line[0].col];

    // A line is a winner if:
    //   1. The first cell is not empty (first != Cell::Empty)
    //   2. ALL cells in the line match the first cell
    // This is a single expression using && for short-circuit evaluation
    return (first != Cell::Empty &&
            std::all_of(line.begin(), line.end(), [&](const Position& p) {
                return board[p.row][p.col] == first;
            }))
        ? first
        : Cell::Empty;
}

// Helper function: Check if a specific line is a winning line for the given board
// Returns true if the line is a winning line
bool isWinningLine(const Board& board, const std::array<Position, 3>& line) {
    return lineWinner(board, line) != Cell::Empty;
}

// ============================================================================
// std::find_if - Find the FIRST element that satisfies a condition
//
// Signature: std::find_if(begin_iterator, end_iterator, predicate)
//
// What it does:
//   1. Starts at begin_iterator
//   2. For each element, calls the predicate function
//   3. If predicate returns true, IMMEDIATELY returns iterator to that element
//   4. If no element matches, returns end_iterator
//
// IMPORTANT: Returns an ITERATOR, not the element itself!
//   - To get the element, dereference it with *it
//   - To check if found, compare: it != container.end()
//
// It's like this for-loop, but more concise:
//   for (auto it = begin; it != end; ++it) {
//       if (predicate(*it)) return it;  // Found!
//   }
//   return end;  // Not found
//
// Pattern for using find_if:
//   auto it = std::find_if(container.begin(), container.end(), predicate);
//   if (it != container.end()) {
//       // Found! Use *it to get the element
//   } else {
//       // Not found
//   }
// ============================================================================

Cell checkWinner(const Board& board) {
    // Find the first winning line (if any exists)
    // winningLines contains all 8 possible lines (3 rows, 3 cols, 2 diagonals)
    //
    // The lambda [&](const auto& line) { return isWinningLine(board, line); }
    // creates a predicate that checks each line against our board
    auto it = std::find_if(winningLines.begin(), winningLines.end(),
        [&](const auto& line) { return isWinningLine(board, line); });

    // If found (it != end), get the winner from that line
    // If not found, return Cell::Empty (no winner)
    return (it != winningLines.end()) ? lineWinner(board, *it) : Cell::Empty;
}

bool isFull(const Board& board) {
    // Check if ALL positions have a non-empty cell
    //
    // allPositions.begin() = iterator to {0,0}
    // allPositions.end()   = iterator past {2,2}
    //
    // For each Position p in allPositions:
    //   - The lambda checks: is board[p.row][p.col] not empty?
    //   - If ALL return true, the board is full
    //
    // The [&] lets the lambda access 'board' from the outer scope
    return std::all_of(allPositions.begin(), allPositions.end(),
        [&](const Position& p) { return board[p.row][p.col] != Cell::Empty; });
}

bool isGameOver(const Board& board) {
    return checkWinner(board) != Cell::Empty || isFull(board);
}

Cell nextPlayer(Cell current) {
    return (current == Cell::X) ? Cell::O : Cell::X;
}

// ============================================================================
// std::count_if - Count elements that satisfy a condition
//
// Signature: std::count_if(begin_iterator, end_iterator, predicate)
//
// What it does:
//   1. Starts at begin_iterator with count = 0
//   2. For each element, calls the predicate function
//   3. If predicate returns true, increments count
//   4. Returns the final count
//
// It's like this for-loop, but more concise:
//   int count = 0;
//   for (auto it = begin; it != end; ++it) {
//       if (predicate(*it)) count++;
//   }
//   return count;
// ============================================================================

int countMoves(const Board& board) {
    // Count how many positions have a piece (X or O)
    //
    // For each Position p in allPositions:
    //   - The lambda checks: is board[p.row][p.col] not empty?
    //   - If true, that position is counted
    //
    // Returns the total number of moves made so far
    return std::count_if(allPositions.begin(), allPositions.end(),
        [&](const Position& p) { return board[p.row][p.col] != Cell::Empty; });
}

// ============================================================================
// std::copy_if - Copy elements that satisfy a condition to a new container
//
// Signature: std::copy_if(begin_iterator, end_iterator, output_iterator, predicate)
//
// What it does:
//   1. Starts at begin_iterator
//   2. For each element, calls the predicate function
//   3. If predicate returns true, copies element to output_iterator
//   4. Advances output_iterator after each copy
//
// std::back_inserter(container):
//   Creates a special iterator that calls push_back() on the container
//   whenever you "write" to it. This grows the container as needed.
//
// It's like this for-loop, but more concise:
//   for (auto it = begin; it != end; ++it) {
//       if (predicate(*it)) {
//           result.push_back(*it);
//       }
//   }
// ============================================================================

std::vector<Position> getValidMoves(const Board& board) {
    // Build a list of all empty positions (valid moves)
    //
    // For each Position p in allPositions:
    //   - The lambda checks: is board[p.row][p.col] empty?
    //   - If true, that position is added to 'moves'
    //
    // std::back_inserter(moves) means: for each match, call moves.push_back(p)
    std::vector<Position> moves;
    std::copy_if(allPositions.begin(), allPositions.end(),
        std::back_inserter(moves),
        [&](const Position& p) { return board[p.row][p.col] == Cell::Empty; });
    return moves;
}

std::string boardToString(const Board& board) {
    // Helper function to convert one row to a string
    // Uses cellToChar to convert Cell enum to display character
    auto rowToString = [](const std::array<Cell, 3>& row) {
        return " " + std::string(1, cellToChar(row[0])) + " | " +
               std::string(1, cellToChar(row[1])) + " | " +
               std::string(1, cellToChar(row[2])) + "\n";
    };

    // Compose the rows together with separators
    return rowToString(board[0]) + "---|---|---\n" +
           rowToString(board[1]) + "---|---|---\n" +
           rowToString(board[2]);
}

// ============================================================================
// RECURSION AS LOOP REPLACEMENT
//
// In functional programming, we avoid mutable variables in loops.
// Instead of:
//   while (!done) { state = update(state); }
//
// We use recursion:
//   function(state) {
//       if (done) return result;
//       return function(newState);  // Recursive call with updated state
//   }
//
// This is "tail recursion" - the recursive call is the LAST thing we do.
// Modern compilers optimize this to be as efficient as a loop.
// ============================================================================

// ============================================================================
// Strategy Selection - Pure Function
//
// This function selects the appropriate strategy based on the current player.
// It's a pure function: given the same inputs, always returns the same output.
// ============================================================================
Strategy selectStrategy(Cell player, Strategy xStrategy, Strategy oStrategy) {
    return (player == Cell::X) ? xStrategy : oStrategy;
}

// Helper function for the recursive game loop
// This is an internal function - users call playGame() instead
std::pair<Board, Cell> playGameStep(const Board& board, Cell player,
                                     Strategy xStrategy, Strategy oStrategy) {
    // Single expression using nested ternary operators
    // This structure mirrors how you'd write it in a functional language:
    //
    //   playGameStep board player xStrat oStrat =
    //     if isGameOver board
    //       then (board, checkWinner board)
    //       else case makeMove board (strategy board player) player of
    //              Just newBoard -> playGameStep newBoard (nextPlayer player) xStrat oStrat
    //              Nothing -> (board, Empty)
    //
    return isGameOver(board)
        ? std::pair{board, checkWinner(board)}
        : [&]() {
            Position move = selectStrategy(player, xStrategy, oStrategy)(board, player);
            auto newBoard = makeMove(board, move.row, move.col, player);
            return newBoard
                ? playGameStep(*newBoard, nextPlayer(player), xStrategy, oStrategy)
                : std::pair{board, Cell::Empty};
          }();
}

std::pair<Board, Cell> playGame(Strategy xStrategy, Strategy oStrategy) {
    return playGameStep(emptyBoard(), Cell::X, xStrategy, oStrategy);
}

// ============================================================================
// Example Strategies
//
// Each strategy is a pure function that takes a board state and current player,
// and returns a Position for where to play. These are expression-based:
// no if-statements, just conditional expressions.
// ============================================================================

Position randomStrategy(const Board& board, Cell player) {
    (void)player;  // Unused - strategy doesn't depend on which player
    auto moves = getValidMoves(board);
    // Single expression: empty check with ternary
    return moves.empty()
        ? Position{-1, -1}
        : moves[rand() % moves.size()];
}

Position firstAvailableStrategy(const Board& board, Cell player) {
    (void)player;  // Unused
    auto moves = getValidMoves(board);
    // Single expression: return first move or invalid position
    return moves.empty()
        ? Position{-1, -1}
        : moves[0];
}

Position centerFirstStrategy(const Board& board, Cell player) {
    (void)player;  // Unused
    // Nested ternary expression: try center, then corners, then first available
    //
    // This reads as:
    //   if center is empty then center
    //   else if any corner is empty then that corner
    //   else first available move
    return (board[1][1] == Cell::Empty)
        ? Position{1, 1}
        : [&]() {
            auto it = std::find_if(corners.begin(), corners.end(),
                [&](const Position& p) { return board[p.row][p.col] == Cell::Empty; });
            return (it != corners.end())
                ? *it
                : firstAvailableStrategy(board, player);
          }();
}
