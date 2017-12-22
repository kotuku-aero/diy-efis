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

extern result_t polypolygon_impl(canvas_t *canvas, const rect_t *clip_rect, const pen_t *outline, color_t fill, uint16_t count, const uint16_t *lengths, const point_t *pts);
extern color_t aplha_blend(color_t pixel, color_t back, uint8_t weighting);

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

// this is a bitmap of alpha values.
typedef struct
  {
  uint16_t width;
  uint16_t height;
  uint8_t pixels[];
  } font_bitmap_t;

struct _rendered_glyph_cache_t;
// a glyph is stored as a vector of alpha values (255 = opaque, 0 = transparent)
// to allow alpha blended font outlines.
typedef struct _glyph_t {
  struct _rendered_glyph_cache_t *font_info;
  uint16_t wchar;         // unicode character
  uint16_t codepoint;     // offset into file
  gdi_dim_t advance;           // advance for the glyph
  gdi_dim_t baseline;         // baseline of the bitmap.
  font_bitmap_t bitmap;       // de-compressed bitmap
  } glyph_t;

typedef struct _rendered_glyph_cache_t {
  uint16_t version;
  uint16_t size;              // height of the font this bitmap renders
  gdi_dim_t vertical_height;  // height including ascender/descender
  gdi_dim_t baseline;         // where logical 0 is for the font outline.
  struct _font_t *fontrec;    // reference to the cache of fonts
  handle_t glyphs;            // map of characters to cached bitmap rects
  } sized_font_t;

typedef struct _font_info_t
  {
  handle_t stream;              // pointer to .ttf file
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
  } fontinfo_t;

// We cache the fonts into an array of fonts.
// this structure holds the name->font mapping
typedef struct _font_t {
  uint16_t version;       // sizeof(font_t)
  const char *name;       // registered name of the font
  handle_t fonts;         // font vector, these are the fonts once rendered
  handle_t shapes;        // map of codepoints to shape records
  fontinfo_t info;        // loaded font info
  } font_t;

uint16_t font_t_size = sizeof(font_t);

//////////////////////////////////////////////////////////////////////////
//
// accessors to parse stream from file
//
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

static void destroy_shape(void *value)
  {
  glyph_shape_t *shape = (glyph_shape_t *)value;
  uint16_t i;
  for(i = 0; i < shape->num_contours; i++)
    neutron_free(shape->contours[i]->vertices);

  neutron_free(shape);
  }

static void dup_shape(const void *src, void **dst)
  {
  *dst = *((glyph_shape_t **)src);
  }

result_t find_glyph_index(const fontinfo_t *info, uint16_t unicode_codepoint, uint16_t *glyph_offset)
  {
  uint32_t index_map = info->index_map;

  if (glyph_offset == 0)
    return e_bad_parameter;

  *glyph_offset = 0;

  uint16_t format = read_uint16_setpos(info->stream, index_map + 0);
  if (format == 0)
    { // apple byte encoding
    int32_t bytes = read_uint16_setpos(info->stream, index_map + 2);
    if (unicode_codepoint < bytes - 6)
      {
      *glyph_offset = read_uint8_setpos(info->stream, index_map + 6 + unicode_codepoint);
      return s_ok;
      }
    return e_not_found;
    }
  else if (format == 6)
    {
    uint32_t first = read_uint16_setpos(info->stream, index_map + 6);
    uint32_t count = read_uint16_setpos(info->stream, index_map + 8);
    if ((uint32_t)unicode_codepoint >= first && (uint32_t)unicode_codepoint < first + count)
      {
      *glyph_offset = read_uint16_setpos(info->stream, index_map + 10 + (unicode_codepoint - first) * 2);
      return s_ok;
      }
    return e_not_found;
    }
  else if (format == 2)
    {
    return e_not_found;
    }
  else if (format == 4)
    { // standard mapping for windows fonts: binary end_count_pos collection of ranges
    read_uint16(info->stream);      // length
    read_uint16(info->stream);      // version
    uint16_t seg_count = read_uint16(info->stream) >> 1;
    uint16_t search_range = read_uint16(info->stream) >> 1;
    uint16_t entry_selector = read_uint16(info->stream);
    uint16_t range_shift = read_uint16(info->stream) >> 1;

    // do a binary end_count_pos of the segments
    if (unicode_codepoint > 0xffff)
      return e_not_found;

    uint16_t end_count_pos;
    stream_getpos(info->stream, &end_count_pos);
    uint16_t start_count_pos = end_count_pos + (seg_count << 1) + 2;
    uint16_t delta_pos = start_count_pos + (seg_count << 1);
    uint16_t range_pos = delta_pos + (seg_count << 1);

    uint16_t i;
    for (i = 0; i < seg_count; i++)
      {
      if (unicode_codepoint <= read_uint16_setpos(info->stream, end_count_pos + (i << 1)) &&
        unicode_codepoint >= read_uint16_setpos(info->stream, start_count_pos + (i << 1)))
        {
        int16_t delta = read_int16_setpos(info->stream, delta_pos + (i << 1));
        unicode_codepoint += delta;
        *glyph_offset = unicode_codepoint;
        break;
        }
      }

    if (glyph_offset == 0)
      return e_not_found;

    return s_ok;
    }
  else if (format == 12 || format == 13)
    {
    uint32_t ngroups = read_uint32_setpos(info->stream, index_map + 12);
    int32_t low, high;
    low = 0; high = (int32_t)ngroups;
    // Binary end_count_pos the right group.
    while (low < high)
      {
      int32_t mid = low + ((high - low) >> 1); // rounds down, so low <= mid < high
      uint32_t start_char = read_uint32_setpos(info->stream, index_map + 16 + mid * 12);
      uint32_t end_char = read_uint32_setpos(info->stream, index_map + 16 + mid * 12 + 4);
      if ((uint32_t)unicode_codepoint < start_char)
        high = mid;
      else if ((uint32_t)unicode_codepoint > end_char)
        low = mid + 1;
      else
        {
        uint32_t start_glyph = read_uint32_setpos(info->stream, index_map + 16 + mid * 12 + 8);
        if (format == 12)
          *glyph_offset = start_glyph + unicode_codepoint - start_char;
        else // format == 13
          *glyph_offset = start_glyph;
        return s_ok;
        }
      }
    }

  return e_not_found;
  }

  static double scale_for_pixel_height(const fontinfo_t *info, double height)
    {
    double ascender = read_int16_setpos(info->stream, info->hhea + 4);
    double descender = read_int16_setpos(info->stream, info->hhea + 6);


    return (double)height / (ascender + descender);
    }

