#ifndef __uart_device_h__
#define __uart_device_h__

#include <stdint.h>
#include <stdbool.h>
#include "microkernel.h"
  
#define UART_RTS_ENABLE         0x01
#define NMEA_DECODER_ENABLE     0x02
#define UART_EOL_CHAR           0x04      // if set then the eol char ends input
#define UART_INTERACTIVE        0x08      // if set then every character is eol

#if defined(__dsPIC33EP512GP504__) | defined(__dsPIC33EP512GP506__) | defined(__dsPIC33EP256GP502__)
#define NUM_SERIAL_CHANNELS 2
#elif defined(__32MZ2048EFH144__) | defined(__32MZ2048EFH064__) | defined(__32MZ2064DAH176)
#define NUM_SERIAL_CHANNELS 6
#endif

struct _uart_config_t;

// callback when a buffer is full or the EOL character is seen.
typedef void (*rx_callback_fn)(struct _uart_config_t *, uint8_t *buffer, uint8_t length);

// ON PIC32 this runs at IPL3

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
typedef uint8_t (*nmea_callback_fn)(struct _uart_config_t *, char *buffer, uint8_t sentence, uint8_t word);

typedef union _uart_callback_t
  {
  rx_callback_fn uart_callback;
  nmea_callback_fn nmea_callback;
  } uart_callback_t;

typedef struct _uart_config_t
  {
  uint8_t uart_number;              // depends on device
  uint32_t rate;
  uint16_t flags;                   // see flags above
  uint16_t rx_length;                // length of the temporary buffer
  uint8_t num_rx_buffers;           // how many buffers to hold
  uint8_t eol_char;                 // normally \n in uart and nmea mode
  uint8_t start_char;               // normally $ in nmea mode
  uart_callback_t callback;         // depends on mode
  
  deque_p tx_queue;
  deque_p rx_queue;                // As each string is read it goes here
  deque_p rx_buffers;              // available buffers

  // the characters read from the uart accumulate here
  uint16_t rx_offset;                // offset into the read buffer
  
  // used by the interrupt routines
  uint8_t *rx_buffer;               // buffer for data read

  semaphore_p uart_tx_ready;            // signal used for uart tx ready
  task_p rx_worker;                 // rx worker
  task_p tx_worker;                 // tx worker for uart
  } uart_config_t;

 
/**
 * @function open_uart(uart_config_t *config, uint16_t rx_worker_stack_length, uint16_t tx_worker_stack_length)
 * initialize a uart and return the thread start argument
 * @param config        Uart configuration
 * @return  true if the uart can be configured.
 */
extern result_t open_uart(uart_config_t *config, uint16_t rx_worker_stack_length, uint16_t tx_worker_stack_length);
/**
 * @finction close_uart(uart_config_t *config)
 * Close a uart and release all resources
 * @param config  Uart to release
 * @return s_ok if closed ok
 */
extern result_t close_uart(uart_config_t *config);

/**
 * Send a string to the uart
 * @param config      device to send to
 * @param str         text to send
 * @return true if the characters are queued
 */
extern result_t send_str(uart_config_t *config, const char *str);
/**
 * Send a string with the NMEA lead-in and checksum
 * @param config  uart to send to
 * @param str string to send
 * @return s_ok if sent ok
 */
extern result_t send_nmea(uart_config_t *config, const char *str);
/**
 * Send a character to the uart
 * @param config
 * @param data
 * @return true if the character was sent
 */
extern result_t write_uart(uart_config_t *config, uint8_t data);
/**
 * send a string of characters to the nmea, does not prepend anything
 * @param config      device to send to
 * @param str         text to send
 * @param checksum    checksum to update
 * @return true if characters were sent
 */
extern result_t nmea_send_chars(uart_config_t *config, const char *str, uint8_t *checksum);
/**
 * calculate lower 4 bits as hex
 * @param value value to convert to hex
 * @return value as hex nibble
 */
extern char to_hex(uint8_t value);
/**
 * send the checksum after a '*'
 * @param config    Uart to send to
 * @param chksum    checksum to send
 * @return true if characters queued
 */
extern result_t send_checksum(uart_config_t *config, uint8_t chksum);

#endif
