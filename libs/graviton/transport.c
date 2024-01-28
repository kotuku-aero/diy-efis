#include "viewport.h"

typedef struct _transport_viewport_t {
  viewport_t base;

  } transport_viewport_t;


static result_t close(viewport_t* handle)
  {
  transport_viewport_t* viewport = (transport_viewport_t*)handle;

  close_handle(viewport->base.container);

  return neutron_free(handle);
  }

static result_t select(viewport_t* vp, const viewport_params_t* params, handle_t canvas)
  {
  return e_not_implemented;
  }


result_t create_transport_viewport(handle_t file, const db_header_t* hdr, viewport_t** out)
  {
  result_t result;

  transport_viewport_t* viewport;

  if (failed(result = neutron_malloc(sizeof(transport_viewport_t), (void**)&viewport)))
    return result;

  viewport->base.select = select;
  viewport->base.close = close;
  viewport->base.container = file;

  viewport->base.hdr = hdr;

  *out = &viewport->base;

  return s_ok;
  }

