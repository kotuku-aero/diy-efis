#include "i2c.h"
#include "../neutron/bsp.h"
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <linux/i2c-dev.h>

#define DIYEFIS_ID  0x48
#define RXBF 0x01
#define TXFULL 0x02
#define RXOF 0x04
#define MSKE 0x08
#define CANR 0xF0

typedef struct _i2c_can_t {
  uint16_t version;

  deque_p rx_queue;
  semaphore_p txrdy;
  semaphore_p rxrdy;
  int fd;       // handle of i2c device
  } i2c_can_t;

static i2c_can_t *the_driver;

const char *txrdy_int_s = "txrdy-gpio";
const char *rxrdy_int_s = "rxrdy-gpio";
const char *i2c_device_s = "device";

struct i2c_msg {
__u16 addr;
__u16 flags;
#define I2C_M_TEN		0x0010
#define I2C_M_RD		0x0001
#define I2C_M_NOSTART		0x4000
#define I2C_M_REV_DIR_ADDR	0x2000
#define I2C_M_IGNORE_NAK	0x1000
#define I2C_M_NO_RD_ACK		0x0800
#define I2C_M_RECV_LEN		0x0400
__u16 len;
__u8 * buf;
};
/*
struct i2c_rdwr_ioctl_data {
struct i2c_msg *msgs;  // ptr to array of simple messages
int nmsgs;             // number of messages to exchange 
  }
  */

static semaphore_p i2c_mutex;

//#ifdef _DEBUG
//static void dump_ioctl_msg(const struct i2c_rdwr_ioctl_data *ioctl_msg)
//  {
//  trace_debug("--- ioctl %d msg ----\n", ioctl_msg->nmsgs);
//  uint16_t n;
//  for (n = 0; n < ioctl_msg->nmsgs; n++)
//    {
//    trace_debug("  msg %d\n", n);
//    trace_debug("    .addr = %d\n", ioctl_msg->msgs[n].addr);
//    trace_debug("    .flags = %d\n", ioctl_msg->msgs[n].flags);
//    trace_debug("    .len = %d\n", ioctl_msg->msgs[n].len);
//    trace_debug("    .buf = ");
//    uint16_t i;
//    for (i = 0; i < ioctl_msg->msgs[n].len; i++)
//      trace_debug("%d ", ioctl_msg->msgs[n].buf[i]);
//    trace_debug("\n");
//    }
//  }
//#endif

result_t read_registers(i2c_can_t *driver, uint8_t cmd, uint8_t len, uint8_t *data)
  {
  result_t result = s_ok;
  semaphore_wait(i2c_mutex, INDEFINITE_WAIT);
  if (len > 8)
    return e_bad_parameter;

  // send the start address.
  struct i2c_msg msg =
    {
      .addr = DIYEFIS_ID, .flags = 0, .len = 1, .buf = &cmd,
    };

  struct i2c_rdwr_ioctl_data ioctl_msg = {
    .msgs = &msg,
    .nmsgs = 1
    };

  if (ioctl(driver->fd, I2C_RDWR, &ioctl_msg) < 0)
    result = e_unexpected;
  else
    {
    msg.flags = I2C_M_RD;
    msg.len = len;
    msg.buf = data;

    if (ioctl(driver->fd, I2C_RDWR, &ioctl_msg) < 0)
      result = e_unexpected;
    }

//#ifdef _DEBUG
//  trace_debug("*** read registers ***\n");
//  dump_ioctl_msg(&ioctl_msg);
//#endif
//
  semaphore_signal(i2c_mutex);

  return result;
  }

