/*
 * Nokia 3310 Style Snake - CS-101 Semester Project
 * Single-player console game demonstrating:
 *   - Arrays (1D & 2D)
 *   - Functions & Recursion
 *   - File Handling
 *   - Pointers & References
 *   - Functions with arrays / pointers
 *   - Pass by value & pass by reference
 *   - if / switch / loops
 *
 * Controls: W A S D  |  P = pause  |  X = quit to menu
 * Compile (MinGW):  g++ -o snake.exe snake.cpp
 * Compile (MSVC):   cl snake.cpp /Fe:snake.exe
 */

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cstring>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#endif

using namespace std;

// -------------------- Constants --------------------
const int WIDTH = 20;
const int HEIGHT = 15;
const int MAX_LENGTH = WIDTH * HEIGHT;
const char* HIGHSCORE_FILE = "highscore.txt";

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };

// -------------------- Game State --------------------
char board[HEIGHT][WIDTH];   // 2D array: game grid
int snakeX[MAX_LENGTH];      // 1D array: snake body X
int snakeY[MAX_LENGTH];      // 1D array: snake body Y
int snakeLength;
int foodX, foodY;
int score;
Direction dir;
bool gameOver;
bool quitToMenu;

// -------------------- Cross-platform helpers --------------------
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void sleepMs(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

#ifndef _WIN32
int kbhitNonBlock() {
    termios oldt, newt;
    int ch;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

int getchCompat() {
    termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif

bool keyPressed() {
#ifdef _WIN32
    return _kbhit() != 0;
#else
    return kbhitNonBlock() != 0;
#endif
}

int readKey() {
#ifdef _WIN32
    return _getch();
#else
    return getchCompat();
#endif
}

// -------------------- File Handling --------------------
// Load high score from file (returns 0 if file missing)
int loadHighScore() {
    ifstream file(HIGHSCORE_FILE);
    int high = 0;
    if (file.is_open()) {
        file >> high;
        file.close();
    }
    return high;
}

// Save high score if beaten (pass by reference for clarity)
void saveHighScore(int& currentScore) {
    int high = loadHighScore();
    if (currentScore > high) {
        ofstream file(HIGHSCORE_FILE);
        if (file.is_open()) {
            file << currentScore;
            file.close();
            cout << "  New high score saved!\n";
        }
    }
}

// -------------------- Recursion --------------------
// Recursively print snake segment coordinates (tail -> head)
void printSnakeSegments(const int* x, const int* y, int index) {
    if (index < 0)
        return;
    printSnakeSegments(x, y, index - 1);
    cout << "(" << x[index] << "," << y[index] << ") ";
}

// Find the maximum in a score history array using iteration
int findMaxIterative(const int* arr, int n) {
    int maxValue = arr[0];
    for (int i = 1; i < n; i++) {
        if (arr[i] > maxValue)
            maxValue = arr[i];
    }
    return maxValue;
}

// -------------------- Pointer / array helpers --------------------
// Clear board using pointer to 2D array row
void clearBoard(char boardGrid[][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            boardGrid[i][j] = ' ';
        }
    }
}

// Place snake and food onto the board (function with arrays)
void placeEntities(char boardGrid[][WIDTH], const int* sx, const int* sy, int length,
                   int fx, int fy) {
    clearBoard(boardGrid);
    for (int i = 0; i < length; i++) {
        if (sx[i] >= 0 && sx[i] < WIDTH && sy[i] >= 0 && sy[i] < HEIGHT) {
            boardGrid[sy[i]][sx[i]] = (i == 0) ? 'O' : 'o';
        }
    }
    if (fx >= 0 && fx < WIDTH && fy >= 0 && fy < HEIGHT)
        boardGrid[fy][fx] = '*';
}

// Swap two ints via pointers
void swapInt(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Grow snake: update length via pointer
void growSnake(int* length) {
    (*length)++;
}

// -------------------- Game setup --------------------
void spawnFood(int* fx, int* fy, const int* sx, const int* sy, int length) {
    bool onSnake;
    do {
        *fx = rand() % WIDTH;
        *fy = rand() % HEIGHT;
        onSnake = false;
        for (int i = 0; i < length; i++) {
            if (sx[i] == *fx && sy[i] == *fy) {
                onSnake = true;
                break;
            }
        }
    } while (onSnake);
}

void initGame() {
    snakeLength = 3;
    snakeX[0] = WIDTH / 2;
    snakeY[0] = HEIGHT / 2;
    snakeX[1] = snakeX[0] - 1;
    snakeY[1] = snakeY[0];
    snakeX[2] = snakeX[0] - 2;
    snakeY[2] = snakeY[0];

    score = 0;
    dir = RIGHT;
    gameOver = false;
    quitToMenu = false;

    spawnFood(&foodX, &foodY, snakeX, snakeY, snakeLength);
}

// -------------------- Drawing --------------------
void drawBoard(const char boardGrid[][WIDTH], int currentScore, int highScore) {
    clearScreen();
    cout << "========== NOKIA SNAKE ==========\n";
    cout << " Score: " << currentScore << "   High: " << highScore << "\n";
    cout << "=================================\n";

    cout << "+";
    for (int j = 0; j < WIDTH; j++) cout << "-";
    cout << "+\n";

    for (int i = 0; i < HEIGHT; i++) {
        cout << "|";
        for (int j = 0; j < WIDTH; j++) {
            cout << boardGrid[i][j];
        }
        cout << "|\n";
    }

    cout << "+";
    for (int j = 0; j < WIDTH; j++) cout << "-";
    cout << "+\n";
    cout << " W/A/S/D move | P pause | X quit\n";
}

// -------------------- Input (pass direction by reference) --------------------
void handleInput(Direction& currentDir, bool& over, bool& quit) {
    if (!keyPressed())
        return;

    int key = readKey();
    // Handle Windows arrow keys (two-byte: 224 then code)
#ifdef _WIN32
    if (key == 224 || key == 0) {
        key = readKey();
        switch (key) {
            case 72: key = 'w'; break;
            case 80: key = 's'; break;
            case 75: key = 'a'; break;
            case 77: key = 'd'; break;
            default: return;
        }
    }
#endif

    switch (key) {
        case 'a':
        case 'A':
            if (currentDir != RIGHT) currentDir = LEFT;
            break;
        case 'd':
        case 'D':
            if (currentDir != LEFT) currentDir = RIGHT;
            break;
        case 'w':
        case 'W':
            if (currentDir != DOWN) currentDir = UP;
            break;
        case 's':
        case 'S':
            if (currentDir != UP) currentDir = DOWN;
            break;
        case 'p':
        case 'P':
            cout << "\n  PAUSED - press any key...\n";
            readKey();
            break;
        case 'x':
        case 'X':
            over = true;
            quit = true;
            break;
        default:
            break;
    }
}

// -------------------- Logic --------------------
// Move snake: arrays passed to function; score/length by reference
void moveSnake(int* sx, int* sy, int& length, Direction currentDir,
               int& fx, int& fy, int& currentScore, bool& over) {
    // Shift body from tail to head
    for (int i = length - 1; i > 0; i--) {
        sx[i] = sx[i - 1];
        sy[i] = sy[i - 1];
    }

    // Move head (pass-by-value used for direction)
    switch (currentDir) {
        case LEFT:  sx[0]--; break;
        case RIGHT: sx[0]++; break;
        case UP:    sy[0]--; break;
        case DOWN:  sy[0]++; break;
        case STOP:  break;
    }

    // Wall collision
    if (sx[0] < 0 || sx[0] >= WIDTH || sy[0] < 0 || sy[0] >= HEIGHT) {
        over = true;
        return;
    }

    // Self collision
    for (int i = 1; i < length; i++) {
        if (sx[0] == sx[i] && sy[0] == sy[i]) {
            over = true;
            return;
        }
    }

    // Eat food
    if (sx[0] == fx && sy[0] == fy) {
        growSnake(&length);
        if (length < MAX_LENGTH) {
            sx[length - 1] = sx[length - 2];
            sy[length - 1] = sy[length - 2];
        }
        currentScore += 10;
        spawnFood(&fx, &fy, sx, sy, length);
    }
}

// Speed based on score (pass by value)
int getDelayMs(int currentScore) {
    int delay = 150 - (currentScore / 50) * 10;
    if (delay < 60)
        delay = 60;
    return delay;
}

// -------------------- Screens --------------------
void showBanner() {
    clearScreen();
    cout << "\n";
    cout << "  ##############################\n";
    cout << "  #                            #\n";
    cout << "  #      N O K I A  S N A K E  #\n";
    cout << "  #         CS-101 Project     #\n";
    cout << "  #                            #\n";
    cout << "  ##############################\n\n";
}

void showHowToPlay() {
    clearScreen();
    cout << "========== HOW TO PLAY ==========\n\n";
    cout << "  Eat * food to grow and score.\n";
    cout << "  Avoid walls and your own body.\n\n";
    cout << "  W - Up\n";
    cout << "  S - Down\n";
    cout << "  A - Left\n";
    cout << "  D - Right\n";
    cout << "  P - Pause\n";
    cout << "  X - Quit to menu\n\n";
    cout << "  Press any key to return...\n";
    readKey();
}

void showHighScore() {
    clearScreen();
    int high = loadHighScore();
    cout << "========== HIGH SCORE ==========\n\n";
    cout << "  Best score: " << high << "\n\n";

    // Demonstrate finding the maximum in a sample score history
    int sample[] = { high, high / 2, high / 3, 20, 10 };
    int n = 5;
    cout << "  Sample score list max (iteration): "
         << findMaxIterative(sample, n) << "\n\n";
    cout << "  Press any key to return...\n";
    readKey();
}

void runGame() {
    initGame();
    int highScore = loadHighScore();

    while (!gameOver) {
        placeEntities(board, snakeX, snakeY, snakeLength, foodX, foodY);
        drawBoard(board, score, highScore);
        handleInput(dir, gameOver, quitToMenu);
        if (gameOver)
            break;
        moveSnake(snakeX, snakeY, snakeLength, dir, foodX, foodY, score, gameOver);
        sleepMs(getDelayMs(score));
    }

    clearScreen();
    cout << "\n  ========== GAME OVER ==========\n";
    cout << "  Final score: " << score << "\n";
    cout << "  Snake length: " << snakeLength << "\n";
    cout << "  Body segments: ";
    printSnakeSegments(snakeX, snakeY, snakeLength - 1);
    cout << "\n\n";

    if (!quitToMenu)
        saveHighScore(score);

    cout << "  Press any key to return to menu...\n";
    readKey();
}

// Demo: pass-by-value does not change original
void addBonusByValue(int points) {
    points += 5;  // only local copy changes
}

// Demo: pass-by-reference changes original
void addBonusByReference(int& points) {
    points += 5;
}

void showConceptsDemo() {
    clearScreen();
    cout << "=== Quick concept demo (for viva) ===\n\n";

    int demoScore = 10;
    cout << " Start score: " << demoScore << "\n";

    addBonusByValue(demoScore);
    cout << " After pass-by-value (+5): " << demoScore << " (unchanged)\n";

    addBonusByReference(demoScore);
    cout << " After pass-by-reference (+5): " << demoScore << " (changed)\n";

    int a = 3, b = 7;
    cout << " Before swap via pointers: a=" << a << " b=" << b << "\n";
    swapInt(&a, &b);
    cout << " After  swap via pointers: a=" << a << " b=" << b << "\n\n";

    cout << " Press any key to return...\n";
    readKey();
}

// -------------------- Menu (switch + loop) --------------------
void mainMenu() {
    bool running = true;
    while (running) {
        showBanner();
        cout << "  1. Play\n";
        cout << "  2. How to Play\n";
        cout << "  3. High Score\n";
        cout << "  4. Concepts Demo\n";
        cout << "  5. Exit\n\n";
        cout << "  Choose: ";

        char choice;
        cin >> choice;

        // clear leftover newline for getch later
        cin.ignore(10000, '\n');

        switch (choice) {
            case '1':
                runGame();
                break;
            case '2':
                showHowToPlay();
                break;
            case '3':
                showHighScore();
                break;
            case '4':
                showConceptsDemo();
                break;
            case '5':
                running = false;
                cout << "\n  Thanks for playing!\n";
                break;
            default:
                cout << "\n  Invalid choice. Press any key...\n";
                readKey();
                break;
        }
    }
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    mainMenu();
    return 0;
}
