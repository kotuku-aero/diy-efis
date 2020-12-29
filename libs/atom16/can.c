#include "../atom/microkernel.h"
#include "../../libs/neutron/bsp.h"
#include "pps_maps.h"

#ifdef TON
#undef TON
#endif

#define NUM_OF_ECAN_BUFFERS 16

static deque_p can_rx_queue;

static semaphore_p can_tx_event;

/** we use buffers 0..7 as a ring buffer
 */
static uint16_t ecan_msg_buffer[NUM_OF_ECAN_BUFFERS << 3] __attribute__((aligned(NUM_OF_ECAN_BUFFERS * 16)));

inline uint16_t make_txbuffer(uint8_t byte_0, uint8_t byte_1)
  {
  return (((uint16_t) byte_1) << 8) | byte_0;
  }
static int next_free_tx_buffer();
static void enable_tx(int buffer, int priority);

/**
 * Get the next buffer to send a can message to
 * @return -1 if no free buffers
 */
static int next_free_tx_buffer()
  {
  if(C1TR67CONbits.TXREQ7 == 0)
    return 7;
/*
  if(C1TR67CONbits.TXREQ6 == 0)
    return 6;

  if(C1TR45CONbits.TXREQ5 == 0)
    return 5;

  if(C1TR45CONbits.TXREQ4 == 0)
    return 4;

  if(C1TR23CONbits.TXREQ3 == 0)
    return 3;

  if(C1TR23CONbits.TXREQ2 == 0)
    return 2;

  if(C1TR01CONbits.TXREQ1 == 0)
    return 1;

  if(C1TR01CONbits.TXREQ0 == 0)
    return 0;
*/
  return -1;
  }

void enable_tx(int next_buffer, int priority)
  {
  switch(next_buffer)
    {
    case 0:
      C1TR01CONbits.TX0PRI = priority;
      C1TR01CONbits.RTREN0 = 0;
      C1TR01CONbits.TXREQ0 = 1;
      break;
    case 1:
      C1TR01CONbits.TX1PRI = priority;
      C1TR01CONbits.RTREN1 = 0;
      C1TR01CONbits.TXREQ1 = 1;
      break;
    case 2:
      C1TR23CONbits.TX2PRI = priority;
      C1TR23CONbits.RTREN2 = 0;
      C1TR23CONbits.TXREQ2 = 1;
      break;
    case 3:
      C1TR23CONbits.TX3PRI = priority;
      C1TR23CONbits.RTREN3 = 0;
      C1TR23CONbits.TXREQ3 = 1;
      break;
    case 4:
      C1TR45CONbits.TX4PRI = priority;
      C1TR45CONbits.RTREN4 = 0;
      C1TR45CONbits.TXREQ4 = 1;
      break;
    case 5:
      C1TR45CONbits.TX5PRI = priority;
      C1TR45CONbits.RTREN5 = 0;
      C1TR45CONbits.TXREQ5 = 1;
      break;
    case 6:
      C1TR67CONbits.TX6PRI = priority;
      C1TR67CONbits.RTREN6 = 0;
      C1TR67CONbits.TXREQ6 = 1;
      break;
    case 7:
      C1TR67CONbits.TX7PRI = priority;
      C1TR67CONbits.RTREN7 = 0;
      C1TR67CONbits.TXREQ7 = 1;
      break;
    }
  }


#define BITRATE 	125               // 125kbs
#define PRSEG_LEN   1
#define PRSEG_VAL   (PRSEG_LEN -1)
#define SEG1PH_LEN  5
#define SEG1PH_VAL  (SEG1PH_LEN -1)
#define SEG2PH_LEN  3
#define SEG2PH_VAL  (SEG2PH_LEN -1)
#define NTQ 		(1 + PRSEG_LEN + SEG1PH_LEN + SEG2PH_LEN) // Number of Tq cycles which will make the
//CAN Bit Timing .
//#define BRP_VAL		((FCY/(2* NTQ * BITRATE))-1)  //Formulae used for C1CFG1bits.BRP

