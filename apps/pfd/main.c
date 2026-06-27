#include "../../src/mfdlib/mfd.h"
#include "../../src/proton/include/menu_window.h"

// layout files
#include "pfd.h"

uint32_t num_key_captions = 7;
key_number_t key_captions[7] = { kn_decka | kn_key0, kn_key2, kn_key3, kn_key4, kn_key5, kn_key6, kn_deckb | kn_key1 };


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

  return run_proton(0, create_layout, create_pfd, mfd_wndproc);
  }
