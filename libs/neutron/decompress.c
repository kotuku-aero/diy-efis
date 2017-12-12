#include "neutron.h"

#define NUM_CODE_LENGTH_CODES 19	/*the code length_code codes. 0-15: code lengths, 16: copy previous 3-6 times, 17: 3-10 zeros, 18: 11-138 zeros */
#define NUM_DEFLATE_CODE_SYMBOLS 288	/*256 literals, the end code, some length_code codes, and 2 unused codes */
#define NUM_DISTANCE_SYMBOLS 32	/*the distance codes have their own symbols, 30 used, 2 unused */

#define FIRST_LENGTH_CODE_INDEX 257
#define LAST_LENGTH_CODE_INDEX 285

#define MAX_SYMBOLS 288 /* largest number of symbols used by any tree type */

#define DEFLATE_CODE_BITLEN 15
#define DISTANCE_BITLEN 15
#define CODE_LENGTH_BITLEN 7
#define MAX_BIT_LENGTH 15 /* largest bitlen used by any tree type */

///////////////////////////////////////////////////////////////////////////////
//
// Hauffman decoder
//

static const uint16_t length_base[29] = {	/*the base lengths represented by codes 257-285 */
  3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59,
  67, 83, 99, 115, 131, 163, 195, 227, 258
  };

static const uint16_t length_extra[29] = {	/*the extra bits used by codes 257-285 (added to base length_code) */
  0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5,
  5, 5, 5, 0
  };

static const uint16_t distance_base[30] = {	/*the base backwards distances (the bits of distance codes appear after length_code codes and use their own huffman tree) */
  1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513,
  769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577
  };

static const uint16_t distance_extra[30] = {	/*the extra bits of backwards distances (added to base) */
  0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10,
  11, 11, 12, 12, 13, 13
  };

static const uint16_t code_lengths_order[NUM_CODE_LENGTH_CODES]	/*the order in which "code length_code alphabet code lengths" are stored, out of this the huffman tree of the dynamic huffman tree lengths is generated */
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


typedef enum {
  ct_stored,
  ct_haufman_fixed,
  ct_hauffman_dynamic
  } compression_type;

typedef uint16_t huffman_node_t[2];

typedef struct _huffman_tree_t {
  huffman_node_t *tree2d;   // this is [numcodes] long
  uint16_t maxbitlen;	/*maximum number of bits a single code can get */
  uint16_t numcodes;	/*number of symbols in the alphabet = number of codes */
  } huffman_tree_t;

typedef struct _decoder_state_t {
  // Compressed stream variables
  compression_type compression;       // true if reading from the hauffman decoder
  bool end_of_block;      // end of block found
  bool last_block;        // was last block
                          // this is used by the decoder
  uint32_t bitpointer;    // if huaffman then this is a bit offset, otherwise length_code of block
  uint8_t data;

  huffman_tree_t *codetree;
  huffman_tree_t *codetree_distances;
  huffman_tree_t *codelengthcodetree;
  huffman_node_t *codetree_buffer;
  huffman_node_t *codetree_distance_buffer;
  huffman_node_t *codelengthcodetree_buffer;
  } decoder_state_t;

static uint8_t read_bit(decoder_state_t *bitstream)
  {
  uint32_t cbp = bitstream->bitpointer;

  uint8_t result = (bitstream->data >> (cbp & 0x7)) & 1;
  bitstream->bitpointer++;

  if ((cbp & 0xfffffff8) != (bitstream->bitpointer & 0xfffffff8))
    stream_read(bitstream, &bitstream->data, 1, 0);

  return result;
  }

static uint16_t read_bits(decoder_state_t *bitstream, uint16_t nbits)
  {
  uint16_t result = 0, i;
  for (i = 0; i < nbits; i++)
    result |= ((uint16_t)read_bit(bitstream)) << i;
  return result;
  }

/* the buffer must be numcodes*2 in size! */
static void huffman_tree_init(huffman_tree_t* tree, huffman_node_t *buffer, uint16_t numcodes, uint16_t maxbitlen)
  {
  tree->tree2d = buffer;
  tree->numcodes = numcodes;
  tree->maxbitlen = maxbitlen;
  }

