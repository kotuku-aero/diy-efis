#include "viewport.h"
#include "layer.h"

// TODO: only handles GTOPO 30 at present....
typedef struct {
  int32_t lat : 16;
  int32_t lng : 16;
  } tile_id_t;

static inline bool is_same_tile(const tile_id_t* left, const tile_id_t* right)
  {
  return left->lng == right->lng && left->lat == right->lat;
  }

static inline bool is_unused_tile(const tile_id_t* tile)
  {
  return tile->lat == 0 && tile->lng == 0;
  }

typedef struct _terrain_viewport_t {
  viewport_t base;

  tile_id_t loaded_tile;      // holds what tile cached
  tile_t tile;                // currently loaded tile
  } terrain_viewport_t;


static inline bool tile_contains_point(const tile_id_t* tile, const latlng_t* pt)
  {
  return tile->lng == fixed_to_int(pt->lng) && tile->lat == fixed_to_int(pt->lat);
  }

/**
 * @brief Return a cached pixel
 * @param wnd
 * @param geo
 * @return
*/
static const pixel_t* pixel_at(terrain_viewport_t* viewport, const latlng_t* geo)
  {
  const tile_t* tile = 0;
  int32_t geo_lat = fixed_to_int(geo->lat);
  int32_t geo_lng = fixed_to_int(geo->lng);

  if(geo_lng < 0)
    --geo_lng;        // look at the tile to the left

  // see which tile row
  if (viewport->loaded_tile.lat != geo_lat || viewport->loaded_tile.lng != geo_lng)
    {
    // load a new tile
    if (failed(tile_at(viewport->base.container, geo_lat, geo_lng, &viewport->tile)))
      return 0;

    viewport->loaded_tile.lat = geo_lat;
    viewport->loaded_tile.lng = geo_lng;
    }

  int lng_min = spatial_min(geo->lng);
  int lng_sec = spatial_sec(geo->lng);

  int32_t index_x = (lng_min * 60) + lng_sec;
  index_x /= 30;        // how many arc-seconds

  int lat_min = spatial_min(geo->lat);
  int lat_sec = spatial_sec(geo->lat);

  int32_t index_y = (lat_min * 60) + lat_sec;
  index_y /= 30;

  // the y pixels are stored with the index[0] = most northerly
  // point so make inverted
  index_y = 119 - index_y;

  assert(index_x < 120 && index_y < 120);

  // trace_info("Lng: %f Lat: %f Pel: y=%d : x=%d\n",  fixed_to_float(geo->lng), fixed_to_float(geo->lat), index_y, index_x);

  // TODO : check this in gtopo conversion
  return &viewport->tile[index_y][index_x];
  }

static inline color_t hypsometric_tint(terrain_viewport_t* viewport, const terrain_params_t* params, const pixel_t* pix)
  {
  // todo: pass in day/night mode
  const map_theme_t* theme = &params->base.map->day_theme;

  // special case for water.  gtopo30 stores water
  // as -9999, we also don't store empty tiles
  if (pix == 0 || pix->elevation == -9999)
    return color_hollow;
//    return theme->water_color;

  color_t pixel = theme->land_color;
  int16_t last_elev = 0;

  if (params->show_terrain)
    {
    // shade the pixel to amsl elevations
    for (int i = 0; i < 20; i++)
      {
      // so the last is always used as default
      pixel = theme->theme[i].color;
      if (pix->elevation > last_elev &&
        pix->elevation <= theme->theme[i].elevation)
        {
        if (params->hypsometric_tint && i < 19)
          {
          // calculate the distance between elevations 
          // this is not good for embedded (yet)
          double elev = pix->elevation - last_elev;
          double dist = theme->theme[i].elevation - last_elev;

          double pct = elev / dist;

          uint32_t pix_r = red(pixel);
          uint32_t pix_g = green(pixel);
          uint32_t pix_b = blue(pixel);

          int32_t r = red(theme->theme[i + 1].color) - pix_r;
          int32_t g = green(theme->theme[i + 1].color) - pix_g;
          int32_t b = blue(theme->theme[i + 1].color) - pix_b;

          uint32_t grad_r = (uint32_t)(pix_r + pct * r);
          uint32_t grad_g = (uint32_t)(pix_g + pct * g);
          uint32_t grad_b = (uint32_t)(pix_b + pct * b);

          pixel = RGB(grad_r, grad_g, grad_b);
          }
        break;
        }
      last_elev = theme->theme[i].elevation;
      }
    }

  bool hillshade = params->show_hillshade;

  // is the pixel witin range
  if (params->show_terrain_warning/* && distance_to_pixel <= wnd->terrain_warning_distance*/)
    {
    int32_t agl = params->base.map->ned_center.elevation - pix->elevation;
    if (agl <= params->alarm_elevation)
      {
      pixel = theme->alarm_color;
      hillshade = false;
      }
    else if (agl < params->warning_elevation)
      {
      pixel = theme->warning_color;
      hillshade = false;
      }
    }

  if (hillshade)
    {
    uint32_t shade = pix->shade;

    shade = 255 - shade;

    uint32_t r = (red(pixel) * shade) >> 8;
    uint32_t g = (green(pixel) * shade) >> 8;
    uint32_t b = (blue(pixel) * shade) >> 8;
    uint32_t a = alpha(pixel);

    // shade the pixel based on the slope
    pixel = RGBA(r, g, b, a);
    }
  return pixel;
  }

