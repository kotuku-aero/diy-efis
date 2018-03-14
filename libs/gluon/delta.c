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
#include "delta.h"
#include "../neutron/stream.h"

/*
each byte of the timeseries is run-length encoded
the order is:

7------0|7------0|
enc->

the encoding is variable length.

Values are stored using a compressed length of length
  - 000 stored as absoute value (no length field follows)
  - 001 1
  - 010 2
  - 011 3
  - 100 4
  - 101 5
     - next n bits is the length of the absolute value with leading 0's removed -1
       32 bits is 11111 (encoded as 10111111), 16 bits is 1111 encoded as (1001111)
       8 bits is 111 (encoded as 011111), a 0 value is stored as 0010
     - next n bits is the value stored as MSB---LSB (Big endian) then padded to native type

0 absoulte value, leading 0's are removed
  - length encoded as above
1 relative or count
  - 0 relative value, leading 0's are removed.
    - 0/1 plus/minus relative
      - length encoded as above
  - 1 count of value repeated
    - count as encoded above.  0 is NOT used

A bit stream with a 16 seriesof 0, 1, 3, 6, 6, 6, 6, 6, 6, 5, 9, 6 would be encoded as:

Encoding     value number of bits
--------------------------------------------------------------------------------
0               0      1              -- absolute value
00100           0      6              -- most efficient representation of 0
0               0      7              -- absolute value
00101           1     12              -- most efficent to store +1 as abosulte
100             1     15              -- relative +
0100110         3     22              -- add 2
100             3     25              -- relative +
0100111         6     32              -- add 3
11              6     34              -- repeat
01110101        6     42              -- repeat 5 times
101             6     45              -- relative -
0011            5     49              -- subtract 1
100             5     52              -- relative +
01111100        9     61              -- add 4

This stores the series in 8 bytes. if it was not encoded then the
storage required would be 24 bytes or a 30% compression ratio

the storage of a floating point number is handled slightly differently
as there are 2 streams running inside the same stream.

the first encoding is the exponent and often does not change (8bits)
then the mantissa (24bits) and frequently changes
*/

typedef union {
  uint16_t uint16_val;
  int16_t int16_val;
  uint32_t uint32_val;
  int32_t int32_val;
  } value_t;


typedef struct _cursor_t {
  uint32_t offset;              // current timeseries offset (timestamp)
  uint32_t bitpointer;          // is the next bit to read/write from the stream
  uint32_t blockpointer;        // byte offset of data[0] in the stream
  uint16_t count;               // encoded run-length
  uint16_t length;              // number of bytes in the data buffer
  uint8_t data[4];              // run-length encoding.
  value_t value;                // last read value
  } cursor_t;

typedef struct _float_cursor_t {
  cursor_t mantissa;
  // exponent as read from the stream
  uint8_t exponent;
  // count of the exponent when repeating
  uint16_t exponent_count;
  } float_cursor_t;

typedef struct _deltastream_handle_t
  {
  stream_handle_t stream;
  stream_p backing_stream;       // backing stream
  series_t series;              // read from the stream
  } deltastream_handle_t;

typedef struct _scalar_handle_t {
  deltastream_handle_t stream;
  cursor_t reader;
  cursor_t writer;
  } scalar_handle_t;

typedef struct _float_handle_t {
  deltastream_handle_t stream;
  float_cursor_t reader;
  float_cursor_t writer;
  } float_handle_t;

static inline result_t as_scalar(stream_handle_t *hndl, scalar_handle_t **stream)
  {
  if (hndl->version != sizeof(scalar_handle_t))
    return e_bad_parameter;

  *stream = (scalar_handle_t *)hndl;
  return s_ok;
  }

static inline result_t as_float(stream_handle_t *hndl, float_handle_t **stream)
  {
  if (hndl->version != sizeof(float_handle_t))
    return e_bad_parameter;

  *stream = (float_handle_t *)hndl;
  return s_ok;
  }

static inline result_t as_datastream(stream_handle_t *hndl, deltastream_handle_t **stream)
  {
  if (hndl->version < sizeof(deltastream_handle_t))
    return e_bad_parameter;

  *stream = (deltastream_handle_t *)hndl;
  return s_ok;
  }


static inline uint8_t get_matissa(const float *value)
  {
  return ((const uint8_t *)value)[3];
  }

static inline uint32_t get_exponent(const float *value)
  {
  return ((uint32_t)((const uint8_t *)value)[0]) |
    (((uint32_t)((const uint8_t *)value)[1]) << 8) |
    (((uint32_t)((const uint8_t *)value)[2]) << 16);
  }

