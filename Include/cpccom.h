#ifndef CPCCOM_H
#define CPCCOM_H

class CPCEMU;

class CPCCOM {
protected:
    CPCEMU& emu;

public:
    explicit CPCCOM(CPCEMU& parentEmu) : emu(parentEmu) {}
    virtual ~CPCCOM() = default;

    // Strict System Paradigm Hooks
    virtual bool powerOn()  = 0;
    virtual void powerOff() = 0;
    virtual void reset()    = 0;
};

#endif // CPCCOM_H