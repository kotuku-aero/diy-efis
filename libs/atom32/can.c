#include "../atom/microkernel.h"
#include <sys/kmem.h>

#ifdef TON
#undef TON
#endif
#define NUM_OF_ECAN_BUFFERS 32

static deque_p can_rx_queue;

static semaphore_p can_tx_event;

#include <sys/attribs.h>

// these are the CAN fifo buffers. We use 4 tx priorities and 4 rx priorities
// each is 16 bytes x 32 slots
__attribute__ ((packed)) struct _pic_can_msg_t {
  uint32_t sid;
  uint32_t eid;
  uint8_t data[8];
  };
  
typedef struct _pic_can_msg_t pic_can_msg_t;
  
 
static inline void set_dlc(pic_can_msg_t *msg, int len)
  {
  msg->eid = (msg->eid & 0xfff0) | (len & 0x0f);
  }

static inline int get_dlc(const pic_can_msg_t *msg)
  {
  return msg->eid & 0x0f;
  }

static inline void set_rtr(pic_can_msg_t *msg, bool is_it)
  {
  msg->eid = (msg->eid & 0xffDF) | (is_it ? 0x0020 : 0);
  }

static inline bool get_rtr(const pic_can_msg_t *msg)
  {
  return (msg->eid & 0x0020)!= 0 ? true : false;
  }
    
// THESE MUST be adjacent in memory and correspond to the pic32 fifo buffers
static pic_can_msg_t __attribute__((coherent)) tx_fifo[NUM_OF_ECAN_BUFFERS];
static pic_can_msg_t __attribute__((coherent)) rx_fifo[NUM_OF_ECAN_BUFFERS];

#define BITRATE 	125000               // 125kbs
#define SJW_LEN     1
#define SJW_VAL     (SJW_LEN -1)
#define PRSEG_LEN   1
#define PRSEG_VAL   (PRSEG_LEN -1)
#define SEG1PH_LEN  5
#define SEG1PH_VAL  (SEG1PH_LEN -1)
#define SEG2PH_LEN  3
#define SEG2PH_VAL  (SEG2PH_LEN -1)
#define NTQ 		(PRSEG_LEN + SEG1PH_LEN + SEG2PH_LEN + SJW_LEN) // Number of Tq cycles which will make the
//CAN Bit Timing .


result_t bsp_can_init(handle_t rx_queue, uint32_t fcy, uint32_t bitrate)
  {
  C1CONbits.REQOP = 4;
  while(C1CONbits.OPMOD != 4);

  while(C1CONbits.CANBUSY == 1);

  semaphore_create(&can_tx_event);
  
  can_rx_queue = rx_queue;
  
  uint32_t brp = (fcy / (2* NTQ * ((uint32_t) bitrate)))-1;
    // set up the can bus interface
  C1CFGbits.SJW = SJW_VAL; //Synchronized jump width time is 1 x TQ when SJW is equal to 00
  C1CFGbits.BRP = brp;
  C1CFGbits.SEG2PHTS = 1;       //
  C1CFGbits.PRSEG = PRSEG_VAL;  // Preamble
  C1CFGbits.SEG1PH = SEG1PH_VAL;
  C1CFGbits.SEG2PH = SEG2PH_VAL;
  
  C1FIFOBA = (uint32_t) KVA_TO_PA(tx_fifo);
  
  // FIFO 0 is for TX
  C1FIFOCON0bits.TXEN = 1;
  C1FIFOCON0bits.FSIZE = NUM_OF_ECAN_BUFFERS-1;
  C1FIFOCON0bits.TXPRI = 0;
  C1FIFOINT0bits.TXEMPTYIE = 0;     // will be enabled when fifo full
  
  // FIFO 1 is for rx
  C1FIFOCON1bits.TXEN = 0;
  C1FIFOCON1bits.DONLY = 0;
  C1FIFOCON1bits.FSIZE = NUM_OF_ECAN_BUFFERS-1;
  C1FIFOINT1bits.RXNEMPTYIE = 1;
  
  C1CONbits.CANCAP = 1;       // 16byte messages
  
  // no msg priorities yet
  C1RXM0bits.SID = 0;         // any address is valid
  C1FLTCON0bits.FSEL0 = 1;
  C1FLTCON0bits.MSEL0 = 0;
  C1FLTCON0bits.FLTEN0 = 1;
  
  C1INTbits.RBIE = 1;
  C1INTbits.TBIE = 1;
  C1INTbits.IVRIE = 1;

  IFS4bits.CAN1IF = 0;
  IPC37bits.CAN1IP = 6;
  IPC37bits.CAN1IS = 0;
  IEC4bits.CAN1IE = 1;
    
  // enable the can controller
  C1CONbits.REQOP = 0;

  while(C1CONbits.OPMOD != 0); // wait for ready
  
  return s_ok;
  }

