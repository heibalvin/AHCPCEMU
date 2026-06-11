#ifndef CPCAPP_H
#define CPCAPP_H

#include <SDL3/SDL.h>

class CPCAPP {
public:
    CPCAPP();
    ~CPCAPP();

    // Re-mapped hardware initialization and safety breakdown pathways
    bool powerOn();
    void powerOff();
    
    // Core iterative framework runners
    void run();
    void step();

private:
    void processEvents();
    void render();

    SDL_Window* window;
    SDL_Renderer* renderer;
    
    bool isRunning;
    Uint64 lastTimeTicks; 
};

#endif // CPCAPP_H