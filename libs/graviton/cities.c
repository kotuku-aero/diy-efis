#include "atomdb_viewport.h"

static result_t render_polygon(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t *clip_rect, const viewport_params_t* params)
  {
  const map_theme_t* theme = params->map->is_night ? &params->map->night_theme : &params->map->day_theme;
  for (gdi_polyline_t* it = gdi_polylines_begin(&viewport->gdi_polylines); it != gdi_polylines_end(&viewport->gdi_polylines); it++)
    polygon(canvas, clip_rect, color_hollow, theme->residential_area, points_count(&it->gdi_points), points_begin(&it->gdi_points));

  return s_ok;
  }

result_t create_cities_viewport(handle_t container, const db_header_t* hdr, viewport_t** out)
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
  viewport->render_polygon = render_polygon;

  *out = &viewport->base;

  return s_ok;
  }