void __attribute__( (interrupt(IPL0AUTO), vector(_CAN1_VECTOR))) _can1_interrupt( void );
void can1_interrupt(void)
  {
  uint16_t icode;
  while((icode = C1VEC) != 0x40)
    {
    if(icode == 0x48)      // Invalid message received (IVRIF)
      {
      // clear it
      C1INTbits.IVRIF = 0;
      
      }
    else if(icode == 0x47) // CAN module mode change (MODIF)
      {
      C1INTbits.MODIF = 0;
      }
    else if(icode == 0x46) // CAN timestamp timer (CTMRIF)
      {
      C1INTbits.CTMRIF = 0;
      }
    else if(icode == 0x45) // Bus bandwidth error (SERRIF)
      {
      C1INTbits.SERRIF = 0;
      }
    else if(icode == 0x44) // Address error interrupt (SERRIF)
      {
      C1INTbits.SERRIF = 0;
      }
    else if(icode == 0x43) // Receive FIFO overflow interrupt (RBOVIF)
      {
      C1INTbits.RBOVIF = 0;
      }
    else if(icode == 0x42) // Wake-up interrupt (WAKIF)
      {
      C1INTbits.WAKIF = 0;
      }
    else if(icode == 0x41) // Error Interrupt (CERRIF)
      {
      // turn the ecan off and on again...????
      C1INTbits.CERRIF = 0;
      
      }
    else if(icode == 0)
      {
      C1INTbits.TBIF = 0;
      signal_from_isr(can_tx_event);  // signal an interrupt
      
      // and mask the tx empty flag
      C1FIFOINT0bits.TXNFULLIE = 0;
      }
    else if(icode == 1)
      {
      pic_can_msg_t *rx = (pic_can_msg_t *) PA_TO_KVA1(C1FIFOUA1);
      
      canmsg_t frame;
      set_can_id(&frame, rx->sid);
      set_can_reply(&frame, get_rtr(rx));
      set_can_len(&frame, get_dlc(rx));
      frame.raw[0] = rx->data[0];
      frame.raw[1] = rx->data[1];
      frame.raw[2] = rx->data[2];
      frame.raw[3] = rx->data[3];
      frame.raw[4] = rx->data[4];
      frame.raw[5] = rx->data[5];
      frame.raw[6] = rx->data[6];
      frame.raw[7] = rx->data[7];

      C1FIFOCON1bits.UINC = 1;

      push_back_from_isr(can_rx_queue, &frame);
      }
    }
  
  // clear the IRQ
  IFS4bits.CAN1IF = 0;
  }
 
result_t bsp_send_can(const canmsg_t *msg)
  {
  pic_can_msg_t *next_buffer;
  
  // spin till a buffer is free
  while(C1FIFOINT0bits.TXNFULLIF == 0)
    {
    C1FIFOINT0bits.TXNFULLIE = 1;   // we want an IRQ when there is at least 1
    semaphore_wait(can_tx_event, INDEFINITE_WAIT);
    }

  enter_critical();
  next_buffer = (pic_can_msg_t *) PA_TO_KVA1(C1FIFOUA0);

  next_buffer->sid = get_can_id(msg);
  set_dlc(next_buffer, get_can_len(msg));
  set_rtr(next_buffer, get_can_reply(msg));
  next_buffer->data[0] = msg->canas.node_id;
  next_buffer->data[1] = msg->canas.data_type;
  next_buffer->data[2] = msg->canas.service_code;
  next_buffer->data[3] = msg->canas.message_code;
  next_buffer->data[4] = msg->canas.data[0];
  next_buffer->data[5] = msg->canas.data[1];
  next_buffer->data[6] = msg->canas.data[2];
  next_buffer->data[7] = msg->canas.data[3];
  
  // increment the fifo
  C1FIFOCON0bits.UINC = 1;
  C1FIFOCON0bits.TXREQ = 1;
  
  exit_critical();
  
  return s_ok;
  }