#define ON_CURVE 1
#define X_IS_BYTE 2
#define Y_IS_BYTE 4
#define REPEAT 8
#define X_DELTA 16
#define Y_DELTA 32

static void read_coord(bool is_y, uint8_t byte_flag, uint8_t delta_flag, int16_t val_min,
  int16_t val_max, handle_t stream, glyph_contour_t *contour, int16_t *value)
  {
  uint16_t index;

  for (index = 0; index < contour->num_vertices; index++)
    {
    if (contour->vertices[index].type & byte_flag)
      {
      uint8_t offs;
      stream_read(stream, &offs, 1, 0);
      if (contour->vertices[index].type & delta_flag)
        *value += offs;
      else
        *value -= offs;
      }
    else if ((contour->vertices[index].type & delta_flag) == 0)
      {
      int16_t offs = read_int16(stream);
      *value += offs;
      }

    if (is_y)
      contour->vertices[index].y = *value; // min(val_max, max(val_min, value));
    else
      contour->vertices[index].x = *value; // min(val_max, max(val_min, value));
    }
  }

  /*
  Description

  TrueType is a method of encoding font information into a file. It was
  created by Apple in the 1980s and is widely used today.
  TrueType defines each glyph (character shape) by using a series of
  straight lines and quadratic Bézier curves. This approach means that
  each character is a vector image and can be easily scaled up as required.
  The TrueType file format is highly-space efficient, but easy to read and
  uses two simple primtives to draw the individual character glyphs.
  The file format is big endian throughout.

  File Format

  The TypeType Font file contains a number of tables, the most significant of which are:
  'cmap' which maps individual character codes to glyph indices,
  'loca' which maps glyph indices to offsets into the main glyph table, and
  'glyf' which stores the actual glyph vector stream itself.

  Displaying a character

  The general order of operations to display a character is as follows:
  Find a suitable character map is the 'cmap' section of the file for the
  encoding of character code you have.
  Use the character map to map the character code to a Glyph Index.
  Lookup the Glyph index in the 'loca' table to find the offset into the
  glyph table where this glyph starts.
  Lookup the Glyph index + 1 in the 'loca' table to find the offset for
  the following glyph, this allows us to calculate the dlyph stream length.
  Locate the glyph stream and plot it.

  The Glyph Data In-Depth

  The glyph stream itself describes a set of coordinate points that define
  either straight lines or Bézier curves to draw the actual glyph image.
  It is described by a block of stream from the 'glyf' section of the file.
  The glyph stream contains:

  A count of the contours,
  The bounding box for the character stream,
  An array of 1-based point indices indicating the last point for each contour,
  Zero or more of "grid-fitting" hints,
  Flags defining attributes of points,
  X coordinate stream, and
  Y coordinate stream.

  Contours

  Contours are a collection of subsequent points that form a closed loop.
  Some characters have only one contour to draw them, others have more. Here are a few examples:
  '-' (hyphen) has only one contour, the path around the outside of the bar
  '1' (digit one) also has only one contour, around the outside of the shape
  'O' (capital oh) has two contours, one around the outside of the shape,
  and one around the inside
  '=' (equals) has two contours, the path around each of the bars
  '%' (percent) has five contours, the path around the bar and a path inside
  and outside each of the circles

  The glyph stream stores a count of the contours for the glyph and also an array of the
  last point for each contour. By reading the point index for the last contour,
  you can determine the number of points in total that make up this glyph.
  Note that the file does explicitly close a contour. For example, a square
  will be defined by four points and it is up to the drawing code to draw the
  fourth side of square by drawing from the fourth point back to the first point.

  "Grid-fitting" hints

  A glyph can optionally contain "grid-fitting" hints. These are instructions
  provided by the font designer to specify pixel level details that should be
  included when the font is rendered as a bitmap (rasterised). These are not
  covered by this article.

  Point Data

  The remainder of the glyph stream forms three byte arrays, the flags,
  the X coordinates, and the Y coordinates.
  The points of the file are defined by a series of flag bytes in the
  glyph stream, each flag represents one or more points in the glyph and as such
  the flags have to be parsed until the correct number of points has been found.
  This is the only way to determine how many bytes of flag stream is present in the file.
  Each flag byte indicates a number of details about the point or points that it represents:
  Whether the point is On Curve of Off Curve,
  Whether there are zero, one or two bytes of stream for the X coordinate,
  Whether there are zero, one or two bytes of stream for the Y coordinate, and
  How many points this flag represents.

  Drawing the Points

  The On Curve / Off Curve flag is used to identify if the particular point lies on the
  outline of the character, or is a control point for a Bézier curve. By analysing
  the on /off curve bit of the flag, the type of line required can be determined:
  OnCurve point to OnCurve point: This is a straight line segment.
  OnCurve, OffCurve, OnCurve: This is a quadratic Bézier.
  OnCurve, OffCurve, OffCurve, OnCurve: This is two quadratic Béziers.
  This last type of arrangement is a nuance of the file format. Whilst the file encodes
  four points in the form of "OnCurve, OffCurve, OffCurve, OnCurve", it actually represents
  five points in the form of "OnCurve, OffCurve, OnCurve, OffCurve, OnCurve".
  The extra OnCurve point is added in to form two quadratic Béziers. (Whilst I cannot find
  any definitive information on this, I believe the OnCurve point should be located half-way
  between the two OffCurve points.)
  */

