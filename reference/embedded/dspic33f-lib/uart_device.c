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
#include "uart_device.h"
#include "microkernel.h"
#include <string.h>
#include <ctype.h>
#include <p33Exxxx.h>

static void process_char(uart_config_t *uart, char ch)
  {
  if((uart->flags & NMEA_DECODER_ENABLE) != 0)
    {
    ch &= 0x7f;
    if(uart->state == us_scan)
      {
      if(ch == '$')
        {
        uart->rx_offset = 0;
        uart->state = us_sentence;
        }
      }
    else
      {
      switch(ch)
        {
        case '\r':
          break; // ignore characters
        case '\n':
          // if we are about to send the checksum then we start scanning
          if(ch == '\n')
            uart->state = us_scan;
          
          // terminate the string
          uart->rx_buffer[uart->rx_offset] = 0;
          uart->rx_offset = 0;
          
          // push back the message, don't wait if full..
          push_back_from_isr(&uart->rx_queue, uart->rx_buffer);
          break;
        default:
          uart->rx_buffer[uart->rx_offset++] = ch;
          uart->rx_buffer[uart->rx_offset] = 0;

          // we allow 1 byte at end for terminating 0 byte
          if(uart->rx_offset > uart->rx_length-2)
            uart->state = us_scan; // give up as buffer over-run
          break;
        }
      }
    }
  else
    {
    // store the character in the buffer
    uart->rx_buffer[uart->rx_offset++] = ch;

    if((uart->flags & UART_EOL_CHAR  && ch == uart->eol_char) ||
       uart->rx_offset > (uart->rx_length - 2))
      {
      // place a null at the end of the string
      uart->rx_buffer[uart->rx_offset] = 0;
          
      // push back the message, don't wait if full..
      push_back_from_isr(&uart->rx_queue, uart->rx_buffer);
      }
    }
  }

char to_hex(unsigned char value)
  {
  value &= 0x0f;
  if(value > 10)
    return 'A' + value - 10;

  return '0' + value;
  }

static uart_config_t *uart1; // set once the uart is configured

void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void)
  {
  IFS0bits.U1TXIF = 0;

  set_event_from_isr(&uart1->uart_tx_ready);
  }

void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void)
  {
  /* Read the receive receive_buffer till no more characters can be read */
  while(U1STAbits.URXDA)
    process_char(uart1, U1RXREG);

  if(U1STAbits.OERR)
    U1STAbits.OERR = 0;

  if(U1STAbits.FERR)
    U1STAbits.FERR = 0;

  IFS0bits.U1RXIF = 0;
  }

#ifdef _U2RXIF
static uart_config_t *uart2;

void __attribute__((interrupt, no_auto_psv)) _U2TXInterrupt(void)
  {
  IFS1bits.U2TXIF = 0;

  set_event_from_isr(&uart2->uart_tx_ready);
  }


void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt(void)
  {
  /* Read the receive receive_buffer till no more characters can be read */
  while(U2STAbits.URXDA)
    process_char(uart2, U2RXREG);

  if(U2STAbits.OERR)
    U2STAbits.OERR = 0;

  if(U2STAbits.FERR)
    U2STAbits.FERR = 0;

  IFS1bits.U2RXIF = 0;
  }

#endif
  
#ifndef SYSCLK
#define SYSCLK 70000000
#endif

