#include <stdint.h>
#include <pic32mx.h>
#include "miniproj.h"

const int tickrate = 100;
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
			IFS(0) |= 0xffffefff; // Reset flag
		}
	}
	return;
}