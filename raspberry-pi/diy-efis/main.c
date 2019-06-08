// Basic can aerospace framework support files
#include "../../libs/electron/electron.h"

#include "../../libs/proton/proton.h"
#include "../../libs/muon/muon.h"

#include "diyefis_cli.h"

#define CAN_TX_BUFFER_LEN 1024
#define CAN_RX_BUFFER_LEN 1024

const char *node_name = "diy-efis";

/*
 * This build uses the SLCAN as an interface.  THe i2c version of the pi interface
 * works perfectly, the Raspberry-pi does not.  This was a big waste of time.  The device
 * does not implement clock stretching so the i2c interface sits abandoned.
 * fortunately the serial interface (USB) does work so the SLCAN is enabled.
 * defaults to ttyUSB0 so easy to implement
*/

#define _SLCAN

int main(int argc, char **argv)
  {
  result_t result = electron_init(argc, argv);

  // register the muon command line handler
  muon_initialize_cli(&diyefis_cli_root);


  uint16_t node_id;
  if(failed(reg_get_uint16(0, "node-id", &node_id)))
    node_id = mfd_node_id;

  neutron_parameters_t init_params;
  init_params.node_id = (uint8_t) node_id;
  init_params.node_type = unit_pi;
  init_params.hardware_revision = 0x11;
  init_params.software_revision = 0x10;
  init_params.tx_length = CAN_TX_BUFFER_LEN;
  init_params.tx_stack_length = 4096;
  init_params.rx_length = CAN_RX_BUFFER_LEN;
  init_params.rx_stack_length = 4096;
  init_params.publisher_stack_length = 4096;

  // start the canbus stuff working
  can_aerospace_init(&init_params, result == s_false, false);

  run_proton(0);

  return 0;
  }

void bsp_invalidate_framebuffer()
  {
  invalidate_rect(main_window, 0);
  }

bool bsp_repaint_framebuffer()
  {

  // when next the paint message is found a repaint will be done.
  return true;
  }

static handle_t driver;

#include "../../libs/neutron/slcan.h"
#include "../../libs/electron/i2c.h"

static bool i2c_can = false;

result_t bsp_can_init(deque_p rx_queue, uint16_t bitrate)
  {
  result_t result;

  if (failed(result = reg_get_bool(0, "12c_can", &i2c_can)))
    i2c_can = false;

  memid_t key;
  if (failed(result = reg_open_key(0, "electron", &key)))
    return result;

  if (i2c_can)
    return i2c_init(key, rx_queue, &driver);

  return slcan_create(key, rx_queue, &driver);
  }

result_t bsp_send_can(const canmsg_t *msg)
  {
  if(msg == 0 || driver == 0)
    return e_bad_parameter;

  if(i2c_can)
    return i2c_send_can(driver, msg);

  return slcan_send(driver, msg);
  }

