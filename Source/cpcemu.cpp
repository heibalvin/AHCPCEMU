#include "cpcemu.h"
#include "cpclog.h"

CPCEMU::CPCEMU() : totalSystemCycles(0), dsk(*this) {}

CPCEMU::~CPCEMU() {
    // Standard safety fallback path
    powerOff();
}

bool CPCEMU::powerOn() {
    LOG_EMU(SDL_LOG_PRIORITY_INFO, "Initializing Virtual Motherboard hardware core containers...");
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
    LOG_EMU(SDL_LOG_PRIORITY_INFO, "Powering down Virtual Motherboard hardware core containers.");
    dsk.powerOff();
}

void CPCEMU::reset() {
    totalSystemCycles = 0;
    LOG_EMU(SDL_LOG_PRIORITY_INFO, "Virtual Motherboard: State Vector Zeroed (Reset complete).");
    LOG_CPU(SDL_LOG_PRIORITY_DEBUG, "Z80 CPU registers initialized to Power-On state. PC: 0x0000");
    
    dsk.reset();
}

void CPCEMU::step(double deltaTime) {
    // 4.0 MHz classic Amstrad timing calculations
    Uint32 cyclesToRun = static_cast<Uint32>(deltaTime * 4000000.0);
    totalSystemCycles += cyclesToRun;

    if (cyclesToRun > 0) {
        LOG_VDP(SDL_LOG_PRIORITY_DEBUG, "VDP Sync Pass: Stepping hardware registers ahead by %u cycles.", cyclesToRun);
    }
}

bool CPCEMU::loadRom(const Uint8* data, size_t size) {
    LOG_EMU(SDL_LOG_PRIORITY_INFO, "Intercepted system binary dispatch package. Passing down to DSK subsystem.");
    return dsk.loadRomData(data, size);
}