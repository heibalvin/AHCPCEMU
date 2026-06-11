#include "cpcemu.h"

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    // Set logging to show information blocks in the terminal
    SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "=== AHCPCPEMU HEADLESS DIAGNOSTIC CORE ===");

    CPCEMU headlessEmu;
    
    // Simulate running 3 frames of hardware cycles (60Hz timing slice)
    double targetFrameTime = 0.016666; 
    
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Stepping virtual motherboard sequentially...");
    for (int i = 1; i <= 3; ++i) {
        headlessEmu.update(targetFrameTime);
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, " [Cycle Step %d] Total Motherboard Clocks: %llu", 
                    i, headlessEmu.getExecutedCycles());
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Headless verification loop complete.");
    return 0;
}