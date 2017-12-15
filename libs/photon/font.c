#include "window.h"
#include "font.h"

static int compare_font(const void *left, const void *right)
  {
  char fl = (char)left;
  char fr = (char)right;

  return (fl > fr) ? 1 : (fl < fr) ? -1 : 0;
  }

static void dup_font(const void *src, void **dst)
  {
  *dst = (void *)src;       // stored by reference
  }

static void destroy_key_font(void *key)
  {
  // do not destroy the key!
  }

static void destroy_value_font(void *value)
  {
  neutron_free(value);
  }


// cache (if possible) the character ch in th font
result_t ensure_glyph(font_t *font, char ch, const glyph_t **gp)
  {
  result_t result;
  glyph_t *glyph = 0;

  if (font == 0)
    return e_bad_parameter;

  result = map_find(font->glyphs, (const void *)ch, (void **)&glyph);
  if (result != e_not_found && failed(result))
    return result;

  if (succeeded(result))
    {
    if (gp != 0)
      *gp = glyph;

    return s_ok;
    }

  handle_t hscreen;
  get_screen(&hscreen);

  screen_t *phys_screen;
  as_screen(hscreen, &phys_screen);

  if(failed(result = create_scaled_glyph(&font->fontrec->info, font->size, ch, &glyph)))
    return result;

  if (gp != 0)
    *gp = glyph;

  // add the glyph
  return map_add(font->glyphs, ch, glyph);
  }

static result_t release_glyph(glyph_t *glyph)
  {
  // just free it
  neutron_free(glyph);

  return s_ok;
  }

result_t create_font(const char *name, uint16_t points, const char *hint, handle_t  *fh)
  {
  result_t result;
  if (name == 0 || points == 0)
    return e_bad_parameter;

  handle_t hscreen;
  get_screen(&hscreen);

  screen_t *phys_screen;
  as_screen(hscreen, &phys_screen);

  fontrec_t *fr = 0;
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

  font_t *font = 0;
  for (font_index = 0; font_index < count; font_index++)
    {
    if (failed(result = vector_at(fr->fonts, font_index, &font)))
      return result;

    if (font->size == points)
      break;
    }

  if (font == 0)
    {
    // point length does not exist so create it
    font = (font_t *)neutron_malloc(sizeof(font_t));
    font->fontrec = fr;
    font->size = points;
    font->version = sizeof(font_t);
    if (failed(result = map_create(dup_font, dup_font, compare_font, destroy_key_font, destroy_value_font, &font->glyphs)))
      {
      neutron_free(font);
      return result;
      }

    if(failed(result = vector_push_back(fr->fonts, &font)))
      return result;
    }

  // the font is loaded, see if the user wants it
  if (fh != 0)
    *fh = font;

  // font is created.  So not init the cache
  if (hint != 0)
    {
    char ch;
    for (ch = *hint++; ch != 0; ch = *hint++)
      {
      if (failed(result = ensure_glyph(font, ch, 0)))
        return result;
      }
    }

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

  fontrec_t *fr;
  uint16_t count;         // will never be many fonts, could convert to a nv map.
  if (failed(result = vector_count(phys_screen->fonts, &count)))
    return result;

  uint16_t i;
  for (i = 0; i < count; i++)
    {
    if (failed(result = vector_at(phys_screen->fonts, i, &fr)))
      return result;

    if (strcmp(fr->name, name) == 0)
      return e_exists;
    }

  // font does not exist.
  fr = (fontrec_t *)neutron_malloc(sizeof(fontrec_t));
  memset(fr, 0, sizeof(fontrec_t));
  fr->file = stream;
  // create an empty font list
  if (failed(result = vector_create(sizeof(font_t), &fr->fonts)) ||
    failed(result = init_font(&fr->info, stream, 0)))
    {
    neutron_free(fr);
    return result;
    }

  fr->name = neutron_strdup(name);

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

  fontrec_t *fr = 0;
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

  if (failed(result = stream_close(fr->file)))
    {
    fr->file = 0;
    return result;
    }

  // fontinfo is only stream...
  neutron_free((void *)fr->name);
  fr->name = 0;

  if (failed(result = vector_erase(phys_screen->fonts, font_index)))
    return result;

  neutron_free(fr);

  return s_ok;
  }
