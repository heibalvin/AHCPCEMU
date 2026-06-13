#include "cpcdsk.h"
#include "cpcemu.h"
#include "cpclog.h"

CPCDSK::CPCDSK(CPCEMU& centralEmu)
    : CPCCOM(centralEmu), diskBuffer(nullptr), diskBufferSize(0), 
      isDiskLoaded(false), currentTrack(0), currentSector(0), motorOn(false) {}

CPCDSK::~CPCDSK() {
    powerOff();
}

bool CPCDSK::powerOn() {
    reset();
    return true;
}

void CPCDSK::powerOff() {
    ejectDisk();
}

void CPCDSK::reset() {
    currentTrack = 0;
    currentSector = 0;
    motorOn = false;
    LOG_DSK(SDL_LOG_PRIORITY_INFO, "FDC Floppy Disk Controller subsystem reset.");
}

// ======================================================================
// PURE BUFFER DATA INJECTION ENGINE (No OS File Paths)
// ======================================================================
bool CPCDSK::loadDisk(const Uint8* rawData, size_t dataSize) {
    ejectDisk(); // Flush any previously allocated drive contents safely

    if (!rawData || dataSize == 0) {
        LOG_DSK(SDL_LOG_PRIORITY_CRITICAL, "FDC Error: Input array pointer payload is empty.");
        return false;
    }

    // Allocate an isolated memory footprint inside the emulator matrix using SDL3 allocations
    diskBuffer = static_cast<Uint8*>(SDL_malloc(dataSize));
    if (!diskBuffer) {
        LOG_DSK(SDL_LOG_PRIORITY_CRITICAL, "FDC Allocation Failure: Out of memory for DSK buffer.");
        return false;
    }

    diskBufferSize = dataSize;
    
    // Block copy the array contents directly from CPCAPP's input parameter buffer
    SDL_memcpy(diskBuffer, rawData, diskBufferSize);

    // Structural validation verification check pass (Validating MV - CPC signature tag descriptors)
    if (SDL_memcmp(diskBuffer, "MV - CPC", 8) != 0 && SDL_memcmp(diskBuffer, "EXTENDED", 8) != 0) {
        LOG_DSK(SDL_LOG_PRIORITY_WARN, "FDC Notice: Injected buffer signature does not match standard Amstrad DSK profiles.");
    }

    isDiskLoaded = true;
    LOG_DSK(SDL_LOG_PRIORITY_INFO, "Floppy Drive A: Mounted successfully via raw array transfer (%zu bytes).", diskBufferSize);
    return true;
}

void CPCDSK::ejectDisk() {
    if (diskBuffer) {
        SDL_free(diskBuffer);
        diskBuffer = nullptr;
    }
    diskBufferSize = 0;
    isDiskLoaded = false;
}

// ======================================================================
// HARDWARE PORT REGISTRATION READ/WRITE API WRAPPERS
// ======================================================================
Uint8 CPCDSK::readFDCStatus() {
    // Return a hardware status layout byte mirroring μPD765 expectations
    Uint8 status = 0x00;
    if (isDiskLoaded && motorOn) {
        status |= 0x10; // Drive 0 Seek Command Active Status Flag Bit High 
    }
    status |= 0x80;     // Data Register Ready for CPU I/O Transactions
    return status;
}

Uint8 CPCDSK::readFDCData() {
    if (!isDiskLoaded || !motorOn) return 0xFF;
    
    // Abstracted sectors structural read bypass vector formula mapping out basic track offsets
    // A standard track block size envelope on traditional CPC layout profiles spans ~0x1300 data chunks
    size_t computedOffset = 0x100 + (currentTrack * 0x1300) + (currentSector * 512);
    if (computedOffset < diskBufferSize) {
        return diskBuffer[computedOffset];
    }
    return 0x00;
}

void CPCDSK::writeFDCData(Uint8 value) {
    // Process standard commands injected from CPC firmware sequence instructions
    static Uint8 commandPhaseByteCount = 0;
    static Uint8 trackingCommandByte = 0x00;

    if (commandPhaseByteCount == 0) {
        trackingCommandByte = value;
        // Basic Z80 BIOS FDC Opcode Command Routing Matrix Blocks
        if ((trackingCommandByte & 0x0F) == 0x0F) commandPhaseByteCount = 3;      // Seek Track Command Sequence
        else if ((trackingCommandByte & 0x1F) == 0x06) commandPhaseByteCount = 9; // Read Data Block Command Sequence
        else commandPhaseByteCount = 1; 
    }

    commandPhaseByteCount--;
}

void CPCDSK::setMotorState(bool state) {
    motorOn = state;
    LOG_DSK(SDL_LOG_PRIORITY_INFO, "FDC Physical Floppy Spindle Drive Motor: %s", state ? "ON" : "OFF");
}

// ======================================================================
// CORE MEMORY/ROM SWAPPING LINKAGES
// ======================================================================
/* TBD: Banking logic removed for flat memory model testing 
void CPCDSK::requestUpperRomBank(Uint8 bankIndex) {
    // Bridges communication straight over to the updated CPCBUS layer
    emu.bus.selectUpperRomBank(bankIndex);
    LOG_DSK(SDL_LOG_PRIORITY_INFO, "CPCDSK triggered I/O request: Upper ROM active slot shifted to Bank %d", bankIndex);
}

void CPCDSK::requestRomVisibility(bool lowerEnabled, bool upperEnabled) {
    // Reconstructs a dummy configuration byte to parse down to Gate Array logic
    Uint8 configByte = 0x00;
    if (!lowerEnabled) configByte |= 0x04;
    if (!upperEnabled) configByte |= 0x08;
    
    emu.bus.setRomBankingConfig(configByte);
    LOG_DSK(SDL_LOG_PRIORITY_INFO, "CPCDSK triggered I/O request: Memory map layout configured (Lower OS ROM: %s | Upper Expansion ROM: %s)", 
            lowerEnabled ? "VISIBLE" : "HIDDEN", upperEnabled ? "VISIBLE" : "HIDDEN");
}

*/