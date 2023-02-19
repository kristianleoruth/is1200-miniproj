#include <stdint.h>

/* SPI functions */
void spi_PowerOnDisplay(void); // Kristian
uint8_t spi_send_recv(uint8_t data);

/* Display functions */
void disp_Write(const uint8_t pix[32][128]); // Lennart


/* Timer functions */
void time_Timer3(int time);
void time_Tick(int tickMult);

/* ADC */
void adc_Init(void); // Lennart