result_t bsp_can_init(deque_p rx_queue, uint32_t fcy, uint32_t bitrate)
  {
  // configure dma channel 0 to the txcan function
  DMA0CONbits.SIZE = 0x0;
  DMA0CONbits.DIR = 0x1;
  DMA0CONbits.AMODE = 0x2;
  DMA0CONbits.MODE = 0x0;
  DMA0REQ = 0x46;
  DMA0CNT = 7;
  DMA0PAD = (volatile unsigned int) &C1TXD;
  DMA0STAL = (uint16_t) &ecan_msg_buffer;
  DMA0STAH = 0;
  IEC0bits.DMA0IE = 1;
  DMA0CONbits.CHEN = 0x1;
  

  // assign dma1 for receive
  DMA1CONbits.SIZE = 0x0;
  DMA1CONbits.DIR = 0x0;
  DMA1CONbits.AMODE = 0x2;
  DMA1CONbits.MODE = 0x0;
  DMA1REQ = 0x22;
  DMA1CNT = 7;
  DMA1PAD = (volatile unsigned int) &C1RXD;
  DMA1STAL = (uint16_t) &ecan_msg_buffer[64];
  DMA1STAH = 0;
  IEC0bits.DMA1IE = 1;
  DMA1CONbits.CHEN = 0x1;

  /* Set the ECAN module for Configuration Mode before writing into the Baud
  Rate Control Registers */
  C1CTRL1bits.REQOP = 4;
  
  while(C1CTRL1bits.OPMODE != 4);
  
  C1CTRL1bits.WIN = 0;

  // set up the can bus interface
  C1CTRL1bits.CANCKS = 0; // Select can clock == Fp (70mhz)
  C1CFG1bits.SJW = 00; //Synchronized jump width time is 1 x TQ when SJW is equal to 00
  
  // set the initial rate to 125kbs
  // should be 27 for 70000, 7 for 20000
  
  uint32_t brp = (fcy / (2* NTQ * bitrate))-1;
  
  C1CFG1bits.BRP = (uint16_t) brp;

  C1CFG2bits.PRSEG = PRSEG_VAL; // Preamble
  C1CFG2bits.SEG1PH = SEG1PH_VAL;
  C1CFG2bits.SEG2PH = SEG2PH_VAL;
  C1CFG2bits.SEG2PHTS = 1; // use PS2 for calcs

  C1TR01CONbits.TXEN0 = 1;
  C1TR01CONbits.TXEN1 = 1;
  C1TR23CONbits.TXEN2 = 1;
  C1TR23CONbits.TXEN3 = 1;
  C1TR45CONbits.TXEN4 = 1;
  C1TR45CONbits.TXEN5 = 1;
  C1TR67CONbits.TXEN6 = 1;
  C1TR67CONbits.TXEN7 = 1;
 
  
  IEC2bits.C1IE = 1;
  
  // set up a 24 word fifo
  C1FCTRLbits.DMABS = 4;        // 16 buffers in ram
  C1FCTRLbits.FSA = 8;          // 0..7 are tx buffers
  
  // select the buffer bits
  C1CTRL1bits.WIN = 1;

  // set mask to 1 to not mask any id's
  C1RXM0SID = 0x0000;
  C1RXF0SID = 0;
  
  // only enable filter 1
  C1FEN1 = 0x0001;
  C1BUFPNT1bits.F0BP = 15;
  
  /* Acceptance Filter 0 to check for Standard Identifier */
  C1RXM0SIDbits.MIDE = 0x1;
  C1RXF0SIDbits.EXIDE= 0x0;

  //Interrupt Section of CAN Peripheral
  //C1INTF = 0; //Reset all The CAN Interrupts
  // enable the rx and tx interrupts
  //C1INTE = 0x0003;
  C1INTEbits.RBIE = 1;
  C1INTEbits.TBIE = 1;
  C1INTEbits.RBOVIE = 1;
  C1INTEbits.ERRIE = 1;
  
  C1CTRL1bits.WIN = 0;
  
  C1CTRL1bits.REQOP = 0;

  while(C1CTRL1bits.OPMODE != 0); // wait for ready
  
  //C1CTRL1bits.REQOP = 2;

  //while(C1CTRL1bits.OPMODE != 2); // wait for ready
  semaphore_create(&can_tx_event);
  can_rx_queue = rx_queue;
  
  return s_ok;
  }

