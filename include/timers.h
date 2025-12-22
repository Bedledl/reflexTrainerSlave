#ifndef TIMERS_H
#define TIMERS_H
#include "avrTimerAdapterTimer.h"
#include "avrTimerAdapterClock.h"

namespace myTimers{
extern AvrTimerAdapterClock clock;
extern AvrTimerAdapterTimer avrTimer0;
extern AvrTimerAdapterTimer avrTimer2;
}

#endif // TIMERS_H