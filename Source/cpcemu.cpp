#include "cpcemu.h"

CPCEMU::CPCEMU() : totalSystemCycles(0) {
    systemReset();
}

CPCEMU::~CPCEMU() {}

void CPCEMU::systemReset() {
    totalSystemCycles = 0;
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Virtual Motherboard: State Vector Zeroed (Reset complete).");
}

void CPCEMU::update(double deltaTime) {
    // 4 MHz Amstrad CPC timing = 4,000,000 cycles per second
    Uint32 cyclesToRun = static_cast<Uint32>(deltaTime * 4000000.0);
    
    // Increment the absolute master clock counter
    totalSystemCycles += cyclesToRun;
    
    if (cyclesToRun > 0) {
        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "EMU Step: Simulated %u hardware cycles.", cyclesToRun);
    }
}