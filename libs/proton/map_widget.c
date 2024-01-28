#include "map_widget.h"
#include "../graviton/schema.h"
#include "../atomdb/spatial.h"
#include "proton.h"
#include <stdio.h>

typedef struct _map_entity_t map_entity_t;

static canmsg_t request_paint = { id_request_paint_background };

static void paint_worker(void* args)
  {
  map_widget_t* wnd = (map_widget_t*)args;

  while (!wnd->terminate_background_worker)
    {
    semaphore_wait(wnd->worker_start, INDEFINITE_WAIT);

#ifdef _WIN32
    extent_t ex;
    canvas_extents(wnd->background_bitmap, &ex);
    rect_t rect;
    rect_create_ex(0, 0, ex.dx, ex.dy, &rect);

    rectangle(wnd->background_bitmap, &rect, color_hollow, wnd->params.day_theme.water_color, &rect);

#endif

    viewport_t *viewport;

    int scale_in_nm = wnd->params.scale / 1850;

    // always render the coastlines
    for (viewportp_t* vp = viewportps_begin(&wnd->coastlines_layers); vp != viewportps_end(&wnd->coastlines_layers); vp++)
      {
      viewport = *vp;
      if (viewport->hdr->min_scale <= scale_in_nm && viewport->hdr->max_scale > 0 ? viewport->hdr->max_scale > scale_in_nm : true)
        select_and_render_viewport(viewport, &wnd->coastlines_params.base, wnd->background_bitmap);
      }

    for (viewportp_t* vp = viewportps_begin(&wnd->landareas_layers); vp != viewportps_end(&wnd->landareas_layers); vp++)
      {
      viewport = *vp;
      if (viewport->hdr->min_scale <= scale_in_nm && viewport->hdr->max_scale > 0 ? viewport->hdr->max_scale > scale_in_nm : true)
        select_and_render_viewport(viewport, &wnd->landareas_params.base, wnd->background_bitmap);
      }

    if (wnd->show_terrain && wnd->terrain_layer != nullptr)
      select_and_render_viewport(wnd->terrain_layer, &wnd->terrain_params.base, wnd->background_bitmap);

    for (viewportp_t* vp = viewportps_begin(&wnd->contours_layers); vp != viewportps_end(&wnd->contours_layers); vp++)
      {
      viewport = *vp;
      if (viewport->hdr->min_scale <= scale_in_nm && viewport->hdr->max_scale > 0 ? viewport->hdr->max_scale > scale_in_nm : true)
        select_and_render_viewport(viewport, &wnd->contours_params.base, wnd->background_bitmap);
      }

    for (viewportp_t* vp = viewportps_begin(&wnd->cities_layers); vp != viewportps_end(&wnd->cities_layers); vp++)
      {
      viewport = *vp;
      if (viewport->hdr->min_scale <= scale_in_nm && viewport->hdr->max_scale > 0 ? viewport->hdr->max_scale > scale_in_nm : true)
        select_and_render_viewport(viewport, &wnd->cities_params.base, wnd->background_bitmap);
      }

    for (viewportp_t* vp = viewportps_begin(&wnd->water_layers); vp != viewportps_end(&wnd->water_layers); vp++)
      {
      viewport = *vp;
      if (viewport->hdr->min_scale <= scale_in_nm && viewport->hdr->max_scale > 0 ? viewport->hdr->max_scale > scale_in_nm : true)
        select_and_render_viewport(viewport, &wnd->water_params.base, wnd->background_bitmap);
      }

    for (viewportp_t* vp = viewportps_begin(&wnd->transport_layers); vp != viewportps_end(&wnd->transport_layers); vp++)
      {
      viewport = *vp;
      if (viewport->hdr->min_scale <= scale_in_nm && viewport->hdr->max_scale > 0 ? viewport->hdr->max_scale > scale_in_nm : true)
        select_and_render_viewport(viewport, &wnd->transport_params.base, wnd->background_bitmap);
      }

    for (viewportp_t* vp = viewportps_begin(&wnd->obstacles_layers); vp != viewportps_end(&wnd->obstacles_layers); vp++)
      {
      viewport = *vp;
      if (viewport->hdr->min_scale <= scale_in_nm && viewport->hdr->max_scale > 0 ? viewport->hdr->max_scale > scale_in_nm : true)
        select_and_render_viewport(viewport, &wnd->obstacles_params.base, wnd->background_bitmap);
      }

    for (viewportp_t* vp = viewportps_begin(&wnd->navdata_layers); vp != viewportps_end(&wnd->navdata_layers); vp++)
      {
      viewport = *vp;
      if (viewport->hdr->min_scale <= scale_in_nm && viewport->hdr->max_scale > 0 ? viewport->hdr->max_scale > scale_in_nm : true)
        select_and_render_viewport(viewport, &wnd->navdata_params.base, wnd->background_bitmap);
      }

    /*
  extent_t ex;
  canvas_extents(wnd->background_bitmap, &ex);

  rect_t canvas_rect;
  rect_create_ex(0, 0, ex.dx, ex.dy, &canvas_rect);

  // TODO: this should be on the foreground layer
  canvas_extents(wnd->map_aircraft, &ex);
  rect_t icon_rect;
  rect_create_ex(0, 0, ex.dx, ex.dy, &icon_rect);

  point_t src_pt = { 0, 0 };

  rect_t map_rect;
  rect_create_ex(wnd->map_center.x - (ex.dx >> 1),
    wnd->map_center.y - (ex.dy >> 1),
    ex.dx, ex.dy, &map_rect);

  // draw the aircraft on the canvas
  bit_blt(wnd->background_bitmap, &canvas_rect, &map_rect, wnd->map_aircraft, &icon_rect, &src_pt, src_alpha_blend);
  */
  // todo: should be layers...
    wnd->render_background_complete = true;
    wnd->render_background_busy = false;

    post_message(wnd->hwnd, &request_paint, INDEFINITE_WAIT);
    }
  }

