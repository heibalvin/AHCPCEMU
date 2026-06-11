#include "cpclog.h"

void CPCLOG::initialize() {
    // Pass the static class method directly to the SDL3 routing layer
    SDL_SetLogOutputFunction(CPCLOG::logOutputCallback, nullptr);

    // Set engine visibility defaults to capture everything from debug up
    SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG);
    SDL_SetLogPriority(SDL_LOG_CATEGORY_VIDEO,       SDL_LOG_PRIORITY_DEBUG);
    SDL_SetLogPriority(SDL_LOG_CATEGORY_INPUT,       SDL_LOG_PRIORITY_DEBUG);
    SDL_SetLogPriority(CPCLOG::CAT_CPU,              SDL_LOG_PRIORITY_DEBUG);
    SDL_SetLogPriority(CPCLOG::CAT_VDP,              SDL_LOG_PRIORITY_DEBUG);
    SDL_SetLogPriority(CPCLOG::CAT_BUS,              SDL_LOG_PRIORITY_DEBUG);
    SDL_SetLogPriority(CPCLOG::CAT_DSK,              SDL_LOG_PRIORITY_DEBUG);
}

void CPCLOG::logOutputCallback(void* userdata, int category, SDL_LogPriority priority, const char* message) {
    (void)userdata;

    // 1. Map Severity Literals
    const char* severity = "Log";
    switch (priority) {
        case SDL_LOG_PRIORITY_VERBOSE:  severity = "Verbose";  break;
        case SDL_LOG_PRIORITY_DEBUG:    severity = "Debug";    break;
        case SDL_LOG_PRIORITY_INFO:     severity = "Info";     break;
        case SDL_LOG_PRIORITY_WARN:     severity = "Warn";     break;
        case SDL_LOG_PRIORITY_ERROR:    severity = "Error";    break;
        case SDL_LOG_PRIORITY_CRITICAL: severity = "Critical"; break;
        default:                        severity = "Unknown";  break;
    }

    // 2. Map Architectural Subsystem Tags
    const char* subsystem = "CPC_UNKNOWN";
    switch (category) {
        case SDL_LOG_CATEGORY_APPLICATION: subsystem = "CPC_EMU";    break;
        case SDL_LOG_CATEGORY_VIDEO:       subsystem = "CPC_APP";    break;
        case SDL_LOG_CATEGORY_INPUT:       subsystem = "CPC_MMI";    break;
        case CPCLOG::CAT_CPU:              subsystem = "CPC_CPU";    break;
        case CPCLOG::CAT_VDP:              subsystem = "CPC_VDP";    break;
        case CPCLOG::CAT_BUS:              subsystem = "CPC_BUS";    break;
        case CPCLOG::CAT_DSK:              subsystem = "CPC_DSK";    break;
        default:                            subsystem = "SDL_SYSTEM"; break;
    }

    // 3. Performance stack buffer format updated to: <subsystem>(<severity>): <message>
    char logBuffer[1024];
    int written = SDL_snprintf(logBuffer, sizeof(logBuffer), 
                               "%s(%s): %s", 
                               subsystem, severity, message);

    // 4. Safely pipe directly to SDL's default platform output handler 
    if (written > 0) {
        SDL_LogOutputFunction defaultLogFunc = SDL_GetDefaultLogOutputFunction();
        if (defaultLogFunc) {
            defaultLogFunc(nullptr, category, priority, logBuffer);
        }
    }
}