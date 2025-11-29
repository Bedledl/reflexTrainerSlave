#include "avrTimerAdapterTimer.h"

AvrTimerAdapterTimer::AvrTimerAdapterTimer(TimerUtils::AvrTimerInterface timerInterface) : timerInterface{std::move(timerInterface)}
    {
        disableCompareMatchAInterrupts();
        disableCompareMatchBInterrupts();
        setCtcMode();
        resetTimerCounter();
    }

bool AvrTimerAdapterTimer::registerCallbackUs(std::function<void(void)> callback, uint16_t time_us) {
    /*
    Max values for time_us:
    - No prescaling: 255 us (1 tick = 1 us)
    - Prescaling 8: 2040 us (1 tick = 8 us)
    */
    if (slotAOccupied) {
        return false;
    }
    uint16_t ticks;
    if (prescaling8) {
        ticks = (time_us + 7) / 8; // round up to nearest tick

    } else {
        ticks = time_us; // 1 tick = 1 microsecond
    }

    if (ticks > 255) {
        return false;
    }

    return registerCallbackTicks(callback, ticks);
}
bool AvrTimerAdapterTimer::registerCallbackTicks(std::function<void(void)> callback, uint8_t ticks) {
    if(slotAOccupied) {
        return false;
    }
    callbackA = callback;
    slotAOccupied = true;
    resetTimerCounter();
    setCompareRegisterA(ticks);
    enableCompareMatchAInterrupts();
    return true;
}

void AvrTimerAdapterTimer::compareMatchHandlerA() {
    auto cb = callbackA;
    slotAOccupied = false;
    disableCompareMatchAInterrupts();
    callbackA = nullptr;
    if (cb) {
        cb();
    }
}



