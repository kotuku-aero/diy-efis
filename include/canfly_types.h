#ifndef __canfly_types_h__
#define __canfly_types_h__

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _MSC_VER
#define PACKED
#pragma pack(push, 1)
#endif
#ifdef __GNUC__
#define PACKED __attribute__((__packed__))
#define ALIGNED __attribute__((aligned))
#endif

/**
 * @brief Generic error value
 */
typedef int result_t;
/**
 * @brief handle to a configuration memory key
 */
typedef uint16_t memid_t;
/**
 * @brief Handle to an object
 */
typedef void *handle_t;

typedef uint8_t byte_t;

#ifndef __cplusplus
#define nullptr ((void *)0)
#endif

enum
{
  s_orphaned = 3,
  s_dropped = 2,
  s_relocated = 1,
  s_ok = 0,
  s_false = -1,
  e_unexpected = -2,
  e_invalid_handle = -3,
  e_not_implemented = -4,
  e_bad_pointer = -5,
  e_bad_parameter = -6,
  e_more_data = -7,
  e_no_more_information = -8,
  e_path_not_found = -9,
  e_operation_pending = -10,
  e_operation_cancelled = -11,
  e_invalid_operation = -12,
  e_buffer_too_small = -13,
  e_generic_error = -14,
  e_timeout = -15,
  e_no_space = -16,
  e_not_enough_memory = -17,
  e_not_found = -18,
  e_bad_handle = -19,
  e_bad_ioctl = -20,
  e_ioctl_buffer_too_small = -21,
  e_not_initialized = -22,
  e_exists = -23,
  e_wrong_type = -24,
  e_parse_error = -25,
  e_incomplete_command = -26,
  e_bad_type = -27,
  e_not_supported = -28,
  e_out_of_range = -29,
  e_object_released = -30,
  e_unsupported_instruction = -31,
  e_overflow = -32,
  e_stream_closed = -33,
  e_corrupt = -34,
  e_not_directory = -35,
  e_not_file = -36,
  e_not_empty = -37,
  e_file_too_large = -38,
  e_no_attributes = -39,
  e_bad_block = -40,
  e_ecc = -41,
  e_too_bad = -42,
  e_recover = -43,
  e_end_of_stream = -44,
  e_directory_not_empty = -45,
};

static inline bool failed(result_t r)
{
  return r < 0;
}

static inline bool succeeded(result_t r)
{
  return r >= 0;
}

#define numelements(a) (sizeof(a) / sizeof(a[0]))

#define NO_WAIT 0
#define INDEFINITE_WAIT 0xFFFFFFFF

///////////////////////////////////////////////////////////////////////
//
// Other kernel types
/**
 * @brief structure type for a time.
 */
typedef struct PACKED _tm_t
{
  uint16_t year;
  uint16_t month;
  uint16_t day;
  uint16_t hour;
  uint16_t minute;
  uint16_t second;
} tm_t;

// these are the hdr.data_type values for a field
typedef enum
{
  field_none, // no definition
  field_key, // a key is a parent of other fields
  field_bool, // a boolean field (see notes)
  field_int8,
  field_uint8,
  field_int16,
  field_uint16,
  field_int32,
  field_uint32,
  field_float,
  field_xyz,
  field_matrix,
  field_string,
  field_qtn,
  field_lla,
  field_stream, // a stream field.  needs stream functions
  field_utc,    // a UTC date/time
    } field_datatype;

#define CFG_STRING_MAX (36)
#define CFG_NAME_MAX (16)
/**
   * @struct lla_t
   * Lat/Lng altitude parameter type
   * @param lat Lattitude
   * @param lng Longitude
   * @param alt Altitude
   */
typedef struct _lla_t
{
  float lat;
  float lng;
  float alt;
} lla_t;

/**
 * @struct xyz_t
 * Generic Euclidean point
 * @param x X
 * @param y Y
 * @param z Z
 */
typedef struct _xyz_t
{
  float x;
  float y;
  float z;
} xyz_t;

/** @struct qtn_t
 * Quarternion type
 */
typedef struct _qtn_t
{
  float x;
  float y;
  float z;
  float w;
} qtn_t;

/** @struct matrix_t
 * Generic matrix parameter
 * @param v Values
 */
typedef struct _matrix_t
  {
  float xx;
  float xy;
  float xz;
  float yx;
  float yy;
  float yz;
  float zx;
  float zy;
  float zz;
  } matrix_t;


////////////////////////////////////////////////////////////////////////
//
// Types supporting the window and GDI subsystems

typedef uint32_t color_t;
 /**
   * @brief a coordinate part in the GDI, allows for +/- 32767 pixels
   */
  typedef int32_t gdi_dim_t;
/**
 * @struct point_t
 * A point on the GDI drawing space
 * @param x  x-coordinate
 * @param y  y-coordinate
 */
