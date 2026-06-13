#include "cpcemu.h"
#include "cpclog.h"

CPCEMU::CPCEMU() : isRunning(false), bus(*this), cpu(*this), dsk(*this) {}

bool CPCEMU::powerOn() {
    LOG_EMU(SDL_LOG_PRIORITY_INFO, "Powering On Amstrad CPC Central Core Subsystems...");

    if (!bus.powerOn()) {
        LOG_EMU(SDL_LOG_PRIORITY_CRITICAL, "BUS layer power-on failure.");
        return false;
    }
    if (!dsk.powerOn()) {
        LOG_EMU(SDL_LOG_PRIORITY_CRITICAL, "FDC DSK layer power-on failure.");
        bus.powerOff();
        return false;
    }
    if (!cpu.powerOn()) {
        LOG_EMU(SDL_LOG_PRIORITY_CRITICAL, "CPU layer power-on failure.");
        dsk.powerOff();
        bus.powerOff();
        return false;
    }

    isRunning = true;
    LOG_EMU(SDL_LOG_PRIORITY_INFO, "Amstrad CPC Emulator Environment successfully powered on.");
    return true;
}

void CPCEMU::powerOff() {
    LOG_EMU(SDL_LOG_PRIORITY_INFO, "Executing central power-off sequence cascade...");
    cpu.powerOff();
    dsk.powerOff();
    bus.powerOff();
    isRunning = false;
}

void CPCEMU::reset() {
    bus.reset();
    dsk.reset();
    cpu.reset();
    isRunning = true;
}

int CPCEMU::step() {
    // TEMPORARY TRACE: Print every instruction address
    // LOG_EMU(SDL_LOG_PRIORITY_DEBUG, "PC: 0x%04X", cpu.PC);
    
    int cyclesThisStep = cpu.step();
    if (cyclesThisStep == 0) {
        LOG_EMU(SDL_LOG_PRIORITY_WARN, "Execution engine halted at PC: 0x%04X", cpu.PC);
        isRunning = false;
    }
    return cyclesThisStep;
}

void CPCEMU::run() {
    LOG_EMU(SDL_LOG_PRIORITY_INFO, "Core simulation pipeline entered master execution cycle loop.");
    Uint64 totalCyclesExecuted = 0;
    while (isRunning) {
        totalCyclesExecuted += step();
    }
    LOG_EMU(SDL_LOG_PRIORITY_INFO, "Simulation ended. Total Hardware Cycles: %llu", totalCyclesExecuted);
}

bool CPCEMU::loadRom(const Uint8* data, size_t size, bool bootstrap) {
    if (!data || size == 0) return false;

    LOG_EMU(SDL_LOG_PRIORITY_INFO, "Injecting %zu bytes to 0x0100", size);

    for (size_t i = 0; i < size; ++i) {
        bus.write(0x0100 + i, data[i]);
    }

    // SANITY CHECK: Verify the first few bytes
    Uint8 check = bus.read(0x0100);
    LOG_EMU(SDL_LOG_PRIORITY_INFO, "Verify: Byte at 0x0100 is 0x%02X", check);

    if (bootstrap) {
        cpu.PC = 0x0100;
        cpu.SP = 0xF000;
    }
    return true;
}

bool CPCEMU::loadDiskArray(const Uint8* arrayData, size_t arraySize) {
    if (!isRunning) return false;
    return dsk.loadDisk(arrayData, arraySize);
}