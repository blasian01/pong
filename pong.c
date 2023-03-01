#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#define ROWS 24
#define COLS 80
#define BALL_SPEED 2
#define PADDLE_SPEED 2

// Define a structure to represent the ball
typedef struct {
    int row;
    int col;
    int dr;
    int dc;
} Ball;

// Define a structure to represent the player's paddle
typedef struct {
    int row;
    int col;
    int height;
} Paddle;

// Set up the keyboard input to read input without pressing enter
void set_terminal_mode() {
    struct termios ttystate;
    tcgetattr(STDIN_FILENO, &ttystate);
    ttystate.c_lflag &= ~(ICANON | ECHO);
    ttystate.c_cc[VMIN] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

// Reset the keyboard input to normal mode
void reset_terminal_mode() {
    struct termios ttystate;
    tcgetattr(STDIN_FILENO, &ttystate);
    ttystate.c_lflag |= ICANON | ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

// Draw a character at the given row and column
void draw_char(char c, int row, int col) {
    printf("\033[%d;%dH%c", row, col, c);
    fflush(stdout);
}

// Draw the game board
void draw_board() {
    int row, col;
    for (row = 1; row <= ROWS; row++) {
        for (col = 1; col <= COLS; col++) {
            if (row == 1 || row == ROWS) {
                draw_char('-', row, col);
            } else if (col == 1 || col == COLS) {
                draw_char('|', row, col);
            }
        }
    }
}

// Draw the ball at its current position
void draw_ball(Ball ball) {
    draw_char('O', ball.row, ball.col);
}

// Draw the paddle at its current position
void draw_paddle(Paddle paddle) {
    int i;
    for (i = 0; i < paddle.height; i++) {
        draw_char('|', paddle.row + i, paddle.col);
    }
}

// Move the ball to its next position
void move_ball(Ball *ball) {
    ball->row += ball->dr;
    ball->col += ball->dc;
    // Check for collisions with walls
    if (ball->row == 2 || ball->row == ROWS) {
        ball->dr *= -1;
    }
}

// Move the paddle up or down by the given amount
void move_paddle(Paddle *paddle, int amount) {
    paddle->row += amount;
    // Check for collisions with walls
    if (paddle->row < 2) {
        paddle->row = 2;
    } else if (paddle->row + paddle->height > ROWS) {
        paddle->row = ROWS - paddle->height;
    }
}

// Check for collisions between the ball and the paddle
void check_collision(Ball *ball, Paddle *paddle) {
    if (ball->col == paddle->col) {
        int ball_row = ball->row;
        if (ball_row >= paddle->row && ball_row < paddle->row + paddle->height) {
            ball->dc *= -1;
        }
    }
}

int main() {
    set_terminal_mode();
    int input;
    int quit = 0;
    // Initialize the ball
    Ball ball = { ROWS/2, COLS/2, BALL_SPEED, BALL_SPEED };
    // Initialize the player's paddle
    Paddle paddle = { ROWS/2, 2, 6 };
    // Draw the initial game state
    draw_board();
    draw_ball(ball);
    draw_paddle(paddle);
    while (!quit) {
        usleep(50000);
        // Move the ball and check for collisions
        move_ball(&ball);
        check_collision(&ball, &paddle);
        // Move the player's paddle based on input
        input = getchar();
        if (input == 'q') {
            quit = 1;
        } else if (input == 'w') {
            move_paddle(&paddle, -PADDLE_SPEED);
        } else if (input == 's') {
            move_paddle(&paddle, PADDLE_SPEED);
        }
        // Clear the screen and redraw the game state
        system("clear");
        draw_board();
        draw_ball(ball);
        draw_paddle(paddle);
    }
    reset_terminal_mode();
    return 0;
}
