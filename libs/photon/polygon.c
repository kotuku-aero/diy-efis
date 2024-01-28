#include "photon_priv.h"

// #define DEBUG_POLYGON

static inline edge_t* edge_at(edges_t* vec, uint16_t index) { return edges_begin(vec) + index; }

static int compare_edges(const edge_t* l, const edge_t* r)
  {
  /* if the minimum y values are different, sort on minimum y */
  if (l->p1.y != r->p1.y)
    return l->p1.y < r->p1.y ? -1 : 1;

  /* if the current x values are different, sort on current x */
  if (l->p1.x != r->p1.x)
    return l->p1.x < r->p1.x ? -1 : 1;

  /* otherwise they are equal */
  return 0;
  }

static void edge_copy(const edge_t* left, edge_t* right)
  {
  right->d = left->d;
  right->fn = left->fn;
  right->mn = left->mn;
  right->p1.x = left->p1.x;
  right->p1.y = left->p1.y;
  right->p2.x = left->p2.x;
  right->p2.y = left->p2.y;
  }

static void add_edge(handle_t edges, edge_t* edge)
  {
  // a horizontal edge is ignored as this is handled by the end
  // and start of the previous edges
  if (edge->p1.y == edge->p2.y)
    return;

  // swap the points so p1->y is always < p2->y
  if (edge->p1.y > edge->p2.y)
    point_swap(&edge->p1, &edge->p2);

  edge->mn = edge->p2.x - edge->p1.x;
  edge->d = edge->p2.y - edge->p1.y;
  edge->fn = edge->mn >> 1;

  edges_push_back(edges, edge);
  }