/*given the code lengths, generate the tree as defined by Deflate.
maxbitlen is the maximum bits that a code in the tree can have. return value is error.*/
static result_t huffman_tree_create_lengths(decoder_state_t* decoder, huffman_tree_t* tree, const uint16_t *bitlen)
  {
  uint16_t *tree1d = (uint16_t *)neutron_malloc(sizeof(int16_t) * tree->numcodes);
  //uint16_t tree1d[MAX_SYMBOLS];
  uint16_t *blcount = (uint16_t *)neutron_malloc(sizeof(int16_t) * MAX_BIT_LENGTH);
  //uint16_t blcount[MAX_BIT_LENGTH];
   uint16_t *nextcode = (uint16_t *)neutron_malloc(sizeof(int16_t) * (MAX_BIT_LENGTH + 1));
  //uint16_t nextcode[MAX_BIT_LENGTH+1];

  uint16_t bits, n, i;
  uint16_t nodefilled = 0;	/*up to which node it is filled */
  uint16_t treepos = 0;	/*position in the tree (1 of the numcodes columns) */

                        /* initialize local vectors */
  memset(blcount, 0, sizeof(int16_t) * MAX_BIT_LENGTH);
  memset(nextcode, 0, sizeof(int16_t) * (MAX_BIT_LENGTH + 1));

  /*step 1: count number of instances of each code length_code */
  for (bits = 0; bits < tree->numcodes; bits++)
    blcount[bitlen[bits]]++;

  /*step 2: generate the nextcode values */
  for (bits = 1; bits <= tree->maxbitlen; bits++)
    nextcode[bits] = (nextcode[bits - 1] + blcount[bits - 1]) << 1;

  /*step 3: generate all the codes */
  for (n = 0; n < tree->numcodes; n++)
    {
    if (bitlen[n] != 0)
      tree1d[n] = nextcode[bitlen[n]]++;
    }

  /*
  convert tree1d[] to tree2d[][]. In the 2D array, a value of 32767 means uninited,
  a value >= numcodes is an address to another bit, a value < numcodes is a code.
  The 2 rows are the 2 possible bit values (0 or 1), there are as many columns as codes - 1
  a good huffmann tree has N * 2 - 1 nodes, of which N - 1 are internal nodes.
  Here, the internal nodes are stored (what their 0 and 1 option point to). There is only
  memory for such good tree currently, if there are more nodes (due to too long length_code codes),
  error 55 will happen */

  for (n = 0; n < tree->numcodes; n++)
    {
    tree->tree2d[n][0] = 32767;	/*32767 here means the tree2d isn't filled there yet */
    tree->tree2d[n][1] = 32767;
    }

  for (n = 0; n < tree->numcodes; n++)
    {	/*the codes */
    uint16_t length_of_code = bitlen[n];
    uint16_t bits = tree1d[n];
    for (i = 0; i < length_of_code; i++)
      {	
      /* check if oversubscribed */
      if (treepos >= tree->numcodes)
        {
        neutron_free(tree1d);
        neutron_free(blcount);
        neutron_free(nextcode);
        return e_generic_error;
        }

      /*the bits for this code */
      uint16_t shift = length_of_code - i - 1;
      uint8_t bit = (uint8_t)((bits >> shift) & 1);

      if(tree->tree2d[treepos][bit] == 32767)
        {	
        /*not yet filled in */

        if (i + 1 == length_of_code)
          {	
          /*last bit */
          tree->tree2d[treepos][bit] = n;
          treepos = 0;
          }
        else
          {	/*put address of the next step in here, first that address has to be found of course (it's just nodefilled + 1)... */
          nodefilled++;
          tree->tree2d[treepos][bit] = nodefilled + tree->numcodes;	/*addresses encoded with numcodes added to it */
          treepos = nodefilled;
          }
        }
      else
        {
        treepos = tree->tree2d[treepos][bit] - tree->numcodes;
        }
      }
    }

  for (n = 0; n < tree->numcodes; n++)
    {
    if (tree->tree2d[n][0] == 32767)
      tree->tree2d[n][0] = 0;	/*remove possible remaining 32767's */
    if (tree->tree2d[n][1] == 32767)
      tree->tree2d[n][1] = 0;	/*remove possible remaining 32767's */
    }

  neutron_free(tree1d);
  neutron_free(blcount);
  neutron_free(nextcode);

  return s_ok;
  }

static result_t huffman_decode_symbol(decoder_state_t *decoder, const huffman_tree_t* codetree, uint16_t *value)
  {
  uint16_t treepos = 0, ct;
  uint8_t bit;
  for (;;)
    {

    bit = read_bit(decoder);

    // walk the tree
    ct = codetree->tree2d[treepos][bit];
    if (ct < codetree->numcodes)
      {
      *value = ct;
      return s_ok;
      }

    treepos = ct - codetree->numcodes;
    if (treepos >= codetree->numcodes)
      {
      return e_unexpected;
      }
    }
  }

