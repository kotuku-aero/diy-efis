#include "../../src/mfdlib/mfd.h"

#include "nav.h"

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

  return run_proton(0, create_layout, create_nav, mfd_wndproc);
  }

