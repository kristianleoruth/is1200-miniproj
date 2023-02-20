#include <stdint.h>

/* SPI */
void spi_PowerOnDisplay(void); // Kristian
uint8_t spi_send_recv(uint8_t data);

#define SPI_DISPLAY_MODE_COMMAND (PORTF &= 0xffffffef) // Pin 39 (D/C#) is RF4, set to 0 is Command Mode
#define SPI_DISPLAY_MODE_DATA (PORTF |= 0x10) // 1 is Data Mode

/* Display functions */
void disp_Write();
void disp_Text(char* str, uint8_t page, uint8_t col);
uint8_t d_mat[32][128];

/* Switches and Btns */
int btn1(void);
int btn2(void);

/* Timer functions */
void time_Timer3(int time);
void time_Tick();

/* ADC */
void adc_Init(void); // Lennart
int adc_GetDial(void);

