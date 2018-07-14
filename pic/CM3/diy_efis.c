#include "../../libs/neutron/neutron.h"
#include "../../libs/atom/uart_device.h"
#include "../../libs/atom/microkernel.h"
#include "../../libs/atom/eeprom.h"
#include "../../libs/atom/i2c.h"
#include "../../libs/atom16/pps_maps.h"
#include <ctype.h>

// DSPIC33EP512GP502 Configuration Bit Settings

#include <xc.h>

// FICD
#pragma config ICS = PGD1               // ICD Communication Channel Select bits (Communicate on PGEC1 and PGED1)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)

// FPOR
#pragma config ALTI2C1 = ON             // Alternate I2C1 pins (I2C1 mapped to SDA1/SCL1 pins)
#pragma config ALTI2C2 = ON             // Alternate I2C2 pins (I2C2 mapped to SDA2/SCL2 pins)
#pragma config WDTWIN = WIN25           // Watchdog Window Select bits (WDT Window is 25% of WDT period)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler bits (1:32,768)
#pragma config WDTPRE = PR128           // Watchdog Timer Prescaler bit (1:128)
#pragma config PLLKEN = ON              // PLL Lock Enable bit (Clock switch to PLL source will wait until the PLL lock signal is valid.)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#ifdef _DEBUG
#pragma config FWDTEN = OFF              // Watchdog Timer Enable bit
#else
#pragma config FWDTEN = ON              // Watchdog Timer Enable bit (Watchdog timer always enabled)
#endif

// FOSC
#pragma config POSCMD = XT              // Primary Oscillator Mode Select bits (XT Crystal Oscillator Mode)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function bit (OSC2 is clock output)
#pragma config IOL1WAY = OFF            // Peripheral pin select configuration (Allow multiple reconfigurations)
#pragma config FCKSM = CSDCMD           // Clock Switching Mode bits (Both Clock switching and Fail-safe Clock Monitor are disabled)

// FOSCSEL
#pragma config FNOSC = PRIPLL           // Oscillator Source Selection (Primary Oscillator with PLL module (XT + PLL, HS + PLL, EC + PLL))
#pragma config IESO = ON                // Two-speed Oscillator Start-up Enable bit (Start up device with FRC, then switch to user-selected oscillator source)

// FGS
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GCP = OFF                // General Segment Code-Protect bit (General Segment Code protect is Disabled)

int main(void);