static void store_vertex(glyph_contour_t *contour, uint16_t pos, glyph_vertex_t *vertex)
  {
  if(contour->x_min > vertex->x)
    contour->x_min = vertex->x;

  if(contour->x_max < vertex->x)
    contour->x_max = vertex->x;

  if(contour->y_min > vertex->y)
    contour->y_min = vertex->y;

  if(contour->y_max < vertex->y)
    contour->y_max = vertex->y;

  memcpy(&contour->vertices[pos], vertex, sizeof(glyph_vertex_t));
  }

static result_t get_glyph_shape(const fontinfo_t *font, uint16_t glyph_index, glyph_shape_t **hshape)
  {
  int num_vertices = 0;
  uint16_t offset = 0;
  uint16_t end_offset;

  if(glyph_index >= font->num_glyphs)
    return e_bad_parameter; // glyph index out of range
  if(font->index_to_location_format >= 2)
    return e_bad_handle; // unknown index->glyph map format

  if(font->index_to_location_format == 0)
    {
    offset = font->glyf + read_uint16_setpos(font->stream, font->loca + glyph_index * 2) * 2;
    end_offset = font->glyf + read_uint16_setpos(font->stream, font->loca + glyph_index * 2 + 2) * 2;
    }
  else
    {
    offset = font->glyf + read_uint32_setpos(font->stream, font->loca + glyph_index * 4);
    end_offset = font->glyf + read_uint32_setpos(font->stream, font->loca + glyph_index * 4 + 4);
    }

  int16_t number_of_contours;
  if(offset == end_offset)
    number_of_contours = 0;
  else
    number_of_contours = read_int16_setpos(font->stream, offset);

  // a glyph is an array of arrays of vertices describing the shape.
  glyph_shape_t *shape = (glyph_shape_t *)neutron_malloc(sizeof(glyph_shape_t) + (sizeof(glyph_vertex_t *) * number_of_contours));
  glyph_contour_t *contour;

  memset(shape, 0, sizeof(glyph_shape_t) + (sizeof(glyph_vertex_t *) * number_of_contours));
  shape->num_contours = number_of_contours;

  *hshape = shape;

  if(number_of_contours == 0)
    return s_ok;                  // space glyph

  // this will seek to the start of the contours
  shape->x_min = read_int16(font->stream);
  shape->y_min = read_int16(font->stream);
  shape->x_max = read_int16(font->stream);
  shape->y_max = read_int16(font->stream);

  // number of contours == -1 is a compound glyph (not supported)

  if (number_of_contours > 0)
    {
    uint8_t flags = 0;
    int32_t j = 0, was_off = 0, start_off = 0;
    int16_t i;
    uint16_t end_pt = 0;
    uint16_t start_pt = 0;

#if _DEBUG_FONTS
    trace_debug("Shape Index: %d, x_min:%d, x_max:%d, y_min:%d, y_max:%d\r\n",
      glyph_index, shape->x_min, shape->x_max, shape->y_min, shape->y_max);
#endif

    for (i = 0; i < number_of_contours; i++)
      {
      uint16_t end_flag = read_uint16(font->stream);
      uint16_t num_vertices = end_flag - start_pt + 1;

      shape->contours[i] = (glyph_contour_t *)neutron_malloc(sizeof(glyph_contour_t));
      memset(shape->contours[i], 0, sizeof(glyph_contour_t));
      shape->contours[i]->x_max = shape->x_min;
      shape->contours[i]->x_min = shape->x_max;
      shape->contours[i]->y_max = shape->y_min;
      shape->contours[i]->y_min = shape->y_max;

      shape->contours[i]->vertices = (glyph_vertex_t *)neutron_malloc((sizeof(glyph_vertex_t) * num_vertices));
      memset(shape->contours[i]->vertices, 0, (sizeof(glyph_vertex_t) * num_vertices));

      shape->contours[i]->num_vertices = num_vertices;
      shape->contours[i]->end_flag = end_flag;
      start_pt = end_flag + 1;
      }

    // skip the instructions
    uint16_t ins = read_uint16(font->stream);
    uint16_t pos;
    stream_getpos(font->stream, &pos);
    stream_setpos(font->stream, pos + ins);         // skip the instructions

    int16_t contour_num;
    ins = 0;
    // now we read the x and y stream and flags
    for (contour_num = 0; contour_num < number_of_contours; contour_num++)
      {
      contour = shape->contours[contour_num];
      uint16_t v = 0;
      for (; ins <= contour->end_flag; ins++)
        {
        uint8_t type; 
        stream_read(font->stream, &type, 1, 0);

        if ((type & REPEAT) != 0)
          {
          uint8_t rpt = 0;
          // ins += rpt;       // same flag for a while...
          stream_read(font->stream, &rpt, 1, 0);

          // this expands the array
          while (rpt--)
            {
            ins++;
            contour->vertices[v++].type = type;
            }
          }
        contour->vertices[v++].type = type;
        }
      }

    // re-read the contors now
    int16_t value = 0;
    for (contour_num = 0; contour_num < number_of_contours; contour_num++)
      {
      contour = shape->contours[contour_num];
      // now we read the x-coordinates
      read_coord(false, X_IS_BYTE, X_DELTA, shape->x_min, shape->x_max, font->stream, contour, &value);
      }

    value = 0;
    for(contour_num = 0; contour_num < number_of_contours; contour_num++)
      {
      contour = shape->contours[contour_num];
      read_coord(true, Y_IS_BYTE, Y_DELTA, shape->y_min, shape->y_max, font->stream, contour, &value);
      }

    for(contour_num = 0; contour_num < number_of_contours; contour_num++)
      {
      contour = shape->contours[contour_num];

#if _DEBUG_FONTS
      trace_debug("Contour %d, x_min:%d, x_max:%d, y_min:%d, y_max:%d\r\n",
        contour_num, contour->x_min, contour->x_max, contour->y_min, contour->y_max);
      for(i = 0; i < contour->num_vertices; i++)
        {
        trace_debug("Vertex %d : flags: %x, x:%d, y:%d\r\n", i, contour->vertices[i].type, contour->vertices[i].x, contour->vertices[i].y);
        }
#endif

      // we now walk the contour and convert to a ready to process form
      bool start_off = false;       // if the first point was an off-curve then this will be set
      bool was_off = false;
      int16_t start_x = 0;              // only used if first is a control point
      int16_t start_y = 0;
      uint16_t i = 0;
      uint16_t next_pt = 0;         // these are the actual points
      glyph_vertex_t curve_pt;      // the point on the curve
      memset(&curve_pt, 0, sizeof(glyph_vertex_t));

      glyph_vertex_t *vert = &contour->vertices[i++];

      if ((vert->type & ON_CURVE) == 0)
        {
        // first vertex is not on the curve.  copy some values
        start_x = vert->x;
        start_y = vert->y;
        start_off = true;

        // read the next point.
        vert = &contour->vertices[i++];

        if ((vert->type & ON_CURVE) == 0)
          {
          // next point is also a curve point, so interpolate an on-point curve
          curve_pt.x = (start_x + vert->x) >> 1;
          curve_pt.y = (start_y + vert->y) >> 1;
          curve_pt.cx = vert->x;
          curve_pt.cy = vert->y;
          curve_pt.type = contour_vcurve;
          }
        else
          {
          curve_pt.x = vert->x;
          curve_pt.y = vert->y;
          curve_pt.type = contour_vmove;
          }
        }
      else
        {
        // just move to the point
        curve_pt.x = vert->x;
        curve_pt.y = vert->y;
        curve_pt.type = contour_vmove;
        }

      // set the start point
      store_vertex(contour, next_pt++, &curve_pt);

      for (; i < contour->num_vertices; i++)
        {
        vert = &contour->vertices[i];

        if ((vert->type & ON_CURVE) == 0)
          {
          // its a curve rather than an on-curve point
          if (was_off)
            {
            // if the previous was off the curve then this is two beziers.
            // convert the first part to a curve, and insert the new point into the array
            curve_pt.x = (curve_pt.cx + vert->x) >> 1;
            curve_pt.y = (curve_pt.cy + vert->y) >> 1;
            curve_pt.cx = vert->x;
            curve_pt.cy = vert->y;
            curve_pt.type = contour_vcurve;
            store_vertex(contour, next_pt++, &curve_pt);

            was_off = false;
            }
          else
            {
            // previous was a point so we make this a curve
            was_off = true;
            next_pt--;
            curve_pt.cx = vert->x;
            curve_pt.cy = vert->y;
            curve_pt.type = contour_vcurve;
            store_vertex(contour, next_pt++, &curve_pt);
            }
          }
        else
          {
          memset(&curve_pt, 0, sizeof(glyph_vertex_t));
          was_off = false;
          curve_pt.x = vert->x;
          curve_pt.y = vert->y;
          curve_pt.type = contour_vline;

          store_vertex(contour, next_pt++, &curve_pt);
          }
        }

      // we now connect the line back to the start
      store_vertex(contour, next_pt++, &curve_pt);
      if(contour->num_vertices == next_pt)
        contour->vertices = (glyph_vertex_t *)neutron_realloc(contour->vertices, (next_pt+1) * sizeof(glyph_vertex_t));

      curve_pt.cx = start_x;
      curve_pt.cy = start_y;
      curve_pt.type = start_off ? contour_vcurve : contour_vline;
      store_vertex(contour, next_pt++, &curve_pt);
      contour->num_vertices = next_pt;
      }
    }
  else
    return e_bad_handle;

  return s_ok;
  }


