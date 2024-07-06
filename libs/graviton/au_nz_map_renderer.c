#include <stdio.h>
#include <stdarg.h>

#include "../graviton/atomdb_viewport.h"
//#include "../proton/proton.h"

// only compile if AU, NZ locale
#if defined(EN_NZ) | defined(EN_AU)

#define GRASS_RUNWAY_COLOR RGB(186,221,105)
#define BITUM_RUNWAY_COLOR RGB(130,132,137)

static const char* fiscom_str = "FISCOM";
static const char* line_str = "-";        // special use.
static const char* class_fmt = "%s LL %s TM";
static const char* class_names[] = {
  "",   // act_unknown,
  "A",  // act_A,
  "B",  // act_B,
  "C",  // act_C,
  "D",  // act_D,
  "E",  // act_E,
  "F",  // act_F,
  "G",  // act_G,
  };
static const char* info_freq_fmt = "%s %s";

#define LABEL_LINE_LEN 32

static result_t format_altitude(int32_t altitude, char** out)
  {
  if (altitude == -9999 || altitude == 0)   // surface
    return neutron_strdup("SFC", 0, out);

  result_t result;
  char* str;
  if (failed(result = neutron_malloc(32, &str)))
    return result;

  if (altitude > 10500)
    {
    int32_t fl = altitude / 1000;
    sprintf(str, "FL%d", fl);
    }
  else
    sprintf(str, "%d", altitude);

  *out = str;
  return s_ok;
  }

type_vector_t(extent);

// this holds a cached airspace so the expensive
// string formatting is not lost
typedef struct _cached_entity_details_t {
  spatial_entity_hdr_t* entity;
  uint32_t id;                  // offset into the DB
  extent_t indentifier_extents;
  extent_t name_extents;
  char* name_str;
  extent_t upper_limit_extents;
  char* upper_limit_str;
  extent_t lower_limit_extents;
  char* lower_limit_str;
  extent_t notes_extents;
  char* notes_str;
  extent_t csabbrev_extents;
  char* csabbrev_str;
  extent_t frequency_extents;
  char* frequency_str;
  extent_t times_extents;
  char* times_str;
  extent_t label_line1_extents;
  char* label_line1_str;
  extent_t label_line2_extents;
  char* label_line2_str;
  extent_t runway_end1_extents;
  char* runway_end1_str;
  extent_t runway_end2_extents;
  char* runway_end2_str;
  // used by the renderers to cache static string extents
  extents_t cached_extents;
  } cached_airspace_details_t;

typedef cached_airspace_details_t* airspace_detail_p;

vector_t(airspace_detail_p);

static airspace_detail_ps_t airspace_cache;

static result_t release_info_panel(handle_t db, cached_airspace_details_t* details)
  {
  neutron_free(details->name_str);
  neutron_free(details->upper_limit_str);
  neutron_free(details->lower_limit_str);
  neutron_free(details->notes_str);
  neutron_free(details->csabbrev_str);
  neutron_free(details->frequency_str);
  neutron_free(details->times_str);
  neutron_free(details->label_line1_str);
  neutron_free(details->label_line2_str);
  neutron_free(details->runway_end1_str);
  neutron_free(details->runway_end2_str);
  extents_close(&details->cached_extents);
  etherealize_spatial_entity(db, details->entity);

  return neutron_free(details);
  }

result_t purge_cache(spatial_entity_ids_t* new_ids)
  {
  // release all entities not in the newly selected list
  return e_not_implemented;
  }

static result_t get_entity_details(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, const font_t* font, cached_airspace_details_t** out)
  {
  // see if the spatial_entity exists in the cache
  for (airspace_detail_p* it = airspace_detail_ps_begin(&airspace_cache); it != airspace_detail_ps_end(&airspace_cache); it++)
    {
    if (id->offset == (*it)->id)
      {
      *out = *it;
      return s_ok;
      }
    }

  result_t result;
  spatial_entity_hdr_t* entity = 0;
  if (failed(result = incarnate_spatial_entity(viewport->base.container, id, &entity)))
    return result;

  // the entity is not found, load it and cache t
  cached_airspace_details_t* details;
  if (failed(result = neutron_calloc(1, sizeof(cached_airspace_details_t), (void**)&details)))
    return result;

  details->entity = entity;
  details->id = id->offset;

  airspace_detail_ps_push_back(&airspace_cache, details);
  *out = details;

  return s_ok;
  }

