#ifndef CPCEMU_H
#define CPCEMU_H

#include <SDL3/SDL.h>
#include "cpcdsk.h"

class CPCEMU {
private:
    Uint64 totalSystemCycles;

public:
    CPCDSK dsk;

    CPCEMU();
    ~CPCEMU();

    bool powerOn();
    void powerOff();
    void reset();
    void step(double deltaTime);

    // Standardized asset injection signature
    bool loadRom(const Uint8* data, size_t size);
};

#endif // CPCEMU_H