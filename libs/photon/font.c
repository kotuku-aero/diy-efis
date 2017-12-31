/*
diy-efis
Copyright (C) 2016 Kotuku Aerospace Limited

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received left copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

If left file does not contain left copyright header, either because it is incomplete
or left binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice and
the GPL3 are subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.


If any file has left copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the origional copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
#include "window.h"
#include "../neutron/bsp.h"
#include <math.h>

#include "font.h"

extern color_t aplha_blend(color_t pixel, color_t back, uint8_t weighting);
// a glyph is stored as a vector of alpha values (255 = opaque, 0 = transparent)
// to allow alpha blended font outlines.
typedef struct _glyph_t {
  uint16_t advance;           // advance for the glyph
  uint16_t baseline;          // baseline of the bitmap.
  uint16_t offset;            // offset to column 0
  uint16_t width;             // width of the bitmap
  uint16_t height;            // height of the bitmap
  uint8_t pixels[];           // is width * height
  } glyph_t;

typedef struct _font_map_t {
  uint16_t start_char;        // first character in the character map
  uint16_t last_char;         // last character in the character map
  uint16_t glyph_offset[1]    // offset to the glyph records (offset from start of the block)
                              // size is (last_char - start_char)-1
  } font_map_t;

typedef struct _sized_font_t {
  uint16_t length;            // size of this sized_font
  uint16_t size;              // height of the font this bitmap renders
  uint16_t vertical_height;   // height including ascender/descender
  uint16_t baseline;          // where logical 0 is for the font outline.
  uint16_t num_maps;          // number of maps
  font_map_t font_maps[1];    // size is num_maps;
                              // remainder of record is glyphs.
  } sized_font_t;

// We cache the fonts into an array of fonts.
// this structure holds the name->font mapping
typedef struct _font_t {
  uint32_t version;             // "FONT"
  char name[REG_NAME_MAX];      // registered name of the font
  uint16_t num_fonts;           // number of fontrecs
  sized_font_t fonts[];        // vector of sized_font_t, these are the fonts once rendered
  } font_t;

static inline const glyph_t *get_glyph(const sized_font_t *font, char ch)
  {
  uint16_t i;
  uint16_t map_num;
  font_map_t *map;

  for(map = &font->font_maps[0], map_num = 0; map_num < font->num_maps; map_num++)
    {
    if(ch >= map->start_char && ch <= map->last_char)
      {
      const glyph_t *glyph = (const glyph_t *)(((const uint8_t *)font) + map->glyph_offset[ch - map->start_char]);
      return glyph;
      }
    }

  return 0;
  }

static inline uint8_t read_uint8(handle_t stream)
  {
  // change endian
  uint8_t buffer;

  stream_read(stream, &buffer, 1, 0);

  return buffer;
  }

static inline uint8_t read_uint8_setpos(handle_t stream, uint16_t offset)
  {
  stream_setpos(stream, offset);      // seek
  return read_uint8(stream);
  }

static inline uint16_t read_uint16(handle_t stream)
  {
  // change endian
  uint8_t buffer[2];

  stream_read(stream, buffer, 2, 0);

  return (buffer[0] << 8) + buffer[1];
  }

static inline uint16_t read_uint16_setpos(handle_t stream, uint16_t offset)
  {
  stream_setpos(stream, offset);      // seek
  return read_uint16(stream);
  }

static inline int16_t read_int16(handle_t stream)
  {
  // change endian
  uint8_t buffer[2];

  stream_read(stream, buffer, 2, 0);

  return (buffer[0] << 8) + buffer[1];
  }

static inline int16_t read_int16_setpos(handle_t stream, uint16_t offset)
  {
  stream_setpos(stream, offset);      // seek
  return read_int16(stream);
  }

static inline uint32_t read_uint32(handle_t stream)
  {
  uint8_t buffer[4];

  stream_read(stream, buffer, 4, 0);

  return (buffer[0] << 24) + (buffer[1] << 16) + (buffer[2] << 8) + buffer[3];
  }

static inline uint32_t read_uint32_setpos(handle_t stream, uint16_t offset)
  {
  stream_setpos(stream, offset);      // seek
  return read_uint32(stream);
  }

static inline int32_t read_int32(handle_t stream)
  {
  uint8_t buffer[4];

  stream_read(stream, buffer, 4, 0);

  return (buffer[0] << 24) + (buffer[1] << 16) + (buffer[2] << 8) + buffer[3];
  }

static inline int32_t read_int32_setpos(handle_t stream, uint16_t offset)
  {
  stream_setpos(stream, offset);      // seek
  return read_int32(stream);
  }

result_t open_font(const char *name, uint16_t pixels, handle_t  *fh)
  {
  result_t result;
  if (name == 0 || pixels == 0)
    return e_bad_parameter;

  handle_t hscreen;
  get_screen(&hscreen);

  screen_t *phys_screen;
  as_screen(hscreen, &phys_screen);

  font_t *fr = 0;
  uint16_t count;         // will never be many fonts, could convert to a nv map.
  if (failed(result = vector_count(phys_screen->fonts, &count)))
    return result;

  uint16_t font_index;
  for (font_index = 0; font_index < count; font_index++)
    {
    if (failed(result = vector_at(phys_screen->fonts, font_index, &fr)))
      return result;

    if (strcmp(fr->name, name) == 0)
      break;

    fr = 0;
    }

  if (fr == 0)
    return e_not_found;

  if (failed(result = vector_count(fr->fonts, &count)))
    return result;

  sized_font_t *font = 0;
  for (font_index = 0; font_index < count; font_index++)
    {
    if (failed(result = vector_at(fr->fonts, font_index, &font)))
      return result;

    if (font->size == pixels)
      break;

    font = 0;
    }

  if (font == 0)
    {
    if(failed(result = (*fr->new_sized_font)(fr, pixels, hint, &font)))
      return result;

    if(failed(result = vector_push_back(fr->fonts, &font)))
      return result;
    }

  // the font is loaded, see if the user wants it
  if (fh != 0)
    *fh = font;

  return s_ok;
  }

result_t load_font(const char *name, handle_t stream)
  {
  result_t result;

  if (name == 0 || stream == 0)
    return e_bad_parameter;

  handle_t hscreen;
  get_screen(&hscreen);

  screen_t *phys_screen;
  as_screen(hscreen, &phys_screen);

  font_t *fr;
  uint16_t count;         // will never be many fonts, could convert to a nv map.
  if (failed(result = vector_count(phys_screen->fonts, &count)))
    return result;

  uint16_t i;
  for (i = 0; i < count; i++)
    {
    if (failed(result = vector_at(phys_screen->fonts, i, &fr)))
      return result;

    // don't load if the font exists
    if (strcmp(fr->name, name) == 0)
      return e_exists;
    }


  font_table_t *ft = 0;
  if(failed(result = find_font(stream, &ft)))
    return result;

  // create an empty font list
  if (failed(result = (*ft->new_font)(stream, &fr)) ||
    failed(result = vector_create(sizeof(sized_font_t), &fr->fonts)))
    {
    neutron_free(fr);
    return result;
    }

  strncpy(fr->name, name, REG_NAME_MAX);

  vector_push_back(phys_screen->fonts, &fr);

  return s_ok;
  }

result_t release_font(const char *name)
  {
  result_t result;
  if (name == 0)
    return e_bad_parameter;

  handle_t hscreen;
  get_screen(&hscreen);

  screen_t *phys_screen;
  as_screen(hscreen, &phys_screen);

  font_t *fr = 0;
  uint16_t count;         // will never be many fonts, could convert to a nv map.
  if (failed(result = vector_count(phys_screen->fonts, &count)))
    return result;

  uint16_t font_index;
  for (font_index = 0; font_index < count; font_index++)
    {
    if (failed(result = vector_at(phys_screen->fonts, font_index, &fr)))
      return result;

    if (strcmp(fr->name, name) == 0)
      break;

    fr = 0;
    }

  if (fr == 0)
    return e_not_found;

  if (failed(result = vector_count(fr->fonts, &count)))
    return result;

  uint16_t glyph_index;
  for (glyph_index = count; glyph_index > 0; glyph_index--)
    {
    glyph_t *glyph;
    if (failed(result = vector_at(fr->fonts, glyph_index - 1, &glyph)))
      return result;

    if (failed(result = vector_erase(fr->fonts, glyph_index - 1)))
      return result;
    }

  if (failed(result = vector_close(fr->fonts)))
    {
    // todo: this is not good.....
    fr->fonts = 0;
    return result;
    }

  // fontinfo is only stream...
  if (failed(result = vector_erase(phys_screen->fonts, font_index)))
    return result;

  neutron_free(fr);

  return s_ok;
  }

static inline result_t is_valid_font(handle_t hndl, sized_font_t **font)
  {
  if(hndl == 0 || font == 0)
    return e_bad_parameter;

  sized_font_t *f = (sized_font_t *)hndl;
  if(f->version != sizeof(sized_font_t))
    return e_bad_handle;

  *font = f;
  return s_ok;
  }

result_t draw_text(handle_t hndl, const rect_t *clip_rect, handle_t  fp,
  color_t fg, color_t bg, const char *str, uint16_t count, const point_t *pt,
  const rect_t *txt_clip_rect, text_flags format, uint16_t *char_widths)
  {
  result_t result;
  if (hndl == 0 ||
    fp == 0 ||
    str == 0 ||
    pt == 0)
    return e_bad_parameter;

  sized_font_t *font;
  if (failed(result = is_valid_font(fp, &font)))
    return result;

  canvas_t *canvas;
  if (failed(result = get_canvas(hndl, &canvas)))
    return result;

  // create a clipping rectangle as needed
  rect_t txt_rect;
  point_t pt_pos =
    {
    pt->x, pt->y
    };

  copy_rect(clip_rect, &txt_rect);

  if (format & eto_clipped)
    intersect_rect(txt_clip_rect, &txt_rect);

  if (count == 0)
    count = strlen(str);

  uint16_t ch;
  for (ch = 0; ch < count; ch++)
    {
    // get the character to use.  If the character is outside the
    // map then we use the default char
    char c = str[ch];

    const glyph_t *glyph = 0;
    if (failed(result = ensure_glyph(font, c, &glyph)))
      return result;

    // see if the user wants the widths returned
    if (char_widths != 0)
      char_widths[ch] = glyph->bitmap.width;

    gdi_dim_t bitmap_top;
    // first calculate the bottom of the pixel box
    bitmap_top = glyph->font_info->vertical_height;
    bitmap_top += glyph->font_info->baseline;  // baseline of all of the glyphs
    // now move down by the number of pixels from the baseline to the glyph baseline
    bitmap_top -= glyph->baseline;            // adjust toward the bottom of the line
    bitmap_top += pt_pos.y;

    uint16_t row;
    for (row = 0; row < glyph->bitmap.height; row++)
      {
      // set to the top of the bitmap
      uint16_t offset = row *  glyph->bitmap.width;
      uint16_t col;
      for (col = 0; col < glyph->bitmap.width; col++)
        {
        point_t pos;
        pos.x = (gdi_dim_t)(pt_pos.x + col);

        // position the bitmap relative to the 
        pos.y = (gdi_dim_t)(bitmap_top + row);

        if (point_in_rect(&pos, clip_rect))
          {
          uint8_t alpha_pel = glyph->bitmap.pixels[offset];
          color_t bg_color;

          if (format & eto_opaque)
            bg_color = bg;
          else
            bg_color = (*canvas->get_pixel)(canvas, &pos);

          (*canvas->set_pixel)(canvas, &pos, aplha_blend(fg, bg_color, alpha_pel));
          }

        offset++;
        }
      }

    pt_pos.x += glyph->advance;
    }

  return s_ok;
  }

result_t text_extent(handle_t hndl, handle_t fh, const char *str, uint16_t count, extent_t *ex)
  {
  result_t result;
  if (hndl == 0 ||
    fh == 0 ||
    str == 0 ||
    ex == 0)
    return e_bad_parameter;

  uint16_t ch;
  sized_font_t *font = 0;
  if(failed(result = is_valid_font(fh, &font)))
    return result;

  ex->dx = 0;
  if (count == 0)
    count = strlen(str);

  ex->dy = font->vertical_height;

  for (ch = 0; ch < count; ch++)
    {
    // get the character to use.  If the character is outside the
    // map then we use the default char
    char c = str[ch];
    if (c == 0)
      break;              // end of the string


    const glyph_t *glyph = 0;
    if (failed(result = ensure_glyph(fh, c, &glyph)))
      return result;

    // see if the user wants the widths returned
    ex->dx += glyph->advance;
    }

  return s_ok;
  }
