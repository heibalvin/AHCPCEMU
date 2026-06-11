#include "cpcapp.h"

CPCAPP::CPCAPP() : window(nullptr), renderer(nullptr), isRunning(false), lastTimeTicks(0) {}

CPCAPP::~CPCAPP() {
    powerOff();
}

bool CPCAPP::powerOn() {
    // 1. Core SDL3 Subsystem Initialization (Video Events Only)
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL Core Failure: %s", SDL_GetError());
        return false;
    }

    // 2. Instantiate high-DPI host window border layer
    window = SDL_CreateWindow(
        "AHCPCPEMU (Amstrad CPC Hardware Framework)", 
        640, 400, 
        SDL_WINDOW_HIGH_PIXEL_DENSITY
    );
    if (!window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window Spawn Failure: %s", SDL_GetError());
        return false;
    }

    // 3. Bind an accelerated, vertical sync-enabled hardware rendering device 
    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer Creation Failure: %s", SDL_GetError());
        return false;
    }

    // Ensure VSync is explicitly active to lock loops cleanly to host frame updates
    SDL_SetRenderVSync(renderer, 1);

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "SDL3 Structural Matrix Powered On successfully.");
    return true;
}

void CPCAPP::processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            isRunning = false;
        }
        else if (event.type == SDL_EVENT_KEY_DOWN) {
            // Hotkey Escape Exit Bind
            if (event.key.key == SDLK_ESCAPE) {
                isRunning = false;
            }
        }
    }
}

void CPCAPP::render() {
    // Solid Amstrad CPC Classic System Blue
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x80, 0xFF);
    SDL_RenderClear(renderer);

    // Present the back buffer directly to the window layer
    SDL_RenderPresent(renderer);
}

void CPCAPP::step() {
    processEvents();

    // Calculate real elapsed time changes for timing-matched cycle processing down the line
    Uint64 currentTimeTicks = SDL_GetPerformanceCounter();
    Uint64 deltaTicks = currentTimeTicks - lastTimeTicks;
    lastTimeTicks = currentTimeTicks;

    double deltaTime = static_cast<double>(deltaTicks) / static_cast<double>(SDL_GetPerformanceFrequency());
    (void)deltaTime; 

    render();
}

void CPCAPP::run() {
    lastTimeTicks = SDL_GetPerformanceCounter();
    isRunning = true;

    while (isRunning) {
        step();
    }
}

void CPCAPP::powerOff() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "System subsystems cleanly Powered Off.");
}