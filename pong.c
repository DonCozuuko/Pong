#include <raylib.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

static const int screenWidth = 960;
static const int screenHeight = 540;
static const int starting_y_pos_pad = 235;
static const int score_limit = 11;

typedef enum Screen { TITLE=0, GAME_START, GAME, GAME_POINT, END } Screen;

typedef struct {
    int size;
    int x_pos;
    int y_pos;
    int x_vel;
    int y_vel;
} Ball;

void eventHandler(int *y_pos1, int *y_pos2, int min_y, int max_y);
void paddles(Ball *ball, int min_y, int max_y, Rectangle *pad1, Rectangle *pad2, Sound sound);
void drawDashedLines();
Ball randomVels(Ball *ball);
void updateScore(Ball *ball, int *score1, int *score2);

int main(void) {
    srand(time(NULL));
    InitWindow(screenWidth, screenHeight, "pong motha fuckas");
    InitAudioDevice();
    Screen currentScreen = TITLE;

    int y_pos1 = starting_y_pos_pad;
    int y_pos2 = starting_y_pos_pad;
    int p1_score = 0;
    int p2_score = 0;
    int max_x = screenWidth, max_y = screenHeight;
    int min_x = 0, min_y = 0;

    Ball ball = { 20, screenWidth / 2, screenHeight / 2, 5, 5 };
    randomVels(&ball);

    SetTargetFPS(60);
    static int count = 0;
    static int gamePointFlag = 0;
    static int gameWinner = 0;

    Sound pongSound = LoadSound("bin/low_blip.wav");
    Sound victory = LoadSound("bin/fart.mp3");

    while (!WindowShouldClose()) {
        // Init paddle recs     
        Rectangle pad1 = { 810, y_pos1, 15, 70 };
        Rectangle pad2 = { 150, y_pos2, 15, 70 };

        char score1[10];
        char score2[10];

        switch (currentScreen) {
            case TITLE: {
                if (IsKeyDown(KEY_ENTER)) {
                    currentScreen = GAME_START;
                }
            } break;
            case GAME_START: {
                y_pos1 = starting_y_pos_pad, y_pos2 = starting_y_pos_pad;
                ball.x_pos = screenWidth / 2, ball.y_pos = screenHeight / 2;
                count++;
                if (count > 80) {
                    count = 0;
                    currentScreen = GAME;
                }
                // After 1 second switch to GAME
            } break;
            case GAME_POINT: {
                count++;
                if (count > 120) {
                    count = 0;
                    currentScreen = GAME_START;
                }
            } break;
            case GAME: {
                // Handle keyboard inputs
                eventHandler(&y_pos1, &y_pos2, min_y, max_y);
                // Collision for paddles and ball interactions
                paddles(&ball, min_y, max_y, &pad1, &pad2, pongSound);
                // Make the ball move
                ball.x_pos += ball.x_vel;
                ball.y_pos += ball.y_vel;
                // Update the score
                if (ball.x_pos > screenWidth) {
                p2_score += 1;
                currentScreen = GAME_START;
                }
                if (ball.x_pos < 0) {
                    p1_score += 1;
                    currentScreen = GAME_START;
                }
                if (gamePointFlag == 0 &&
                    (p1_score == score_limit - 1 || p2_score == score_limit - 1)) {
                    gamePointFlag = 1;
                    currentScreen = GAME_POINT;
                }
                
                if (p1_score == score_limit) {
                    // declare winner with a flag
                    gameWinner = 1;
                    PlaySound(victory);
                    currentScreen = END;
                }
                if (p2_score == score_limit) {
                    // declare winner with a flag
                    gameWinner = 2;
                    PlaySound(victory);
                    currentScreen = END;
                }
                // Convert the score into a string
                sprintf(score1, "%d", p1_score);
                sprintf(score2, "%d", p2_score);
                // When a player scores,
                // if that score now exceeds a score limit then switch to END
                // else switch to GAME_START
            } break;
            case END: {
                Rectangle playAgain = { 430, 420, 150, 70 };
                if (CheckCollisionPointRec(GetMousePosition(), playAgain)) {
                    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                        p1_score = 0, p2_score = 0;
                        currentScreen = TITLE;
                    }
                }
            } break;
            default: break;
        }

        BeginDrawing();
            ClearBackground(BLACK);
            switch(currentScreen) {
                case TITLE: {
                    DrawText("PONG BITCH!", 200, screenHeight / 2 - 90, 90, WHITE);
                    DrawText("Press Enter to Play", 275, screenHeight / 2 + 30, 40, WHITE);
                    DrawText("Player 1 Controls (Left-Side):\na = Up, s = Down", 15, screenHeight / 2 + 150, 20, LIGHTGRAY);
                    DrawText("Player 2 Controls (Right-Side):\nl = Up, k = Down", 15, screenHeight / 2 + 200, 20, LIGHTGRAY);
                } break;
                case GAME_START: {
                    if (count > 0 && count < 21) {
                        DrawText("3", screenWidth / 2 - 80, 110, 400, WHITE);
                    }
                    else if (count > 20 && count < 41) {
                        DrawText("2", screenWidth / 2 - 80, 110, 400, WHITE);
                    }
                    else if (count > 40 && count < 61) {
                        DrawText("1", screenWidth / 2 - 80, 110, 400, WHITE);
                    }
                    else if (count > 60 && count < 81) {
                        DrawText("PONG", 85, 110, 300, WHITE);
                    }
                } break;
                case GAME_POINT: {
                    if (count > 0 && count < 31) {
                        DrawText("GAME", 80, 20, 280, WHITE);
                        DrawText("POINT", 30, 270, 280, WHITE);
                    }
                    else if (count > 30 && count < 61) {
                        DrawText("", 0, 0, 0, WHITE);
                    }
                    else if (count > 60 && count < 91) {
                        DrawText("GAME", 80, 20, 280, WHITE);
                        DrawText("POINT", 30, 270, 280, WHITE);
                    }
                    else if (count > 90 && count < 121) {
                        DrawText("", 0, 0, 0, WHITE);
                    }

                } break;
                case GAME: {
                    // Paddles
                    DrawRectangle(pad1.x, pad1.y, pad1.width, pad1.height, WHITE);
                    DrawRectangle(pad2.x, pad2.y, pad2.width, pad2.height, WHITE);
                    // Ball
                    DrawRectangle(ball.x_pos, ball.y_pos, ball.size, ball.size, WHITE);
                    // Middle line
                    drawDashedLines();
                    // Scores
                    DrawText(score1, 750, 60, 30, WHITE);
                    DrawText(score2, 250, 60, 30, WHITE);
                } break;
                case END: {
                    if (gameWinner == 1) {
                        DrawText("PLAYER1 WINS", 200, 110, 80, WHITE);
                    }
                    else if (gameWinner == 2) {
                        DrawText("PLAYER2 WINS", 200, 110, 80, WHITE);
                    }
                    DrawText("Play Again", 410, 375, 40, WHITE);
                    DrawRectangle(430, 420, 150, 70, BLACK);
                    DrawRectangleLines(430, 420, 150, 70, WHITE);
                } break;
                default: break;
            }
            DrawFPS( 10, 10 );
        EndDrawing();
    }
    UnloadSound(pongSound);
    UnloadSound(victory);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

