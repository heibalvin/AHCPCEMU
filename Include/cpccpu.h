#ifndef CPCCPU_H
#define CPCCPU_H

#include <SDL3/SDL.h>
#include "cpccom.h"

class CPCCPU : public CPCCOM {
    friend class CPCEMU;
    friend class CPCBUS;
    
private:
    // Core Z80 Registers
    Uint16 PC;
    Uint16 SP;
    Uint8 A, F;
    Uint16 BC, DE, HL;

    // Interrupt State Variables
    bool IFF1;       // Interrupt Flip-Flop 1 (Master Maskable Enable)
    bool IFF2;       // Interrupt Flip-Flop 2 (Temporary Storage for NMI)
    Uint8 IM;        // Interrupt Mode (Tracked via Z80 IM 0, IM 1, or IM 2)

    // Storage, Manipulation & Index Utility Helpers
    void push16(Uint16 value);
    Uint16 pop16();
    Uint8 getReg8(int regIndex);
    void  setReg8(int regIndex, Uint8 value);
    Uint16 getReg16(int pairIndex);
    void   setReg16(int pairIndex, Uint16 value);

    // Unified Opcode Arithmetic & Bitwise Logic Calculators
    void opcodeAdd(Uint8 value, bool useCarry);
    void opcodeSub(Uint8 value, bool useCarry);
    void opcodeAnd(Uint8 value);
    void opcodeXor(Uint8 value);
    void opcodeOr(Uint8 value);
    void opcodeCp(Uint8 value);
    
    // 16-Bit Pointer Arithmetic Calculators
    void opcodeAdd16(Uint16 value);
    void opcodeAdc16(Uint16 value);
    void opcodeSbc16(Uint16 value);

    // Control Flow Branch Calculators
    void opcodeJp(bool condition);
    void opcodeJr(bool condition);
    void opcodeCall(bool condition);
    void opcodeRet(bool condition);

    // Advanced 0xCB Bit-Manipulator & 0xED Block Engine Layouts
    int   executeCB();
    Uint8 opcodeExecuteShiftRotate(int shiftType, Uint8 value);
    void  opcodeLdBlock(bool increment, bool repeat);
    void  opcodeCpBlock(bool increment, bool repeat);

    // Master execution matrix routing block
    int execute(Uint8 opcode);

public:
    explicit CPCCPU(CPCEMU& centralEmu);
    ~CPCCPU() override = default;

    bool powerOn() override;
    void powerOff() override;
    void reset() override;

    // Executes a single instruction sweep, returning elapsed clock cycles
    int step();
    
    // Headless Programmatic Hardware Injectors
    void triggerNMI();
    void triggerINT(Uint8 vectorByte = 0xFF);

    Uint16 getPC() const { return PC; }
};

#endif // CPCCPU_H