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
////////////////////////////////////////////////////////////////////////////
//
// This canvas is based on the UPNG decoder
//
/*
uPNG -- derived from LodePNG version 20100808

Copyright (c) 2005-2010 Lode Vandevenne
Copyright (c) 2010 Sean Middleditch

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*/

#include "../neutron/neutron.h"
#include "photon.h"
#include "../neutron/bsp.h"
#include "../neutron/stream.h"

typedef enum upng_format {
  UPNG_BADFORMAT,
  UPNG_RGB8,
  UPNG_RGB16,
  UPNG_RGBA8,
  UPNG_RGBA16,
  UPNG_LUMINANCE1,
  UPNG_LUMINANCE2,
  UPNG_LUMINANCE4,
  UPNG_LUMINANCE8,
  UPNG_LUMINANCE_ALPHA8
  } upng_format;

#define FIRST_LENGTH_CODE_INDEX 257
#define LAST_LENGTH_CODE_INDEX 285

#define NUM_DEFLATE_CODE_SYMBOLS 288	/*256 literals, the end code, some length codes, and 2 unused codes */
#define NUM_DISTANCE_SYMBOLS 32	/*the distance codes have their own symbols, 30 used, 2 unused */
#define NUM_CODE_LENGTH_CODES 19	/*the code length codes. 0-15: code lengths, 16: copy previous 3-6 times, 17: 3-10 zeros, 18: 11-138 zeros */
#define MAX_SYMBOLS 288 /* largest number of symbols used by any tree type */

#define DEFLATE_CODE_BITLEN 15
#define DISTANCE_BITLEN 15
#define CODE_LENGTH_BITLEN 7
#define MAX_BIT_LENGTH 15 /* largest bitlen used by any tree type */

#define DEFLATE_CODE_BUFFER_SIZE (NUM_DEFLATE_CODE_SYMBOLS * 2)
#define DISTANCE_BUFFER_SIZE (NUM_DISTANCE_SYMBOLS * 2)
#define CODE_LENGTH_BUFFER_SIZE (NUM_DISTANCE_SYMBOLS * 2)

#define SET_ERROR(upng,code) do { (upng)->error = (code); (upng)->error_line = __LINE__; } while (0)

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
  } upng_color;

typedef enum {
  ct_stored,
  ct_haufman_fixed,
  ct_hauffman_dynamic
  } compression_type;

typedef struct _huffman_tree_t {
  uint16_t* tree2d;
  uint16_t maxbitlen;	/*maximum number of bits a single code can get */
  uint16_t numcodes;	/*number of symbols in the alphabet = number of codes */
  } huffman_tree_t;

typedef struct _upng_t
  {
  stream_handle_t stream;
  // only 1 of these can be valid at a time
  handle_t source;        // source stream if not 0
  uint16_t chunk0;        // start of the linked list of chunks.
                          // if 0 then has not been set yet.
  uint16_t chunk_start;   // where the chunk starts in the file
  uint16_t chunk_offset;  // current offset in the stream.
  uint16_t chunk_length;  // length of the current chunk
  uint16_t source_offset; // current source position in the decoder.
  // Compressed stream variables
  compression_type compression;       // true if reading from the hauffman decoder
  bool end_of_block;      // end of block found
  bool last_block;        // was last block
  // this is used by the decoder
  uint32_t bitpointer;    // if huaffman then this is a bit offset, otherwise length of block
  uint8_t data;

  huffman_tree_t *codetree;
  huffman_tree_t *codetreeD;
  huffman_tree_t *codelengthcodetree;
  uint16_t *codetree_buffer;
  uint16_t *codetreeD_buffer;
  uint16_t *codelengthcodetree_buffer;

  // these are read from the png stream
  uint16_t version;
  uint16_t width;
  uint16_t height;
  upng_color		color_type;
  uint16_t		color_depth;
  upng_format		format;
  byte_t *buffer;
  uint16_t	size;
  result_t error;
  uint16_t error_line;
  upng_state		state;
  } upng_t;

static inline result_t check_handle(handle_t hndl)
  {
  if (hndl == 0)
    return e_bad_parameter;

  stream_handle_t *stream = (stream_handle_t *)hndl;
  if (stream->version != sizeof(upng_t))
    return e_bad_parameter;

  return s_ok;
  }

static result_t get_next_chunk(upng_t *upng)
  {
  // skip
  result_t result;

  while (failed(stream_eof(upng->source)))
    {
    uint16_t next_chunk = upng->chunk_start + upng->chunk_length + 12;

    if (failed(result = stream_setpos(upng->source, next_chunk)))
      return result;

    uint32_t v_uint32;
    if (failed(result = read_uint32(upng->source, &v_uint32)))
      return result;
    upng->chunk_length = (uint16_t)v_uint32;

    if (failed(read_uint32(upng->source, &v_uint32)))
      return e_png_malformed;

    upng->chunk_start = next_chunk;
    upng->chunk_offset = 0;

    if(v_uint32 == make_uint32('I', 'D', 'A', 'T'))
      return s_ok;
    }

  return e_not_found;
  }

static result_t png_stream_eof(stream_handle_t *hndl)
  {
  result_t result;
  if (hndl == 0)
    return e_bad_parameter;

  if (failed(result = check_handle(hndl)))
    return result;

  upng_t *stream = (upng_t *)hndl;

  return stream_eof(stream->source);
  }

