#include "avrTimerAdapterTimer.h"
#include "gtest/gtest.h"

struct TimerRegisters {
    uint8_t TCCRA;
    uint8_t TCCRB;
    uint8_t TCNT;
    uint8_t TIMSK;
    uint8_t OCR_A;
    uint8_t OCR_B;
};

class TestTimer : public ::testing::Test
{
    public:
    TimerRegisters timerRegisters;
    TimerUtils::AvrTimerInterface timerInterface =
    {
        &timerRegisters.TCCRA,
        &timerRegisters.TCCRB,
        &timerRegisters.TCNT,
        &timerRegisters.TIMSK,
        &timerRegisters.OCR_A,
        &timerRegisters.OCR_B,
    };
    AvrTimerAdapterTimer timer{timerInterface};
};

TEST_F(TestTimer, RegisterCallbackUsNoPrescalingSuccess)
{
    timer.setNoPrescaling();
    bool result = timer.registerCallbackUs([](){}, 100);
    EXPECT_TRUE(result);
}

TEST_F(TestTimer, RegisterCallbackUsPrescaling8Success)
{
    timer.setPrescaling8();
    bool result = timer.registerCallbackUs([](){}, 1000); // 1000 us = 125 ticks
    EXPECT_TRUE(result);
}

TEST_F(TestTimer, RegisterCallbackUsNoPrescalingTooLong)
{
    timer.setNoPrescaling();
    bool result = timer.registerCallbackUs([](){}, 300); // 300 us > 255 ticks
    EXPECT_FALSE(result);
}

TEST_F(TestTimer, RegisterCallbackUsPrescaling8TooLong)
{
    timer.setPrescaling8();
    bool result = timer.registerCallbackUs([](){}, 3000); // 3000 us > 255 ticks
    EXPECT_FALSE(result);
}

TEST_F(TestTimer, RegisterCallbackTicksSuccess)
{
    bool result = timer.registerCallbackTicks([](){}, 200);
    EXPECT_TRUE(result);
}

TEST_F(TestTimer, matchHandlerAWithoutCallbackDoesNotCrash)
{
    timer.compareMatchHandlerA();
}

TEST_F(TestTimer, matchHandlerACallsCallback)
{
    bool called = false;
    timer.registerCallbackTicks([&called](){ called = true; }, 100);
    EXPECT_EQ(timerRegisters.TIMSK & (1 << 1), (1 << 1));
    timer.compareMatchHandlerA();
    EXPECT_TRUE(called);
    EXPECT_EQ(timerRegisters.TIMSK & (1 << 1), 0);
}

TEST_F(TestTimer, RegisterCallbackTicksWhenSlotOccupiedFails)
{
    bool firstResult = timer.registerCallbackTicks([](){}, 100);
    EXPECT_TRUE(firstResult);
    bool secondResult = timer.registerCallbackTicks([](){}, 150);
    EXPECT_FALSE(secondResult);
}

TEST_F(TestTimer, RegisterInitialization)
{
    timerRegisters.TCNT = 0xFF; // set to non-zero to verify reset
    timerRegisters.TIMSK = 0xFF; // set all bits to verify disabling
    AvrTimerAdapterTimer testTimer{timerInterface};
    // check that counter is reset
    EXPECT_EQ(timerRegisters.TCNT, 0x0);
    // check that CTC mode is set
    EXPECT_EQ(timerRegisters.TCCRA & (1 << 1), (1 << 1)); // WGM01 bit
    // check that compare match interrups are disabled
    EXPECT_EQ(timerRegisters.TIMSK & (1 << 1), 0); // OCIE0A bit
    EXPECT_EQ(timerRegisters.TIMSK & (1 << 2), 0); // OCIE0B bit
}