#ifndef NEC_DECODER_H
#define NEC_DECODER_H

#include "stdint.h"
#include "avrTimerAdapterClock.h"
#include "functional"

constexpr uint16_t INITIAL_BURST_MICROS = 9 * 1000;
constexpr uint16_t INITIAL_PAUSE_MICROS = 45 * 100;
constexpr uint16_t BIT_BURST = 560;
constexpr uint16_t BIT_PAUSE_1 = 1690; // (2.25 * 1000 ) - 560
constexpr uint16_t BIT_PAUSE_0 = 560;  // (1.12 * 1000 ) - 560
constexpr uint16_t TOLERANCE = 50;

typedef AvrTimerAdapterClock::EdgeType EdgeType;

class NecDecoder
{

public:
    enum class ReceptionState
    {
        NOT_STARTED,
        WAITING_INITIAL_BURST, // waiting for initial burst
        DURING_INITIAL_BURST,  // during 9ms brust
        DURING_INITIAL_PAUSE,  // during 4.5 ms pause
        BIT_RECEPTION_BURST,
        BIT_RECEPTION_PAUSE,
    };

    /*
    Listens to IR input for the 9ms burst and 4.5ms pause. Will call itself when
    */
    void waitForNecHeader();
    void irInputCallback();
    ReceptionState getState () const {
        return state;
    }

private:
    void updateTimestamp();
    bool fitsExpectedTime(const uint64_t &actualDurationMicros, const uint16_t &expectedDurationMicros) const;
    void enableInputInterrupt(EdgeType edgeType);
    void resetToWaitingForInitialBurst();
    void evaluateDuration(const uint16_t &expectedDurationMicros, ReceptionState nextState, bool wasBurst);
    void decodeBitPause();

    ReceptionState state = ReceptionState::NOT_STARTED;
    uint64_t last_timestamp;
    uint8_t bit_number = 0;
    uint32_t data;
    std::function<void(void)> temp;
};

extern NecDecoder theOneAndOnlyNecDecoder;

void necInputInterrupt();

#endif // NEC_DECODER_H