#ifndef CPCAPP_H
#define CPCAPP_H

#include <SDL3/SDL.h>
#include "cpcemu.h"

class CPCAPP {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;
    bool isHeadless; // The engine configuration toggle
    
    Uint64 lastTicks;
    CPCEMU emu;

public:
    // Defaults to false (Standard Interactive UI Mode)
    explicit CPCAPP(bool isHeadless = false);
    ~CPCAPP();

    bool powerOn();
    void powerOff();
    void reset();

    void step();
    void run();

    // Standardized file-handling wrapper
    bool loadRom(const char* filepath);
    bool loadRom(const Uint8* data, size_t size);
};

#endif // CPCAPP_H