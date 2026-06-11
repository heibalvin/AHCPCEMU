#include "cpclog.h"
#include "cpcapp.h"

int main(int argc, char* argv[]) {
    CPCLOG::initialize();
    LOG_EMU(SDL_LOG_PRIORITY_INFO, "=== AHCPCPEMU Framework Main Engine Entry ===");

    bool launchHeadless = false;
    for (int i = 1; i < argc; ++i) {
        if (SDL_strcmp(argv[i], "--headless") == 0) {
            launchHeadless = true;
        }
    }

    // CPCAPP is always the entry point, configuring its internal dependencies contextually
    CPCAPP app(launchHeadless);

    if (!app.powerOn()) {
        LOG_EMU(SDL_LOG_PRIORITY_CRITICAL, "Application environment aborted execution.");
        return -1;
    }

    if (launchHeadless) {
        const Uint8 mockDiagnosticPayload[] = { 0xAA, 0xBB, 0xCC, 0xDD };
        app.loadRom(mockDiagnosticPayload, sizeof(mockDiagnosticPayload));
    } else {
        app.loadRom("Resources/OS_6128.ROM");
    }

    app.run();
    app.powerOff();
    return 0;
}