static inline float make_float(float_cursor_t *cursor)
  {
  uint8_t result[4];
  *((uint32_t *)&result[0]) = cursor->mantissa.value.uint32_val;
  result[3] = cursor->exponent;

  return *((float *)&result[0]);
  }

// flush any pending data
static result_t flush_cursor(deltastream_handle_t *stream, cursor_t *cursor)
  {

  }

// advance to the next byte
static result_t advance(deltastream_handle_t *stream, cursor_t *cursor)
  {

  }

static result_t read_bits(deltastream_handle_t *stream, cursor_t *cursor, uint16_t len, uint32_t *value)
  {
  // exp_value is only used if this is a floating point series.
  }

static result_t write_bits(deltastream_handle_t *stream, cursor_t *cursor, uint16_t len, uint32_t value)
  {

  }

static result_t encode_value(deltastream_handle_t *stream, cursor_t *cursor, int32_t delta,
  uint16_t num_bits, uint32_t msb_mask, uint16_t max_rle_bits)
  {
  result_t result;
  bool negative;
  int32_t work;
  uint16_t sig_bits;

  // determine the change type.
  if (delta == 0)
    {
    // just count it
    cursor->count++;
    }
  else
    {
    // see if end of counter.
    if (cursor->count > 0 &&
      failed(result = flush_cursor(stream, cursor)))
      return result;

    if ((delta & msb_mask) != 0)
      {
      negative = true;
      delta = 0 - delta;      // make +number
      }
    else
      negative = false;

    // work out where the first significant bit is.
    work = delta;
    for (sig_bits = num_bits; sig_bits > 0; sig_bits--)
      {
      if ((work & msb_mask) != 0)
        break;          // found first bit

      work <<= 1;       // next bit
      }

    if (sig_bits > max_rle_bits)
      {
      // just send the value after a 0000 flag, this means absolute
      if (failed(result = write_bits(stream, cursor, 4, 0)))
        return result;

      if (failed(result = write_bits(stream, cursor, num_bits, cursor->value.uint32_val)))
        return result;
      }
    else
      {
      // is a relative flag so send it
      if (failed(result = write_bits(stream, cursor, 3, negative ? 0x101 : 0x100)))
        return result;

      if (sig_bits > 0)
        sig_bits--;       // sent at n-1
      if (sig_bits < 2)
        {
        if (failed(result = write_bits(stream, cursor, 3, 1)))
          return result;

        if (failed(result = write_bits(stream, cursor, 1, sig_bits)))
          return result;
        }
      else if (sig_bits < 4)
        {
        if (failed(result = write_bits(stream, cursor, 3, 2)))
          return result;

        if (failed(result = write_bits(stream, cursor, 2, sig_bits)))
          return result;
        }
      else if (sig_bits < 8)
        {
        if (failed(result = write_bits(stream, cursor, 3, 3)))
          return result;

        if (failed(result = write_bits(stream, cursor, 3, sig_bits)))
          return result;
        }
      else if (sig_bits < 16)
        {
        if (failed(result = write_bits(stream, cursor, 3, 4)))
          return result;

        if (failed(result = write_bits(stream, cursor, 4, sig_bits)))
          return result;
        }
      else if (sig_bits < 32)
        {
        if (failed(result = write_bits(stream, cursor, 3, 5)))
          return result;

        if (failed(result = write_bits(stream, cursor, 5, sig_bits)))
          return result;
        }
      else
        return e_unexpected;

      sig_bits++;
      if (failed(result = write_bits(stream, cursor, sig_bits, delta)))
        return result;
      }
    }

  return s_ok;
  }

static result_t pack_uint(deltastream_handle_t *stream, cursor_t *cursor, uint32_t value)
  {
  // pack an integer where the length is len bits
  result_t result;
  // pack an integer where the length is len bits
  int32_t delta;
  uint16_t num_bits;
  uint16_t sig_bits;
  uint32_t msb_mask;
  // number of bits that need to be sent, or just send value
  uint16_t max_rle_bits;

  switch (stream->series.type)
    {
    case st_int16:
      delta = value - cursor->value.uint16_val;
      cursor->value.uint16_val = (uint16_t)value;
      msb_mask = 0x00008000;
      num_bits = 16;
      max_rle_bits = 9;
      break;
    case st_int32:
      // we treat the values as int32's so the logic works
      delta = *((int32_t *)&value) - cursor->value.int32_val;
      cursor->value.uint32_val = value;
      msb_mask = 0x80000000;
      num_bits = 32;
      max_rle_bits = 24;
      break;
    default:
      return e_bad_parameter;
    }

  return encode_value(stream, cursor, delta, num_bits, msb_mask, max_rle_bits);
  }

