#include "cpcemu.h"
#include "cpclog.h"

CPCEMU::CPCEMU() : totalSystemCycles(0), dsk(*this) {}

CPCEMU::~CPCEMU() {
    // Standard safety fallback path
    powerOff();
}

bool CPCEMU::powerOn() {
    totalSystemCycles = 0;

    // Propagate down the hardware trace path
    if (!dsk.powerOn()) {
        LOG_EMU(SDL_LOG_PRIORITY_CRITICAL, "Motherboard Boot Failure: DSK component powerOn failed.");
        return false;
    }

    reset();
    return true;
}

void CPCEMU::powerOff() {
    dsk.powerOff();
}

void CPCEMU::reset() {
    totalSystemCycles = 0;
    
    dsk.reset();
}

void CPCEMU::step(double deltaTime) {
    // 4.0 MHz classic Amstrad timing calculations
    Uint32 cyclesToRun = static_cast<Uint32>(deltaTime * 4000000.0);
    totalSystemCycles += cyclesToRun;

}

bool CPCEMU::loadRom(const Uint8* data, size_t size) {
    return dsk.loadRom(data, size);
}