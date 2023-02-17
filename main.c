#include <stdint.h>
#include <pic32mx.h>
#include "miniproj.h"

int main() {
	uint8_t[128][32] d_matrix;
	d_matrix[63][31] = 1;
	d_matrix[63][32] = 1;
	d_matrix[64][31] = 1;
	d_matrix[64][32] = 1;

	spi_PowerOnDisplay();
	while (1) {
		disp_Write(d_matrix);
	}
	return 0;
}
