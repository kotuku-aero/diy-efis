#include "atomdb_viewport.h"


static result_t close(viewport_t* handle)
  {
  atomdb_viewport_t* viewport = (atomdb_viewport_t*)handle;

  for (gdi_polyline_t* it = gdi_polylines_begin(&viewport->gdi_polylines); it != gdi_polylines_end(&viewport->gdi_polylines); it++)
    {
    points_close(&it->gdi_points);
    int16_ts_close(&it->gdi_elevations);
    }

  close_handle(viewport->base.container);

  return neutron_free(handle);
  }

static void incarnate_gdi_polyline(gdi_polyline_t* it)
  {
  memset(&it->gdi_points, 0, sizeof(points_t));
  memset(&it->gdi_elevations, 0, sizeof(int16_ts_t));
  }

static result_t select(viewport_t* vp, viewport_params_t* params, handle_t canvas)
  {
  result_t result;
  atomdb_viewport_t* viewport = (atomdb_viewport_t*)vp;

  // clear the memory that will be used
  spatial_entity_ids_clear(&viewport->entities);

  const uint32_t entity_types[] = { SPATIAL_POLYGON, SPATIAL_POLYLINE, SPATIAL_POINT };

  //trace_debug("Render coastlines for (%d,%d), (%d,%d), (%d, %d), (%d, %d)\n",
  //  fixed_to_int(params->base.map->geo_pos.top_left.lat),
  //  fixed_to_int(params->base.map->geo_pos.top_left.lng),
  //  fixed_to_int(params->base.map->geo_pos.top_right.lat),
  //  fixed_to_int(params->base.map->geo_pos.top_right.lng),
  //  fixed_to_int(params->base.map->geo_pos.bottom_left.lat),
  //  fixed_to_int(params->base.map->geo_pos.bottom_left.lng),
  //  fixed_to_int(params->base.map->geo_pos.bottom_right.lat),
  //  fixed_to_int(params->base.map->geo_pos.bottom_right.lng));

  // Build a list of spatial entities that exist within the spatial rhombus
  if (failed(result = select_spatial_entities(vp->container, &params->map->geo_pos, numelements(entity_types), entity_types, &viewport->entities)))
    return result;

  // calculate the distance per pixel for the goecoordinates.

  // the map scale is NM for the width of the screen, divide the
  // width by 60 to get the degrees scale

  fixed_t map_scale_in_nm = int_to_fixed(params->map->scale / 1850);

  extent_t canvas_ex;
  canvas_extents(canvas, &canvas_ex);
  rect_t canvas_rect = { 0, 0, canvas_ex.dx, canvas_ex.dy };

  // todo: this should be a matrix to allow for rotation of the viewport below
  params->geo_scale_y = div_fixed(int_to_fixed(canvas_ex.dx), div_fixed(map_scale_in_nm, int_to_fixed(60)));
  params->geo_scale_x = mul_fixed(params->geo_scale_y, fixed_cos(fixed_to_int(params->map->ned_center.latlng.lng)));

#ifdef DEBUG_POLYGONS
  trace_debug("-- Render polygons ---\nThere are %d polygons\n", polylines_count(&viewport->clipped_contours));
#endif
  // this section flattens the spatial polygons into a list of GDI points
  // in a format needed for the polypolygon routine
  for (spatial_entity_id_t* entity = spatial_entity_ids_begin(&viewport->entities); entity != spatial_entity_ids_end(&viewport->entities); entity++)
    {
    polylines_clear(&viewport->clipped_contours);

    // return the polygons
    if (failed(result = load_metafile(vp->container, entity, &viewport->clipped_contours)))
      return result;

    // at this point we have a rombus that has closed polygons in it.
    // however all of the points now need to be rotated by the 'UP' vector
    //
    // for example if the aircraft is tracking to the east (90 degrees)
    // the map must be rotated by -90 (270 actual) to make the
    // track up screen work.
    switch (params->map->mode)
      {
      case mdm_north:
        // no need to rotate the clipped contours
        // stretch_copy(viewport, &canvas_rect, params, canvas, &params->base.map->screen_pos, &params->base.map->geo_pos);
        break;
      case mdm_track:
      case mdm_course:
        rotate_contours(&params->map->ned_center, params->map->rotation, &viewport->clipped_contours);
        break;
      }

    // clear the cache
    for (gdi_polyline_t* it = gdi_polylines_begin(&viewport->gdi_polylines); it != gdi_polylines_end(&viewport->gdi_polylines); it++)
      {
      points_clear(&it->gdi_points);
      int16_ts_clear(&it->gdi_elevations);
      }

    gdi_polylines_clear(&viewport->gdi_polylines);

    for (polyline_t* it = polylines_begin(&viewport->clipped_contours); it != polylines_end(&viewport->clipped_contours); it++)
      {
      // expand the cache to get 1 more polyline
      gdi_polylines_expand(&viewport->gdi_polylines, 1, incarnate_gdi_polyline);

      gdi_polyline_t *polyline = &gdi_polylines_begin(&viewport->gdi_polylines)[gdi_polylines_count(&viewport->gdi_polylines)-1];

      for (spatial_point_t* pt = spatial_points_begin(it); pt != spatial_points_end(it); pt++)
        {
        point_t gdi_pt;
        to_screen_coordinates(params, &params->map->geo_pos.top_left, pt, &gdi_pt);

        // only add the point if it is not the same as the previous point.
        // at large zoom scales the points will coalesce into 1 point
        if (points_count(&polyline->gdi_points) == 0 ||
          !point_equals(&points_begin(&polyline->gdi_points)[points_count(&polyline->gdi_points) - 1], &gdi_pt))
          {
          points_push_back(&polyline->gdi_points, &gdi_pt);
          int16_ts_push_back(&polyline->gdi_elevations, pt->elevation);
          }
        }

      switch (entity->type & SPATIAL_TYPE_MASK)
        {
        case SPATIAL_POLYGON:
          if (viewport->render_polygon != nullptr)
            (*viewport->render_polygon)(viewport, entity, canvas, &canvas_rect, params);
          break;
        case SPATIAL_POLYLINE:
          if (viewport->render_polyline != nullptr)
            (*viewport->render_polyline)(viewport, entity, canvas, &canvas_rect, params);
          break;
        case SPATIAL_POINT:
          if (viewport->render_point != nullptr)
            (*viewport->render_point)(viewport, entity, canvas, &canvas_rect, params);
          break;
        }
      }
    }

  return s_ok;
  }

result_t create_atomdb_viewport(handle_t container, const db_header_t* hdr, atomdb_viewport_t* viewport)
  {
  viewport->base.select = select;
  viewport->base.close = close;
  viewport->base.container = container;

  return s_ok;
  }