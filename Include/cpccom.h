#ifndef CPCCOM_H
#define CPCCOM_H

// Forward declaration of the parent motherboard class
class CPCEMU;

class CPCCOM {
protected:
    CPCEMU& emu; // Shared reference to the central communication mediator

public:
    // Every component MUST be constructed with a reference to the motherboard
    explicit CPCCOM(CPCEMU& centralEmu) : emu(centralEmu) {}
    virtual ~CPCCOM() = default;

    virtual bool powerOn() = 0;
    virtual void powerOff() = 0;
    virtual void reset() = 0;
};

#endif // CPCCOM_H