static inline airspace_t* as_airspace(spatial_entity_hdr_t* entity) { return (airspace_t*)entity; }
static inline runway_t* as_runway(spatial_entity_hdr_t* entity) { return (runway_t*)entity; }

static const char* get_string(atomdb_viewport_t* viewport, const font_t* font, const spatial_string_t* ref, char** str, extent_t* ex)
  {
  if (succeeded(resolve_string(viewport->base.container, ref, str)))
    text_extent(font, 0, *str, ex);
  else
    {
    *str = nullptr;
    ex->dx = 0;
    ex->dy = 0;

    return 0;
    }

  return *str;
  }

/**
 * @brief Get the name string from the airspace
 * @param viewport  database to use
 * @param font      font to calculate size from
 * @param details   cached details
 * @return The name, 0 if no name
*/
static inline const char* get_name(atomdb_viewport_t* viewport, const font_t* font, cached_airspace_details_t* details)
  {
  return get_string(viewport, font, &as_airspace(details->entity)->base.name, &details->name_str, &details->name_extents);
  }
/**
 * @brief Get the notes for the airspace
 * @param viewport  database to use
 * @param font      font to calculate size from
 * @param details   cached details
 * @return The notes, 0 if no notes
*/
static inline const char* get_notes(atomdb_viewport_t* viewport, const font_t* font, cached_airspace_details_t* details)
  {
  return get_string(viewport, font, &as_airspace(details->entity)->notes, &details->notes_str, &details->notes_extents);
  }
/**
 * @brief Get the formatted lower limits
 * @param viewport  database to use
 * @param font      font to use
 * @param details   cached details
 * @return formatted string
*/
static const char* get_lower_alt(atomdb_viewport_t* viewport, const font_t* font, cached_airspace_details_t* details)
  {
  if (details->lower_limit_str == 0)
    {
    if (succeeded(format_altitude(as_airspace(details->entity)->lower_limit, &details->lower_limit_str)))
      text_extent(font, 0, details->lower_limit_str, &details->lower_limit_extents);
    else
      {
      details->lower_limit_extents.dx = 0;
      details->lower_limit_extents.dy = 0;
      }
    }

  return details->lower_limit_str;
  }
/**
 * @brief Get the formatted lower limits
 * @param viewport  database to use
 * @param font      font to use
 * @param details   cached details
 * @return formatted string
*/
static const char* get_upper_alt(atomdb_viewport_t* viewport, const font_t* font, cached_airspace_details_t* details)
  {
  if (details->upper_limit_str == 0)
    {
    if (succeeded(format_altitude(as_airspace(details->entity)->upper_limit, &details->upper_limit_str)))
      text_extent(font, 0, details->upper_limit_str, &details->upper_limit_extents);
    else
      {
      details->upper_limit_extents.dx = 0;
      details->upper_limit_extents.dy = 0;
      }
    }

  return details->lower_limit_str;
  }
/**
 * @brief Get the csabbrev string from the airspace
 * @param viewport  database to use
 * @param font      font to calculate size from
 * @param details   cached details
 * @return The csabbrev, 0 if no csabbrev
*/
static inline const char* get_csabbrev(atomdb_viewport_t* viewport, const font_t* font, cached_airspace_details_t* details)
  {
  return get_string(viewport, font, &as_airspace(details->entity)->csabbrev, &details->csabbrev_str, &details->csabbrev_extents);
  }
/**
 * @brief Get the frequency string from the airspace
 * @param viewport  database to use
 * @param font      font to calculate size from
 * @param details   cached details
 * @return The frequency, 0 if no frequency
*/
static inline const char* get_frequency(atomdb_viewport_t* viewport, const font_t* font, cached_airspace_details_t* details)
  {
  return get_string(viewport, font, &as_airspace(details->entity)->frequency, &details->frequency_str, &details->frequency_extents);
  }