bool init_uart(uart_config_t *p_uart,
               uint16_t *rx_worker_stack,
               uint16_t rx_worker_stack_length,
               uint16_t *tx_worker_stack,
               uint16_t tx_worker_stack_length)
  {
  uint32_t divisor = (SYSCLK / ((uint32_t)(p_uart->rate << 4)))-1;
  // set the baud rate and init the interrupts for the uart
  // TODO: check the pin assignments I thing uart1 is altio
  switch(p_uart->uart_number)
    {
    case 1:
      uart1 = p_uart;

      map_rpo(p_uart->tx_pin, rpo_u1tx);
      map_rpi(p_uart->rx_pin, rpi_u1rx);
      
      U1MODEbits.UARTEN = 1;
      
      U1BRG = (unsigned int)divisor;

      U1MODEbits.STSEL = 1;
      U1MODEbits.PDSEL = 0;

      U1STAbits.URXISEL = 0;
      U1STAbits.ADDEN = 0;
      U1STAbits.UTXEN = 1;

      // enable the interrupts
      IPC3bits.U1TXIP = 4;
      IPC2bits.U1RXIP = 4;
      IFS0bits.U1TXIF = 0;
      IEC0bits.U1TXIE = 1;
      IFS0bits.U1RXIF = 0;
      IEC0bits.U1RXIE = 1;

      if((p_uart->flags & UART_RTS_ENABLE) != 0)
        {
        map_rpo(p_uart->rts_pin, rpo_u1rts);
        map_rpi(p_uart->cts_pin, rpi_u1cts);
        }
      
      if(U1STAbits.UTXBF == 0)
        set_event(&p_uart->uart_tx_ready);
      
      break;
    case 2:
      uart2 = p_uart;

      map_rpo(p_uart->tx_pin, rpo_u2tx);
      map_rpi(p_uart->rx_pin, rpi_u1rx);
      
      U2MODEbits.UARTEN = 1;
      U2BRG = p_uart->rate;

      U2MODEbits.STSEL = 0;
      U2MODEbits.PDSEL = 0;

      U2STAbits.URXISEL = 0;
      U2STAbits.ADDEN = 0;
      U2STAbits.UTXEN = 1;

      // enable the interrupts
      IPC7bits.U2TXIP = 4;
      IPC7bits.U2RXIP = 4;
      IFS1bits.U2TXIF = 0;
      IEC1bits.U2TXIE = 1;
      IFS1bits.U2RXIF = 0;
      IEC1bits.U2RXIE = 1;

      if((p_uart->flags & UART_RTS_ENABLE) != 0)
        {
        map_rpo(p_uart->rts_pin, rpo_u2rts);
        map_rpi(p_uart->cts_pin, rpi_u2cts);
        }
      
      if(U2STAbits.UTXBF == 0)
        set_event(&p_uart->uart_tx_ready);
      break;
    }

  if(p_uart->rx_buffer != 0)
    resume(p_uart->rx_worker_pid = create_task("UART_RX", rx_worker_stack, rx_worker_stack_length, uart_rx_worker, p_uart, NORMAL_PRIORITY));
  
  if(p_uart->tx_buffer != 0)
    resume(p_uart->tx_worker_pid = create_task("UART_TX", tx_worker_stack, tx_worker_stack_length, uart_tx_worker, p_uart, NORMAL_PRIORITY));

  return true;
  }

static unsigned char hex_to_digit(char ch)
  {
  ch = (ch >= 'A' && ch <= 'Z') ? ch - ('A' - 'a') : ch;
  unsigned char value = ch - '0';
  if(value > 9)
    value = value - ('a' - '9' - 1);
  return value;
  }

static bool is_alphanum(char ch)
  {
  ch = (ch >= 'A' && ch <= 'Z') ? ch - ('A' - 'a') : ch;
  
  return (ch >= '0' && ch <= '9') ||
    (ch >= 'a' && ch <= 'f');
  }