result_t write_registers(i2c_can_t *driver, uint8_t cmd, uint8_t len, const uint8_t *data)
  {
  result_t result = s_ok;
  semaphore_wait(i2c_mutex, INDEFINITE_WAIT);
  uint8_t buffer[32];
  buffer[0] = cmd;
  memcpy(&buffer[1], data, len);
  struct i2c_msg msg[1] =
    {
      { .addr = DIYEFIS_ID, .flags = 0, .len = 1 + len, .buf = buffer },
    };
  struct i2c_rdwr_ioctl_data ioctl_msg = {
    .msgs = msg,
    .nmsgs = 1
    };

//#ifdef _DEBUG
//  trace_debug("*** Write registers ***\n");
//  dump_ioctl_msg(&ioctl_msg);
//#endif

  if (ioctl(driver->fd, I2C_RDWR, &ioctl_msg) < 0)
    result = e_unexpected;

  semaphore_signal(i2c_mutex);

  return result;
  }

static void rxrdy_int()
  {
  semaphore_signal(the_driver->rxrdy);
  }

static void txrdy_int()
  {
  semaphore_signal(the_driver->txrdy);
  }

static result_t get_status(i2c_can_t *driver, uint8_t *reg)
  {
  return read_registers(driver, 0x10, 1, reg);
  }

static result_t read_msg(i2c_can_t *driver, canmsg_t *msg)
  {
  result_t result;
  uint8_t r30;
  if (failed(result = read_registers(driver, 0x30, 1, &r30)))
    return result;

  uint8_t len = r30 >> 4;
  uint8_t *buffer = (uint8_t *)msg;
  buffer++;

  // burst read the message
  if (failed(result = read_registers(driver, 0x31, len + 1, buffer)))
    return result;

  uint16_t flags = r30 << 8;
  flags |= *buffer;           // get the low 8 bits
  msg->flags = flags;

//#ifdef _DEBUG
//  uint16_t msg_len = get_can_len(msg);
//  trace_debug("can rcv: %d(%d) ", get_can_id(msg), msg_len);
//  int i;
//  for (i = 0; i < msg_len; i++)
//    trace_debug("%02.2x ", msg->raw[i]);
//
//  trace_debug("\n");
//#endif

  return s_ok;
  }

static result_t write_msg(i2c_can_t *driver, const canmsg_t *msg)
  {

//#ifdef _DEBUG
//  uint16_t len = get_can_len(msg);
//  trace_debug("can snd: %d(%d) ", get_can_id(msg), len);
//  int i;
//  for (i = 0; i < len; i++)
//    trace_debug("%02.2x ", msg->raw[i]);
//
//  trace_debug("\n");
//#endif
  // we have to change the format to big endian...
  uint8_t buffer[10];
  buffer[0] = msg->flags >> 8;
  buffer[1] = msg->flags;
  memcpy(buffer + 2, msg->raw, 8);

  // send the flags and as much data as available.
  return write_registers(driver, 0x20, get_can_len(msg) + 2, buffer);
  }

static void rx_worker(void *parg)
  {
  i2c_can_t *driver = (i2c_can_t *)parg;
  uint8_t status;
  while (true)
    {
    if (failed(get_status(driver, &status)))
      {
      trace_critical("Cannot read the status from the i2c\n");
      continue;
      }

    if ((status & RXBF) == 0)
      {
      semaphore_wait(driver->rxrdy, INDEFINITE_WAIT);
      continue;
      }

    canmsg_t msg;
    if (succeeded(read_msg(driver, &msg)))
      push_back(driver->rx_queue, &msg, INDEFINITE_WAIT);
    }
  }

