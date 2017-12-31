#ifndef __tt_font_h__
#define __tt_font_h__

#include "font.h"


extern result_t polypolygon_impl(canvas_t *canvas, const rect_t *clip_rect, const pen_t *outline, color_t fill, uint16_t count, const uint16_t *lengths, const point_t *pts);
// true type font data structures
enum {
  contour_vmove = 1,      // move to point
  contour_vline,          // line to point x,y is point
  contour_vcurve,         // bezier curve with control point in cx, cy 
  };

typedef struct
  {
  gdi_dim_t x, y, cx, cy;
  uint8_t type;
  } glyph_vertex_t;

typedef struct _glyph_contour_t {
  uint16_t end_flag;
  uint16_t num_vertices;
  gdi_dim_t x_min;
  gdi_dim_t y_min;
  gdi_dim_t x_max;
  gdi_dim_t y_max;
  glyph_vertex_t *vertices;
  } glyph_contour_t;

typedef struct _glyph_shape_t {
  gdi_dim_t x_min;
  gdi_dim_t y_min;
  gdi_dim_t x_max;
  gdi_dim_t y_max;
  uint16_t codepoint;
  uint16_t num_contours;
  glyph_contour_t *contours[];
  } glyph_shape_t;

typedef struct _tt_font_t {
  font_t hdr;
  handle_t stream;              // pointer to font file
  handle_t shapes;              // map of codepoints to shape records
  int16_t num_glyphs;           // number of glyphs, needed for range checking
  rect_t clip_rect;             // bounding box of the glyphs.
  uint16_t index_map;                     // a cmap mapping for our chosen character encoding
  uint16_t index_to_location_format;      // format needed to map from glyph index to glyph
  uint16_t loca;                  // index to location table
  uint16_t head;                  // header table
  uint16_t glyf;
  uint16_t hhea;
  uint16_t hmtx;
  uint16_t charstrings;            // the charstring index
  uint16_t gsubrs;                 // global charstring subroutines index
  uint16_t subrs;                  // private charstring subroutines index
  uint16_t fontdicts;              // array of font dicts
  uint16_t fdselect;               // map from glyph to fontdict
  } tt_font_t;

#endif