/* get the tree of a deflated block with dynamic tree, the tree itself is also Huffman compressed with a known tree*/
static result_t get_tree_inflate_dynamic(decoder_state_t* decoder, huffman_tree_t* codetree, huffman_tree_t* codetree_distance, huffman_tree_t* codelengthcodetree)
  {
  result_t result = s_ok;
  uint16_t n;
  uint16_t hlit;
  uint16_t hdist;
  uint16_t hclen;
  uint16_t i;

  /*the bit pointer is or will go past the memory */
  hlit = read_bits(decoder, 5) + 257;	/*number of literal/length_code codes + 257. Unlike the spec, the value 257 is added to it here already */
  hdist = read_bits(decoder, 5) + 1;	/*number of distance codes. Unlike the spec, the value 1 is added to it here already */
  hclen = read_bits(decoder, 4) + 4;	/*number of code length_code codes. Unlike the spec, the value 4 is added to it here already */

  if(hlit > NUM_DEFLATE_CODE_SYMBOLS || hdist > NUM_DISTANCE_SYMBOLS)
    return e_bad_parameter;

  uint16_t *codelengthcode = (uint16_t *)neutron_malloc(sizeof(uint16_t) * NUM_CODE_LENGTH_CODES);
  uint16_t *bitlen = (uint16_t *)neutron_malloc(sizeof(uint16_t) * NUM_DEFLATE_CODE_SYMBOLS);
  uint16_t *bitlength_distance = (uint16_t *)neutron_malloc(sizeof(uint16_t) * NUM_DISTANCE_SYMBOLS);

  /* clear bitlen arrays */
  memset(bitlen, 0, sizeof(uint16_t) * NUM_DEFLATE_CODE_SYMBOLS);
  memset(bitlength_distance, 0, sizeof(uint16_t) * NUM_DISTANCE_SYMBOLS);

  for(i = 0; i < hclen; i++)
    codelengthcode[code_lengths_order[i]] = read_bits(decoder, 3);
  for (; i < NUM_CODE_LENGTH_CODES; i++)
    codelengthcode[code_lengths_order[i]] = 0;	/*if not, it must stay 0 */

  if (failed(result = huffman_tree_create_lengths(decoder, codelengthcodetree, codelengthcode)))
    {
    neutron_free(codelengthcode);
    neutron_free(bitlen);
    neutron_free(bitlength_distance);
    return result;
    }

  /*now we can use this tree to read the lengths for the tree that this function will return */
  i = 0;
  while (i < hlit + hdist)
    {	
    /*i is the current symbol we're reading in the part that contains the code lengths of lit/len codes and dist codes */
    uint16_t code;

    if(failed(result = huffman_decode_symbol(decoder, codelengthcodetree, &code)))
      break;

    if (code <= 15)
      {	/*a length_code code */
      if (i < hlit)
        {
        bitlen[i] = code;
        }
      else
        {
        bitlength_distance[i - hlit] = code;
        }
      i++;
      }
    else if (code == 16)
      {	/*repeat previous */
      uint16_t replength = 3;	/*read in the 2 bits that indicate repeat length_code (3-6) */
      uint16_t value;	/*set value to the previous code */

      replength += read_bits(decoder, 2);

      if ((i - 1) < hlit)
        {
        value = bitlen[i - 1];
        }
      else
        {
        value = bitlength_distance[i - hlit - 1];
        }

      /*repeat this value in the next lengths */
      for (n = 0; n < replength; n++)
        {
        /* i is larger than the amount of codes */
        if (i >= hlit + hdist)
          {
          result = e_unexpected;
          break;
          }

        if (i < hlit)
          {
          bitlen[i] = value;
          }
        else
          {
          bitlength_distance[i - hlit] = value;
          }
        i++;
        }
      }
    else if (code == 17)
      {	/*repeat "0" 3-10 times */
      uint16_t replength = 3;	/*read in the bits that indicate repeat length_code */

                              /*error, bit pointer jumps past memory */
      replength += read_bits(decoder, 3);

      /*repeat this value in the next lengths */
      for (n = 0; n < replength; n++)
        {
        /* error: i is larger than the amount of codes */
        if (i >= hlit + hdist)
          {
          result = e_unexpected;
          break;
          }

        if (i < hlit)
          {
          bitlen[i] = 0;
          }
        else {
          bitlength_distance[i - hlit] = 0;
          }
        i++;
        }
      }
    else if (code == 18)
      {	/*repeat "0" 11-138 times */
      uint16_t replength = 11;	/*read in the bits that indicate repeat length_code */
      replength += read_bits(decoder, 7);

      /*repeat this value in the next lengths */
      for (n = 0; n < replength; n++)
        {
        /* i is larger than the amount of codes */
        if (i >= hlit + hdist)
          {
          result = e_unexpected;
          break;
          }
        if (i < hlit)
          bitlen[i] = 0;
        else
          bitlength_distance[i - hlit] = 0;
        i++;
        }
      }
    else
    {
      /* somehow an unexisting code appeared. This can never happen. */
      result = e_unexpected;
      break;
      }
    }

  // check for an end of block code.
  if (succeeded(result) && bitlen[256] == 0)
    result = e_unexpected;

  /*the length_code of the end code 256 must be larger than 0 */
  /*now we've finally got hlit and hdist, so generate the code trees, and the function is done */
  if (succeeded(result))
    result = huffman_tree_create_lengths(decoder, codetree, bitlen);

  if (succeeded(result))
    result = huffman_tree_create_lengths(decoder, codetree_distance, bitlength_distance);

  neutron_free(codelengthcode);
  neutron_free(bitlen);
  neutron_free(bitlength_distance);

  return result;
  }

