#ifndef CPCCPU_H
#define CPCCPU_H

#include <SDL3/SDL.h>
#include "cpccom.h"

class CPCCPU : public CPCCOM {
	friend class CPCEMU;
	
private:
    Uint16 PC;
    Uint16 SP;
    Uint8 A, F;
    Uint16 BC, DE, HL;

public:
    explicit CPCCPU(CPCEMU& centralEmu);
    ~CPCCPU() override = default;

    bool powerOn() override;
    void powerOff() override;
    void reset() override;

    void step();
    Uint16 getPC() const { return PC; }
};

#endif // CPCCPU_H