typedef struct _point_t
{
  gdi_dim_t x;
  gdi_dim_t y;
} point_t;
/**
 * @struct extent_t
 * Distance between two GDI points
 * @param dx  x Distance
 * @param dy  y Distance
 */
typedef struct _extent_t
{
  gdi_dim_t dx;
  gdi_dim_t dy;
} extent_t;

/**
 * @struct rect_t
 * A rectangle on the GDI
 * @param left  Left extent
 * @param top   Top extent
 * @param right Right extent
 * @param bottom Bottom extent
 * @remark As a general rule all coordinates in the GDI
 * are inverted in a real sense.  This is because most
 * video buffers consider 0,0 to be top left and (screen_x),(screen_y)
 * to be bottom right.
 * Also the gdi fills lines from left->(right-1).  This
 * is because the screen coordinates are 0-(n-1)
 */
typedef struct _rect_t
{
  gdi_dim_t left;
  gdi_dim_t top;
  gdi_dim_t right;
  gdi_dim_t bottom;
} rect_t;

  /**
   * @brief return the top-left of a rectangle as a point
   * @param rect Rectange to query
   * @param pt point to update
   * @return pt
   */
  static inline const point_t *rect_top_left(const rect_t *rect, point_t *pt)
    {
    pt->x = rect->left;
    pt->y = rect->top;

    return pt;
    }
  /**
   * @brief return the top-right of a rectangle as a point
   * @param rect Rectange to query
   * @param pt point to update
   * @return pt
   */
  static inline const point_t *rect_top_right(const rect_t *rect, point_t *pt)
    {
    pt->x = rect->right;
    pt->y = rect->top;

    return pt;
    }
  /**
   * @brief return the bottom-right of a rectangle as a point
   * @param rect Rectange to query
   * @param pt point to update
   * @return pt
   */
  static inline const point_t *rect_bottom_right(const rect_t *rect,
                                                 point_t *pt)
    {
    pt->x = rect->right;
    pt->y = rect->bottom;

    return pt;
    }
  /**
   * @brief return the bottom-left of a rectangle as a point
   * @param rect Rectange to query
   * @param pt point to update
   * @return pt
   */
  static inline const point_t *rect_bottom_left(const rect_t *rect, point_t *pt)
    {
    pt->x = rect->left;
    pt->y = rect->bottom;

    return pt;
    }
  /**
   * @brief Return the width of a rectangle
   * @param rect Rectngle to query
   * @return right - left
   */
  static inline gdi_dim_t rect_width(const rect_t *rect)
    {
    return rect->right - rect->left;
    }
  /**
   * @brief eturn the height of a rectange
   * @param rect Rectangle to query
   * @return bottom - top
   */
  static inline gdi_dim_t rect_height(const rect_t *rect)
    {
    return rect->bottom - rect->top;
    }
  /**
   * @brief Return the center of a rectangle
   * @param rect Rectngle to query
   * @param point to update
   * @return point
   */
  static inline const point_t *rect_center(const rect_t *rect, point_t *pt)
    {
    pt->x = rect->left + (rect_width(rect) >> 1);
    pt->y = rect->top + (rect_height(rect) >> 1);

    return pt;
    }
  /**
   * @brief Return the extents of a rectangle
   * @param rect Rectangle to query
   * @param ex Extent of the rectangle
   * @return ex
   */
  static inline const extent_t *rect_extents(const rect_t *rect, extent_t *ex)
    {
    ex->dx = rect->right - rect->left;
    ex->dy = rect->bottom - rect->top;

    return ex;
    }
  /**
   * @brief Return true if the point is withing the rectangle
   * @param rect rectangle to query
   * @param pt point to query
   * @return true if pt is within the bounds of the rectange
   */
  static inline bool rect_contains(const rect_t *rect, const point_t *pt)
    {
    return pt->x >= rect->left && pt->x < rect->right && pt->y >= rect->top &&
           pt->y < rect->bottom;
    }
  /**
   * @brief Add an extent to a rectange
   * @param rect Rectangle to base on
   * @param ex Distance to move
   * @param value Updated rectange
   * @return
   */
  static inline const rect_t *rect_offset(const rect_t *rect,
                                          const extent_t *ex, rect_t *value)
    {
    gdi_dim_t left = rect->left + ex->dx;
    gdi_dim_t top = rect->top + ex->dy;
    gdi_dim_t right = rect->right + ex->dx;
    gdi_dim_t bottom = rect->bottom + ex->dy;

    value->left = left;
    value->top = top;
    value->right = right;
    value->bottom = bottom;

    return value;
    }
  /**
   * @brief Return a rectange that is the intersection of two rectangles
   * @param r1 Rectangle 1
   * @param r2 Rectangle 2
   * @param value Result
   * @return value
   * @remarks If the rectanges do not overlap the value is
   * set to 0, 0, 0, 0
   */
  static inline const rect_t *rect_intersect(const rect_t *r1, const rect_t *r2,
                                             rect_t *value)
    {
    value->left = r1->left > r2->left ? r1->left : r2->left;
    value->top = r1->top > r2->top ? r1->top : r2->top;
    value->right = r1->right < r2->right ? r1->right : r2->right;
    value->bottom = r1->bottom < r2->bottom ? r1->bottom : r2->bottom;

    if (value->left >= value->right || value->top >= value->bottom)
      value->left = value->right = value->top = value->bottom = 0;

    return value;
    }
  /**
   * @brief Copy a rectangle
   * @param r1 Rectangle from
   * @param r2 Rectangle to
   * @return r2
   */
  static inline const rect_t *rect_copy(const rect_t *r1, rect_t *r2)
    {
    r2->left = r1->left;
    r2->top = r1->top;
    r2->right = r1->right;
    r2->bottom = r1->bottom;

    return r2;
    }

  /**
   * @brief Create a rectangle
   * @param l   left coordinate
   * @param t   top coordinate
   * @param r   right coordinate
   * @param b   bottom coordinate
   * @param value rectangle to set
   * @return regerence to the rectangle
   */
  static inline const rect_t *rect_create(gdi_dim_t l, gdi_dim_t t, gdi_dim_t r,
                                          gdi_dim_t b, rect_t *value)
    {
    value->left = l;
    value->top = t;
    value->right = r;
    value->bottom = b;

    return value;
    }
  /**
   * @brief Create a rectangle given point and extent
   * @param l     left coordinate
   * @param t     top coordinate
   * @param w     width of rectangle
   * @param h     height of rectange
   * @param value rectang to set
   * @return pointer to the rectangle
   */
  static inline const rect_t *rect_create_ex(gdi_dim_t l, gdi_dim_t t,
                                             gdi_dim_t w, gdi_dim_t h,
                                             rect_t *value)
    {
    value->left = l;
    value->top = t;
    value->right = l + w;
    value->bottom = t + h;

    return value;
    }
  /**
   * @brief Create a rectangle using a point
   * @param pt
   * @param ex
   * @param value
   * @return
   */
  static inline const rect_t *rect_create_pt(const point_t *pt,
                                             const extent_t *ex, rect_t *value)
    {
    value->left = pt->x;
    value->top = pt->y;
    value->right = pt->x + ex->dx;
    value->bottom = pt->y + ex->dy;

    return value;
    }

  static inline const point_t *point_copy(const point_t *p1, point_t *p2)
    {
    p2->x = p1->x;
    p2->y = p1->y;

    return p2;
    }

  static inline bool point_equals(const point_t *p1, const point_t *p2)
    {
    return p1->x == p2->x && p1->y == p2->y;
    }

  static inline void point_swap(point_t *left, point_t *right)
    {
    point_t pt;
    point_copy(left, &pt);
    point_copy(right, left);
    point_copy(&pt, right);
    }

