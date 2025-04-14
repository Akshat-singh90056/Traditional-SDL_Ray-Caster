#include "tracer.h"
#include "map.h"

#define PLAYER_SIZE 20
#define PLAYER_SPEED 5
#define LINE_LENGTH 1500
#define PI 3.14159
#define FOV (PI / 3)
#define STEP_SIZE 2.0f
#define NUMBER_RAYS 120

float PLAYER_ANGLE = 0.0f;

Tracer::Tracer(int W_W, int W_H)
{
    WINDOW_HEIGHT = W_H;
    WINDOW_WIDTH = W_W;
}

Tracer::~Tracer() {}

bool Tracer::init(const char *title)
{
    if (SDL_Init(SDL_INIT_VIDEO) == 0)
    {
        return false;
    }

    window = SDL_CreateWindow(title, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        return false;
    }

    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer)
    {
        return false;
    }

    player = {(float)(WINDOW_WIDTH / 4 + PLAYER_SIZE / 2), (float)(WINDOW_HEIGHT / 2 + PLAYER_SIZE / 2), PLAYER_SIZE, PLAYER_SIZE};
    isrunning = true;
    return true;
}

void Tracer::handelEvent()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
        {
            isrunning = false;
        }
        else if (event.type == SDL_EVENT_MOUSE_MOTION)
        {
            usingMouse();
        }
        else if (event.type == SDL_EVENT_KEY_DOWN)
        {
            float moveX = 0.0f;
            float moveY = 0.0f;

            switch (event.key.key)
            {
            case SDLK_W:
                moveX = cos(PLAYER_ANGLE) * PLAYER_SPEED;
                moveY = sin(PLAYER_ANGLE) * PLAYER_SPEED;
                break;

            case SDLK_S:
                moveX = -cos(PLAYER_ANGLE) * PLAYER_SPEED;
                moveY = -sin(PLAYER_ANGLE) * PLAYER_SPEED;
                break;
            }

            float nextX = player.x + moveX;
            float nextY = player.y + moveY;

            float radius = PLAYER_SIZE / 2.0f;

            float centerX = nextX + PLAYER_SIZE / 2.0f;
            float centerY = nextY + PLAYER_SIZE / 2.0f;

            if (!isWall(centerX, player.y + PLAYER_SIZE / 2.0f, radius))
            {
                player.x = nextX;
            }

            if (!isWall(player.x + PLAYER_SIZE / 2.0f, centerY, radius))
            {
                player.y = nextY;
            }
        }
    }
}

void Tracer::update() {}

void Tracer::renderPlayer()
{
    SDL_SetRenderDrawColor(renderer, 255, 225, 225, 255);
    SDL_RenderFillRect(renderer, &player);
}

void Tracer::usingMouse()
{
    float mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    float deltaX = mouseX - (player.x + player.w / 2);
    float deltaY = mouseY - (player.y + player.h / 2);

    PLAYER_ANGLE = atan2(deltaY, deltaX);
    if (PLAYER_ANGLE < 0)
    {
        PLAYER_ANGLE += 2 * PI;
    }
}

void Tracer::render()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    renderMap();
    rayCaster();
    renderPlayer();

    SDL_RenderPresent(renderer);
}

void Tracer::renderMap()
{
    for (int i = 0; i < MAP_H * CELL_SIZE; i += CELL_SIZE)
    {
        for (int j = 0; j < MAP_W * CELL_SIZE; j += CELL_SIZE)
        {
            int indexX = i / CELL_SIZE;
            int indexY = j / CELL_SIZE;

            SDL_FRect grid = {
                static_cast<float>(j),
                static_cast<float>(i),
                static_cast<float>(CELL_SIZE),
                static_cast<float>(CELL_SIZE)};

            if (map[indexX][indexY] == 1)
            {
                SDL_SetRenderDrawColor(renderer, 0, 180, 255, 255);
                SDL_RenderFillRect(renderer, &grid);
            }
            else if (map[indexX][indexY] == 2)
            {
                SDL_SetRenderDrawColor(renderer, 255, 51, 153, 255);
                SDL_RenderFillRect(renderer, &grid);
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, 10, 10, 30, 255);
                SDL_RenderFillRect(renderer, &grid);
            }

            SDL_SetRenderDrawColor(renderer, 40, 40, 80, 100);
            SDL_RenderRect(renderer, &grid);
        }
    }

    SDL_SetRenderDrawColor(renderer, 0, 255, 100, 255);
    SDL_RenderFillRect(renderer, &player);
}

void Tracer::rayCaster()
{
    float startX = player.x + player.w / 2;
    float startY = player.y + player.h / 2;

    float rayAngle = PLAYER_ANGLE - (FOV / 2);

    for (int i = 0; i < NUMBER_RAYS; i++)
    {
        float rayX = startX;
        float rayY = startY;

        float rayCos = cos(rayAngle);
        float raySin = sin(rayAngle);

        float distance = 0.0f;
        bool collision = false;

        for (int j = 0; j < LINE_LENGTH; j++)
        {
            rayX += rayCos * STEP_SIZE;
            rayY += raySin * STEP_SIZE;
            distance += STEP_SIZE;

            int mapX = static_cast<int>(rayX) / CELL_SIZE;
            int mapY = static_cast<int>(rayY) / CELL_SIZE;

            if (mapX >= 0 && mapX < MAP_W && mapY >= 0 && mapY < MAP_H)
            {
                if (map[mapY][mapX] > 0)
                {
                    collision = true;
                    break;
                }
            }
            else
            {
                break;
            }
        }

        // 2D Ray Line
        if (collision)
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);
            SDL_RenderLine(renderer, startX, startY, rayX, rayY);
        }

        // ------ 3D Wall Projection ------

        // Fish-eye correction
        float correctedDistance = distance * cos(rayAngle - PLAYER_ANGLE);

        float distanceToProjectionPlane = (WINDOW_WIDTH / 2) / tan(FOV / 2);
        float wallHeight = (CELL_SIZE / correctedDistance) * distanceToProjectionPlane;

        // Choose wall color based on distance
        Uint8 brightness = (Uint8)(255.0f / (1.0f + correctedDistance * 0.01f));
        SDL_SetRenderDrawColor(renderer, brightness, brightness, brightness, 255);

        // Calculate x and y for the 3D wall
        int columnWidth = (WINDOW_WIDTH / 2) / NUMBER_RAYS;
        int columnX = (WINDOW_WIDTH / 2) + (i * columnWidth); // Start after 2D map
        int wallTop = (WINDOW_HEIGHT / 2) - (wallHeight / 2);
        int wallBottom = wallTop + wallHeight;

        SDL_RenderLine(renderer, columnX, wallTop, columnX, wallBottom);

        rayAngle += FOV / NUMBER_RAYS;
    }
}

bool Tracer::isWall(float x, float y, float radius)
{
    float checkPoints[4][2] = {
        {x + radius, y},
        {x - radius, y},
        {x, y + radius},
        {x, y - radius}};

    for (int i = 0; i < 4; i++)
    {
        int mapX = static_cast<int>(checkPoints[i][0]) / CELL_SIZE;
        int mapY = static_cast<int>(checkPoints[i][1]) / CELL_SIZE;

        if (mapX < 0 || mapX >= MAP_W || mapY < 0 || mapY >= MAP_H)
            return true;

        if (map[mapY][mapX] > 0)
            return true;
    }

    return false;
}

void Tracer::clean()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