void uart_rx_worker(void *parg)
  {
  uart_config_t *uart = (uart_config_t *) parg;

  while(true)
    {
    pop_front(&uart->rx_queue, uart->rx_worker_buffer, INDEFINITE_WAIT);
    
    if((uart->flags & NMEA_DECODER_ENABLE) != 0)
      {
      char *start = (char *) uart->rx_worker_buffer;
      char *end = start;
      uint8_t sentence = 0;
      uint8_t word = 0;
      uint8_t checksum = 0;
      bool checksum_marker = false;
      
      // calculate the checksum of the string
      while(*start != 0 && *start != '*')
        checksum ^= *start++;

      start = end;
      
      while(*start != 0)
        {
        if(checksum_marker)         // checksum marker
          {
                    // compare the checksum
          uint8_t chk = 0;
          if(is_alphanum(start[0]) &&
             is_alphanum(start[1]) &&
             start[2] == 0)
            {
            chk = hex_to_digit(start[0]) << 4;
            chk += hex_to_digit(start[1]);
            }

          (uart->callback.nmea_callback)(uart, 
            checksum != chk ? 0 : (char *) 1,
            sentence, word);

          start++;
          break;                  // and done, wait for next
          }
        
        for(end = start; *end != 0 && *end != ',' && *end != '*' ; end++);
        
        if(*end == '*')
          checksum_marker = true;
        
        *end = 0;
        
        sentence = (uart->callback.nmea_callback)(uart, start, sentence, word);
        word++;
        
        // skip token
        start = end;
        // and increment to next word
        start++;
        }
      }
    else
      (uart->callback.uart_callback)(uart, uart->rx_worker_buffer, strlen((const char *)uart->rx_worker_buffer));
    }
  }

void uart_tx_worker(void *parg)
  {
  uart_config_t *uart = (uart_config_t *) parg;

  while(true)
    {
    if(uart->tx_worker_offset >= uart->tx_length)
      {
      // wait for a queued message
      pop_front(&uart->tx_queue, uart->tx_worker_buffer, INDEFINITE_WAIT);
      uart->tx_worker_offset = 0;
      }
    
    while(uart->tx_worker_offset < uart->tx_length)
      {
      bool should_block = false;
      uint8_t ch;
      
      if(uart->uart_number == 1)
        {
        while(U1STAbits.UTXBF == 1)
          wait_for_event(&uart->uart_tx_ready, 1);
        }
      else
        {
        while(U2STAbits.UTXBF == 1)
          wait_for_event(&uart->uart_tx_ready, 1);
        }

      ch = uart->tx_worker_buffer[uart->tx_worker_offset++];
      if(uart->uart_number == 1)
        U1TXREG = ch;
      else
        U2TXREG = ch;
      
      if(uart->flags & UART_EOL_CHAR && ch == uart->eol_char)
        {
        // flag end of buffer.
        uart->tx_worker_offset = uart->tx_length;
        break;
        }
      }
    }
  }

bool write_uart(uart_config_t *p_uart, uint8_t data)
  {
  if(p_uart->tx_buffer == 0)
    return false;
  
  p_uart->tx_buffer[p_uart->tx_offset++] = data;
  
  if(p_uart->tx_offset >= p_uart->tx_length ||
     (p_uart->flags & UART_EOL_CHAR  && p_uart->eol_char == data))
    {
    push_back(&p_uart->tx_queue, p_uart->tx_buffer, INDEFINITE_WAIT);
    p_uart->tx_offset = 0;
    }

  return true;
  }

bool send_str(uart_config_t *p_uart, const char *str)
  {
  while(*str != 0)
    if(!write_uart(p_uart, *str))
      return false;

  return true;
  }

bool send_checksum(uart_config_t *p_uart, unsigned char chksum)
  {
  return write_uart(p_uart, '*') &&
    write_uart(p_uart, to_hex(chksum >> 4)) &&
    write_uart(p_uart, to_hex(chksum)) &&
    write_uart(p_uart, '\n');
  }


// send a string to the nmea (blocking call)

bool nmea_send_chars(uart_config_t *p_uart, const char *str, unsigned char *chksum)
  {
  while(*str != 0)
    {
    *chksum ^= *str;
    if(!write_uart(p_uart, *str))
      return false;
    str++;
    }

  return true;
  }

uint16_t write_buffer_avail(uart_config_t *config)
  {
  // we will always block so return the buffer length
  return capacity(&config->tx_queue);
  }
