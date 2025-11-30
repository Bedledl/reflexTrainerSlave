#define F_CPU 1000000UL

#include "necDecoder.h"


//#include "IR_Receiver.h"

//Gpio<PinMode::OUTPUT> led(PB0);
//
//void blink(int times) {
//    for (int i = 0; i < times; ++i) {
//        led.write(PinState::HIGH);
//        _delay_ms(1000);
//        led.write(PinState::LOW);
//    }
//}


//struct IR_Packet received_packet;
int main()
{
    theOneAndOnlyNecDecoder.waitForNecHeader();
}
