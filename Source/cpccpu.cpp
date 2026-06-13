#include "cpccpu.h"
#include "cpcemu.h"
#include "cpclog.h"

CPCCPU::CPCCPU(CPCEMU& centralEmu) 
    : CPCCOM(centralEmu), PC(0x0000), SP(0x0000), A(0), F(0), BC(0), DE(0), HL(0),
      IFF1(false), IFF2(false), IM(0) {}

bool CPCCPU::powerOn() {
    reset();
    return true;
}

void CPCCPU::powerOff() {}

void CPCCPU::reset() {
    // The Z80 hardware power-on default reset entry point vector is always 0x0000
    PC = 0x0000;
    SP = 0x0000;
    A = 0x00;
    F = 0x00;
    BC = 0x0000;
    DE = 0x0000;
    HL = 0x0000;

    // Hardware defaults on boot
    IFF1 = false;
    IFF2 = false;
    IM = 0;
}

int CPCCPU::step() {
    // Fetch opcode byte via 8-bit bus read configuration
    Uint8 opcode = emu.bus.read(PC);
    printf("DEBUG: Fetching 0x%02X from 0x%04X\n", opcode, PC);

    // LOG_CPU(SDL_LOG_PRIORITY_INFO, "CPU Fetch -> PC: [0x%04X] Opcode: 0x%02X", PC, opcode);
    
    // Advance PC past the fetched instruction block byte
    PC++;
    
    // Process and route operation, capturing execution hardware cycles spent
    int cyclesConsumed = execute(opcode);

    return cyclesConsumed;
}

// ======================================================================
// EXTERNAL HEADLESS INTERRUPT TRIGGERS
// ======================================================================
void CPCCPU::triggerNMI() {
    IFF2 = IFF1; // Preserve maskable enable flag status
    IFF1 = false; 

    push16(PC);
    PC = 0x0066; // Non-Maskable Interrupt hardwired vector branch destination
}

void CPCCPU::triggerINT(Uint8 vectorByte) {
    if (!IFF1) return; // DroppedCPU Fetch -> PC automatically if interrupts are currently disabled via DI

    IFF1 = false;
    IFF2 = false;

    push16(PC);

    if (IM == 1) {
        PC = 0x0038; // Amstrad CPC standard mode 1 hardware vector address
    } 
    else if (IM == 2) {
        LOG_CPU(SDL_LOG_PRIORITY_WARN, "IM 2 Interrupt requested but fallback table lookup not completed yet.");
        PC = 0x0038;
    } 
    else { 
        if ((vectorByte & 0xC7) == 0xC7) { 
            PC = vectorByte & 0x38;
        } else {
            PC = 0x0038; 
        }
    }
}

// ======================================================================
// STORAGE, REGISTER SELECTION AND LAYER STACK HELPERS
// ======================================================================
void CPCCPU::push16(Uint16 value) {
    SP -= 2;
    emu.bus.writeWord(SP, value); // Native 16-bit little-endian write handling
}

Uint16 CPCCPU::pop16() {
    Uint16 value = emu.bus.readWord(SP); // Native 16-bit little-endian read handling
    SP += 2;
    return value;
}

Uint8 CPCCPU::getReg8(int regIndex) {
    switch (regIndex) {
        case 0: return (BC >> 8) & 0xFF; // Register B
        case 1: return BC & 0xFF;        // Register C
        case 2: return (DE >> 8) & 0xFF; // Register D
        case 3: return DE & 0xFF;        // Register E
        case 4: return (HL >> 8) & 0xFF; // Register H
        case 5: return HL & 0xFF;        // Register L
        case 7: return A;                // Register A
        default: return 0;
    }
}

void CPCCPU::setReg8(int regIndex, Uint8 value) {
    switch (regIndex) {
        case 0: BC = (value << 8) | (BC & 0x00FF); break; // B
        case 1: BC = (BC & 0xFF00) | value;        break; // C
        case 2: DE = (value << 8) | (DE & 0x00FF); break; // D
        case 3: DE = (DE & 0xFF00) | value;        break; // E
        case 4: HL = (value << 8) | (HL & 0x00FF); break; // H
        case 5: HL = (HL & 0xFF00) | value;        break; // L
        case 7: A = value;                         break; // A
    }
}