static result_t pack_int(deltastream_handle_t *stream, cursor_t *cursor, int32_t value)
  {
  result_t result;
  // pack an integer where the length is len bits
  int32_t delta;
  uint16_t num_bits;
  uint16_t sig_bits;
  uint32_t msb_mask;
  // number of bits that need to be sent, or just send value
  uint16_t max_rle_bits;

  switch (stream->series.type)
    {
    case st_int16 :
      delta = value - cursor->value.int16_val;
      cursor->value.int16_val = (int16_t)value;
      msb_mask = 0x00008000;
      num_bits = 16;
      max_rle_bits = 9;
      break;
    case st_int32 :
      delta = value - cursor->value.int32_val;
      cursor->value.int32_val = value;
      msb_mask = 0x80000000;
      num_bits = 32;
      max_rle_bits = 24;
      break;
    default:
      return e_bad_parameter;
    }

  return encode_value(stream, cursor, delta, num_bits, msb_mask, max_rle_bits);
  }

static result_t unpack_rle(deltastream_handle_t *stream, cursor_t *cursor, uint32_t *value)
  {
  uint32_t data;
  result_t result;

  if (failed(result = read_bits(stream, cursor, 3, &data)))
    return result;

  switch (data)
    {
    case 0 :
      // use the stream type as the length
      switch (stream->series.type)
        {
        case st_int16 :
        case st_uint16 :
          data = 15;
          break;
        default :
          data = 31;
          break;
        }
      break;
    case 1 :
      if (failed(result = read_bits(stream, cursor, 1, &data)))
        return result;
      break;
    case 2 :
      if (failed(result = read_bits(stream, cursor, 2, &data)))
        return result;
      break;
    case 3 :
      if (failed(result = read_bits(stream, cursor, 3, &data)))
        return result;
      break;
    case 4 :
      if (failed(result = read_bits(stream, cursor, 4, &data)))
        return result;
      break;
    case 5 :
      if (failed(result = read_bits(stream, cursor, 5, &data)))
        return result;
      break;
    default :
      return e_unexpected;
    }

  // the length bits are the number of significant bits-1
  data++;

  // and just read the bits from the stream
  return read_bits(stream, cursor, data, value);
  }

static result_t unpack_int16(deltastream_handle_t *hndl, int16_t *value)
  {
  uint32_t data;
  result_t result;

  scalar_handle_t *stream;
  if (failed(result = as_scalar(hndl, &stream)))
    return result;

  if (stream->reader.count == 0)
    {
    if (failed(result = read_bits(stream, &stream->reader, 1, &data)))
      return result;

    if (data == 0)
      {
      // absolute value
      // read the data
      if (failed(result = unpack_rle(stream, &stream->reader, &data)))
        return result;

      // and assign the data
      stream->reader.value.int16_val = (int16_t)(*((int32_t *)&data));
      }
    else
      {
      // relative or count.
      if (failed(result = read_bits(stream, &stream->reader, 1, &data)))
        return result;

      if (data == 0)
        {
        // this is a relative value to be added to the current

        // read the sign
        if (failed(result = read_bits(stream, &stream->reader, 1, &data)))
          return result;

        int16_t sign = data == 0 ? 1 : -1;
        // read the data
        if (failed(result = unpack_rle(stream, &stream->reader, &data)))
          return result;

        stream->reader.value.int16_val += ((int16_t)(*((int32_t *)&data)) * sign);
        }
      else
        {
        // is a count
        if (failed(result = unpack_rle(stream, &stream->reader, &data)))
          return result;

        // just store the count
        stream->reader.count = (uint16_t)data;
        }
      }
    }

  // we may have read a count from the stream
  if (stream->reader.count > 0)
    stream->reader.count--;                            // one less

  *value = stream->reader.value.int16_val;
  return s_ok;
  }

static result_t pack_int16(deltastream_handle_t *hndl, int16_t value)
  {
  uint32_t delta;
  result_t result;

  scalar_handle_t *stream;
  if (failed(result = as_scalar(hndl, &stream)))
    return result;

  if (stream->stream.series.type != st_int16)
    return e_bad_parameter;

  return pack_int(hndl, &stream->writer, value);
  }