static inline const point_t *point_create(gdi_dim_t x, gdi_dim_t y,
                                          point_t *pt)
  {
    pt->x = x;
    pt->y = y;

    return pt;
  }


////////////////////////////////////////////////////////////////////////////////////
//
//  File system support.
//

// Maximum name size in bytes, may be redefined to reduce the size of the
// info struct.
#ifndef FILE_NAME_MAX
#define FILE_NAME_MAX 16
#endif

// Maximum size of a file in bytes.
#ifndef FILE_LENGTH_MAX
#define FILE_LENGTH_MAX 2147483647
#endif

typedef struct _stat_t
{
  // Set when the file is a directory
  uint8_t type;

  // Size of the file, only valid for files.
  uint32_t size;

  // Name of the file stored as a null-terminated string. Limited to
  // FILE_NAME_MAX+1, which can be changed by redefining FILE_NAME_MAX to
  // reduce RAM. FILE_NAME_MAX is stored in superblock and must be
  // respected by other drivers.
  char name[FILE_NAME_MAX + 1];

} stat_t;

#define FILE_TYPE_DIRECTORY 0x02
#define FILE_TYPE_STREAM 0x01

// open flags
// Open a file as read only
#define STREAM_O_RD 0x0001
// Open a file as write only
#define STREAM_O_WR 0x0002
// read-write stream
#define STREAM_O_RDWR 0x0003
// Create a file if it does not exist
#define STREAM_O_CREAT 0x0100
// Fail if a file already exists
#define STREAM_O_EXCL 0x0200
// Truncate the existing file to zero size
#define STREAM_O_TRUNC 0x0400
// Move to end of file on every write
#define STREAM_O_APPEND 0x0800

// Seek relative to an absolute position
#define STREAM_SEEK_SET 0
// Seek relative to the current file position
#define STREAM_SEEK_CUR 1
// Seek relative to the end of the file
#define STREAM_SEEK_END 2

////////////////////////////////////////////////////////////////////
//
//  Can Message support.

