#include "cpcapp.h"
#include "cpclog.h"

CPCAPP::CPCAPP() : window(nullptr), renderer(nullptr), isRunning(false) {}

CPCAPP::~CPCAPP() {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
    LOG_APP(SDL_LOG_PRIORITY_INFO, "Host OS SDL3 Subsystems successfully terminated.");
}

bool CPCAPP::powerOn() {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        LOG_APP(SDL_LOG_PRIORITY_CRITICAL, "SDL_Init Subsystem Failure: %s", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("AHCPCPEMU - Amstrad CPC Emulator", 800, 600, 0);
    if (!window) {
        LOG_APP(SDL_LOG_PRIORITY_CRITICAL, "Failed to instantiate window viewport: %s", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        LOG_APP(SDL_LOG_PRIORITY_CRITICAL, "Failed to instantiate hardware accelerated renderer: %s", SDL_GetError());
        return false;
    }

    isRunning = true;
    LOG_APP(SDL_LOG_PRIORITY_INFO, "poweron() successful - Host Window and Graphics Pipeline linked.");
    return true;
}

void CPCAPP::run() {
    SDL_Event event;
    LOG_APP(SDL_LOG_PRIORITY_INFO, "Entering interactive emulation loop execution state.");

    while (isRunning) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                LOG_MMI(SDL_LOG_PRIORITY_INFO, "Quit command intercept triggered via Host window OS manager.");
                isRunning = false;
            }
        }

        // Mock frame render pass
        SDL_SetRenderDrawColor(renderer, 0, 0, 128, 255); // Classic Amstrad Blue background
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }
}