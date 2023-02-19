#include <stdint.h>
#include <pic32mx.h>
#include "miniproj.h"

#define SPI_DISPLAY_MODE_COMMAND (PORTF &= 0xffffffef) // Pin 39 (D/C#) is RF4, set to 0 is Command Mode
#define SPI_DISPLAY_MODE_DATA (PORTF |= 0x10) // 1 is Data Mode

// Set Vdd Enable (Pin 38, RF6)
#define SPI_POWER_ON_VDD (PORTFCLR = 0x40)

// VBAT Enable (Pin 40, RF4)
#define SPI_POWER_ON_VBAT (PORTFCLR = 0x20)

// Reset Pin (RD5) Active Low
#define SPI_RESET_ON (PORTGCLR = 0x200)
#define SPI_RESET_OFF (PORTGSET = 0x200)

void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

/*
========================== 23.3.3.1 MASTER MODE OPERATION ==============================
Perform the following steps to set up the SPI module for the Master mode operation:
1. Disable the SPI interrupts in the respective IECx register.
2. Stop and reset the SPI module by clearing the ON bit.
3. Clear the receive buffer.
4. Clear the ENHBUF bit (SPIxCON<16>) if using Standard Buffer mode or set the bit if using
Enhanced Buffer mode.
5. If SPI interrupts are not going to be used, skip this step and continue to step 5. Otherwise
the following additional steps are performed:
a) Clear the SPIx interrupt flags/events in the respective IFSx register.
b) Write the SPIx interrupt priority and subpriority bits in the respective IPCx register.
c) Set the SPIx interrupt enable bits in the respective IECx register.
6. Write the Baud Rate register, SPIxBRG.
7. Clear the SPIROV bit (SPIxSTAT<6>).
8. Write the desired settings to the SPIxCON register with MSTEN (SPIxCON<5>) = 1.
9. Enable SPI operation by setting the ON bit (SPIxCON<15>).
10. Write the data to be transmitted to the SPIxBUF register. Transmission (and reception) will
start as soon as data is written to the SPIxBUF register.


SPIxSTAT bit 6 SPIROV: Receive Overflow Flag bit
1 = A new data is completely received and discarded. The user software has not read the previous data in
the SPIxBUF register.
0 = No overflow has occurred
This bit is only set by hardware. It can be cleared by writing a zero, preferably with the command
SPIxSTATCLR = 1<<6. It can also be cleared by disabling and re-enabling the module using the
SPIxCON.ON bit.
*/
void spi_DisplayInit(void)
{
	TRISECLR = 0xFF;
	TRISFCLR = 0x70;  //  Set D/C#, VDD, VCC as outputs
	TRISGCLR = 0x300; // Set SDIN as output (RG8), and RES# (RG9)
	PORTG |= 0x200;

	SPI2CON = 0;
	/* == Disable SPI Interrupts (SPI2RXIE, SPI2TXIE, SPI2EIE) ==*/
	IEC(1) &= 0xffffff1f;
	/* == Stop and reset the SPI module by clearing the ON bit == */
	SPI2CONCLR = 0x8000;
	/* == Clear the receive buffer == */
	SPI2BUF = 0;
	/* == Clear the ENHBUF bit (SPIxCON<16>) if using Standard Buffer mode or set the bit if using Enhanced Buffer mode == */
	SPI2CONCLR = 0x10000;
	/* == Write the Baud Rate register, SPIxBRG == */
	SPI2BRG = 4;
	/* == Clear the SPIROV bit (SPIxSTAT<6>) == */
	SPI2STATCLR = 0x40;
	/* SPI2CON bit CKP = 1; */
	SPI2CONSET = 0x40;
	/* == Write the desired settings to the SPIxCON register with MSTEN (SPIxCON<5>) = 1 == */
	SPI2CONSET = 0x20;
	/* == Enable SPI operation by setting the ON bit (SPIxCON<15>) == */
	SPI2CONSET = 0x8000;
}
/*
spi_send_recv:

Copyright (c) 2015, Axel Isaksson
Copyright (c) 2015, F Lundevall

If you're a student, and you have modified one or more files,
you must add your name here.
Kristian Ruth
Lennart Sturm

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
uint8_t spi_send_recv(uint8_t data)
{
	while (!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while (!(SPI2STAT & 1));
	return SPI2BUF;
}

/*
== Power on sequence ==
1.Apply power to VDD.
2.Send Display Off command.
3.Initialize display to desired operating mode.
4.Clear screen.
5.Apply power to VBAT.
6.Delay 100ms.
7.Send Display On command.
*/
void spi_PowerOnDisplay(void)
{
	spi_DisplayInit();

	SPI_POWER_ON_VDD;
	time_Timer3(1000);

	SPI_DISPLAY_MODE_COMMAND;
	spi_send_recv(0xAE);

	SPI_RESET_ON;
	time_Timer3(50);
	SPI_RESET_OFF;
	time_Timer3(50);

	/* == Enable Charge Pump == */
	spi_send_recv(0x8D); // Charge Pump Setting
	spi_send_recv(0x14); // Enable Charge Pump

	/* == Set Pre-Charge Period == */
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);

	SPI_POWER_ON_VBAT;
	time_Timer3(100000);

	spi_send_recv(0xAF);
	spi_send_recv(0xA5);
}

void adc_Init(){
	

}