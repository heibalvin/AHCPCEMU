#ifndef CPCBUS_H
#define CPCBUS_H

#include <SDL3/SDL.h>
#include "cpccom.h"

class CPCBUS : public CPCCOM {
	friend class CPCCPU;

private:
    Uint8 ram[128 * 1024];
    Uint8 lowerRom[16 * 1024];
    Uint8 upperRom[16 * 1024];
    bool lowerRomEnabled;
    bool upperRomEnabled;

public:
    // Pass the emu reference forward to CPCCOM
    explicit CPCBUS(CPCEMU& centralEmu);
    ~CPCBUS() override = default;

    bool powerOn() override;
    void powerOff() override;
    void reset() override;
    
    void injectLowerRom(const Uint8* data, size_t size);
    void injectUpperRom(const Uint8* data, size_t size);
    void setRomStates(bool lowerEnabled, bool upperEnabled);
    
    Uint8 readByte(Uint16 address);
    void writeByte(Uint16 address, Uint8 value);
};

#endif // CPCBUS_H