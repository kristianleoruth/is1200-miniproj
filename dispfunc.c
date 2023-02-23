#include <stdint.h>
#include <pic32mx.h>
#include "miniproj.h"
// Set Vdd Enable (Pin 38, RF6)
#define SPI_POWER_ON_VDD (PORTFCLR = 0x40)

// VBAT Enable (Pin 40, RF4)
#define SPI_POWER_ON_VBAT (PORTFCLR = 0x20)

// Reset Pin (Pin 10, RG9) Active Low
#define SPI_RESET_ON (PORTGCLR = 0x200)
#define SPI_RESET_OFF (PORTGSET = 0x200)

uint8_t font[] = {
	// 'A'
	0,0,0,1,1,0,0,0,
	0,0,1,0,0,1,0,0,
	0,0,1,1,1,1,0,0,
	0,1,0,0,0,0,1,0,
	0,1,0,0,0,0,1,0,
	0,1,0,0,0,0,1,0,
	// 'B'
	0,0,1,1,1,0,0,0,
	0,0,1,0,0,1,0,0,
	0,0,1,1,1,0,0,0,
	0,0,1,1,1,1,0,0,
	0,0,1,0,0,0,1,0,
	0,0,1,1,1,1,0,0,
	// 'C'
	0,0,0,1,1,1,0,0,
	0,0,1,0,0,0,1,0,
	0,1,0,0,0,0,0,0,
	0,1,0,0,0,0,0,0,
	0,0,1,0,0,0,1,0,
	0,0,0,1,1,1,0,0,
	// 'D'
	0,1,1,1,1,0,0,0,
	0,1,0,0,0,1,0,0,
	0,1,0,0,0,0,1,0,
	0,1,0,0,0,0,1,0,
	0,1,0,0,0,1,0,0,
	0,1,1,1,1,0,0,0,
	// 'E'
	0,0,1,1,1,1,0,0,
	0,0,1,0,0,0,0,0,
	0,0,1,1,1,0,0,0,
	0,0,1,0,0,0,0,0,
	0,0,1,0,0,0,0,0,
	0,0,1,1,1,1,0,0,
	// 'F'
	0,0,1,1,1,1,0,0,
	0,0,1,0,0,0,0,0,
	0,0,1,1,1,0,0,0,
	0,0,1,0,0,0,0,0,
	0,0,1,0,0,0,0,0,
	0,0,1,0,0,0,0,0,
	// 'G'
	0,0,0,1,1,1,0,0,
	0,0,1,0,0,0,1,0,
	0,1,0,0,0,0,0,0,
	0,1,0,0,1,1,1,0,
	0,1,0,0,0,0,1,0,
	0,0,1,1,1,1,1,0,
	// 'H'
	0,0,1,0,0,1,0,0,
	0,0,1,0,0,1,0,0,
	0,0,1,1,1,1,0,0,
	0,0,1,0,0,1,0,0,
	0,0,1,0,0,1,0,0,
	0,0,1,0,0,1,0,0,
	// 'I'
	0,0,0,1,1,1,0,0,
	0,0,0,0,1,0,0,0,
	0,0,0,0,1,0,0,0,
	0,0,0,0,1,0,0,0,
	0,0,0,0,1,0,0,0,
	0,0,0,1,1,1,0,0,
	// 'J'
	0,0,0,0,1,1,1,0,
	0,0,0,0,0,1,0,0,
	0,0,0,0,0,1,0,0,
	0,0,0,0,0,1,0,0,
	0,0,1,0,0,1,0,0,
	0,0,0,1,1,0,0,0,
	// 'K'
	0,1,0,0,1,0,0,0,
	0,1,0,1,0,0,0,0,
	0,1,1,0,0,0,0,0,
	0,1,0,1,0,0,0,0,
	0,1,0,0,1,0,0,0,
	0,1,0,0,0,1,0,0,
	// 'L'
	0,0,1,0,0,0,0,0,
	0,0,1,0,0,0,0,0,
	0,0,1,0,0,0,0,0,
	0,0,1,0,0,0,0,0,
	0,0,1,0,0,0,0,0,
	0,0,1,1,1,1,0,0,
	// 'M'
	0,1,0,0,0,0,1,0,
	0,1,1,0,0,1,1,0,
	0,1,0,1,1,0,1,0,
	0,1,0,0,0,0,1,0,
	0,1,0,0,0,0,1,0,
	0,1,0,0,0,0,1,0,
	// 'N'
	0,1,0,0,0,0,1,0,
	0,1,1,0,0,0,1,0,
	0,1,0,1,0,0,1,0,
	0,1,0,0,1,0,1,0,
	0,1,0,0,0,1,1,0,
	0,1,0,0,0,0,1,0,
	// 'O'
	0,0,0,1,1,0,0,0,
	0,0,1,0,0,1,0,0,
	0,1,0,0,0,0,1,0,
	0,1,0,0,0,0,1,0,
	0,0,1,0,0,1,0,0,
	0,0,0,1,1,0,0,0,
	// 'P'
	0,0,1,1,1,0,0,0,
	0,0,1,0,0,1,0,0,
	0,0,1,1,1,0,0,0,
	0,0,1,0,0,0,0,0,
	0,0,1,0,0,0,0,0,
	0,0,1,0,0,0,0,0,
	// 'Q'
	0,0,0,1,1,0,0,0,
	0,0,1,0,0,1,0,0,
	0,1,0,0,0,0,1,0,
	0,1,0,0,1,0,1,0,
	0,0,1,0,0,1,0,0,
	0,0,0,1,1,0,1,0,
// 'R'
	0,0,1,1,1,0,0,0,
	0,0,1,0,0,1,0,0,
	0,0,1,1,1,0,0,0,
	0,0,1,0,0,1,0,0,
	0,0,1,0,0,1,0,0,
	0,0,1,0,0,0,1,0,
// 'S'
	0,0,0,1,1,0,0,0,
	0,0,1,0,0,0,0,0,
	0,0,0,1,1,0,0,0,
	0,0,0,0,0,1,0,0,
	0,0,1,0,0,1,0,0,
	0,0,0,1,1,0,0,0,
// 'T'
	0,0,1,1,1,1,1,0,
	0,0,0,0,1,0,0,0,
	0,0,0,0,1,0,0,0,
	0,0,0,0,1,0,0,0,
	0,0,0,0,1,0,0,0,
	0,0,0,0,1,0,0,0,
// U
	0,0,1,0,0,1,0,0,
	0,0,1,0,0,1,0,0,
	0,0,1,0,0,1,0,0,
	0,0,1,0,0,1,0,0,
	0,0,1,0,0,1,0,0,
	0,0,0,1,1,0,0,0,
// 'V'
	0,1,0,0,0,0,1,0,
	0,1,0,0,0,0,1,0,
	0,0,1,0,0,1,0,0,
	0,0,1,0,0,1,0,0,
	0,0,1,0,0,1,0,0,
	0,0,0,1,1,0,0,0,
// 'W'
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	0,1,0,0,0,0,1,0,
	0,1,0,1,1,0,1,0,
	0,1,0,1,1,0,1,0,
	0,0,1,0,0,1,0,0,
// 'X'
	0,1,0,0,0,0,1,0,
	0,0,1,0,0,1,0,0,
	0,0,0,1,1,0,0,0,
	0,0,0,1,1,0,0,0,
	0,0,1,0,0,1,0,0,
	0,1,0,0,0,0,1,0,
// 'Y'
	0,0,1,0,0,0,1,0,
	0,0,1,0,0,0,1,0,
	0,0,0,1,1,1,0,0,
	0,0,0,0,1,0,0,0,
	0,0,0,0,1,0,0,0,
	0,0,0,0,1,0,0,0,
// 'Z'
	0,0,1,1,1,1,0,0,
	0,0,0,0,0,1,0,0,
	0,0,0,0,1,0,0,0,
	0,0,0,1,0,0,0,0,
	0,0,1,0,0,0,0,0,
	0,0,1,1,1,1,0,0,
// '0'
	0,0,0,1,1,0,0,0,
	0,0,1,0,0,1,0,0,
	0,0,1,0,0,1,0,0,
	0,0,1,0,0,1,0,0,
	0,0,1,0,0,1,0,0,
	0,0,0,1,1,0,0,0,
// '1'
	0,0,0,0,1,0,0,0,
	0,0,0,1,1,0,0,0,
	0,0,0,0,1,0,0,0,
	0,0,0,0,1,0,0,0,
	0,0,0,0,1,0,0,0,
	0,0,0,1,1,1,0,0,
// '2'
	0,0,0,1,1,1,0,0,
	0,0,1,0,0,0,1,0,
	0,0,0,0,0,0,1,0,
	0,0,0,0,1,1,0,0,
	0,0,0,1,0,0,0,0,
	0,0,1,1,1,1,1,0,
// '3'
	0,0,1,1,1,1,1,0,
	0,0,0,0,0,1,0,0,
	0,0,0,0,1,1,0,0,
	0,0,0,0,0,0,1,0,
	0,0,1,0,0,0,1,0,
	0,0,0,1,1,1,0,0,
// '4'
	0,0,0,0,0,1,0,0,
	0,0,0,0,1,1,0,0,
	0,0,0,1,0,1,0,0,
	0,0,1,1,1,1,1,0,
	0,0,0,0,0,1,0,0,
	0,0,0,0,0,1,0,0,
// '5'
	0,0,1,1,1,1,1,0,
	0,0,1,0,0,0,0,0,
	0,0,1,1,1,1,0,0,
	0,0,0,0,0,0,1,0,
	0,0,1,0,0,0,1,0,
	0,0,0,1,1,1,0,0,
// '6'
	0,0,0,1,1,0,0,0,
	0,0,1,0,0,0,0,0,
	0,0,1,1,1,0,0,0,
	0,0,1,0,0,1,0,0,
	0,0,1,0,0,1,0,0,
	0,0,0,1,1,0,0,0,
// '7'
	0,0,1,1,1,1,0,0,
	0,0,0,0,0,1,0,0,
	0,0,0,0,1,0,0,0,
	0,0,0,0,1,0,0,0,
	0,0,0,1,0,0,0,0,
	0,0,0,1,0,0,0,0,
// '8'
	0,0,0,1,1,0,0,0,
	0,0,1,0,0,1,0,0,
	0,0,0,1,1,0,0,0,
	0,0,1,0,0,1,0,0,
	0,0,1,0,0,1,0,0,
	0,0,0,1,1,0,0,0,
// '9'
	0,0,0,1,1,0,0,0,
	0,0,1,0,0,1,0,0,
	0,0,1,0,0,1,0,0,
	0,0,0,1,1,1,0,0,
	0,0,0,0,0,1,0,0,
	0,0,0,1,1,0,0,0,

	// 0,0,0,0,0,0,0,0,
	// 0,0,0,0,0,0,0,0,
	// 0,0,0,0,0,0,0,0,
	// 0,0,0,0,0,0,0,0,
	// 0,0,0,0,0,0,0,0,
	// 0,0,0,0,0,0,0,0,
};

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
void spi_Config(void)
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

