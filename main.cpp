#include <iostream>
#include <cmath>
#include <SDL3/SDL.h>
using namespace std;

// Constants
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 512
#define CELL_SIZE 64
#define PLAYER_SIZE 20
#define LINE_LENGTH 200
#define PLAYER_SPEED 10
#define PI 3.14159

// Player variables
float playerX = WINDOW_WIDTH / 2, playerY = WINDOW_HEIGHT / 2;
float playerAngle = 0.0;
float playerDX = cos(playerAngle), playerDY = sin(playerAngle);
SDL_FRect player = {playerX - WINDOW_WIDTH / 4, playerY, PLAYER_SIZE, PLAYER_SIZE};
SDL_FRect parameter = {0, 0, 512, 512};

// Function declarations
void RenderPlayer(SDL_Renderer *renderer);
void MovePlayer(SDL_Event event);
void RenderGrid(SDL_Renderer *renderer);
void CastRay(SDL_Renderer *renderer);

// Map
int mapp[] = {
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    1,
    0,
    0,
    1,
    0,
    0,
    0,
    1,
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    1,
    0,
    0,
    0,
    1,
    0,
    0,
    1,
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
};

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) == 0)
        return -1;

    SDL_Window *window = SDL_CreateWindow("Ray-Caster", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (!window)
        return -1;

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer)
        return -1;

    SDL_Event event;
    bool running = true;
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN)
            {
                MovePlayer(event);
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        RenderGrid(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderRect(renderer, &parameter);

        RenderPlayer(renderer);
        CastRay(renderer);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void RenderPlayer(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &player);

    float lineEndX = player.x + PLAYER_SIZE / 2 + LINE_LENGTH * cos(playerAngle);
    float lineEndY = player.y + PLAYER_SIZE / 2 + LINE_LENGTH * sin(playerAngle);
    // SDL_RenderLine(renderer, player.x + PLAYER_SIZE / 2, player.y + PLAYER_SIZE / 2, lineEndX, lineEndY);
}

void MovePlayer(SDL_Event event)
{
    playerDX = PLAYER_SPEED * cos(playerAngle);
    playerDY = PLAYER_SPEED * sin(playerAngle);

    switch (event.key.key)
    {
    case SDLK_W:
        if (player.x > 0 && player.x + PLAYER_SIZE < WINDOW_WIDTH / 2)
            player.x += playerDX;
        else
            player.x = WINDOW_WIDTH / 2 - 1 - PLAYER_SIZE;

        if (player.y > 0 && player.y + PLAYER_SIZE < WINDOW_HEIGHT)
            player.y += playerDY;
        else
            player.y = WINDOW_HEIGHT - 1 - PLAYER_SIZE;
        break;

    case SDLK_S:
        if (player.x > 0 && player.x + PLAYER_SIZE < WINDOW_WIDTH / 2)
            player.x -= playerDX;
        else
            player.x = WINDOW_WIDTH / 2 - 1 - PLAYER_SIZE;

        if (player.y > 0 && player.y + PLAYER_SIZE < WINDOW_HEIGHT)
            player.y -= playerDY;
        else
            player.y = WINDOW_HEIGHT - 1 - PLAYER_SIZE;
        break;

    case SDLK_A:
        playerAngle += 0.05;
        if (playerAngle > 2 * PI)
            playerAngle -= 2 * PI;
        break;

    case SDLK_D:
        playerAngle -= 0.05;
        if (playerAngle < 0)
            playerAngle += 2 * PI;
        break;
    }
}

void RenderGrid(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int horizontalGrid = 0; horizontalGrid < 512; horizontalGrid += CELL_SIZE)
    {
        for (int verticalGrid = 0; verticalGrid < 512; verticalGrid += CELL_SIZE)
        {
            int mapIndex = (verticalGrid / CELL_SIZE) * 8 + (horizontalGrid / CELL_SIZE);

            SDL_FRect grid = {horizontalGrid, verticalGrid, CELL_SIZE, CELL_SIZE};

            if (mapp[mapIndex] == 1)
            {
                SDL_SetRenderDrawColor(renderer, 255, 100, 255, 255);
                SDL_RenderFillRect(renderer, &grid);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            }

            SDL_RenderRect(renderer, &grid);
        }
    }
}

void CastRay(SDL_Renderer *renderer)
{

    int depthOfField = 0;

    float stepX, stepY;
    float rayX, rayY;
    float horizontalHitX, horizontalHitY;

    if (playerAngle > PI)
    {
        rayY = (((int)player.y / CELL_SIZE) * CELL_SIZE) - 0.0000f;
        rayX = player.x + (rayY - player.y) / tan(playerAngle);
        stepY = -CELL_SIZE;
        stepX = stepY / tan(playerAngle);
    }
    else if (playerAngle < PI)
    {
        rayY = (((int)player.y / CELL_SIZE) * CELL_SIZE) + CELL_SIZE;
        rayX = player.x + (rayY - player.y) / tan(playerAngle);
        stepY = CELL_SIZE;
        stepX = stepY / tan(playerAngle);
    }
    else
    {
        rayX = player.x;
        rayY = player.y;
        depthOfField = 8;
    }

    while (depthOfField < 8)
    {

        int tileX, tileY;
        tileY = (int)(rayY / 64);
        if (playerAngle > PI) tileY--;
        tileX = (int)(rayX / 64);
        int index = tileY * 8 + tileX;

        if (index >= 0 && index < 8 * 8 && mapp[index] == 1)
        {
            depthOfField = 8;
        }
        else
        {
            rayX += stepX;
            rayY += stepY;
            depthOfField++;
        }
    }
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 0);
    SDL_RenderLine(renderer, player.x + player.w / 2, player.y + player.h / 2, rayX, rayY);
}
