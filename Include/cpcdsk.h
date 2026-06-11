#ifndef CPCDSK_H
#define CPCDSK_H

#include "cpccom.h"
#include <SDL3/SDL.h>

class CPCDSK : public CPCCOM {
public:
    explicit CPCDSK(CPCEMU& parentEmu);
    ~CPCDSK() override;

    // Lifecycle Overrides
    bool powerOn() override;
    void powerOff() override;
    void reset() override;

    // Load raw disk image/ROM payload straight into component memory bounds
    bool loadRom(const Uint8* data, size_t size);
};

#endif // CPCDSK_H