result_t init_hardware(memid_t key)
  {
  if (the_driver == 0)
    {
    result_t result;
    uint16_t rxrdy_irq = 14;      // GPIO14
    uint16_t txrdy_irq = 15;      // GPIO15

    semaphore_create(&i2c_mutex);
    semaphore_signal(i2c_mutex);

    char device_name[REG_STRING_MAX];

    if (failed(result = reg_get_string(key, i2c_device_s, device_name, 0)))
      return result;

    reg_get_uint16(key, txrdy_int_s, &txrdy_irq);
    reg_get_uint16(key, rxrdy_int_s, &rxrdy_irq);

    // open the I2C
    int fd = open(device_name, O_RDWR);
    if (fd < 0)
      return e_bad_handle;

    //unsigned long funcs;

    //if (ioctl(fd  , I2C_FUNCS, &funcs) < 0)
    //  {
    //  return e_bad_handle;
    //  }

    //if ((funcs & I2C_FUNC_I2C) == 0)
    //  {
    //  return e_bad_handle;
    //  }

    // set the device id
    if (ioctl(fd, I2C_SLAVE, DIYEFIS_ID) < 0)
      return e_bad_handle;

    the_driver = (i2c_can_t *)neutron_malloc(sizeof(i2c_can_t));

    // create the irq's
    semaphore_create(&the_driver->rxrdy);
    semaphore_create(&the_driver->txrdy);
    the_driver->version = sizeof(i2c_can_t);
    the_driver->fd = fd;

    wiringPiSetupGpio();

    // hook the interrupts.  We signal the semaphore when they change
    wiringPiISR(rxrdy_irq, INT_EDGE_FALLING, &rxrdy_int);
    wiringPiISR(txrdy_irq, INT_EDGE_RISING, &txrdy_int);
    }

  return s_ok;
  }

#define _DEBUG_SLAVE
#ifdef _DEBUG_SLAVE
static void i2c_tests();
#endif

result_t i2c_init(memid_t key, deque_p rx_queue, handle_t *handle)
  {
  result_t result;
  
  if (failed(result = init_hardware(key)))
    return result;

#ifdef _DEBUG_SLAVE
  i2c_tests();
#endif

  *handle = the_driver;
  the_driver->rx_queue = rx_queue;
  // create the worker task
  if (failed(result = task_create("CANRX", DEFAULT_STACK_SIZE, rx_worker, the_driver, NORMAL_PRIORITY, 0)))
    return result;

  // setup I2C
  uint8_t sr = 0x00;      // close first
  if (failed(result = write_registers(the_driver, 0x10, 1, &sr)))
    return result;
  
  sr |= 0x80;

  return write_registers(the_driver, 0x10, 1, &sr);
  }

result_t i2c_close(handle_t driver)
  {

  }

/* Called from within the tx worker process so we can block
 * without having to worry about a queue
*/
result_t i2c_send_can(handle_t handle, const canmsg_t *msg)
  {
  result_t result;
  i2c_can_t *driver = (i2c_can_t *)handle;
  do
    {
    // get the status register
    uint8_t status;
    if (failed(result = get_status(driver, &status)))
      return result;

    if ((status & TXFULL) == 0)
      return write_msg(driver, msg);

    // wait till a txempty irq is raised.
    semaphore_wait(driver->txrdy, INDEFINITE_WAIT);
    } while (true);
  }

#ifdef _DEBUG_SLAVE
// i2c tests
static const uint8_t tests[7] = {
    0x55,
    0xAA,
    0xBA,
    0xAD,
    0xF0,
    0x0D,
    0x80,
  };

static void i2c_tests()
  {
  uint8_t wr_regs[7];
  uint8_t rd_regs[7];

  uint16_t test_num;
  for (test_num = 0; test_num < 10000; test_num++)
    {
    uint16_t i;
    memcpy(wr_regs, tests, 7);

    if (test_num & 1)
      for (i = 0; i < 7; i++)
        wr_regs[i] = wr_regs[i] ^ 0xff;

    if (failed(write_registers(the_driver, 0x11, 7, wr_regs)))
      {
      trace_error("Unable to write regs on test iteration %d\n", test_num);
      return;
      }

    if (failed(read_registers(the_driver, 0x11, 7, rd_regs)))
      {
      trace_error("Unable to read regs on test iteration %d\n", test_num);
      return;
      }

    if(memcmp(wr_regs, rd_regs, 7)!= 0)
      {
      trace_error("Register comparison failed on test iteration %d\n", test_num);
      return;
      }
    }
  }

#endif
