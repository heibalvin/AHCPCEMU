#include "cpcdsk.h"
#include "cpclog.h"
#include "cpcemu.h"

CPCDSK::CPCDSK(CPCEMU& parentEmu) : CPCCOM(parentEmu) {}

CPCDSK::~CPCDSK() {
    powerOff();
}

bool CPCDSK::powerOn() {
    return true;
}

void CPCDSK::powerOff() {
}

void CPCDSK::reset() {
}

bool CPCDSK::loadRom(const Uint8* data, size_t size) {
    if (!data || size < 0x100) {
        LOG_DSK(SDL_LOG_PRIORITY_ERROR, "Target binary array footprint is too small to inspect.");
        return false;
    }

    Uint8 tracks = data[0x30];
    Uint8 sides  = data[0x31];

    LOG_DSK(SDL_LOG_PRIORITY_INFO, "================================================");
    LOG_DSK(SDL_LOG_PRIORITY_INFO, "   AMSTRAD DISK FORMAT GEOMETRY REPORT          ");
    LOG_DSK(SDL_LOG_PRIORITY_INFO, "================================================");
    LOG_DSK(SDL_LOG_PRIORITY_INFO, "Total Physical Tracks: %d", tracks);
    LOG_DSK(SDL_LOG_PRIORITY_INFO, "Total Physical Sides : %d", sides);
    LOG_DSK(SDL_LOG_PRIORITY_INFO, "Sectors per Track    : 9 (Standard AMSDOS Layout)");
    LOG_DSK(SDL_LOG_PRIORITY_INFO, "================================================");

    return true;
}