// fast copy when mdm_north
static void stretch_copy(terrain_viewport_t* viewport, const rect_t* clip_rect,
  const terrain_params_t* params, handle_t canvas,
  const pixel_rhombus_t* screen, const geo_rhombus_t* cover)
  {
  // calculate number of pixels to stretch by
  // params->base.map->scale is scale in meters, which is 1 minute
  // of longitude si scale is minutes / screen width

  // was fixed_to_int(scale_nm(params->base.map->scale)) * SPATIAL_ONE_MINUTE
  // #define SPATIAL_ONE_MINUTE fix16_from_float(1.0f / 60.0f)
  // #define SPATIAL_ONE_MINUTE (1092)
  // make_fixed(0, wnd->params.scale, 0)
  // (SPATIAL_ONE_MINUTE * min)

  fixed_t geo_incr = make_fixed(0, params->base.map->scale / 1850, 0);
  // this calculates how many 
  geo_incr = idiv_fixed(geo_incr, screen->top_right.x - screen->bottom_left.x);

  const map_theme_t* theme = &params->base.map->day_theme;


  latlng_t spatial_pixel;
  copy_latlng(&cover->top_left, &spatial_pixel);

  // each geopixel is 1 minute so cache the last one read
  int spatial_x_deg = fixed_to_int(spatial_pixel.lng);
  int spatial_x_min = spatial_min(spatial_pixel.lng);

  int spatial_y_deg = fixed_to_int(spatial_pixel.lat);
  int spatial_y_min = spatial_min(spatial_pixel.lat);

  // get the tile data from the tile
  const pixel_t* pix = pixel_at(viewport, &spatial_pixel);

  point_t pt;
  for (pt.y = screen->top_left.y; pt.y < screen->bottom_right.y; pt.y++)
    {
    for (pt.x = screen->top_left.x; pt.x < screen->bottom_right.x; pt.x++)
      {
      color_t color = hypsometric_tint(viewport, params, pix/*, spatial_distance(&wnd->position.pos, &spatial_pixel)*/);
      set_pixel(canvas, clip_rect, &pt, color, 0);

      spatial_pixel.lng = add_fixed(spatial_pixel.lng, geo_incr);

      if (spatial_x_deg != fixed_to_int(spatial_pixel.lng) || spatial_x_min != spatial_min(spatial_pixel.lng))
        {
        pix = pixel_at(viewport, &spatial_pixel);

        spatial_x_deg = fixed_to_int(spatial_pixel.lng);
        spatial_x_min = spatial_min(spatial_pixel.lng);
        }
      }

    // restore the x pixel
    spatial_pixel.lng = cover->top_left.lng;
    spatial_x_deg = fixed_to_int(spatial_pixel.lng);
    spatial_x_min = spatial_min(spatial_pixel.lng);

    spatial_pixel.lat = sub_fixed(spatial_pixel.lat, geo_incr);
    if (spatial_y_deg != fixed_to_int(spatial_pixel.lat) || spatial_y_min != spatial_min(spatial_pixel.lat))
      {
      pix = pixel_at(viewport, &spatial_pixel);

      spatial_y_deg = fixed_to_int(spatial_pixel.lat);
      spatial_y_min = spatial_min(spatial_pixel.lat);
      }
    }

  }

static result_t select(viewport_t* vp, const viewport_params_t* _params, handle_t canvas)
  {
  terrain_viewport_t* viewport = (terrain_viewport_t*)vp;
  terrain_params_t *params = (terrain_params_t*)_params;

  extent_t canvas_ex;
  canvas_extents(canvas, &canvas_ex);
  rect_t canvas_rect = { 0, 0, canvas_ex.dx, canvas_ex.dy };

  switch (params->base.map->mode)
    {
    case mdm_north:
      stretch_copy(viewport, &canvas_rect, params, canvas, &params->base.map->screen_pos, &params->base.map->geo_pos);
      break;
    case mdm_track:
    case mdm_course:
      break;
    }
  return s_ok;
  }


static result_t close(viewport_t* handle)
  {
  terrain_viewport_t* viewport = (terrain_viewport_t*)handle;

  close_handle(viewport->base.container);

  return neutron_free(handle);
  }

result_t create_terrain_viewport(handle_t file, const db_header_t* hdr, viewport_t** out)
  {
  result_t result;

  terrain_viewport_t* viewport;

  if (failed(result = neutron_malloc(sizeof(terrain_viewport_t), (void**)&viewport)))
    return result;

  viewport->base.select = select;
  viewport->base.close = close;
  viewport->base.container = file;
  viewport->base.hdr = hdr;

  *out = &viewport->base;

  return s_ok;
  }