Uint16 CPCCPU::getReg16(int pairIndex) {
    switch (pairIndex) {
        case 0: return BC;
        case 1: return DE;
        case 2: return HL;
        case 3: return SP;
        default: return 0;
    }
}

void CPCCPU::setReg16(int pairIndex, Uint16 value) {
    switch (pairIndex) {
        case 0: BC = value; break;
        case 1: DE = value; break;
        case 2: HL = value; break;
        case 3: SP = value; break;
    }
}

// ======================================================================
// 8-BIT ARITHMETIC CORE CALCULATORS
// ======================================================================
void CPCCPU::opcodeAdd(Uint8 value, bool useCarry) {
    Uint8 carryIn = (useCarry && (F & 0x01)) ? 1 : 0;
    Uint16 result = A + value + carryIn;
    
    bool halfCarry = ((A & 0x0F) + (value & 0x0F) + carryIn) > 0x0F;
    bool overflow = ((A ^ result) & (value ^ result) & 0x80) != 0;

    A = result & 0xFF;

    F = 0;
    if (A & 0x80) F |= 0x80;            // S
    if (A == 0)   F |= 0x40;            // Z
    if (A & 0x20) F |= 0x20;            // Y
    if (halfCarry)F |= 0x10;            // H
    if (A & 0x08) F |= 0x08;            // X
    if (overflow) F |= 0x04;            // V
    if (result > 0xFF) F |= 0x01;       // C
}

void CPCCPU::opcodeSub(Uint8 value, bool useCarry) {
    Uint8 carryIn = (useCarry && (F & 0x01)) ? 1 : 0;
    Uint16 result = A - value - carryIn;
    
    bool halfCarry = ((A & 0x0F) - (value & 0x0F) - carryIn) > 0x0F;
    bool overflow = ((A ^ value) & (A ^ result) & 0x80) != 0;

    A = result & 0xFF;

    F = 0;
    if (A & 0x80) F |= 0x80;            // S
    if (A == 0)   F |= 0x40;            // Z
    if (A & 0x20) F |= 0x20;            // Y
    if (halfCarry)F |= 0x10;            // H
    if (A & 0x08) F |= 0x08;            // X
    if (overflow) F |= 0x04;            // V
    F |= 0x02;                          // N is forced high on subtraction
    if (result > 0xFF) F |= 0x01;       // C
}

// ======================================================================
// 16-BIT ARITHMETIC CORE CALCULATORS
// ======================================================================
void CPCCPU::opcodeAdd16(Uint16 value) {
    Uint32 result = static_cast<Uint32>(HL) + value;
    bool halfCarry = ((HL & 0x0FFF) + (value & 0x0FFF)) > 0x0FFF;

    HL = result & 0xFFFF;

    F &= 0xC4; // Preserve existing S, Z, and P/V bits untouched
    if (halfCarry)     F |= 0x10; // H
    if (HL & 0x2000)   F |= 0x20; // Y
    if (HL & 0x0800)   F |= 0x08; // X
    if (result > 0xFFFF) F |= 0x01; // C
}

void CPCCPU::opcodeAdc16(Uint16 value) {
    Uint8 carryIn = (F & 0x01) ? 1 : 0;
    Uint32 result = static_cast<Uint32>(HL) + value + carryIn;
    
    bool halfCarry = ((HL & 0x0FFF) + (value & 0x0FFF) + carryIn) > 0x0FFF;
    bool overflow = ((HL ^ result) & (value ^ result) & 0x8000) != 0;

    HL = result & 0xFFFF;

    F = 0;
    if (HL & 0x8000)   F |= 0x80; // S
    if (HL == 0)       F |= 0x40; // Z
    if (HL & 0x2000)   F |= 0x20; // Y
    if (halfCarry)     F |= 0x10; // H
    if (HL & 0x0800)   F |= 0x08; // X
    if (overflow)      F |= 0x04; // V
    if (result > 0xFFFF) F |= 0x01; // C
}

