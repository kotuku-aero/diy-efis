#include "photon_priv.h"

result_t draw_text(handle_t hndl, const rect_t *clip_rect, const font_t *font,
  color_t fg, color_t bg, uint16_t count, const char *str, const point_t *pt,
  const rect_t *txt_clip_rect, text_flags format, uint16_t *char_widths)
  {
  result_t result;
  canvas_t *canvas;
  if (failed(result = is_typeof(hndl, &canvas_type, (void **)&canvas)))
    return result;

  // create a clipping rectangle as needed
  rect_t txt_rect;
  point_t pt_pos =
    {
    pt->x,
    pt->y
    };

  if ((format & eto_clipped) != 0)
    rect_intersect(clip_rect, txt_clip_rect, &txt_rect);
  else
    rect_copy(clip_rect, &txt_rect);

  if (count == 0)
    count = (uint16_t) strlen(str);

  for (uint16_t ch = 0; ch < count; ch++)
    {
    // get the character to use.  If the character is outside the
    // map then we use the default char
    char c = str[ch];

    if(c < font->first_char || c > font->last_char)
      c = font->first_char;

    size_t c_offs = c - font->first_char;
    const uint8_t *glyph = font->bitmap_pointer + font->char_table[c_offs].offset;

    uint16_t advance = font->char_table[c_offs].width;
    // see if the user wants the widths returned
    if (char_widths != 0)
      char_widths[ch] = (uint16_t) advance;

    size_t stride = (((advance-1) |7) +1)>> 3;
    size_t row;
    for (row = 0; row < font->height; row++)
      {
      // set to the top of the bitmap
      size_t col;
      for (col = 0; col < advance; )
        {
        size_t byte = (col >> 3);
        point_t pos;
        pos.x = ((gdi_dim_t)(pt_pos.x + col));
        pos.y = ((gdi_dim_t)(pt_pos.y + row));

        uint8_t bitmap_bits = glyph[byte];

        size_t bit;
        for (bit = 8; bit > 0; bit--)
          {
          if (succeeded(rect_contains(clip_rect, &pos)))
            {
            // pick up the column byte
            if(((bitmap_bits >> (bit - 1)) & 0x01))
              (*canvas->fb->set_pixel)(canvas->fb, &pos, fg, 0);
            else if ((format & eto_opaque)!= 0)
              (*canvas->fb->set_pixel)(canvas->fb, &pos, bg, 0);
            }

          pos.x++;

          col++;
          if (col >= advance)
            break;
          }
        }

      // skip to next row
      glyph += stride;
      }

    pt_pos.x += (gdi_dim_t) advance;
    }

  return s_ok;
  }

result_t text_extent(const font_t* font, uint16_t count, const char *str, extent_t *ex)
  {
  if (str == 0 || ex == 0)
    return e_bad_pointer;

  if (count == 0)
    count = (uint16_t) strlen(str);

  ex->dx = 0;
  ex->dy =  font->height;

  for (uint16_t ch = 0; ch < count; ch++)
    {
    // get the character to use.  If the character is outside the
    // map then we use the default char
    char c = str[ch];
    if (c == 0)
      break;              // end of the string

    if (c < font->first_char || c > font->last_char)
      c = font->first_char;

    // see if the user wants the widths returned
    ex->dx += (gdi_dim_t)font->char_table[c - font->first_char].width;
    }

  return s_ok;
  }
