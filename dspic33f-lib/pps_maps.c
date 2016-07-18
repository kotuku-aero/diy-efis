/*
diy-efis
Copyright (C) 2016 Kotuku Aerospace Limited

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice and
the GPL3 are subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.


If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the origional copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
#include "pps_maps.h"

#include <stdint.h>

/**
 * Map an output pin to a device
 * @param pin       pin  0..13 not checked for device
 * @param value     map to assign
 */
void map_rpo(rpo_pin pin, rpo_function value)
  {
  uint8_t *reg_ptr = ((uint8_t *)pin);       // value is 0x670
  
  *reg_ptr = value;
  }

void map_rpi(rpi_pin pin, rpi_function value)
  {
  uint8_t *port_base = ((uint8_t *)0x06A0);
  
  port_base[value] = pin;
  
  switch(pin)
    {
    case rpi_rpi16 :
      TRISAbits.TRISA0 = 1;
      break;
    case rpi_rpi17 :
      TRISAbits.TRISA1 = 1;
        break;
#if defined(__dsPIC33EP512GP504__)
    case rpi_rpi18 :
      TRISAbits.TRISA2 = 1;
        break;
    case rpi_rpi19 :
      TRISAbits.TRISA3 = 1;
        break;
#endif
    case rpi_rpi32 :
      TRISBbits.TRISB0 = 1;
        break;
    case rpi_rpi33 :
      TRISBbits.TRISB1 = 1;
        break;
    case rpi_rpi34:
      TRISBbits.TRISB2 = 1;
        break;
    case rpi_rp35 :
      TRISBbits.TRISB3 = 1;
        break;
    case rpi_rpi44 :
      TRISBbits.TRISB12 = 1;
        break;
    case rpi_rpi45 :
      TRISBbits.TRISB13 = 1;
        break;
    case rpi_rpi46 :
      TRISBbits.TRISB14 = 1;
        break;
    case rpi_rpi47 :
      TRISBbits.TRISB15 = 1;
        break;
    case rpi_rpi25 :
      TRISAbits.TRISA9 = 1;
        break;
#if defined(__dsPIC33EV256GM104__) | defined(__dsPIC33EV256GM004__) | defined(__dsPIC33EV256GM106__) | defined(__dsPIC33EV256GM006__)
    case rpi_rp20 :
        break;
    case rpi_rpi24 :
        break;
    case rpi_rpi27 :
        break;
    case rpi_rpi28 :
        break;
    case rpi_rp36 :
        break;
    case rpi_rp37 :
        break;
    case rpi_rp38 :
        break;
    case rpi_rp39 :
        break;
    case rpi_rp40 :
        break;
    case rpi_rp48 :
        break;
    case rpi_rpi50 :
        break;
    case rpi_rpi51 :
        break;
    case rpi_rpi52 :
        break;
    case rpi_rpi53 :
        break;
    case rpi_rp54 :
        break;
    case rpi_rp55 :
        break;
    case rpi_rp56 :
        break;
    case rpi_rp57 :
        break;
    case rpi_rpi58 :
        break;
    case rpi_rpi60 :
        break;
    case rpi_rpi61 :
        break;
    case rpi_rpi63 :
        break;
    case rpi_rp69 :
        break;
    case rpi_rp70 :
        break;
    case rpi_rpi72 :
        break;
    case rpi_rp41 :
        break;
    case rpi_rp42 :
        break;
    case rpi_rp43 :
        break;
    case rpi_rp49 :
        break;
    case rpi_rpi96 :
        break;
    case rpi_rp97 :
        break;
    case rpi_rpi94 :
        break;
    case rpi_rpi95 :
        break;
    case rpi_rp118 :
        break;
    case rpi_rpi119 :
        break;
    case rpi_rp120 :
        break;
    case rpi_rpi121 :
        break;
    case rpi_rpi124 :
        break;
    case rpi_rp125 :
        break;
    case rpi_rp126 :
        break;
    case rpi_rp127 :
        break; 
#endif
    }
  }