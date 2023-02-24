#include <stdint.h>
#include <pic32mx.h>
#include "miniproj.h"

int cspt = 5; // centiseconds per tick (100 = 1s)

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
	PR2 = 3125; // Period register, 80MHz / 256 / 100

	int c = 0;
	while (c < cspt) {
		if (IFS(0) & 0x00000100) {
			c++;
			IFS(0) &= 0xFFFFFEFF;
		}
	}
}

/* 
Ignores units part
*/
void time_ChangeTickSpeed(int milliseconds) {
    cspt = milliseconds / 10;
}

void adc_Init() {
    AD1PCFG = 0xFFFB;   //set port
    AD1CON1 = 0;        //end sampling
    AD1CHS = 0x00020000;//connect port to sample and hold amplifier
    AD1CSSL = 0;
    AD1CON3 = 0x0002; //set conversion clock
    AD1CON2 = 0;
    TRISBCLR = 0x400;
    time_Timer3(1000); //100ms delay
}

int adc_GetDial() {
    AD1CON1SET = 0x8000; //turn adc-module on
    AD1CON1SET = 0x0002; //tell SAH to start sampling 
    time_Timer3(1000); //100ms delay
    AD1CON1CLR = 0x0002;
    while(!(AD1CON1&0x0001));
    AD1CON1 = 0;
    return ADC1BUF0>>2;
    
    }

int btn(int select) {
    switch (select) {
        case 1:
            return PORTF & 0x2;
        case 2:
            return PORTD & 0x20;
        case 3:
            return PORTD & 0x40;
        case 4:
            return PORTD & 0x80;
        default:
            return 0;
    }
}

int sw (int select) {
    switch(select) {
        case 1:
            return PORTD &0x100;
        case 2:
            return PORTD & 0x200;
        case 3: 
            return PORTD & 0x400;
        case 4:
            return PORTD & 0x800;
        default:
            return 0;
    }
}