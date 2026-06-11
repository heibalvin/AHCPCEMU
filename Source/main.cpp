#include "cpclog.h"
#include "cpcapp.h"

int main(int argc, char* argv[]) {
    CPCLOG::initialize();

    bool launchHeadless = false;
    for (int i = 1; i < argc; ++i) {
        if (SDL_strcmp(argv[i], "--headless") == 0) {
            launchHeadless = true;
        }
    }

    CPCAPP app(launchHeadless);
    if (!app.powerOn()) {
        LOG_EMU(SDL_LOG_PRIORITY_CRITICAL, "Application environment aborted execution.");
        return -1;
    }

    app.loadRom("Resources/Sorcery (UK) (1985).dsk");

    // app.run();
    app.powerOff();
    return 0;
}