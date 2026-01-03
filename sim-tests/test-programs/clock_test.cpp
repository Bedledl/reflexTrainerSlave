

#ifdef F_CPU
#undef F_CPU
#endif

#define F_CPU 1000000UL

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "timers.h"
#include "print.h"

/*
 * This demonstrate how to use the avr_mcu_section.h file
 * The macro adds a section to the ELF file with useful
 * information for the simulator
 */
#include "../../packages/simavr/simavr/sim/avr/avr_mcu_section.h"
AVR_MCU(F_CPU, "atmega328p");

static int uart_putchar(char c, FILE *stream) {
  if (c == '\n')
    uart_putchar('\r', stream);
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = c;
  return 0;
}

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL,
                                         _FDEV_SETUP_WRITE);


int main()
{
	stdout = &mystdout;

	printf("Bootloader properly programmed, and ran me! Huzzah!\n");
  sei();
  printuint64(myTimers::clock.getSystemTimeMicros());
  _delay_us(7000);
  printuint64(myTimers::clock.getSystemTimeMicros());

}