typedef struct PACKED {
  // Bits 15:12 - Length
  // Bit 11 - binary data (used by a pipe)
  // bits 10-0 ID
  uint16_t flags; // this makes the same header as an FD msg
  uint8_t data[8];
} canmsg_t;

// Un-used
#define unused_id 0

// these are the messages defined by the kernel and
// the canfly devices
#define CANFLYID(id, num, type, descr) id = num,

enum canfly_id
{
#include "CanFlyID.def"
};

#undef CANFLYID

/*
 * CAN message subscription constraints:
 * - Maximum 8 subscription blocks per task (32 bytes)
 * - IDs 1500..2047 are reserved by neutron and cannot be subscribed
 * - Proton window events (1400..1499) are always delivered
 */
#define MAX_SUBSCRIPTION_BLOCKS 8
#define CANFLY_RESERVED_FIRST   1500
#define CANFLY_RESERVED_LAST    2047

/////////////////////////////////////////////////////////////////////////////////
//
// General variant types
/**
 * @brief encapsulated variant
 */
typedef enum
  {
  v_none,
  v_bool,
  v_int8,
  v_uint8,
  v_int16,
  v_uint16,
  v_int32,
  v_uint32,
  v_float,
  v_utc,
  v_lla
    } variant_type;

typedef struct
  {
  variant_type vt;

  union
    {
    bool boolean;
    int8_t int8;
    uint8_t uint8;
    int16_t int16;
    uint16_t uint16;
    int32_t int32;
    uint32_t uint32;
    float flt;
    tm_t utc;
    } value;
  } variant_t;


/////////////////////////////////////////////////////////////////////////////////
//
// Photon GDI support
// window style bits
// The window is a tab stop for the id_next, id_prev messages
#define DS_TABSTOP        0x80000000
// The window is displayed
#define DS_VISIBLE        0x40000000
// // the window has the current focus
// #define DS_FOCUSED        0x20000000
// the window is enabled
#define DS_ENABLED        0x10000000
#define DS_ZORDER_MASK    0x0000FF00
#define DS_ID_MASK        0x000000FF
#define DS_USERSTYLE_MASK 0x0FFF0000

// these are non-routable messages that are generated by the O/S
/**
 * @brief message sent when a touch screen is provided
*/
typedef struct _touch_msg_t
  {
  uint16_t flags; // message flags
  point_t location; // screen relative position
  uint16_t arg; // depends on message
  } touch_msg_t;

/* touch message flags */
#define TOUCH_BEGIN   0x0001
#define TOUCH_INERTIA 0x0002
#define TOUCH_END     0x0004

/* specific window messages
*
* Note these are always local messages so byte[0] of the
* raw message is never used.
*/
  // these are the events generated by the proton window library that
  // can be hand;ed by the window wndproc.

#define id_first_internal_msg   1400

#define BUTTON_MASK       0xFFFF0000
#define BUTTON_COUNT_MASK  0x0000FFFF

#define id_key0         (1 << 16)
#define id_hold_key0    (2 << 16)
#define id_key1         (3 << 16)
#define id_hold_key1    (4 << 16)
#define id_key2         (5 << 16)
#define id_hold_key2    (6 << 16)
#define id_key3         (7 << 16)
#define id_hold_key3    (8 << 16)
#define id_key4         (9 << 16)
#define id_hold_key4    (10 << 16)
#define id_key5         (11 << 16)
#define id_hold_key5    (12 << 16)
#define id_key6         (13 << 16)
#define id_hold_key6    (14 << 16)
#define id_key7         (15 << 16)
#define id_hold_key7    (16 << 16)
#define id_decka        (17 << 16)
#define id_press_decka  (18 << 16)
#define id_deckb        (19 << 16)
#define id_press_deckb  (20 << 16)
#define id_decka_up     (21 << 16)
#define id_decka_dn     (22 << 16)
#define id_press_decka_up (23 << 16)
#define id_press_decka_dn (24 << 16)
#define id_deckb_up       (25 << 16)
#define id_deckb_dn       (26 << 16)
#define id_press_deckb_up (27 << 16)
#define id_press_deckb_dn (28 << 16)

// hardware events from the buttons.  The 32bit value holds
// the button number and action
#define id_button       1400

#define id_paint        1401        // notification to paint a window
#define id_create       1402        // window has been created
#define id_close        1403        // the window is closed
#define id_setfocus     1404        // focus set to a control
#define id_loosefocus   1405        // focus is lost from a control
#define id_timer        1406        // 1 second timer
#define id_overlap      1407        // overlap completion event
  /*
  * Touch screen interfaces.
  *
  */
#define id_touch_tap      1408    // a simple tap
#define id_touch_two_tap  1409    // a two finger tap
  // arg is distance between fingers
#define id_touch_press    1410    // a press, like a tap but longer
#define id_touch_press_and_tap 1411 // one finger pressed, second tap
  // arg is distance between
