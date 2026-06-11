#include "cpclog.h"
#include "cpcapp.h"

int main(int argc, char* argv[]) {
    (void)argc; 
    (void)argv;

    // Boot the unified static object-oriented logging framework
    CPCLOG::initialize();

    LOG_APP(SDL_LOG_PRIORITY_INFO, "Launching Amstrad CPC Graphical Application Shell...");

    CPCAPP app;
    if (!app.powerOn()) {
        LOG_APP(SDL_LOG_PRIORITY_CRITICAL, "Application powerOn sequence aborted execution.");
        return -1;
    }

    app.run();
    return 0;
}