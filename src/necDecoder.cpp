#include "necDecoder.h"
#include "timers.h"
#include <functional>

typedef AvrTimerAdapterClock::EdgeType EdgeType;

NecDecoder theOneAndOnlyNecDecoder;

void NecDecoder::waitForNecHeader()
{
    resetToWaitingForInitialBurst();
}

void NecDecoder::updateTimestamp()
{
    last_timestamp = clock.getInputCaptureTimeMicros();
}

bool NecDecoder::fitsExpectedTime(const uint64_t &actualDurationMicros, const uint16_t &expectedDurationMicros) const
{
    return ((expectedDurationMicros - TOLERANCE) < actualDurationMicros) && (actualDurationMicros < (expectedDurationMicros + TOLERANCE));
}

void NecDecoder::enableInputInterrupt(EdgeType edgeType)
{
    (void) edgeType;
    //TODO
    // clock.enableInputCaptureInterrupts(
    //     edgeType,
    //     &necInputInterrupt,
    //     true // noise cancelling
    // );
    // TODO make something with result
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
    auto timestamp = clock.getInputCaptureTimeMicros();
    if (fitsExpectedTime(timestamp - last_timestamp, expectedDurationMicros))
    {
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
        resetToWaitingForInitialBurst();
    }
}

void NecDecoder::irInputCallback()
{

    switch (state)
    {
    case ReceptionState::NOT_STARTED:
    case ReceptionState::WAITING_INITIAL_BURST:
        state = ReceptionState::DURING_INITIAL_BURST;
        updateTimestamp();
        enableInputInterrupt(EdgeType::FALLING);
        break;
    case ReceptionState::DURING_INITIAL_BURST:
        // expect finished 9 ms burst
        evaluateDuration(INITIAL_BURST_MICROS, ReceptionState::DURING_INITIAL_PAUSE, true);
        break;
    case ReceptionState::DURING_INITIAL_PAUSE:
        // expect finished 4.5 ms pause
        evaluateDuration(INITIAL_PAUSE_MICROS, ReceptionState::BIT_RECEPTION_BURST, false);
        break;
    case ReceptionState::BIT_RECEPTION_BURST:
        // expect finished 560us burst
        evaluateDuration(BIT_BURST, ReceptionState::BIT_RECEPTION_PAUSE, true);
        break;
    case ReceptionState::BIT_RECEPTION_PAUSE:
        // expect finished some pause (1 or 0)
        decodeBitPause();
        break;

    default:
        break;
    }
}
void NecDecoder::decodeBitPause()
{
    auto timestamp = clock.getInputCaptureTimeMicros();
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
