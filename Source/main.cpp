#include "cpclog.h"
#include "cpcapp.h"
#include "cpcemu.h"

int main(int argc, char* argv[]) {
    // 1. INITIALIZE High-Level Host Logging Architecture
    CPCLOG::initialize();

    bool launchHeadless = false;
    for (int i = 1; i < argc; ++i) {
        if (SDL_strcmp(argv[i], "--headless") == 0) {
            launchHeadless = true;
        }
    }

    // Instantiate your application framework container layer
    CPCAPP app(launchHeadless);

    // 2. POWER ON Subsystems (Fires SDL_Init for OS resource tracking)
    if (!app.powerOn()) {
        LOG_APP(SDL_LOG_PRIORITY_CRITICAL, "Application environment aborted execution during powerOn.");
        return -1;
    }

    // 3. LOAD REAL GAME ASSET (Routes downward to CPCDSK file-system layout blocks)
    // Make sure the space formatting matches your local file configuration perfectly
    app.loadRom("Resources/Sorcery (UK) (1985).dsk");

    // 4. EXECUTE EXACTLY ONE CLOCK STEP SWEEP
    // The CPU looks at physical reset vector location 0x0000, reads the bus state, 
    // and shifts the Program Counter forward.
    app.getEmu().step(1.0);

    // 5. POWER OFF Subsystems Cleanly
    app.powerOff();

    return 0;
}