void spi_Reset(void) {
	SPI_RESET_ON;
	time_Timer3(5);
	SPI_RESET_OFF;
	time_Timer3(5);
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
	spi_Config();

	SPI_POWER_ON_VDD;
	time_Timer3(1000); // 1 millisecond

	SPI_DISPLAY_MODE_COMMAND;
	spi_send_recv(0xAE);

	SPI_RESET_ON;
	time_Timer3(50); // 50 microseconds
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

	spi_send_recv(0xA1);
	spi_send_recv(0xC8);
	
	spi_send_recv(0xDA);
	spi_send_recv(0x20);

	// spi_send_recv(0x2E); // Deactivate Scroll

	spi_send_recv(0x20); // Horizontal Address Mode
	spi_send_recv(0x00);

	spi_send_recv(0x81);
	spi_send_recv(0xff);

	spi_send_recv(0x22);
	spi_send_recv(0x0);
	spi_send_recv(0x3);

	// spi_send_recv(0x21); // Set Column Start-End Address
	// spi_send_recv(0x00);
	// spi_send_recv(0x7f);
}

void disp_Write()
{
	uint8_t val = 0;
	int i, j, k;

	SPI_DISPLAY_MODE_DATA;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 128; j++) {
			for (k = 0; k < 8; k++) {
				val |= d_mat[i * 8 + k][j] << k;
			}
			spi_send_recv(val);
			val = 0;
		}
	}
}