/*
 * diy-efis can interface
 *
 * diy-efis slave header
 * |7 6 5 4 3 2 1 0|  Byte 0
 *  +-----+-----+ |
 *        |       +---- R/W bit (0 = write, 1 = read)
 *        +------------ Slave address Default ix 0x48
 *
 * |7 6 5 4 3 2 1 0|  Byte 1 - Register address
 *  | +-----+-----+
 *  |       +-------- Address (see below)
 *  +---------------- Auto increment.  Will do a multiple read/write till master sets end
 *
 * 1..n bytes of register read/write
 *
 * Register 0x10    Status/control register
 * |7 6 5 4 3 2 1 0|
 *  | 0 0 0 | | | +--- RO RX Ready.   At least 1 canmsg is available fron the FIFO
 *  |       | | +----- RO TX FULL.    The transmit fifo is full
 *  |       | +------- RW RXOVF       Receive overflow.  At least 1 message lost. 
 *  |       +--------- RW MASK        Mask and acceptance registers enable
 *  +----------------- RW Enable      Enable bit.  Default 0.
 *
 * Register 0x11    Version
 * |7 6 5 4 3 2 1 0|
 *  +------+------+
 *         +---------- RO Revision register, 0x00010001
 * Register 0x12    MASK Low
 * |7 6 5 4 3 2 1 0|
 *  +------+------+
 *         +---------- RW Mask bits 0-7  1=enable compare, 0 = don't enable
 * Register 0x13    MASK High
 * |7 6 5 4 3 2 1 0|
 *  0 0 0 0 0 +-+-+
 *              +----- RW Mask bits 8-10 1=enable compare, 0 = don't enable
 * Register 0x14    Acceptance Low
 * |7 6 5 4 3 2 1 0|
 *  +------+------+
 *         +---------- RW Acceptance bits 0-7
 * Register 0x15    MASK High
 * |7 6 5 4 3 2 1 0|
 *  0 0 0 0 0 +-+-+
 *              +----- RW Acceptance bits 8-10
 * Register 0x16    Bitrate low
 * |7 6 5 4 3 2 1 0|
 *  +------+------+
 *         +---------- Low 7 bits of CAN bitrate * 1000
 * Register 0x17    Bitrate high
 * |7 6 5 4 3 2 1 0|
 *  +------+------+
 *         +---------- Upper 8 bits of CAN bitrate * 1000
 * NOTE the maximum value is limited to 1000, minimum is 1
 * Default is set to 125
 *
 * -------------------------- TX FIFO  ---------------------------------------
 *
 * Register 0x20    TX Register 0
 * |7 6 5 4 3 2 1 0|
 *  +--+--+ | +-+-+
 *     |    | +----- W  TX address 8-10
 *     |    +------- W  RTR       Reply bit
 *     +------------ W  LEN       Length.  When written, registers 22-29 can be
 *                                loaded with data, on the <reg>(LEN) write the
 *                                message is sent
 * Register 0x21    TX Register 1
 * |7 6 5 4 3 2 1 0|
 *  +------+------+
 *         +----------  W TX address 0-7
 * Register 0x22-29   TX Buffer
 * |7 6 5 4 3 2 1 0|
 *  +------+------+
 *         +----------  W Data    Data to be sent
 *
 * -------------------------- RX FIFO  ---------------------------------------
 *
 * Register 0x30    RX Register 1
 * |7 6 5 4 3 2 1 0|
 *  +--+--+ | +-+-+
 *     |    | +----- R  RX address 8-10
 *     |    +------- R  RTR       Reply bit
 *     +------------ R  LEN       Length.  Number of bytes available in the RX buffer
 *                                When the last register is read, another will be loaded
 *                                from the RX queue.  Note all 8 can be read, the reload
 *                                trigger is only set when register 31 is read and
 *                                a burst read is being done.
 * Register 0x31    RX Register 0
 * |7 6 5 4 3 2 1 0|
 *  +------+------+
 *         +---------- R  RX address 0-7
 * Register 0x32-39   RX Buffer
 * |7 6 5 4 3 2 1 0|
 *  +------+------+
 *         +---------- R  Data    Data to be received
 *
 * -------------------------- FIFO Operation  ---------------------------------------
 *
 * The intent of the tx and rx fifo is to do burst reads and writes ( Byte 1 bit 7 = 1)
 *
 * The following is a burst write
 *
 * 0x48 <-- command
 * 0xA0 <-- write reg20, burst write
 * 0x61 <-- write 6 bytes, address is 0x1xx
 * 0xAA <-- address bits 0-7 address is 0x1AA
 * 0x00 <-- data byte 1
 * 0x01 <-- data byte 2
 * 0x02 <-- data byte 3
 * 0x03 <-- data byte 4
 * 0x04 <-- data byte 5
 * 0x05 <-- data byte 6
 *
 * once the last register is written the message will be transferred to the tx queue
 *
 * the following is a burst read.  only 6 bytes available
 *
 * 0x49 <-- command - read
 * 0x30 <-- read 30, not a burst
 *
 * 0x6N <-- length is 6 bytes, N bytes remaining
 *
 * 0x49 <-- command - read
 * 0xB1 <-- read register 31, burst read
 *
 * 0xNN <-- byte 1 (Register 31)
 * 0xNN <-- byte 2 (Register 32)
 * 0xNN <-- byte 3 (Register 33)
 * 0xNN <-- byte 4 (Register 34)
 * 0xNN <-- byte 5 (Register 35)
 * 0xNN <-- byte 6 (Register 36)
 * 0xNN <-- byte 7 (Register 37)  -> last data register, load next canmsg from queue
 */

#define DIYEFIS_ID  0x48

#define RXRDY 0x01
#define TXFULL 0x02
#define RXOVF 0x04
#define MSKEN 0x08
#define ENBL  0x80

static uint8_t regs10[8];           // registers 0x10 - 0x17
static uint8_t regs20[10];          // registers 0x20 - 0x29
static uint8_t regs30[10];          // registers 0x30 - 0x39


static neutron_parameters_t init_params = 
  {
  .node_name = "diy-efis",
  .node_type = 0,
  .hardware_revision = 0x30,
  .software_revision = 0x10,
  .bitrate = 125
  };

#define EEPROM_SIZE (128L * 1024L)    // eeprom is 128k bytes
uint32_t fcy = 70000;

