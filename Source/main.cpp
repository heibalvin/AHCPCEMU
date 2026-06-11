#include "cpclog.h"
#include "cpcemu.h"

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;

    // Initialize the Static Logging Engine Class
    CPCLOG::initialize();

    LOG_EMU(SDL_LOG_PRIORITY_INFO, "=== AHCPCPEMU OBJECT REFACTOR BOOT ===");
    LOG_VDP(SDL_LOG_PRIORITY_DEBUG, "VDP register 0x01 initialized to 50Hz mode refresh parameters.");

    return 0;
}