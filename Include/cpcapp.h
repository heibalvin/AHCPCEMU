#ifndef CPCAPP_H
#define CPCAPP_H

#include <SDL3/SDL.h>
#include "cpcemu.h"

class CPCAPP {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
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

    // CRITICAL FIX: Expose the emulator core to main.cpp
    CPCEMU& getEmu() { return emu; }
};

#endif // CPCAPP_H