static result_t unpack_int32(deltastream_handle_t *hndl, int32_t *value)
  {
  uint32_t data;
  result_t result;

  scalar_handle_t *stream;
  if (failed(result = as_scalar(hndl, &stream)))
    return result;

  if (stream->reader.count == 0)
    {
    if (failed(result = read_bits(stream, &stream->reader, 1, &data)))
      return result;

    if (data == 0)
      {
      // absolute value
      // read the data
      if (failed(result = unpack_rle(stream, &stream->reader, &data)))
        return result;

      // and assign the data
      stream->reader.value.int32_val = *((int32_t *)&data);
      }
    else
      {
      // relative or count.
      if (failed(result = read_bits(stream, &stream->reader, 1, &data)))
        return result;

      if (data == 0)
        {
        // this is a relative value to be added to the current

        // read the sign
        if (failed(result = read_bits(stream, &stream->reader, 1, &data)))
          return result;

        int16_t sign = data == 0 ? 1 : -1;
        // read the data
        if (failed(result = unpack_rle(stream, &stream->reader, &data)))
          return result;

        stream->reader.value.int32_val += (*((int32_t *)&data) * sign);
        }
      else
        {
        // is a count
        if (failed(result = unpack_rle(stream, &stream->reader, &data)))
          return result;

        // just store the count
        stream->reader.count = (uint16_t)data;
        }
      }
    }

  // we may have read a count from the stream
  if (stream->reader.count > 0)
    stream->reader.count--;                            // one less

  *value = stream->reader.value.int32_val;
  return s_ok;
  }

static result_t pack_int32(deltastream_handle_t *hndl, int32_t value)
  {
  uint32_t delta;
  result_t result;

  scalar_handle_t *stream;
  if (failed(result = as_scalar(hndl, &stream)))
    return result;

  if (stream->stream.series.type != st_int32)
    return e_bad_parameter;

  return pack_int(hndl, &stream->writer, value);
  }

static result_t unpack_uint16(deltastream_handle_t *hndl, uint16_t *value)
  {
  uint32_t data;
  result_t result;

  scalar_handle_t *stream;
  if (failed(result = as_scalar(hndl, &stream)))
    return result;

  if (stream->reader.count == 0)
    {
    if (failed(result = read_bits(stream, &stream->reader, 1, &data)))
      return result;

    if (data == 0)
      {
      // absolute value
      // read the data
      if (failed(result = unpack_rle(stream, &stream->reader, &data)))
        return result;

      // and assign the data
      stream->reader.value.int16_val = (uint16_t)data;
      }
    else
      {
      // relative or count.
      if (failed(result = read_bits(stream, &stream->reader, 1, &data)))
        return result;

      if (data == 0)
        {
        // this is a relative value to be added to the current

        // read the sign
        if (failed(result = read_bits(stream, &stream->reader, 1, &data)))
          return result;

        int16_t sign = data == 0 ? 1 : -1;
        // read the data
        if (failed(result = unpack_rle(stream, &stream->reader, &data)))
          return result;

        stream->reader.value.int16_val += ((int16_t)(*((int32_t *)&data)) * sign);
        }
      else
        {
        // is a count
        if (failed(result = unpack_rle(stream, &stream->reader, &data)))
          return result;

        // just store the count
        stream->reader.count = (uint16_t)data;
        }
      }
    }

  // we may have read a count from the stream
  if (stream->reader.count > 0)
    stream->reader.count--;                            // one less

  *value = stream->reader.value.uint16_val;
  return s_ok;
  }

static result_t pack_uint16(deltastream_handle_t *hndl, uint16_t value)
  {
  uint32_t delta;
  result_t result;

  scalar_handle_t *stream;
  if (failed(result = as_scalar(hndl, &stream)))
    return result;

  if (stream->stream.series.type != st_uint16)
    return e_bad_parameter;

  return pack_int(hndl, &stream->writer, value);
  }

