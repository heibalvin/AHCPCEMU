#ifndef CPCEMU_H
#define CPCEMU_H

#include <SDL3/SDL.h>

class CPCEMU {
public:
    CPCEMU();
    ~CPCEMU();

    // Power operations inside the virtual hardware boundaries
    void systemReset();
    
    // Step the emulator forward by a precise amount of real-world time
    void update(double deltaTime);

    // Read-only state accessors for structural debugging
    Uint64 getExecutedCycles() const { return totalSystemCycles; }

private:
    Uint64 totalSystemCycles;
};

#endif // CPCEMU_H