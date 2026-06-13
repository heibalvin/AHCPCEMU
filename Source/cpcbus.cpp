#include "cpcbus.h"
#include "cpclog.h"

CPCBUS::CPCBUS(CPCEMU& centralEmu) 
    : CPCCOM(centralEmu), memory(nullptr) {}

CPCBUS::~CPCBUS() { powerOff(); }

bool CPCBUS::powerOn() {
    memory = static_cast<Uint8*>(SDL_malloc(MEMORY_SIZE));
    if (!memory) return false;
    reset();
    return true;
}

void CPCBUS::powerOff() {
    if (memory) {
        SDL_free(memory);
        memory = nullptr;
    }
}

void CPCBUS::reset() {
    if (memory) {
        SDL_memset(memory, 0x00, MEMORY_SIZE);
    }
}

Uint8 CPCBUS::read(Uint16 address) {
    // CP/M BDOS Intercept (Function 0x0005)
    // When the Z80 executes a CALL to 0x0005, we intercept here.
    if (address == 0x0005) {
        // TODO: Access CPU registers via a back-reference to handle syscalls
        return 0x00; 
    }
    
    return memory[address];
}

void CPCBUS::write(Uint16 address, Uint8 value) {
    memory[address] = value;
}

Uint16 CPCBUS::readWord(Uint16 address) {
    // Z80 is Little-Endian
    Uint8 low  = read(address);
    Uint8 high = read(static_cast<Uint16>(address + 1));
    return static_cast<Uint16>(low | (high << 8));
}

void CPCBUS::writeWord(Uint16 address, Uint16 value) {
    write(address, static_cast<Uint8>(value & 0xFF));
    write(static_cast<Uint16>(address + 1), static_cast<Uint8>((value >> 8) & 0xFF));
}