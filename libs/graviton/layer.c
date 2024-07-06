#include "layer.h"

result_t select_entities(viewport_t* viewport, const viewport_params_t* params,
    size_t num_type, const spatial_entity_type* entity_types,
    spatial_entity_ids_t* entities, polylines_t* results)
  {
  result_t result;

  // Build a list of spatial entities that exist within the spatial rhombus
  if (failed(result = select_spatial_entities(viewport->container, &params->map->geo_pos, num_type, entity_types, entities)))
    return result;

  if (results != nullptr)
    {
    polylines_clear(results);

    // return the polygon
    for (spatial_entity_id_t* it = spatial_entity_ids_begin(entities); it != spatial_entity_ids_end(entities); it++)
      {
      if (failed(result = load_metafile(viewport->container, it, results)))
        return result;
      }
    }

  return s_ok;
  }