static const char* everyday_str = "H24";
static const char* notam_str = "NOTAM";
static const char* day_str = "DAY";
/**
 * @brief Get the cached times display string
 * @param viewport  database to use
 * @param font      font to calculate size from
 * @param details   cached details
 * @return The string, 0 if no details
*/
static inline const char* get_times(atomdb_viewport_t* viewport, const font_t* font, cached_airspace_details_t* details)
  {
  return get_string(viewport, font, &as_airspace(details->entity)->airspace_active, &details->times_str, &details->times_extents);
  }

static void incarnate_extent(extent_t* ex)
  {
  ex->dx = 0;
  ex->dy = 0;
  }

static void draw_label(handle_t canvas, const rect_t* clip_rect, const font_t* font,
  color_t fg, color_t bg, point_t* pt, ...)
  {
  va_list ap;

  const char* str;
  const extent_t* ex;
  gdi_dim_t height_hint = 0;
  va_start(ap, pt);

  // first step is to calculate the height of the label
  for (str = va_arg(ap, const char*); str != 0; str = va_arg(ap, const char*))
    {
    ex = va_arg(ap, const extent_t*);
    if (ex != 0)
      height_hint += ex->dy;
    else
      height_hint += 5;
    }


  gdi_dim_t offset = pt->x;
  pt->y -= height_hint >> 1;

  va_start(ap, pt);
  for (str = va_arg(ap, const char*); str != 0; str = va_arg(ap, const char*))
    {
    ex = va_arg(ap, const extent_t*);
    if (str == line_str)
      {
      // draw a line the width of the longest strint
      pt->x = offset - (ex->dx >> 1);
      pt->y += 2;
      point_t pt2[2] =
        {
        { pt->x, pt->y },
        { pt->x + ex->dx, pt->y }
        };

      polyline(canvas, clip_rect, fg, 2, pt2);

      pt->y += 3;
      }
    else
      {
      pt->x = offset - (ex->dx >> 1);
      draw_text(canvas, clip_rect, font, fg, bg, 0, str, pt, clip_rect, 0, 0);

      pt->y += ex->dy;     // next line
      }

    }
  }

static void draw_class_info_panel(atomdb_viewport_t* viewport, airspace_params_t* params, handle_t canvas, const rect_t* clip_rect, color_t pen, cached_airspace_details_t* details)
  {
  if (params->base.map->scale <= params->info_panel_zoom)
    {
    point_t pt;
    // calculate where the label will draw
    to_screen_coordinates(&params->base, &params->base.map->geo_pos.top_left, &as_airspace(details->entity)->base.base.center_pt, &pt);

    if (rect_contains(clip_rect, &pt))
      {
      const font_t* font = params->base.map->font;
      // drawn as:
      // <class> LL <lower_alt> {TM}
      // <notes>
      if (details->label_line1_str == 0)
        {
        // allocate temp strings
        neutron_malloc(LABEL_LINE_LEN, (void**)&details->label_line1_str);
        neutron_malloc(LABEL_LINE_LEN, (void**)&details->label_line2_str);

        snprintf(details->label_line1_str, LABEL_LINE_LEN, class_fmt,
          class_names[as_airspace(details->entity)->classification],
          get_lower_alt(viewport, font, details));
        text_extent(font, 0, details->label_line1_str, &details->label_line1_extents);

        snprintf(details->label_line2_str, LABEL_LINE_LEN, info_freq_fmt,
          get_csabbrev(viewport, font, details),
          get_frequency(viewport, font, details));
        text_extent(font, 0, details->label_line2_str, &details->label_line2_extents);
        }

      draw_label(canvas, clip_rect, font,
        pen, color_hollow, &pt,
        details->label_line1_str, &details->label_line1_extents,
        details->label_line2_str, &details->label_line2_extents,
        0, 0);
      }
    }
  }

