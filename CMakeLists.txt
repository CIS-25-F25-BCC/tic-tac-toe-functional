cmake_minimum_required(VERSION 3.10)
project(TicTacToeFunctional)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Main executable
add_executable(tictactoe_functional
    main.cpp
    tictactoe.cpp
)

# Compiler warnings
target_compile_options(tictactoe_functional PRIVATE
    -Wall -Wextra -Wpedantic
)
