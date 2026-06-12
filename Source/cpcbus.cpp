#include "cpcbus.h"
#include "cpclog.h"

CPCBUS::CPCBUS(CPCEMU& centralEmu) 
    : CPCCOM(centralEmu), lowerRomEnabled(true), upperRomEnabled(true) {
    SDL_memset(ram, 0, sizeof(ram));
    SDL_memset(lowerRom, 0, sizeof(lowerRom));
    SDL_memset(upperRom, 0, sizeof(upperRom));
}

bool CPCBUS::powerOn() {
    reset();
    return true;
}

void CPCBUS::powerOff() {

}

void CPCBUS::reset() {
    lowerRomEnabled = true; // CPC hardware defaults to both ROMs visible on boot
    upperRomEnabled = true;
}

void CPCBUS::injectLowerRom(const Uint8* data, size_t size) {
    if (!data || size == 0) return;
    size_t copySize = (size > 16384) ? 16384 : size;
    SDL_memcpy(lowerRom, data, copySize);
}

void CPCBUS::injectUpperRom(const Uint8* data, size_t size) {
    if (!data || size == 0) return;
    size_t copySize = (size > 16384) ? 16384 : size;
    SDL_memcpy(upperRom, data, copySize);
}

void CPCBUS::setRomStates(bool lowerEnabled, bool upperEnabled) {
    lowerRomEnabled = lowerEnabled;
    upperRomEnabled = upperEnabled;
}

Uint8 CPCBUS::readByte(Uint16 address) {
    // 0x0000 - 0x3FFF (Lower ROM / RAM Bank 0)
    if (address <= 0x3FFF) {
        return lowerRomEnabled ? lowerRom[address] : ram[address];
    }
    
    // 0x4000 - 0xBFFF (RAM Banks 1 & 2)
    if (address <= 0xBFFF) {
        return ram[address];
    }
    
    // 0xC000 - 0xFFFF (Upper ROM / RAM Bank 3)
    if (upperRomEnabled) {
        return upperRom[address - 0xC000];
    }
    return ram[address];
}

void CPCBUS::writeByte(Uint16 address, Uint8 value) {
    // ROM lines are read-only; writes always bypass to the background RAM bank
    ram[address] = value;
}