static deque_p i2c_rx_queue;      // messages to be sent to the I2C
static deque_p i2c_tx_queue;      // messages received from the I2C

static void set_rxrdy(bool is_it)
  {
  // open collector
  if(is_it)
    {
    regs10[0] |= RXRDY;
    PORTBbits.RB15 = 0;
    TRISBbits.TRISB15 = 0;
    }
  else
    {
    TRISBbits.TRISB15 = 1;
    regs10[0] &= ~RXRDY;
    }
  }

static void set_txfull(bool is_it)
  {
  // open collector
  if(is_it)
    {
    PORTBbits.RB10 = 0;
    TRISBbits.TRISB10 = 0;
    regs10[0] |= TXFULL;
    }
  else
    {
    regs10[0] &= ~TXFULL;
    TRISBbits.TRISB10 = 1;
    }
  }

static result_t process_can(const canmsg_t *msg, void *parg)
  {
  if((regs10[0] & ENBL)== 0)
    return s_ok;
  
  // queue the packet, aloow 5 msec for the queue
  if(failed(push_back(i2c_rx_queue, msg, 5)))
    // set the overflow flag
    regs10[0] |= RXOVF;
  else
    regs10[0] &= ~RXOVF;

  set_rxrdy(true);           // flag at least 1 message
  return s_ok;
  }

static result_t valid_reg(int channel, uint8_t reg)
  {
  if(reg >= 0x10 && reg <= 0x18)
    return s_ok;
  
  if(reg >= 0x30 && reg <= 0x39)
    return s_ok;
  
  if(reg >= 0x20 && reg <= 0x29)
    return s_ok;
  
  return s_false;
  }

static result_t read_reg(int channel, uint8_t reg, uint8_t *value)
  {
  if(reg >= 0x10 && reg < 0x18)
    {
    *value = regs10[reg - 0x10];
    return s_ok;
    }
  
  if(reg >= 0x20 && reg <= 0x29)
    {
    *value = regs20[reg - 0x20];
    return s_ok;
    }
  
  if(reg >= 0x30 && reg <= 0x39)
    {
    // when we read reg 30 we pop the can queue
    if(reg == 0x30)
      {
      canmsg_t msg;
      if(failed(pop_front_from_isr(i2c_rx_queue, &msg)))
        {
        // no more data :-(
        set_rxrdy(false);       // flag no more data
        return e_no_more_information;
        }
      
      // copy the message into the buffer
      regs30[0] = msg.flags >> 8;
      regs30[1] = msg.flags;
      memcpy(&regs30[2], msg.raw, 8);
      }
    
    *value = regs30[reg - 0x30];
    
    if(((regs30[0]>> 4) + 0x31) == reg)  // last byte read?
      {
      uint16_t num_rx;
      count(i2c_rx_queue, &num_rx);
      
      set_rxrdy(num_rx > 0);
      }
    
    return s_ok;
    }
  
  return e_unexpected;
  }

static result_t write_reg(int channel, uint8_t reg_num, uint8_t value)
  {
  if(reg_num >= 0x10 && reg_num < 0x18)
    {
    reg_num -= 0x10;
    
    uint8_t old_reg = regs10[reg_num];
    
    // decide what was changed....
    if(reg_num == 0)
      {
      value &= ENBL;        // only flag we can change
      regs10[0] &= ~ENBL;
      regs10[0] |= value;
      // see if the register changed
      if((old_reg & ENBL) != value)
        {
        if(value != 0)
          {
          // opening the channel
          uint16_t rate = regs10[0x16];
          rate |= regs10[0x17] << 8;
          bsp_set_can_rate(rate);
          
          // do other opening stuff...
          // this will be the masks, for now just accept everything
          set_txfull(false);     // we are ready to send..
          }
        else
          {
          // closing the channel
          reset(i2c_rx_queue);
          reset(i2c_tx_queue);
          
          // clear the irq flags
          set_rxrdy(false);
          set_txfull(false);
          
          // clear the flags
          regs10[0] &= ~RXOVF;
          }
        }
      }
    else
      regs10[reg_num] = value;
    
    
    return s_ok;
    }
  
  if(reg_num >= 0x20 && reg_num < 0x2A)
    {
     regs20[reg_num - 0x20] = value;
    
    if(((regs20[0]>> 4) + 0x21) == reg_num)  // last byte written?
      {
      canmsg_t msg;
      msg.flags = (regs20[0] << 8) | regs20[1];
      if((regs20[0]>> 4) > 0)
        memcpy(msg.raw, &regs20[2], regs20[0]>> 4);
      
      if(failed(push_back(i2c_tx_queue, &msg, 0)))
        {
        set_txfull(true);       // not ready any more...
        return e_no_space;
        }
       
      // if the queue is full then reset ready
      uint16_t num_tx;
      capacity(i2c_tx_queue, &num_tx);
      
      if(num_tx == 0)
        set_txfull(true);       // not ready any more...
      }
    
    return s_ok;
    }
  
  return e_unexpected;
  }

