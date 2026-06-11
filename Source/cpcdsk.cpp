#include "cpcdsk.h"
#include "cpclog.h"
#include "cpcemu.h"

CPCDSK::CPCDSK(CPCEMU& parentEmu) : CPCCOM(parentEmu) {}

CPCDSK::~CPCDSK() {
    powerOff();
}

bool CPCDSK::powerOn() {
    return true;
}

void CPCDSK::powerOff() {
}

void CPCDSK::reset() {
}

bool CPCDSK::loadRom(const Uint8* data, size_t size) {
    if (!data || size < 0x100) {
        LOG_DSK(SDL_LOG_PRIORITY_ERROR, "Target binary array footprint is too small to inspect.");
        return false;
    }

    // 1. Extract Identity Signature Properties
    char magicBuffer[17];
    SDL_strlcpy(magicBuffer, reinterpret_cast<const char*>(data), 16);

    Uint8 tracks = data[0x30];
    Uint8 sides  = data[0x31];

    LOG_DSK(SDL_LOG_PRIORITY_INFO, "================================================");
    LOG_DSK(SDL_LOG_PRIORITY_INFO, "   AMSTRAD DISK FORMAT GEOMETRY REPORT          ");
    LOG_DSK(SDL_LOG_PRIORITY_INFO, "================================================");
    LOG_DSK(SDL_LOG_PRIORITY_INFO, "File Signature Stamp : [ %s ]", magicBuffer);
    LOG_DSK(SDL_LOG_PRIORITY_INFO, "Total Physical Tracks: %d", tracks);
    LOG_DSK(SDL_LOG_PRIORITY_INFO, "Total Physical Sides : %d", sides);

    // 2. Step down through the Track Information Blocks
    size_t trackPointerOffset = 0x100;
    
    for (Uint8 t = 0; t < tracks; ++t) {
        if (trackPointerOffset + 0x18 > size) break;

        Uint8 currentTrackID = data[trackPointerOffset + 0x10];
        Uint8 currentSideID  = data[trackPointerOffset + 0x11];
        Uint8 sectorSizeCode = data[trackPointerOffset + 0x14];
        Uint8 sectorsCount   = data[trackPointerOffset + 0x15];

        // Translate the physical sector sizing parameter formula natively
        size_t actualSectorBytes = 128 * (1 << sectorSizeCode);

        // Only print out structural summaries for critical boundary checkpoints to keep logs clean
        if (t == 0 || t == tracks - 1) {
            LOG_DSK(SDL_LOG_PRIORITY_INFO, "------------------------------------------------");
            LOG_DSK(SDL_LOG_PRIORITY_INFO, "TRACK 0x%02X -> Side: %d | Sectors Found: %d | Size: %zu bytes per sector", 
                    currentTrackID, currentSideID, sectorsCount, actualSectorBytes);
            
            // Loop and trace the Sector ID allocation array map matrices
            size_t sectorHeaderBase = trackPointerOffset + 0x18;
            for (Uint8 s = 0; s < sectorsCount; ++s) {
                size_t currentSlotIndex = sectorHeaderBase + (s * 8);
                if (currentSlotIndex + 4 > size) break;

                Uint8 sectorID = data[currentSlotIndex + 2];
                LOG_DSK(SDL_LOG_PRIORITY_INFO, "  -> [Index Slot %d] Hardware Sector Identifier ID: 0x%02X", s, sectorID);
            }
        } else if (t == 1) {
            LOG_DSK(SDL_LOG_PRIORITY_INFO, "... [Skipping middle track iterations from display context output] ...");
        }

        // Advance to the next physical track offset segment 
        // Standard uncompressed track size allocation footprint: 256 byte header + (9 sectors * 512 bytes) = 4864
        trackPointerOffset += 256 + (sectorsCount * actualSectorBytes);
    }

    LOG_DSK(SDL_LOG_PRIORITY_INFO, "================================================");
    return true;
}