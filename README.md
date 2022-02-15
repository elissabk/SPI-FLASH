# SPI-FLASH
This project aims to bit-bang the SPI protocol and create a driver to communicate with a FLASH memory. The project uses the PIC18F25K22 microcontroller and the AT25F512 FLASH memory.

This project covers the same main functionalities used in the I2C-EEPROM project:
SPI driver
FLASH read/write driver
main loop to access the FLASH

In the proteus simulation, an SPI debugger is used to parse the SPI transactions made and show the start/stop/byte read/write.

To run the program, compile the code in CCS C Compiler and use the resulting .hex file as firmware for the PIC18F25K22 microcontroller in proteus.
