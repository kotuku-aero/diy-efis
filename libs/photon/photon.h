#ifndef __photon_h__
#define __photon_h__

#include "../neutron/neutron.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file photon.h
 * Photon GDI functions
 */

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
  typedef struct _point_t {
    gdi_dim_t x;
    gdi_dim_t y;
    } point_t;
  /**
   * @struct extent_t
   * Distance between two GDI points
   * @param dx  x Distance
   * @param dy  y Distance
  */
  typedef struct _extent_t {
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
  typedef struct _rect_t {
    gdi_dim_t left;
    gdi_dim_t top;
    gdi_dim_t right;
    gdi_dim_t bottom;
    } rect_t;

  typedef uint32_t color_t;
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
  static inline const point_t *rect_bottom_right(const rect_t *rect, point_t *pt)
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
    return  rect->right - rect->left;
    }
  /**
   * @brief eturn the height of a rectange
   * @param rect Rectangle to query
   * @return bottom - top
  */
  static inline gdi_dim_t rect_height(const rect_t *rect)
    {
    return  rect->bottom - rect->top;
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
    return 
      pt->x >= rect->left &&
      pt->x < rect->right &&
      pt->y >= rect->top &&
      pt->y < rect->bottom
      ;
    }
  /**
   * @brief Add an extent to a rectange
   * @param rect Rectangle to base on
   * @param ex Distance to move
   * @param value Updated rectange
   * @return 
  */
  static inline const rect_t *rect_offset(const rect_t *rect, const extent_t *ex, rect_t *value)
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
  static inline const rect_t *rect_intersect(const rect_t *r1, const rect_t *r2, rect_t *value)
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
  static inline const rect_t *rect_create(gdi_dim_t l, gdi_dim_t t, gdi_dim_t r, gdi_dim_t b, rect_t *value)
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
  static inline const rect_t* rect_create_ex(gdi_dim_t l, gdi_dim_t t, gdi_dim_t w, gdi_dim_t h, rect_t* value)
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
  static inline const rect_t *rect_create_pt(const point_t *pt, const extent_t *ex, rect_t *value)
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

  extern result_t rotate_point(const point_t *center, int16_t angle, point_t *pt);

  static inline const point_t *point_create(gdi_dim_t x, gdi_dim_t y, point_t *pt)
    {
    pt->x = x;
    pt->y = y;

    return pt;
    }

#ifdef RGB
#undef RGB
#endif

#define RGBA(r,g,b,a) ( (color_t)( ((((color_t)(r)) & 0xff)<<24) | (((color_t)(g))<<16)  | ((((color_t)(b)) & 0xff)<<8) | (((color_t)(a)) & 0xff) ) )
#define RGB(r,g,b) RGBA((r),(g),(b),0xff)

#define rgb(red, green, blue) RGBA(red,green,blue,255)
#define rgba(alpha, red, green, blue) RGBA(red,green,blue,alpha)

#define rgb_alpha(c, a) ((c & 0xffffff00) | (a & 0xff))

#define red(c) ((uint8_t)((( c ) >> 24) & 255))
#define green(c) ((uint8_t)((( c ) >> 16) & 255))
#define blue(c) ((uint8_t)((( c ) >> 8) & 255))
#define alpha(c) ((uint8_t)((( c ) >> 0) & 255))

