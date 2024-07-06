#include "photon_priv.h"


const typeid_t framebuffer_type =
  {
  .name = "framebuffer_t"
  };

result_t photon_init(const photon_parameters_t *params, bool init_mode)
  {
  if (params == 0)
    return e_bad_parameter;

  // get the startup screen.
  return initialize_screen(params);
  }

