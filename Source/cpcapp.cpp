#include "cpcapp.h"
#include "cpclog.h"

CPCAPP::CPCAPP(bool isHeadless) 
    : window(nullptr), renderer(nullptr), isHeadless(isHeadless), lastTicks(0), emu() {}

CPCAPP::~CPCAPP() {
    powerOff();
}

bool CPCAPP::powerOn() {
    if (isHeadless) {        
        // Initialize SDL with NO subsystems for deterministic unit testing or terminal runs
        if (!SDL_Init(0)) {
            LOG_APP(SDL_LOG_PRIORITY_CRITICAL, "SDL Baseline Headless Init Failure: %s", SDL_GetError());
            return false;
        }
    } else {        
        // Full initialization for standard interactive display window mode
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

    // Power on the core agnostic emulator motherboard layer
    if (!emu.powerOn()) return false;

    return true;
}

void CPCAPP::powerOff() {
    if (!emu.isRunning && !window && !renderer) return;
    
    emu.powerOff();

    if (!isHeadless) {
        if (renderer) { SDL_DestroyRenderer(renderer); renderer = nullptr; }
        if (window)   { SDL_DestroyWindow(window);     window = nullptr; }
    }
    
    SDL_Quit();
}

void CPCAPP::reset() {
    emu.reset();
}

void CPCAPP::step() {
    if (isHeadless) {
        emu.step();
        return;
    }

    Uint64 currentTicks = SDL_GetPerformanceCounter();
    Uint64 elapsedTicks = currentTicks - lastTicks;
    lastTicks = currentTicks;

    double deltaTime = static_cast<double>(elapsedTicks) / static_cast<double>(SDL_GetPerformanceFrequency());
    if (deltaTime > 0.1) deltaTime = 0.1;

    emu.step();
}

void CPCAPP::run() {
    if (isHeadless) {
        // Uniform headless automation limit checkpoint
        for (int i = 0; i < 5; ++i) {
            if (!emu.isRunning) break;
            step();
        }
        emu.isRunning = false;
        return;
    }

    SDL_Event event;
    while (emu.isRunning) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                emu.isRunning = false;
            }
        }

        if (!emu.isRunning) break;

        step();

        SDL_SetRenderDrawColor(renderer, 0, 0, 128, 255); // Amstrad Cyan/Blue
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }
}

// ======================================================================
// PURE FRONTEND RAW ARRAY TRANSLATORS (Driven via SDL_LoadFile)
// ======================================================================
bool CPCAPP::loadRom(const char* filepath, bool bootstrap) {
    if (!filepath) return false;

    size_t fileSize = 0;
    void* rawBuffer = SDL_LoadFile(filepath, &fileSize);
    if (!rawBuffer) {
        LOG_APP(SDL_LOG_PRIORITY_ERROR, "Failed to load ROM: %s", filepath);
        return false;
    }

    // Delegate the injection to the core
    bool status = emu.loadRom(static_cast<const Uint8*>(rawBuffer), fileSize, bootstrap);
    
    SDL_free(rawBuffer);
    return status;
}

bool CPCAPP::loadDisk(const char* filepath) {
    if (!filepath) return false;

    size_t fileSize = 0;
    void* rawBuffer = SDL_LoadFile(filepath, &fileSize);
    if (!rawBuffer) {
        LOG_APP(SDL_LOG_PRIORITY_ERROR, "Failed to read disk image payload: %s", filepath);
        return false;
    }

    bool status = emu.loadDiskArray(reinterpret_cast<const Uint8*>(rawBuffer), fileSize);
    SDL_free(rawBuffer);
    return status;
}