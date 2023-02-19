#include <stdint.h>
#include <stdio.h>
#include <pic32mx.h>
#include "miniproj.h"

void disp_Write(uint8_t pix[32][128])
{
	SPI_DISPLAY_MODE_COMMAND;
	spi_send_recv(0x22);
	spi_send_recv(0x0);
	spi_send_recv(0x3);


	SPI_DISPLAY_MODE_DATA;
	uint8_t val = 0;
	int i, j, k;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 128; j++) {
			for (k = 0; k < 8; k++) {
				val |= pix[i * 8 + k][j] << k;
			}
			spi_send_recv(val);
			val = 0;
		}
	}

	// for (i = 0; i < 8; i++) {
	// 	val |= (pix[i][0] << i);
	// }

	// spi_send_recv(val);

	// uint8_t val = 0;
	// int i, j, k;
	// for (i = 0; i < 128; i++) {
	// 	for (j = 0; j < 4; j++) { // Page multiplier
	// 		for (k = 0; k < 8; k++) {
	// 			val |= (pix[j*8 + k][i]) << k;
	// 		}
	
	// 		spi_send_recv(val);
	// 		val = 0;
	// 	}
	// }
    // int i, j, k;
    // uint8_t value = 0;
    // for (i = 0; i < 4; i++)
    // {
    //     for (j = 0; j < 128; j++)
    //     {
    //         for(k = 0; k < 8; k++) {
    //             value |= (pix[j][(i*8)+k])<<k;
    //         }

    //         spi_send_recv(value);
    //         value = 0;
    //     }
    // }
}