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

//////////// SPI_lib.h is the driver to bit-bang the SPI protocol in this project ////////////

#ifndef _SPI_
#define _SPI_

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

unsigned int16 SPI__PULSE;

void SPI_FREQ_KHZ(unsigned int16 SPI__FREQ){
   // the pulse calculated should be in us since we are using the delay_us function
   // (freq in Hz) = (freq in kHz)*1000
   // (period in s) = 1/(freq in Hz)
   // (period in us) = (period in s)/1000000
   //                = (1/(freq in kHz)*1000)/1000000
   //                = 1000/(freq in kHz)
   // pulse = (period in us)/2
   //       = (1000/(freq in kHz))/2
   //       = 500/(freq in kHz)
   SPI__PULSE = (unsigned int16)(500/SPI__FREQ);
   if(SPI__PULSE == 0){
      SPI__PULSE = 1;                         // setting a minimum of 1us to avoid getting SPI__PULSE=0
   }
}

void SPI__INIT(void){
   SCK_T = 0;
   SCK=0;
   
   MOSI_T = 0;
   MOSI=1;
   
   CS_T = 1;
   MISO_T = 1;
}

// note:  CS line is connected to a pull-up resistor
void SPI__CS(void){                           // selecting the CS line
   SCK=0;
   CS_T=0;
   CS=0;
}

void SPI__DONE(void){                         // releasing the CS line
   SCK=0;
   CS_T=1;
}

unsigned int8 SPI__READ(unsigned int1 NB){    // NB is used to indicate the number of consecutive reads to be done
   unsigned int8 data;
   unsigned int i;

   delay_us(1);
   for(i=0; i<8; i++){
      SCK=0;
      data <<= 1;
      if(MISO==1){
         data |= 1;
      }
      delay_us(1);              // should be delay_us(SPI__PULSE); but proteus is changing this variable to 65535 if unsigned int16 and to 255 if unsigned int8
      SCK=1;
      delay_us(1);              // should be delay_us(SPI__PULSE); but proteus is changing this variable to 65535 if unsigned int16 and to 255 if unsigned int8
   }
   SCK=0;
   // 8 bits were read
   
   if(NB==1){                                 // Number of Bytes is 1
      CS_T=1;                                 // end the READ
   }
   else{                                      // Number of Bytes is !1
      CS_T=0;
      CS=0;                                   // keep the Chip Select down to keep reading
   }
   
   return(data);
}

void SPI__WRITE(unsigned int8 data){
   unsigned int i;
   for(i=0 ; i<8 ; i++){                      // send data MSB first
      SCK=0;
      if( (data & 0x80) == 0x80 ){
         MOSI=1;
      }
      else{
         MOSI=0;
      }
      delay_us(1);              // should be delay_us(SPI__PULSE); but proteus is changing this variable to 65535 if unsigned int16 and to 255 if unsigned int8
      SCK=1;
      delay_us(1);              // should be delay_us(SPI__PULSE); but proteus is changing this variable to 65535 if unsigned int16 and to 255 if unsigned int8
      data <<= 1;
   }
   SCK=0;
}

#endif