static void on_paint_background(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata)
  {
  map_widget_t* wnd = (map_widget_t*)wnddata;

  on_paint_widget_background(canvas, wnd_rect, msg, wnddata);

  extent_t ex;
  rect_extents(wnd_rect, &ex);

  // This takes 2 passes, if the background is complete then
  // the worker has rendered the background, otherwise
  // the params are set and the worker is asked to paint
  if (wnd->render_background_complete)
    {
    wnd->render_background_complete = false;

    extent_t ex;
    canvas_extents(wnd->background_bitmap, &ex);

    rect_t src_rect = { 0, 0, ex.dx, ex.dy };
    point_t top_left = { 0, 0 };

    // copy the background
    bit_blt(canvas, wnd_rect, wnd_rect, wnd->background_bitmap, &src_rect, &top_left, src_copy);
    }
  else if (!wnd->render_background_busy)
    {
    wnd->render_background_busy = true;

    // create the corners of the display, it could be rotated
    // in respect to the screen
    // TODO: cache these when the position/rotation changes
    // in gtopo30 1 pixel = 1km
    int32_t pixel_dist = ex.dx >> 1;

    pixel_rhombus_t screen_area =
      {
      .top_left.x = 0,
      .top_left.y = 0,

      .bottom_left.x = 0,
      .bottom_left.y = ex.dy,

      .top_right.x = ex.dx,
      .top_right.y = 0,

      .bottom_right.x = ex.dx,
      .bottom_right.y = ex.dy
      };


    switch (wnd->params.mode)
      {
      case mdm_north:
        break;
      case mdm_course:
        rotate_point(&wnd->map_center, wnd->course, &screen_area.top_left);
        rotate_point(&wnd->map_center, wnd->course, &screen_area.top_right);
        rotate_point(&wnd->map_center, wnd->course, &screen_area.bottom_left);
        rotate_point(&wnd->map_center, wnd->course, &screen_area.bottom_right);
        break;
      case mdm_track:
        rotate_point(&wnd->map_center, wnd->track, &screen_area.top_left);
        rotate_point(&wnd->map_center, wnd->track, &screen_area.top_right);
        rotate_point(&wnd->map_center, wnd->track, &screen_area.bottom_left);
        rotate_point(&wnd->map_center, wnd->track, &screen_area.bottom_right);
        break;
      }

    memcpy(&wnd->params.screen_pos, &screen_area, sizeof(pixel_rhombus_t));


    // 30 arc seconds == 1:1

    // the zoom sets the number of pixels per 30 arcsecond
    fixed_t degrees_per_pixel_y = make_fixed(0, wnd->params.scale / 1850, 0);
    // this calculates how many 
    degrees_per_pixel_y = idiv_fixed(degrees_per_pixel_y, ex.dx);

    fixed_t degrees_per_pixel_x = div_fixed(degrees_per_pixel_y, fixed_cos(fixed_to_int(wnd->params.ned_center.latlng.lng)));

    // the wnd->map_center pixel == wnd->position.pos
    // the distance of each pixel now sets the goespatial
    // positions
    // note: gdi->top < gdi->bottom while geo->top > gdi->bottom
    // hence lat calc upside down to gdi

    wnd->params.geo_pos.top_left.lng = sub_spatial_lng(wnd->params.ned_center.latlng.lat,
      imul_fixed(wnd->map_center.x - screen_area.top_left.x, degrees_per_pixel_x));

    wnd->params.geo_pos.top_left.lat = add_spatial_lat(wnd->params.ned_center.latlng.lng,
      imul_fixed(wnd->map_center.y - screen_area.top_left.y, degrees_per_pixel_y));

    wnd->params.geo_pos.top_right.lng = add_spatial_lng(wnd->params.ned_center.latlng.lat,
      imul_fixed(screen_area.top_right.x - wnd->map_center.x, degrees_per_pixel_x));

    wnd->params.geo_pos.top_right.lat = wnd->params.geo_pos.top_left.lat;

    wnd->params.geo_pos.bottom_left.lng = wnd->params.geo_pos.top_left.lng;

    wnd->params.geo_pos.bottom_left.lat = sub_spatial_lat(wnd->params.ned_center.latlng.lng,
      imul_fixed(screen_area.bottom_left.y - wnd->map_center.y, degrees_per_pixel_y));

    wnd->params.geo_pos.bottom_right.lng = wnd->params.geo_pos.top_right.lng;

    wnd->params.geo_pos.bottom_right.lat = wnd->params.geo_pos.bottom_left.lat;

#ifdef _WIN32
    float top_left_lng = fixed_to_float(wnd->params.geo_pos.top_left.lng);
    float top_left_lat = fixed_to_float(wnd->params.geo_pos.top_left.lat);

    float bottom_right_lng = fixed_to_float(wnd->params.geo_pos.bottom_right.lng);
    float bottom_right_lat = fixed_to_float(wnd->params.geo_pos.bottom_right.lat);

    float deg_x = fixed_to_float(degrees_per_pixel_x);
    float deg_y = fixed_to_float(degrees_per_pixel_y);
#endif

    // start the worker
    semaphore_signal(wnd->worker_start);
    }

  // else the worker is busy so no changes
  }

