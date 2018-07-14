#include "../atom/uart_device.h"
#include "../atom/microkernel.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
  
#ifndef FCY
#define FCY 70000000
#endif

static void uart_rx_worker(void *parg);
static void uart_tx_worker(void *parg);

#define TX_QUEUE_LENGTH 256

#define NUM_UART_DEVICES  2

uart_config_t *uart[NUM_UART_DEVICES];

/**
 * Queue a character into the receive buffer
 * @param p_uart  uart to queue onto
 * @param ch      character to save
 */
static void enqueue_char(uart_config_t *p_uart, uint8_t ch)
  {
  if(p_uart->rx_buffer == 0)
    return;
  
  uint8_t *rx_buffer = p_uart->rx_buffer;
  
  if((p_uart->flags & NMEA_DECODER_ENABLE) &&
     (p_uart->rx_offset == 0) &&
     (p_uart->start_char != ch))
    return;         // wait till we have the start char
  
  rx_buffer[p_uart->rx_offset++] = ch;
  rx_buffer[p_uart->rx_offset] = 0;
  if((p_uart->rx_offset >= (p_uart->rx_length-1)) ||
     (ch == p_uart->eol_char))
    {
    push_back_from_isr(p_uart->rx_queue, &rx_buffer);
    p_uart->rx_buffer = 0;
    p_uart->rx_offset = 0;
    // get the next free buffer (if one available!)
    pop_front_from_isr(p_uart->rx_buffers, &p_uart->rx_buffer);
    }
  }

extern void yield(void);

void __attribute__((interrupt, auto_psv)) _U1TXInterrupt(void)
  {
  IFS0bits.U1TXIF = 0;
  signal_from_isr(uart[0]->uart_tx_ready);
  
  yield();
  }

void __attribute__((interrupt, auto_psv)) _U1RXInterrupt(void)
  {
  /* Read the receive receive_buffer till no more characters can be read */
  while(U1STAbits.URXDA)
    {
    uint8_t val = (uint8_t)U1RXREG;
    
    enqueue_char(uart[0], val);
    }

  if(U1STAbits.OERR)
    U1STAbits.OERR = 0;

  if(U1STAbits.FERR)
    U1STAbits.FERR = 0;

  IFS0bits.U1RXIF = 0;
  
  yield();
  }

void __attribute__((interrupt, auto_psv)) _U2TXInterrupt(void)
  {
  IFS1bits.U2TXIF = 0;
  signal_from_isr(uart[1]->uart_tx_ready);
  
  yield();
  }


void __attribute__((interrupt, auto_psv)) _U2RXInterrupt(void)
  {
  /* Read the receive receive_buffer till no more characters can be read */
  while(U2STAbits.URXDA)
    {
    uint8_t val = (uint8_t)U2RXREG;
    enqueue_char(uart[1], val);
    }

  if(U2STAbits.OERR)
    U2STAbits.OERR = 0;

  if(U2STAbits.FERR)
    U2STAbits.FERR = 0;


  IFS1bits.U2RXIF = 0;
  
  yield();
  }

static void tx_uart1_worker(void *parg)
  {
  uart_tx_worker(uart[0]);
  }

static void rx_uart1_worker(void *parg)
  {
  uart_rx_worker(uart[0]);
  }

static void tx_uart2_worker(void *parg)
  {
  uart_tx_worker(uart[1]);
  }

static void rx_uart2_worker(void *parg)
  {
  uart_rx_worker(uart[1]);
  }

extern uint32_t fcy;

result_t open_uart(uart_config_t *p_uart,
               uint16_t rx_worker_stack_length,
               uint16_t tx_worker_stack_length)
  {
  result_t result;
  task_callback rx_worker;
  task_callback tx_worker;
  int i;
  
  uint16_t divisor;
  // note: datasheet calculatons state that BRG is FCY/rate*16 but since we 
  // expect FCY is khz we calculate FCY/rate * 2)
  divisor = (uint16_t)(((fcy * 1000) / (p_uart->rate << 4))-1);

  if(failed(result = deque_create(sizeof(uint8_t *), p_uart->num_rx_buffers, &p_uart->rx_queue)))
    return result;
  
  if(failed(result = deque_create(sizeof(uint8_t *), p_uart->num_rx_buffers, &p_uart->rx_buffers)))
    return result;
  
  uint8_t *buffer;
  
  // create the rx buffers, but assign 1 to the state machine
  for(i = 0; i < p_uart->num_rx_buffers; i++)
    {
    buffer = (uint8_t *)neutron_malloc(p_uart->rx_length);
    if(i == 0)
      p_uart->rx_buffer = buffer;
    else
      push_back(p_uart->rx_buffers, &buffer, INDEFINITE_WAIT);
    }
  
  if(failed(result = semaphore_create(&p_uart->uart_tx_ready)))
    return result;

  if(failed(result = deque_create(sizeof(uint8_t), TX_QUEUE_LENGTH, &p_uart->tx_queue)))
    return result;
  
  // set the baud rate and init the interrupts for the uart
  // TODO: check the pin assignments I thing uart1 is altio
  switch(p_uart->uart_number)
    {
    case 1:
      uart[0] = p_uart;
      
      rx_worker = rx_uart1_worker;
      tx_worker = tx_uart1_worker;
      
      U1MODEbits.UARTEN = 1;
      U1MODEbits.BRGH = 0;
      
      U1BRG = (uint16_t)divisor;

      U1MODEbits.STSEL = 0;
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

      
      if(U1STAbits.UTXBF == 0)
        semaphore_signal(p_uart->uart_tx_ready);
      
      break;
    case 2:
      uart[1] = p_uart;
      
      rx_worker = rx_uart2_worker;
      tx_worker = tx_uart2_worker;
     
      U2MODEbits.UARTEN = 1;
      U2MODEbits.BRGH = 0;
      
      U2BRG = (uint16_t)divisor;

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
      
      if(U2STAbits.UTXBF == 0)
        semaphore_signal(p_uart->uart_tx_ready);
      break;
    }

  if(failed(result = task_create("UART_RX", rx_worker_stack_length, rx_worker, 0, NORMAL_PRIORITY, 0)))
    return result;
  
  if(failed(result = task_create("UART_TX", tx_worker_stack_length, tx_worker, 0, NORMAL_PRIORITY, 0)))
    return result;

  return s_ok;
  }


