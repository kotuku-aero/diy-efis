#ifndef __clipper_h__
#define __clipper_h__

#include "../neutron/neutron.h"
#include "../neutron/type_vector.h"
#include "../neutron/vector.h"
#include "../atomdb/spatial.h"

//use_lines: Enables line clipping. Adds a very minor cost to performance.

typedef enum _e_clip_type { ctIntersection, ctUnion, ctDifference, ctXor } e_clip_type;
typedef enum _e_polygon_type { ptSubject, ptClip } e_polygon_type;

//By far the most widely used winding rules for polygon filling are
//EvenOdd & NonZero (GDI, GDI+, XLib, OpenGL, Cairo, AGG, Quartz, SVG, Gr32)
//Others rules include Positive, Negative and ABS_GTR_EQ_TWO (only in OpenGL)
//see http://glprogramming.com/red/chapter11.html
typedef enum _e_polygon_fill_type { pftEvenOdd, pftNonZero, pftPositive, pftNegative } e_polygon_fill_type;


typedef struct _fixed_rect_t
  { 
  fixed_t left;
  fixed_t top;
  fixed_t right;
  fixed_t bottom;
  } fixed_rect_t;

typedef struct _clipper_t clipper_t;

extern result_t create_clipper(clipper_t **clipper);
extern result_t destroy_clipper(clipper_t *clipper);

extern bool get_preserve_collinear(const clipper_t *clipper);
extern void set_preserve_collinear(clipper_t *clipper, bool value);
extern bool get_strictly_simple(const clipper_t *clipper);
extern void set_strictly_simple(clipper_t *clipper, bool value);

extern result_t clipper_add_path(clipper_t *clipper, const spatial_points_t *pg, e_polygon_type polygon_type, bool Closed);
extern result_t clipper_clear(clipper_t *clipper);
extern const fixed_rect_t *clipper_get_bounds(clipper_t *clipper, fixed_rect_t *rect);

extern result_t clipper_execute(clipper_t *clipper, e_clip_type clip_type, 
  e_polygon_fill_type subjFillType, e_polygon_fill_type clipFillType,
  polylines_t *solution);

#endif