// tesselate until threshhold p is happy... @TODO warped to compensate for non-linear stretching

static result_t tesselate_curve(double start_x, double start_y, double mid_x, double mid_y, double end_x, double end_y, double objspace_flatness_squared, handle_t poly, int n)
  {
  result_t result;

  // midpoint
  double midpoint_x = (start_x + 2 * mid_x + end_x) / 4;
  double midpoint_y = (start_y + 2 * mid_y + end_y) / 4;
  // versus directly drawn line
  double distance_x = (start_x + end_x) / 2 - midpoint_x;
  double distance_y = (start_y + end_y) / 2 - midpoint_y;

  if (n > 16) // 65536 segments on one curve better be enough!
    return e_unexpected;

  if (distance_x * distance_x + distance_y*distance_y > objspace_flatness_squared)
    {
    // half-pixel error allowed... need to be smaller if AA
    // tesselate from start->mid
    if (failed(result = tesselate_curve(
          start_x, start_y, 
          (start_x + mid_x) / 2.0f, (start_y + mid_y) / 2.0f, 
          midpoint_x, midpoint_y, objspace_flatness_squared, poly, n + 1)))
      return result;

    // tesselate mid->end
    if (failed(result = tesselate_curve(
      midpoint_x, midpoint_y, 
      (mid_x + end_x) / 2.0f, (mid_y + end_y) / 2.0f, 
      end_x, end_y, objspace_flatness_squared, poly, n + 1)))
      return result;
    }
  else
    {
    uint16_t len;
    point_t prev_pt;

    if(failed(result = vector_count(poly, &len)) ||
       failed(result = vector_at(poly, len-1, &prev_pt)))
       return result;

    point_t pt;
    pt.x = (gdi_dim_t) round(end_x);
    pt.y = (gdi_dim_t) round(end_y);

    if(pt.x != prev_pt.x || pt.y != prev_pt.y)
      return vector_push_back(poly, &pt);
    }
  return s_ok;
  }

