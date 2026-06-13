#ifndef CPCDSK_H
#define CPCDSK_H

#include <SDL3/SDL.h>
#include "cpccom.h"

class CPCDSK : public CPCCOM {
    friend class CPCEMU;

private:
    Uint8* diskBuffer;
    size_t diskBufferSize;
    bool   isDiskLoaded;

    // Track state parameters mapping back to the physical uPD765 FDC
    Uint8 currentTrack;
    Uint8 currentSector;
    bool  motorOn;

public:
    explicit CPCDSK(CPCEMU& centralEmu);
    ~CPCDSK() override;

    bool powerOn() override;
    void powerOff() override;
    void reset() override;

    // --- Pure Buffer Array Data Loader Interface ---
    bool loadDisk(const Uint8* rawData, size_t dataSize);
    void ejectDisk();

    // --- FDC Hardware Port Interface Linkages ---
    Uint8 readFDCStatus();
    Uint8 readFDCData();
    void  writeFDCData(Uint8 value);
    void  setMotorState(bool state);

    // --- Core Memory/ROM Swapping Linkages ---
    void requestUpperRomBank(Uint8 bankIndex);
    void requestRomVisibility(bool lowerEnabled, bool upperEnabled);

    bool isLoaded() const { return isDiskLoaded; }
};

#endif // CPCDSK_H