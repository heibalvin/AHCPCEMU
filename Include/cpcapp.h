#ifndef CPCAPP_H
#define CPCAPP_H

#include <SDL3/SDL.h>
#include "cpcemu.h"

class CPCAPP {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool          isHeadless; // Keeps the local frontend environment flag context
    Uint64        lastTicks;
    CPCEMU        emu;

public:
    explicit CPCAPP(bool isHeadless = false);
    ~CPCAPP();

    bool powerOn();
    void powerOff();
    void reset();

    void step();
    void run();

    bool loadRom(const char* filepath, bool bootstrap = false);
    bool loadDisk(const char* filepath);

    CPCEMU& getEmu() { return emu; }
};

#endif // CPCAPP_H