// this is used to create a simple canvas that will fill the glyph bitmap
// with alpha values.  Fonts are stored as a polypolygon after scalling
// from a bezier curve.
typedef struct _font_canvas_t {
  canvas_t canvas;
  glyph_t *glyph;
  } font_canvas_t;

#define BITMAP_ANTI_ALIAS_SCALE 15
#define BITMAP_ANTI_ALIAS_OFFSET (BITMAP_ANTI_ALIAS_SCALE >> 1)
#define BITMAP_PEL_WEIGHT (256 / BITMAP_ANTI_ALIAS_SCALE)

static inline result_t as_font_canvas(canvas_t *hndl, font_canvas_t **canvas)
  {
  if (hndl == 0 || canvas == 0 || hndl->version != sizeof(font_canvas_t))
    return e_bad_parameter;

  *canvas = (font_canvas_t *)hndl;

  return s_ok;
  }

static color_t fc_get_pixel(canvas_t *hndl, const point_t *src)
  {
  return color_hollow;
  }

static void fc_set_pixel(canvas_t *hndl, const point_t *dest, color_t color)
  {
  font_canvas_t *canvas;
  if (dest == 0 || failed(as_font_canvas(hndl, &canvas)))
    return;

  // calculate the distancs
  gdi_dim_t x_pel = (dest->x - BITMAP_ANTI_ALIAS_OFFSET)/ BITMAP_ANTI_ALIAS_SCALE;
  gdi_dim_t y_pel = (dest->y - BITMAP_ANTI_ALIAS_OFFSET)/ BITMAP_ANTI_ALIAS_SCALE;

  // each time a pixel is writen we add 1
#ifdef _DEBUG
  if(x_pel > canvas->glyph->bitmap.width || y_pel > canvas->glyph->bitmap.height)
    {
    trace_debug("Range error\r\n");
    }
#endif

  canvas->glyph->bitmap.pixels[x_pel + (y_pel * canvas->glyph->bitmap.width)] += BITMAP_PEL_WEIGHT;
  }

static void fc_fast_fill(canvas_t *hndl, const rect_t *dest, color_t fill_color)
  {
  }

static void fc_fast_line(canvas_t *hndl, const point_t *p1, const point_t *p2, color_t fill_color)
  {
  font_canvas_t *canvas;
  if (p1 == 0 || p2 == 0 || failed(as_font_canvas(hndl, &canvas)))
    return;
  uint8_t ac = alpha(fill_color);

  gdi_dim_t x;
  // calculate the distancs
  gdi_dim_t y_pel = (p1->y - BITMAP_ANTI_ALIAS_OFFSET) / BITMAP_ANTI_ALIAS_SCALE;

  for (x = p1->x; x < p2->x; x++)
    {
    gdi_dim_t x_pel = (x - BITMAP_ANTI_ALIAS_OFFSET) / BITMAP_ANTI_ALIAS_SCALE;

#ifdef _DEBUG
    if(x_pel > canvas->glyph->bitmap.width || y_pel > canvas->glyph->bitmap.height)
      {
      trace_debug("Range error\r\n");
      }
#endif

    canvas->glyph->bitmap.pixels[x + (y_pel * canvas->glyph->bitmap.width)] += 17;
    }
  }