static result_t png_stream_read(stream_handle_t *hndl, void *buffer, uint16_t size, uint16_t *read)
  {
  result_t result;
  if (hndl == 0 || buffer == 0 || size == 0)
    return e_bad_parameter;

  if (failed(result = check_handle(hndl)))
    return result;

  uint8_t *bufp = (uint8_t *)buffer;

  upng_t *stream = (upng_t *)hndl;

  // read chunks as required
  while (size > 0)
    {
    uint16_t bytes_to_read = size;
    uint16_t bytes_available = stream->chunk_length - stream->chunk_offset;
    if(bytes_to_read > bytes_available)
      bytes_to_read = bytes_available;

    if(failed(result = stream_read(stream->source, bufp, bytes_to_read, 0)))
      return result;

    size -= bytes_to_read;
    stream->chunk_offset += bytes_to_read;
    stream->source_offset += bytes_to_read;
    bufp += bytes_to_read;

    // see if we need to read the next chunk
    if(stream->chunk_offset >= stream->chunk_length &&
       failed(result = get_next_chunk(stream)))
      return result;
    }

  return s_ok;
  }

static result_t png_stream_getpos(stream_handle_t *hndl, uint16_t *pos)
  {
  result_t result;
  if (hndl == 0 || pos == 0)
    return e_bad_parameter;

  if (failed(result = check_handle(hndl)))
    return result;

  upng_t *stream = (upng_t *)hndl;

  *pos = stream->source_offset;
  return s_ok;
  }

static result_t png_stream_setpos(stream_handle_t *hndl, uint16_t pos)
  {
  result_t result;
  if (hndl == 0)
    return e_bad_parameter;

  if (failed(result = check_handle(hndl)))
    return result;

  upng_t *stream = (upng_t *)hndl;

  // go to chunk 0
  stream_setpos(stream->source, stream->chunk0);
  // make sure this is set
  stream->chunk_start = stream->chunk0;

  // skip the 2 bytes zlib header.
  uint32_t value;
  // length
  read_uint32(stream->source, &value);
  stream->chunk_length = (uint16_t) value;

  uint16_t chunk_length = value -2;
  pos += 2;         // discard the zlib header
  stream->source_offset = 0;          // decompressed header positon

  while (pos > chunk_length)
    {
    if(failed(result = get_next_chunk(stream)))
      return result;

    stream->source_offset += chunk_length;      // skip to next block
    pos -= chunk_length;

    chunk_length = stream->chunk_length;
    }

  if(failed(result = stream_setpos(stream->source, stream->chunk_start + 8 + pos)))
    return result;

  stream->chunk_offset = pos;
  stream->source_offset += pos;

  return s_ok;
  }

static result_t png_stream_length(stream_handle_t *hndl, uint16_t *length)
  {
  result_t result;
  if (hndl == 0 || length == 0)
    return e_bad_parameter;

  if (failed(result = check_handle(hndl)))
    return result;

  upng_t *stream = (upng_t *)hndl;

  return stream_length(stream->source, length);
  }

static result_t png_stream_close(stream_handle_t *hndl)
  {
  result_t result;
  if (failed(result = check_handle(hndl)))
    return result;

  upng_t *stream = (upng_t *)hndl;


  neutron_free(stream);

  return result;
  }

