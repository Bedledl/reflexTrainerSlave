#include "avrTimerAdapterClock.h"
#include "avrTimerInterface.h"
#include <utility>
#include "print.h"

AvrTimerAdapterClock::AvrTimerAdapterClock(TimerUtils::AvrTimerInterface16Bit timerInterface) : timerInterface{std::move(timerInterface)}
{
    print("clock init\n");
    *(timerInterface.controlRegisterA) = 0x0; // sets it to normal mode, so that it resets to 0 after an overflow
    *(timerInterface.counterRegister) = 0x0;
    TimerUtils::setPrescaling8ForTimer(timerInterface.controlRegisterB);
    *(timerInterface.interruptMaskRegister) |= (1 << TOIE1); // allow interrupts on overflow
    // disable interrups on compare match
    *(timerInterface.interruptMaskRegister) &= ~(1 << OCIE0A);
    *(timerInterface.interruptMaskRegister) &= ~(1 << OCIE0B);
    disableInputCaptureInterrupts();
}

void AvrTimerAdapterClock::overflowHandler()
{
    print("overflow handler\n");
    systemMicroseconds += overflowMicroseconds;
}

void AvrTimerAdapterClock::inputCaptureInterruptHandler()
{
    print("input capture interrupt handler\n");
    if (inputCaptureCallback) {
        auto cb = inputCaptureCallback;
        inputCaptureCallback = nullptr;
        cb();
    }
}

uint64_t AvrTimerAdapterClock::getSystemTimeMicros()
{
    // the resolution is 8 microseconds
    return gpt_get_time_elapsed();
}

uint64_t AvrTimerAdapterClock::gpt_get_time_elapsed()
{
    // https://github.com/modm-io/avr-libstdcpp/blob/master/examples/chrono/chrono.cpp

    // Do the first read of the TIMER1 counter and the system tick.
    const auto tim1_cnt_1 = *timerInterface.counterRegister;
    const auto sys_tick_1 = systemMicroseconds;

    // Do the second read of the TIMER1 counter.
    const auto tim1_cnt_2 = *timerInterface.counterRegister;

    // Perform the consistency check to obtain the concatenated,
    // consistent, 64-bit microseconds value.
    if (tim1_cnt_2 >= tim1_cnt_1)
    {
        return static_cast<uint64_t>(sys_tick_1) +
               static_cast<uint64_t>(TimerUtils::ticksToMicrosecondsPrescaled8(tim1_cnt_1));
    }
    return static_cast<uint64_t>(systemMicroseconds) +
           static_cast<uint64_t>(TimerUtils::ticksToMicrosecondsPrescaled8(tim1_cnt_2));
}

bool AvrTimerAdapterClock::enableInputCaptureInterrupts(const EdgeType edge, std::function<void(void)> callback, bool noiseCanceler)
{
    print("Enable Input capture\n");
    if (inputCaptureCallback) {
        return false; // already enabled
    }
    inputCaptureCallback = callback;
    // enable input capture noise canceling
    if (noiseCanceler) {
        *timerInterface.controlRegisterB |= (1 << 7); // ICNC1 bit
    } else {
        *timerInterface.controlRegisterB &= ~(1 << 7); // ICNC1 bit
    }

    // input edge select
    if (edge == EdgeType::RISING) {
        *timerInterface.controlRegisterB |= (1 << 6); // ICES1 bit
    } else {
        *timerInterface.controlRegisterB &= ~(1 << 6); // ICES1 bit
    }

    // enable input capture interrupt
    *(timerInterface.interruptMaskRegister) |= (1 << 5); // TICIE1 bit

    print("Enabling of Input Capture successful\n");
    return true;
}
void AvrTimerAdapterClock::disableInputCaptureInterrupts()
{
    print("Disable Input capture\n");
    // disable input capture interrupt
    *(timerInterface.interruptMaskRegister) &= ~(1 << 5); // TICIE1 bit
    inputCaptureCallback = nullptr;
}
uint16_t AvrTimerAdapterClock::getInputCaptureRegister(bool addNoiseCancelerDelay)
{
    print("Get Input Capture Ticks\n");
    // returns the value of the input capture register (ICR1) (ticks not us!)
    return *timerInterface.inputCaptureRegister + (addNoiseCancelerDelay ? 4 : 0);
}

uint64_t AvrTimerAdapterClock::getInputCaptureTimeMicros(bool addNoiseCancelerDelay)
{
    uint16_t inputCaptureTicks = getInputCaptureRegister(addNoiseCancelerDelay);
    print("Input Capture Ticks: %u\n", inputCaptureTicks);
    auto microseconds = systemMicroseconds;
    print("System Microseconds:\n");
    printuint64(microseconds);
    const auto currentCounterTicks = *timerInterface.counterRegister;


    // Perform the consistency check to obtain the concatenated,
    // consistent, 64-bit microseconds value.
    if (currentCounterTicks >= inputCaptureTicks)
    {
        // there has not happen an overflow

        return microseconds
             + static_cast<uint64_t>(TimerUtils::ticksToMicrosecondsPrescaled8(inputCaptureTicks));
    }
    // has happened an overflow!
    print("There has been an overflow\n");
    return microseconds
         - overflowMicroseconds
         + static_cast<uint64_t>(TimerUtils::ticksToMicrosecondsPrescaled8(inputCaptureTicks));

}