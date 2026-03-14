#include "../../src/mfdlib/mfd.h"
#include "../../src/proton/include/menu_window.h"
#include "../../src/mfdlib/event_handlers.h"
#include "../../src/mfdlib/menu_handlers.h"

#include "hsi_au_nz.h"

static result_t my_wndproc(handle_t hndl, const canmsg_t* msg, void* wnddata)
  {
  // add event handlers here

  return mfd_wndproc(hndl, msg, wnddata);
  }

#ifdef _MSC_VER
// debug arguments:
// -c 4096 -x 320 -y 240 canfly.cdb
int main(int argc, char** argv)
  {
  if (failed(canfly_init(argc, argv)))
    return -1;
#else
int main()
  {
#endif

  return run_proton(0, create_layout, create_hsi_au_nz, my_wndproc);
  }

