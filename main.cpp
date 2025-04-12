#include <iostream>
#include <SDL3/SDL.h>
#include "tracer.h"
#include "map.h"

#define WINDOW_W 1024
#define WINDOW_H 512

Tracer tracer(WINDOW_W, WINDOW_H);

int main()
{

    if (!tracer.init("RayCaster"))
    {
        return -1;
    }

    while (tracer.running())
    {
        tracer.handelEvent();
        tracer.update();
        tracer.render();
        tracer.renderMap();
    }
    SDL_Delay(16);
    tracer.clean();

    return 0;
}