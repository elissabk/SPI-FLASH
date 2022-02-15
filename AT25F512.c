// Copyright 2021 Elissa Bou Karam
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//       http://www.apache.org/licenses/LICENSE-2.0
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

//// AT25F512.c is the driver needed to communicate with the FLASH being used in this project ////

#include <SPI_lib.h>

#ifndef _AT25F512A_
#define _AT25F512A_


#ifndef _SPI_PINS_
#define _SPI_PINS_

#byte TRISB = 0xF93
#bit SCK_T = TRISB.0
#bit MOSI_T = TRISB.1
#bit MISO_T = TRISB.2
#bit CS_T = TRISB.3
#bit WP_T = TRISB.6
#bit HOLD_T = TRISB.7

#byte PORTB = 0xF81
#bit SCK = PORTB.0                            //clock pin
#bit MOSI = PORTB.1                           //Master-Out/Slave-In pin
#bit MISO = PORTB.2                           //Master-In/Slave-Out pin
#bit CS = PORTB.3                             //Chip Select pin
#bit WP = PORTB.6                             //Write Protect pin
#bit HOLD = PORTB.7                           //Hold pin

#endif


#ifndef _FLASH_INSTRUCTIONS_
#define _FLASH_INSTRUCTIONS_

#define WREN 0b00000110
#define WRDI 0b00000100
#define RDSR 0b00000101
#define WRSR 0b00000001
#define READ 0b00000011
#define PROGRAM 0b00000010
#define SECTOR_ERASE 0b01010010
#define CHIP_ERASE 0b01100010
#define RDID 0b00010101

#endif


void FLASH_SEND_INSTRUCTION(unsigned int8 OP_CODE){
   SPI__CS();
   SPI__WRITE(OP_CODE);
}

void FLASH__INIT(void){
   WP_T = 0;
   WP = 1;
   
   HOLD_T = 0;
   HOLD = 1;
}

void FLASH__WREN(void){                       // Write Enable function, must be called before everytime we need to write to the FLASH
   FLASH_SEND_INSTRUCTION(WREN);
   SPI__DONE();
}

void FLASH__WRDI(void){                       // Write Disable function
   FLASH__WREN();
   FLASH_SEND_INSTRUCTION(WRDI);
   SPI__DONE();
}

unsigned int8 FLASH__RDSR(void){              // Read Status Register funsction
   FLASH_SEND_INSTRUCTION(RDSR);
   SPI__DONE();
   return(SPI__READ(1));
}

unsigned int16 FLASH__RDID(void){             // Read Product ID function
   unsigned int8 ID1, ID2;
   unsigned int16 ID;
   FLASH_SEND_INSTRUCTION(RDID);
   ID1 = SPI__READ(0);                        // 1st byte = manufacturer code (1FH = ATMEL)
   ID2 = SPI__READ(1);                        // 2nd code = device code
   SPI__DONE();
   
   ID = ID1;
   ID <<= 8;
   ID |= ID2;
   return(ID);
}

void FLASH__WRSR(unsigned int8 status_reg){   // Write Status Register function
   FLASH__WREN();
   FLASH_SEND_INSTRUCTION(WRSR);
   SPI__WRITE(status_reg);
   SPI__DONE();
}

void FLASH_WR_ADDRESS(unsigned int16 address){
   //this function will only be used internally to send out the address
   // there is no need to call FLASH__WREN() and SPI__CS() in here since they were already done before calling FLASH_WR_ADDRESS() function
   // address must be sent as a 3-byte address
   unsigned int8 j, k, address_8;
   SPI__WRITE(0x00);                        // first byte in this FLASH is 00 since highest address is 00FFFF
   for(k=0; k<2 ; k++){
      for(j=8; j>0; j--){
         if( (address & 0x8000) == 0x8000 ){
            bit_set(address_8,(j-1));
         }
         else{
            bit_clear(address_8,(j-1));
         }
         address <<= 1;  
      }
      SPI__WRITE(address_8);
      //second and third bytes
   }
}

void FLASH__READ(unsigned int16 address, unsigned int16 length, unsigned int8 *data_r){   // Read function
   FLASH_SEND_INSTRUCTION(READ);
   FLASH_WR_ADDRESS(address);
   
   if( length > 1 ){
      unsigned int16 i, len_new;
      if( (address+length) >= 0xFFFF){
         len_new = 0xFFFF;                    // in this FLASH, reading must end at the highest address 00FFFF, cannot roll over to the first address
      }
      else{
         len_new = address + length;
      }
      for(i=address ; i<len_new ; i++){
         *data_r++ = SPI__READ(0);
      }      
   }
   *data_r = SPI__READ(1);
   SPI__DONE();
}

void FLASH_SECTOR_ERASE(unsigned int16 address){   // Sector Erase function
   FLASH__WREN();
   FLASH_SEND_INSTRUCTION(SECTOR_ERASE);
   FLASH_WR_ADDRESS(address);
   SPI__DONE();
   delay_ms(1000);                            //time needed for the FLASH to finish SECTOR_ERASE
}

void FLASH_CHIP_ERASE(void){                  // Chip erase function
   FLASH__WREN();
   FLASH_SEND_INSTRUCTION(CHIP_ERASE);
   SPI__DONE();
   delay_ms(3500);                            //time needed for the FLASH to finish CHIP_ERASE
}

void FLASH__PROGRAM(unsigned int16 address, unsigned int16 length, unsigned int8 *data_wr){    // Program (or write) function
   unsigned int8 sector[1024];                // should be 32768, but this PIC doesn't have enough memory space
   unsigned int16 sector_size = 1024;         // should be 32768, but this PIC doesn't have enough memory space
   unsigned int16 starting_add;
   unsigned int16 i;
   
   if( address <= 0x7FFF ){                   // reading the sector and storing it in an array
      starting_add = 0x0000;
      FLASH__READ(0x0000, sector_size, sector[0]);
   }
   else if( address >= 0x8000 ){
      starting_add = 0x8000;
      FLASH__READ(0x8000, sector_size, sector[0]);
   }
   
   FLASH_SECTOR_ERASE(address);               // erasing the entire sector
   
   FLASH__WREN();
   FLASH_SEND_INSTRUCTION(PROGRAM);
   FLASH_WR_ADDRESS(address);
                                              // writing the entire sector
   for(i=starting_add ; i<(starting_add+sector_size) ; i++){
      if( ( i >= address ) && ( i <= (address+length) ) ){
         SPI__WRITE(*data_wr++);              // writing the new data we are adding to the FLASH on the specified addresses
      }
      else{
         SPI__WRITE(sector[i]);               // writing what was already in the FLASH on the addresses we don't want to change
      }
   }
   SPI__DONE();
   delay_us(60*sector_size);                  // FLASH needs 60us/byte to finish program
}

void FLASH__HOLD(void){                       // HOLD function can be used to pause instead of stopping
   CS_T=0;
   CS=0;
   SCK=0;
   HOLD=0;
}

void FLASH__RESUME(void){                     // Resume function can be used to continue from were we paused instead of restarting from the beginning
   CS_T=0;
   CS=0;
   SCK=0;
   HOLD=1;
}

#endif