void eventHandler(int *y_pos1, int *y_pos2, int min_y, int max_y) {
    // Handles paddle input
    if (IsKeyDown(KEY_A)) {
        *y_pos2 = *y_pos2 - 10;
    }
    else if (IsKeyDown(KEY_S)) {
        *y_pos2 = *y_pos2 + 10;
    }
    if (IsKeyDown(KEY_L)) {
        *y_pos1 = *y_pos1 - 10;
    }
    else if (IsKeyDown(KEY_K)) {
        *y_pos1 = *y_pos1 + 10;
    }
    // Handles paddle top and bottom wall collision
    if (*y_pos1 < min_y) {
        *y_pos1 = 0;
    }
    else if (*y_pos1 + 70 > max_y) {
        *y_pos1 = max_y - 70;
    }
    
    if (*y_pos2 < min_y) {
       *y_pos2 = 0;
    }
    else if (*y_pos2 + 70 > max_y) {
       * y_pos2 = max_y - 70;
    }
}

void paddles(Ball *ball, int min_y, int max_y, Rectangle *pad1, Rectangle *pad2, Sound sound) {
    // Handles ball top and bottom wall collision
    if (ball->y_pos + ball->size > max_y) {
        ball->y_vel = -ball->y_vel;
        PlaySound(sound);
    }
    else if (ball->y_pos < min_y) {  
        ball->y_vel = -ball->y_vel;
        PlaySound(sound);
    }
    // Handles ball collision with paddle 1
    // Zone behind the paddle
    if (ball->x_pos > pad1->x + pad1->width &&
        ball->y_pos + ball->size > pad1->y &&
        ball->y_pos < pad1->y + pad1->height ) {
        ;
    }
    // Zone in front of the paddle
    else if (ball->x_pos + ball->size > pad1->x &&
            ball->x_pos > pad1->x - ball->size &&
            ball->y_pos + ball->size > pad1->y &&
            ball->y_pos < pad1->y + pad1->height){
            ball->x_vel = -ball->x_vel;
            PlaySound(sound);
        }
    // Handles ball collision with paddle 2
    // Zone behind the paddle
    if (ball->x_pos < pad2->x &&
        ball->y_pos + ball->size > pad2->y &&
        ball->y_pos < pad2->y + pad2->height) {
        ;
    }
    // Zone in front of the paddle
    else if (ball->x_pos < pad2->x + pad2->width &&
            ball->x_pos < pad2->x + pad2->width + ball->size &&
            ball->y_pos + ball->size > pad2->y &&
            ball->y_pos < pad2->y + pad2->height){
                ball->x_vel = -ball->x_vel;
                PlaySound(sound);
        }
}

void drawDashedLines() {
    int dashSize = 20;
    int gap_size = 20;

    int y;
    for (y = 0; y < screenHeight; y += 40) {
        DrawLine(screenWidth / 2, y, screenWidth / 2, y + 20, WHITE);
        DrawLine(screenWidth / 2, y + 20, screenWidth / 2, y + 40, BLACK);
    }
}

// min & max inclusive
int randint(int min, int max) {
    return min + (rand() % (min - max + 1));
}
// picks a random x and y velocity
Ball randomVels(Ball *ball) {
    int x_vel = randint(4, 9);
    int y_vel = randint(4, 9);

    ball->x_vel = x_vel, ball->y_vel = y_vel;
}

void updateScore(Ball *ball, int *score1, int *score2) {
    if (ball->x_pos > screenWidth) {
        score1 += 1;
    }
}