extern void yield(void);

void __attribute__((interrupt, auto_psv)) _C1Interrupt(void)
  {
  uint16_t icode;
  while((icode = C1VEC) != 64)
    {
    int index = icode;
    if(index == 65)
      {
      // ERROR interrupt
      C1INTFbits.ERRIF = 0;       // clear the error.
      C1INTFbits.IVRIF = 0;
      
      if(C1INTFbits.TXBO == 1)
        {
        // tx buffer in bus off
        }
      }
    else if(icode == 0x43)
      {
      if(C1INTFbits.RBOVIF == 1)
        {
        // rx overflow
        C1RXFUL1 = 0x00;        // clear all flags
        C1INTFbits.RBOVIF = 0;
        }
      }
    else if(icode >= 0 && icode < 8)
      {
      C1INTFbits.TBIF = 0; // clear the flag
      signal_from_isr(can_tx_event); // signal an interrupt
      }
    else if(icode >= 8 && icode < 32)
      {
      uint16_t *ptr;
      uint16_t value;
      canmsg_t frame;
      // process message from buffer
      
      while(true)
        {
        index = C1FIFObits.FNRB;
        if((C1RXFUL1 & (1 << index)) == 0)
          break;
        
        ptr = ecan_msg_buffer + (index <<3);
        
        set_can_id(&frame, *ptr >> 2);
        ptr++;
        ptr++;    // skip eid
        value = *ptr;
        ptr++;
        set_can_reply(&frame, (value & 0x0200) != 0);
        set_can_len(&frame, value & 0x000f);

        value = *ptr;
        ptr++;
        frame.canas.node_id = (uint8_t) value;
        frame.canas.data_type = (uint8_t) (value >> 8);

        value = *ptr;
        ptr++;
        frame.canas.service_code = (uint8_t) value;
        frame.canas.message_code = (uint8_t) (value >> 8);

        value = *ptr;
        ptr++;
        frame.canas.data[0] = (uint8_t) value;
        frame.canas.data[1] = (uint8_t) (value >> 8);

        value = *ptr;
        ptr++;
        frame.canas.data[2] = (uint8_t) value;
        frame.canas.data[3] = (uint8_t) (value >> 8);
        
        ptr++;    // skip flthit

        push_back_from_isr(can_rx_queue, &frame);

        C1RXFUL1 &= ~(1 << index);
        }
      C1INTFbits.RBIF = 0;
      }
    }
  
  // clear the IRQ
  IFS2bits.C1IF = 0;
  
  yield();
  }

void __attribute__((interrupt, auto_psv)) _DMA0Interrupt(void)
  {
  IFS0bits.DMA0IF = 0;
  
  yield();
  }

void __attribute__((interrupt, auto_psv)) _DMA1Interrupt(void)
  {
  IFS0bits.DMA1IF = 0;
  
  yield();
  }


result_t bsp_send_can(const canmsg_t *msg)
  {
  int next_buffer;
  // spin till a buffer is free
  while((next_buffer = next_free_tx_buffer()) == -1)
    semaphore_wait(can_tx_event, INDEFINITE_WAIT);
  
  uint16_t *ptr = ecan_msg_buffer + (next_buffer << 3);
  
  int length = get_can_len(msg);
  int priority = 0;

  uint16_t id = get_can_id(msg);
/*
  if(id < 128)
    priority = 3;
  else if(id < 300)
    priority = 2;
  else if(id < 1800)
    priority = 1;
  else
    priority = 0;
 * */

  *ptr++ = id << 2;
  *ptr++ = 0;
  *ptr++ = (length & 0x000f);
  *ptr++ = make_txbuffer(msg->canas.node_id, msg->canas.data_type);
  *ptr++ = make_txbuffer(msg->canas.service_code, msg->canas.message_code);
  *ptr++ = make_txbuffer(msg->canas.data[0], msg->canas.data[1]);
  *ptr++ = make_txbuffer(msg->canas.data[2], msg->canas.data[3]);
  *ptr++ = 0;
  
  if((length & 0x0f)== 0)
    {
    // don't send runt packets...
    return s_false;
    }

  enable_tx(next_buffer, priority);
  
  return s_ok;
  }
