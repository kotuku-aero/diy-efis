#ifndef __atomdb_viewport_h__
#define __atomdb_viewport_h__

#include "viewport.h"

#include "layer.h"

#include "../atomdb/db.h"
#include "../graviton/schema.h"

// #define DEBUG_POLYGONS

type_vector_t(uint32);
type_vector_t(point);
vector_t(int16_t);

typedef struct _gdi_polyline_t {
  points_t gdi_points;              // used to reuse mem allocated drawing polygons/points
  int16_ts_t gdi_elevations;        // optional elevations of the gdi points
  } gdi_polyline_t;

type_vector_t(gdi_polyline);

typedef struct _atomdb_viewport_t atomdb_viewport_t;
/**
 * @brief Render the clipped polygon
 * @param canvas            Canvas to draw on
 * @param clip_rect         clipping rectange
 * @param params            Viewport parameters
 * @return s_ok when rendered
*/
typedef result_t (*render_polygon_fn)(atomdb_viewport_t *viewport, const spatial_entity_id_t *id, handle_t canvas, const rect_t *clip_rect, viewport_params_t* params);
/**
 * @brief Render clipped polylines
 * @param canvas            Canvas to draw on
 * @param clip_rect         clipping rectange
 * @param params            Viewport parameters
 * @return s_ok when rendered
*/
typedef result_t (*render_polyline_fn)(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params);
/**
 * @brief Render clipped points
 * @param canvas            Canvas to draw on
 * @param clip_rect         clipping rectange
 * @param params            Viewport parameters
 * @return
*/
typedef result_t (*render_points_fn)(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params);
/**
 * @brief 
 * @param viewport 
 * @param id 
 * @param canvas 
 * @param clip_rect 
 * @param params 
 * @param pt 
 * @return 
*/
typedef result_t (*select_entity_fn)(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params, const point_t *pt);


typedef struct _atomdb_viewport_t {
  viewport_t base;

  spatial_entity_ids_t entities;    // holds the selected index ID's
  polylines_t clipped_contours;     // cached graphics

  spatial_entity_hdr_t *selected_entity;

  // these are all of the polypolygons to be rendered
  gdi_polylines_t gdi_polylines;      // cached points to render

  render_polygon_fn render_polygon;   // callback to render selected polygon
  render_polyline_fn render_polyline; // callback to render selected polyline
  render_points_fn render_point;   // callback to render selected pointsets
  select_entity_fn select_entity;     // callback to select an entity
  } atomdb_viewport_t;


extern result_t create_atomdb_viewport(handle_t container, const db_header_t* hdr, atomdb_viewport_t* vp);

extern void on_render_ground_cover(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params);
extern void on_render_name_line(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params);
extern void on_render_power_line(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params);
extern void on_render_railway_line(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params);
extern void on_render_river(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params);
extern void on_render_road(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params);
extern void on_render_sand(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params);
extern void on_render_terrain(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params);
extern void on_render_trees(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params);
extern void on_render_airfield(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params);
extern void on_render_airspace(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params);
extern void on_render_airway(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params);
extern void on_render_instrument_reporting_point(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params);
extern void on_render_visual_reporting_point(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params);
extern void on_render_vfr_approach(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params);
extern void on_render_ifr_approach(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params);
extern void on_render_ifr_departure(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params);
extern void on_render_obstruction(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params);
extern void on_render_spot_entity(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params);

extern result_t on_select_aerodrome(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params, const point_t* pt);
extern result_t on_select_airspace(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params, const point_t* pt);
extern result_t on_select_airway(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params, const point_t* pt);
extern result_t on_select_instrument_reporting_point(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params, const point_t* pt);
extern result_t on_select_visual_reporting_point(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params, const point_t* pt);
extern result_t on_select_vfr_approach(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params, const point_t* pt);
extern result_t on_select_ifr_approach(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params, const point_t* pt);
extern result_t on_select_ifr_departure(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params, const point_t* pt);
extern result_t on_select_obstruction(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params, const point_t* pt);
extern result_t on_select_spot_entity(atomdb_viewport_t *viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params, const point_t* pt);

#endif