static void on_paint_foreground(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* _msg, void* wnddata)
  {
  map_widget_t* wnd = (map_widget_t*)wnddata;

  rectangle(canvas, wnd_rect, color_hollow, color_hollow, wnd_rect);

  point_t aircraft[] = {
    { wnd->map_center.x + 0, wnd->map_center.y - 15 },
    { wnd->map_center.x + 2, wnd->map_center.y - 14 },
    { wnd->map_center.x + 3, wnd->map_center.y - 10 },
    { wnd->map_center.x + 3, wnd->map_center.y - 8 },
    { wnd->map_center.x + 16, wnd->map_center.y - 8 },
    { wnd->map_center.x + 17, wnd->map_center.y - 7 },
    { wnd->map_center.x + 18, wnd->map_center.y - 5 },
    { wnd->map_center.x + 19, wnd->map_center.y - -1 },
    { wnd->map_center.x + 2, wnd->map_center.y - -1 },
    { wnd->map_center.x + 2, wnd->map_center.y - -8 },
    { wnd->map_center.x + 6, wnd->map_center.y - -9 },
    { wnd->map_center.x + 7, wnd->map_center.y - -10 },
    { wnd->map_center.x + 7, wnd->map_center.y - -13 },
    { wnd->map_center.x + 1, wnd->map_center.y - -13 },
    { wnd->map_center.x + 0, wnd->map_center.y - -15 },
    { wnd->map_center.x + -1, wnd->map_center.y - -13 },
    { wnd->map_center.x + -7, wnd->map_center.y - -13 },
    { wnd->map_center.x + -7, wnd->map_center.y - -10 },
    { wnd->map_center.x + -6, wnd->map_center.y - -9 },
    { wnd->map_center.x + -2, wnd->map_center.y - -8 },
    { wnd->map_center.x + -2, wnd->map_center.y - -1 },
    { wnd->map_center.x + -19, wnd->map_center.y - -1 },
    { wnd->map_center.x + -18, wnd->map_center.y - 5 },
    { wnd->map_center.x + -17, wnd->map_center.y - 7 },
    { wnd->map_center.x + -16, wnd->map_center.y - 8 },
    { wnd->map_center.x + -3, wnd->map_center.y - 8 },
    { wnd->map_center.x + -3, wnd->map_center.y - 10 },
    { wnd->map_center.x + -2, wnd->map_center.y - 14 },
    { wnd->map_center.x + -0, wnd->map_center.y - 15 },
    };

  polygon(canvas, wnd_rect, color_hollow, color_white, numelements(aircraft), aircraft);


  // based on the current scale, draw the range circle that is scale / 4
  extent_t ex;
  rect_extents(wnd_rect, &ex);

  int32_t range_dist = ex.dx >> 2;

  rect_t range_rect =
    {
    wnd->map_center.x - range_dist,
    wnd->map_center.y - range_dist,
    wnd->map_center.x + range_dist,
    wnd->map_center.y + range_dist
    };

  ellipse(canvas, wnd_rect, color_white, color_hollow, &range_rect);

  // TODO: map in km, miles
  // range at right of arc (maybe at right)
  char range[8];
  sprintf(range, "%d nm", wnd->params.scale / 7400);

  text_extent(wnd->params.font, 0, range, &ex);

  // TODO: when vertical fonts supported use other dims
  point_t txt_point =
    {
    range_rect.right - ex.dx,
    wnd->map_center.y
    };

  draw_text(canvas, wnd_rect, wnd->params.font, color_white, color_hollow, 0, range, &txt_point, wnd_rect, eto_vertical, 0);
  }


