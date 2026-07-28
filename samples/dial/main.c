#include "../../src/mfdlib/mfd.h"
#include "../../src/proton/include/menu_window.h"
#include "../../src/mfdlib/event_handlers.h"
#include "../../src/mfdlib/menu_handlers.h"

// layout files
#include "au_nz.h"
#include "au_nz_priv.h"

uint32_t num_key_captions = 7;
key_number_t key_captions[7] = { kn_decka | kn_key0, kn_key2, kn_key3, kn_key4, kn_key5, kn_key6, kn_deckb | kn_key1 };



static result_t my_wndproc(handle_t hndl, const canmsg_t* msg, void* wnddata)
  {

  return mfd_wndproc(hndl, msg, wnddata);
  }

#ifdef _MSC_VER
// debug arguments:
// -c 4096 -x 1024 -y 600 canfly.cdb
int main(int argc, char** argv)
  {
  if (failed(canfly_init(argc, argv)))
    return -1;
#else
int main()
  {
#endif

  return run_proton(0, create_layout, create_au_nz, my_wndproc);
  }
