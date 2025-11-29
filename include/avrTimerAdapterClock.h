#ifndef AVRTIMERADAPTERCLOCK_H
#define AVRTIMERADAPTERCLOCK_H
#include <stdint.h>
#include "avrTimerInterface.h"
#include "functional"

class AvrTimerAdapterClock
{
    /*
    Connects to the 16-bit timer 1 to provide a system time in microseconds.
    We run with a 1MHz frequency. We use a prescaler of 8, so the each timer tick is 8 microseconds.
    Thus, to get 1 millisecond, we need 125 ticks.
    Each overflow of the 16-bit timer happens every 524.288 milliseconds (65536 * 8 us).
    */
    static constexpr auto overflowMicroseconds = static_cast<uint32_t>(65536U) << 3U;

public:
    enum class EdgeType
    {
        RISING,
        FALLING
    };
    AvrTimerAdapterClock(TimerUtils::AvrTimerInterface16Bit timerInterface);
    void overflowHandler();
    void inputCaptureInterruptHandler();
    uint64_t getSystemTimeMicros();
    bool enableInputCaptureInterrupts(const EdgeType edge, std::function<void(void)> callback, bool noiseCanceler = true);
    void disableInputCaptureInterrupts();
    uint16_t getInputCaptureRegister(bool addNoiseCancelerDelay = true);
    uint64_t getInputCaptureTimeMicros(bool addNoiseCancelerDelay = true);

private:
    uint64_t systemMicroseconds = 0;
    uint64_t gpt_get_time_elapsed();
    TimerUtils::AvrTimerInterface16Bit timerInterface;
    std::function<void(void)> inputCaptureCallback = nullptr;
};

#endif // AVRTIMERADAPTERCLOCK_H