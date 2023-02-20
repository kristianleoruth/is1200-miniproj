#include <stdint.h>
#include <pic32mx.h>
#include "miniproj.h"

const int mpt = 10; // seconds per tick

/* 
time is in microseconds (us)
 */
void time_Timer3(int time) {
	T3CON |= 0x8030; // Set Timer 3 ON, Prescaling to 1:8, then 1 count is 100 ns
	TMR3 = 0;
	PR3 = 10; // Count up to 10 and then change T3IF, this means every change is 1 us

	int c = 0;
	while (c < time) {
		if (IFS(0) & 0x1000) {
			c++;
			IFS(0) &= 0xffffefff; // Reset flag
		}
	}
	return;
}

void time_Tick() {
	T2CON |= 0x00008070; // Prescaling to 256, 8 to set ON bit = 1
	TMR2 = 0;
	PR2 = 3125; // Period register, 80MHz / 256 / 10

	int c = 0;
	while (c < mpt) {
		if (IFS(0) & 0x00000100) {
			c++;
			IFS(0) &= 0xFFFFFEFF;
		}
	}


	// T2CON |= 0x8030; // Set Timer 3 ON, Prescaling to 1:8, then 1 count is 100 ns
	// TMR2 = 0;
	// PR2 = 10000; // Count up to 10 and then change T3IF, this means every change is 1 millisecond

	// int c = 0;
	// while (c < 1) {
	// 	if (IFS(0) & 0x100) {
	// 		c++;
	// 		IFS(0) |= 0xfffffeff; // Reset flag
	// 	}
	// }
	// return;
}

void adc_Init() {
    AD1PCFG = 0xFFFB;   //set port
    AD1CON1 = 0;        //end sampling
    AD1CHS = 0x00020000;//connect port to sample and hold amplifier
    AD1CSSL = 0;
    AD1CON3 = 0x0002; //set conversion clock
    AD1CON2 = 0;
    AD1CON1SET = 0x8000; //turn adc-module on
}

int adc_GetDial() {

    AD1CON1SET = 0x0002; //tell SAH to start sampling 
    time_Timer3(100000); //100ms delay
    while(!(AD1CON1&0x0001)); //is the conversion done?
    return ADC1BUF0;
}

int btn1 () {
    if (PORTF & 0x1) {
        return 1;
    }
    return 0;
}

int btn2 () {
    if (PORTD & 0x20) {
        return 1;
    }
    return 0;
}
int btn3 () {
    return PORTD & 0x40;
}

int btn4 () {
    if (PORTD & 0x80) {
        return 1;
    }
    return 0;
}

int sw1 () {
    if (PORTD & 0x100) {
        return 1;
    }
    return 0;
}

int sw2 () {
    if (PORTD & 0x200) {
        return 1;
    }
    return 0;
}

int sw3 () {
    if (PORTD & 0x400) {
        return 1;
    }
    return 0;
}

int sw4 () {
    if (PORTD & 0x800) {
        return 1;
    }
    return 0;
}