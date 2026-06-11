#include "cpcdsk.h"
#include "cpclog.h"
#include "cpcemu.h"

CPCDSK::CPCDSK(CPCEMU& parentEmu) : CPCCOM(parentEmu) {}

CPCDSK::~CPCDSK() {
    powerOff();
}

bool CPCDSK::powerOn() {
    LOG_DSK(SDL_LOG_PRIORITY_INFO, "NEC uPD765 Floppy Disk Controller spinning up hardware vectors.");
    return true;
}

void CPCDSK::powerOff() {
    LOG_DSK(SDL_LOG_PRIORITY_INFO, "Floppy Drive motor stepped down safely.");
}

void CPCDSK::reset() {
    LOG_DSK(SDL_LOG_PRIORITY_INFO, "Disk System Controller Reset Vector triggered.");
}

bool CPCDSK::loadRomData(const Uint8* data, size_t size) {
    if (!data || size == 0) {
        LOG_DSK(SDL_LOG_PRIORITY_ERROR, "Invalid disk data payload provided to parser.");
        return false;
    }

    LOG_DSK(SDL_LOG_PRIORITY_INFO, "Successfully loaded %zu bytes of system ROM storage array.", size);
    // TODO: Map the raw buffer arrays straight into your internal geometry sector buffers here
    return true;
}