#define id_touch_zoom     1412    // the zoom gesture
  // location is center of zoom
  // arg is distance
#define id_touch_pan      1413    // the pan gesture
  // location is end of gesture
  // arg is distance
#define id_touch_rotate   1414    // the rotate gesture
  // location is center of rotate
  // if flags & TOUCH_BEGIN the arg is start angle (0-360)
  // othwewise relative rotation in +/- 360

// these messages are sent to the control that has the focus for keyboard input
// on a graphics layout this will be the menu window, on a dialog layout the
// focus changes using the next/prev key
#define id_ok 1415                // the control received the ok message normally id_sw0
#define id_cancel 1416            // the control received the cancel message normally id_sw6
#define id_next 1417              // normally assigned to id_decka_up
#define id_prev 1418              // normally assigned to id_decka_dn
#define id_up 1419                // normally assigned to id_deckb_up
#define id_down 1420              // normally assigned id id_deckb_dn

#define id_last_photon  1421      // last message assigned
#define id_usermsg  1430

#define id_last_internal_msg 1499


// these are used for the canflyID mapping
#define CANFLY_NODATA 0         // No data in the message
#define CANFLY_ERROR 1          // An error code is in the message
#define CANFLY_UINT8 2          // An unsigned integer
#define CANFLY_INT8 3           // An integer
#define CANFLY_UINT16 4         // A 16 bit integer
#define CANFLY_INT16 5          // A signed 16 bit integer
#define CANFLY_UINT32 6         // An unsigned 32 bit integer
#define CANFLY_INT32 7          // A signed 32 bit integer
#define CANFLY_BOOL_TRUE 8      // A true flag
#define CANFLY_BOOL_FALSE 9     // a false flag
#define CANFLY_FLOAT 10         // A floating point number
#define CANFLY_TM 11           // an encoded UTC time, seconds since 2000-01-01
#define CANFLY_CHARS 12         // Up to 7 characters

#define CANFLY_BINARY 0xFF      // the id is a binary type.

#define CANFLY_BOOL 0x100       // only used to send a flag, never sent

#define LENGTH_MASK 0xF000
#define LENGTH_MASK_FD 0x0007F000
#define ID_MASK 0x07FF
#define BINARY_MASK 0x0800

/**
 * @brief Set the length of a CANbus message
 * @param msg Message
 * @param len Length to assign
 * @remark This method should only be called for internal
 * messages (canID >= 1400 && canID < 1520) as the
 * published ID's all have very specific uses.
 */
static inline void set_can_len(canmsg_t *msg, uint16_t len)
{
  msg->flags &= ~LENGTH_MASK;
  msg->flags |= (len << 12) & LENGTH_MASK;
}

/**
 * @brief Set the ID of a CANbus message
 * @param msg Message
 * @param id ID to assign
 * @remark This method should only be called for internal
 * messages (canID >= 1400 && canID < 1520) as the
 * published ID's all have very specific uses.
 * Use the create_can_msg_<datatype> functions to
 * create defined can messages
 */
static inline void set_can_id(canmsg_t *msg, uint16_t id)
{
  msg->flags &= ~ID_MASK;
  msg->flags |= (id & ID_MASK);
}

/**
 * @brief Get the length of a CANbus message
 * @param msg message
 * @return length
 * @remark The length of data types (can_id < 1520) is one more
 * than the length of the actual type.  The canfly protocol puts
 * the type of the data as the first byte of the message, followed
 * by a big endian encoded data field.
 */
static inline uint8_t get_can_len(const canmsg_t *msg)
{
  return (uint8_t)((msg->flags & LENGTH_MASK) >> 12);
}

/**
 * @brief Return the CANbus ID of the message
 * @param msg Message
 * @return ID of the message
 */
static inline uint16_t get_can_id(const canmsg_t *msg)
{
  return msg->flags & ID_MASK;
}

static inline bool is_can_msg(const canmsg_t *msg)
{
  return get_can_id(msg) < 1792;
}

/**
 * @brief Return true if the can message is a binary format
 * @param msg Message
 * @return true if the message is a binary message
 * @remark This is returned true for all can ID's greater
 * that 1520 which is used by the pipe messages.
 */
static inline bool get_can_is_binary(const canmsg_t *msg)
{
  return msg->flags & BINARY_MASK || get_can_id(msg) >= 1536;
}

/**
  * @brief Set a can message as binary
  * @param msg Message
  * @param is_it true if a binary message
  */
static inline void set_can_is_binary(canmsg_t *msg, bool is_it)
{
  msg->flags &= ~BINARY_MASK;
  if (is_it)
    msg->flags |= BINARY_MASK;
}

/**
  * @brief Return the encoded type of a message
  * @param msg Message
  * @return One of the CANFLY_ data types
  */
