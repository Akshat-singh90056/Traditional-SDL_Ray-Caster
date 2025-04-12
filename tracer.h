#ifndef TRACER_H
#define TRACER_H
#include <SDL3/SDL.h>
#include <iostream>
#include <math.h>
#include <map>

class Tracer
{
public:
    Tracer(int W_W, int W_H);
    ~Tracer();

    bool init(const char *title);
    void handelEvent();
    void render();
    void renderPlayer();
    void renderMap();
    void usingMouse();
    void rayCaster();
    void update();
    void clean();
    bool running()
    {
        return isrunning;
    }

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    int WINDOW_WIDTH;
    int WINDOW_HEIGHT;
    bool isrunning;

    SDL_FRect player;
};

#endif