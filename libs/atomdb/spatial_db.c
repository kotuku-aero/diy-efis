#include "db_priv.h"

typedef struct _geospatial_db_t {
  base_t base;
  const geospatial_container_header_t* hdr;         // loaded header
  handle_t db;
  handle_t stream;

  } geospatial_container_t;

/**
 * @brief Return the offset and length of a spatial_entity_id_t list from a spatial container
 * @param cont          Container to query
 * @param tile          Tile to sample
 * @param list          details of the list
 * @return s_ok if tile located and results returned
*/
static result_t get_spatial_list(geospatial_container_t* cont, int lat, int lng, spatial_offset_t* list)
  {
  result_t result;
  int lat_abs = 90 - lat;      // latitude is -90 to 90, make 0-180
  int lng_abs = lng + 180;     // longitude id -180 to 180, make 0-360

  //int lat_offs = 90 - fixed_to_int(cont->hdr->base.bounds.top);     // 90 == 0, 89 = 1 ...
  //lat_abs -= lat_offs;                                // makes index into array

  //int lng_offs = -180 - fixed_to_int(cont->hdr->base.bounds.left);    // -180 = 0, -179 = -1 ...
  //lng_abs -= lng_offs;

  if (lat_abs < 0 || lng_abs < 0 || ((uint32_t)lat_abs) > cont->hdr->tiles_lat || ((uint32_t)lng_abs) > cont->hdr->tiles_lng)
    return e_not_found;

  uint32_t offset = sizeof(geospatial_container_header_t);
  offset += (((lat_abs * cont->hdr->tiles_lng) + lng_abs) * sizeof(spatial_offset_t));

  if (failed(result = stream_setpos(cont->stream, offset, STREAM_SEEK_SET)))
    return result;

  uint32_t read = 0;
  if (failed(result = stream_read(cont->stream, list, sizeof(spatial_offset_t), &read)))
    return result;

  if (read != sizeof(spatial_offset_t))
    return e_no_more_information;

  return s_ok;
  }

static result_t destroy_db(handle_t handle)
  {
  geospatial_container_t* db = (geospatial_container_t*)handle;

  close_handle(db->stream);

  return s_ok;
  }

const typeid_t spatial_container_type =
  {
  .name = "spatial_db",
  .etherealize = destroy_db
  };

result_t open_geospatial_container(handle_t atom_db, handle_t stream, const geospatial_container_header_t* hdr, handle_t* out)
  {
  result_t result;
  geospatial_container_t* hndl;

  if (failed(result = neutron_malloc(sizeof(geospatial_container_t), (void**)&hndl)))
    return result;

  // create a handle to the spatial database
  hndl->base.type = &spatial_container_type;
  hndl->db = atom_db;
  hndl->hdr = hdr;
  hndl->stream = stream;

  *out = hndl;

  return s_ok;
  }

result_t incarnate_spatial_entity(handle_t db, const spatial_entity_id_t* oid, spatial_entity_hdr_t **entity)
  {
  result_t result;
  geospatial_container_t* cont;

  if (failed(result = is_typeof(db, &spatial_container_type, (void**)&cont)))
    return result;

  // note we do not read the metafile, only the header!
  spatial_entity_hdr_t* out;
  if (failed(result = neutron_malloc(oid->length, (void**)&out)))
    return result;

  if (failed(stream_setpos(cont->stream, oid->offset, STREAM_SEEK_SET)))
    return result;

  uint32_t read;
  if (failed(stream_read(cont->stream, out, oid->length, &read)) ||
    read != oid->length)
    {
    neutron_free(out);
    if (succeeded(result))
      result = e_not_found;

    return result;
    }

  *entity = out;
  return result;
  }

result_t etherealize_spatial_entity(handle_t db, spatial_entity_hdr_t* entity)
  {
  return neutron_free(entity);
  }

static result_t read_uint32(handle_t stream, uint32_t* value)
  {
  result_t result;
  uint32_t read;
  if(failed(result = stream_read(stream, value, sizeof(uint32_t), &read)))
    return result;

  if(read != sizeof(uint32_t))
    return e_no_more_information;

  return s_ok;
  }

static result_t read_int32(handle_t stream, int32_t* value)
  {
  result_t result;
  uint32_t read;
  if(failed(result = stream_read(stream, value, sizeof(int32_t), &read)))
    return result;

  if(read != sizeof(int32_t))
    return e_no_more_information;

  return s_ok;
  }


static result_t read_int16(handle_t stream, int16_t* value)
  {
  result_t result;
  uint32_t read;
  if (failed(result = stream_read(stream, value, sizeof(uint16_t), &read)))
    return result;

  if (read != sizeof(uint16_t))
    return e_no_more_information;

  return s_ok;
  }


