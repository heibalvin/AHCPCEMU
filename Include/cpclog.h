#ifndef CPCLOG_H
#define CPCLOG_H

#include <SDL3/SDL.h>

class CPCLOG {
public:
    // Custom Category Shift Offsets for our Subsystem Architecture Matrix
    enum CategoryOffset {
        CAT_CPU  = SDL_LOG_CATEGORY_CUSTOM + 1,
        CAT_VDP  = SDL_LOG_CATEGORY_CUSTOM + 2,
        CAT_BUS  = SDL_LOG_CATEGORY_CUSTOM + 3,
        CAT_DSK  = SDL_LOG_CATEGORY_CUSTOM + 4
    };

    // Initialize and bind the custom SDL routing callback 
    static void initialize();

private:
    // The underlying SDL-compliant routing callback (must be static to match function pointer expectations)
    static void logOutputCallback(void* userdata, int category, SDL_LogPriority priority, const char* message);
};

// ------------------------------------------------------------------------------
// High-Performance Subsystem Shorthand Macros (Piping directly to the Class Enums)
// ------------------------------------------------------------------------------
#define LOG_CPU(priority, fmt, ...)  SDL_LogMessage(CPCLOG::CAT_CPU, priority, fmt, ##__VA_ARGS__)
#define LOG_VDP(priority, fmt, ...)  SDL_LogMessage(CPCLOG::CAT_VDP, priority, fmt, ##__VA_ARGS__)
#define LOG_BUS(priority, fmt, ...)  SDL_LogMessage(CPCLOG::CAT_BUS, priority, fmt, ##__VA_ARGS__)
#define LOG_DSK(priority, fmt, ...)  SDL_LogMessage(CPCLOG::CAT_DSK, priority, fmt, ##__VA_ARGS__)

#define LOG_EMU(priority, fmt, ...)  SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, priority, fmt, ##__VA_ARGS__)
#define LOG_APP(priority, fmt, ...)  SDL_LogMessage(SDL_LOG_CATEGORY_VIDEO, priority, fmt, ##__VA_ARGS__)
#define LOG_MMI(priority, fmt, ...)  SDL_LogMessage(SDL_LOG_CATEGORY_INPUT, priority, fmt, ##__VA_ARGS__)

#endif // CPCLOG_H