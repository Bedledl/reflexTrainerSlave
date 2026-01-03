#include "necDecoder.h"
#include "timers.h"
#include "functional"
#include "print.h"

typedef AvrTimerAdapterClock::EdgeType EdgeType;

NecDecoder theOneAndOnlyNecDecoder;

void necInputInterrupt()
{
    print("nec input interrup\n");
    theOneAndOnlyNecDecoder.irInputCallback();
}

void NecDecoder::waitForNecHeader()
{
    print("wait for nec header:)\n");
    resetToWaitingForInitialBurst();
}

void NecDecoder::updateTimestamp()
{
    last_timestamp = myTimers::clock.getInputCaptureTimeMicros();
}

bool NecDecoder::fitsExpectedTime(const uint64_t &actualDurationMicros, const uint16_t &expectedDurationMicros) const
{
    print("%u\n", expectedDurationMicros);
    printuint64(actualDurationMicros);
    return ((expectedDurationMicros - TOLERANCE) < actualDurationMicros) && (actualDurationMicros < (expectedDurationMicros + TOLERANCE));
}

void NecDecoder::enableInputInterrupt(EdgeType edgeType)
{
    myTimers::clock.enableInputCaptureInterrupts(
         edgeType,
         std::function<void(void)>{necInputInterrupt},
         true // noise cancelling
     );
    // TODO make something with result
    print("Enabled Input Interrupt from Nec Decoder\n");
}

void NecDecoder::resetToWaitingForInitialBurst()
{
    state = ReceptionState::WAITING_INITIAL_BURST;
    enableInputInterrupt(EdgeType::RISING);
    bit_number = 0;
    data = 0;
}

void NecDecoder::evaluateDuration(const uint16_t &expectedDurationMicros, ReceptionState nextState, bool wasBurst)
{
    auto timestamp = myTimers::clock.getInputCaptureTimeMicros();
    if (fitsExpectedTime(timestamp - last_timestamp, expectedDurationMicros))
    {
        print("Fits expected time\n");
        last_timestamp = timestamp;
        state = nextState;
        if (wasBurst)
        {
            enableInputInterrupt(EdgeType::RISING);
        }
        else
        {
            enableInputInterrupt(EdgeType::FALLING);
        }
    }
    else
    {
        print("Doesn't fit expected time\n");
        resetToWaitingForInitialBurst();
    }
}

void NecDecoder::irInputCallback()
{
    print("IR input callback:)\n");
    switch (state)
    {
    case ReceptionState::NOT_STARTED:
    case ReceptionState::WAITING_INITIAL_BURST:
        state = ReceptionState::DURING_INITIAL_BURST;
        print("WAITING_BURST\n");
        updateTimestamp();
        enableInputInterrupt(EdgeType::FALLING);
        break;
    case ReceptionState::DURING_INITIAL_BURST:
        print("INITIAL_BURST\n");
        // expect finished 9 ms burst
        evaluateDuration(INITIAL_BURST_MICROS, ReceptionState::DURING_INITIAL_PAUSE, true);
        break;
    case ReceptionState::DURING_INITIAL_PAUSE:
        print("DURING INITIAL PAUSE\n");
        // expect finished 4.5 ms pause
        evaluateDuration(INITIAL_PAUSE_MICROS, ReceptionState::BIT_RECEPTION_BURST, false);
        break;
    case ReceptionState::BIT_RECEPTION_BURST:
        print("BIT RECEPTION BURST\n");
        // expect finished 560us burst
        evaluateDuration(BIT_BURST, ReceptionState::BIT_RECEPTION_PAUSE, true);
        break;
    case ReceptionState::BIT_RECEPTION_PAUSE:
        print("BIT RECEPTION PAUSE\n");
        // expect finished some pause (1 or 0)
        decodeBitPause();
        break;

    default:
        break;
    }
}
void NecDecoder::decodeBitPause()
{
    auto timestamp = myTimers::clock.getInputCaptureTimeMicros();
    if (fitsExpectedTime(timestamp - last_timestamp, BIT_PAUSE_0))
    {
        // set bit 0, should already be set
    }
    else if (fitsExpectedTime(timestamp - last_timestamp, BIT_PAUSE_1))
    {
        // set bit 1
        data |= (1 << bit_number);
    }
    else
    {
        // pause is not long enough for either 0 or 1
        resetToWaitingForInitialBurst();
        return;
    }
    state = ReceptionState::BIT_RECEPTION_BURST;
    enableInputInterrupt(EdgeType::RISING);
    last_timestamp = timestamp;
    bit_number += 1;
}
