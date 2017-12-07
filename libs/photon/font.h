#ifndef __font_h__
#define __font_h__

#include "photon.h"

struct _fontrec_t;

// a glyph is stored as a vector of alpha values (255 = opaque, 0 = transparent)
// to allow alpha blended font outlines.
typedef struct _glyph_t {
  uint16_t length;        // is length of bitmap, structure length is sizeof(font_glyph_t) + length
  uint16_t wchar;         // unicode character
  uint16_t codepoint;     // offset into file
  uint16_t width;         // width of glyph
  uint16_t height;        // height of glyph
  uint8_t bitmap[];
  } glyph_t;

typedef struct _font_t {
  uint16_t version;
  struct _fontrec_t *fontrec;
  uint16_t size;          // length of the font
  handle_t glyphs;       // map of characters to cached bitmap rects
  } font_t;

// The following structure is defined publically so you can declare one on
// the stack or as a global or etc, but you should treat it as opaque.
typedef struct _font_info_t
  {
  handle_t stream;              // pointer to .ttf file
  int16_t num_glyphs;                     // number of glyphs, needed for range checking

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
  } fontinfo_t;

// We cache the fonts into an array of fonts.
// this structure holds the name->font mapping
typedef struct _fontrec_t {
  const char *name;       // registered name of the font
  handle_t file;          // handle to file for the font
  handle_t fonts;         // font vector.
  fontinfo_t info;    // loaded font info
  } fontrec_t;


// Given a file that defines a font, this function builds
// the necessary cached info for the rest of the system. You must allocate
// the _font_info_t yourself, and stbtt_InitFont will fill it out. You don't
// need to do anything special to free it, because the contents are pure
// value stream with no additional stream structures. Returns 0 on failure.
extern result_t init_font(fontinfo_t *info, handle_t stream);

/**
 * @function result_t create_scaled_glyph(fontinfo_t *fontinfo, uint16_t height, char ch, glyph_t **glyph)
 * Look up the character from the font information and create a scaled glyph
 * bitmap that can be rendered
 * @param fontinfo  Loaded true type font
 * @param height    Height of the glyph in pixels
 * @param ch        Character to render
 * @param glyph     Resulting scalled and rasterized glyph
 * @return s_ok if the glyph can be rendered
*/
extern result_t create_scaled_glyph(fontinfo_t *f, uint16_t pixel_height, char ch, glyph_t **glyph);
/**
* @function result_t ensure_glyph(font_t *font, char ch, const glyph_t **gp)
* Ensure that a character has a cached glyph
* @param font       Loaded true type font
* @param ch         Character to render
* @param glyph      Resulting scalled and rasterized glyph
* @return s_ok if the glyph can be rendered
*/
extern result_t ensure_glyph(font_t *font, char ch, const glyph_t **gp);

#endif
