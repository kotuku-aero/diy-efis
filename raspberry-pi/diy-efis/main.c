// Basic can aerospace framework support files
#include "../../libs/electron/electron.h"

#include "../../libs/proton/proton.h"
#include "../../libs/muon/muon.h"

#define CAN_TX_BUFFER_LEN 1024
#define CAN_RX_BUFFER_LEN 1024

const char *node_name = "diy-efis";

int main(int argc, char **argv)
  {
	// The command line can pass in the name of the registry used to set us up.  In any
  // case we need to implement some code
  const char *ini_path;
  if(argc > 1)
    ini_path = argv[1];
  else
    ini_path = "diy-efis.reg";

  // TODO: handle this better
  bool factory_reset = false;

  electron_init(ini_path, factory_reset);

  // register the muon command line handler
  muon_initialize_cli();


  uint16_t node_id;
  if(failed(reg_get_uint16(0, "node-id", &node_id)))
    node_id = mfd_node_id;

  nuetron_parameters_t init_params;
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
  nuetron_init(&init_params, factory_reset);

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

#include "../../libs/nuetron/slcan.h"

static handle_t driver;

result_t bsp_can_init(handle_t rx_queue)
  {
  result_t result;
  memid_t key;
  if(failed(result = reg_open_key(0, "slcan", &key)))
    return result;

  return slcan_create(key, rx_queue, &driver);
  }

result_t bsp_send_can(const can_msg_t *msg)
  {
  if(msg == 0 || driver == 0)
    return e_bad_parameter;

  return slcan_send(driver, msg);
  }