static void draw_airspace_info_panel(atomdb_viewport_t* viewport, airspace_params_t* params, handle_t canvas, const rect_t* clip_rect, color_t pen, cached_airspace_details_t* details)
  {
  if (params->base.map->scale <= params->info_panel_zoom)
    {
    point_t pt;
    // calculate where the label will draw
    to_screen_coordinates(&params->base, &params->base.map->geo_pos.top_left, &as_airspace(details->entity)->base.base.center_pt, &pt);

    if (rect_contains(clip_rect, &pt))
      {
      // drawn as (full detail)
      //    
      //  <designator>
      //    <name>
      //  <upper limit>
      //  -------------
      //  <lower limit>
      //    <times>
      // 
      // compressed detail
      //  <designator>
      //  <upper limit>
      //  -------------
      //  <lower limit>

      if (details->indentifier_extents.dx == 0 && details->indentifier_extents.dy == 0)
        {
        // create / load the cached strings
        text_extent(params->base.map->font, 0, as_airspace(details->entity)->base.identifier, &details->indentifier_extents);
        get_name(viewport, params->base.map->font, details);
        get_lower_alt(viewport, params->base.map->font, details);
        get_upper_alt(viewport, params->base.map->font, details);
        get_times(viewport, params->base.map->font, details);
        }

      if (params->base.map->scale <= params->detail_info_panel_zoom)
        // draws at airspace center 
        draw_label(canvas, clip_rect, params->base.map->font,
          pen, color_hollow, &pt,
          as_airspace(details->entity)->base.identifier, &details->indentifier_extents,
          details->name_str, &details->name_extents,
          details->upper_limit_str, &details->upper_limit_extents,
          line_str, &details->upper_limit_extents,
          details->lower_limit_str, &details->lower_limit_extents,
          details->times_str, &details->times_extents,
          0, 0);
      else
        draw_label(canvas, clip_rect, params->base.map->font,
          pen, color_hollow, &pt,
          as_airspace(details->entity)->base.identifier, &details->indentifier_extents,
          details->upper_limit_str, &details->upper_limit_extents,
          line_str, &details->upper_limit_extents,
          details->lower_limit_str, &details->lower_limit_extents,
          0, 0);

      }
    }
  }

void on_render_ground_cover(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params)
  {
  //ground_cover_t* entity = (ground_cover_t*)obj;
  // only renders a polygon


  }

void on_render_name_line(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params)
  {
  //name_line_t* entity = (name_line_t*)obj;
  }

void on_render_power_line(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params)
  {
  //power_line_t* entity = (power_line_t*)obj;
  }

void on_render_railway_line(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params)
  {
  //railway_t* entity = (railway_t*)obj;
  }

void on_render_river(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params)
  {
  //river_t* river = (river_t*)obj;
  }

void on_render_road(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params)
  {
  //road_t* road = (road_t*)obj;
  }

void on_render_sand(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params)
  {
  //sand_t* entity = (sand_t*)obj;
  }

void on_render_terrain(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params)
  {
  //terrain_t* entity = (terrain_t*)obj;
  }

void on_render_trees(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params)
  {
  //trees_t* entity = (trees_t*)obj;
  }

result_t on_select_aerodrome(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params, const point_t* pt)
  {
  return s_false;
  }

void on_render_airfield(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params)
  {

  //point_t pt;
  //// calculate where the label will draw
  //to_screen_coordinates(params, &params->base.map->geo_pos.top_left, &details->airspace->base.base.center_pt, &pt);

  //rect_t airfield;
  //rect_create(pt.x - 4, pt.y - 4, pt.x + 3, pt.y + 3, &airfield);
  //ellipse(canvas, clip_rect, pen, color_white, &airfield);

  }

result_t on_select_airspace(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params, const point_t* pt)
  {
  return s_false;
  }

