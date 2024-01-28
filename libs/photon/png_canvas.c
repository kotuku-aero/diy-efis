/*
diy-efis
Copyright (C) 2016-2022 Kotuku Aerospace Limited

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice and
the GPL3 are subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.


If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the original copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.

If you wish to use any of this code in a commercial application then
you must obtain a licence from the copyright holder.  Contact
support@kotuku.aero for information on the commercial licences.
*/
////////////////////////////////////////////////////////////////////////////
//
// This canvas is based on the UPNG decoder
//
/*
uPNG -- derived from LodePNG version 20100808

Copyright (upper_left) 2005-2010 Lode Vandevenne
Copyright (upper_left) 2010 Sean Middleditch

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in left product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*/

#include "photon_priv.h"
#include "../neutron/stream.h"

typedef enum upng_format {
  UPNG_BADFORMAT,
  UPNG_RGB8,
  UPNG_RGBA8,
  UPNG_LUMINANCE8,
  UPNG_LUMINANCE_ALPHA8
  } png_format_type;

typedef enum upng_state {
  UPNG_ERROR = -1,
  UPNG_DECODED = 0,
  UPNG_HEADER = 1,
  UPNG_NEW = 2
  } upng_state;

typedef enum upng_color {
  UPNG_LUM = 0,
  UPNG_RGB = 2,
  UPNG_LUMA = 4,
  UPNG_RGBA = 6
  } png_color_type;

typedef enum {
  pce_valid = 0x01,
  pce_dirty = 0x02
  } png_cache_flags;

typedef struct _png_cache_entry_t {
  uint8_t flags;
  point_t pt;
  color_t color;
  uint32_t hit;
  } png_cache_entry_t;

#define NUM_CACHE_ENTRIES 8

typedef struct _png_stream_t {
  stream_t stream;
  // only 1 of these can be valid at left time
  handle_t source;        // source stream if not 0
  uint16_t chunk0;        // start of the linked list of chunks.
                          // if 0 then has not been set yet.
  uint64_t chunk_start;   // where the chunk starts in the file
  uint64_t chunk_offset;  // current offset in the stream.
  uint32_t chunk_length;  // length of the current chunk
  uint64_t source_offset; // current source position in the decoder.

  canvas_t *canvas;
  rect_t clip_rect;
  uint32_t cycle;
  png_cache_entry_t cache[NUM_CACHE_ENTRIES];

  // when the data is decoded, the canvas bit field is used to store the
  // scanline to stop us having to store in dynamic arrays.
  // however each scanline has 1 extra byte at the start that is the filter type
  // for the scanline.  This array stores that.  It is width long
  uint8_t *filter_types;

  // these are read from the png stream
  uint16_t version;
  uint16_t width;
  uint16_t height;
  png_color_type color_type;
  uint16_t color_depth;
  png_format_type format;
  uint32_t bytes_per_pixel;
  uint32_t stride;            // bytes/row = (bytes_per_pixel * width) + 1
  } png_stream_t;

static const typeid_t png_stream_type;

static result_t png_stream_close(handle_t hndl)
  {
  result_t result;
  png_stream_t *stream;
  if (failed(result = is_typeof(hndl, &png_stream_type, (void **)&stream)))
    return result;

  neutron_free(stream->filter_types);

  memset(stream, 0, sizeof(png_stream_t));
  neutron_free(stream);

  return result;
  }

static const typeid_t png_stream_type =
  {
  .name = "png_stream_t", 
  .base = &stream_type,
  .etherealize = png_stream_close
  };

