#include <stdint.h>
#include <pic32mx.h>
#include "miniproj.h"

int main() {
	PORTE = 0;
	uint8_t d_matrix[32][128];
	int i, j;
	for (i = 0; i < 32; i++) {
		for (j = 0; j < 128; j++) {
			d_matrix[i][j] = 0;
		}
	}

	for (i = 0; i < 8; i++) {
		for (j = 0; j < 128; j++) {
			d_matrix[i][j] = 1;
		}
	}
	for (i = 8; i < 16; i++) {
		for (j = 0; j < 64; j++) {
			d_matrix[i][j] = 1;
		}
	}
	for (i = 16; i < 24; i++) {
		for (j = 0; j < 32; j++) {
			d_matrix[i][j] = 1;
		}
	}
	for (i = 24; i < 32; i++) {
		for (j = 0; j < 16; j++) {
			d_matrix[i][j] = 1;
		}
	}


	spi_PowerOnDisplay();

	disp_Write(d_matrix);

	while (1) {

	}
	return 0;
}