static result_t open_png_stream(handle_t stream, upng_t **hndl)
  {
  result_t result;
  /*read the information from the header and store it in the upng_Info. return value is error*/

  uint16_t len;
  if (failed(result = stream_length(stream, &len)))
    return result;

  /* minimum length of a valid PNG file is 29 bytes
  * FIXME: verify this against the specification, or
  * better against the actual code below */
  if (len < 29)
    return e_not_png;

  upng_t *upng = (upng_t *)neutron_malloc(sizeof(upng_t));

  memset(upng, 0, sizeof(upng_t));

  upng->stream.version = sizeof(upng_t);
  upng->stream.stream_eof = png_stream_eof;
  upng->stream.stream_getpos = png_stream_getpos;
  upng->stream.stream_length = png_stream_length;
  upng->stream.stream_read = png_stream_read;
  upng->stream.stream_setpos = png_stream_setpos;
  upng->stream.stream_truncate = 0;
  upng->stream.stream_write = 0;
  upng->stream.stream_close = png_stream_close;
  upng->stream.stream_delete = 0;

  upng->color_type = UPNG_RGBA;
  upng->color_depth = 8;
  upng->format = UPNG_RGBA8;
  upng->state = UPNG_NEW;
  upng->error = s_ok;
  upng->source = stream;

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

  upng->chunk_length = v_uint32;

  if(failed(read_uint32(stream, &v_uint32)) ||
    v_uint32 != make_uint32('I', 'H', 'D', 'R'))
    return e_png_malformed;

  if (failed(result = read_uint32(stream, &v_uint32)))
    return result;

  upng->width = (uint16_t)v_uint32;

  if (failed(result = read_uint32(stream, &v_uint32)))
    return result;

  upng->height = (uint16_t)v_uint32;

  if (failed(result = stream_read(stream, &upng->color_depth, 1, 0)))
    return result;

  if (failed(result = stream_read(stream, &upng->color_type, 1, 0)))
    return result;

  /* determine our color format */
  switch (upng->color_type)
    {
    case UPNG_LUM:
      switch (upng->color_depth)
        {
        case 1:
          upng->format = UPNG_LUMINANCE1;
          break;
        case 2:
          upng->format = UPNG_LUMINANCE2;
          break;
        case 4:
          upng->format = UPNG_LUMINANCE4;
          break;
        case 8:
          upng->format = UPNG_LUMINANCE8;
          break;
        default:
          return e_png_bad_format;
        }
        break;
    case UPNG_RGB:
      switch (upng->color_depth)
        {
        case 8:
          upng->format = UPNG_RGB8;
          break;
        case 16:
          upng->format = UPNG_RGB16;
          break;
        default:
          return e_png_bad_format;
        }
        break;
    case UPNG_LUMA:
      switch (upng->color_depth)
        {
        case 8:
          upng->format = UPNG_LUMINANCE_ALPHA8;
          break;
        default:
          return e_png_bad_format;
        }
        break;
    case UPNG_RGBA:
      switch (upng->color_depth)
        {
        case 8:
          upng->format = UPNG_RGBA8;
          break;
        case 16:
          upng->format = UPNG_RGBA16;
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

  upng->chunk_start = 8;      // first 8 bytes are the header

  if(failed(result = get_next_chunk(upng)))
    return result;
  // create a decompression stream.  Uses the same logic as a stream
  // but will handle chunking of the file
  /* we require two bytes for the zlib data header */
  uint8_t hdr[2];
  if (failed(stream_read(upng, hdr, 2, 0)) ||
    ((hdr[0] * 256 + hdr[1]) % 31 != 0) ||                /* 256 * in[0] + in[1] must be a multiple of 31, the FCHECK value is supposed to be made that way */
    ((hdr[0] & 15) != 8 || ((hdr[0] >> 4) & 15) > 7) ||   /*error: only compression method 8: inflate with sliding window of 32k is supported by the PNG spec */
    (((hdr[1] >> 5) & 1) != 0))                            /* the specification of PNG says about the zlib stream: "The additional flags shall not specify a preset dictionary." */
    {
    return e_png_malformed;
    }

  *hndl = upng;

  return s_ok;
  }

static const uint16_t length_base[29] = {	/*the base lengths represented by codes 257-285 */
  3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59,
  67, 83, 99, 115, 131, 163, 195, 227, 258
  };

static const uint16_t length_extra[29] = {	/*the extra bits used by codes 257-285 (added to base length) */
  0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5,
  5, 5, 5, 0
  };

static const uint16_t distance_base[30] = {	/*the base backwards distances (the bits of distance codes appear after length codes and use their own huffman tree) */
  1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513,
  769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577
  };

static const uint16_t distance_extra[30] = {	/*the extra bits of backwards distances (added to base) */
  0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10,
  11, 11, 12, 12, 13, 13
  };

static const uint16_t clcl[NUM_CODE_LENGTH_CODES]	/*the order in which "code length alphabet code lengths" are stored, out of this the huffman tree of the dynamic huffman tree lengths is generated */
= { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };

static const uint16_t fixed_deflate_codetree[NUM_DEFLATE_CODE_SYMBOLS * 2] = {
  289, 370, 290, 307, 546, 291, 561, 292, 293, 300, 294, 297, 295, 296, 0, 1,
  2, 3, 298, 299, 4, 5, 6, 7, 301, 304, 302, 303, 8, 9, 10, 11, 305, 306, 12,
  13, 14, 15, 308, 339, 309, 324, 310, 317, 311, 314, 312, 313, 16, 17, 18,
  19, 315, 316, 20, 21, 22, 23, 318, 321, 319, 320, 24, 25, 26, 27, 322, 323,
  28, 29, 30, 31, 325, 332, 326, 329, 327, 328, 32, 33, 34, 35, 330, 331, 36,
  37, 38, 39, 333, 336, 334, 335, 40, 41, 42, 43, 337, 338, 44, 45, 46, 47,
  340, 355, 341, 348, 342, 345, 343, 344, 48, 49, 50, 51, 346, 347, 52, 53,
  54, 55, 349, 352, 350, 351, 56, 57, 58, 59, 353, 354, 60, 61, 62, 63, 356,
  363, 357, 360, 358, 359, 64, 65, 66, 67, 361, 362, 68, 69, 70, 71, 364,
  367, 365, 366, 72, 73, 74, 75, 368, 369, 76, 77, 78, 79, 371, 434, 372,
  403, 373, 388, 374, 381, 375, 378, 376, 377, 80, 81, 82, 83, 379, 380, 84,
  85, 86, 87, 382, 385, 383, 384, 88, 89, 90, 91, 386, 387, 92, 93, 94, 95,
  389, 396, 390, 393, 391, 392, 96, 97, 98, 99, 394, 395, 100, 101, 102, 103,
  397, 400, 398, 399, 104, 105, 106, 107, 401, 402, 108, 109, 110, 111, 404,
  419, 405, 412, 406, 409, 407, 408, 112, 113, 114, 115, 410, 411, 116, 117,
  118, 119, 413, 416, 414, 415, 120, 121, 122, 123, 417, 418, 124, 125, 126,
  127, 420, 427, 421, 424, 422, 423, 128, 129, 130, 131, 425, 426, 132, 133,
  134, 135, 428, 431, 429, 430, 136, 137, 138, 139, 432, 433, 140, 141, 142,
  143, 435, 483, 436, 452, 568, 437, 438, 445, 439, 442, 440, 441, 144, 145,
  146, 147, 443, 444, 148, 149, 150, 151, 446, 449, 447, 448, 152, 153, 154,
  155, 450, 451, 156, 157, 158, 159, 453, 468, 454, 461, 455, 458, 456, 457,
  160, 161, 162, 163, 459, 460, 164, 165, 166, 167, 462, 465, 463, 464, 168,
  169, 170, 171, 466, 467, 172, 173, 174, 175, 469, 476, 470, 473, 471, 472,
  176, 177, 178, 179, 474, 475, 180, 181, 182, 183, 477, 480, 478, 479, 184,
  185, 186, 187, 481, 482, 188, 189, 190, 191, 484, 515, 485, 500, 486, 493,
  487, 490, 488, 489, 192, 193, 194, 195, 491, 492, 196, 197, 198, 199, 494,
  497, 495, 496, 200, 201, 202, 203, 498, 499, 204, 205, 206, 207, 501, 508,
  502, 505, 503, 504, 208, 209, 210, 211, 506, 507, 212, 213, 214, 215, 509,
  512, 510, 511, 216, 217, 218, 219, 513, 514, 220, 221, 222, 223, 516, 531,
  517, 524, 518, 521, 519, 520, 224, 225, 226, 227, 522, 523, 228, 229, 230,
  231, 525, 528, 526, 527, 232, 233, 234, 235, 529, 530, 236, 237, 238, 239,
  532, 539, 533, 536, 534, 535, 240, 241, 242, 243, 537, 538, 244, 245, 246,
  247, 540, 543, 541, 542, 248, 249, 250, 251, 544, 545, 252, 253, 254, 255,
  547, 554, 548, 551, 549, 550, 256, 257, 258, 259, 552, 553, 260, 261, 262,
  263, 555, 558, 556, 557, 264, 265, 266, 267, 559, 560, 268, 269, 270, 271,
  562, 565, 563, 564, 272, 273, 274, 275, 566, 567, 276, 277, 278, 279, 569,
  572, 570, 571, 280, 281, 282, 283, 573, 574, 284, 285, 286, 287, 0, 0
  };

static const uint16_t fixed_distance_tree[NUM_DISTANCE_SYMBOLS * 2] = {
  33, 48, 34, 41, 35, 38, 36, 37, 0, 1, 2, 3, 39, 40, 4, 5, 6, 7, 42, 45, 43,
  44, 8, 9, 10, 11, 46, 47, 12, 13, 14, 15, 49, 56, 50, 53, 51, 52, 16, 17,
  18, 19, 54, 55, 20, 21, 22, 23, 57, 60, 58, 59, 24, 25, 26, 27, 61, 62, 28,
  29, 30, 31, 0, 0
  };

static uint8_t read_bit(upng_t *bitstream)
  {
  uint32_t cbp = bitstream->bitpointer;

  uint8_t result = (bitstream->data >> (cbp & 0x7)) & 1;
  bitstream->bitpointer++;

  if ((cbp & 0xfffffff8) != (bitstream->bitpointer & 0xfffffff8))
    stream_read(bitstream, &bitstream->data, 1, 0);

  return result;
  }

static uint16_t read_bits(upng_t *bitstream, uint16_t nbits)
  {
  uint16_t result = 0, i;
  for (i = 0; i < nbits; i++)
    result |= ((uint16_t)read_bit(bitstream)) << i;
  return result;
  }

/* the buffer must be numcodes*2 in size! */
static void huffman_tree_init(huffman_tree_t* tree, uint16_t* buffer, uint16_t numcodes, uint16_t maxbitlen)
  {
  tree->tree2d = buffer;
  tree->numcodes = numcodes;
  tree->maxbitlen = maxbitlen;
  }

/*given the code lengths (as stored in the PNG file), generate the tree as defined by Deflate.
maxbitlen is the maximum bits that a code in the tree can have. return value is error.*/
static void huffman_tree_create_lengths(upng_t* upng, huffman_tree_t* tree, const uint16_t *bitlen)
  {
  uint16_t tree1d[MAX_SYMBOLS];
  uint16_t blcount[MAX_BIT_LENGTH];
  uint16_t nextcode[MAX_BIT_LENGTH + 1];
  uint16_t bits, n, i;
  uint16_t nodefilled = 0;	/*up to which node it is filled */
  uint16_t treepos = 0;	/*position in the tree (1 of the numcodes columns) */

  /* initialize local vectors */
  memset(blcount, 0, sizeof(blcount));
  memset(nextcode, 0, sizeof(nextcode));

  /*step 1: count number of instances of each code length */
  for (bits = 0; bits < tree->numcodes; bits++) {
    blcount[bitlen[bits]]++;
    }

  /*step 2: generate the nextcode values */
  for (bits = 1; bits <= tree->maxbitlen; bits++) {
    nextcode[bits] = (nextcode[bits - 1] + blcount[bits - 1]) << 1;
    }

  /*step 3: generate all the codes */
  for (n = 0; n < tree->numcodes; n++) {
    if (bitlen[n] != 0) {
      tree1d[n] = nextcode[bitlen[n]]++;
      }
    }

  /*convert tree1d[] to tree2d[][]. In the 2D array, a value of 32767 means uninited, a value >= numcodes is an address to another bit, a value < numcodes is a code. The 2 rows are the 2 possible bit values (0 or 1), there are as many columns as codes - 1
     a good huffmann tree has N * 2 - 1 nodes, of which N - 1 are internal nodes. Here, the internal nodes are stored (what their 0 and 1 option point to). There is only memory for such good tree currently, if there are more nodes (due to too long length codes), error 55 will happen */
  for (n = 0; n < tree->numcodes * 2; n++) {
    tree->tree2d[n] = 32767;	/*32767 here means the tree2d isn't filled there yet */
    }

  for (n = 0; n < tree->numcodes; n++) {	/*the codes */
    for (i = 0; i < bitlen[n]; i++) {	/*the bits for this code */
      uint8_t bit = (uint8_t)((tree1d[n] >> (bitlen[n] - i - 1)) & 1);
      /* check if oversubscribed */
      if (treepos > tree->numcodes - 2) {
        SET_ERROR(upng, e_png_malformed);
        return;
        }

      if (tree->tree2d[2 * treepos + bit] == 32767) {	/*not yet filled in */
        if (i + 1 == bitlen[n]) {	/*last bit */
          tree->tree2d[2 * treepos + bit] = n;	/*put the current code in it */
          treepos = 0;
          }
        else {	/*put address of the next step in here, first that address has to be found of course (it's just nodefilled + 1)... */
          nodefilled++;
          tree->tree2d[2 * treepos + bit] = nodefilled + tree->numcodes;	/*addresses encoded with numcodes added to it */
          treepos = nodefilled;
          }
        }
      else {
        treepos = tree->tree2d[2 * treepos + bit] - tree->numcodes;
        }
      }
    }

  for (n = 0; n < tree->numcodes * 2; n++) {
    if (tree->tree2d[n] == 32767) {
      tree->tree2d[n] = 0;	/*remove possible remaining 32767's */
      }
    }
  }

static uint16_t huffman_decode_symbol(upng_t *upng, const huffman_tree_t* codetree)
  {
  uint16_t treepos = 0, ct;
  uint8_t bit;
  for (;;)
    {

    bit = read_bit(upng);

    ct = codetree->tree2d[(treepos << 1) | bit];
    if (ct < codetree->numcodes)
      {
      return ct;
      }

    treepos = ct - codetree->numcodes;
    if (treepos >= codetree->numcodes)
      {
      SET_ERROR(upng, e_png_malformed);
      return 0;
      }
    }
  }

/* get the tree of a deflated block with dynamic tree, the tree itself is also Huffman compressed with a known tree*/
static result_t get_tree_inflate_dynamic(upng_t* upng, huffman_tree_t* codetree, huffman_tree_t* codetreeD, huffman_tree_t* codelengthcodetree)
  {
  uint16_t codelengthcode[NUM_CODE_LENGTH_CODES];
  uint16_t bitlen[NUM_DEFLATE_CODE_SYMBOLS];
  uint16_t bitlenD[NUM_DISTANCE_SYMBOLS];
  uint16_t n, hlit, hdist, hclen, i;

  /* clear bitlen arrays */
  memset(bitlen, 0, sizeof(bitlen));
  memset(bitlenD, 0, sizeof(bitlenD));

  /*the bit pointer is or will go past the memory */
  hlit = read_bits(upng, 5) + 257;	/*number of literal/length codes + 257. Unlike the spec, the value 257 is added to it here already */
  hdist = read_bits(upng, 5) + 1;	/*number of distance codes. Unlike the spec, the value 1 is added to it here already */
  hclen = read_bits(upng, 4) + 4;	/*number of code length codes. Unlike the spec, the value 4 is added to it here already */

  for (i = 0; i < NUM_CODE_LENGTH_CODES; i++)
    {
    if (i < hclen)
      {
      codelengthcode[clcl[i]] = read_bits(upng, 3);
      }
    else
      {
      codelengthcode[clcl[i]] = 0;	/*if not, it must stay 0 */
      }
    }

  huffman_tree_create_lengths(upng, codelengthcodetree, codelengthcode);

  /* bail now if we encountered an error earlier */
  if (failed(upng->error))
    return upng->error;

  /*now we can use this tree to read the lengths for the tree that this function will return */
  i = 0;
  while (i < hlit + hdist)
    {	/*i is the current symbol we're reading in the part that contains the code lengths of lit/len codes and dist codes */
    uint16_t code = huffman_decode_symbol(upng, codelengthcodetree);
    if (upng->error != s_ok)
      {
      break;
      }

    if (code <= 15)
      {	/*a length code */
      if (i < hlit)
        {
        bitlen[i] = code;
        }
      else
        {
        bitlenD[i - hlit] = code;
        }
      i++;
      }
    else if (code == 16)
      {	/*repeat previous */
      uint16_t replength = 3;	/*read in the 2 bits that indicate repeat length (3-6) */
      uint16_t value;	/*set value to the previous code */


      /*error, bit pointer jumps past memory */
      replength += read_bits(upng, 2);

      if ((i - 1) < hlit)
        {
        value = bitlen[i - 1];
        }
      else
        {
        value = bitlenD[i - hlit - 1];
        }

      /*repeat this value in the next lengths */
      for (n = 0; n < replength; n++)
        {
        /* i is larger than the amount of codes */
        if (i >= hlit + hdist)
          {
          SET_ERROR(upng, e_png_malformed);
          break;
          }

        if (i < hlit)
          {
          bitlen[i] = value;
          }
        else
          {
          bitlenD[i - hlit] = value;
          }
        i++;
        }
      }
    else if (code == 17)
      {	/*repeat "0" 3-10 times */
      uint16_t replength = 3;	/*read in the bits that indicate repeat length */

      /*error, bit pointer jumps past memory */
      replength += read_bits(upng, 3);

      /*repeat this value in the next lengths */
      for (n = 0; n < replength; n++)
        {
        /* error: i is larger than the amount of codes */
        if (i >= hlit + hdist)
          {
          SET_ERROR(upng, e_png_malformed);
          break;
          }

        if (i < hlit)
          {
          bitlen[i] = 0;
          }
        else {
          bitlenD[i - hlit] = 0;
          }
        i++;
        }
      }
    else if (code == 18)
      {	/*repeat "0" 11-138 times */
      uint16_t replength = 11;	/*read in the bits that indicate repeat length */
      /* error, bit pointer jumps past memory */

      replength += read_bits(upng, 7);

      /*repeat this value in the next lengths */
      for (n = 0; n < replength; n++)
        {
        /* i is larger than the amount of codes */
        if (i >= hlit + hdist)
          {
          SET_ERROR(upng, e_png_malformed);
          break;
          }
        if (i < hlit)
          bitlen[i] = 0;
        else
          bitlenD[i - hlit] = 0;
        i++;
        }
      }
    else {
      /* somehow an unexisting code appeared. This can never happen. */
      SET_ERROR(upng, e_png_malformed);
      break;
      }
    }

  if (upng->error == s_ok && bitlen[256] == 0)
    {
    SET_ERROR(upng, e_png_malformed);
    }

  /*the length of the end code 256 must be larger than 0 */
  /*now we've finally got hlit and hdist, so generate the code trees, and the function is done */
  if (upng->error == s_ok)
    {
    huffman_tree_create_lengths(upng, codetree, bitlen);
    }
  if (upng->error == s_ok)
    {
    huffman_tree_create_lengths(upng, codetreeD, bitlenD);
    }
  }

/*inflate a block with dynamic of fixed Huffman tree*/
static result_t inflate_huffman(upng_t* upng, uint8_t *out, uint16_t outsize, uint16_t *bytes_read, uint16_t btype)
  {
  }

/*Paeth predicter, used by PNG filter type 4*/
static int paeth_predictor(int a, int b, int c)
  {
  int p = a + b - c;
  int pa = p > a ? p - a : a - p;
  int pb = p > b ? p - b : b - p;
  int pc = p > c ? p - c : c - p;

  if (pa <= pb && pa <= pc)
    return a;
  else if (pb <= pc)
    return b;
  else
    return c;
  }

static uint16_t upng_get_components(const upng_t* upng)
  {
  switch (upng->color_type) {
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

static uint16_t upng_get_bpp(const upng_t* upng)
  {
  return upng->color_depth * upng_get_components(upng);
  }

static inline uint32_t make_uint32(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
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

static result_t read_uint16(handle_t stream, uint16_t *value)
  {
  result_t result;
  uint8_t buffer[2];
  if (failed(result = stream_read(stream, buffer, 2, 0)))
    return result;

  *value = (buffer[2] << 8) | buffer[3];
  return s_ok;
  }

static result_t cleanup(upng_t *upng, result_t result)
  {
  if (upng->compression > 0)
    {
    neutron_free(upng->codetree);
    neutron_free(upng->codetreeD);

    if(upng->compression > 1)
      {
      neutron_free(upng->codetree_buffer);
      neutron_free(upng->codelengthcodetree);
      neutron_free(upng->codelengthcodetree_buffer);
      neutron_free(upng->codelengthcodetree);
      }

    upng->codetree = 0;
    upng->codetreeD = 0;
    upng->codetree_buffer = 0;
    upng->codelengthcodetree = 0;
    upng->codelengthcodetree_buffer = 0;
    upng->codelengthcodetree = 0;
    }

  return result;
  }

// read bytes from compressed stream into the buffer
static result_t decompress(upng_t *upng, uint8_t *out, uint16_t bytes)
  {
  result_t result;
  uint16_t bp = 0;	/*bit pointer in the "in" data, current byte is bp >> 3, current bit is bp & 0x7 (from lsb to msb of the byte) */
  uint16_t pos = 0;	/*byte position in the out buffer */
  uint16_t bytes_read = 0;
  
  while (bytes > 0)
    {
    if(upng->end_of_block)
      {
      if (upng->last_block)
        return cleanup(upng, e_no_more_information);

      /* read block control bits */
      // must be on a byte boundary
      upng->bitpointer = upng->source_offset << 3;
      upng->last_block = read_bit(upng);

      upng->compression = read_bit(upng) | (read_bit(upng) << 1);

      if (upng->compression == 0)
        {
        // this is a stored block so we read it.
        uint16_t len, nlen;

        // the underlying stream is at the next byte. 
        if (failed(result = read_uint16(upng, &len)) ||
          failed(result = read_uint16(upng, &nlen)))
          return cleanup(upng, result);

        /* read len (2 bytes) and nlen (2 bytes) */
        /* check if 16-bit nlen is really the one's complement of len */
        if (len + nlen != 65535)
          return cleanup(upng, e_png_malformed);

        // number of bytes in the stored block
        upng->bitpointer = len; 
        }
      else if (upng->compression == 1)
        {
        upng->codetree_buffer = fixed_deflate_codetree;
        upng->codelengthcodetree = fixed_distance_tree;
        upng->codetree = (huffman_tree_t *)neutron_malloc(sizeof(huffman_tree_t));
        upng->codetreeD = (huffman_tree_t *)neutron_malloc(sizeof(huffman_tree_t));
        /* fixed trees */
        huffman_tree_init(upng->codetree, upng->codetree_buffer, NUM_DEFLATE_CODE_SYMBOLS, DEFLATE_CODE_BITLEN);
        huffman_tree_init(upng->codetreeD, upng->codelengthcodetree, NUM_DISTANCE_SYMBOLS, DISTANCE_BITLEN);
        }
      else if (upng->compression == 2)
        {
        /* dynamic trees */
        upng->codetree_buffer = (uint16_t *)neutron_malloc(sizeof(uint16_t) * NUM_DEFLATE_CODE_SYMBOLS);
        upng->codelengthcodetree = (uint16_t *)neutron_malloc(sizeof(uint16_t) * NUM_DISTANCE_SYMBOLS);
        upng->codelengthcodetree_buffer = (uint16_t *)neutron_malloc(sizeof(uint16_t) * NUM_DISTANCE_SYMBOLS * 2);
        upng->codetree = (huffman_tree_t *)neutron_malloc(sizeof(huffman_tree_t));
        upng->codetreeD = (huffman_tree_t *)neutron_malloc(sizeof(huffman_tree_t));
        upng->codelengthcodetree = (huffman_tree_t *)neutron_malloc(sizeof(huffman_tree_t));

        huffman_tree_init(upng->codetree, upng->codetree_buffer, NUM_DEFLATE_CODE_SYMBOLS, DEFLATE_CODE_BITLEN);
        huffman_tree_init(upng->codetreeD, upng->codelengthcodetree, NUM_DISTANCE_SYMBOLS, DISTANCE_BITLEN);
        huffman_tree_init(upng->codelengthcodetree, upng->codelengthcodetree_buffer, NUM_CODE_LENGTH_CODES, CODE_LENGTH_BITLEN);
        get_tree_inflate_dynamic(upng, upng->codetree, upng->codetreeD, upng->codelengthcodetree);
        }
      }

    if(upng->compression != 0)
      {
      while (bytes > 0)
        {
        uint16_t code = huffman_decode_symbol(upng, upng->codetree);
        if (upng->error != s_ok)
          return cleanup(upng, upng->error);

        if (code == 256)
          {
          /* end code */
          upng->end_of_block = true;
          break;
          }
        else if (code <= 255)
          {
          /* store output */
          out[pos++] = (uint8_t) code;
          bytes--;
          }
        else if (code >= FIRST_LENGTH_CODE_INDEX && code <= LAST_LENGTH_CODE_INDEX)
          {	/*length code */
            /* part 1: get length base */
          uint16_t length = length_base[code - FIRST_LENGTH_CODE_INDEX];
          uint16_t codeD, distance, numextrabitsD;
          uint16_t start, forward, backward, numextrabits;

          /* part 2: get extra bits and add the value of that to length */
          numextrabits = length_extra[code - FIRST_LENGTH_CODE_INDEX];

          length += read_bits(upng, numextrabits);

          /*part 3: get distance code */
          codeD = huffman_decode_symbol(upng, upng->codetreeD);
          if (failed(upng->error))
            return cleanup(upng, upng->error);

          /* invalid distance code (30-31 are never used) */
          if (codeD > 29)
            return cleanup(upng, e_png_malformed);

          distance = distance_base[codeD];

          /*part 4: get extra bits from distance */
          numextrabitsD = distance_extra[codeD];

          distance += read_bits(upng, numextrabitsD);

          /*part 5: fill in all the out[n] values based on the length and dist */
          start = pos;
          backward = start - distance;

          for (forward = 0; forward < length; forward++)
            {
            out[pos++] = out[backward];
            backward++;

            if (backward >= start)
              backward = start - distance;
            }
          }
        }
      }
    else
      {
      if(failed(result = stream_read(upng, out, bytes, &bytes_read)))
        return cleanup(upng, result);

      if(bytes_read < bytes)
        upng->end_of_block = true;
      }

    bytes -= bytes_read;
    out += bytes_read;
    }

  return cleanup(upng, s_ok);
  }

static inline uint8_t get_bits(const uint8_t *buffer, uint16_t bitpos, uint8_t num_bits)
  {
  // bit 7 is the left most pixel
  uint16_t bitoff = (8 - num_bits) - (bitpos & 7);

  uint16_t result = buffer[bitpos >> 3] >> bitoff;
  result &= (1 << num_bits) - 1;

  return (uint8_t)result;
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

result_t create_png_canvas(handle_t stream, handle_t *hndl)
  {
  result_t result;
  if (stream == 0 || hndl == 0)
    return e_bad_parameter;

  upng_t *upng;
  if(failed(result = open_png_stream(stream, &upng)))
    return result;

  /* allocate space to store inflated (but still filtered) data */
  extent_t dim;
  dim.dx = upng->width;
  dim.dy = upng->height;

  handle_t canvas;
  if (failed(result = create_rect_canvas(&dim, &canvas)))
    {
    stream_close(upng);
    return result;
    }

  rect_t clip_rect = { 0, 0, upng->width, upng->height };
  uint16_t bpp = upng_get_bpp(upng);
  uint16_t x;
  uint16_t y;
  uint16_t bytewidth = (bpp + 7) >> 3;	/*bytewidth is used for filtering, is 1 when bpp < 8, number of bytes per pixel otherwise */
  uint16_t linebytes = (upng->width * bpp + 7) >> 3;

  // we allocate 3 buffer for the scanline decoder.  this makes a sliding window:
  // 0 -> previous un-converted scanline
  // 1 -> un-converted current scanline
  // 2 -> reconstituded scanline

  uint16_t bufp[3] = { 0, 0, linebytes };
  uint8_t *buffer;

  buffer = (uint8_t *)neutron_malloc(linebytes * upng->height);

  upng->end_of_block = true;
  // and read the first byte
  stream_read(upng, &upng->data, 1, 0);

  // read the next scanline
  if (failed(result = decompress(upng, buffer, upng->height * linebytes)))
    {
    neutron_free(buffer);
    canvas_close(canvas);
    stream_close(upng);

    return result;
    }


  // we now work through all of the chunks in the file
  for (y = 0; y < upng->height; y++)
    {

    /*
    For PNG filter method 0
    this function unfilters a single image (e.g. without interlacing this is called once, with Adam7 it's called 7 times)
    out must have enough bytes allocated already, in must have the scanlines + 1 filtertype byte per scanline
    x and y are image dimensions or dimensions of reduced image, bpp is bpp per pixel
    in and out are allowed to be the same memory address!
    */

    uint16_t inindex = (1 + linebytes) * y;	/*the extra filterbyte added to each row */
    uint8_t filterType = buffer[bufp[1] + inindex];

    /*
    For PNG filter method 0
    unfilter a PNG image scanline by scanline. when the pixels are smaller than 1 byte, the filter works byte per byte (bytewidth = 1)
    precon is the previous unfiltered scanline, recon the result, scanline the current one
    the incoming scanlines do NOT include the filtertype byte, that one is given in the parameter filterType instead
    recon and scanline MAY be the same memory address! precon must be disjoint.
    */

    uint16_t i;
    switch (filterType)
      {
      case 0:
        for (i = 0; i < linebytes; i++)
          buffer[bufp[2] + i] = buffer[bufp[1] + i];
        break;
      case 1:
        for (i = 0; i < bytewidth; i++)
          buffer[bufp[2] + i] = buffer[bufp[1] + i];
        for (i = bytewidth; i < linebytes; i++)
          buffer[bufp[2] + i] = buffer[bufp[1] + i] + buffer[bufp[2] + i - bytewidth];
        break;
      case 2:
        if (i > 0)
          for (i = 0; i < linebytes; i++)
            buffer[bufp[2] + i] = buffer[bufp[1] + i] + buffer[bufp[0] + i];
        else
          for (i = 0; i < linebytes; i++)
            buffer[bufp[2] + i] = buffer[bufp[1] + i];
        break;
      case 3:
        if (i > 0)
          {
          for (i = 0; i < bytewidth; i++)
            buffer[bufp[2] + i] = buffer[bufp[1] + i] + (buffer[bufp[0] + i] >> 1);
          for (i = bytewidth; i < linebytes; i++)
            buffer[bufp[2] + i] = buffer[bufp[1] + i] + ((buffer[bufp[2] + i - bytewidth] + buffer[bufp[0] + i]) >> 1);
          }
        else
          {
          for (i = 0; i < bytewidth; i++)
            buffer[bufp[2] + i] = buffer[bufp[1] + i];
          for (i = bytewidth; i < linebytes; i++)
            buffer[bufp[2] + i] = buffer[bufp[1] + i] + buffer[bufp[2] + i - bytewidth] / 2;
          }
        break;
      case 4:
        if (i > 0)
          {
          for (i = 0; i < bytewidth; i++)
            buffer[bufp[2] + i] = (uint8_t)(buffer[bufp[1] + i] + paeth_predictor(0, buffer[bufp[0] + i], 0));
          for (i = bytewidth; i < linebytes; i++)
            buffer[bufp[2] + i] = (uint8_t)(buffer[bufp[1] + i] + paeth_predictor(buffer[bufp[2] + i - bytewidth], buffer[bufp[0] + i], buffer[bufp[0] + i - bytewidth]));
          }
        else
          {
          for (i = 0; i < bytewidth; i++)
            buffer[bufp[2] + i] = buffer[bufp[1] + i];
          for (i = bytewidth; i < linebytes; i++)
            buffer[bufp[2] + i] = (uint8_t)(buffer[bufp[1] + i] + paeth_predictor(buffer[bufp[2] + i - bytewidth], 0, 0));
          }
        break;
      }

    uint16_t bitpos = 0;
    color_t pixel_color;
    // copy the buffer
    for (x = 0; x < upng->width; x++)
      {
      point_t pixel = { x, y };
      switch (upng->format)
        {
        case UPNG_RGB8:
          set_pixel(canvas, &clip_rect, &pixel, rgb(buffer[bufp[1] + bitpos + 0], buffer[bufp[1] + bitpos + 1], buffer[bufp[1] + bitpos + 2]), 0);
          bitpos += 3;
          break;
        case UPNG_RGB16:
          set_pixel(canvas, &clip_rect, &pixel, rgb(buffer[bufp[1] + bitpos + 0], buffer[bufp[1] + bitpos + 2], buffer[bufp[1] + bitpos + 4]), 0);
          bitpos += 8;
          break;
        case UPNG_RGBA8:
          set_pixel(canvas, &clip_rect, &pixel, rgba(buffer[bufp[1] + bitpos + 3], buffer[bufp[1] + bitpos + 0], buffer[bufp[1] + bitpos + 1], buffer[bufp[1] + bitpos + 2]), 0);
          bitpos += 4;
          break;
        case UPNG_RGBA16:
          set_pixel(canvas, &clip_rect, &pixel, rgba(buffer[bufp[1] + bitpos + 6], buffer[bufp[1] + bitpos + 0], buffer[bufp[1] + bitpos + 2], buffer[bufp[1] + bitpos + 4]), 0);
          bitpos += 8;
          break;
        case UPNG_LUMINANCE1:
          // 1 bit grayscale
          set_pixel(canvas, &clip_rect, &pixel, get_grayscale(get_bits(buffer[bufp[1]], bitpos, 1), 1, 255), 0);
          bitpos++;
          break;
        case UPNG_LUMINANCE2:
          // 2 bit grayscale
          set_pixel(canvas, &clip_rect, &pixel, get_grayscale(get_bits(buffer[bufp[1]], bitpos, 2), 2, 255), 0);
          bitpos += 2;
          break;
        case UPNG_LUMINANCE4:
          // 4 bit grayscale
          set_pixel(canvas, &clip_rect, &pixel, get_grayscale(get_bits(buffer[bufp[1]], bitpos, 4), 4, 255), 0);
          bitpos += 4;
          break;
        case UPNG_LUMINANCE8:
          // 1 byte grayscale
          set_pixel(canvas, &clip_rect, &pixel, get_grayscale(get_bits(buffer[bufp[1]], bitpos, 8), 8, 255), 0);
          bitpos += 8;
          break;
        case UPNG_LUMINANCE_ALPHA8:
        {
        uint8_t colorref = get_bits(buffer[bufp[1]], bitpos, 8);
        bitpos += 8;
        set_pixel(canvas, &clip_rect, &pixel, get_grayscale(colorref, 8, get_bits(buffer[bufp[1]], bitpos, 8)), 0);
        bitpos += 8;
        }
        break;
        }
      }


    // rotate the buffer
    bufp[0] = bufp[1];
    bufp[1] = bufp[2];
    bufp[2] = bufp[1] + linebytes;
    }

  neutron_free(buffer);

  if(succeeded(upng->error))
    *hndl = canvas;
  else
    canvas_close(canvas);

  stream_close(upng);
  return upng->error;
  }