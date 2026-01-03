#include "avrTimerAdapterClock.h"
#include "avrTimerAdapterTimer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "timers.h"
#include "print.h"

TimerUtils::AvrTimerInterface16Bit Timer1Interface =
    {
        &TCCR1A,
        &TCCR1B,
        &TCNT1,
        &TIMSK1,
        &ICR1,
};

TimerUtils::AvrTimerInterface Timer0Interface = {
    &TCCR0A,
    &TCCR0B,
    &TCNT0,
    &TIMSK0,
    &OCR0A,
    &OCR0B,
};

TimerUtils::AvrTimerInterface Timer2Interface = {
    &TCCR2A,
    &TCCR2B,
    &TCNT2,
    &TIMSK2,
    &OCR2A,
    &OCR2B,
};

AvrTimerAdapterClock myTimers::clock{Timer1Interface};
AvrTimerAdapterTimer myTimers::avrTimer0{Timer0Interface};
AvrTimerAdapterTimer myTimers::avrTimer2{Timer2Interface};

ISR(TIMER1_OVF_vect)
{
    print("Timer1 overflow\n");
    myTimers::clock.overflowHandler();
}

ISR(TIMER1_CAPT_vect)
{
    myTimers::clock.inputCaptureInterruptHandler();
}

ISR(TIMER0_COMPA_vect)
{
    myTimers::avrTimer0.compareMatchHandlerA();
}

ISR(TIMER2_COMPA_vect)
{
    myTimers::avrTimer2.compareMatchHandlerA();
}

void setPrescaling8ForTimer(volatile uint8_t *controlRegisterB)
{
    // prescaling 8 CS12 = 0, CS11 = 1, CS10 = 0
    *controlRegisterB &= ~(1 << CS12);
    *controlRegisterB &= ~(1 << CS10);
    *controlRegisterB |= (1 << CS11); // prescaler 8
}

void setNoPrescalingForTimer(volatile uint8_t *controlRegisterB)
{
    // no prescaling CS12 = 0, CS11 = 0, CS10 = 1
    *(controlRegisterB) &= ~(1 << CS11);
    *(controlRegisterB) &= ~(1 << CS12);
    *(controlRegisterB) |= (1 << CS10);
}
