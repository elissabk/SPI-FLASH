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

/////////////////////////////////////////////////////////////////////////////////////////////////
//  This project aims to communicate with a FLASH memory by bit-banging the SPI protocol.
/////////////////////////////////////////////////////////////////////////////////////////////////


#include <SPI_main.h>

void main(void){
   unsigned int8 k;
   SPI__INIT();
   SPI_FREQ_KHZ(20000);
   FLASH__INIT();
   delay_ms(200);
   FLASH_CHIP_ERASE();          //just to start on a clear flash memory
   
   for(k=0 ; k<32 ; k++){
      FLASH__PROGRAM(k,1,&k);
   }
   
   unsigned int8 i;
   unsigned int8 data[17];
   for(i=0 ; i<17 ; i++){
      data[i] = 128 + i ;
   }
   
   FLASH__PROGRAM(0, 17, &data);
   FLASH__PROGRAM(257, 17, &data);
   
   unsigned int8 data_r1[127];
   FLASH__READ(0,127,data_r1[0]);
   
   unsigned int8 data_r2[16];
   FLASH__READ(256,16,data_r2[0]);
   
   output_toggle(PIN_B5);       //to check if the micro is working and done
   delay_ms(1000);
   
}
