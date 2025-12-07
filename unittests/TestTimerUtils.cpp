#include "gtest/gtest.h"
#include "avrTimerInterface.h"

TEST(TestTimerUtils, TicksToMicrosecondsPrescaled8) {
    EXPECT_EQ(TimerUtils::ticksToMicrosecondsPrescaled8(0), 0);
    EXPECT_EQ(TimerUtils::ticksToMicrosecondsPrescaled8(1), 8);
    EXPECT_EQ(TimerUtils::ticksToMicrosecondsPrescaled8(10), 80);
    EXPECT_EQ(TimerUtils::ticksToMicrosecondsPrescaled8(255), 2040);
}

TEST(TestTimerUtils, SetPrescaling8ForTimer) {
    uint8_t controlRegisterB = 0x00;
    TimerUtils::setPrescaling8ForTimer(&controlRegisterB);
    uint8_t expectedRegister = (1 << 1);
    EXPECT_EQ(controlRegisterB, expectedRegister);

    controlRegisterB = 0xFF;
    TimerUtils::setPrescaling8ForTimer(&controlRegisterB);
    expectedRegister = 0xF8 + (1 << 1);
    EXPECT_EQ(controlRegisterB, expectedRegister);
}

TEST(TestTimerUtils, SetNoPrescalingForTimer) {
    uint8_t controlRegisterB = 0x00; // all bits cleared
    TimerUtils::setNoPrescalingForTimer(&controlRegisterB);
    uint8_t expectedRegister = (1 << 0);
    EXPECT_EQ(controlRegisterB, expectedRegister);

    controlRegisterB = 0xFF;
    TimerUtils::setNoPrescalingForTimer(&controlRegisterB);
    expectedRegister = 0xF8 + (1 << 0);
    EXPECT_EQ(controlRegisterB, expectedRegister);
}