result_t select_spatial_entities(handle_t db, const geo_rhombus_t* bounds,
  size_t num_types, const spatial_entity_type* types, spatial_entity_ids_t* entities)
  {
  result_t result;
  geospatial_container_t* cont;

  if (failed(result = is_typeof(db, &spatial_container_type, (void**)&cont)))
    return result;

  // this is done by using the spatial index as a first pass,
  // then the entities are recovered, and those that match the bounds and types
  // are read from the database

  latlng_t polygon[5];

  polygon[0].lat = polygon[4].lat = bounds->top_left.lat;
  polygon[0].lng = polygon[4].lng = bounds->top_left.lng;

  polygon[1].lat = bounds->top_right.lat;
  polygon[1].lng = bounds->top_right.lng;

  polygon[2].lat = bounds->bottom_right.lat;
  polygon[2].lng = bounds->bottom_right.lng;

  polygon[3].lat = bounds->bottom_left.lat;
  polygon[3].lng = bounds->bottom_left.lng;

  int min_lat = 90;
  int max_lat = -90;

  int min_lng = 180;
  int max_lng = -180;

  for (int i = 0; i < 5; i++)
    {
    int deg = fixed_to_int(polygon[i].lat);

    if (min_lat > deg)
      min_lat = deg;

    if (max_lat < deg)
      max_lat = deg;

    deg = fixed_to_int(polygon[i].lng);

    if (min_lng > deg)
      min_lng = deg;

    if (max_lng < deg)
      max_lng = deg;
    }

  // check each tile that is within the bounds for
  // overlap.
  for (int lat = max_lat; lat >= min_lat; lat--)
    {
    for (int lng = min_lng; lng <= max_lng; lng++)
      {
      // this is done because the rhombus may not be in a NED frame
      bool inside = false;
      latlng_t* pt_a = polygon + 4;
      for (latlng_t* pt_b = polygon; pt_b < polygon + 5; pt_b++)
        {
        if (fixed_to_int(pt_b->lat) == lat && fixed_to_int(pt_b->lng) == lng)
          {
          inside = true;
          goto inside_poly;
          }

        if ((fixed_to_int(pt_b->lat) == fixed_to_int(pt_a->lat)) && (fixed_to_int(pt_a->lat) == lat))
          {
          if (((fixed_to_int(pt_a->lng) <= lng) && (lng <= fixed_to_int(pt_b->lng))) ||
            ((fixed_to_int(pt_b->lng) <= lng) && (lng <= fixed_to_int(pt_a->lng))))
            {
            inside = true;
            goto inside_poly;
            }
          }

        if (((fixed_to_int(pt_b->lat) < lat) && (fixed_to_int(pt_a->lat))) ||
          ((fixed_to_int(pt_a->lat) < lat) && (fixed_to_int(pt_b->lat) >= lat)))
          {

           if (fixed_to_int(pt_b->lng) + (lat - fixed_to_int(pt_b->lat)) /
            ((fixed_to_int(pt_a->lat) - fixed_to_int(pt_b->lat)) * (fixed_to_int(pt_a->lng) - fixed_to_int(pt_b->lng)) < lng))
            inside = !inside;
          //if (div_fixed(add_fixed(pt_b->lng, sub_fixed(int_to_fixed(lat), pt_b->lat)),
          //  mul_fixed(sub_fixed(pt_a->lat, pt_b->lat), sub_fixed(pt_a->lng,pt_b->lng))) < int_to_fixed(lng))
          //  inside = !inside;
          }

        pt_a = pt_b;
        }
      /*
            if ((b.X == point.X) && (b.Y == point.Y))
                return true;

            if ((b.Y == a.Y) && (point.Y == a.Y))
            {
                if ((a.X <= point.X) && (point.X <= b.X))
                    return true;

                if ((b.X <= point.X) && (point.X <= a.X))
                    return true;
            }

            if ((b.Y < point.Y) && (a.Y >= point.Y) || (a.Y < point.Y) && (b.Y >= point.Y))
            {
                if (b.X + (point.Y - b.Y) / (a.Y - b.Y) * (a.X - b.X) <= point.X)
                    result = !result;
            }
            a = b;
      */

    inside_poly:
      if (inside)
        {
        spatial_offset_t entity_list;
        // the tile is contained in the polygon, load the list of entities
        if (failed(get_spatial_list(cont, lat, lng, &entity_list)))
          continue;       // no entities

        if (entity_list.length == 0 || entity_list.offset == 0)
          continue;

        // read the list entries,
        if (failed(result = stream_setpos(cont->stream, entity_list.offset, STREAM_SEEK_SET)))
          return result;

        spatial_entity_id_t entity;
        while (entity_list.length-- > 0)
          {
          if(failed(result = read_uint32(cont->stream, &entity.type)) ||
            failed(result = read_uint32(cont->stream, &entity.offset)) ||
            failed(result = read_uint32(cont->stream, &entity.length)) ||
            failed(result = read_uint32(cont->stream, &entity.graphics_offset)) ||
            failed(result = read_uint32(cont->stream, &entity.graphics_length)))
              return result;

          for (size_t i = 0; i < num_types; i++)
            {
            // if this entity matches then add it
            if ((entity.type & SPATIAL_TYPE_MASK) == types[i])
              {
              bool is_found = false;
              for (spatial_entity_id_t* it = spatial_entity_ids_begin(entities); it != spatial_entity_ids_end(entities); it++)
                if (it->offset == entity.offset && it->graphics_offset == entity.graphics_offset)
                  {
                  is_found = true;
                  break;
                  }

              if (!is_found)
                spatial_entity_ids_push_back(entities, &entity);
              }
            }
          }
        }
      }
    }

  return s_ok;
  }