#define color_white RGB(255, 255, 255)
#define color_black RGB(0, 0, 0)
#define color_gray RGB(128, 128, 128)
#define color_red RGB(255, 0, 0)
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

  typedef enum _text_flags {
    eto_none = 0x00,
    eto_opaque = 0x02,
    eto_clipped = 0x04,
    eto_vertical = 0x08
    } text_flags;
  /**
 * @struct bitmap_t
 */
  typedef struct PACKED _bitmap_t
    {
    gdi_dim_t bitmap_width; // width of the bitmap_t in pixels
    gdi_dim_t bitmap_height; // height of the bitmap_t in pixels
    const color_t *pixels;  // array of pixels in color format (rgba)
    } bitmap_t;


  typedef enum _pen_style
    {
    ps_solid,
    ps_dash,
    ps_dot,
    ps_dash_dot,
    ps_dash_dot_dot,
    ps_null
    } pen_style;

  typedef struct _pen_t
    {
    color_t color;
    size_t width;
    pen_style style;
    } pen_t;

  typedef struct _font_dimensions_t {
    uint16_t width;
    uint16_t offset;
    }font_dimensions_t;

  typedef struct _font_t
    {
    gdi_dim_t height;               // height of the bitmap
    char first_char;                // first character in this font
    char last_char;                 // last character in this font
    const uint8_t* bitmap_pointer; // pointer to the bitmap for all characters
    // this is written as columns so if the char width > 8 pixels
    // the _next column is + bitmap_height
    const font_dimensions_t* char_table; // pointer to a set of offsets for each character
    } font_t;

  /**
   * Create an off screen canvas
   * @param extent_t    dimensions of the display context
   */
  extern result_t canvas_create(const extent_t *size, handle_t *hndl);
  /**
   * @brief Create a canvas from the dimensions of the bitmap and select the pixels
   * into the canvas
   * @param bitmap    bitmap to create from
   */
  extern result_t canvas_create_bitmap(const bitmap_t *bitmap, handle_t *hndl);
  /**
  * @brief Create a canvas from the PNG image provided in the stream
  * @param stream    stream to read the PNG image from
  * @param canvas      handle to the stream
  * @return s_ok if stream created ok
  */
  extern result_t canvas_create_png(handle_t stream, handle_t *hndl);
  /**
   * @brief return the extents of the canvas
   * @param canvas  handle to canvas, window or screen
   * @param ex    
   * @return 
  */
  extern result_t canvas_extents(handle_t canvas, extent_t *ex);
  /**
   * @brief Draw a polyline
   * @param canvas        handle to a gdi object (canvas, window, screen)
   * @param clip_rect   rectangle to clip to
   * @param pen         pen to draw with
   * @param count       number of points
   * @param points      points to draw
   */
  extern result_t polyline(handle_t canvas, const rect_t *clip_rect, color_t pen, uint32_t count, const point_t *points);
  /**
   * @brief Draw an ellipse
   * @param canvas        gdi object to draw ellipse on
   * @param clip_rect   rectangle to clip to
   * @param pen         pen to draw ellipse with
   * @param color       color to fill ellipse with
   * @param area        area of the ellise
   */
  extern result_t ellipse(handle_t canvas, const rect_t *clip_rect, color_t pen, color_t fill, const rect_t *area);
  /**
   * @brief Draw a polygon and optionally fill it
   * @param canvas        gdi object to draw polygon on
   * @param clip_rect   rectangle to clip to
   * @param pen         pen to draw lines with
   * @param color       color to fill with
   * @param count       number of points
   * @param points      points of the polygon
   */
  extern result_t polygon(handle_t canvas, const rect_t *clip_rect, color_t pen, color_t fill, uint32_t count, const point_t *points);
  /**
   * @brief Draw a series of polygons.  Right-handed polygons are fill, left-handed are voids
   * @param canvas              gdi object to draw ploygons on
   * @param clip_rect           rectangle to clip to
   * @param pen                 Pen to draw lines with
   * @param fill                Fill color for the polygons
   * @param num_polygon         Number of polygons
   * @param polygon_counts      array of lengths for the polygons
   * @param points              points that form the polygons
   * @return s_ok if rendered ok
  */
  extern result_t polypolygon(handle_t canvas, const rect_t* clip_rect, color_t pen, color_t fill, uint32_t num_polygon, const uint32_t *polygon_counts, const point_t* points);
  /**
   * @brief Draw a rectangle
   * @param clip_rect     rectangle to clip to
   * @param pen           pen to outline with, can be 0
   * @param color         color to fill with, can be color_hollow
   * @param area          area of rectangle
   */
  extern result_t rectangle(handle_t canvas, const rect_t *clip_rect, color_t pen, color_t fill, const rect_t *area);
  /**
   * @brief Draw a rectangle with rounded corners
   * @param canvas          handle to a canvas
   * @param clip_rect     rectangle to clip to
   * @param pen           pen to outline with
   * @param color         color to fill with
   * @param area          area of rectangle
   * @param corners       radius of corners
   */
  extern result_t round_rect(handle_t canvas, const rect_t *clip_rect, color_t pen, color_t fill, const rect_t *area, gdi_dim_t radius);

  typedef enum {
    src_copy = 0,
    src_alpha_blend = 1
    } raster_operation;
  /**
   * @brief Copy pixels from one canvas to another
   * @param clip_rect     rectangle to clip to
   * @param dest_rect     rectangle to draw into
   * @param src_canvas    canvas to copy from
   * @param src_clip_rect area of canvas to copy from
   * @param src_pt        top-left point of the source rectangle
   * @param operation     raster operation
   */
  extern result_t bit_blt(handle_t canvas, const rect_t *clip_rect, const rect_t *dest_rect, handle_t src_canvas,
    const rect_t *src_clip_rect, const point_t *src_pt, raster_operation operation);
  /**
   * @brief Return pixel
   * @param canvas      canvas to query
   * @param clip_rect   rectangle to clip to
   * @param pt          point to get
   */
  extern result_t get_pixel(handle_t canvas, const rect_t *clip_rect, const point_t *pt, color_t *pix);
  /**
   * @brief Set a pixel
   * @param canvas      canvas to write to
   * @param clip_rect   rectangle to clip to
   * @param pt          point to wite
   * @param c           color to write
   * @return pixel       optional old pixel
   */
  extern result_t set_pixel(handle_t canvas, const rect_t *clip_rect, const point_t *pt, color_t c, color_t *pix);
  /**
   * @brief Draw an arc
   * @param canvas      canvas to draw on
   * @param clip_rect   rectangle to clip to
   * @param pen         pen to use for line
   * @param pt          center point
   * @param radius      radius of arc
   * @param start       start angle in degrees 0-359
   * @param end         end angle in degress 0-359
   */
  extern result_t arc(handle_t canvas, const rect_t *clip_rect, color_t pen, const point_t *pt, gdi_dim_t radius, int16_t start, int16_t end);
  /**
   * @brief Draw an arc fitted to 3 points
   * @param canvas      canvas to draw on
   * @param clip_rect   rectangle to clip to
   * @param pen         pen to use for line
   * @param center      center point
   * @param start       starting point of arc
   * @param end         endding point of arc
   * @return s_ok if arc is drawn
  */
  extern result_t arc_3pt(handle_t canvas, const rect_t *clip_rect, color_t pen, const point_t *center, const point_t *start, const point_t *end);
  /**
   * @brief Draw a pie
   * @param clip_rect   rectangle to clip to
   * @param pen         pen to use for outline
   * @param color       color to fill with
   * @param pt          center point
   * @param start       start angle in degrees 0-359
   * @param end         end angle in degrees 0-359
   * @param radii       pie radius
   * @param inner       innert radius
   * @return  s_ok if completed
   */
  extern result_t pie(handle_t canvas, const rect_t *clip_rect, color_t pen, color_t fill, 
    const point_t *pt, int16_t start, int16_t end, gdi_dim_t radii, gdi_dim_t inner);
  /**
   * @brief Draw text
   * @param clip_rect   rectangle to clip to
   * @param font        font to write
   * @param fg          foreground color
   * @param bg          background color
   * @param count       number of characters
   * @param str         text to write
   * @param src_pt      top left point of text
   * @param txt_clip_rect area to clip text to
   * @param format      text operation
   * @param char_widths optional array of widths of characters written
   */
  extern result_t draw_text(handle_t canvas, const rect_t *clip_rect, const font_t *font,
    color_t fg, color_t bg, uint16_t count, const char *str,
    const point_t *src_pt, const rect_t *txt_clip_rect, text_flags format, uint16_t *char_widths);
  /**
   * @brief Return the area text draws within
   * @param font        font to use
   * @param str         text to write
   * @param count       number of characters
   * @param extent      resulting extents
   */
  extern result_t text_extent(const font_t * font, uint16_t count, const char *str, extent_t *ex);

#ifdef __cplusplus
  }
#endif

#endif
