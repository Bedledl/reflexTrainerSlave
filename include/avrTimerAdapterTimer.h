#ifndef AVRTIMERADAPTERTIMER_H
#define AVRTIMERADAPTERTIMER_H
#include "stdint.h"
#include <functional>
#include "avrTimerInterface.h"

class AvrTimerAdapterTimer
{
    /*
    Connects to an 8-bit AVR timer in CTC mode to provide timer callbacks.
    We run with a 1MHz frequency.
    */
public:
    AvrTimerAdapterTimer(TimerUtils::AvrTimerInterface timerInterface);
    void compareMatchHandlerA();
    bool registerCallbackUs(std::function<void(void)> callback, uint16_t time_us);
    bool registerCallbackTicks(std::function<void(void)> callback, uint8_t ticks);
    void setPrescaling8()
    {
        TimerUtils::setPrescaling8ForTimer(timerInterface.controlRegisterB);
        prescaling8 = true;
    }
    void setNoPrescaling()
    {
        TimerUtils::setNoPrescalingForTimer(timerInterface.controlRegisterB);
        prescaling8 = false;
    }

private:
    TimerUtils::AvrTimerInterface timerInterface;
    bool slotAOccupied = false;
    std::function<void(void)> callbackA = nullptr;
    bool prescaling8 = false;

    void setCtcMode()
    {
        *(timerInterface.controlRegisterA) |= (1 << WGM01); // Set WGM01 bit for CTC mode
    }
    void enableCompareMatchAInterrupts()
    {
        *(timerInterface.interruptMaskRegister) |= (1 << OCIE0A);
    }
    void enableCompareMatchBInterrupts()
    {
        *(timerInterface.interruptMaskRegister) |= (1 << OCIE0B);
    }
    void disableCompareMatchAInterrupts()
    {
        *(timerInterface.interruptMaskRegister) &= ~(1 << OCIE0A);
    }
    void disableCompareMatchBInterrupts()
    {
        *(timerInterface.interruptMaskRegister) &= ~(1 << OCIE0B);
    }
    void setCompareRegisterA(uint8_t ticks)
    {
        *(timerInterface.compareRegisterA) = ticks;
    }
    void setCompareRegisterB(uint8_t ticks)
    {
        *(timerInterface.compareRegisterB) = ticks;
    }
    void resetTimerCounter()
    {
        *(timerInterface.counterRegister) = 0x0; // resets the counter
    }
};

#endif // AVRTIMERADAPTERTIMER_H