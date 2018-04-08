#include "i2c.h"
#include "../neutron/bsp.h"
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <linux/i2c-dev.h>

#define DIYEFIS_ID  0x48
#define RXBF 0x01
#define TXBE 0x02
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

static result_t read_registers(i2c_can_t *driver, uint8_t cmd, uint8_t len, uint8_t *data)
  {
  struct i2c_msg msg[2] =
    {
      { .addr = DIYEFIS_ID | 1, .flags = 0, .len = 1, .buf = &cmd },
      { .addr = DIYEFIS_ID, .flags = I2C_M_RD, .len = len, .buf = data }
    };
  struct i2c_rdwr_ioctl_data ioctl_msg = {
    .msgs = msg,
    .nmsgs = 2
    };

  if (ioctl(driver->fd, I2C_RDWR, &ioctl_msg) < 0)
    return e_unexpected;

  return s_ok;
  }

static result_t write_registers(i2c_can_t *driver, uint8_t cmd, uint8_t len, const uint8_t *data)
  {
  char buffer[32];
  buffer[0] = cmd;
  memcpy(buffer[1], data, len);
  struct i2c_msg msg[1] =
    {
      { .addr = DIYEFIS_ID, .flags = 0, .len = 1 + len, .buf = buffer },
    };
  struct i2c_rdwr_ioctl_data ioctl_msg = {
    .msgs = msg,
    .nmsgs = 1
    };

  if (ioctl(driver->fd, I2C_RDWR, &ioctl_msg) < 0)
    return e_unexpected;

  return s_ok;
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
  if (failed(result = read_registers(driver, 0xB1, len + 1, buffer)))
    return result;

  uint16_t flags = r30 << 8;
  flags |= *buffer;           // get the low 8 bits
  msg->flags = flags;

  return s_ok;
  }

static result_t write_msg(i2c_can_t *driver, const canmsg_t *msg)
  {
  // we have to change the format to big endian...
  uint8_t buffer[10];
  buffer[0] = msg->flags >> 8;
  buffer[1] = msg->flags;
  memcpy(buffer + 2, msg->raw, 8);

  // send the flags and as much data as available.
  return write_registers(driver, 0xA0, msg->length + 2, buffer);
  }

static void rx_worker(void *parg)
  {
  i2c_can_t *driver = (i2c_can_t *)parg;
  while (true)
    {
    semaphore_wait(driver->rxrdy, INDEFINITE_WAIT);

    uint8_t status;
    while (succeeded(get_status(driver, &status)) &&
      ((status & RXBF) != 0))
      {
      canmsg_t msg;
      if (succeeded(read_msg(driver, &msg)))
        push_back(driver->rx_queue, &msg, INDEFINITE_WAIT);
      }
    }
  }

result_t i2c_init(memid_t key, deque_p rx_queue, handle_t *handle)
  {
  result_t result;
  uint16_t rxrdy_irq = 14;      // GPIO14
  uint16_t txrdy_irq = 15;      // GPIO15

  char device_name[REG_STRING_MAX];

  if (failed(result = reg_get_string(key, i2c_device_s, device_name, 0)))
    return result;

  reg_get_uint16(key, txrdy_int_s, &txrdy_irq);
  reg_get_uint16(key, rxrdy_int_s, &rxrdy_irq);

  // open the I2C
  int fd = open(device_name, O_RDWR);
  if (fd < 0)
    return e_bad_handle;

  // set the device id
  if (ioctl(fd, I2C_SLAVE, DIYEFIS_ID) < 0)
    return e_bad_handle;

  i2c_can_t *driver = (i2c_can_t *)neutron_malloc(sizeof(i2c_can_t));
  
  // create the irq's
  semaphore_create(&driver->rxrdy);
  semaphore_create(&driver->txrdy);
  driver->version = sizeof(i2c_can_t);
  driver->rx_queue = rx_queue;
  driver->fd = fd;

  the_driver = driver;
  *handle = driver;


  // create the worker task
  if (failed(result = task_create("CANRX", DEFAULT_STACK_SIZE, rx_worker, driver, NORMAL_PRIORITY, 0)))
    return result;

  // hook the interrupts.  We signal the semaphore when they change
  wiringPiISR(rxrdy_irq, INT_EDGE_FALLING, &rxrdy_int);
  wiringPiISR(txrdy_irq, INT_EDGE_RISING, &txrdy_int);

  // setup I2C
  uint8_t sr = 0xC0;      // open bit and 125 kbs
  return write_registers(driver, 0x10, 1, &sr);
  // opening the port will generate an txe interrupt...
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

    if (status & TXBE)
      return write_msg(driver, msg);

    // wait till a txempty irq is raised.
    semaphore_wait(driver->txrdy, INDEFINITE_WAIT);
    } while (true);
  }