void CPCCPU::opcodeSbc16(Uint16 value) {
    Uint8 carryIn = (F & 0x01) ? 1 : 0;
    Uint32 result = static_cast<Uint32>(HL) - value - carryIn;
    
    bool halfCarry = ((HL & 0x0FFF) - (value & 0x0FFF) - carryIn) > 0x0FFF;
    bool overflow = ((HL ^ value) & (HL ^ result) & 0x8000) != 0;

    HL = result & 0xFFFF;

    F = 0;
    if (HL & 0x8000)   F |= 0x80; // S
    if (HL == 0)       F |= 0x40; // Z
    if (HL & 0x2000)   F |= 0x20; // Y
    if (halfCarry)     F |= 0x10; // H
    if (HL & 0x0800)   F |= 0x08; // X
    if (overflow)      F |= 0x04; // V
    F |= 0x02;                    // N forced high on subtraction
    if (result > 0xFFFF) F |= 0x01; // C
}

// ======================================================================
// BITWISE LOGICAL CORE CALCULATORS
// ======================================================================
void CPCCPU::opcodeAnd(Uint8 value) {
    A &= value;
    F = 0;
    if (A & 0x80) F |= 0x80;            // S
    if (A == 0)   F |= 0x40;            // Z
    if (A & 0x20) F |= 0x20;            // Y
    F |= 0x10;                          // H flag is always hardwired high on Z80 AND
    if (A & 0x08) F |= 0x08;            // X
    
    Uint8 bits = A;
    bits ^= bits >> 4; bits ^= bits >> 2; bits ^= bits >> 1;
    if (!(bits & 1)) F |= 0x04;         // P/V Even Parity
}

void CPCCPU::opcodeXor(Uint8 value) {
    A ^= value;
    F = 0;
    if (A & 0x80) F |= 0x80;            // S
    if (A == 0)   F |= 0x40;            // Z
    if (A & 0x20) F |= 0x20;            // Y
    if (A & 0x08) F |= 0x08;            // X
    
    Uint8 bits = A;
    bits ^= bits >> 4; bits ^= bits >> 2; bits ^= bits >> 1;
    if (!(bits & 1)) F |= 0x04;         // P/V Even Parity
}

void CPCCPU::opcodeOr(Uint8 value) {
    A |= value;
    F = 0;
    if (A & 0x80) F |= 0x80;            // S
    if (A == 0)   F |= 0x40;            // Z
    if (A & 0x20) F |= 0x20;            // Y
    if (A & 0x08) F |= 0x08;            // X
    
    Uint8 bits = A;
    bits ^= bits >> 4; bits ^= bits >> 2; bits ^= bits >> 1;
    if (!(bits & 1)) F |= 0x04;         // P/V Even Parity
}

void CPCCPU::opcodeCp(Uint8 value) {
    Uint8 originalA = A;
    opcodeSub(value, false);
    A = originalA; // Discard calculation results but retain flags
}

// ======================================================================
// CONTROL FLOW BRANCH CALCULATORS
// ======================================================================
void CPCCPU::opcodeJp(bool condition) {
    Uint16 destination = emu.bus.readWord(PC);
    PC += 2;
    if (condition) PC = destination;
}

void CPCCPU::opcodeJr(bool condition) {
    int8_t displacement = static_cast<int8_t>(emu.bus.read(PC));
    PC++;
    if (condition) PC += displacement;
}

void CPCCPU::opcodeCall(bool condition) {
    Uint16 destination = emu.bus.readWord(PC);
    PC += 2; 
    if (condition) {
        push16(PC);
        PC = destination;
    }
}

void CPCCPU::opcodeRet(bool condition) {
    if (condition) PC = pop16();
}

