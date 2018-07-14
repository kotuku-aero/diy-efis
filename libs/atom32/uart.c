#include "../atom/uart_device.h"
#include "../atom/microkernel.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/attribs.h>

static void uart_rx_worker(void *parg);
static void uart_tx_worker(void *parg);

#define TX_QUEUE_LENGTH 256
#define NUM_UART_DEVICES 6

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

void __attribute__( (interrupt(IPL0AUTO), vector(_UART1_TX_VECTOR))) _u1_tx_interrupt( void );
void u1_tx_interrupt(void)
  {
 if(uart[0] != 0)
    signal_from_isr(uart[0]->uart_tx_ready);
  IFS3bits.U1TXIF = 0;
  IEC3bits.U1TXIE = 0;
  }

void __attribute__( (interrupt(IPL0AUTO), vector(_UART1_RX_VECTOR))) _u1_rx_interrupt( void );
void u1_rx_interrupt(void)
  {
  /* Read the receive receive_buffer till no more characters can be read */
  while(U1STAbits.URXDA)
    {
    uint8_t val = (uint8_t)U1RXREG;
    
    if(uart[0] != 0)
      enqueue_char(uart[0], val);
    }

  if(U1STAbits.OERR)
    U1STAbits.OERR = 0;

  if(U1STAbits.FERR)
    U1STAbits.FERR = 0;
  
  IFS3bits.U1RXIF = 0;
  }

void __attribute__( (interrupt(IPL0AUTO), vector(_UART2_TX_VECTOR))) _u2_tx_interrupt( void );
void u2_tx_interrupt(void)
  {
  if(uart[1] != 0)
    signal_from_isr(uart[1]->uart_tx_ready);
  IFS4bits.U2TXIF = 0;
  IEC4bits.U2TXIE = 0;    // mask till sr loaded
  }

void __attribute__( (interrupt(IPL0AUTO), vector(_UART2_RX_VECTOR))) _u2_rx_interrupt( void );
void u2_rx_interrupt(void)
  {
  /* Read the receive receive_buffer till no more characters can be read */
  while(U2STAbits.URXDA)
    {
    uint8_t val = (uint8_t)U2RXREG;
    if(uart[1] != 0)
      enqueue_char(uart[1], val);
    }

  if(U2STAbits.OERR)
    U2STAbits.OERR = 0;

  if(U2STAbits.FERR)
    U2STAbits.FERR = 0;

  IFS4bits.U2RXIF = 0;
  }

void __attribute__( (interrupt(IPL0AUTO), vector(_UART3_TX_VECTOR))) _u3_tx_interrupt( void );
void u3_tx_interrupt(void)
  {
  if(uart[2]!= 0)
    signal_from_isr(uart[2]->uart_tx_ready);
  IFS4bits.U3TXIF = 0;
  IEC4bits.U3TXIE = 0;
  }

void __attribute__( (interrupt(IPL0AUTO), vector(_UART3_RX_VECTOR))) _u3_rx_interrupt( void );
void u3_rx_interrupt(void)
  {
  /* Read the receive receive_buffer till no more characters can be read */
  while(U3STAbits.URXDA)
    {
    uint8_t val = (uint8_t)U3RXREG;
    if(uart[2]!= 0)
      enqueue_char(uart[2], val);
    }

  if(U3STAbits.OERR)
    U3STAbits.OERR = 0;

  if(U3STAbits.FERR)
    U3STAbits.FERR = 0;

  IFS4bits.U3RXIF = 0;
  }

void __attribute__( (interrupt(IPL0AUTO), vector(_UART4_TX_VECTOR))) _u4_tx_interrupt( void );
void u4_tx_interrupt(void)
  {
  if(uart[3]!= 0)
    signal_from_isr(uart[3]->uart_tx_ready);
  IFS5bits.U4TXIF = 0;
  IEC5bits.U4TXIE = 0;
  }

void __attribute__( (interrupt(IPL0AUTO), vector(_UART4_RX_VECTOR))) _u4_rx_interrupt( void );
void u4_rx_interrupt(void)
  {
  /* Read the receive receive_buffer till no more characters can be read */
  while(U4STAbits.URXDA)
    {
    uint8_t val = (uint8_t)U4RXREG;
    if(uart[3]!= 0)
      enqueue_char(uart[3], val);
    }

  if(U4STAbits.OERR)
    U4STAbits.OERR = 0;

  if(U4STAbits.FERR)
    U4STAbits.FERR = 0;

  IFS5bits.U4RXIF = 0;
  }

