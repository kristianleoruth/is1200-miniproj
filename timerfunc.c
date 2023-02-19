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