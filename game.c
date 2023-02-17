#include <stdint.h>
#include <stdio.h>
#include <pic32mx.h>
#include "miniproj.h"

void disp_Write(const uint8_t pix[128][32])
{
    int i, j, k;
    uint8_t value = 0;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 128; j++)
        {
            for(k = 0; k < 8; k++) {
                value |= (pix[j][(i*8)+k])<<k;
            }

            spi_send_recv(value);
            value = 0;
        }
    }
}