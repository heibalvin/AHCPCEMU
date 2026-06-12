#include "cpccpu.h"
#include "cpcemu.h" // Gives full access to cross-component routing pathways
#include "cpclog.h"

CPCCPU::CPCCPU(CPCEMU& centralEmu) 
    : CPCCOM(centralEmu), PC(0x0000), SP(0x0000), A(0), F(0), BC(0), DE(0), HL(0) {}

bool CPCCPU::powerOn() {
    reset();
    return true;
}

void CPCCPU::powerOff() {

}

void CPCCPU::reset() {
    // The Z80 hardware power-on default reset entry point vector is always 0x0000
    PC = 0x0000;
    SP = 0x0000;
    A = 0x00;
    F = 0x00;
    BC = 0x0000;
    DE = 0x0000;
    HL = 0x0000;
}

void CPCCPU::step() {
    // Read opcode byte directly via system bus translation
    Uint8 opcode = emu.bus.readByte(PC);
    
    LOG_CPU(SDL_LOG_PRIORITY_INFO, "CPU Fetch -> PC: [0x%04X] Opcode: 0x%02X", PC, opcode);
    
    // Placeholder iteration forward to simulate clock ticks
    PC++;
}