void on_render_airspace(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* _params)
  {
  airspace_params_t* params = (airspace_params_t*)_params;

  const map_theme_t* theme = params->base.map->is_night ? &params->base.map->night_theme : &params->base.map->day_theme;

  cached_airspace_details_t* details;
  if (succeeded(get_entity_details(viewport, id, params->base.map->font, &details)))
    {
    point_t pt;
    // calculate where the label will draw
    to_screen_coordinates(&params->base, &params->base.map->geo_pos.top_left, &as_airspace(details->entity)->base.base.center_pt, &pt);

    // TODO: add graphics pens....
    color_t pen = color_hollow;
    color_t fill = color_hollow;

    e_entity_type type = id->type & PAYLOAD_TYPE_MASK;
    e_controlled_airspace_class as_class = (e_controlled_airspace_class)id->type >> 24;

    bool draw_polyline = false;
    bool draw_polygon = false;
    // decide what type of airspace to render
    switch (type)
      {
      case ast_fiscom:
        break;
      case ast_volcanic_hazard:
      case ast_danger_area:
        pen = theme->danger_area;
        draw_polyline = true;
        break;
      case ast_restricted_area:
        pen = theme->restricted_area;
        draw_polyline = true;
        break;
      case ast_general_aviation:
      case ast_mandatory_broadcast_zone:
      case ast_common_frequency_zone:
        pen = theme->class_g_airspace;
        draw_polyline = true;
        break;
      case ast_cta:
        pen = theme->class_c_airspace;
        draw_polyline = true;
        break;
      case ast_ctr:
        switch (as_class)
          {
          case act_A:
            pen = theme->class_a_airspace;
            break;
          case act_B:
            pen = theme->class_b_airspace;
            break;
          case act_C:
            pen = theme->class_c_airspace;
            break;
          case act_D:
            pen = theme->class_d_airspace;
            break;
          case act_E:
            pen = theme->class_e_airspace;
            break;
          case act_F:
            pen = theme->class_f_airspace;
            break;
          case act_G:
          case act_unknown:
            pen = theme->class_g_airspace;
            break;
          }
        draw_polyline = true;
        break;
      case ast_millitary_zone:
        pen = theme->class_m_airspace;
        draw_polyline = true;
        break;
      case ast_aerodrome:
      case ast_heliport:
      case ast_visual_reporting_point:
        break;
      case ast_runway:
        if (as_runway(details->entity)->surface_type == rs_grass)
          {
          pen = theme->runway_grass_pen;
          fill = theme->runway_grass_color;
          }
        else
          {
          pen = theme->runway_pen;
          fill = theme->runway_color;
          }
        draw_polygon = true;
        break;
      }

    // this is to draw detail, perhaps not here
    // depends on zoom etc

    // this is the case when there is a polyline only, or a segment of a polyline
    // there can be many polylines depending on the entity type, this only draws the first
    if (draw_polygon || draw_polyline)
      {
      for (gdi_polyline_t* poly = gdi_polylines_begin(&viewport->gdi_polylines); poly != gdi_polylines_end(&viewport->gdi_polylines); poly++)
        {
        if (draw_polyline)
          polyline(canvas, clip_rect, pen, points_count(&poly->gdi_points), points_begin(&poly->gdi_points));
        else if (draw_polygon)
          polygon(canvas, clip_rect, pen, fill, points_count(&poly->gdi_points), points_begin(&poly->gdi_points));
        }
      }

    // The text is drawn at the center pt of the airspace
    switch (type)
      {
      case ast_fiscom:
        // this is 2 lines high as in
        //    FISCOM        <-- static text
        // CH INFO 129.8    <-- detail notes

        // cache the size of the extents first
        if (extents_count(&details->cached_extents) == 0)
          {
          extents_expand(&details->cached_extents, 1, incarnate_extent);
          text_extent(params->base.map->font, 0, fiscom_str, extents_begin(&details->cached_extents));

          // incarnate the csabbrev
          const char* csabbrev = get_csabbrev(viewport, params->base.map->font, details);
          // incarnate the frequency
          const char* frequency = get_frequency(viewport, params->base.map->font, details);

          size_t notes_len = as_airspace(details->entity)->csabbrev.length + 2 + as_airspace(details->entity)->frequency.length;
          // allocate a notes string that is made from the csname and frequency
          neutron_malloc(notes_len, (void**)&details->notes_str);
          snprintf(details->notes_str, notes_len, "%s %s", csabbrev, frequency);
          text_extent(params->base.map->font, 0, details->notes_str, &details->notes_extents);
          }

        if (rect_contains(clip_rect, &pt))
          // draws at airspace center 
          draw_label(canvas, clip_rect, params->base.map->font,
            pen, color_hollow, &pt,
            "FISCOM", extents_begin(&details->cached_extents),
            details->notes_str, &details->notes_extents,
            0, 0);
        break;
      case ast_volcanic_hazard:
      case ast_danger_area:
      case ast_restricted_area:
        draw_airspace_info_panel(viewport, params, canvas, clip_rect, pen, details);
        break;
      case ast_general_aviation:
      case ast_mandatory_broadcast_zone:
      case ast_common_frequency_zone:
        pen = theme->class_g_airspace;
        break;
      case ast_ctr:
        // drawn as
        // <class> LL <lower_limit> {TM}
        // <notes (freq)>
        draw_class_info_panel(viewport, params, canvas, clip_rect, pen, details);
        break;
      case ast_cta:
        break;
      case ast_millitary_zone:
        pen = theme->class_m_airspace;
        draw_airspace_info_panel(viewport, params, canvas, clip_rect, pen, details);
        break;
      case ast_aerodrome:
      case ast_heliport:
      case ast_visual_reporting_point:
        break;
      case ast_runway:
        // draw polygon1 if is extended runway
        if (gdi_polylines_count(&viewport->gdi_polylines) > 1)
          {
          if (as_runway(details->entity)->surface_type == rs_grass)
            fill = theme->runway_grass_ext_color;
          else
            fill = theme->runway_ext_color;

          gdi_polyline_t* poly1 = gdi_polylines_begin(&viewport->gdi_polylines) + 1;

          polygon(canvas, clip_rect, pen, fill, points_count(&poly1->gdi_points), points_begin(&poly1->gdi_points));
          }

        if (as_runway(details->entity)->designator_end1.length > 0)
          {
          to_screen_coordinates(&params->base, &params->base.map->geo_pos.top_left, &as_runway(details->entity)->designator_end1_position, &pt);

          if (details->runway_end1_str == 0)
            get_string(viewport, params->base.map->font, &as_runway(details->entity)->designator_end1, &details->runway_end1_str, &details->runway_end1_extents);

          if (as_runway(details->entity)->right_hand_circuits)
            {
            // draw inbound arrow
            }

          // TODO: rotate text
          pt.x -= details->runway_end1_extents.dx >> 1;
          draw_text(canvas, clip_rect, params->base.map->font, pen, color_hollow,
            as_runway(details->entity)->designator_end1.length, details->runway_end1_str, &pt, clip_rect, 0, 0);
          }

        if (as_runway(details->entity)->designator_end2.length > 0)
          {
          to_screen_coordinates(&params->base, &params->base.map->geo_pos.top_left, &as_runway(details->entity)->designator_end2_position, &pt);

          if (details->runway_end1_str == 0)
            get_string(viewport, params->base.map->font, &as_runway(details->entity)->designator_end2, &details->runway_end2_str, &details->runway_end2_extents);

          if (as_runway(details->entity)->right_hand_circuits)
            {
            // draw outbound arrow
            }

          // TODO: rotate text
          pt.x -= details->runway_end2_extents.dx >> 1;
          draw_text(canvas, clip_rect, params->base.map->font, pen, color_hollow,
            as_runway(details->entity)->designator_end2.length, details->runway_end2_str, &pt, clip_rect, 0, 0);
          }
        break;
      }
    }
  }

