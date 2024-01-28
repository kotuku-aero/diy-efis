#include "atomdb_viewport.h"

static result_t render_airspace(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas,
                  const rect_t* clip_rect, viewport_params_t* params)
  {
  on_render_airspace(viewport, id, canvas, clip_rect, params);
  return s_ok;
  }

result_t create_airspace_viewport(handle_t container, const db_header_t* hdr, viewport_t** out)
  {
  result_t result;

  atomdb_viewport_t* viewport;

  if (failed(result = neutron_calloc(1, sizeof(atomdb_viewport_t), (void**)&viewport)))
    return result;

  if (failed(result = create_atomdb_viewport(container, hdr, viewport)))
    {
    neutron_free(viewport);
    return result;
    }

  viewport->base.hdr = hdr;
  viewport->render_polygon = render_airspace;
  viewport->render_polyline = render_airspace;

  *out = &viewport->base;

  return s_ok;
  }

