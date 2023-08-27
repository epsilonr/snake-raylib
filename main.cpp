#include <raylib.h>
#include <vector>
#include <cstdio>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 1024

#define GRID_WIDTH 64
#define GRID_HEIGHT 64

enum Direction {
    DIR_LEFT = 1,
    DIR_RIGHT = 2,
    DIR_DOWN = 3,
    DIR_UP = 4
};

typedef struct Player {
    Rectangle rect;
    int x;
    int y;
    int speed = 64;
    Direction dir;
    int score;
    std::vector<Rectangle> tail;
} Player;

typedef struct Food {
    int x;
    int y;
} Food;

//! Todo: Modify, food shouldn't be spawned inside snake
Food GenerateRandomFood(const Player &player) {
    Food food;
    bool validPosition = false;

    while (!validPosition) {
        food.x = GetRandomValue(0, (SCREEN_WIDTH / GRID_WIDTH) - 1) * GRID_WIDTH;
        food.y = GetRandomValue(0, (SCREEN_HEIGHT / GRID_HEIGHT) - 1) * GRID_HEIGHT;

        Rectangle foodRect = { static_cast<float>(food.x), static_cast<float>(food.y), GRID_WIDTH, GRID_HEIGHT };
        validPosition = !CheckCollisionRecs(foodRect, player.rect);

        for (const Rectangle& tailSegment : player.tail) {
            if (CheckCollisionRecs(foodRect, tailSegment)) {
                validPosition = false;
                break;
            }
        }
    }

    return food;
}

Player InitializePlayer() {
    Player player;
    player.rect.x = 0;
    player.rect.y = 0;
    player.rect.width = GRID_WIDTH;
    player.rect.height = GRID_HEIGHT;
    player.speed = 64;
    player.dir = DIR_RIGHT;
    player.score = 0;
    player.tail.clear();
    return player;
}

void DrawGrid() {
    for (int x = 0; x < SCREEN_WIDTH; x += GRID_WIDTH)
        DrawLine(x, 0, x, SCREEN_HEIGHT, LIGHTGRAY);
    for (int y = 0; y < SCREEN_HEIGHT; y += GRID_HEIGHT)
        DrawLine(0, y, SCREEN_WIDTH, y, LIGHTGRAY);
}

void DrawTextCentered(const char* text, int fontSize, Color color, int y) {
    float textWidth = MeasureText(text, fontSize);
    int x = (SCREEN_WIDTH - textWidth) / 2;
    DrawText(text, x, y, fontSize, color);
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake Game");

    Player player = InitializePlayer();
    Rectangle oldHead;

    Food food = GenerateRandomFood(player);

    char scoreText[32];

    bool onMenu = false;
    bool died = false;

    SetExitKey(KEY_F12);

    SetTargetFPS(10);
    while (!WindowShouldClose()) {

        // Handle User Input
        if (IsKeyPressed(KEY_RIGHT) && player.dir != DIR_LEFT)
            player.dir = DIR_RIGHT;
        else if (IsKeyPressed(KEY_LEFT) && player.dir != DIR_RIGHT)
            player.dir = DIR_LEFT;
        else if (IsKeyPressed(KEY_DOWN) && player.dir != DIR_UP)
            player.dir = DIR_DOWN;
        else if (IsKeyPressed(KEY_UP) && player.dir != DIR_DOWN)
            player.dir = DIR_UP;
        if (IsKeyPressed(KEY_ENTER) && onMenu) {
            if (died) {
                died = false;
                player = InitializePlayer();
                food = GenerateRandomFood(player);
            }

            onMenu = false;
        }
        else if (IsKeyPressed(KEY_ESCAPE)) {
            if (onMenu && died) {
                died = false;
                player = InitializePlayer();
                food = GenerateRandomFood(player);
            }

            onMenu = !onMenu;
        }

        snprintf(scoreText, sizeof(scoreText), "Score: %d", player.score);
        if (onMenu) {
            BeginDrawing();
            ClearBackground(BLACK);

            if (died)
                DrawTextCentered("YOU DIED!", 64, RED, SCREEN_HEIGHT / 2 - 64);

            DrawTextCentered(scoreText, 32, WHITE, 32);
            DrawTextCentered("Press \"Enter\" to continue.", 32, WHITE, SCREEN_HEIGHT / 2 + 32);

            EndDrawing();

            continue;
        }

        oldHead = player.rect;

        switch (player.dir) {
            case DIR_LEFT:
                player.rect.x -= player.speed;
                break;
            case DIR_DOWN:
                player.rect.y += player.speed;
                break;
            case DIR_RIGHT:
                player.rect.x += player.speed;
                break;
            case DIR_UP:
                player.rect.y -= player.speed;
                break;
            default:
                break;
        }

        if (player.rect.x >= SCREEN_WIDTH)
            player.rect.x = static_cast<int>(player.rect.x) % SCREEN_WIDTH;
        else if (player.rect.x < 0)
            player.rect.x += SCREEN_WIDTH;
        if (player.rect.y >= SCREEN_HEIGHT)
            player.rect.y = static_cast<int>(player.rect.y) % SCREEN_HEIGHT;
        else if (player.rect.y < 0)
            player.rect.y += SCREEN_HEIGHT;

        if (CheckCollisionRecs(player.rect, (Rectangle){ static_cast<float>(food.x), static_cast<float>(food.y), GRID_WIDTH, GRID_HEIGHT })) {
            player.score++;
            player.tail.push_back(oldHead);
            food = GenerateRandomFood(player);
        }

        // Move the tail
        if (!player.tail.empty()) {
            Rectangle prevSegment = oldHead;
            Rectangle temp;

            for (Rectangle& tailSegment : player.tail) {
                temp = tailSegment;
                tailSegment = prevSegment;
                prevSegment = temp;
            }
        }

        // Check tail collision
        bool tailCollision = false;
        for (const Rectangle& tailSegment : player.tail) {
            if (CheckCollisionRecs(player.rect, tailSegment)) {
                tailCollision = true;
                break;
            }
        }

        if (tailCollision) {
            tailCollision = false;
            died = true;
            onMenu = true;

            continue;
        }

        // Draw
        BeginDrawing();
        ClearBackground(BLACK);

        // Draw grid
        DrawGrid();

        //Draw head
        DrawRectangleRec(player.rect, WHITE);

        // Draw tail
        for (const Rectangle& tailSegment : player.tail) {
            DrawRectangleRec(tailSegment, WHITE);
        }

        // Draw food
        DrawRectangle(food.x, food.y, GRID_WIDTH, GRID_HEIGHT, RED);

        // Draw score
        DrawText(scoreText, 16, 16,  32, WHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}