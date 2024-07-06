#include "atomdb_viewport.h"

#include "../atomdb/db.h"
#include "../graviton/schema.h"

typedef struct _contours_viewport_t {
  atomdb_viewport_t base;

  uint32_t cached_elevations[8];
  color_t cached_fill_colors[8];
  color_t cached_outline_colors[8];
  } contours_viewport_t;

static int calculate_theme_index(contours_viewport_t* viewport, uint32_t elevation)
  {
  for (int i = 0; i < 7; i++)
    {
    if (elevation >= viewport->cached_elevations[i] && elevation < viewport->cached_elevations[i + 1])
      return i;
    }

  return 7;
  }

static void build_color_cache(const contours_params_t* params, contours_viewport_t* viewport)
  {
  // set to 1
  int32_t base_alt = 0;
  const map_theme_t* theme = params->base.map->is_night ? &params->base.map->night_theme : &params->base.map->day_theme;
  for (int i = 0; i < 8; i++)
    {
    viewport->cached_elevations[i] = base_alt;
    base_alt = theme->theme[i].elevation;       // next one
    viewport->cached_fill_colors[i] = theme->theme[i].color;
    viewport->cached_outline_colors[i] = theme->theme[i].contour_color;
    }
  }

static result_t render_polygon(atomdb_viewport_t* vp, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, const viewport_params_t* params, uint32_t len, const point_t* points)
  {
  contours_viewport_t *viewport = (contours_viewport_t *)vp;

  for (gdi_polyline_t* it = gdi_polylines_begin(&vp->gdi_polylines); it != gdi_polylines_end(&vp->gdi_polylines); it++)
  {
    // these are colorised based on the hypsometric tinting. rather than trying to compare
    // lower/upper contors the elevation of the contor is used. This sets the minimum
    // elevation.  The hypsometric tinting data is the other way :<
    int color_index = 0;
    if(int16_ts_count(&it->gdi_elevations)> 0)
      color_index = calculate_theme_index(viewport, *int16_ts_begin(&it->gdi_elevations));

    color_t contour_color = viewport->cached_outline_colors[color_index];
    color_t fill_color = viewport->cached_fill_colors[color_index];


    polygon(canvas, clip_rect, contour_color, fill_color, points_count(&it->gdi_points), points_begin(&it->gdi_points));
    }

  return s_ok;
  }

result_t create_contours_viewport(handle_t container, const db_header_t* hdr, viewport_t** out)
  {
  result_t result;

  contours_viewport_t* viewport;

  if (failed(result = neutron_calloc(1, sizeof(contours_viewport_t), (void**)&viewport)))
    return result;

  create_atomdb_viewport(container, hdr, &viewport->base);

  viewport->base.base.hdr = hdr;

  *out = &viewport->base.base;

  return s_ok;
  }