// ======================================================================
// ADVANCED 0xCB PRE-INDEXED BIT MANIPULATION DECODER STRATEGIES
// ======================================================================
int CPCCPU::executeCB() {
    Uint8 subOpcode = emu.bus.read(PC++);
    
    int group    = (subOpcode >> 6) & 0x03; 
    int bitIndex = (subOpcode >> 3) & 0x07; 
    int regIndex = subOpcode & 0x07;        

    Uint8 value = (regIndex == 6) ? emu.bus.read(HL) : getReg8(regIndex);
    int cyclesSpent = (regIndex == 6) ? 15 : 8;

    switch (group) {
        case 0x00: // SHIFT & ROTATE OPERATIONS
            value = opcodeExecuteShiftRotate(bitIndex, value);
            break;

        case 0x01: // BIT TESTS (BIT b, r)
            {
                bool bitValue = (value & (1 << bitIndex)) != 0;
                F &= 0x01; // Retain only Carry flag
                if (!bitValue) F |= 0x40; // Set Z if target bit was 0
                F |= 0x10;                // H is forced high on BIT checks
                if (bitIndex == 7 && bitValue) F |= 0x80; // S
                if (value & 0x20) F |= 0x20;              // Y
                if (value & 0x08) F |= 0x08;              // X
                return (regIndex == 6) ? 12 : 8; 
            }

        case 0x02: // BIT CLEAR (RES b, r)
            value &= ~(1 << bitIndex);
            break;

        case 0x03: // BIT SET (SET b, r)
            value |= (1 << bitIndex);
            break;
    }

    if (regIndex == 6) {
        emu.bus.write(HL, value);
    } else {
        setReg8(regIndex, value);
    }

    return cyclesSpent;
}

Uint8 CPCCPU::opcodeExecuteShiftRotate(int shiftType, Uint8 value) {
    Uint8 oldCarry = F & 0x01;
    Uint8 newCarry = 0;
    Uint8 result = 0;

    switch (shiftType) {
        case 0: // RLC
            newCarry = (value >> 7) & 0x01;
            result = (value << 1) | newCarry;
            break;
        case 1: // RRC
            newCarry = value & 0x01;
            result = (value >> 1) | (newCarry << 7);
            break;
        case 2: // RL
            newCarry = (value >> 7) & 0x01;
            result = (value << 1) | oldCarry;
            break;
        case 3: // RR
            newCarry = value & 0x01;
            result = (value >> 1) | (oldCarry << 7);
            break;
        case 4: // SLA
            newCarry = (value >> 7) & 0x01;
            result = value << 1;
            break;
        case 5: // SRA
            newCarry = value & 0x01;
            result = (value >> 1) | (value & 0x80);
            break;
        case 7: // SRL
            newCarry = value & 0x01;
            result = value >> 1;
            break;
        default:
            result = value;
            break;
    }

    F = 0;
    if (result & 0x80) F |= 0x80;        // S
    if (result == 0)   F |= 0x40;        // Z
    if (result & 0x20) F |= 0x20;        // Y
    if (result & 0x08) F |= 0x08;        // X
    Uint8 bits = result;
    bits ^= bits >> 4; bits ^= bits >> 2; bits ^= bits >> 1;
    if (!(bits & 1)) F |= 0x04;          // P/V
    F |= newCarry;                       // C
    return result;
}

// ======================================================================
// ADVANCED 0xED BLOCK TRANSFER AND HARDWARE LOOP SEARCH ENGINES
// ======================================================================
void CPCCPU::opcodeLdBlock(bool increment, bool repeat) {
    Uint8 value = emu.bus.read(HL);
    emu.bus.write(DE, value);

    if (increment) { HL++; DE++; } else { HL--; DE--; }
    BC--;

    F &= 0xC1; // Clear N, H, and P/V bits cleanly
    if (BC != 0) F |= 0x04; // V flag stays high if counter hasn't reached zero

    if (repeat && BC != 0) {
        PC -= 2; // Wind PC back to repeat the 2-byte prefix sequence
    }
}

void CPCCPU::opcodeCpBlock(bool increment, bool repeat) {
    Uint8 memValue = emu.bus.read(HL);
    int result = A - memValue;

    if (increment) HL++; else HL--;
    BC--;

    bool sign = (result & 0x80) != 0;
    bool zero = ((result & 0xFF) == 0);
    bool halfCarry = ((A & 0x0F) - (memValue & 0x0F)) < 0;

    F = (F & 0x01); 
    if (sign)      F |= 0x80;
    if (zero)      F |= 0x40;
    if (halfCarry) F |= 0x10;
    F |= 0x02; // N is forced high on block compare operations
    if (BC != 0)   F |= 0x04;

    int undocumentedDiff = result - (halfCarry ? 1 : 0);
    if (undocumentedDiff & 0x02) F |= 0x08; // X
    if (undocumentedDiff & 0x08) F |= 0x20; // Y

    if (repeat && BC != 0 && !zero) {
        PC -= 2; 
    }
}

