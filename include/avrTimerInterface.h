#ifndef AVRTIMERINTERFACE_H
#define AVRTIMERINTERFACE_H
#include "stdint.h"
#if CROSS_COMPILING == FALSE
#define CS10 0
#define CS11 1
#define CS12 2
#define TOIE1 0
#define WGM01 1
#define OCIE0A 1
#define OCIE0B 2
#else
#include <avr/io.h>
#endif


namespace TimerUtils {
struct AvrTimerInterface
{
    volatile uint8_t *controlRegisterA;
    volatile uint8_t *controlRegisterB;
    volatile uint8_t *counterRegister;
    volatile uint8_t *interruptMaskRegister;
    volatile uint8_t *compareRegisterA;
    volatile uint8_t *compareRegisterB;
};

struct AvrTimerInterface16Bit
{
    volatile uint8_t *controlRegisterA;
    volatile uint8_t *controlRegisterB;
    volatile uint16_t *counterRegister;
    volatile uint8_t *interruptMaskRegister;
    volatile uint16_t *inputCaptureRegister;
};

constexpr uint32_t ticksToMicrosecondsPrescaled8(const uint16_t &ticks)
{
    return static_cast<uint32_t>(ticks) << 3U; // prescaler is 8, so each tick is 8 microseconds
}


inline void setPrescaling8ForTimer(volatile uint8_t *controlRegisterB)
{
    // prescaling 8 CS12 = 0, CS11 = 1, CS10 = 0
    *controlRegisterB &= ~(1 << CS12);
    *controlRegisterB &= ~(1 << CS10);
    *controlRegisterB |= (1 << CS11); // prescaler 8
}

inline void setNoPrescalingForTimer(volatile uint8_t *controlRegisterB)
{
    // no prescaling CS12 = 0, CS11 = 0, CS10 = 1
    *(controlRegisterB) &= ~(1 << CS11);
    *(controlRegisterB) &= ~(1 << CS12);
    *(controlRegisterB) |= (1 << CS10);
}


};

#endif // AVRTIMERINTERFACE_H