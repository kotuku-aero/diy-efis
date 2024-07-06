#include "atomdb_viewport.h"

static result_t render_polygon(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, const viewport_params_t* params)
  {
  const map_theme_t* theme = params->map->is_night ? &params->map->night_theme : &params->map->day_theme;

  return polygon(canvas, clip_rect, color_hollow, theme->water_color, points_count(&viewport->gdi_points), points_begin(&viewport->gdi_points));
  }

static result_t render_polyline(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, const viewport_params_t* params)
  {
  const map_theme_t* theme = params->map->is_night ? &params->map->night_theme : &params->map->day_theme;

  return polyline(canvas, clip_rect, theme->water_color, points_count(&viewport->gdi_points), points_begin(&viewport->gdi_points));
  }

result_t create_surface_water_viewport(handle_t container, const db_header_t* hdr, viewport_t** out)
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

  viewport->base.max_scale = int_to_fixed(100);
  viewport->base.min_scale = float_to_fixed(0.1f);
  viewport->render_polygon = render_polygon;
  viewport->render_polyline = render_polyline;

  *out = &viewport->base;

  return s_ok;
  }