char to_hex(uint8_t value)
  {
  value &= 0x0f;
  if(value > 10)
    return 'A' + value - 10;

  return '0' + value;
  }

static uint8_t hex_to_digit(char ch)
  {
  ch = (ch >= 'A' && ch <= 'Z') ? ch - ('A' - 'a') : ch;
  uint8_t value = ch - '0';
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

static void uart_rx_worker(void *parg)
  {
  uart_config_t *uart = (uart_config_t *) parg;
  uint8_t *buffer;

  while(true)
    {
    buffer = 0;
    pop_front(uart->rx_queue, &buffer, INDEFINITE_WAIT);
    
    if(buffer == 0)
      continue;
    
    if((uart->flags & NMEA_DECODER_ENABLE) != 0)
      {
      char *start = (char *) buffer;
      if(*start == '$')
        {
        start++;
        if(*start != 0)
          {
          char *end = start;
          uint8_t sentence = 0;
          uint8_t word = 0;
          uint8_t checksum = 0;
          bool checksum_marker = false;

          // calculate the checksum of the string
          while(*start != 0 && *start != '*')
            checksum ^= *start++;
          
          // search for any \r or \n
          while(*start != 0)
            {
            if(*start == '\r' || *start == '\n')
              {
              *start = 0;
              break;
              }
            start++;
            }

          // reset the string pointer.
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

              (uart->callback.nmea_callback)(uart, checksum != chk ? 0 : (char *) 1, sentence, word);
              break;                  // and done, wait for next
              }

            // search for the end of the token
            for(end = start; *end != 0 && *end != ',' && *end != '*' ; end++);

            if(*end == '*')
              checksum_marker = true;

            // mark the end of the token
            *end = 0;

            // identify the sentence
            sentence = (uart->callback.nmea_callback)(uart, start, sentence, word);
            word++;

            // skip token
            start = end;
            // and increment to next word
            start++;
            }
          }
        }
      }
    else
      {
      // push back the message, don't wait if full..
      (uart->callback.uart_callback)(uart, buffer, strlen((const char *)uart->rx_buffer));
      }

    enter_critical();
    
    if(uart->rx_buffer == 0)
      uart->rx_buffer = buffer;
    else
      push_back_from_isr(uart->rx_buffers, &buffer);

    exit_critical();
    }
  }

static void uart_tx_worker(void *parg)
  {
  uart_config_t *uart = (uart_config_t *) parg;

  while(true)
    {
    uint8_t ch;
    pop_front(uart->tx_queue, &ch, INDEFINITE_WAIT);

    // make sure we are ready to send, block if not
    if(uart->uart_number == 1)
      {
      while(U1STAbits.UTXBF == 1)
        semaphore_wait(uart->uart_tx_ready, 1);
      }
    else
      {
      while(U2STAbits.UTXBF == 1)
        semaphore_wait(uart->uart_tx_ready, 1);
      }

    if(uart->uart_number == 1)
      U1TXREG = ch;
    else
      U2TXREG = ch;
    }
  }

result_t write_uart(uart_config_t *p_uart, uint8_t data)
  {
  return push_back(p_uart->tx_queue, &data, INDEFINITE_WAIT);
  }

result_t send_str(uart_config_t *p_uart, const char *str)
  {
  result_t result;
  while(*str != 0)
    if(failed(result = write_uart(p_uart, *str++)))
      return result;

  return s_ok;
  }

result_t send_checksum(uart_config_t *p_uart, uint8_t chksum)
  {
  result_t result;
  if(failed(result = write_uart(p_uart, '*')) ||
    failed(result = write_uart(p_uart, to_hex(chksum >> 4))) ||
    failed(result = write_uart(p_uart, to_hex(chksum))) ||
    failed(result = write_uart(p_uart, '\n')))
    return result;
  
  return result;
  }

result_t nmea_send_chars(uart_config_t *p_uart, const char *str, uint8_t *chksum)
  {
  result_t result;
  while(*str != 0)
    {
    *chksum ^= *str;
    if(failed(result = write_uart(p_uart, *str)))
      return result;
    str++;
    }

  return s_ok;
  }

result_t send_nmea(uart_config_t *p_uart, const char *str)
  {
  uint8_t chksum;
  result_t result;
  if(failed(result = send_str(p_uart, "$")))
    return result;
  
  if(failed(result = nmea_send_chars(p_uart, str, &chksum))) 
    return result;
  
  return send_checksum(p_uart, chksum);
  }