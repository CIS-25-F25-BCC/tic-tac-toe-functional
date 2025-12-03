# Tic-Tac-Toe: Functional Style (C++)

This is a **functional programming** implementation of Tic-Tac-Toe in C++, demonstrating:

- **Pure functions**: Same input always produces same output, no side effects
- **Immutability**: Board state is never modified; new boards are created
- **First-class functions**: Functions passed as arguments (for AI strategies)
- **No hidden state**: Everything is explicit

## Functional vs OOP Comparison

| Aspect | OOP Version | Functional Version |
|--------|-------------|-------------------|
| State | Hidden inside object | Explicit, passed as argument |
| Mutation | `game.makeMove()` modifies object | `makeMove()` returns new board |
| Undo | Requires storing history | Just use the previous board |
| Testing | Need to set up object state | Pass input, check output |
| Parallelism | Risky (shared mutable state) | Safe (no shared state) |

**See also**: [OOP Version](https://github.com/CIS-25-F25-BCC/tic-tac-toe) for comparison

## Building

```bash
mkdir build && cd build
cmake ..
make
```

## Running

```bash
./tictactoe_functional
```

## Key Concepts Demonstrated

### 1. Pure Functions
```cpp
// Pure: only depends on inputs, no side effects
char checkWinner(const Board& board);
bool isFull(const Board& board);
```

### 2. Immutability
```cpp
// Returns NEW board, doesn't modify input
Board makeMove(const Board& board, int row, int col, char player);

// Original board unchanged:
Board after1 = makeMove(empty, 0, 0, 'X');  // empty still empty!
Board after2 = makeMove(after1, 1, 1, 'O'); // after1 still has just X
```

### 3. Easy Undo/Branching
```cpp
// Want to undo? Just use the previous board
Board current = after2;
current = after1;  // "Undo" - after2 still exists if needed

// Want to explore alternatives? Branch from any state
Board branch1 = makeMove(after1, 0, 1, 'O');  // O plays differently
Board branch2 = makeMove(after1, 2, 2, 'O');  // Another option
```

## Course Information

**CIS-25: Programming Using C++**
Fall 2025, Berkeley City College

GitHub Organization: [CIS-25-F25-BCC](https://github.com/CIS-25-F25-BCC)
