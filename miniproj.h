#include <stdint.h>
#include <stdlib.h>

/* SPI */
void spi_PowerOnDisplay(void); // Kristian
uint8_t spi_send_recv(uint8_t data);
void spi_Reset(void);

#define SPI_DISPLAY_MODE_COMMAND (PORTF &= 0xffffffef) // Pin 39 (D/C#) is RF4, set to 0 is Command Mode
#define SPI_DISPLAY_MODE_DATA (PORTF |= 0x10) // 1 is Data Mode

/* Display functions */
void disp_Write();
void disp_Text(char* str, uint8_t page, uint8_t col);
uint8_t d_mat[32][128];

/* Game */
void game_Init();
void game_Update();

/* Switches and Btns */
int btn(int select);

/* Timer functions */
void time_Timer3(int time);
void time_Tick();

/* ADC */
void adc_Init(void); // Lennart
int adc_GetDial(void);
