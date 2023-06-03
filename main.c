#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define GRAVITY 0.5
#define JUMP_VELOCITY -10
#define PIPE_GAP 200
#define PIPE_SPEED 5

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

typedef struct {
    double x;
    double y;
    double velocity;
} Bird;

Bird bird;

typedef struct {
    double x;
    double y;
} Pipe;

Pipe pipes[3];
int pipeIndex;

int score;

bool Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("Flappy Bird", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void Shutdown()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void ResetGame()
{
    score = 0;
    bird.x = WINDOW_WIDTH / 2;
    bird.y = WINDOW_HEIGHT / 2;
    bird.velocity = 0;

    for (int i = 0; i < 3; i++) {
        pipes[i].x = WINDOW_WIDTH + i * WINDOW_WIDTH / 3;
        pipes[i].y = rand() % (WINDOW_HEIGHT - PIPE_GAP);
    }

    pipeIndex = 0;
}

void ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            exit(0);
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_SPACE)
                bird.velocity = JUMP_VELOCITY;
        }
    }
}

void Update()
{
    bird.velocity += GRAVITY;
    bird.y += bird.velocity;

    if (bird.y < 0 || bird.y >= WINDOW_HEIGHT) {
        ResetGame();
    }

    for (int i = 0; i < 3; i++) {
        if (bird.x > pipes[i].x + 50 && bird.x < pipes[i].x + 50 + PIPE_SPEED) {
            score++;
        }
        pipes[i].x -= PIPE_SPEED;
        if (pipes[i].x < -50) {
            pipes[i].x = WINDOW_WIDTH;
            pipes[i].y = rand() % (WINDOW_HEIGHT - PIPE_GAP);
        }

        if (bird.x + 50 >= pipes[i].x && bird.x <= pipes[i].x + 50 &&
            (bird.y <= pipes[i].y || bird.y + 50 >= pipes[i].y + PIPE_GAP)) {
            ResetGame();
        }
    }
}

void Render()
{
    char scoreText[10];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);
    SDL_Color textColor = { 255, 255, 255, 255};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect birdRect = { bird.x, bird.y, 50, 50 };
    SDL_RenderFillRect(renderer, &birdRect);

    for (int i = 0; i < 3; i++) {
        SDL_Rect topPipe = { pipes[i].x, 0, 50, pipes[i].y };
        SDL_RenderFillRect(renderer, &topPipe);

        SDL_Rect bottomPipe = { pipes[i].x, pipes[i].y + PIPE_GAP, 50, WINDOW_HEIGHT - pipes[i].y - PIPE_GAP };
        SDL_RenderFillRect(renderer, &bottomPipe);
    }

    SDL_RenderPresent(renderer);
}

int main()
{
    if (!Initialize())
        return 1;

    ResetGame();

    while (1) {
        ProcessInput();
        Update();
        Render();

        SDL_Delay(16);
    }

    Shutdown();

    return 0;
}