static void fc_fast_copy(canvas_t *hndl, const point_t *dest, const struct _canvas_t *src_canvas, const rect_t *src)
  {

  }

#if _DEBUG_FONTS
static glyph_t *tg = 0;
#endif

static pen_t white_pen = { color_white, 1, ps_solid };

// cache (if possible) the character ch in th font
result_t ensure_glyph(sized_font_t *font_info, char ch, const glyph_t **gp)
  {
  result_t result;
  glyph_t *glyph = 0;

  if (font_info == 0)
    return e_bad_parameter;

  result = map_find(font_info->glyphs, (const void *)ch, (void **)&glyph);
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

  fontinfo_t *fontinfo = &font_info->fontrec->info;

  int16_t x = 0;
  int16_t y = 0;
  int16_t bottom_y = 0;

  // calculate the x, y
  double scale = scale_for_pixel_height(fontinfo, font_info->size);

  int16_t advance;
  int16_t gw;
  int16_t gh;
  uint16_t g;

  if (failed(result = find_glyph_index(fontinfo, ch, &g)))
    return result;

  uint16_t numOfLongHorMetrics = read_uint16_setpos(fontinfo->stream, fontinfo->hhea + 34);
  if (g < numOfLongHorMetrics)
    advance = read_int16_setpos(fontinfo->stream, fontinfo->hmtx + 4 * g);
  else
    advance = read_int16_setpos(fontinfo->stream, fontinfo->hmtx + 4 * (numOfLongHorMetrics - 1));

  glyph_shape_t *shape;

  // check to see if the shape is in the cache of font shapes.
  if (failed(map_find(font_info->fontrec->shapes, (const void *)g, (void **)&shape)))
    {
    // try to load the shape from the stream.  Very expensive operation
    if (failed(result = get_glyph_shape(fontinfo, g, &shape)))
      return result;

    // this caches the font into the map of stored shapes
    if (failed(result = map_add(font_info->fontrec->shapes, (const void *)g, (const void *)&shape)))
      {
      destroy_shape(shape);
      return result;
      }
    }

  gw = (gdi_dim_t)(scale * (shape->x_max - shape->x_min));
  gh = (gdi_dim_t)(scale * (shape->y_max - shape->y_min));

  uint16_t bitmap_length = sizeof(glyph_t) + (gw * gh);
  glyph = (glyph_t *)neutron_malloc(bitmap_length);
  memset(glyph, 0, bitmap_length);

  glyph->bitmap.width = gw;
  glyph->bitmap.height = gh;
  glyph->wchar = ch;
  glyph->codepoint = g;
  glyph->advance = ((gdi_dim_t)(scale * advance));
  glyph->font_info = font_info;
  glyph->baseline = ((gdi_dim_t) floor(shape->y_min * scale))+gh;

  if(bitmap_length > sizeof(glyph_t))
    {
    handle_t poly;
    handle_t lengths;
    uint16_t contour_num;
    uint16_t n = 0;
    uint16_t start = 0;
    uint16_t v;

    // we know how many vertices there are so we can allocate a polypolygon
    result = vector_create(sizeof(point_t), &poly);

    if (failed(result))
      return result;

    result = vector_create(sizeof(uint16_t), &lengths);

    if(failed(result))
      {
      vector_close(poly);
      return result;
      }

    point_t prev_pt = { 0, 0 };
    point_t pt;
    point_t gdi_pt = { 0, 0 };
    uint16_t poly_length;
    uint16_t contour_start = 0;

    // this inverts the shape
    gdi_dim_t y_offs = shape->y_max - shape->y_min;

    // the scale will shrink the curve down to the pixel size
    // however to anti-alias we tell the GDI we are working with
    // a bitmap (n * BITMAP_ANTI_ALIAS_SCALE) + BITMAP_ANTI_ALIAS_SCALE 
    // with an offset of BITMAP_ANTI_ALIAS_OFFSET
    // this will then give us a simple way to determine
    // the alpha value of the pixel to a 25% subpixel scaling
    scale *= BITMAP_ANTI_ALIAS_SCALE;

    // remove un-needed curves
    double objspace_flatness_squared = 1 / scale;

    objspace_flatness_squared *= objspace_flatness_squared;
    objspace_flatness_squared *= 0.35;

    for (contour_num = 0; contour_num < shape->num_contours; contour_num++)
      {
      for (v = 0; v < shape->contours[contour_num]->num_vertices; v++)
        {
        pt.x = shape->contours[contour_num]->vertices[v].x;
        pt.y = shape->contours[contour_num]->vertices[v].y;

        if(prev_pt.x != pt.x || prev_pt.y != pt.y)
          {
          if(failed(result = vector_push_back(poly, &pt)))
            {
            vector_close(poly);
            vector_close(lengths);
            return result;
            }
          }

        if(shape->contours[contour_num]->vertices[v].type == contour_vcurve)
          {
          // add points from here to the next point
          if(failed(result = tesselate_curve(prev_pt.x, prev_pt.y,
                                             shape->contours[contour_num]->vertices[v].cx, shape->contours[contour_num]->vertices[v].cy,
                                             shape->contours[contour_num]->vertices[v].x, shape->contours[contour_num]->vertices[v].y,
                                             objspace_flatness_squared, poly, 1)))
            {
            vector_close(poly);
            vector_close(lengths);
            return result;
            }
          }

        prev_pt.x = pt.x;
        prev_pt.y = pt.y;
        }

        if(failed(result = vector_count(poly, &poly_length)))
          {
          vector_close(poly);
          vector_close(lengths);
          return result;
          }

        uint16_t contour_length = poly_length - contour_start;
        contour_start = poly_length;
        if(failed(vector_push_back(lengths, &contour_length)))
          {
          vector_close(poly);
          vector_close(lengths);
          return result;
          }
      }

    // at this point we have a scaled and flattened version of the bezier curves that make
    // up the font.  
    // we create a canvas that renders the alpha values assuming a white polygon
    font_canvas_t fc;

    memset(&fc, 0, sizeof(font_canvas_t));

    fc.glyph = glyph;
    fc.canvas.fast_copy = fc_fast_copy;
    fc.canvas.fast_fill = fc_fast_fill;
    fc.canvas.fast_line = fc_fast_line;
    fc.canvas.get_pixel = fc_get_pixel;
    fc.canvas.set_pixel = fc_set_pixel;
    fc.canvas.height = (gh * BITMAP_ANTI_ALIAS_SCALE)+ (BITMAP_ANTI_ALIAS_SCALE <<1);
    fc.canvas.width = (gw * BITMAP_ANTI_ALIAS_SCALE) + (BITMAP_ANTI_ALIAS_SCALE << 1);
    fc.canvas.version = sizeof(font_canvas_t);

    rect_t rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = (gw * BITMAP_ANTI_ALIAS_SCALE)+ (BITMAP_ANTI_ALIAS_OFFSET << 1);
    rect.bottom = (gh * BITMAP_ANTI_ALIAS_SCALE) + (BITMAP_ANTI_ALIAS_OFFSET << 1);

    uint16_t *ptr_lengths;
    point_t *ptr_points;

    if(failed(result = vector_count(poly, &poly_length)))
      {
      vector_close(poly);
      vector_close(lengths);
      return result;
      }

    if(succeeded(result = vector_begin(poly, (void **)&ptr_points)) &&
      succeeded(result = vector_begin(lengths, (void **)&ptr_lengths)))
      {
      uint16_t start = 0;
      for(contour_num = 0; contour_num < shape->num_contours; contour_num++)
        {
        // we have to modify all of the points to match our object space
        for(n = 0; n < ptr_lengths[contour_num]; n++)
          {
          ptr_points[n + start].x = BITMAP_ANTI_ALIAS_OFFSET +(gdi_dim_t)((ptr_points[n + start].x - shape->x_min) * scale);
          ptr_points[n + start].y = BITMAP_ANTI_ALIAS_OFFSET + (gdi_dim_t)((y_offs - ptr_points[n + start].y - shape->y_min) * scale);

          if(n == 0)
            {
            prev_pt.x = ptr_points[n + start].x;
            prev_pt.y = ptr_points[n + start].y;
            }
          else
            {
            if(prev_pt.x == ptr_points[n + start].x && prev_pt.y == ptr_points[n + start].y)
              {
              // duplicate point, remove from the array
              if(failed(result = vector_erase(poly, n + start)))
                {
                vector_close(poly);
                vector_close(lengths);
                return result;
                }

              // remove a point
              n--;
              ptr_lengths[contour_num]--;
              }
            else
              {
              prev_pt.x = ptr_points[n + start].x;
              prev_pt.y = ptr_points[n + start].y;
              }
            }
          }

        start += n;
        }

  #ifdef _DEBUG
      trace_debug("Num contours: %d, clip: left:%d, top:%d, right:%d, bottom:%d\r\n",
        shape->num_contours, rect.left, rect.top, rect.right, rect.bottom);
      n = 0;
      for(contour_num = 0; contour_num < shape->num_contours; contour_num++)
        {
        trace_debug("Contour %d, Length: %d\r\n", contour_num, ptr_lengths[contour_num]);
        for(v = 0; v < ptr_lengths[contour_num]; v++)
          {
          trace_debug("Vertex %d, x:%d, y:%d\r\n", v, ptr_points[n + v].x, ptr_points[n + v].y);
          }
        n += ptr_lengths[contour_num];
        }
  #endif
      // render the polypolygon onto the bitmap
      result = polypolygon_impl(&fc.canvas, &rect, 0, color_white, shape->num_contours, ptr_lengths, ptr_points);
      }

    vector_close(poly);
    vector_close(lengths);

    if(failed(result))
      return result;
    }

  if (gp != 0)
    *gp = glyph;

#if _DEBUG_FONTS
    if(tg == 0 && ch == '1')
      tg = glyph;
    else if(tg != 0)
      if(tg->wchar != '1')
        {
        trace_debug("Help me....\r\n");
        }
#endif
  // add the glyph
  return map_add(font_info->glyphs, (void *) ch, &glyph);
  }