result_t on_select_airway(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params, const point_t* pt)
  {
  return s_false;
  }

void on_render_airway(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params)
  {
  }

result_t on_select_instrument_reporting_point(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params, const point_t* pt)
  {
  return s_false;
  }

void on_render_instrument_reporting_point(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params)
  {
  }

result_t on_select_visual_reporting_point(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params, const point_t* pt)
  {
  return s_false;
  }

void on_render_visual_reporting_point(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params)
  {
  }

result_t on_select_vfr_approach(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params, const point_t* pt)
  {
  return s_false;
  }

void on_render_vfr_approach(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params)
  {
  }

result_t on_select_ifr_approach(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params, const point_t* pt)
  {
  return s_false;
  }

void on_render_ifr_approach(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params)
  {
  }

result_t on_select_ifr_departure(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params, const point_t* pt)
  {
  return s_false;
  }

void on_render_ifr_departure(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params)
  {
  }

result_t on_select_obstruction(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params, const point_t* pt)
  {
  return s_false;
  }

void on_render_obstruction(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params)
  {

  }

result_t on_select_spot_entity(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params, const point_t* pt)
  {
  return s_false;
  }

void on_render_spot_entity(atomdb_viewport_t* viewport, const spatial_entity_id_t* id, handle_t canvas, const rect_t* clip_rect, viewport_params_t* params)
  {
  }
#endif
