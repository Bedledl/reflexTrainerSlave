#define F_CPU 1000000UL

#include <util/delay.h>
#include <avr/interrupt.h>
#include "gpio.h"
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

    //debouncer.registerRaisingEdgeHandler(handleRaisingEdge);
    //debouncer.registerFallingEdgeHandler(handleFallingEdge);

    //IR_STATUS_LED_ON;
    //_delay_ms(1000);
    //IR_STATUS_LED_OFF;
    //_delay_ms(1000);
    //IR_STATUS_LED_ON;
    //_delay_ms(1000);
    //IR_STATUS_LED_OFF;
//
//
    //init_receiver();
    //IR_STATUS_LED_OFF;

    // _delay_ms(50);

	// sei();

	// while (1)
	// {

	// 	cli();
	// 	uint8_t check_result = check_new_packet(&received_packet);
	// 	sei();

	// 	if (check_result)
	// 	{
    //         blink(7);
	// 		char buff[10];
	// 		(void)  buff;
	// 	}

	// 	_delay_ms(50);
	// }
}