static result_t release_glyph(glyph_t *glyph)
  {
  // just free it
  neutron_free(glyph);

  return s_ok;
  }

static result_t get_index(handle_t stream, uint16_t offset, uint16_t *index)
  {
  uint32_t value;

  offset += 8;          // skip the name and checksum
  value = read_uint32(stream);
  *index = (uint16_t)value;

  // length not used.
  value = read_uint32(stream);

  return s_ok;
  }

static inline bool is_tag(uint32_t value, uint8_t v1, uint8_t v2, uint8_t v3, uint8_t v4)
  {
  return ((uint8_t)(value >> 24)) == v1 &&
    ((uint8_t)(value >> 16)) == v2 &&
    ((uint8_t)(value >> 8)) == v3 &&
    ((uint8_t)(value)) == v4;
  }

static inline bool is_str(uint32_t value, const char *str)
  {
  return is_tag(value, (uint8_t)str[0], (uint8_t)str[1], (uint8_t)str[2], (uint8_t)str[3]);
  }

static result_t find_table(handle_t stream, const char *name, uint16_t *index)
  {
  int32_t num_tables = read_uint16_setpos(stream, 4);
  int32_t i;
  uint32_t loc = 12;
  uint32_t length; 
  for (i = 0; i < num_tables; ++i)
    {
    stream_setpos(stream, loc);

    uint32_t tag = read_uint32(stream);
    if (is_str(tag, name))
      {
      read_uint32(stream);    // skip the checksum
      *index = read_uint32(stream);
      length = read_uint32(stream);    // skip the length

      return s_ok;
      }

    loc += 16;
    }

  return e_not_found;
  }

