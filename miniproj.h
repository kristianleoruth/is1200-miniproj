#include <stdint.h>

/* SPI */
void spi_PowerOnDisplay(void); // Kristian
uint8_t spi_send_recv(uint8_t data);

#define SPI_DISPLAY_MODE_COMMAND (PORTF &= 0xffffffef) // Pin 39 (D/C#) is RF4, set to 0 is Command Mode
#define SPI_DISPLAY_MODE_DATA (PORTF |= 0x10) // 1 is Data Mode

/* Display functions */
void disp_Write(uint8_t pix[32][128]); // Lennart


/* Timer functions */
void time_Timer3(int time);
void time_Tick();

/* ADC */
void adc_Init(void); // Lennart