# Nokia Snake

A terminal-based Snake game inspired by the classic Nokia 3310 game. Guide the snake around the board, eat food to grow and earn points, and avoid hitting the walls or your own body. The project is also designed to demonstrate introductory C++ concepts, including arrays, functions, pointers, references, recursion, loops, and file handling.

## Features

- Play Snake on a 20 × 15 terminal board.
- Control the snake with **W**, **A**, **S**, and **D**. Windows builds also support the arrow keys.
- Pause gameplay with **P** or quit the current game and return to the menu with **X**.
- Earn 10 points and grow by one segment each time the snake eats food.
- Snake speed increases as the score rises.
- View instructions, the saved high score, and a C++ concepts demonstration from the main menu.
- Save a new high score to `highscore.txt` after a game ends.

## Requirements

- A C++ compiler with C++17 support, such as `g++`.
- A terminal that supports keyboard input.
- On macOS and Linux, run the game in an interactive terminal so it can read key presses.
- `make` is optional and is needed only to use the provided Makefile.

## Build and Run

### macOS or Linux

From the project directory, build and start the game with:

```sh
make
./snake
```

Or use the Makefile's run target:

```sh
make run
```

To compile without `make`:

```sh
g++ -std=c++17 -o snake snake.cpp
./snake
```

### Windows (MinGW)

Compile and run from Command Prompt or PowerShell:

```sh
g++ -std=c++17 -o snake.exe snake.cpp
.\snake.exe
```

## How to Play

1. Select **Play** from the main menu.
2. Use **W/A/S/D** to steer the snake. On Windows, arrow keys can also be used.
3. Eat `*` food to grow and increase your score.
4. Avoid the board edges and the snake's body; either collision ends the game.
5. Press **P** to pause and resume by pressing a key. Press **X** to leave the current game and return to the menu.

The main menu also provides **How to Play**, **High Score**, **Concepts Demo**, and **Exit** options.

## High Scores

The game reads and writes the high score in `highscore.txt` in the current working directory. If the file does not exist, the high score starts at 0 and is created when a higher score is saved. The high score is a single shared value; the current version does not have usernames, individual player profiles, levels, a leaderboard, or administrator features.

## Project Files

- `snake.cpp` — game implementation.
- `makefile` — build, run, and clean targets for macOS/Linux environments with `make` and `g++`.
- `highscore.txt` — saved high score.

## Clean Build Output

Remove the compiled executable using:

```sh
make clean
```