result_t init_font(fontinfo_t *info, handle_t stream)
  {
  result_t result;
  uint16_t cmap;
  int32_t i;

  info->stream = stream;

  uint32_t version = read_uint32(stream);

  if (failed(result = find_table(stream, "cmap", &cmap)) ||
    failed(result = find_table(stream, "loca", &info->loca)) ||
    failed(result = find_table(stream, "head", &info->head)) ||
    failed(result = find_table(stream, "glyf", &info->glyf)) ||
    failed(result = find_table(stream, "hhea", &info->hhea)) ||
    failed(result = find_table(stream, "hmtx", &info->hmtx)))
    return result;

  uint16_t maxp;
  if (failed(result = find_table(stream, "maxp", &maxp)))
    info->num_glyphs = -1;
  else
    info->num_glyphs = read_int16_setpos(stream, maxp + 4);

  // find a cmap encoding table we understand *now* to avoid searching
  // later. (todo: could make this installable)
  // the same regardless of glyph.
  uint16_t cmap_version = read_uint16_setpos(stream, cmap);
  uint16_t num_cmap_tables = read_uint16_setpos(stream, cmap + 2);
  info->index_map = 0;
  for (i = 0; i < num_cmap_tables; ++i)
    {
    uint16_t platform_id = read_uint16(stream);
    uint16_t encoding_id = read_uint16(stream);
    uint32_t offset = read_uint32(stream);

    // find an encoding we understand:
    switch (platform_id)
      {
      case 3:             // PLATFORM_ID_MICROSOFT
        switch (encoding_id)
          {
          case 1:         // UNICODE BMP
          case 10:        // UNICODE FULL
                          // MS/Unicode
            info->index_map = cmap + (uint16_t)offset;
            break;
          }
        break;
      case 0:               // PLATFORM_ID_UNICODE
                            // Mac/iOS has these
                            // all the encodingIDs are unicode, so we don't bother to check it
        info->index_map = cmap + (uint16_t)offset;
        break;
      }
    }

  if (info->index_map == 0)
    return 0;

  info->index_to_location_format = read_uint16_setpos(stream, info->head + 50);

  info->clip_rect.left = read_int16_setpos(stream, info->head + 36);
  info->clip_rect.bottom = read_int16(stream);
  info->clip_rect.right = read_int16(stream);
  info->clip_rect.top = read_int16(stream);

  return s_ok;
  }

result_t create_font(const char *name, uint16_t pixels, const char *hint, handle_t  *fh)
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
    // point length does not exist so create it
    font = (sized_font_t *)neutron_malloc(sizeof(sized_font_t));
    font->fontrec = fr;
    font->size = pixels;
    font->version = sizeof(sized_font_t);
    double scale = scale_for_pixel_height(&fr->info, pixels);
    font->vertical_height = (gdi_dim_t) floor(fabs(rect_height(&fr->info.clip_rect) * scale));
    font->baseline = (gdi_dim_t) floor(fr->info.clip_rect.bottom * scale);
    
    if (failed(result = map_create(dup_codepoint, dup_glyph, compare_codepoint, destroy_codepoint, destroy_glyph, &font->glyphs)))
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

  font_t *fr;
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
  fr = (font_t *)neutron_malloc(sizeof(font_t));
  memset(fr, 0, sizeof(font_t));
  fr->version = sizeof(font_t);

  // create an empty font list
  if (failed(result = vector_create(sizeof(sized_font_t), &fr->fonts)) ||
    failed(result = map_create(dup_codepoint, dup_shape, compare_codepoint, destroy_codepoint, destroy_shape, &fr->shapes)) ||
    failed(result = init_font(&fr->info, stream)))
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
  neutron_free((void *)fr->name);
  fr->name = 0;

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