static result_t map_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  map_widget_t* wnd = (map_widget_t*)wnddata;

  bool changed = false;
  switch (get_can_id(msg))
    {
    case id_request_paint_background:
      invalidate_background_rect(hwnd, 0);
      break;
    case id_magnetic_heading:
    {
    int16_t direction;
    get_param_int16(msg, &direction);

    while (direction < 0)
      direction += 360;
    while (direction > 359)
      direction -= 360;

    changed = wnd->direction != direction;
    wnd->direction = direction;
    }
    break;
    case id_selected_course:
      {
      int16_t value;
      get_param_int16(msg, &value);
      changed = wnd->course != value;
      wnd->course = value;
      }
      break;
    case id_track:
      {
      int16_t value;
      get_param_int16(msg, &value);
      changed = wnd->track != value;
      wnd->track = value;
      }
      break;
    case id_touch_zoom:
      {
      // TODO: pass message as binary
      // withthe zoom center 
      int16_t value;
      get_param_int16(msg, &value);

      // todo: clamp??
      if ((value < 0 && wnd->params.scale > 1) ||
        (value > 0 && wnd->params.scale < 10000000))
        {
        wnd->params.scale += value * 1000;

        if (wnd->params.scale < 1)
          wnd->params.scale = 1;

        invalidate_background_rect(hwnd, 0);
        changed = true;
        }
      }
      break;
    case id_touch_pan:
      {
      uint32_t value;
      get_param_uint32(msg, &value);

      extent_t ex;
      ex.dx = (gdi_dim_t)(((int32_t)value) >> 16);
      ex.dy = (gdi_dim_t)((int16_t)(value & 0xffff));

      handle_t background_canvas;
      get_background_canvas(hwnd, &background_canvas);

      extent_t canvas_ex;
      canvas_extents(background_canvas, &canvas_ex);

      // calculate the offsets based on the scale
      fixed_t degrees_per_pixel = make_fixed(0, wnd->params.scale / 1850, 0);
      // this calculates how many 
      degrees_per_pixel = idiv_fixed(degrees_per_pixel, canvas_ex.dx);

      wnd->params.ned_center.latlng.lng = sub_fixed(wnd->params.ned_center.latlng.lng, mul_fixed(degrees_per_pixel, int_to_fixed(ex.dy)));
      wnd->params.ned_center.latlng.lat = add_fixed(wnd->params.ned_center.latlng.lat, mul_fixed(degrees_per_pixel, int_to_fixed(ex.dx)));

      invalidate_background_rect(hwnd, 0);
      changed = true;
      }
      break;
    }

  if (changed)
    invalidate_foreground_rect(hwnd, 0);

  // pass to default
  return widget_wndproc(hwnd, msg, wnddata);
  }