// ======================================================================
// CENTRAL SWITCH DECODER MATRIX
// ======================================================================
int CPCCPU::execute(Uint8 opcode) {
    printf("DEBUG: Executing 0x%02X\n", opcode);
    LOG_CPU(SDL_LOG_PRIORITY_INFO, "Executing Opcode: 0x%02X at PC: 0x%04X", opcode, PC - 1);
    
    // 1. Z80 8-Bit Register-to-Register Direct Block optimization pattern mapping (0x40 - 0x7F)
    if (opcode >= 0x40 && opcode <= 0x7F && opcode != 0x76) {
        int dstIndex = (opcode >> 3) & 0x07;
        int srcIndex = opcode & 0x07;

        if (dstIndex == 6) { 
            emu.bus.write(HL, getReg8(srcIndex)); return 7;
        } else if (srcIndex == 6) { 
            setReg8(dstIndex, emu.bus.read(HL)); return 7;
        } else { 
            setReg8(dstIndex, getReg8(srcIndex)); return 4;
        }
    }

    switch (opcode) {
        case 0x00: return 4; // NOP

        // ======================================================================
        // 16-BIT HARDWARE MATH DIRECT INSTRUCTIONS
        // ======================================================================
        case 0x09: opcodeAdd16(BC); return 11; // ADD HL, BC
        case 0x19: opcodeAdd16(DE); return 11; // ADD HL, DE
        case 0x29: opcodeAdd16(HL); return 11; // ADD HL, HL
        case 0x39: opcodeAdd16(SP); return 11; // ADD HL, SP

        case 0x03: BC++; return 6; // INC BC
        case 0x13: DE++; return 6; // INC DE
        case 0x23: HL++; return 6; // INC HL
        case 0x33: SP++; return 6; // INC SP

        case 0x0B: BC--; return 6; // DEC BC
        case 0x1B: DE--; return 6; // DEC DE
        case 0x2B: HL--; return 6; // DEC HL
        case 0x3B: SP--; return 6; // DEC SP

        // ======================================================================
        // LOAD OPERATIONS GROUP
        // ======================================================================
        case 0x06: setReg8(0, emu.bus.read(PC++)); return 7;  // LD B, n
        case 0x0E: setReg8(1, emu.bus.read(PC++)); return 7;  // LD C, n
        case 0x16: setReg8(2, emu.bus.read(PC++)); return 7;  // LD D, n
        case 0x1E: setReg8(3, emu.bus.read(PC++)); return 7;  // LD E, n
        case 0x26: setReg8(4, emu.bus.read(PC++)); return 7;  // LD H, n
        case 0x2E: setReg8(5, emu.bus.read(PC++)); return 7;  // LD L, n
        case 0x3E: setReg8(7, emu.bus.read(PC++)); return 7;  // LD A, n
        case 0x36: emu.bus.write(HL, emu.bus.read(PC++)); return 10; // LD (HL), n

        case 0x02: emu.bus.write(BC, A); return 7;           // LD (BC), A
        case 0x12: emu.bus.write(DE, A); return 7;           // LD (DE), A
        case 0x0A: A = emu.bus.read(BC); return 7;           // LD A, (BC)
        case 0x1A: A = emu.bus.read(DE); return 7;           // LD A, (DE)

        case 0x01: BC = emu.bus.readWord(PC); PC += 2; return 10; // LD BC, nn
        case 0x11: DE = emu.bus.readWord(PC); PC += 2; return 10; // LD DE, nn
        case 0x21: HL = emu.bus.readWord(PC); PC += 2; return 10; // LD HL, nn
        case 0x31: SP = emu.bus.readWord(PC); PC += 2; return 10; // LD SP, nn

        case 0x2A: { Uint16 addr = emu.bus.readWord(PC+1); HL = emu.bus.readWord(addr); PC += 3; return 16; }
        case 0x22: { Uint16 addr = emu.bus.readWord(PC+1); emu.bus.writeWord(addr, HL); PC += 3; return 16; }
        case 0x3A: { Uint16 addr = emu.bus.readWord(PC+1); A = emu.bus.read(addr); PC += 3; return 13; }
        case 0x32: { Uint16 addr = emu.bus.readWord(PC+1); emu.bus.write(addr, A); PC += 3; return 13; }

        // ======================================================================
        // STACK OPERATIONS GROUP
        // ======================================================================
        case 0xC5: push16(BC); return 11; 
        case 0xD5: push16(DE); return 11; 
        case 0xE5: push16(HL); return 11; 
        case 0xF5: push16((static_cast<Uint16>(A) << 8) | F); return 11; // PUSH AF

        case 0xC1: BC = pop16(); return 10; 
        case 0xD1: DE = pop16(); return 10; 
        case 0xE1: HL = pop16(); return 10; 
        case 0xF1: { Uint16 af = pop16(); A = af >> 8; F = af & 0xFF; return 10; } // POP AF

        // ======================================================================
        // ARITHMETIC OPERATIONS GROUP
        // ======================================================================
        case 0xC6: opcodeAdd(emu.bus.read(PC++), false); return 7; // ADD A, n
        case 0xCE: opcodeAdd(emu.bus.read(PC++), true);  return 7; // ADC A, n
        case 0xD6: opcodeSub(emu.bus.read(PC++), false); return 7; // SUB n
        case 0xDE: opcodeSub(emu.bus.read(PC++), true);  return 7; // SBC A, n

        case 0x80: opcodeAdd(BC >> 8, false);  return 4; // ADD A, B
        case 0x81: opcodeAdd(BC & 0xFF, false); return 4; // ADD A, C
        case 0x90: opcodeSub(BC >> 8, false);  return 4; // SUB B
        case 0x91: opcodeSub(BC & 0xFF, false); return 4; // SUB C

        // ======================================================================
        // BITWISE LOGICAL OPERATIONS GROUP
        // ======================================================================
        case 0xE6: opcodeAnd(emu.bus.read(PC++)); return 7; // AND n
        case 0xEE: opcodeXor(emu.bus.read(PC++)); return 7; // XOR n
        case 0xF6: opcodeOr(emu.bus.read(PC++));  return 7; // OR n
        case 0xFE: opcodeCp(emu.bus.read(PC++));  return 7; // CP n

        case 0xA0: opcodeAnd(BC >> 8);  return 4; 
        case 0xA1: opcodeAnd(BC & 0xFF); return 4; 
        case 0xA7: opcodeAnd(A);          return 4; 
        case 0xAF: opcodeXor(A);          return 4; // XOR A (Clear to 0 optimization)
        case 0xB0: opcodeOr(BC >> 8);   return 4; 
        case 0xB1: opcodeOr(BC & 0xFF);  return 4; 
        case 0xB8: opcodeCp(BC >> 8);   return 4; 
        case 0xB9: opcodeCp(BC & 0xFF);  return 4; 

        // ======================================================================
        // BRANCH CONTROL FLOW GROUP
        // ======================================================================
        case 0xC3: opcodeJp(true);            return 10; 
        case 0xCA: opcodeJp((F & 0x40) != 0); return 10; // JP Z
        case 0xC2: opcodeJp((F & 0x40) == 0); return 10; // JP NZ

        case 0x18: opcodeJr(true);            return 12; 
        case 0x28: { bool t = (F & 0x40) != 0; opcodeJr(t); return t ? 12 : 7; } // JR Z
        case 0x20: { bool t = (F & 0x40) == 0; opcodeJr(t); return t ? 12 : 7; } // JR NZ

        case 0xCD: { // CALL nn
            Uint16 returnAddr = PC + 2; 
            Uint16 destination = emu.bus.readWord(PC);
            
            LOG_CPU(SDL_LOG_PRIORITY_INFO, "CALLing subroutine at: 0x%04X", destination);
            
            push16(returnAddr);
            PC = destination;
            return 17;
        }
        case 0xCC: { bool t = (F & 0x40) != 0; opcodeCall(t); return t ? 17 : 10; } // CALL Z
        case 0xC4: { bool t = (F & 0x40) == 0; opcodeCall(t); return t ? 17 : 10; } // CALL NZ

        // ======================================================================
        // EXTENSION PREFIX MATRIX DIRECT ENTRY ROUTINGS
        // ======================================================================
        case 0xCB: return executeCB();

        case 0xF3: IFF1 = false; IFF2 = false; return 4; // DI
        case 0xFB: IFF1 = true;  IFF2 = true;  return 4; // EI

        case 0xED:
            {
                Uint8 subOpcode = emu.bus.read(PC++);
                switch (subOpcode) {
                    // --- Integrated Amstrad Gate Array I/O Banking Routing Intercept ---
                    case 0x79: // OUT (C), A
                        {
                            Uint16 portHigh = (BC >> 8) & 0xFF;
                            
                            // Gate Array Selection Decode Interlock (Bit 14 Low, Bit 15 Low)
                            if ((portHigh & 0xC0) == 0x40) {
                                /* TBD: Banking logic removed for flat memory model testing
                                emu.bus.setRomBankingConfig(A);
                                */
                            }
                            // Upper Selection Expansion Slot Decode (0xDF00)
                            else if (portHigh == 0xDF) {
                                /* TBD: Banking logic removed for flat memory model testing
                                emu.bus.selectUpperRomBank(A);
                                */
                            }
                            return 12;
                        }

                    // 16-Bit Arithmetic with Carry Table
                    case 0x4A: opcodeAdc16(BC); return 15; // ADC HL, BC
                    case 0x5A: opcodeAdc16(DE); return 15; // ADC HL, DE
                    case 0x6A: opcodeAdc16(HL); return 15; // ADC HL, HL
                    case 0x7A: opcodeAdc16(SP); return 15; // ADC HL, SP

                    case 0x42: opcodeSbc16(BC); return 15; // SBC HL, BC
                    case 0x52: opcodeSbc16(DE); return 15; // SBC HL, DE
                    case 0x62: opcodeSbc16(HL); return 15; // SBC HL, HL
                    case 0x72: opcodeSbc16(SP); return 15; // SBC HL, SP

                    // Block Transfer Operations
                    case 0xA0: opcodeLdBlock(true, false); return 14; // LDI
                    case 0xB0: { bool l = (BC > 1); opcodeLdBlock(true, true); return l ? 21 : 14; } // LDIR
                    case 0xA8: opcodeLdBlock(false, false); return 14; // LDD
                    case 0xB8: { bool l = (BC > 1); opcodeLdBlock(false, true); return l ? 21 : 14; } // LDDR

                    // Block Search Operations
                    case 0xA1: opcodeCpBlock(true, false); return 14; // CPI
                    case 0xB1: { bool m = (A == emu.bus.read(HL)); bool l = (BC > 1 && !m); opcodeCpBlock(true, true); return l ? 21 : 14; } // CPIR
                    case 0xA9: opcodeCpBlock(false, false); return 14; // CPD
                    case 0xB9: { bool m = (A == emu.bus.read(HL)); bool l = (BC > 1 && !m); opcodeCpBlock(false, true); return l ? 21 : 14; } // CPDR

                    // Interrupt Configuration Hooks
                    case 0x45: IFF1 = IFF2; PC = pop16(); return 14; // RETN
                    case 0x4D: PC = pop16();               return 14; // RETI
                    case 0x46: IM = 0;                     return 8;  
                    case 0x56: IM = 1;                     return 8;  
                    case 0x5E: IM = 2;                     return 8;  

                    default:
                        LOG_CPU(SDL_LOG_PRIORITY_CRITICAL, "UNIMPLEMENTED ED PREFIX EXTENSION: 0x%02X at PC: 0x%04X", subOpcode, (Uint16)(PC - 2));
                        emu.isRunning = false; // Direct state manipulation
                        return 0;
                }
            }

        default:
            LOG_CPU(SDL_LOG_PRIORITY_CRITICAL, "UNIMPLEMENTED OPCODE TRAP: 0x%02X at PC: 0x%04X", opcode, (Uint16)(PC - 1));
            emu.isRunning = false;
            return 0;
    }
}