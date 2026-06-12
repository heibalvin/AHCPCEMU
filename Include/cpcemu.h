#ifndef CPCEMU_H
#define CPCEMU_H

#include "cpcbus.h"
#include "cpcdsk.h"
#include "cpccpu.h"

class CPCEMU {
    friend class CPCCPU;
    friend class CPCAPP;
    
private:
    size_t totalSystemCycles;
    bool isRunning; // Renamed for clean boolean nomenclature
    
    CPCBUS bus;
    CPCDSK dsk;
    CPCCPU cpu;

public:
    CPCEMU();
    ~CPCEMU();

    bool powerOn();
    void powerOff();
    void reset();
    void step(double deltaTime);
    bool loadRom(const Uint8* data, size_t size);
};

#endif // CPCEMU_H