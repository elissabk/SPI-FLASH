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

//////////// SPI_main.h has all the setup needed to run the SPI_main.c program ////////////

#include <18F25K22.h>
#device ADC=8
#use delay(internal=64000000)

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

#include <SPI_lib.h>
#include <AT25F512.c>