result_t resolve_string(handle_t hndl, const spatial_string_t* ref, char** out)
  {
  result_t result;
  geospatial_container_t* cont;

  if (failed(result = is_typeof(hndl, &spatial_container_type, (void**)&cont)))
    return result;

  *out = 0;
  if (ref->length == 0 || cont->hdr->base.string_table == 0)
    return s_ok;

  char *str;
  if (failed(result = neutron_malloc(ref->length + 1, (void**)&str)))
    return result;

  int32_t pos = ref->offset + cont->hdr->base.string_table;

  // move to the stream pos
  if (failed(result = stream_setpos(cont->stream, pos, STREAM_SEEK_SET)))
    return result;

  uint32_t read = 0;
  if(failed(result = stream_read(cont->stream, str, ref->length, &read)))
    {
    neutron_free(str);
    return result;
    }

  if (read == 0)
    {
    neutron_free(str);
    return e_not_found;
    }

  str[read] = 0;      // null term
  *out = str;

  return s_ok;
  }

result_t load_metafile(handle_t hndl, const spatial_entity_id_t* ref, polylines_t* out)
  {
  result_t result;
  geospatial_container_t* cont;

  if(failed(result = is_typeof(hndl, &spatial_container_type, (void**)&cont)))
    return result;

  if(ref->graphics_offset == 0 || ref->graphics_length == 0)
    return s_ok;

  // move to the stream pos
  if(failed(result = stream_setpos(cont->stream, ref->graphics_offset, STREAM_SEEK_SET)))
    return result;

  int32_t bytes_remaining = (int32_t) ref->graphics_length;
  while (bytes_remaining > 0)
    {
    uint32_t read;
    uint32_t num_polylines;
    if(failed(result = stream_read(cont->stream, &num_polylines, sizeof(uint32_t), &read)))
      return result;

    if(read != sizeof(uint32_t))
      return e_bad_pointer;

    bytes_remaining -= read;
    for (uint32_t line_num = 0; line_num < num_polylines; line_num++)
      {
      uint32_t line_length;
      if(failed(result = stream_read(cont->stream, &line_length, sizeof(uint32_t), &read)))
        return result;

      if(read != sizeof(uint32_t))
        return e_bad_pointer;

      bytes_remaining -= read;
      if(bytes_remaining < 0)
        break;

      // this is to aviod wastefull copying
      uint16_t index = polylines_count(out);
      polyline_t polyline;
      memset(&polyline, 0, sizeof(polyline_t));
      polylines_push_back(out, &polyline);

      spatial_points_t *line = polylines_begin(out)+ index;
      spatial_point_t pt;
      for (uint32_t pt_num = 0; pt_num < line_length; pt_num++)
        {
        // stream read does not work.  The structure is packed, however the size of
        // the structure is 10 bytes (uint32, uint32, uint16) but the MSVC compiler
        // thinks it 12 bytes !
        if(failed(result = read_int32(cont->stream, &pt.latlng.lat)) ||
          failed(result = read_int32(cont->stream, &pt.latlng.lng)) ||
          failed(result = read_int16(cont->stream, &pt.elevation)))
          return result;

        bytes_remaining -= 10;
        if(bytes_remaining < 0)
          break;

        spatial_points_push_back(line, &pt);
        }
      }
    }

  if(bytes_remaining < 0)
    // read past end of allocated stream!
    return e_bad_pointer;

  return s_ok;
  }