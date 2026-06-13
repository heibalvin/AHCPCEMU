#include "cpcapp.h"
#include <SDL3/SDL.h>

int main(int argc, char* argv[]) {
    // 1. Parse command line parameters for headless execution state configurations
    bool startHeadless = false;
    for (int i = 1; i < argc; i++) {
        if (SDL_strcmp(argv[i], "--headless") == 0) {
            startHeadless = true;
            break;
        }
    }

    // 2. Instantiate your frontend application layer on stack passing configuration
    CPCAPP app(startHeadless);

    // 3. Fire up host windows or baseline subsystems
    if (!app.powerOn()) {
        return -1; // Graceful failure escape route if initialization breaks
    }

    SDL_SetLogPriorities(SDL_LOG_PRIORITY_DEBUG);

    // Optional workspace loads can safely happen here:
    // app.loadDisk("Resources/Sorcery (UK) (1985).dsk");
    if (!app.loadRom("Resources/ZEXALL.COM", true)) {
        printf("FATAL: Could not load ZEXALL.COM\n");
        return -1;
    }

    // 4. Run the master simulation execution loop
    app.run();

    // 5. Tear down host resources cleanly
    app.powerOff();

    return 0;
}