static inline uint16_t get_can_type(const canmsg_t *msg)
{
  if ((msg->flags & BINARY_MASK) != 0)
    return CANFLY_BINARY;
  // non binary messages store the data type as the
  // first byte of the message
  switch (msg->data[0])
  {
  case CANFLY_BOOL_FALSE:
  case CANFLY_BOOL_TRUE:
    return CANFLY_BOOL;
  default:
    return msg->data[0];
  }
}

// enumeration for a status field
typedef enum _e_board_status
{
  bs_unknown = 0,
  bs_starting = 1,
  bs_running = 2,
  bs_inhibited = 3,
  bs_failed = 4,
  bs_wdt_reset = 0x0008,
  bs_bor_reset = 0x0010,
  bs_dmt_reset = 0x0020,
  bs_bootrequest = 0x0080, // is
} e_board_status;

// A Status message is sent from each node.  Only one status message ID is used
// and the following bitfields are defined

typedef struct PACKED _status_msg_t
{
  uint8_t msg_type; // set to CANFLY_BINARY
  uint8_t node_id; // is well known, or if kMAG can be the same id initially
  uint8_t board_status; // see enum above
  uint8_t board_type; // type of board
  uint32_t serial_number; // board serial
} status_msg_t;

// this is the message used to set a node's id

typedef struct _set_node_id_msg_t
{
  uint8_t msg_type; // set to CANFLY_BINARY
  uint32_t serial_number; // device to set
  uint8_t existing_id; // must match
  uint8_t new_id; // and set to
  uint8_t new_subtype; // allows setting the subtype as well
} set_node_id_msg_t;

/////////////////////////////////////////////////////////////////////////
//
// Overlapped I/O support.
//
// This section provides support for overlapped I/O operations.  Where an operation
// may block, this structure can be passed to the API and when the operation completes
// the overlapped I/O context is used to identify the completed operation.  The
// address is passed as a 32bit number using the id_overlapped can message and will
// be posted to the task's mailbox when the operation completes.
//

typedef struct _overlapped_t overlapped_t;

typedef void (*overlapped_callback_fn)(void *context, result_t result);

typedef struct _overlapped_t
{
  overlapped_callback_fn cb; // this will be called when the operation completes
  result_t result;        // this is the result of the operation
  uint16_t can_id;        // id of the message to post when completed
  uint16_t reserved; // reserved for use by the kernel
  uint32_t reserved2; // this is the id of the overlapped operation
} overlapped_t;


typedef result_t (*wndproc_fn)(handle_t hwnd, const canmsg_t *msg,
                               void *wnddata);
#ifdef RGB
#undef RGB
#endif

#define RGBA(r, g, b, a)                                                       \
  ((color_t)(((((color_t)(r)) & 0xff) << 24) | (((color_t)(g)) << 16) |        \
             ((((color_t)(b)) & 0xff) << 8) | (((color_t)(a)) & 0xff)))
#define RGB(r, g, b) RGBA((r), (g), (b), 0xff)

#define rgb(red, green, blue) RGBA(red, green, blue, 255)
#define rgba(alpha, red, green, blue) RGBA(red, green, blue, alpha)

#define rgb_alpha(c, a) ((c & 0xffffff00) | (a & 0xff))

#define red(c) ((uint8_t)(((c) >> 24) & 255))
#define green(c) ((uint8_t)(((c) >> 16) & 255))
#define blue(c) ((uint8_t)(((c) >> 8) & 255))
#define alpha(c) ((uint8_t)(((c) >> 0) & 255))

#define color_white RGB(255, 255, 255)
#define color_black RGB(0, 0, 0)
#define color_grey RGB(128, 128, 128)
#define color_red RGB(255, 0, 0)
#define color_orange_red RGB(255, 69, 0)
#define color_scarlet RGB(255, 36, 0)
#define color_pink RGB(255, 128, 128)
#define color_blue RGB(0, 0, 255)
#define color_darkblue RGB(64, 64, 255)
#define color_green RGB(0, 255, 0)
#define color_lightgreen RGB(0, 192, 0)
#define color_yellow RGB(255, 255, 64)
#define color_magenta RGB(255, 0, 255)
#define color_cyan RGB(0, 255, 255)
#define color_teal RGB(64, 128, 128)
#define color_maroon RGB(128, 0, 0)
#define color_purple RGB(128, 0, 128)
#define color_orange RGB(255, 192, 64)
#define color_khaki RGB(167, 151, 107)
#define color_olive RGB(128, 128, 0)
#define color_brown RGB(192, 128, 32)
#define color_darkbrown RGB(96, 64, 16)
#define color_navy RGB(0, 64, 128)
#define color_lightgrey RGB(192, 192, 192)
#define color_darkgrey RGB(64, 64, 64)
#define color_paleyellow RGB(255, 255, 208)
#define color_lightyellow RGB(255, 255, 128)
#define color_limegreen RGB(192, 220, 192)
#define color_darkgreen RGB(0, 128, 0)
#define color_lightblue RGB(128, 128, 255)
#define color_fadedblue RGB(192, 192, 255)
#define color_hollow RGBA(0, 0, 0, 0)

