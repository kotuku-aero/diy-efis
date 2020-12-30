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
#include "../neutron/bsp.h"

extern color_t aplha_blend(color_t pixel, color_t back, uint8_t weighting);
// a glyph is stored as a vector of alpha values (255 = opaque, 0 = transparent)
// to allow alpha blended font outlines.

// the following strcture is aligned to a 16 byte boundary
typedef struct _glyph_t {
  uint8_t advance;           // advance for the glyph
  uint8_t baseline;          // baseline of the bitmap.
  uint8_t offset;            // offset to column 0
  uint8_t width;             // width of the bitmap
  uint8_t height;            // height of the bitmap
  // a pixel is a bitmap..  The width of the font is mod 8
  // so a bitmap 1 x 12 will be 12 bytes
  // a bitmap 8 x 12 will be 12 bytes
  // etc.
  uint8_t pixels[];           // is width * height /8
  } glyph_t;

typedef struct _font_map_t {
  uint8_t start_char;        // first character in the character map
  uint8_t last_char;         // last character in the character map
  uint16_t glyph_offset[1];  // offset to the glyph records (offset from start of the block) (not including the length) so index +2
                             // size is (last_char - start_char)-1
  } font_map_t;

typedef struct _sized_font_t {
  uint16_t length;            // size of this sized_font
  uint8_t size;              // height of the font this bitmap renders
  uint8_t vertical_height;   // height including ascender/descender
  uint8_t baseline;          // where logical 0 is for the font outline.
  uint8_t num_maps;          // number of maps
  uint16_t reserved;          // ensures next record is aligned on 8 bytes
  font_map_t font_maps[1];    // size is num_maps;
  // the above header is 0 packed to a 16 byte boundary
  // remainder of record is glyphs.
  } sized_font_t;

// This is the structure of a font header in a font file.
typedef struct _fontrec_t {
  uint32_t version;             // "FONT", or CFNT
  char name[REG_NAME_MAX];      // registered name of the font
  uint16_t length;              // Length of all of the records that follow including the
  // version, and name.  
  uint8_t num_fonts;            // number of fontrecs
  uint8_t reserved[9];          // makes the header 32 bytes long
  // The following data can be deflate compressed. -- Start deflated data --
  sized_font_t fonts[];         // vector of sized_font_t, these are the fonts once rendered
  } fontrec_t;

typedef struct _font_t {
  const fontrec_t *fontrec;
  // this a a const sized_font_t *
  vector_p fonts;
  } font_t;

static inline uint16_t to_small_endian(uint16_t value)
  {
  return (value << 8) | (value >> 8);
  }

static const glyph_t *get_glyph(const sized_font_t *font, char ch)
  {
  uint16_t i;
  uint16_t map_num;
  const font_map_t *map = &font->font_maps[0];

  for(map_num = 0; map_num < font->num_maps; map_num++)
    {
    if(ch >= map->start_char && ch <= map->last_char)
      {
      uint16_t offset = to_small_endian(map->glyph_offset[ch - map->start_char]);
      const glyph_t *glyph = (const glyph_t *)(((const uint8_t *)font) + offset);
      return glyph;
      }

    // calculate the offset as the maps are variable size;
    uint16_t map_size = 2 + ((map->last_char - map->start_char + 1) << 1);
    map = (const font_map_t *)(((const uint8_t *)map) + map_size);
    }

  return 0;
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

    if (strcmp(fr->fontrec->name, name) == 0)
      break;

    fr = 0;
    }

  if (fr == 0)
    return e_not_found;   // no font with that name.

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
    return e_not_found;

  // the font is loaded, see if the user wants it
  if (fh != 0)
    *fh = font;

  return s_ok;
  }

