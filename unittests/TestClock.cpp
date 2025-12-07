#include "avrTimerAdapterClock.h"
#include "gtest/gtest.h"

struct TimerRegisters {
    uint8_t TCCR1A;
    uint8_t TCCR1B;
    uint16_t TCNT1;
    uint8_t TIMSK1;
    uint16_t ICR1;
};

class TestClock : public ::testing::Test
{
    public:
    TimerRegisters timerRegisters;
    TimerUtils::AvrTimerInterface16Bit timerInterface =
    {
        &timerRegisters.TCCR1A,
        &timerRegisters.TCCR1B,
        &timerRegisters.TCNT1,
        &timerRegisters.TIMSK1,
        &timerRegisters.ICR1,
    };
    AvrTimerAdapterClock clock{timerInterface};
    protected:
    virtual void SetUp() override {
        memset(&timerRegisters, 0, sizeof(TimerRegisters));
        clock = AvrTimerAdapterClock{timerInterface};
    }
};

TEST_F(TestClock, Initialization) {
    timerRegisters.TCCR1A = 0xFF; // set to non-zero to verify initialization
    timerRegisters.TCCR1B = 0xFF;
    timerRegisters.TCNT1 = 0xFFFF;
    timerRegisters.TIMSK1 = 0xFF;
    AvrTimerAdapterClock testClock{timerInterface};
    EXPECT_EQ(timerRegisters.TCCR1A, 0);
    EXPECT_EQ(timerRegisters.TCCR1B & (1 << 0), 0); // prescaling 8
    EXPECT_EQ(timerRegisters.TCCR1B & (1 << 1), (1 << 1)); // prescaling 8
    EXPECT_EQ(timerRegisters.TCCR1B & (1 << 2), 0); // prescaling 8

    EXPECT_EQ(timerRegisters.TCNT1, 0);
    EXPECT_EQ(timerRegisters.TIMSK1 & (1 << 0), (1 << 0)); // TOIE1 bit
    EXPECT_EQ(timerRegisters.TIMSK1 & (1 << 1), 0); // OCIE1A bit is disabled
    EXPECT_EQ(timerRegisters.TIMSK1 & (1 << 2), 0); // OCIE1B bit is disabled
    EXPECT_EQ(timerRegisters.TIMSK1 & (1 << 5), 0); // interrupt capture is disabled
}

TEST_F(TestClock, InitialTimeIsZero)
{
    EXPECT_EQ(clock.getSystemTimeMicros(), 0);
}
TEST_F(TestClock, TimeAfterOverflow)
{
    clock.overflowHandler();
    auto max_ticks = static_cast<uint64_t>(65536);
    EXPECT_EQ(clock.getSystemTimeMicros(), max_ticks * 8);
    clock.overflowHandler();
    EXPECT_EQ(clock.getSystemTimeMicros(), max_ticks * 2 * 8);
}

TEST_F(TestClock, TimeWithCounter)
{
    timerRegisters.TCNT1 = 1000; // 1000 ticks
    EXPECT_EQ(clock.getSystemTimeMicros(), 8000); // 1000 * 8 us
}

TEST_F(TestClock, TimeWithOverflowAndCounter)
{
    clock.overflowHandler();
    clock.overflowHandler();
    timerRegisters.TCNT1 = 2000; // 2000 ticks
    auto overflowedTicks = static_cast<uint64_t>(65536) * 2;
    EXPECT_EQ(clock.getSystemTimeMicros(), (overflowedTicks * 8) + (2000 * 8));
}

TEST_F(TestClock, enableInteruptCapture)
{
    for (const auto edgeType: {AvrTimerAdapterClock::EdgeType::RISING, AvrTimerAdapterClock::EdgeType::FALLING}) {
        for (auto noiseCanceling: {true, false}) {
            EXPECT_TRUE(clock.enableInputCaptureInterrupts(edgeType, [](){}, noiseCanceling));

            // check noise canceling
            EXPECT_EQ(timerRegisters.TCCR1B & (1 << 7), noiseCanceling ? (1 << 7) : 0);

            // check input capture edge select
            EXPECT_EQ(timerRegisters.TCCR1B & (1 << 6), (edgeType == AvrTimerAdapterClock::EdgeType::RISING) ? (1 << 6) : 0);

            // check if input capture interrupt is enabled
            EXPECT_EQ(timerRegisters.TIMSK1 & (1 << 5), (1 << 5));

            // check if overflow interrup is still enabled
            EXPECT_EQ(timerRegisters.TIMSK1 & (1 << 0), (1 << 0));

            // try again - expect false
            EXPECT_FALSE(clock.enableInputCaptureInterrupts(edgeType, [](){}, noiseCanceling));
            clock.disableInputCaptureInterrupts();
        }

    }
}

TEST_F(TestClock, disableInterruptCapture)
{
    EXPECT_TRUE(clock.enableInputCaptureInterrupts(AvrTimerAdapterClock::EdgeType::RISING, [](){}, true));
    // check if input capture interrupt is enabled
    EXPECT_EQ(timerRegisters.TIMSK1 & (1 << 5), (1 << 5));

    // check if overflow interrup is still enabled
    EXPECT_EQ(timerRegisters.TIMSK1 & (1 << 0), (1 << 0));

    clock.disableInputCaptureInterrupts();

    // check if input capture interrupt is disabled
    EXPECT_EQ(timerRegisters.TIMSK1 & (1 << 5), 0);

    // check if overflow interrup is still enabled
    EXPECT_EQ(timerRegisters.TIMSK1 & (1 << 0), (1 << 0));
}

TEST_F(TestClock, callbackNotCalledAfterDisabling) {
    bool called = false;
    auto callback = [&called](){called = true;};
    EXPECT_TRUE(clock.enableInputCaptureInterrupts(AvrTimerAdapterClock::EdgeType::RISING, callback, true));
    clock.disableInputCaptureInterrupts();
    //this should not do anything
    clock.inputCaptureInterruptHandler();
    EXPECT_FALSE(called);
}

TEST_F(TestClock, inputCaptureHandlerCallsCallback) {
    bool called = false;
    auto callback = [&called](){called = true;};
    EXPECT_TRUE(clock.enableInputCaptureInterrupts(AvrTimerAdapterClock::EdgeType::RISING, callback, true));
    clock.inputCaptureInterruptHandler();
    EXPECT_TRUE(called);

    // then check if callback is not set anymore
    called = false;
    clock.inputCaptureInterruptHandler();
    EXPECT_FALSE(called);
}

TEST_F(TestClock, getInputCaptureRegister) {
    timerRegisters.ICR1 = 100;
    // with adding noise canceling delay
    EXPECT_EQ(clock.getInputCaptureRegister(true), 100 + 4);
    // without adding noise canceling delay
    EXPECT_EQ(clock.getInputCaptureRegister(false), 100);
}

TEST_F(TestClock, getInputCaptureTimeMicros) {
    timerRegisters.ICR1 = 100;
    timerRegisters.TCNT1 = 110;
    EXPECT_EQ(clock.getInputCaptureTimeMicros(false), 100 * 8);
    clock.overflowHandler();
    EXPECT_EQ(clock.getInputCaptureTimeMicros(false), 100 * 8 + (static_cast<uint64_t>(65536U) << 3U));

    // simulate that there has been an overflow between input capture and function call
    timerRegisters.TCNT1 = 90;
    EXPECT_EQ(clock.getInputCaptureTimeMicros(false), 100 * 8);

    // with adding noise canceling delay
    EXPECT_EQ(clock.getInputCaptureTimeMicros(true), (100 + 4) * 8);
}
