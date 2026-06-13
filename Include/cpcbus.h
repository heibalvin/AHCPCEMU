#ifndef CPCBUS_H
#define CPCBUS_H

#include <SDL3/SDL.h>
#include "cpccom.h"

class CPCBUS: CPCCOM {
private:
    Uint8* memory;
    static const size_t MEMORY_SIZE = 65536;

public:
    explicit CPCBUS(CPCEMU& centralEmu);
    ~CPCBUS();

    bool powerOn();
    void powerOff();
    void reset();

    // Standard Accessors
    Uint8 read(Uint16 address);
    void  write(Uint16 address, Uint8 value);

    // 16-bit helpers for Z80 registers
    Uint16 readWord(Uint16 address);
    void   writeWord(Uint16 address, Uint16 value);

    // Direct access for bootstrap loading
    Uint8* getRawMemoryPointer() { return memory; }
};

#endif // CPCBUS_H