
#include <stdlib.h>
#include <stdio.h>
//#include <libgen.h>

#include <pthread.h>

#include "sim_avr.h"
#include "avr_ioport.h"
#include "sim_elf.h"
#include "sim_gdb.h"
#include "sim_vcd_file.h"
#include "button.h"


int do_button_press = 0;
avr_t * avr = NULL;
avr_vcd_t vcd_file;
uint8_t	pin_state = 0;	// current port B

float pixsize = 64;
int window;

/*
 * called when the AVR change any of the pins on port B
 * so lets update our buffer
 */
void pin_changed_hook(struct avr_irq_t * irq, uint32_t value, void * param)
{
	pin_state = (pin_state & ~(1 << irq->irq)) | (value << irq->irq);
}

void keyCB(unsigned char key, int x, int y)	/* called on key press */
{
	if (key == 'q')
		exit(0);
	//static uint8_t buf[64];
	switch (key) {
		case 'q':
		case 0x1f: // escape
			exit(0);
			break;
		case ' ':
			do_button_press++; // pass the message to the AVR thread
			break;
		case 'r':
			printf("Starting VCD trace\n");
			avr_vcd_start(&vcd_file);
			break;
		case 's':
			printf("Stopping VCD trace\n");
			avr_vcd_stop(&vcd_file);
			break;
	}
}

void test_main() {
	button_t button;
	elf_firmware_t f = {{0}};;
	const char * fname =  "/home/betti/reflexTrainerSlave/build-simtests-avr/sim-tests/test-programs/main"; //.axf?
	//char path[256];

//	sprintf(path, "%s/%s", dirname(argv[0]), fname);
//	printf("Firmware pathname is %s\n", path);
	elf_read_firmware(fname, &f);

	printf("firmware %s f=%d mmcu=%s\n", fname, (int)f.frequency, f.mmcu);

	avr = avr_make_mcu_by_name(f.mmcu);
	if (!avr) {
		fprintf(stderr, "AVR '%s' not known\n", f.mmcu);
		exit(1);
	}
	avr_init(avr);
	avr_load_firmware(avr, &f);


	button_init(avr, &button, "button");
	// "connect" the output irw of the button to the port pin of the AVR
	avr_connect_irq(
		button.irq + IRQ_BUTTON_OUT,
		avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 0)
	);

	for(int i=0; i<100000; i++) {
		avr_run(avr);
	}
	button_press(&button, 0);
	for(int i=0; i<1000; i++) {
		avr_run(avr);
	}
	button_release(&button);
	for(int i=0; i<4000; i++) {
		avr_run(avr);
	}
	for(int i=0; i <32; i++) {
		button_press(&button, 0);
		for(int i=0; i<550; i++) {
			avr_run(avr);
		}
		button_release(&button);
		for(int i=0; i<550; i++) {
			avr_run(avr);
		}
	}
}

void test_clock() {
	elf_firmware_t f = {{0}};;
	const char * fname =  "/home/betti/reflexTrainerSlave/build-simtests-avr/sim-tests/test-programs/clock_test"; //.axf?
	//char path[256];

//	sprintf(path, "%s/%s", dirname(argv[0]), fname);
//	printf("Firmware pathname is %s\n", path);
	elf_read_firmware(fname, &f);

	printf("firmware %s f=%d mmcu=%s\n", fname, (int)f.frequency, f.mmcu);

	avr = avr_make_mcu_by_name(f.mmcu);
	if (!avr) {
		fprintf(stderr, "AVR '%s' not known\n", f.mmcu);
		exit(1);
	}
	avr_init(avr);
	avr_load_firmware(avr, &f);

	for(int i=0; i<100000000; i++) {
		avr_run(avr);
	}
}

int main(int argc, char *argv[])
{
	//test_main();
	test_clock();

}
