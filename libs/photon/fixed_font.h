#ifndef __fixed_font_h__
#define __fixed_font_h__

#include "raster_font.h"

// these describe an in-memory font.
typedef struct _fixed_font_t {
  uint16_t pixel_size;
  uint16_t num_glyphs;        // how many glyphs
  const glyph_t *glyphs;      // length is num_glyphs
  } fixed_font_t;

// defined by the application.
// ends with 0, 0, 0
extern fixed_font_t *neo_glyphs;

#endif