/*
Description: Selects and returns relevant section of font array based on parameter
Param: ASCII byte representing a number or a letter, 48 item array
Result: 48 item array (0/1), representing a matrix 8 cols x 6 rows (to leave some whitespace top & bottom)
*/
void CharToArr_h(char c, uint8_t* ca) {
	int i, ci = (int) c, mult;
	for (i = 0; i < 48; i++)
		ca[i] = 0;

	if (ci <= 0x39)
		mult = 26 + (ci - '0');
	else if (ci <= 0x5A)
		mult = ci - 'A'; // Offset from A
	
	for (i = 0; i < 48; i++) {
		ca[i] = font[i + 48*mult];
	}
}

void CharToArr_v(char c, uint8_t* ca) {
	int i, ci = (int) c, mult;
	for (i = 0; i < 48; i++)
		ca[i] = 0;

	if (ci <= 0x39)
		mult = 26 + (ci - '0');
	else if (ci <= 0x5A)
		mult = ci - 'A'; // Offset from A
	
	/* Pattern for 270 deg rot:
	index: 3 - col# + (row# * 4)
	*/
	/* Pattern for 90 deg rotation:
	index: col# * 4 + 3 - row#
	*/
	// (40 + row#) - (col# * 8)
	for (i = 0; i < 48; i++) {
		// ca[i] = font[(i % 8) * 6 + 3 - ((i/8) * 6) + 48*mult];
		// ca[i] = font[i + 48 * mult];
		ca[i] = font[(40 + i/6) - ((i % 8) * 8) + 48*mult];
	}

}

void disp_Text(char* str, uint8_t page, uint8_t col) {
	int index = 0;
	uint8_t cArr[48];

	page &= 3;
	uint8_t row = page * 8 + 1;
	int i, j;
	while (str[index] != 0) {
		CharToArr_h(str[index], cArr);
		for (i = 0; i < 6; i++) {
			for (j = 0; j < 8; j++) {
				d_mat[row + i][col + j + index * 8] = cArr[i * 8 + j];
			}
		}
		index++;
	}
}

void disp_VerticalText(char* str, uint8_t xOffset, uint8_t yOffset) {
	int index = 0;
	uint8_t cArr[48];

	int i, j;
	while (str[index] != 0) {
		CharToArr_v(str[index], cArr);
		for (i = 0; i < 6; i++) {
			for (j = 0; j < 8; j++) {
				d_mat[yOffset + i + index * 8][xOffset + j] = cArr[i * 8 + j];
			}
		}
		index++;
	}
}