static result_t cleanup(decoder_state_t *state, result_t result)
  {
  if (state->compression > 0)
    {
    neutron_free(state->codetree);
    neutron_free(state->codetree_distances);

    if (state->compression > 1)
      {
      neutron_free(state->codetree_buffer);
      neutron_free(state->codetree_distance_buffer);
      neutron_free(state->codelengthcodetree_buffer);
      neutron_free(state->codelengthcodetree);
      }

    state->codetree = 0;
    state->codetree_distances = 0;
    state->codetree_buffer = 0;
    state->codelengthcodetree = 0;
    state->codelengthcodetree_buffer = 0;
    state->codetree_distance_buffer = 0;
    }


  return result;
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

// read bytes from compressed stream into the buffer
result_t decompress(handle_t stream, handle_t parg, get_byte_fn getter, set_byte_fn setter, uint32_t *length)
  {
  result_t result;
  uint32_t bp = 0;	/*bit pointer in the "in" data, current byte is bp >> 3, current bit is bp & 0x7 (from lsb to msb of the byte) */
  uint32_t pos = 0;	/*byte position in the out buffer */
  uint32_t bytes_read = 0;

  decoder_state_t decoder;
  memset(&decoder, 0, sizeof(decoder));

  decoder.end_of_block = true;
  uint16_t len, nlen;

  // and read the first byte
  stream_read(stream, &decoder.data, 1, 0);

  if(length != 0)
    *length = 0;

  while (true)
    {
    if (decoder.end_of_block)
      {
      if (decoder.last_block)
        return cleanup(&decoder, s_ok);

      /* read block control bits */
      // must be on a byte boundary
      decoder.bitpointer = 0;
      decoder.last_block = read_bit(&decoder);

      decoder.compression = read_bit(&decoder) | (read_bit(&decoder) << 1);

      if (decoder.compression == 0)
        {
        // this is a stored block so we read it.

        // the underlying stream is at the next byte. 
        if (failed(result = read_uint16(stream, &len)) ||
          failed(result = read_uint16(stream, &nlen)))
          return cleanup(&decoder, result);

        /* read len (2 bytes) and nlen (2 bytes) */
        /* check if 16-bit nlen is really the one's complement of len */
        if (len + nlen != 65535)
          return cleanup(&decoder, e_unexpected);
        }
      else if (decoder.compression == 1)
        {
        decoder.codetree_buffer = fixed_deflate_codetree;
        decoder.codetree_distance_buffer = fixed_distance_tree;
        decoder.codetree = (huffman_tree_t *)neutron_malloc(sizeof(huffman_tree_t));
        decoder.codetree_distances = (huffman_tree_t *)neutron_malloc(sizeof(huffman_tree_t));
        /* fixed trees */
        huffman_tree_init(decoder.codetree, decoder.codetree_buffer, NUM_DEFLATE_CODE_SYMBOLS, DEFLATE_CODE_BITLEN);
        huffman_tree_init(decoder.codetree_distances, decoder.codetree_distance_buffer, NUM_DISTANCE_SYMBOLS, DISTANCE_BITLEN);
        }
      else if (decoder.compression == 2)
        {
        /* dynamic trees */
        decoder.codetree_buffer = (huffman_node_t *)neutron_malloc(sizeof(huffman_node_t) * NUM_DEFLATE_CODE_SYMBOLS);
        memset(decoder.codetree_buffer, 0, sizeof(huffman_node_t) * NUM_DEFLATE_CODE_SYMBOLS);

        decoder.codetree_distance_buffer = (huffman_node_t *)neutron_malloc(sizeof(huffman_node_t) * NUM_DISTANCE_SYMBOLS);
        memset(decoder.codetree_distance_buffer, 0, sizeof(huffman_node_t) * NUM_DISTANCE_SYMBOLS);

        decoder.codelengthcodetree_buffer = (huffman_node_t *)neutron_malloc(sizeof(huffman_node_t) * NUM_CODE_LENGTH_CODES);
        memset(decoder.codelengthcodetree_buffer, 0, sizeof(huffman_node_t) * NUM_CODE_LENGTH_CODES);

        decoder.codetree = (huffman_tree_t *)neutron_malloc(sizeof(huffman_tree_t));
        decoder.codetree_distances = (huffman_tree_t *)neutron_malloc(sizeof(huffman_tree_t));
        decoder.codelengthcodetree = (huffman_tree_t *)neutron_malloc(sizeof(huffman_tree_t));

        huffman_tree_init(decoder.codetree, decoder.codetree_buffer, NUM_DEFLATE_CODE_SYMBOLS, DEFLATE_CODE_BITLEN);
        huffman_tree_init(decoder.codetree_distances, decoder.codetree_distance_buffer, NUM_DISTANCE_SYMBOLS, DISTANCE_BITLEN);
        huffman_tree_init(decoder.codelengthcodetree, decoder.codelengthcodetree_buffer, NUM_CODE_LENGTH_CODES, CODE_LENGTH_BITLEN);

        if(failed(result = get_tree_inflate_dynamic(&decoder, decoder.codetree, decoder.codetree_distances, decoder.codelengthcodetree)))
          return cleanup(&decoder, result);
        }
      }

    if (decoder.compression != 0)
      {
      while (true)
        {
        uint16_t code;
        
        if(failed(result = huffman_decode_symbol(&decoder, decoder.codetree, &code)))
          return cleanup(&decoder, result);

        if (code == 256)
          {
          /* end code */
          decoder.end_of_block = true;
          break;
          }
        else if (code <= 255)
          {
          /* store output */
          if(failed(result = (*setter)(parg, pos++, (uint8_t)code)))
            return cleanup(&decoder, result);

          if(length != 0)
            *length = *length + 1;
          }
        else if (code >= FIRST_LENGTH_CODE_INDEX && code <= LAST_LENGTH_CODE_INDEX)
          {	/*length_code code */
            /* part 1: get length_code base */
          uint32_t length_code = length_base[code - FIRST_LENGTH_CODE_INDEX];
          uint16_t codeD, distance, numextrabitsD;
          uint32_t start, forward, backward, numextrabits;

          /* part 2: get extra bits and add the value of that to length_code */
          numextrabits = length_extra[code - FIRST_LENGTH_CODE_INDEX];

          length_code += read_bits(&decoder, numextrabits);

          /*part 3: get distance code */
          if(failed(result = huffman_decode_symbol(&decoder, decoder.codetree_distances, &codeD)))
            return cleanup(&decoder, result);

          /* invalid distance code (30-31 are never used) */
          if (codeD > 29)
            return e_unexpected;

          distance = distance_base[codeD];

          /*part 4: get extra bits from distance */
          numextrabitsD = distance_extra[codeD];

          distance += read_bits(&decoder, numextrabitsD);

          /*part 5: fill in all the out[n] values based on the length_code and dist */
          start = pos;
          backward = start - distance;

          for (forward = 0; forward < length_code; forward++)
            {
            uint8_t value;
            if(failed(result = (*getter)(parg, backward, &value)))
              return cleanup(&decoder, result);

            if(failed(result = (*setter)(parg, pos++, value)))
              return result;

            if (*length != 0)
              *length = *length + 1;

            backward++;

            if (backward >= start)
              backward = start - distance;
            }
          }
        }
      }
    else
      {
      uint8_t byte;
      for (bytes_read = 0; bytes_read < len; bytes_read++)
        {
        uint16_t num_read = 0;

        if (failed(result = stream_read(stream, &byte, 1, &num_read)))
          return result;

        if (bytes_read == 0)
          {
          decoder.end_of_block = true;
          break;
          }
        else
          if(failed(result =(*setter)(parg, pos++, byte)))
            return result;
        }

      if(length != 0)
        *length = *length + len;
      }
    }

  return s_ok;
  }
