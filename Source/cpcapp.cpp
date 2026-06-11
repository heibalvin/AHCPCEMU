#include "cpcapp.h"
#include "cpclog.h"

CPCAPP::CPCAPP(bool isHeadless) 
    : window(nullptr), renderer(nullptr), isRunning(false), isHeadless(isHeadless), lastTicks(0), emu() {}

CPCAPP::~CPCAPP() {
    powerOff();
}

bool CPCAPP::powerOn() {
    if (isHeadless) {        
        // Initialize SDL with NO subsystems. 
        // This gives us File I/O and Core Utilities without spinning up window managers.
        if (!SDL_Init(0)) {
            LOG_APP(SDL_LOG_PRIORITY_CRITICAL, "SDL Baseline Headless Init Failure: %s", SDL_GetError());
            return false;
        }
    } else {        
        // Full initialization for interactive GUI mode
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
            LOG_APP(SDL_LOG_PRIORITY_CRITICAL, "SDL Graphical Init Failure: %s", SDL_GetError());
            return false;
        }

        window = SDL_CreateWindow("AHCPCPEMU - Amstrad CPC Emulator", 800, 600, 0);
        if (!window) return false;

        renderer = SDL_CreateRenderer(window, nullptr);
        if (!renderer) return false;

        lastTicks = SDL_GetPerformanceCounter();
    }

    // Power on the core emulator motherboard layer in both modes
    if (!emu.powerOn()) return false;

    isRunning = true;
    return true;
}

void CPCAPP::powerOff() {
    if (!isRunning) return;
    isRunning = false;
    
    emu.powerOff();

    if (!isHeadless) {
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window)   SDL_DestroyWindow(window);
    }
    
    SDL_Quit(); // Safely closes down whichever initialization matrix was spun up
}

void CPCAPP::reset() {
    emu.reset();
}

void CPCAPP::step() {
    if (isHeadless) {
        // Headless simulation steps forward by a uniform 16.67ms frame clock slice (60Hz)
        emu.step(0.016666);
        return;
    }

    Uint64 currentTicks = SDL_GetPerformanceCounter();
    Uint64 elapsedTicks = currentTicks - lastTicks;
    lastTicks = currentTicks;

    double deltaTime = static_cast<double>(elapsedTicks) / static_cast<double>(SDL_GetPerformanceFrequency());
    if (deltaTime > 0.1) deltaTime = 0.1;

    emu.step(deltaTime);
}

void CPCAPP::run() {
    LOG_APP(SDL_LOG_PRIORITY_INFO, "Entering emulation loop execution state.");

    if (isHeadless) {
        // Run a quick deterministic execution cycle check, then exit automatically
        for (int i = 0; i < 5; ++i) {
            step();
        }
        isRunning = false;
        return;
    }

    SDL_Event event;
    while (isRunning) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                isRunning = false;
            }
        }

        step();

        SDL_SetRenderDrawColor(renderer, 0, 0, 128, 255); // Amstrad Blue
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }
}

bool CPCAPP::loadRom(const char* filepath) {
    if (!filepath) return false;

    // This native SDL function works perfectly under SDL_Init(0)!
    size_t fileSize = 0;
    void* rawBuffer = SDL_LoadFile(filepath, &fileSize);
    if (!rawBuffer) {
        LOG_APP(SDL_LOG_PRIORITY_ERROR, "Failed to load payload file.");
        return false;
    }

    bool status = emu.loadRom(reinterpret_cast<const Uint8*>(rawBuffer), fileSize);
    SDL_free(rawBuffer);
    return status;
}

bool CPCAPP::loadRom(const Uint8* data, size_t size) {
    return emu.loadRom(data, size);
}