result_t polygon_impl(canvas_t* canvas, const rect_t* clip_rect, color_t pen, color_t fill)
  {
  result_t result;

  // if the fill color is not hollow then fill the polygon first
  if (fill != color_hollow)
    {
    edges_clear(&canvas->edges);
    // clip the polygon and ensure the polygons are closed.

    if (failed(result = clip_path(canvas, clip_rect)))
      return result;

#ifdef DEBUG_POLYGON
    trace_debug("Clipped Polygon\n");
    for (size_t i = 0; i < points_count(&canvas->solution); i++)
      {
      point_t* ip = points_begin(&canvas->solution) + i;
      trace_debug("Point %d, x:%d, y:%d\n", i, ip->x, ip->y);
      }
#endif
    // if the polygon does not have enough points then fail.  Could be outside the
    // clipping area completely.
    if (points_count(&canvas->solution) > 3)
      {
      // get the first point, will check to make sure this edge is closed.
      edge_t edge;
      point_copy(points_begin(&canvas->solution), &edge.p1);

      for (uint16_t offs = 1; offs < points_count(&canvas->solution); offs++)
        {
        // form the end of the edge
        point_copy(points_begin(&canvas->solution) + offs, &edge.p2);
        // push the edge
        add_edge(&canvas->edges, &edge);

        // start the next edge
        point_copy(points_begin(&canvas->solution) + offs, &edge.p1);
        }
      }

    // if no edges then the polygon is outside the clip area
    if (edges_count(&canvas->edges) == 0)
      return s_ok;

    edge_t edge1;
    edge_t edge2;

#ifdef DEBUG_POLYGON
    trace_debug("Unsorted edges\n");
    for (size_t i = 0; i < edges_count(&canvas->edges); i++)
      {
      edge_t* edg = edges_begin(&canvas->edges) + i;
      trace_debug("(%d, %d) (%d, %d)\n", edg->p1.x, edg->p1.y, edg->p2.x, edg->p2.y);
      }
#endif

    // now we sort the edges so
    qsort(edges_begin(&canvas->edges), edges_count(&canvas->edges), sizeof(edge_t), compare_edges);

    /* start with the lowest y in the table */
    edge_copy(edge_at(&canvas->edges, 0), &edge1);
    gdi_dim_t y = edge1.p1.y;

#ifdef DEBUG_POLYGON
    trace_debug("Sorted edges\n");
    for (size_t i = 0; i < edges_count(&canvas->edges); i++)
      {
      edge_t* edg = edges_begin(&canvas->edges) + i;
      trace_debug("(%d, %d) (%d, %d)\n", edg->p1.x, edg->p1.y, edg->p2.x, edg->p2.y);
      }
#endif

    do
      {
      /* using odd parity, render alternating line segments */
      for (uint16_t i = 1; i < edges_count(&canvas->edges); i += 2)
        {
        edge_copy(edge_at(&canvas->edges, i - 1), &edge1);
        edge_copy(edge_at(&canvas->edges, i), &edge2);

        if (edge1.p1.y != y)
          break;              // not an active edge so we are done.

        if (edge1.p1.x == edge2.p1.x)
          (*canvas->fb->set_pixel)(canvas->fb, &edge1.p1, fill, 0);
        else
          (*canvas->fb->fast_line)(canvas->fb, &edge1.p1, &edge2.p1, fill);
        }

      /* prepare for the next scan line */
      if (++y > clip_rect->bottom)
        return s_ok;

      bool resort = false;
      /* remove inactive edges from the active edge table */
      /* or update the current x position of active edges */
      for (uint16_t i = 0; i < edges_count(&canvas->edges); i++)
        {
        edge_copy(edge_at(&canvas->edges, i), &edge1);
        if (edge1.p2.y == y)
          {
          // this is the end of an edge
          edges_erase(&canvas->edges, i);
          i--;
          }
        else
          {
          if (edge1.p1.y != y - 1)
            break;
          else
            {
            edge1.fn += edge1.mn;
            if (edge1.fn < 0)
              {
              edge1.p1.x = (edge1.p1.x + (edge1.fn / edge1.d - 1));
              edge1.fn %= edge1.d;
              edge1.fn += edge1.d;
              }

            if (edge1.fn >= edge1.d)
              {
              edge1.p1.x = (edge1.p1.x + (edge1.fn / edge1.d));
              edge1.fn %= edge1.d;
              }

            edge1.p1.y = y;
            resort = true;

            edge_copy(&edge1, edge_at(&canvas->edges, i));
            }
          }
        }

      // re-sort remaining edges
      if (resort)
        {
        qsort(edges_begin(&canvas->edges), edges_count(&canvas->edges), sizeof(edge_t), compare_edges);
#ifdef DEBUG_POLYGON
        trace_debug("Sorted edges after modification of edge\n");
        for (size_t i = 0; i < edges_count(&canvas->edges); i++)
          {
          edge_t* edg = edges_begin(&canvas->edges) + i;
          trace_debug("(%d, %d) (%d, %d)\n", edg->p1.x, edg->p1.y, edg->p2.x, edg->p2.y);
          }
#endif
        }

      /* keep doing this while there are any edges left */
      } while (edges_count(&canvas->edges) > 0);
    }

  return s_ok;
  }

result_t polygon(handle_t hndl, const rect_t* clip_rect, color_t pen, color_t fill, uint32_t count, const point_t* pts)
  {
  if (pen == color_hollow && fill == color_hollow)
    return s_ok;

  if (clip_rect == 0 || pts == 0 || count < 3)
    return e_bad_parameter;

  result_t result;
  canvas_t* canvas;
  if (failed(result = is_typeof(hndl, &canvas_type, (void**)&canvas)))
    return result;

  points_clear(&canvas->path);

  for(uint32_t i = 0; i < count; i++)
    points_push_back(&canvas->path, pts + i);

  if(failed(result = polygon_impl(canvas, clip_rect, pen, fill)))
    return result;

  if (pen != color_hollow)
    {
    points_clear(&canvas->path);

    while (count--)
      points_push_back(&canvas->path, pts++);

    return polyline_impl(canvas, clip_rect, pen);

    }

  return s_ok;
  }
