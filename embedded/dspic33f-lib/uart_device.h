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
#ifndef __uart_device_h__
#define __uart_device_h__

#include <stdint.h>
#include <stdbool.h>

#include "pps_maps.h"
#include "microkernel.h"
  
typedef uint32_t baud_rate;

#define UART_RTS_ENABLE         0x01
#define NMEA_DECODER_ENABLE     0x02
#define UART_EOL_CHAR           0x04      // if set then the eol char ends input

struct _uart_config_t;

// callback when a buffer is full or the EOL character is seen.
typedef void (*rx_callback)(struct _uart_config_t *, unsigned char *buffer, uint8_t length);

// The uart device handles NMEA813 format strings.  This means that when the
// message arrives the header is scanned for a '$' character.  The callback is called
// after each ',' or '*' is found.  The later being the checksum.
//
// on the first call the sentence is 0 and the word is 0, signifying the start
// the code returns the sentence ID.
// as each word is received the sentence and word (starting at 1) are returned
// when the * is found the checksum is calculated and if OK a callback is made
// for the next word. and the buffer will have either 'OK' or 'ERR' for a checksum error.
//
// NOTE: the sentence mus be 1..n as 0 marks a scan for the first '$' character
typedef uint8_t (*nmea_rx_callback)(struct _uart_config_t *, char *buffer, uint8_t sentence, uint8_t word);

typedef union _uart_callback_t
  {
  rx_callback uart_callback;
  nmea_rx_callback nmea_callback;
  } uart_callback_t;

typedef enum _nmea_state
{
  us_scan,            // wait for start of sentence
  us_sentence,        // start of sentence
  us_word,            // word
  us_checksum         // '*' found
} nmea_state;

typedef struct _uart_config_t
  {
  uint8_t uart_number;              // depends on device
  baud_rate rate;
  uint16_t flags;                   // see flags above
  uint8_t eol_char;                 // normally \n in uart and nmea mode
  deque_t rx_queue;                 // buffer for RX
  uint8_t *rx_buffer;               // temporary buffer
  uint8_t rx_length;                // length of the temporary buffer, should match rx_queue size
  uint8_t rx_offset;                // offset into the read buffer
  uint8_t *rx_worker_buffer;        // buffer for data read
  deque_t tx_queue;
  uint8_t tx_length;                // length of the buffer
  // write buffer
  uint8_t *tx_buffer;               // buffer filled by application
  uint8_t tx_offset;                // pointer into write buffer
  // send buffer
  uint8_t *tx_worker_buffer;        // buffer for data sent.
  uint8_t tx_worker_offset;         // pointer into worker buffer
  uint8_t rx_pps;                   // peripheral select rx
  uint8_t cts_pps;
  rpo_pin tx_pin;                   //peripheral select tx
  rpo_pin rts_pin;                  // peripheral pin select RTS
  rpi_pin rx_pin;                   // peripheral pin select rx
  rpi_pin cts_pin;                  // peripheral pin select tx
  uart_callback_t callback;         // depends on mode
  event_t uart_tx_ready;            // signal used for uart tx ready
  int8_t rx_worker_pid;             // process id of uart rx worker
  int8_t tx_worker_pid;             // process id of uart tx worker
  nmea_state state;                 // only used in nmea decoder mode
  } uart_config_t;

 
/**
 * initialize a uart and return the thread start argument
 * @param config        Uart configuration
 * @return  true if the uart can be configured.
 */
extern bool init_uart(uart_config_t *config,
                      uint16_t *rx_worker_stack,
                      uint16_t rx_worker_stack_length,
                      uint16_t *tx_worker_stack,
                      uint16_t tx_worker_stack_length);

/**
 * Worker process
 * @param optional parameter
 * 
 * this should be called after the init_uart call is made and any device initiatlization is
 * done.  This will block and call the callback argument after each buffer is read
 */
extern void uart_rx_worker(void *);
extern void uart_tx_worker(void *);

/**
 * Send a string to the uart
 * @param config      device to send to
 * @param str         text to send
 * @return true if the characters are queued
 */
extern bool send_str(uart_config_t *config, const char *str);
/**
 * Send a character to the uart
 * @param config
 * @param data
 * @return true if the character was sent
 */
extern bool write_uart(uart_config_t *config, unsigned char data);
/**
 * Return the number of characters available in the tx buffer
 * @param config
 * @return How many characters can be sent
 */
extern uint16_t write_buffer_avail(uart_config_t *config);
/**
 * send a string of characters to the nmea, does not prepend anything
 * @param config      device to send to
 * @param str         text to send
 * @param checksum    checksum to update
 * @return true if characters were sent
 */
extern bool nmea_send_chars(uart_config_t *config, const char *str, unsigned char *checksum);
/**
 * calculate lower 4 bits as hex
 * @param value value to convert to hex
 * @return value as hex nibble
 */
extern char to_hex(unsigned char value);
/**
 * send the checksum after a '*'
 * @param config    Uart to send to
 * @param chksum    checksum to send
 * @return true if characters queued
 */
extern bool send_checksum(uart_config_t *config, unsigned char chksum);

#endif