static bool factory_reset = false;

static msg_hook_t hook = { .callback = process_can };

void panic()
  {
  // restart the system using the brown-out
  __builtin_disi(0x3FFF); /* disable interrupts */
  asm volatile("RESET");
  }

void malloc_failed_hook(uint16_t size)
  {
  panic();
  }

static void test_key(uint16_t *prev_state, uint16_t port_bits, uint16_t mask, uint16_t id)
  {
  uint16_t old_state = *prev_state & mask;
  uint16_t masked_bits = port_bits & mask;

  if(old_state != masked_bits)       // test for change
    {
    *prev_state &= ~mask;            // kill bit
    *prev_state |= masked_bits;      // new key
    
    canmsg_t msg;
    
    // if the key is pressed portb will be 0, so send a 1
    push_back(i2c_rx_queue,
              create_can_msg_int16(&msg, id, 0, masked_bits == 0 ? 1 : 0),
              INDEFINITE_WAIT);
    }
  }

static const int16_t transitions[16] = 
  {
   0, -1,  1,  0, 
   1,  0,  0, -1,
  -1,  0,  0,  1,
   0,  1, -1,  0
  };

static void test_encoder(uint16_t *prev_state, uint16_t port_bits, uint16_t a_mask, uint16_t b_mask, uint16_t id)
  {
  // encoder is as follows
  //    A       B
  //    0       0
  //    1       0
  //    1       1
  //    0       1
  uint16_t old_state = *prev_state & (a_mask | b_mask);
  uint16_t masked_bits = port_bits & (a_mask | b_mask);
  
  if(old_state != masked_bits)
    {
    uint16_t state = 0;
    
    if(old_state & a_mask)
      state |= 8;
    if(old_state & b_mask)
      state |= 4;
    
    if(masked_bits & a_mask)
      state |= 2;
    if(masked_bits & b_mask)
      state |= 1;
    
    *prev_state &= ~(a_mask | b_mask);
    *prev_state |= masked_bits;
    
    if(transitions[state] != 0)
      {
      // queue up the message, simulating a message from the canbus
      canmsg_t msg;
      process_can(create_can_msg_int16(&msg, id, 0, transitions[state]), 0);
      }
    }
  }

static void keys_worker(void *parg)
  {
  uint16_t key0_id;
  uint16_t decka_id;
  uint16_t deckb_id;
  
  uint16_t key0_state = 0;
  uint16_t decka_state = 0;
  uint16_t deckb_state = 0;
  
  semaphore_p worker_semp;
  
  semaphore_create(&worker_semp);
  
  if(failed(reg_get_uint16(0, "key0-id", &key0_id)))
    key0_id = id_key0;
  
  if(failed(reg_get_uint16(0, "decka-id", &decka_id)))
    decka_id = id_decka;
  
  if(failed(reg_get_uint16(0, "deckb-id", &deckb_id)))
    deckb_id = id_deckb;
  
  while(true)
    {
    // the worker tests the keys every 100msec
    semaphore_wait(worker_semp, 100);
    
    test_key(&key0_state, PORTB, 1 << 14, key0_id);
    test_encoder(&decka_state, PORTA, 1 << 0, 1 << 1, decka_id);
    test_encoder(&deckb_state, PORTB, 1 << 0, 1 << 1, deckb_id);
    }  
  }

