#include "raster_font.h"

// A raster font stream is defined as the following format
//
// uint32_t magic	0x80010000 for version 1.0.
// char name[REG_NAME_MAX]          // name of the font.
// uint16_t num_fonts               // number of fixed size fonts
// the following record is repeated for num_fonts
// uint16_t record_size;            // length of this font record.
// uint16_t size;                   // height of the font this bitmap renders
// uint16_t vertical_height;        // height including ascender/descender
// uint16_t baseline;               // where logical 0 is for the font outline.
// uint16_t num_maps                // number of character maps
// the character maps then continue for the num_maps
// uint16_t start_char              // first character in the character map
// uint16_t last_char               // last character in the character map
// uint16_t glyphs_offset           // offset to the glyph records (offset from start of the block)
// The glyphs then follow in the following format
// uint16_t glyph_advance           // horizontal advance for the glyph
// uint16_t glyph_baseline          // baseline of the bitmap, is aligned to the baseline when rendered
// uint16_t width                   // width of the actual glyph
// uint16_t height                  // height of the glyph
// uint8_t bitmap[width * height]   // alpha values of the bitmap

// We support a font called NEO that is stored in the application.  The font file is converted to
// an array of bytes.


typedef struct _raster_font_t {
  font_t hdr;
  handle_t stream;

  } raster_font_t;

static void free_raster_font(font_t *font)
  {
  neutron_free(font);
  }

static int compare_codepoint(const void *left, const void *right)
  {
  char fl = (char)left;
  char fr = (char)right;

  return (fl > fr) ? 1 : (fl < fr) ? -1 : 0;
  }

static void dup_codepoint(const void *src, void **dst)
  {
  *dst = (void *)src;       // stored by reference
  }

static void dup_glyph(const void *src, void **dst)
  {
  *dst = *((glyph_t **)src);       // stored by reference
  }

static void destroy_codepoint(void *key)
  {
  // do not destroy the key!
  }

static void destroy_glyph(void *value)
  {
  neutron_free(value);
  }

static result_t is_raster_font_handle(font_t *font, raster_font_t **rf)
  {
  if(font == 0 || rf == 0)
    return e_bad_parameter;

  *rf = (raster_font_t *)font;
  if((*rf)->hdr.version != sizeof(raster_font_t))
    return e_bad_handle;

  return s_ok;
  }

static result_t new_glyph(sized_font_t *font, char ch, glyph_t **glyph)
  {

  }

static result_t load_neo(sized_font_t *glyphs, uint16_t pixels, const char *hints)
  {
  result_t result;
  uint16_t i;
  }

static result_t load_stream(handle_t stream, sized_font_t *glyphs, uint16_t pixels, const char *hints)
  {
  result_t result;
  // the stream is positioned at the num_maps
  uint16_t block_start = 0;
  if(failed(result = stream_getpos(stream, &block_start)))
    return result;

  block_start -= 8;       // skip header

  return s_ok;
  }

static result_t new_sized_font(font_t *fh, uint16_t pixels, const char *hints, sized_font_t **glyphs)
  {
  result_t result;
  raster_font_t *font = 0;

  if(failed(result = is_raster_font_handle(fh, &font)))
    return result;

  // very special case for the neo font.  The stream does not have the
  // glyphs in it.  They are stored as a binary image in the code.
  // we create a stream that has a small record_size and set the
  // glyph offsets to 0.  We then store the code-maps and glyphs
  // in the flash memory to save space.
  bool is_neo = strcmp(font->hdr.name, "neo") == 0;

  // skip through the file and when the font size is found load it.
  uint16_t num_fonts = read_uint16_setpos(font->stream, 4 + REG_NAME_MAX);
  do
    {
    uint16_t len = read_uint16(font->stream);
    uint16_t size = read_uint16(font->stream);
    if(size == pixels)
      {
      // we have found a bitmap font we need
      sized_font_t *sf = (sized_font_t*)neutron_malloc(sizeof(sized_font_t));
      sf->version = sizeof(sized_font_t);
      sf->font = fh;
      sf->new_glyph = new_glyph;
      sf->vertical_height = read_uint16(font->stream);
      sf->baseline = read_uint16(font->stream);

      if(failed(result = map_create(dup_codepoint, dup_glyph, compare_codepoint, destroy_codepoint, is_neo ? 0 : destroy_glyph, &sf->glyphs)))
        {
        neutron_free(sf);
        return result;
        }

      *glyphs = sf;

      if(is_neo)
        return load_neo(sf, pixels, hints);

      return load_stream(font->stream, sf, pixels, hints);
      }
    uint16_t pos;
    if(failed(result = stream_getpos(font->stream, &pos)) ||
      failed(result = stream_setpos(font->stream, len - 4 + pos)))
      return result;
    } while(!stream_eof(font->stream));

  return e_not_found;
  }

result_t is_raster_font(handle_t stream)
  {
  // return ok if this is a raster font.
  return read_uint32_setpos(stream, 0) == 0x80010000 ? s_ok : s_false;
  }

result_t new_raster_font(handle_t stream, font_t **font)
  {

  }