typedef enum _text_flags
  {
    eto_none = 0x000,
    eto_opaque = 0x001,
    eto_clipped = 0x002,
    eto_vertical = 0x004,
    eto_left = 0x000,
    eto_right = 0x008,
    eto_center = 0x010,
    eto_top = 0x000,
    eto_bottom = 0x020,
    eto_middle = 0x040,
    eto_wrap = 0x080,
  } text_flags;

#define ALIGN_FLAGS_MASK 0x30

// this is provided by the application and is a bitmap font that the
// GDI can render
typedef struct PACKED _font_dimensions_t {
  uint16_t width;
  uint16_t offset;
}font_dimensions_t;

typedef struct PACKED _font_t
{
  // this is defined with rtti so it can be safely cast to a type
  // note that static fonts don't have a destructor
  const char* name;         // name of the font
  gdi_dim_t font_size;      // size of the font in pixels
  gdi_dim_t height;         // height of the bitmap
  char first_char;          // first character in this font
  char last_char;           // last character in this font
  const uint8_t* bitmap_pointer; // pointer to the bitmap for all characters
  // this is written as columns so if the char width > 8 pixels
  // the _next column is + bitmap_height
  const font_dimensions_t* char_table; // pointer to a set of offsets for each character
} font_t;

/**
 * @struct bitmap_t
 */
typedef struct PACKED _bitmap_t
{
  gdi_dim_t bitmap_width;  // width of the bitmap_t in pixels
  gdi_dim_t bitmap_height; // height of the bitmap_t in pixels
  const color_t *pixels;   // array of pixels in color format (rgba)
} bitmap_t;

typedef enum
{
  src_copy = 0,
  src_alpha_blend = 1
} raster_operation;

////////////////////////////////////////////////////////////////////////////////////
//
// These support the moving map parameters
typedef struct {
  int32_t elevation;
  color_t color;
  color_t contour_color;
} hypsometric_theme_t;

typedef struct {
  color_t alarm_color;
  color_t warning_color;
  color_t water_color;
  color_t land_color;
  color_t coastline_color;
  // TODO: these should be pens
  color_t residential_street;
  color_t residential_area;
  color_t big_road;
  color_t major_road;
  color_t highway;
  color_t class_a_airspace;
  color_t class_b_airspace;
  color_t class_c_airspace;
  color_t class_d_airspace;
  color_t class_e_airspace;
  color_t class_f_airspace;
  color_t class_g_airspace;
  color_t class_m_airspace;
  color_t cfz_airspace;
  color_t mbz_airspace;
  color_t danger_area;
  color_t restricted_area;

  color_t aerodrome;
  color_t aerodrome_border;

  color_t runway_grass_pen;
  color_t runway_grass_color;
  color_t runway_grass_ext_color;

  color_t runway_pen;
  color_t runway_color;
  color_t runway_ext_color;

  hypsometric_theme_t theme[20];    // up to 10,000 ft
} map_theme_t;

typedef enum {
  mdm_north,          // north up
  mdm_track,          // track up
  mdm_course,         // course up
  mdm_heading,        // heading up
  } map_display_mode;

// these control the types of rendering the map background will show
#define MAP_LAYER_TERRAIN         0x00000001
#define MAP_LAYER_CONTOURS        0x00000002
#define MAP_LAYER_LANDAREA        0x00000004
#define MAP_LAYER_COASTLINES      0x00000008
#define MAP_LAYER_CITIES          0x00000010
#define MAP_LAYER_SURFACE_WATER   0x00000020
#define MAP_LAYER_TRANSPORT       0x00000040
#define MAP_LAYER_OBSTACLES       0x00000080
#define MAP_LAYER_AIRPORTS        0x00000100
#define MAP_LAYER_AIRSPACES       0x00000200
#define MAP_LAYER_TRAFFIC         0x00000400
#define MAP_LAYER_FLIGHTPLAN      0x00000800

///////////////////////////////////////////////////////////////////////////////////////
//
// kHUB Data types support

typedef enum _wpt_type {
  WPT_TYPE_UNSPECIFIED = 0,
  WPT_TYPE_WAYPOINT = 1, // A standard waypoint
  WPT_TYPE_VOR = 2,      // A VOR station
  WPT_TYPE_NDB = 3,      // A NDB station
  WPT_TYPE_INTERSECTION = 4, // An intersection point
  WPT_TYPE_AIRPORT = 5,  // An airport location
  WPT_TYPE_REPORTING_POINT = 6, // A reporting point
  WPT_TYPE_FIX = 7, // A fix point
  } wpt_type;

// default conversions for an aircraft
typedef enum _display_units {
  display_si = 0,
  display_imperial = 1,
} display_units;

