#ifndef CPCEMU_H
#define CPCEMU_H

#include <SDL3/SDL.h>
#include "cpcbus.h"
#include "cpccpu.h"
#include "cpcdsk.h"

class CPCEMU {
    friend class CPCAPP;
    friend class CPCCPU;

private:
    bool isRunning;

public:
    CPCBUS bus;
    CPCCPU cpu;
    CPCDSK dsk;

    CPCEMU();
    ~CPCEMU() = default;

    // --- Lifecycle Subsystems (Now purely uniform) ---
    bool powerOn();
    void powerOff();
    void reset();
    
    // --- Centralized Execution Core Drivers ---
    int  step();
    void run();

    // --- Array Data Injector Pass-throughs ---
    bool loadRom(const Uint8* data, size_t size, bool bootstrap = false);
    bool loadDiskArray(const Uint8* arrayData, size_t arraySize);
};

#endif // CPCEMU_H