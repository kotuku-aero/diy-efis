#include "../../src/mfdlib/mfd.h"

#include "aio.h"

static result_t my_wndproc(handle_t hndl, const canmsg_t* msg, void* wnddata)
  {
  // add event handlers here

  return mfd_wndproc(hndl, msg, wnddata);
  }

#ifndef MIPS32_BUILD
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

  return run_proton(0, create_layout, create_aio, my_wndproc);
  }