typedef struct _aircraft_t {
  uint32_t hex_code;    // this is the adsb hex code
  char registration[CFG_STRING_MAX+1];
  float vs0;        // m/s stall speed landing at mtow
  float vs1;        // m/s stall speed clean at mtow
  float vfe;        // m/s max flap extension speed
  float vno;        // m/s normal operations speed
  float vne;        // m/s vne at amsl
  float va;         // m/s aircraft maouvering speed
  float vg;         // m/s best glide airspeec
  float vx;         // m/s best angle of climb
  float vy;         // m/s best rate of climb
  float vz;         // m/s best rate of descent
  bool auto_flap_retract;
  float vfe_max;    // point at which the flaps will be retracted if enabled
  float vtrim;      // speed at which trim speed is divided by 2
  float climb_amsl_rate;  // m/s sea level climb m/s at Vy
  float climb_ceiling_rate; // service ceiling climb m/s
  uint32_t critical_aoa;  // max angle of attack.
  uint32_t climb_aoa;     // max climb aoa, usually 90% of critical (top of yellow band)
  uint32_t approach_aoa;  // usually 75% of critical_aoa (bottom of yellow band)
  uint32_t cruise_aoa;  // bottom of green band.  Aoa indicator turns off at this point
  uint32_t yaw_max;   // maximum yaw angle

  // this is used by the edu display
  uint16_t num_cylinders;
  display_units units;
} aircraft_t;

#ifndef __cplusplus
// C++ must use std::min and std::max
#ifdef min
#undef min
#endif
#define min(a, b) ((a) < (b) ? (a) : (b))

#ifdef max
#undef max
#endif
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef _SPATIAL_TYPES
#define _SPATIAL_TYPES
#define pi 3.1415926535897932384626433832795f

static inline float degrees_to_radians(float degrees)
  {
  return degrees * (pi / 180);
  }

static inline float radians_to_degrees(float radians)
  {
  float retval = radians / (pi / 180);
  return retval;
  }

#define rad_deg (pi / 180)
#define rad_45 (pi / 4)
#define rad_90 (pi / 2)
#define rad_180 pi;
#define rad_270 (pi * 1.5)
#define rad_360 (pi * 2)

#define FEET_PER_METER 3.2808398950131233595800524934383f

static inline float meters_to_feet(float meters)
  {
  return meters * FEET_PER_METER;
  }

static inline float feet_to_meters(float feet)
  {
  return feet / FEET_PER_METER;
  }

static inline float feet_per_minute_to_meters_per_second(float n)
  {
  return (n / (60 * FEET_PER_METER));
  }

static inline float meters_per_second_to_feet_per_minute(float n)
  {
  return n * 60 * FEET_PER_METER;
  }

//#define METERS_PER_NM (6076.11549 / FEET_PER_METER)
#define METERS_PER_NM (1852.000001352f)

static inline float nm_to_meters(float nm)
  {
  return nm * METERS_PER_NM;
  }

static inline float meters_to_nm(float meters)
  {
  return meters / METERS_PER_NM;
  }

static inline float knots_to_meters_per_second(float knots)
  {
  return nm_to_meters(knots) / 3600;
  }

static inline float meters_per_second_to_knots(float mps)
  {
  return meters_to_nm(mps * 3600);
  }

static inline float meters_to_degrees(float meters)
  {
  return meters_to_nm(meters) / 60;
  }

static inline float meters_to_degrees_ex(float meters, float declination, bool declination_is_radians)
  {
  if (declination == 0.0)
    return meters_to_nm(meters) / 60;

  return (meters_to_nm(meters) / 60)/ cosf(declination_is_radians ? declination : degrees_to_radians(declination));
  }

static inline float degrees_to_meters(float d)
  {
  return nm_to_meters(d) * 60;
  }

static inline float degrees_to_meters_ex(float d, float declination, bool declination_is_radians)
  {
  if (declination == 0.0)
    return nm_to_meters(d) * 60;

  return (nm_to_meters(d) * 60) * cosf(declination_is_radians ? declination : degrees_to_radians(declination));
  }

static inline float meters_to_radians(float meters)
  {
  return degrees_to_radians(meters_to_degrees(meters));
  }

static inline float meters_to_radians_ex(float meters, float declination,
                                bool decl_is_radians)
  {
  return degrees_to_radians(meters_to_degrees_ex(meters, declination, decl_is_radians));
  }

static inline float radians_to_meters(float d)
  {
  return degrees_to_meters(radians_to_degrees(d));
  }

static inline float radians_to_meters_ex(float d, float declination,
                                bool decl_is_radians)
  {
  return degrees_to_meters_ex(radians_to_degrees(d), declination,
                           decl_is_radians);
  }
#endif

#ifdef _MSC_VER
#define PACKED
#pragma pack(pop)
#endif


#ifdef __cplusplus
}
#endif

#endif