void __attribute__( (interrupt(IPL0AUTO), vector(_UART5_TX_VECTOR))) _u5_tx_interrupt( void );
void u5_tx_interrupt(void)
  {
  if(uart[4]!= 0)
    signal_from_isr(uart[4]->uart_tx_ready);
  IFS5bits.U5TXIF = 0;
  IEC5bits.U5TXIE = 0;
  }

void __attribute__( (interrupt(IPL0AUTO), vector(_UART5_RX_VECTOR))) _u5_rx_interrupt( void );
void u5_rx_interrupt(void)
  {
  /* Read the receive receive_buffer till no more characters can be read */
  while(U5STAbits.URXDA)
    {
    uint8_t val = (uint8_t)U5RXREG;
    if(uart[4]!= 0)
      enqueue_char(uart[4], val);
    }

  if(U5STAbits.OERR)
    U5STAbits.OERR = 0;

  if(U5STAbits.FERR)
    U5STAbits.FERR = 0;

  IFS5bits.U5RXIF = 0;
  }

void __attribute__( (interrupt(IPL0AUTO), vector(_UART6_TX_VECTOR))) _u6_tx_interrupt( void );
void u6_tx_interrupt(void)
  {
  if(uart[5]!= 0)
    signal_from_isr(uart[5]->uart_tx_ready);
  IFS5bits.U6TXIF = 0;
  IEC5bits.U6TXIE = 0;
  }

void __attribute__( (interrupt(IPL0AUTO), vector(_UART6_RX_VECTOR))) _u6_rx_interrupt( void );
void u6_rx_interrupt(void)
  {
  /* Read the receive receive_buffer till no more characters can be read */
  while(U6STAbits.URXDA)
    {
    uint8_t val = (uint8_t)U6RXREG;
    if(uart[5]!= 0)
      enqueue_char(uart[5], val);
    }

  if(U6STAbits.OERR)
    U6STAbits.OERR = 0;

  if(U6STAbits.FERR)
    U6STAbits.FERR = 0;

  IFS5bits.U6RXIF = 0;
  }

static const char *tx_task_names[NUM_UART_DEVICES] =
  {
  "UART1_TX",
  "UART2_TX",
  "UART3_TX",
  "UART4_TX",
  "UART5_TX",
  "UART6_TX",
  };

static const char *rx_task_names[NUM_UART_DEVICES] =
  {
  "UART1_RX",
  "UART2_RX",
  "UART3_RX",
  "UART4_RX",
  "UART5_RX",
  "UART6_RX",
  };

#define FCY 10000