static result_t unpack_uint32(deltastream_handle_t *hndl, uint32_t *value)
  {
  uint32_t data;
  result_t result;

  scalar_handle_t *stream;
  if (failed(result = as_scalar(hndl, &stream)))
    return result;

  if (stream->reader.count == 0)
    {
    if (failed(result = read_bits(stream, &stream->reader, 1, &data)))
      return result;

    if (data == 0)
      {
      // absolute value
      // read the data
      if (failed(result = unpack_rle(stream, &stream->reader, &data)))
        return result;

      // and assign the data
      stream->reader.value.uint32_val = data;
      }
    else
      {
      // relative or count.
      if (failed(result = read_bits(stream, &stream->reader, 1, &data)))
        return result;

      if (data == 0)
        {
        // this is a relative value to be added to the current

        // read the sign
        if (failed(result = read_bits(stream, &stream->reader, 1, &data)))
          return result;

        int16_t sign = data == 0 ? 1 : -1;
        // read the data
        if (failed(result = unpack_rle(stream, &stream->reader, &data)))
          return result;

        stream->reader.value.uint32_val += ((*((int32_t *)&data)) * sign);
        }
      else
        {
        // is a count
        if (failed(result = unpack_rle(stream, &stream->reader, &data)))
          return result;

        // just store the count
        stream->reader.count = (uint16_t)data;
        }
      }
    }

  // we may have read a count from the stream
  if (stream->reader.count > 0)
    stream->reader.count--;                            // one less

  *value = stream->reader.value.uint32_val;
  return s_ok;
  }

static result_t pack_uint32(deltastream_handle_t *hndl, uint32_t value)
  {
  uint32_t delta;
  result_t result;

  scalar_handle_t *stream;
  if (failed(result = as_scalar(hndl, &stream)))
    return result;

  if (stream->stream.series.type != st_uint32)
    return e_bad_parameter;

  return pack_int(hndl, &stream->writer, value);
  }

static result_t seek_to(deltastream_handle_t *hndl, uint32_t timestamp)
  {
  result_t result;
  deltastream_handle_t *ds;
  if (failed(result = as_datastream(hndl, &ds)))
    return result;

  if (ds->series.type != st_float)
    {
    scalar_handle_t *cursor;
    if (failed(result = as_scalar(hndl, &cursor)))
      return result;
    

    uint32_t delta;
    if (cursor->reader.offset > timestamp)
      {
      // reset the cursor back to the start.
      memset(&cursor->reader, 0, sizeof(cursor_t));
      // and first entry is the series timestamp
      cursor->reader.offset = ds->series.timestamp;
      }

    delta = timestamp - cursor->reader.offset;

    value_t value;
    // we start reading the values from the reader
    // until the timestamp matches
    while (delta-- > 0)
      {
      switch (ds->series.type)
        {
        case st_int16 :
          if (failed(result = unpack_int16(ds, &value.int16_val)))
            return result;
          break;
        case st_int32 :
          if (failed(result = unpack_int32(ds, &value.int32_val)))
            return result;
          break;
        case st_uint16 :
          if (failed(result = unpack_uint16(ds, &value.uint16_val)))
            return result;
          break;
        case st_uint32 :
          if (failed(result = unpack_uint32(ds, &value.uint32_val)))
            return result;
          break;
        default:
          return e_unexpected;
        }
      }
    }
  else
    {

    }

  return s_ok;
  }

static result_t write_values(deltastream_handle_t *stream, const void *values, uint16_t num_values)
  {

  }

static result_t read_values(deltastream_handle_t *stream, void *values, uint16_t num_values)
  {

  }

static result_t deltastream_eof(stream_handle_t *hndl)
  {

  return s_ok;
  }

static result_t deltastream_read(stream_handle_t *hndl, void *buffer, uint16_t size, uint16_t *read)
  {

  return s_ok;
  }

static result_t deltastream_write(stream_handle_t *hndl, const void *buffer, uint16_t size)
  {

  return s_ok;
  }

static result_t deltastream_getpos(stream_handle_t *hndl, uint32_t *pos)
  {

  return s_ok;
  }

static result_t deltastream_setpos(stream_handle_t *hndl, uint32_t pos)
  {

  return s_ok;
  }

static result_t deltastream_length(stream_handle_t *hndl, uint32_t *length)
  {

  return s_ok;
  }

static result_t deltastream_truncate(stream_handle_t *hndl, uint32_t length)
  {

  return s_ok;
  }

static result_t deltastream_close(stream_handle_t *hndl)
  {

  return s_ok;
  }

static result_t deltastream_delete(stream_handle_t *hndl)
  {

  return s_ok;
  }

static result_t deltastream_path(stream_handle_t *hndl, bool full_path, uint16_t len, char *path)
  {

  return s_ok;
  }

result_t create_delta_stream(memid_t parent,
  const char *stream_name,
  const char *name,
  series_type type,
  uint32_t timestamp,
  uint16_t rate,
  stream_p *stream)
  {
  // a deltastream is a stream on a standard registry stream.  We just create it here.

  return s_ok;
  }