static inline uint32_t make_type(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
  {
  return (((uint32_t)a) << 24) | (((uint32_t)b) << 16) | (((uint32_t)c) << 8) | d;
  }

result_t register_font(const uint8_t *buffer, uint16_t length)
  {
  result_t result;

  if (buffer == 0 || length < sizeof(fontrec_t))
    return e_bad_parameter;

  const fontrec_t *fontrec = (const fontrec_t *)buffer;

  uint16_t fontrec_length = to_small_endian(fontrec->length);
  if (length < fontrec_length)
    return e_bad_parameter;

  // TODO: check the sanity of the font....

  // the buffer is *fontrec
  font_t *font = (font_t *)neutron_malloc(sizeof(font_t));
  if (font == 0)
    return e_not_enough_memory;

  font->fontrec = fontrec;

  if (failed(result = vector_create(sizeof(const sized_font_t *), &font->fonts)))
    {
    neutron_free(font);
    return result;
    }

  handle_t hscreen;
  get_screen(&hscreen);

  screen_t *phys_screen;
  as_screen(hscreen, &phys_screen);

  // TODO: check for duplicate fonts...

  if (failed(result = vector_push_back(phys_screen->fonts, &font)))
    {
    neutron_free(font);
    return result;
    }

  // work over the decompressed fonts and add them to the font mapper
  uint16_t font_num;
  uint16_t font_length;
  const sized_font_t *pt_font = &font->fontrec->fonts[0];
  for (font_num = 0; font_num < fontrec->num_fonts; font_num++)
    {
    if (failed(result = vector_push_back(font->fonts, &pt_font)))
      {
      return result;        // leave font registered but fail.
      }

    font_length = to_small_endian(pt_font->length);
    pt_font = (const sized_font_t *)(((const uint8_t *)pt_font) + font_length);
    }

  return s_ok;
  }

static result_t get_byte_fontrec(handle_t parg, uint32_t offset, uint8_t *data)
  {
  fontrec_t *fr = (fontrec_t *)parg;

  if (offset >= (fr->length - sizeof(fontrec_t)))
    return e_bad_pointer;

  *data = ((const uint8_t *)&fr->fonts[0])[offset];
  return s_ok;
  }

static result_t set_byte_fontrec(handle_t parg, uint32_t offset, uint8_t data)
  {
  fontrec_t *fr = (fontrec_t *)parg;

  if (offset >= (fr->length - sizeof(fontrec_t)))
    return e_bad_pointer;

  ((uint8_t *)&fr->fonts[0])[offset] = data;
  return s_ok;
  }

result_t load_font(handle_t stream)
  {
  result_t result;

  if (stream == 0)
    return e_bad_parameter;

  handle_t hscreen;
  get_screen(&hscreen);

  screen_t *phys_screen;
  as_screen(hscreen, &phys_screen);

  uint16_t count;         // will never be many fonts, could convert to a nv map.
  if (failed(result = vector_count(phys_screen->fonts, &count)))
    return result;

  // determine the details
  fontrec_t *fontrec = (fontrec_t *)neutron_malloc(sizeof(fontrec_t));
  uint16_t read = 0;
  if (failed(result = stream_read(stream, fontrec, sizeof(fontrec_t), &read)) ||
    read != sizeof(fontrec_t))
    {
    neutron_free(fontrec);
    return e_bad_parameter;
    }

  bool compressed = fontrec->version == make_type('C', 'F', 'N', 'T');

  if (!compressed)
    {
    if (fontrec->version != make_type('F', 'O', 'N', 'T'))
      {
      neutron_free(fontrec);
      return e_bad_parameter;
      }
    }

  font_t *fr;
  uint16_t i;
  for (i = 0; i < count; i++)
    {
    if (failed(result = vector_at(phys_screen->fonts, i, &fr)))
      return result;

    // don't load if the font exists
    if (strcmp(fr->fontrec->name, fontrec->name) == 0)
      {
      neutron_free(fr);
      return e_exists;      // TODO: allow adding point sizes?
      }
    }

  // allocate a buffer large enough for the font.
  fontrec = (fontrec_t *)neutron_realloc(fontrec, fontrec->length);
  if (fontrec == 0)
    return e_not_enough_memory;

  uint32_t length = 0;

  if (compressed)
    result = decompress(stream, fontrec, get_byte_fontrec, set_byte_fontrec, &length);
  else
    {
    uint16_t read = 0;
    result = stream_read(stream, fontrec->fonts, fontrec->length - sizeof(fontrec_t), &read);
    if (read != fontrec->length - sizeof(fontrec_t))
      result = e_bad_parameter;
    }

  if (failed(result))
    {
    neutron_free(fontrec);
    return result;
    }

  // all done, we can now register the font
  if (failed(result = register_font((const uint8_t *)fontrec, fontrec->length)))
    neutron_free(fontrec);

  return result;
  }

static inline result_t is_valid_font(handle_t hndl, sized_font_t **font)
  {
  if(hndl == 0 || font == 0)
    return e_bad_parameter;

  sized_font_t *f = (sized_font_t *)hndl;

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

    const glyph_t *glyph = get_glyph(font, c);
    if (glyph == 0)
      return e_not_found;

    // see if the user wants the widths returned
    if (char_widths != 0)
      char_widths[ch] = glyph->advance;

    point_t bitmap_origin;
    bitmap_origin.x = glyph->offset + pt_pos.x;
    bitmap_origin.y = pt_pos.y;

    // add the baseline
    bitmap_origin.y += font->baseline;      // takes to the glyph base reference
    bitmap_origin.y -= glyph->baseline;

    uint8_t row;
    uint16_t offset = 0;
    uint16_t stride = (((glyph->width - 1) | 7) + 1) >> 3;
    for (row = 0; row < glyph->height; row++)
      {
      // set to the top of the bitmap
      uint8_t col;
      for (col = 0; col < glyph->width; )
        {
        point_t pos;
        pos.x = (gdi_dim_t)(bitmap_origin.x + col);

        // position the bitmap relative to the 
        pos.y = (gdi_dim_t)(bitmap_origin.y + row);
        uint8_t bitmap_bits = glyph->pixels[(col >> 3) + offset];

        uint8_t bit;
        for (bit = 8; bit > 0; bit--)
          {
          if (point_in_rect(&pos, clip_rect))
            {
            // pick up the column byte
            uint8_t alpha_pel = ((bitmap_bits >> (bit - 1)) & 0x01) ? 255 : 0;
            color_t bg_color;

            if(alpha_pel == 255)
              (*canvas->set_pixel)(canvas, &pos, fg);
            else if (alpha_pel > 0)
              {
              if (format & eto_opaque)
                bg_color = bg;
              else
                bg_color = (*canvas->get_pixel)(canvas, &pos);

              (*canvas->set_pixel)(canvas, &pos, aplha_blend(fg, bg_color, alpha_pel));
              }
            else if (format & eto_opaque)
              (*canvas->set_pixel)(canvas, &pos, bg);
            }

          pos.x++;

          col++;
          if (col >= glyph->width)
            break;
          }
        }

      offset += stride;
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


    const glyph_t *glyph = get_glyph(fh, c);

    if(glyph == 0)
      return e_not_found;

    // see if the user wants the widths returned
    ex->dx += glyph->advance;
    }

  return s_ok;
  }
