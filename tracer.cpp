#include "tracer.h"
#include "map.h"

#define PLAYER_SIZE 20
#define PLAYER_SPEED 5
#define LINE_LENGTH 1500
#define PI 3.14159

float PLAYER_ANGLE = 0.0f;

Tracer::Tracer(int W_W, int W_H)
{
    WINDOW_HEIGHT = W_H;
    WINDOW_WIDTH = W_W;
}

Tracer ::~Tracer() {}

bool Tracer::init(const char *title)
{

    if (SDL_Init(SDL_INIT_VIDEO) == 0)
    {
        return -1;
    }

    window = SDL_CreateWindow(title, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        return -1;
    }
    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer)
    {
        return -1;
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
            switch (event.key.key)
            {
            case SDLK_D:
                player.x += PLAYER_SPEED;
                break;
            case SDLK_A:
                player.x -= PLAYER_SPEED;
                break;
            case SDLK_W:
                player.y -= PLAYER_SPEED;
                break;
            case SDLK_S:
                player.y += PLAYER_SPEED;
                break;
            }
        }
    }
}

void Tracer::update()
{
}

void Tracer::renderPlayer()
{
    SDL_SetRenderDrawColor(renderer, 255, 225, 225, 255); // Player color
    SDL_RenderFillRect(renderer, &player);

    // Render direction line
    float startX = player.x + player.w / 2;
    float startY = player.y + player.h / 2;
    float endX = startX + cos(PLAYER_ANGLE) * LINE_LENGTH;
    float endY = startY + sin(PLAYER_ANGLE) * LINE_LENGTH;

    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Direction line color
    SDL_RenderLine(renderer, startX, startY, endX, endY);
}

void Tracer::usingMouse()
{
    float mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    if (mouseX >= 0 && mouseX <= 512 && mouseY >= 0 && mouseY <= 512)
    {
        float deltaX = mouseX - (player.x + player.w / 2);
        float deltaY = mouseY - (player.y + player.h / 2);

        PLAYER_ANGLE = atan2(deltaY, deltaX);
        if (PLAYER_ANGLE < 0)
        {
            PLAYER_ANGLE += 2 * PI;
        }
        std::cout << PLAYER_ANGLE << std::endl;
    }
}

void Tracer::render()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    renderMap();

    renderPlayer();
    usingMouse();

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
                // 💙 Cool Neon Blue Walls
                SDL_SetRenderDrawColor(renderer, 0, 180, 255, 255);
                SDL_RenderFillRect(renderer, &grid);
            }
            else if (map[indexX][indexY] == 2)
            {
                // 🔮 Radiant Magenta Highlights
                SDL_SetRenderDrawColor(renderer, 255, 51, 153, 255);
                SDL_RenderFillRect(renderer, &grid);
            }
            else if (map[indexX][indexY] == 0)
            {
                // 🌌 Soft Midnight Floor
                SDL_SetRenderDrawColor(renderer, 10, 10, 30, 255);
                SDL_RenderFillRect(renderer, &grid);
            }

            // ✨ Luminous Grid Glow (very subtle, stylish)
            SDL_SetRenderDrawColor(renderer, 40, 40, 80, 100);
            SDL_RenderRect(renderer, &grid);
        }
    }

    // 🟢 Player Highlight
    SDL_SetRenderDrawColor(renderer, 0, 255, 100, 255);
    SDL_RenderFillRect(renderer, &player);
}

void Tracer::rayCaster()
{
}

void Tracer::clean()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}