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
then the origional copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
//   After the sample programs is the "header file" section. This section
//   includes documentation for each API function.
//
//   Some important concepts to understand to use this library:
//
//      Codepoint
//         Characters are defined by unicode codepoints, e.g. 65 is
//         uppercase A, 231 is lowercase c with a cedilla, 0x7e30 is
//         the hiragana for "ma".
//
//      Glyph
//         A visual character shape (every codepoint is rendered as
//         some glyph)
//
//      Glyph index
//         A font-specific integer ID representing a glyph
//
//      Baseline
//         Glyph shapes are defined relative to a baseline, which is the
//         bottom of uppercase characters. Characters extend both above
//         and below the baseline.
//
//      Current Point
//         As you draw text to the screen, you keep track of a "current point"
//         which is the origin of each character. The current point's vertical
//         position is the baseline. Even "baked fonts" use this model.
//
//      Vertical Font Metrics
//         The vertical qualities of the font, used to vertically position
//         and space the characters. See docs for stbtt_GetFontVMetrics.
//
//      Font Size in Pixels or Points
//         The preferred interface for specifying font sizes in stb_truetype
//         is to specify how tall the font's vertical extent should be in pixels.
//         If that sounds good enough, skip the next paragraph.
//
//         Most font APIs instead use "points", which are a common typographic
//         measurement for describing font length, defined as 72 points per inch.
//         However, true "per inch" conventions don't make much sense on computer displays
//         since they different monitors have different number of pixels per
//         inch. For example, Windows traditionally uses a convention that
//         there are 96 pixels per inch, thus making 'inch' measurements have
//         nothing to do with inches, and thus effectively defining a point to
//         be 1.333 pixels. Additionally, the TrueType font stream provides
//         an explicit scale factor to scale a given font's glyphs to points,
//         but the author has observed that this scale factor is often wrong
//         for non-commercial fonts, thus making fonts scaled in points
//         according to the TrueType spec incoherently sized in practice.
//
//   The system uses the raw stream found in the .ttf file without changing it
//   and without building auxiliary stream structures. This is a bit inefficient
//   on little-endian systems (the stream is big-endian), but assuming you're
//   caching the bitmaps or glyph shapes this shouldn't be a big deal.
//
#include "font.h"

#include <math.h>

typedef struct
  {
  int16_t w;
  int16_t h;
  int16_t stride;
  uint8_t *pixels;
  } font_bitmap_t;

enum {
  contour_vmove = 1,      // move to point
  contour_vline,          // line to point x,y is point
  contour_vcurve,         // bezier curve with control point in cx, cy 
  };