result_t open_uart(uart_config_t *p_uart,
               uint16_t rx_worker_stack_length,
               uint16_t tx_worker_stack_length)
  {
  result_t result;
  int i;
  
  uint16_t divisor;
  divisor = (uint16_t)((FCY / ((uint32_t)(p_uart->rate << 1)))-1);

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
      
      U1MODEbits.UARTEN = 1;
      U1MODEbits.BRGH = 0;
      
      U1BRG = (uint16_t)divisor;

      U1MODEbits.STSEL = 0;
      U1MODEbits.PDSEL = 0;

      U1STAbits.URXISEL = 0;
      U1STAbits.ADDEN = 0;
      U1STAbits.UTXEN = 1;
      
      if(U1STAbits.UTXBF == 0)
        semaphore_signal(p_uart->uart_tx_ready);
      
      IFS3bits.U1RXIF = 0;
      IFS3bits.U1TXIF = 0;
      IPC28bits.U1RXIP = 3;
      IPC28bits.U1RXIS = 0;
      IPC28bits.U1TXIP = 3;
      IPC28bits.U1TXIS = 0;
      IEC3bits.U1TXIE = 0;
      IEC3bits.U1RXIE = 1;
      
      break;
    case 2:
      uart[1] = p_uart;
     
      U2MODEbits.UARTEN = 1;
      U2MODEbits.BRGH = 0;
      
      U2BRG = (uint16_t)divisor;

      U2MODEbits.STSEL = 0;
      U2MODEbits.PDSEL = 0;

      U2STAbits.URXISEL = 0;
      U2STAbits.ADDEN = 0;
      U2STAbits.UTXEN = 1;
      
      if(U2STAbits.UTXBF == 0)
        semaphore_signal(p_uart->uart_tx_ready);
      
      IFS4bits.U2RXIF = 0;
      IFS4bits.U2TXIF = 0;
      IPC36bits.U2RXIP = 3;
      IPC36bits.U2RXIS = 0;
      IPC36bits.U2TXIP = 3;
      IPC36bits.U2TXIS = 0;
      IEC4bits.U2TXIE = 0;
      IEC4bits.U2RXIE = 1;
      
      break;
    case 3:
      uart[2] = p_uart;
     
      U3MODEbits.UARTEN = 1;
      U3MODEbits.BRGH = 0;
      
      U3BRG = (uint16_t)divisor;

      U3MODEbits.STSEL = 0;
      U3MODEbits.PDSEL = 0;

      U3STAbits.URXISEL = 0;
      U3STAbits.ADDEN = 0;
      U3STAbits.UTXEN = 1;
      
      if(U3STAbits.UTXBF == 0)
        semaphore_signal(p_uart->uart_tx_ready);
      
      IFS4bits.U3RXIF = 0;
      IFS4bits.U3TXIF = 0;
      IPC39bits.U3RXIP = 3;
      IPC39bits.U3RXIS = 0;
      IPC39bits.U3TXIP = 3;
      IPC39bits.U3TXIS = 0;
      IEC4bits.U3TXIE = 0;
      IEC4bits.U3RXIE = 1;
      
      break;
    case 4:
      uart[3] = p_uart;
     
      U4MODEbits.UARTEN = 1;
      U4MODEbits.BRGH = 0;
      
      U4BRG = (uint16_t)divisor;

      U4MODEbits.STSEL = 0;
      U4MODEbits.PDSEL = 0;

      U4STAbits.URXISEL = 0;
      U4STAbits.ADDEN = 0;
      U4STAbits.UTXEN = 1;
      
      if(U4STAbits.UTXBF == 0)
        semaphore_signal(p_uart->uart_tx_ready);
      
      IFS5bits.U4RXIF = 0;
      IFS5bits.U4TXIF = 0;
      IPC42bits.U4RXIP = 3;
      IPC42bits.U4RXIS = 0;
      IPC43bits.U4TXIP = 3;
      IPC43bits.U4TXIS = 0;
      IEC5bits.U4TXIE = 0;
      IEC5bits.U4RXIE = 1;
      
      break;
    case 5:
      uart[4] = p_uart;
     
      U5MODEbits.UARTEN = 1;
      U5MODEbits.BRGH = 0;
      
      U5BRG = (uint16_t)divisor;

      U5MODEbits.STSEL = 0;
      U5MODEbits.PDSEL = 0;

      U5STAbits.URXISEL = 0;
      U5STAbits.ADDEN = 0;
      U5STAbits.UTXEN = 1;
      
      if(U5STAbits.UTXBF == 0)
        semaphore_signal(p_uart->uart_tx_ready);
      
      IFS5bits.U5RXIF = 0;
      IFS5bits.U5TXIF = 0;
      IPC45bits.U5RXIP = 3;
      IPC45bits.U5RXIS = 0;
      IPC45bits.U5TXIP = 3;
      IPC45bits.U5TXIS = 0;
      IEC5bits.U5TXIE = 0;
      IEC5bits.U5RXIE = 1;
      
      break;
    case 6:
      uart[5] = p_uart;
     
      U6MODEbits.UARTEN = 1;
      U6MODEbits.BRGH = 0;
      
      U6BRG = (uint16_t)divisor;

      U6MODEbits.STSEL = 0;
      U6MODEbits.PDSEL = 0;

      U6STAbits.URXISEL = 0;
      U6STAbits.ADDEN = 0;
      U6STAbits.UTXEN = 1;
      
      if(U6STAbits.UTXBF == 0)
        semaphore_signal(p_uart->uart_tx_ready);
      
      IFS5bits.U6RXIF = 0;
      IFS5bits.U6TXIF = 0;
      IPC47bits.U6RXIP = 3;
      IPC47bits.U6RXIS = 0;
      IPC47bits.U6TXIP = 3;
      IPC47bits.U6TXIS = 0;
      IEC5bits.U6TXIE = 0;
      IEC5bits.U6RXIE = 1;
      
      break;
      
    default :
      return e_bad_parameter;
   }

  if(failed(result = task_create(rx_task_names[p_uart->uart_number-1], rx_worker_stack_length, uart_rx_worker, p_uart, NORMAL_PRIORITY, &p_uart->rx_worker)))
    return result;
  
  if(failed(result = task_create(tx_task_names[p_uart->uart_number-1], tx_worker_stack_length, uart_tx_worker, p_uart, NORMAL_PRIORITY, &p_uart->tx_worker)))
    return result;

  return s_ok;
  }

