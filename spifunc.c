#include <stdint.h>
#include <pic32mx.h>

#define SPI_DISPLAY_MODE_COMMAND (PORTF |= 0x10) // Pin 39 is RF4, set to 1 is Command Mode
#define SPI_DISPLAY_MODE_DATA (PORTF &= 0xffffffef) // 0 is Data Mode

// Set Vdd Enable (Pin 38, RF6)
#define SPI_POWER_ON_VDD (PORTF |= 0x40)