typedef struct
  {
  int16_t x, y, cx, cy;
  uint8_t type;
  } glyph_vertex_t;

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
  for (i = 0; i < num_tables; ++i)
    {
    stream_setpos(stream, loc);

    uint32_t tag = read_uint32(stream);
    if (is_str(tag, name))
      {
      read_uint32(stream);    // skip the checksum
      *index = read_uint32(stream);
      read_uint32(stream);    // skip the length

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
  return s_ok;
  }

result_t find_glyph_index(const fontinfo_t *info, uint16_t unicode_codepoint, uint16_t *glyph_offset)
  {
  uint32_t index_map = info->index_map;

  if(glyph_offset == 0)
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

    if(glyph_offset == 0)
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

static result_t get_glyph_offset(const fontinfo_t *info, int glyph_index, uint16_t *offset)
  {
  uint32_t g1, g2;

  if (glyph_index >= info->num_glyphs)
    return e_bad_parameter; // glyph index out of range
  if (info->index_to_location_format >= 2)
    return e_bad_handle; // unknown index->glyph map format

  if (info->index_to_location_format == 0)
    {
    g1 = info->glyf + read_uint16_setpos(info->stream, info->loca + glyph_index * 2) * 2;
    g2 = info->glyf + read_uint16_setpos(info->stream, info->loca + glyph_index * 2 + 2) * 2;
    }
  else
    {
    g1 = info->glyf + read_uint32_setpos(info->stream, info->loca + glyph_index * 4);
    g2 = info->glyf + read_uint32_setpos(info->stream, info->loca + glyph_index * 4 + 4);
    }

  if(g1 == g2)
    return e_not_found;

  *offset = (uint16_t)g1;

  return s_ok;
  }

result_t get_glyph_box(const fontinfo_t *info, int glyph_index, int16_t *x0, int16_t *y0, int16_t *x1, int16_t *y1)
  {
  result_t result;
  uint16_t g = 0;
  if(failed(result = get_glyph_offset(info, glyph_index, &g)))
    return result;

  if (x0)
    *x0 = read_int16_setpos(info->stream, g + 2);

  if (y0)
    *y0 = read_int16_setpos(info->stream, g + 4);

  if (x1)
    *x1 = read_int16_setpos(info->stream, g + 6);

  if (y1)
    *y1 = read_int16_setpos(info->stream, g + 8);

  return s_ok;
  }

typedef struct _contour_t {
  uint16_t end_flags;
  handle_t vertices;
  } contour_t;

#define ON_CURVE 1
#define X_IS_BYTE 2
#define Y_IS_BYTE 4
#define REPEAT 8
#define X_DELTA 16
#define Y_DELTA 32

static void read_coord(bool is_y, uint8_t byte_flag, uint8_t delta_flag, int16_t val_min,
  int16_t val_max, handle_t stream, handle_t vertices)
  {
  uint16_t index;
  int16_t value = 0;
  uint16_t num_points;
  vector_count(vertices, &num_points);

  for (index = 0; index < num_points; index++)
    {
    glyph_vertex_t vert;
    vector_at(vertices, index, &vert);
    if (vert.type & byte_flag)
      {
      uint8_t offs;
      stream_read(stream, &offs, 1, 0);
      if (vert.type & delta_flag)
        value += offs;
      else
        value -= offs;
      }
    else if ((vert.type & delta_flag)== 0)
      {
      int16_t offs = read_int16(stream);
      value += offs;
      }

    if (is_y)
      vert.y = value; // min(val_max, max(val_min, value));
    else
      vert.x = value; // min(val_max, max(val_min, value));

    vector_set(vertices, index, &vert);
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

// change vertices to a dynamic array
static result_t get_glyph_shape(const fontinfo_t *info, int glyph_index, handle_t *hshape)
  {
  result_t result;
  int num_vertices = 0;
  uint16_t g = 0;
  if(failed(result = get_glyph_offset(info, glyph_index, &g)))
    return result;

  // a glyph is an array of arrays of vertices describing the shape.
  handle_t shape;
  vector_create(sizeof(contour_t), &shape);

  *hshape = shape;

  if (g < 1)
    return e_not_found;

  // this will seek to the start of the contours
  int16_t numberOfContours = read_int16_setpos(info->stream, g);
  int16_t x_min = read_int16(info->stream);
  int16_t y_min = read_int16(info->stream);
  int16_t x_max = read_int16(info->stream);
  int16_t y_max = read_int16(info->stream);

  // number of contours == -1 is a compound glyph (not supported)

  if (numberOfContours > 0)
    {
    uint8_t flags = 0;
    int32_t j = 0, was_off = 0, start_off = 0;
    int16_t i;
    uint16_t end_pt = 0;
    for (i = 0; i < numberOfContours; i++)
      {
      contour_t ct = { read_uint16(info->stream), 0 };
      vector_push_back(shape, &ct);       // copies the length over
      }

    uint16_t ins = read_uint16(info->stream);
    uint16_t pos;
    stream_getpos(info->stream, &pos);
    stream_setpos(info->stream, pos + ins);         // skip the instructions

    int16_t contour_num;
    ins = 0;
    // now we read the x and y stream and flags
    for (contour_num = 0; contour_num < numberOfContours; contour_num++)
      {
      contour_t ct;
      vector_at(shape, contour_num, &ct);
      glyph_vertex_t vert;

      vector_create(sizeof(glyph_vertex_t), &ct.vertices);    // create the vertex list
      vector_set(shape, contour_num, &ct);

      for (; ins <= ct.end_flags; ins++)
        {
        memset(&vert, 0, sizeof(glyph_vertex_t));
        stream_read(info->stream, &vert.type, 1, 0);

        if ((vert.type & REPEAT) != 0)
          {
          uint8_t rpt = 0;
          // ins += rpt;       // same flag for a while...
          stream_read(info->stream, &rpt, 1, 0);
          while (rpt--)
            {
            ins++;
            vector_push_back(ct.vertices, &vert);
            }
          }

        vector_push_back(ct.vertices, &vert);
        }
      }

    // re-read the contors now
    for (contour_num = 0; contour_num < numberOfContours; contour_num++)
      {
      contour_t ct;
      vector_at(shape, contour_num, &ct);
      glyph_vertex_t vert;

      // now we read the x-coordinates
      read_coord(false, X_IS_BYTE, X_DELTA, x_min, x_max, info->stream, ct.vertices);
      read_coord(true, Y_IS_BYTE, Y_DELTA, y_min, y_max, info->stream, ct.vertices);
      // DEBUG
      if (glyph_index == 18)
        {
        uint16_t i;
        // now we read the x-coordinates
        trace_debug("Contour %d\r\n", contour_num);
          uint16_t num_vertices;
          vector_count(ct.vertices, &num_vertices);
          uint16_t j;
          for (j = 0; j < num_vertices; j++)
            {
            glyph_vertex_t gv;
            vector_at(ct.vertices, j, &gv);
            const char *vertex_name;

            trace_debug("  Vertex %d - flags: %x, x: %d, y: %d\r\n", j, gv.type, gv.x, gv.y);
            }
        }
       // DEBUG

      // we now walk the contour and convert to a ready to process form
      bool start_off = false;       // if the first point was an off-curve then this will be set
      bool was_off = false;
      int16_t start_x = 0;              // only used if first is a control point
      int16_t start_y = 0;
      uint16_t i = 0;
      uint16_t next_pt = 0;         // these are the actual points
      uint16_t num_points;
      glyph_vertex_t curve_pt;      // the point on the curve
      memset(&curve_pt, 0, sizeof(glyph_vertex_t));

      vector_count(ct.vertices, &num_points);
      vector_at(ct.vertices, i++, &vert);

      if ((vert.type & ON_CURVE) == 0)
        {
        // first vertex is not on the curve.  copy some values
        start_x = vert.x;
        start_y = vert.y;
        start_off = true;

        // read the next point.
        vector_at(ct.vertices, i++, &vert);

        if ((vert.type & ON_CURVE) == 0)
          {
          // next point is also a curve point, so interpolate an on-point curve
          curve_pt.x = (start_x + vert.x) >> 1;
          curve_pt.y = (start_y + vert.y) >> 1;
          curve_pt.cx = vert.x;
          curve_pt.cy = vert.y;
          curve_pt.type = contour_vcurve;
          }
        else
          {
          curve_pt.x = vert.x;
          curve_pt.y = vert.y;
          curve_pt.type = contour_vmove;
          }
        }
      else
        {
        // just move to the point
        curve_pt.x = vert.x;
        curve_pt.y = vert.y;
        curve_pt.type = contour_vmove;
        }

      // set the start point
      vector_set(ct.vertices, next_pt++, &curve_pt);

      for (; i < ct.end_flags; i++)
        {
        vector_at(ct.vertices, i, &vert);

        if ((vert.type & ON_CURVE) == 0)
          {
          // its a curve rather than an on-curve point
          if (was_off)
            {
            // if the previous was off the curve then this is two beziers.
            // convert the first part to a curve, and insert the new point into the array
            curve_pt.x = (curve_pt.cx + vert.x) >> 1;
            curve_pt.y = (curve_pt.cy + vert.y) >> 1;
            curve_pt.cx = vert.x;
            curve_pt.cy = vert.y;
            curve_pt.type = contour_vcurve;
            vector_set(ct.vertices, next_pt++, &curve_pt);

            was_off = false;
            }
          else
            {
            // previous was a point so we make this a curve
            was_off = true;
            next_pt--;
            curve_pt.cx = vert.x;
            curve_pt.cy = vert.y;
            curve_pt.type = contour_vcurve;
            vector_set(ct.vertices, next_pt++, &curve_pt);
            }
          }
        else
          {
          memset(&curve_pt, 0, sizeof(glyph_vertex_t));
          was_off = false;
          curve_pt.x = vert.x;
          curve_pt.y = vert.y;
          curve_pt.type = contour_vline;

          vector_set(ct.vertices, next_pt++, &curve_pt);
          }
        }

      // we now connect the line back to the start
      vector_at(ct.vertices, 0, &vert);
      vector_truncate(ct.vertices, next_pt);

      curve_pt.cx = start_x;
      curve_pt.cy = start_y;
      curve_pt.x = vert.x;
      curve_pt.y = vert.y;
      curve_pt.type = start_off ? contour_vcurve : contour_vline;
      vector_push_back(ct.vertices, &curve_pt);


      // DEBUG
      if (glyph_index == 18)
        {
          trace_debug("converted glyph\r\n");
          uint16_t num_vertices;
          vector_count(ct.vertices, &num_vertices);
          uint16_t j;
          for (j = 0; j < num_vertices; j++)
            {
            glyph_vertex_t gv;
            vector_at(ct.vertices, j, &gv);
            const char *vertex_name;
            switch (gv.type)
              {
              case contour_vmove:
                vertex_name = "move";
                break;
              case contour_vline:
                vertex_name = "line";
                break;
              case contour_vcurve:
                vertex_name = "curve";
                break;
              default:
                vertex_name = "unknown";
                break;
              }

            trace_debug("  Vertex %d - Type: %s, x: %d, y: %d, cx: %d, cy: %d\r\n", j, vertex_name, gv.x, gv.y, gv.cx, gv.cy);
            }
        }
      // DEBUG

      // and save the contour
      vector_set(shape, contour_num, &ct);
      }
    }
  else
    return e_bad_handle;

  return s_ok;
  }

static float scale_for_pixel_height(const fontinfo_t *info, float height)
  {
  float ascender = read_int16_setpos(info->stream, info->hhea + 4);
  float descender = read_int16_setpos(info->stream, info->hhea + 6);


  return (float)height / (ascender + descender);
  }

//////////////////////////////////////////////////////////////////////////////
//
// antialiasing software rasterizer
//

static void get_glyph_bitmap_box_subpixel(const fontinfo_t *font, int glyph, float scale_x, float scale_y, float shift_x, float shift_y, int16_t *ix0, int16_t *iy0, int16_t *ix1, int16_t *iy1)
  {
  int16_t x0 = 0, y0 = 0, x1, y1; // =0 suppresses compiler warning
  if (failed(get_glyph_box(font, glyph, &x0, &y0, &x1, &y1)))
    {
    // e.g. space character
    if (ix0)
      *ix0 = 0;

    if (iy0)
      *iy0 = 0;

    if (ix1)
      *ix1 = 0;

    if (iy1)
      *iy1 = 0;
    }
  else
    {
    // move to integral bboxes (treating pixels as little squares, what pixels get touched)?
    if (ix0)
      *ix0 = (int)floor(x0 * scale_x + shift_x);

    if (iy0)
      *iy0 = (int)floor(y0 * scale_y + shift_y);

    if (ix1)
      *ix1 = (int)ceil(x1 * scale_x + shift_x);

    if (iy1)
      *iy1 = (int)ceil(y1 * scale_y + shift_y);
    }
  }

//////////////////////////////////////////////////////////////////////////////
//
//  Rasterizer

typedef struct _heap_chunk_t
  {
  struct _heap_chunk_t *next;
  } heap_chunk_t;

typedef struct _heap_t
  {
  heap_chunk_t *head;
  void   *first_free;
  int    num_remaining_in_head_chunk;
  } heap_t;

static void *heap_alloc(heap_t *hh, size_t size)
  {
  if (hh->first_free)
    {
    void *p = hh->first_free;
    hh->first_free = *(void **)p;
    return p;
    }
  else
    {
    if (hh->num_remaining_in_head_chunk == 0)
      {
      int count = (size < 32 ? 2000 : size < 128 ? 800 : 100);
      heap_chunk_t *c = (heap_chunk_t *)neutron_malloc(sizeof(heap_chunk_t) + size * count);
      if (c == 0)
        return 0;
      c->next = hh->head;
      hh->head = c;
      hh->num_remaining_in_head_chunk = count;
      }

    --hh->num_remaining_in_head_chunk;
    return (char *)(hh->head) + size * hh->num_remaining_in_head_chunk;
    }
  }

static void heap_free(heap_t *hh, void *p)
  {
  *(void **)p = hh->first_free;
  hh->first_free = p;
  }

static void heap_cleanup(heap_t *hh)
  {
  heap_chunk_t *c = hh->head;
  while (c)
    {
    heap_chunk_t *n = c->next;
    neutron_free(c);
    c = n;
    }
  }

typedef struct _edge_t {
  float x0, y0, x1, y1;
  int invert;
  } edge_t;

typedef struct _active_edge_t
  {
  struct _active_edge_t *next;
  float fx, fdx, fdy;
  float direction;
  float sy;
  float ey;

  } active_edge_t;

static active_edge_t *new_active_edge(heap_t *hh, edge_t *e, int off_x, float start_point)
  {
  active_edge_t *z = (active_edge_t *)heap_alloc(hh, sizeof(*z));

  if (!z)
    return z;

  float dxdy = (e->x1 - e->x0) / (e->y1 - e->y0);

  z->fdx = dxdy;
  z->fdy = dxdy != 0.0f ? (1.0f / dxdy) : 0.0f;
  z->fx = e->x0 + dxdy * (start_point - e->y0);
  z->fx -= off_x;
  z->direction = e->invert ? 1.0f : -1.0f;
  z->sy = e->y0;
  z->ey = e->y1;
  z->next = 0;
  return z;
  }

// the edge passed in here does not cross the vertical line at x or the vertical line at x+1
// (i.e. it has already been clipped to those)
static void handle_clipped_edge(float *scanline, int x, active_edge_t *e, float x0, float y0, float x1, float y1)
  {
  if (y0 == y1)
    return;

  if (y0 > e->ey)
    return;

  if (y1 < e->sy)
    return;

  if (y0 < e->sy)
    {
    x0 += (x1 - x0) * (e->sy - y0) / (y1 - y0);
    y0 = e->sy;
    }

  if (y1 > e->ey)
    {
    x1 += (x1 - x0) * (e->ey - y1) / (y1 - y0);
    y1 = e->ey;
    }


  if (x0 <= x && x1 <= x)
    scanline[x] += e->direction * (y1 - y0);
  else if (!(x0 >= x + 1 && x1 >= x + 1))
    scanline[x] += e->direction * (y1 - y0) * (1 - ((x0 - x) + (x1 - x)) / 2); // coverage = 1 - average x position
  }

static void fill_active_edges_new(float *scanline, float *scanline_fill, int len, active_edge_t *e, float y_top)
  {
  float y_bottom = y_top + 1;

  while (e)
    {
    // brute force every pixel

    // compute intersection points with top & bottom

    if (e->fdx == 0)
      {
      float x0 = e->fx;
      if (x0 < len)
        {
        if (x0 >= 0)
          {
          handle_clipped_edge(scanline, (int)x0, e, x0, y_top, x0, y_bottom);
          handle_clipped_edge(scanline_fill - 1, (int)x0 + 1, e, x0, y_top, x0, y_bottom);
          }
        else
          handle_clipped_edge(scanline_fill - 1, 0, e, x0, y_top, x0, y_bottom);
        }
      }
    else
      {
      float x0 = e->fx;
      float dx = e->fdx;
      float xb = x0 + dx;
      float x_top, x_bottom;
      float sy0, sy1;
      float dy = e->fdy;

      // compute endpoints of line segment clipped to this scanline (if the
      // line segment starts on this scanline. x0 is the intersection of the
      // line with y_top, but that may be off the line segment.
      if (e->sy > y_top)
        {
        x_top = x0 + dx * (e->sy - y_top);
        sy0 = e->sy;
        }
      else
        {
        x_top = x0;
        sy0 = y_top;
        }
      if (e->ey < y_bottom)
        {
        x_bottom = x0 + dx * (e->ey - y_top);
        sy1 = e->ey;
        }
      else
        {
        x_bottom = xb;
        sy1 = y_bottom;
        }

      if (x_top >= 0 && x_bottom >= 0 && x_top < len && x_bottom < len)
        {
        // from here on, we don't have to range check x values

        if ((int)x_top == (int)x_bottom)
          {
          float height;
          // simple case, only spans one pixel
          int x = (int)x_top;
          height = sy1 - sy0;

          scanline[x] += e->direction * (1 - ((x_top - x) + (x_bottom - x)) / 2)  * height;
          scanline_fill[x] += e->direction * height; // everything right of this pixel is filled
          }
        else
          {
          int x, x1, x2;
          float y_crossing, step, sign, area;
          // covers 2+ pixels
          if (x_top > x_bottom)
            {
            // flip scanline vertically; signed area is the same
            float t;
            sy0 = y_bottom - (sy0 - y_top);
            sy1 = y_bottom - (sy1 - y_top);
            t = sy0, sy0 = sy1, sy1 = t;
            t = x_bottom, x_bottom = x_top, x_top = t;
            dx = -dx;
            dy = -dy;
            t = x0, x0 = xb, xb = t;
            }

          x1 = (int)x_top;
          x2 = (int)x_bottom;
          // compute intersection with y axis at x1+1
          y_crossing = (x1 + 1 - x0) * dy + y_top;

          sign = e->direction;
          // area of the rectangle covered from y0..y_crossing
          area = sign * (y_crossing - sy0);
          // area of the triangle (x_top,y0), (x+1,y0), (x+1,y_crossing)
          scanline[x1] += area * (1 - ((x_top - x1) + (x1 + 1 - x1)) / 2);

          step = sign * dy;
          for (x = x1 + 1; x < x2; ++x)
            {
            scanline[x] += area + step / 2;
            area += step;
            }
          y_crossing += dy * (x2 - (x1 + 1));

          scanline[x2] += area + sign * (1 - ((x2 - x2) + (x_bottom - x2)) / 2) * (sy1 - y_crossing);

          scanline_fill[x2] += sign * (sy1 - sy0);
          }
        }
      else
        {
        // if edge goes outside of box we're drawing, we require
        // clipping logic. since this does not match the intended use
        // of this library, we use a different, very slow brute
        // force implementation
        int x;
        for (x = 0; x < len; ++x)
          {
          // cases:
          //
          // there can be up to two intersections with the pixel. any intersection
          // with left or right edges can be handled by splitting into two (or three)
          // regions. intersections with top & bottom do not necessitate case-wise logic.
          //
          // the old way of doing this found the intersections with the left & right edges,
          // then used some simple logic to produce up to three segments in sorted order
          // from top-to-bottom. however, this had a problem: if an x edge was epsilon
          // across the x border, then the corresponding y position might not be distinct
          // from the other y segment, and it might ignored as an empty segment. to avoid
          // that, we need to explicitly produce segments based on x positions.

          // rename variables to clearly-defined pairs
          float y0 = y_top;
          float x1 = (float)(x);
          float x2 = (float)(x + 1);
          float x3 = xb;
          float y3 = y_bottom;

          // x = e->x + e->dx * (y-y_top)
          // (y-y_top) = (x - e->x) / e->dx
          // y = (x - e->x) / e->dx + y_top
          float y1 = (x - x0) / dx + y_top;
          float y2 = (x + 1 - x0) / dx + y_top;

          if (x0 < x1 && x3 > x2)
            {         // three segments descending down-right
            handle_clipped_edge(scanline, x, e, x0, y0, x1, y1);
            handle_clipped_edge(scanline, x, e, x1, y1, x2, y2);
            handle_clipped_edge(scanline, x, e, x2, y2, x3, y3);
            }
          else if (x3 < x1 && x0 > x2)
            {  // three segments descending down-left
            handle_clipped_edge(scanline, x, e, x0, y0, x2, y2);
            handle_clipped_edge(scanline, x, e, x2, y2, x1, y1);
            handle_clipped_edge(scanline, x, e, x1, y1, x3, y3);
            }
          else if (x0 < x1 && x3 > x1)
            {  // two segments across x, down-right
            handle_clipped_edge(scanline, x, e, x0, y0, x1, y1);
            handle_clipped_edge(scanline, x, e, x1, y1, x3, y3);
            }
          else if (x3 < x1 && x0 > x1)
            {  // two segments across x, down-left
            handle_clipped_edge(scanline, x, e, x0, y0, x1, y1);
            handle_clipped_edge(scanline, x, e, x1, y1, x3, y3);
            }
          else if (x0 < x2 && x3 > x2)
            {  // two segments across x+1, down-right
            handle_clipped_edge(scanline, x, e, x0, y0, x2, y2);
            handle_clipped_edge(scanline, x, e, x2, y2, x3, y3);
            }
          else if (x3 < x2 && x0 > x2)
            {  // two segments across x+1, down-left
            handle_clipped_edge(scanline, x, e, x0, y0, x2, y2);
            handle_clipped_edge(scanline, x, e, x2, y2, x3, y3);
            }
          else {  // one segment
            handle_clipped_edge(scanline, x, e, x0, y0, x3, y3);
            }
          }
        }
      }
    e = e->next;
    }
  }

// directly AA rasterize edges w/o supersampling
static void rasterize_sorted_edges(font_bitmap_t *result, edge_t *e, int n, int vsubsample, int off_x, int off_y)
  {
  heap_t hh = { 0, 0, 0 };
  active_edge_t *active = 0;
  int y, j = 0, i;
  float scanline_data[129], *scanline, *scanline2;

  if (result->w > 64)
    scanline = (float *)neutron_malloc((result->w * 2 + 1) * sizeof(float));
  else
    scanline = scanline_data;

  scanline2 = scanline + result->w;

  y = off_y;
  e[n].y0 = (float)(off_y + result->h) + 1;

  while (j < result->h)
    {
    // find center of pixel for this scanline
    float scan_y_top = y + 0.0f;
    float scan_y_bottom = y + 1.0f;
    active_edge_t **step = &active;

    memset(scanline, 0, result->w * sizeof(scanline[0]));
    memset(scanline2, 0, (result->w + 1) * sizeof(scanline[0]));

    // update all active edges;
    // remove all active edges that terminate before the top of this scanline
    while (*step)
      {
      active_edge_t * z = *step;
      if (z->ey <= scan_y_top)
        {
        *step = z->next; // delete from list

        z->direction = 0;
        heap_free(&hh, z);
        }
      else
        {
        step = &((*step)->next); // advance through list
        }
      }

    // insert all edges that start before the bottom of this scanline
    while (e->y0 <= scan_y_bottom)
      {
      if (e->y0 != e->y1)
        {
        active_edge_t *z = new_active_edge(&hh, e, off_x, scan_y_top);
        if (z != 0)
          {
          // insert at front
          z->next = active;
          active = z;
          }
        }
      ++e;
      }

    // now process all active edges
    if (active)
      fill_active_edges_new(scanline, scanline2 + 1, result->w, active, scan_y_top);

    float sum = 0;
    for (i = 0; i < result->w; ++i)
      {
      float k;
      int m;
      sum += scanline2[i];
      k = scanline[i] + sum;
      k = (float)fabs(k) * 255 + 0.5f;
      m = (int)k;
      if (m > 255)
        m = 255;
      result->pixels[j*result->stride + i] = (unsigned char)m;
      }

    // advance all the edges
    step = &active;
    while (*step)
      {
      active_edge_t *z = *step;
      z->fx += z->fdx; // advance to position for current scanline
      step = &((*step)->next); // advance through list
      }

    ++y;
    ++j;
    }

  heap_cleanup(&hh);

  if (scanline != scanline_data)
    neutron_free(scanline);
  }

#define COMPARE(a,b)  ((a)->y0 < (b)->y0)

static void sort_edges_ins_sort(edge_t *p, int n)
  {
  int i, j;
  for (i = 1; i < n; ++i)
    {
    edge_t t = p[i], *a = &t;
    j = i;
    while (j > 0)
      {
      edge_t *b = &p[j - 1];
      int c = COMPARE(a, b);
      if (!c)
        break;
      p[j] = p[j - 1];
      --j;
      }
    if (i != j)
      p[j] = t;
    }
  }

static void sort_edges_quicksort(edge_t *p, int n)
  {
  /* threshhold for transitioning to insertion sort */
  while (n > 12)
    {
    edge_t t;
    int c01, c12, c, m, i, j;

    /* compute median of three */
    m = n >> 1;
    c01 = COMPARE(&p[0], &p[m]);
    c12 = COMPARE(&p[m], &p[n - 1]);
    /* if 0 >= mid >= end, or 0 < mid < end, then use mid */
    if (c01 != c12)
      {
      /* otherwise, we'll need to swap something else to middle */
      int z;
      c = COMPARE(&p[0], &p[n - 1]);
      /* 0>mid && mid<num_edges:  0>num_edges => num_edges; 0<num_edges => 0 */
      /* 0<mid && mid>num_edges:  0>num_edges => 0; 0<num_edges => num_edges */
      z = (c == c12) ? 0 : n - 1;
      t = p[z];
      p[z] = p[m];
      p[m] = t;
      }
    /* now p[m] is the median-of-three */
    /* swap it to the beginning so it won't move around */
    t = p[0];
    p[0] = p[m];
    p[m] = t;

    /* partition loop */
    i = 1;
    j = n - 1;
    for (;;)
      {
      /* handling of equality is crucial here */
      /* for sentinels & efficiency with duplicates */
      for (;; ++i)
        {
        if (!COMPARE(&p[i], &p[0]))
          break;
        }

      for (;; --j)
        {
        if (!COMPARE(&p[0], &p[j]))
          break;
        }
      /* make sure we haven't crossed */
      if (i >= j)
        break;
      t = p[i];
      p[i] = p[j];
      p[j] = t;

      ++i;
      --j;
      }
    /* recurse on smaller side, iterate on larger */
    if (j < (n - i))
      {
      sort_edges_quicksort(p, j);
      p = p + i;
      n = n - i;
      }
    else
      {
      sort_edges_quicksort(p + i, n - i);
      n = j;
      }
    }
  }

static void sort_edges(edge_t *p, int n)
  {
  sort_edges_quicksort(p, n);
  sort_edges_ins_sort(p, n);
  }

typedef struct
  {
  float x, y;
  } f_point_t;

static result_t rasterize_windings(font_bitmap_t *result, handle_t contours, float scale_x, float scale_y, float shift_x, float shift_y, int off_x, int off_y, int invert)
  {
  float y_scale_inv = invert ? -scale_y : scale_y;
  edge_t *e;
  uint16_t num_edges;
  uint16_t i, j, k, m;
  int vsubsample = 1;
  // vsubsample should divide 255 evenly; otherwise we won't reach full opacity

  // now we have to blow out the windings into explicit edge lists
  num_edges = 0;
  uint16_t num_contours;
  uint16_t pt_count;
  contour_t contour;
  vector_count(contours, &num_contours);
  for (i = 0; i < num_contours; i++)
    {
    vector_at(contours, i, &contour);
    vector_count(contour.vertices, &pt_count);

    num_edges += pt_count;
    }

  e = (edge_t *)neutron_malloc(sizeof(edge_t) * (num_edges + 1)); // add an extra one as a sentinel
  if (e == 0) return;
  num_edges = 0;

  m = 0;
  for (i = 0; i < num_contours; ++i)
    {
    vector_at(contours, i, &contour);
    vector_count(contour.vertices, &pt_count);

    f_point_t *p;
    vector_begin(contour.vertices, (void **)&p);

    m += pt_count;
    j = 0;
    for (k = 0; k < pt_count; j = k++)
      {
      int a = k, b = j;
      // skip the edge if horizontal
      if (p[j].y == p[k].y)
        continue;
      // add edge from j to k to the list
      e[num_edges].invert = 0;
      if (invert ? p[j].y > p[k].y : p[j].y < p[k].y)
        {
        e[num_edges].invert = 1;
        a = j, b = k;
        }
      e[num_edges].x0 = p[a].x * scale_x + shift_x;
      e[num_edges].y0 = (p[a].y * y_scale_inv + shift_y) * vsubsample;
      e[num_edges].x1 = p[b].x * scale_x + shift_x;
      e[num_edges].y1 = (p[b].y * y_scale_inv + shift_y) * vsubsample;
      ++num_edges;
      }
    }

  // now sort the edges by their highest point (should snap to integer, and then by x)
  //STBTT_sort(e, num_edges, sizeof(e[0]), stbtt__edge_compare);
  sort_edges(e, num_edges);

  // now, traverse the scanlines and find the intersections on each scanline, use xor winding rule
  rasterize_sorted_edges(result, e, num_edges, vsubsample, off_x, off_y);

  neutron_free(e);
  }

static void add_point(f_point_t *points, int n, float x, float y)
  {
  if (!points)
    return; // during first pass, it's unallocated
  points[n].x = x;
  points[n].y = y;
  }

// tesselate until threshhold p is happy... @TODO warped to compensate for non-linear stretching

static result_t tesselate_curve(handle_t points, float x0, float y0, float x1, float y1, float x2, float y2, float objspace_flatness_squared, int n)
  {
  result_t result;

  // midpoint
  float mx = (x0 + 2 * x1 + x2) / 4;
  float my = (y0 + 2 * y1 + y2) / 4;
  // versus directly drawn line
  float dx = (x0 + x2) / 2 - mx;
  float dy = (y0 + y2) / 2 - my;

  if (n > 16) // 65536 segments on one curve better be enough!
    return e_unexpected;

  if (dx*dx + dy*dy > objspace_flatness_squared)
    { // half-pixel error allowed... need to be smaller if AA
    if(failed(result = tesselate_curve(points, x0, y0, (x0 + x1) / 2.0f, (y0 + y1) / 2.0f, mx, my, objspace_flatness_squared, n + 1)))
      return result;

    if(failed(result = tesselate_curve(points, mx, my, (x1 + x2) / 2.0f, (y1 + y2) / 2.0f, x2, y2, objspace_flatness_squared, n + 1)))
      return result;
    }
  else
    {
    f_point_t pt;
    pt.x = x2;
    pt.y = y2;
    vector_push_back(points, &pt);
    }
  return s_ok;
  }

// returns number of contours
static result_t flatten_curves(handle_t vertices, float objspace_flatness)
  {
  result_t result;
  int num_points = 0;

  float objspace_flatness_squared = objspace_flatness * objspace_flatness;
  uint16_t i, n = 0, start = 0, pass;

  uint16_t contour_count;
  if(failed(result = vector_count(vertices, &contour_count)))
    return result;

  for (i = 0; i < contour_count; i++)
    {
    contour_t contour;
    if (succeeded(result = vector_at(vertices, i, &contour)))
      {
      handle_t points;
      if(failed(result = vector_create(sizeof(f_point_t), &points)))
        return result;

      glyph_vertex_t vert;
      f_point_t pt = { 0, 0 };
      uint16_t num_ver;
      if (failed(result = vector_count(contour.vertices, &num_ver)))
        {
        vector_close(points);
        return result;
        }

      uint16_t v;
      for (v = 0; v < num_ver; v++)
        {
        if(failed(result = vector_at(contour.vertices, v, &vert)))
          {
          vector_close(points);
          return result;
          }

        switch (vert.type)
          {
          case contour_vmove :
          case contour_vline :
            pt.x = vert.x;
            pt.y = vert.y;
            vector_push_back(points, &pt);
            break;
          case contour_vcurve :
            if(failed(result = tesselate_curve(points, pt.x, pt.y, vert.cx, vert.cy, vert.x, vert.y, objspace_flatness_squared, 0)))
              return result;
            pt.x = vert.x;
            pt.y = vert.y;
            break;
          }
        }

      // free the old contour
      vector_close(contour.vertices);
      contour.vertices = points;
      contour.end_flags = 0;
      vector_set(vertices, i, &contour);
      }
    }

  return s_ok;
  }

static result_t rasterize(font_bitmap_t *bm, float flatness_in_pixels, handle_t vertices, 
  float scale_x, float scale_y, float shift_x, float shift_y, int x_off, int y_off, int invert)
  {
  result_t result;
  float scale = scale_x > scale_y ? scale_y : scale_x;

  if(failed(result = flatten_curves(vertices, flatness_in_pixels / scale)))
    return result;

  return rasterize_windings(bm, vertices, scale_x, scale_y, shift_x, shift_y, x_off, y_off, invert);
  }

static result_t make_glyph_bitmap_subpixel(const fontinfo_t *info, unsigned char *output,
  int out_w, int out_h, int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int glyph)
  {
  result_t result;
  int16_t ix0, iy0;
  handle_t vertices;

  if(failed(result = get_glyph_shape(info, glyph, &vertices)))
    return result;

  font_bitmap_t gbm;

  get_glyph_bitmap_box_subpixel(info, glyph, scale_x, scale_y, shift_x, shift_y, &ix0, &iy0, 0, 0);
  gbm.pixels = output;
  gbm.w = out_w;
  gbm.h = out_h;
  gbm.stride = out_stride;

  if (gbm.w && gbm.h)
    if(failed(result = rasterize(&gbm, 0.35f, vertices, scale_x, scale_y, shift_x, shift_y, ix0, iy0, 1)))
      return result;

  uint16_t count;
  if(failed(vector_count(vertices, &count)))
    return result;

  uint16_t i;
  for (i = 0; i < count; i++)
    {
    contour_t hndl;
    if(failed(result = vector_at(vertices, i, &hndl)))
      return result;

    if(failed(result = vector_close(hndl.vertices)))
      return result;
    }

  return vector_close(vertices);
  }

result_t create_scaled_glyph(fontinfo_t *f, uint16_t pixel_height, char ch, glyph_t **gp)
  {
  result_t result;

  if(f == 0 || gp == 0)
    return e_bad_parameter;

  int16_t x = 0;
  int16_t y = 0;
  int16_t bottom_y = 0;

  // calculate the x, y
  float scale = scale_for_pixel_height(f, pixel_height);

  int16_t advance;
  int16_t x0;
  int16_t y0;
  int16_t x1;
  int16_t y1;
  int16_t gw;
  int16_t gh;
  uint16_t g;

  if(failed(result = find_glyph_index(f, ch, &g)))
    return result;

  uint16_t numOfLongHorMetrics = read_uint16_setpos(f->stream, f->hhea + 34);
  if (g < numOfLongHorMetrics)
    advance = read_int16_setpos(f->stream, f->hmtx + 4 * g);
  else
    advance = read_int16_setpos(f->stream,  f->hmtx + 4 * (numOfLongHorMetrics - 1));

  get_glyph_bitmap_box_subpixel(f, g, scale, scale, 0.0f, 0.0f, &x0, &y0, &x1, &y1);
  gw = x1 - x0;
  gh = y1 - y0;

  uint16_t bitmap_length = gw * gh;
  glyph_t *glyph = (glyph_t *)neutron_malloc(sizeof(glyph_t) + bitmap_length);

  glyph->width = gw;
  glyph->height = gh;
  glyph->length = bitmap_length;
  glyph->wchar = ch;
  glyph->codepoint = g;

  make_glyph_bitmap_subpixel(f, glyph->bitmap, gw, gh, gw, scale, scale, 0.0f, 0.0f, g);

  *gp = glyph;

  return s_ok;
  }