static void main_task(void *parg)
  {
  
  // initialize the eeprom as we need our settings first
  eeprom_init(factory_reset, I2C_CHANNEL_2, EEPROM_SIZE);
  
  // init the slave
  i2cs_init(I2C_CHANNEL_1, DIYEFIS_ID, read_reg, write_reg, valid_reg);

  uint16_t node_id;
  if (failed(reg_get_uint16(0, "node-id", &node_id)))
    node_id = 200;

  init_params.node_id = node_id;
  
  uint16_t tx_queue_length = 256;
  uint16_t rx_queue_length = 256;
  
  reg_get_uint16(0, "tx-queue-len", &tx_queue_length);
  reg_get_uint16(0, "rx-queue-len", &rx_queue_length);
  
  // create a queue
  deque_create(sizeof(canmsg_t), tx_queue_length, &i2c_tx_queue);
  deque_create(sizeof(canmsg_t), rx_queue_length, &i2c_rx_queue);
  
  reg_get_uint16(0, "rate", &init_params.bitrate);
 
  regs10[0x16] = init_params.bitrate;
  regs10[0x17] = init_params.bitrate >> 8;
  
  // start the canbus stuff working
  can_aerospace_init(&init_params, true, false);
  
  subscribe(&hook);
  
  // TODO: cli for the module
  
  task_create("KEYS", DEFAULT_STACK_SIZE, keys_worker, 0, NORMAL_PRIORITY, 0);
  
  // now we just spin
  while(true)
    {
    canmsg_t msg;
    if(succeeded(pop_front(i2c_tx_queue, &msg, INDEFINITE_WAIT)))
      {
      if((regs10[0] & ENBL) != 0)
        can_send_raw(&msg);         // send to the bus..
      }
    
    uint16_t tx_cap;
    capacity(i2c_tx_queue, &tx_cap);
    if(tx_cap > 2)
      set_txfull(false);            // flag we can send
    }
  }

#define I_PLLPRE 0
#define I_PLLPOST 0
#define I_FOSC 140000000
#define I_FIN 10000000
#define I_PLLDIV 54 // ((I_OSC/I_FIN)*(I_PLLPRE+2)*(2*(I_PLLPOST+1)))-2

int main(void)
  {
  ANSELAbits.ANSA0 = 0;
  ANSELAbits.ANSA1 = 0;
  ANSELBbits.ANSB0 = 0;
  ANSELBbits.ANSB1 = 0;
  TRISAbits.TRISA0 = 1;
  TRISAbits.TRISA1 = 1;
  TRISBbits.TRISB0 = 1;
  TRISBbits.TRISB1 = 1;
  TRISBbits.TRISB10 = 1;
  TRISBbits.TRISB12 = 1;
  TRISBbits.TRISB15 = 1;
  CNPUAbits.CNPUA0 = 1;
  CNPUAbits.CNPUA1 = 1;
  CNPUBbits.CNPUB8 = 1;
  CNPUBbits.CNPUB9 = 1;
  CNPUBbits.CNPUB10 = 1;
  CNPUBbits.CNPUB12 = 1;
  CNPUBbits.CNPUB14 = 1;
  CNPUBbits.CNPUB15 = 1;
 
  // set up the PLL for 20Mips
  // FOSC = 10mHz
  // FPLLI = 5Mhz
  // FSYS = 280Mhz
  // FOSC = 40Mhz
  PLLFBD = I_PLLDIV;                      // PLL divider
  CLKDIVbits.PLLPRE = I_PLLPRE;           // Prescaler divide by 2
  CLKDIVbits.PLLPOST = I_PLLPOST;         // Postscale divide by 2
  
  
    // Initiate Clock Switch to Primary Oscillator with PLL (NOSC=0b011)
  if(OSCCONbits.COSC != 0b011)
    {
    __builtin_write_OSCCONH(0x03);
    __builtin_write_OSCCONL(OSCCON | 0x01);
    // Wait for Clock switch to occur
    while (OSCCONbits.COSC!= 0b011);
    }
  // Wait for PLL to lock
  while (OSCCONbits.LOCK!= 1);
 
  // unlock the PPS functions
  __builtin_write_OSCCONL(OSCCON & 0xbf);
  // map pins
  map_rpo(RP36R, rpo_c1tx);
  map_rpi(rpi_rpi45, rpi_c1rx);

  factory_reset = PORTBbits.RB12 == 0;
  
  
  // pick up a small malloc amount to get the heap pointer
  uint32_t mem = (uint32_t) malloc(4);
  mem = ((mem -1) | 0x0f)+1;
  // this is for dsPIC33EP256GP which is the largest we support
  // we loose 20k as the code space is mapped
  size_t length = 0x8000 - mem;
  
  neutron_run((void *)mem, length, "MAIN", DEFAULT_STACK_SIZE << 1, 
              main_task, 0, NORMAL_PRIORITY, 0);

  
  return 0;
  }