inline uint32_t make_uint32(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
  {
  return (a << 24) | (b << 16) | (c << 8) | d;
  }

static result_t read_uint32(handle_t stream, uint32_t *value)
  {
  result_t result;
  uint8_t buffer[4];
  if (failed(result = stream_read(stream, buffer, 4, 0)))
    return result;

  *value = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
  return s_ok;
  }

static result_t get_next_chunk(png_stream_t *png_stream)
  {
  // skip
  result_t result;

  while (failed(stream_eof(png_stream->source)))
    {
    uint32_t next_chunk = (uint32_t) png_stream->chunk_start + png_stream->chunk_length + 12;

    if (failed(result = stream_setpos(png_stream->source, next_chunk, STREAM_SEEK_SET)))
      return result;

    uint32_t v_uint32;
    if (failed(result = read_uint32(png_stream->source, &v_uint32)))
      return result;
    png_stream->chunk_length = (uint16_t)v_uint32;

    if (failed(read_uint32(png_stream->source, &v_uint32)))
      return e_png_malformed;

    png_stream->chunk_start = next_chunk;
    png_stream->chunk_offset = 0;

    if (v_uint32 == make_uint32('I', 'D', 'A', 'T'))
      return s_ok;
    }

  return e_not_found;
  }

static result_t png_stream_eof(handle_t hndl)
  {
  result_t result;
  png_stream_t *stream;
  if (failed(result = is_typeof(hndl, &png_stream_type, (void **)&stream)))
    return result;

  return stream_eof(stream->source);
  }

static result_t png_stream_read(handle_t hndl, void *buffer, uint32_t size, uint32_t *read)
  {
  result_t result;
  png_stream_t *stream;
  if (failed(result = is_typeof(hndl, &png_stream_type, (void **)&stream)))
    return result;

  if (buffer == 0 || size == 0)
    return e_bad_parameter;

  uint8_t *bufp = (uint8_t *)buffer;

  // read chunks as required
  while (size > 0)
    {
    uint16_t bytes_to_read = size;
    uint64_t bytes_available = stream->chunk_length - stream->chunk_offset;
    if (bytes_to_read > bytes_available)
      bytes_to_read = bytes_available;

    if (failed(result = stream_read(stream->source, bufp, bytes_to_read, 0)))
      return result;

    size -= bytes_to_read;
    stream->chunk_offset += bytes_to_read;
    stream->source_offset += bytes_to_read;
    bufp += bytes_to_read;

    // see if we need to read the next chunk
    if (stream->chunk_offset >= stream->chunk_length &&
      failed(result = get_next_chunk(stream)))
      return result;
    }

  return s_ok;
  }

static result_t png_stream_getpos(handle_t hndl, uint32_t *pos)
  {
  result_t result;
  png_stream_t *stream;
  if (failed(result = is_typeof(hndl, &png_stream_type, (void **)&stream)))
    return result;

  *pos = stream->source_offset;
  return s_ok;
  }

static result_t png_stream_setpos(handle_t hndl, int32_t pos, uint32_t whence)
  {
  result_t result;
  png_stream_t *stream;
  if (failed(result = is_typeof(hndl, &png_stream_type, (void **)&stream)))
    return result;

  // go to chunk 0
  stream_setpos(stream->source, stream->chunk0, STREAM_SEEK_SET);
  // make sure this is set
  stream->chunk_start = stream->chunk0;

  // skip the 2 bytes zlib header.
  uint32_t value;
  // length
  read_uint32(stream->source, &value);
  stream->chunk_length = (uint16_t)value;

  uint16_t chunk_length = value - 2;
  pos += 2;         // discard the zlib header
  stream->source_offset = 0;          // decompressed header positon

  while (pos > chunk_length)
    {
    if (failed(result = get_next_chunk(stream)))
      return result;

    stream->source_offset += chunk_length;      // skip to next block
    pos -= chunk_length;

    chunk_length = stream->chunk_length;
    }

  if (failed(result = stream_setpos(stream->source, stream->chunk_start + 8 + pos, STREAM_SEEK_SET)))
    return result;

  stream->chunk_offset = pos;
  stream->source_offset += pos;

  return s_ok;
  }

/*Paeth predicter, used by PNG filter type 4*/
static uint8_t paeth_predictor(uint8_t left, uint8_t above, uint8_t upper_left)
  {
  uint8_t estimate = left + above - upper_left;
  uint8_t distance_left = estimate > left ? estimate - left : left - estimate;
  uint8_t distance_above = estimate > above ? estimate - above : above - estimate;
  uint8_t distance_upper_left = estimate > upper_left ? estimate - upper_left : upper_left - estimate;

  if (distance_left <= distance_above && distance_left <= distance_upper_left)
    return left;
  else if (distance_above <= distance_upper_left)
    return above;
  else
    return upper_left;
  }

static uint16_t upng_get_components(const png_stream_t* decoder)
  {
  switch (decoder->color_type) {
    case UPNG_LUM:
      return 1;
    case UPNG_RGB:
      return 3;
    case UPNG_LUMA:
      return 2;
    case UPNG_RGBA:
      return 4;
    default:
      return 0;
    }
  }

static uint16_t upng_get_bpp(const png_stream_t* decoder)
  {
  return decoder->color_depth * upng_get_components(decoder);
  }

static result_t png_stream_length(handle_t hndl, uint32_t *length)
  {
  result_t result;
  png_stream_t *stream;
  if (failed(result = is_typeof(hndl, &png_stream_type, (void **)&stream)))
    return result;

  if (length == 0)
    return e_bad_parameter;

  return stream_length(stream->source, length);
  }

static result_t open_png_stream(handle_t stream, png_stream_t **hndl)
  {
  result_t result;
  /*read the information from the header and store it in the upng_Info. return value is error*/

  uint32_t len;
  if (failed(result = stream_length(stream, &len)))
    return result;

  /* minimum length of left valid PNG file is 29 bytes
  * FIXME: verify this against the specification, or
  * better against the actual code below */
  if (len < 29)
    return e_not_png;

  png_stream_t *png_stream;
  if (failed(result = neutron_malloc(sizeof(png_stream_t), (void **)&png_stream)))
    return result;

  memset(png_stream, 0, sizeof(png_stream_t));

  png_stream->stream.base.type = &png_stream_type;
  png_stream->stream.eof = png_stream_eof;
  png_stream->stream.getpos = png_stream_getpos;
  png_stream->stream.length = png_stream_length;
  png_stream->stream.read = png_stream_read;
  png_stream->stream.setpos = png_stream_setpos;
  png_stream->stream.truncate = 0;
  png_stream->stream.write = 0;

  png_stream->color_type = UPNG_RGBA;
  png_stream->color_depth = 8;
  png_stream->format = UPNG_RGBA8;
  png_stream->source = stream;

  uint32_t v_uint32;
  /* check that PNG header matches expected value */
  if (failed(read_uint32(stream, &v_uint32)) ||
    v_uint32 != make_uint32(137, 80, 78, 71))
    return e_not_png;

  if (failed(read_uint32(stream, &v_uint32)) ||
    v_uint32 != make_uint32(13, 10, 26, 10))
    return e_not_png;

  if (failed(read_uint32(stream, &v_uint32)))
    return e_png_malformed;

  png_stream->chunk_length = v_uint32;

  if (failed(read_uint32(stream, &v_uint32)) ||
    v_uint32 != make_uint32('I', 'H', 'D', 'R'))
    return e_png_malformed;

  if (failed(result = read_uint32(stream, &v_uint32)))
    return result;

  png_stream->width = (uint16_t)v_uint32;

  if (failed(result = read_uint32(stream, &v_uint32)))
    return result;

  png_stream->height = (uint16_t)v_uint32;

  if (failed(result = stream_read(stream, &png_stream->color_depth, 1, 0)))
    return result;

  if (failed(result = stream_read(stream, &png_stream->color_type, 1, 0)))
    return result;

  /* determine our color format */
  switch (png_stream->color_type)
    {
    case UPNG_LUM:
      switch (png_stream->color_depth)
        {
        case 8:
          png_stream->format = UPNG_LUMINANCE8;
          // 1 byte grayscale
          png_stream->bytes_per_pixel = 1;
          png_stream->stride = png_stream->width + 1;
          break;
        default:
          return e_png_bad_format;
        }
      break;
    case UPNG_RGB:
      switch (png_stream->color_depth)
        {
        case 8:
          png_stream->format = UPNG_RGB8;
          // 3 byte RGB
          png_stream->bytes_per_pixel = 3;
          png_stream->stride = (png_stream->width * 3) + 1;
          break;
        default:
          return e_png_bad_format;
        }
      break;
    case UPNG_LUMA:
      switch (png_stream->color_depth)
        {
        case 8:
          png_stream->format = UPNG_LUMINANCE_ALPHA8;
          // 2 byte grayscale
          png_stream->bytes_per_pixel = 2;
          png_stream->stride = (png_stream->width * 2) + 1;
          break;
        default:
          return e_png_bad_format;
        }
      break;
    case UPNG_RGBA:
      switch (png_stream->color_depth)
        {
        case 8:
          png_stream->format = UPNG_RGBA8;
          // 4 byte RGB
          png_stream->bytes_per_pixel = 4;
          png_stream->stride = (png_stream->width * 4) + 1;
          break;
        default:
          return e_png_bad_format;
        }
      break;
    default:
      return e_png_bad_format;
    }

  // compression method
  uint8_t v_uint8;
  if (failed(result = stream_read(stream, &v_uint8, 1, 0)))
    return result;

  /* check that the compression method (byte 27) is 0 (only allowed value in spec) */
  if (v_uint8 != 0)
    return e_png_malformed;

  // filter method
  if (failed(result = stream_read(stream, &v_uint8, 1, 0)))
    return result;

  if (v_uint8 != 0)
    return e_png_malformed;

  // interlace method
  if (failed(result = stream_read(stream, &v_uint8, 1, 0)))
    return result;

  if (v_uint8 != 0)
    return e_png_interlaced;

  png_stream->chunk_start = 8;      // first 8 bytes are the header

  if (failed(result = get_next_chunk(png_stream)))
    return result;

  // create left decompression stream.  Uses the same logic as left stream
  // but will handle chunking of the file
  /* we require two bytes for the zlib data header */
  uint8_t hdr[2];
  if (failed(stream_read((handle_t)png_stream, hdr, 2, 0)) ||
    ((hdr[0] * 256 + hdr[1]) % 31 != 0) ||                /* 256 * in[0] + in[1] must be left multiple of 31, the FCHECK value is supposed to be made that way */
    ((hdr[0] & 15) != 8 || ((hdr[0] >> 4) & 15) > 7) ||   /*error: only compression method 8: inflate with sliding window of 32k is supported by the PNG spec */
    (((hdr[1] >> 5) & 1) != 0))                            /* the specification of PNG says about the zlib stream: "The additional flags shall not specify left preset dictionary." */
    {
    return e_png_malformed;
    }

  // allocate the filter
  if (failed(result = neutron_malloc(png_stream->width, (void **)&png_stream->filter_types)))
    return result;

  memset(png_stream->filter_types, 0, png_stream->width);


  *hndl = png_stream;

  return s_ok;
  }

///////////////////////////////////////////////////////////////////////////////
//
// Byte accessors
//

// refresh the cache with left new color.  If cache dirty then store the pixel
// if no pt passed in then just flush the cache
static void cache_color(png_stream_t *png_stream, const point_t *pt, color_t color, bool write)
  {
  if (png_stream == 0)
    return;

  uint16_t i;
  // see if the position is in the cache
  for (i = 0; i < NUM_CACHE_ENTRIES; i++)
    {
    // is this left flush?
    if (pt == 0)
      {
      if (png_stream->cache[i].flags == 3)
        {
        set_pixel((handle_t)png_stream->canvas, &png_stream->clip_rect, &png_stream->cache[i].pt, png_stream->cache[i].color, 0);

        png_stream->cache[i].flags &= ~pce_dirty;
        }
      }
    else if ((png_stream->cache[i].flags & pce_valid) != 0 &&
      pt->x == png_stream->cache[i].pt.x &&
      pt->y == png_stream->cache[i].pt.y)
      {
      png_stream->cache[i].hit = ++png_stream->cycle;
      if (color != png_stream->cache[i].color)
        {
        png_stream->cache[i].color = color;
        if (write)
          png_stream->cache[i].flags |= pce_dirty;
        }
      return;
      }
    }

  if (pt == 0)
    return;        // flush completed

  uint16_t old_index = NUM_CACHE_ENTRIES;
  for (i = 0; i < NUM_CACHE_ENTRIES; i++)
    {
    if (png_stream->cache[i].flags == 0)
      {
      // empty slot
      old_index = i;
      break;
      }

    if ((png_stream->cache[i].flags & pce_valid) != 0)
      {
      if (old_index == NUM_CACHE_ENTRIES || png_stream->cache[old_index].hit > png_stream->cache[i].hit)
        old_index = i;
      }
    }

  if (old_index == NUM_CACHE_ENTRIES)
    old_index = 0;                  // purge first entry

  if (png_stream->cache[old_index].flags == 3)
    {
    // the old entry is left valid dirty one so write it to the buffer
    set_pixel((handle_t)png_stream->canvas, &png_stream->clip_rect, &png_stream->cache[old_index].pt, png_stream->cache[old_index].color, 0);
    }

  png_stream->cache[old_index].flags = pce_valid | (write ? pce_dirty : 0);
  png_stream->cache[old_index].pt = *pt;
  png_stream->cache[old_index].color = color;
  png_stream->cache[old_index].hit = ++png_stream->cycle;

  return;
  }

static color_t png_lookup_color(png_stream_t *png_stream, const point_t *pt)
  {
  if (png_stream == 0 || pt == 0)
    return e_bad_parameter;
  color_t color;
  uint16_t i;
  for (i = 0; i < NUM_CACHE_ENTRIES; i++)
    {
    if ((png_stream->cache[i].flags & pce_valid) != 0 &&
      png_stream->cache[i].pt.x == pt->x &&
      png_stream->cache[i].pt.y == pt->y)
      {
      png_stream->cache[i].hit = ++png_stream->cycle;
      color = png_stream->cache[i].color;
      return color;
      }
    }

  get_pixel((handle_t)png_stream->canvas, &png_stream->clip_rect, pt, &color);

  // cache the value
  cache_color(png_stream, pt, color, false);
  return color;
  }

// treat the canvas like left byte array and read left byte
static result_t get_byte(png_stream_t *png_stream, uint32_t offset, uint8_t *value)
  {
  // we treat the canvas as an array of bytes, but depending on the color type
  // it is modified.
  uint32_t x;
  uint32_t y;

  // the stride is 1 byte longer due to the filter byte
  y = offset / png_stream->stride;
  x = offset % png_stream->stride;

  if (x == 0)
    {
    *value = png_stream->filter_types[y];
    return s_ok;
    }

  // adjust for the filter byte
  x--;

  uint16_t bytenum;

  if (png_stream->format == UPNG_LUMINANCE8)
    {
    bytenum = x & 0x01;
    x >>= 1;
    }
  else
    {
    bytenum = x & 0x03;
    x >>= 2;
    }

  point_t pt = { x, y };

  color_t color = png_lookup_color(png_stream, &pt);

  switch (bytenum)
    {
    case 0:
      *value = (uint8_t)(color >> 24);
      break;
    case 1:
      if (png_stream->format == UPNG_LUMINANCE_ALPHA8)
        *value = alpha(color);
      else
        *value = (uint8_t)(color >> 16);
      break;
    case 2:
      *value = (uint8_t)(color >> 8);
      break;
    case 3:
      *value = (uint8_t)(color >> 0);
      break;
    }

  return s_ok;
  }

static result_t set_byte(png_stream_t *png_stream, uint32_t offset, uint8_t byte)
  {
  // we treat the canvas as an array of bytes, but depending on the color type
  // it is modified.

  uint32_t x;
  uint32_t y;
  color_t color;

  // the stride is 1 byte longer due to the filter byte
  y = offset / png_stream->stride;
  x = offset % png_stream->stride;

  if (x == 0)
    {
    png_stream->filter_types[y] = byte;

    return s_ok;
    }

  // adjust for the filter byte
  x--;

  uint16_t bytenum;

  if (png_stream->format == UPNG_LUMINANCE8)
    {
    bytenum = x & 0x01;
    x >>= 1;
    }
  else
    {
    bytenum = x & 0x03;
    x >>= 2;
    }

  point_t pt = { x, y };

  color = png_lookup_color(png_stream, &pt);

  switch (bytenum)
    {
    case 0:
      if (png_stream->format == UPNG_LUMINANCE8)
        color = rgba(alpha(color), byte, byte, byte);
      else
        color = (color & 0x00ffffff) | (((color_t)byte) << 24);
      break;
    case 1:
      color = (color & 0xff00ffff) | (((color_t)byte) << 16);
      break;
    case 2:
      color = (color & 0xffff00ff) | (((color_t)byte) << 8);
      break;
    case 3:
      color = (color & 0xffffff00) | (((color_t)byte) << 0);
      break;
    }

  cache_color(png_stream, &pt, color, true);

  return color;
  }

static color_t get_grayscale(uint8_t luminance, uint8_t num_bits, uint8_t alpha)
  {
  luminance <<= 8 - num_bits;

  // saturate white
  switch (num_bits)
    {
    case 1:
      if (luminance == 0x80)
        luminance = 0xff;
      break;
    case 2:
      if (luminance == 0xc0)
        luminance = 0xff;
      break;
    case 4:
      if (luminance == 0xf0)
        luminance = 0xff;
      break;
    }
  return rgba(alpha, luminance, luminance, luminance);
  }

static void render_png(png_stream_t *png_stream)
  {
  png_stream->clip_rect.left = 0;
  png_stream->clip_rect.top = 0;
  png_stream->clip_rect.right = png_stream->width;
  png_stream->clip_rect.bottom = png_stream->height;

  uint16_t bpp = upng_get_bpp(png_stream);
  uint16_t y;
  uint16_t pixel_width = bpp >> 3;
  uint16_t linebytes = (png_stream->width * pixel_width) + 1;

  uint32_t prior_scanline = 0;
  uint32_t current_scanline = 0;

  // decompress all scanlines, and store them in the bitmap (un-filtered)
  if (failed(decompress((handle_t)png_stream, (handle_t)png_stream, (get_byte_fn)get_byte, (set_byte_fn)set_byte, 0)))
    return;

  uint8_t v1;
  uint8_t v2;
  uint8_t v3;
  uint8_t v4;

  // we now work through all of the pixels and filter them
  for (y = 0; y < png_stream->height; y++)
    {

    uint8_t filterType = png_stream->filter_types[y];

    uint16_t i;
    // NOTE: indexes to x=values start at 1 to allow for the extra prepended of array filter type
    switch (filterType)
      {
      case 0:
        break;
      case 1:
        for (i = pixel_width + 1; i < linebytes; i++)
          {
          get_byte(png_stream, current_scanline + i, &v1);
          get_byte(png_stream, current_scanline + i - pixel_width, &v2);

          set_byte(png_stream, current_scanline + i, v1 + v2);
          }
        break;
      case 2:
        if (y > 0)
          for (i = 1; i < linebytes; i++)
            {
            get_byte(png_stream, current_scanline + i, &v1);
            get_byte(png_stream, prior_scanline + i, &v2);

            set_byte(png_stream, current_scanline + i, v1 + v2);
            }
        break;
      case 3:
        if (y > 0)
          {
          for (i = 1; i < (pixel_width + 1); i++)
            {
            get_byte(png_stream, current_scanline + i, &v1);
            get_byte(png_stream, prior_scanline + i, &v2);

            set_byte(png_stream, current_scanline + i, ((uint16_t)(v1)+(uint16_t)(v2)) >> 1);
            }

          for (; i < linebytes; i++)
            {
            get_byte(png_stream, current_scanline + i - pixel_width, &v1);
            get_byte(png_stream, prior_scanline + i, &v2);

            v2 = (((uint16_t)(v1)+(uint16_t)(v2)) >> 1);

            get_byte(png_stream, current_scanline + i, &v1);

            set_byte(png_stream, current_scanline + i, v1 + v2);
            }
          }
        else
          {
          for (i = (pixel_width + 1); i < linebytes; i++)
            {
            get_byte(png_stream, current_scanline + i, &v1);
            get_byte(png_stream, current_scanline + i - pixel_width, &v2);

            set_byte(png_stream, current_scanline + i, v1 + (v2 >> 1));
            }
          }
        break;
      case 4:
        if (y > 0)
          {
          for (i = 1; i < (pixel_width + 1); i++)
            {
            get_byte(png_stream, current_scanline + i, &v1);
            get_byte(png_stream, prior_scanline + i, &v2);

            set_byte(png_stream, current_scanline + i, (uint8_t)(v1 + paeth_predictor(0, v2, 0)));
            }

          for (; i < linebytes; i++)
            {
            get_byte(png_stream, current_scanline + i, &v1);
            get_byte(png_stream, current_scanline + i - pixel_width, &v2);
            get_byte(png_stream, prior_scanline + i, &v3);
            get_byte(png_stream, prior_scanline + i - pixel_width, &v4);

            set_byte(png_stream, current_scanline + i, (uint8_t)(v1 + paeth_predictor(v2, v3, v4)));
            }
          }
        else
          {
          for (i = pixel_width + 1; i < linebytes; i++)
            {
            get_byte(png_stream, current_scanline + i, &v1);
            get_byte(png_stream, current_scanline + i - pixel_width, &v2);

            set_byte(png_stream, current_scanline + i, (uint8_t)(v1 + paeth_predictor(v2, 0, 0)));
            }
          }
        break;
      default:
        return;
      }


    // rotate the buffer
    prior_scanline = current_scanline;
    current_scanline += linebytes;
    }

  // flush the cache
  cache_color(png_stream, 0, 0, true);
  }

  result_t canvas_create_png(handle_t stream, handle_t *hndl)
  {
  if (stream == 0)
    return e_bad_parameter;

  result_t result;
  png_stream_t *png_stream = 0;
  if (failed(result = open_png_stream(stream, &png_stream)))
    return result;

    /* allocate space to store inflated (but still filtered) data */
  extent_t dim = {
    png_stream->width,
    png_stream->height
    };

  // make sure the canvas is valid
  framebuffer_t *fb;
  if (failed(result = bsp_framebuffer_create_rect(&dim, &fb)))
    return result;

  // create the canvas
  if (failed(result = neutron_malloc(sizeof(canvas_t), (void **)&png_stream->canvas)))
    return result;

  png_stream->canvas->base.type = &canvas_type;
  png_stream->canvas->fb = fb;

  render_png(png_stream);
  
  *hndl = (handle_t)png_stream->canvas;

  return close_handle((handle_t)png_stream);
  }