result_t close_uart(uart_config_t *p_uart)
  {
  // first we stop the uart
  switch(p_uart->uart_number)
    {
    case 1:
      U1MODEbits.UARTEN = 0;
      uart[0] = 0;
      break;
    case 2:
      U2MODEbits.UARTEN = 0;
      uart[1] = 0;
      break;
    case 3:
      U3MODEbits.UARTEN = 0;
      uart[2] = 0;
      break;
    case 4:
      U4MODEbits.UARTEN = 0;
      uart[3] = 0;
      break;
    case 5:
      U5MODEbits.UARTEN = 0;
      uart[4] = 0;
      break;
    case 6:
      U6MODEbits.UARTEN = 0;
      uart[5] = 0;
      break;
      
    default :
      return e_bad_parameter;
   }
  
  // stop the tasks
  close_task(p_uart->tx_worker);
  p_uart->tx_worker = 0;
  close_task(p_uart->rx_worker);
  p_uart->rx_worker = 0;
  
  while(succeeded(can_pop(p_uart->rx_queue)))
    {
    void *buffer;
    if(succeeded(pop_front(p_uart->rx_queue, &buffer, 0)))
      neutron_free(buffer);
    else
      break;
    }
  
  p_uart->rx_queue = 0;
  
  deque_close(p_uart->rx_queue);
  
  while(succeeded(can_pop(p_uart->rx_buffers)))
    {
    void *buffer;
    if(succeeded(pop_front(p_uart->rx_buffers, &buffer, 0)))
      neutron_free(buffer);
    else
      break;
    }
  
  deque_close(p_uart->rx_buffers);
  p_uart->rx_buffers = 0;
  
  semaphore_close(p_uart->uart_tx_ready);
  p_uart->uart_tx_ready = 0;
  
  deque_close(p_uart->tx_queue);
  p_uart->tx_queue = 0;
  
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

    switch(uart->uart_number)
      {
      case 1 :
        while(U1STAbits.UTXBF == 1)
          {
          IEC3bits.U1TXIE = 1;
          semaphore_wait(uart->uart_tx_ready, 1);
          }
        
        U1TXREG = ch;
        break;
      case 2 :
        while(U2STAbits.UTXBF == 1)
          {
          IEC4bits.U2TXIE = 1;
          semaphore_wait(uart->uart_tx_ready, 1);
          }

        U2TXREG = ch;
        break;
      case 3 :
        while(U3STAbits.UTXBF == 1)
          {
          IEC4bits.U3TXIE = 1;
          semaphore_wait(uart->uart_tx_ready, 1);
          }

        U3TXREG = ch;
        break;
      case 4 :
        while(U4STAbits.UTXBF == 1)
          {
          IEC5bits.U4TXIE = 1;
          semaphore_wait(uart->uart_tx_ready, 1);
          }

        U4TXREG = ch;
        break;
      case 5 :
        while(U5STAbits.UTXBF == 1)
          {
          IEC5bits.U5TXIE = 1;
          semaphore_wait(uart->uart_tx_ready, 1);
          }

        U5TXREG = ch;
        break;
      case 6 :
        while(U6STAbits.UTXBF == 1)
          {
          IEC5bits.U6TXIE = 1;
          semaphore_wait(uart->uart_tx_ready, 1);
          }

        U6TXREG = ch;
        break;
      }
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