result_t create_map_widget(handle_t parent, uint16_t id, aircraft_t *aircraft, map_widget_t* wnd, handle_t* out)
  {
  result_t result;
  if (failed(result = create_widget(parent, id, map_wndproc, &wnd->base, &wnd->hwnd)))
    return result;

  extent_t background_ex = { rect_width(&wnd->base.rect), rect_height(&wnd->base.rect) };

  if (failed(result = canvas_create(&background_ex, &wnd->background_bitmap)))
    return result;

  wnd->aircraft = aircraft;
  wnd->base.on_paint_background = on_paint_background;
  wnd->base.on_paint_foreground = on_paint_foreground;

  if (failed(result = semaphore_create(&wnd->worker_start)))
    return result;

  if (failed(result = task_create(0, DEFAULT_STACK_SIZE, paint_worker, wnd, BELOW_NORMAL, &wnd->background_worker)))
    return result;

  if (out != 0)
    *out = wnd->hwnd;

  return s_ok;

  }

#ifndef _MAX_DIR
#define _MAX_DIR 256
#endif

result_t load_layers(map_widget_t* wnd, const char* path)
  {
  result_t result;

  uint32_t num_containers;

  if (failed(result = open_atom_db(path, &num_containers, &wnd->atom_db)))
    return result;

  for (uint32_t i = 0; i < num_containers; i++)
    {
    const db_header_t* hdr;
    handle_t container;

    if (failed(result = open_container(wnd->atom_db, i, &container, &hdr)))
      return result;

    wnd->terrain_params.base.map = &wnd->params;
    wnd->contours_params.base.map = &wnd->params;
    wnd->coastlines_params.base.map = &wnd->params;
    wnd->landareas_params.base.map = &wnd->params;
    wnd->cities_params.base.map = &wnd->params;
    wnd->water_params.base.map = &wnd->params;
    wnd->transport_params.base.map = &wnd->params;
    wnd->obstacles_params.base.map = &wnd->params;
    wnd->navdata_params.base.map = &wnd->params;

    viewport_t *vp;

    // TODO: if contours exist then the landmass layer is not used.
    // but this could be dependendent on the position???
    switch (hdr->content_type)
      {
      case ct_terrain:
        create_terrain_viewport(container, hdr, &wnd->terrain_layer);
        break;
      case ct_contours:
        create_contours_viewport(container, hdr, &vp);
        viewportps_push_back(&wnd->contours_layers, &vp);
        break;
      case ct_landarea :
        create_land_viewport(container, hdr, &vp);
        viewportps_push_back(&wnd->landareas_layers, &vp);
        break;
      case ct_coastlines:
        create_coastlines_viewport(container, hdr, &vp);
        viewportps_push_back(&wnd->coastlines_layers, &vp);
        break;
      case ct_cities:
        create_cities_viewport(container, hdr, &vp);
        viewportps_push_back(&wnd->cities_layers, &vp);
        break;
      case ct_surface_water:
        create_surface_water_viewport(container, hdr, &vp);
        viewportps_push_back(&wnd->water_layers, &vp);
        break;
      case ct_transport:
        create_transport_viewport(container, hdr, &vp);
        viewportps_push_back(&wnd->transport_layers, &vp);
        break;
      case ct_obstacles:
        create_obstacles_viewport(container, hdr, &vp);
        viewportps_push_back(&wnd->obstacles_layers, &vp);
        break;
      case ct_airspace:
        create_airspace_viewport(container, hdr, &vp);
        viewportps_push_back(&wnd->navdata_layers, &vp);
        break;
      }
    }

  if (result == e_no_more_information)
    return s_ok;

  return result;
  }


