#include "cpcemu.h"
#include "cpclog.h"

CPCEMU::CPCEMU() : totalSystemCycles(0), bus(*this), dsk(*this), cpu(*this) {}

CPCEMU::~CPCEMU() {
    // Standard safety fallback path
    powerOff();
}

bool CPCEMU::powerOn() {
    if (isRunning) return true; // Already active, bail early smoothly
    
    if (!bus.powerOn()) return false;
    if (!dsk.powerOn()) return false;
    if (!cpu.powerOn()) return false;

    isRunning = true; // State locked to active
    return true;
}

void CPCEMU::powerOff() {
    if (!isRunning) return; // Prevent duplicate teardown loop prints cleanly

    cpu.powerOff();
    dsk.powerOff();
    bus.powerOff();

    isRunning = false; // State reset to offline
}

void CPCEMU::reset() {
    bus.reset();
    cpu.reset();
}

void CPCEMU::step(double deltaTime) {
    (void)deltaTime;
    
    // Step the CPU execution cycles forward
    cpu.step();
}

bool CPCEMU::loadRom(const Uint8* data, size_t size) {
    if (!data || size == 0) return false;

    if (size == 16384) {
        bus.injectLowerRom(data, size);
        return true;
    }

    // DISK HACK: Load disk geometry normally
    dsk.loadRom(data, size);

    // Force copy the very first raw sector of the disk (skipping the 256-byte DSK header)
    // into RAM Bank 0 so the CPU has something immediate to read.
    Uint16 visualResetVector = 0x0100; 
    for (size_t i = 0; i < 512; ++i) {
        bus.writeByte(visualResetVector + i, data[0x100 + i]);
    }

    // Manually force the CPU Program Counter to bypass the boot ROM 
    // and jump straight to our injected disk data!
    bus.setRomStates(false, false);
    cpu.PC = visualResetVector; 

    LOG_EMU(SDL_LOG_PRIORITY_INFO, "Developer Hack: Bypassed FDC. Forced PC to 0x%04X", visualResetVector);
    return true;
}