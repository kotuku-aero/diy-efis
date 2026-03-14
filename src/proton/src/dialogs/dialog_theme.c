#include <stdlib.h>
#include "../../include/dialog_theme.h"
#define DASH_LENGTH 4
#define DOT_LENGTH 2
#define GAP_LENGTH 1
#define DASH_PATTERN_LENGTH  DASH_LENGTH + GAP_LENGTH
#define DOT_PATTERN_LENGTH DOT_LENGTH + GAP_LENGTH
#define DASHDOT_PATTERN_LENGTH DASH_PATTERN_LENGTH + DOT_PATTERN_LENGTH

const dialog_theme_t *dialog_theme;
/**
 * Called when an application is loaded to set the application theme
 * @param theme Theme to use
 */
result_t set_layout_theme(const dialog_theme_t *theme)
  {
  dialog_theme = theme;
  return s_ok;
  }

typedef enum {
  DIRECTION_RIGHT,
  DIRECTION_DOWN,
  DIRECTION_LEFT,
  DIRECTION_UP
} draw_direction_t;

/**
 * Draw a line segment for a highlight box
 * @param canvas Drawing canvas
 * @param clip_rect Screen clipping area
 * @param color Color to draw with
 * @param start_pos Start of line
 * @param end_pos End of line
 * @param fixed_coord Invariant coordinate for orthoginal lines
 * @param direction Drawing direction
 * @param is_last_segment true if the line should be clipped rather than drawn around the corner
 * @param pattern_offset_inout variable to hold how much line was drawn around the corner on the last line
 */
static void draw_dashed_line_segment(
  handle_t canvas,
  const rect_t* clip_rect,
  color_t color,
  gdi_dim_t start_pos,
  gdi_dim_t end_pos,
  gdi_dim_t fixed_coord,
  draw_direction_t direction,
  bool is_last_segment,
  gdi_dim_t* pattern_offset_inout)
  {
  point_t pts[3];
  gdi_dim_t pattern_offset = *pattern_offset_inout;

  // Set fixed coordinate based on direction
  if (direction == DIRECTION_RIGHT || direction == DIRECTION_LEFT)
    pts[0].y = pts[1].y = fixed_coord;
  else
    pts[0].x = pts[1].x = fixed_coord;

  // decide if the step increments (right & down case) or decrements (left & up case)
  int step = (direction == DIRECTION_RIGHT || direction == DIRECTION_DOWN) ?
             DASH_PATTERN_LENGTH : -DASH_PATTERN_LENGTH;

  for (gdi_dim_t pos = start_pos + pattern_offset;
       (step > 0) ? (pos < end_pos) : (pos > end_pos);
       pos += step)
    {
    // assume drawing a single line not around the corner
    uint32_t num_points = 2;

    // Set starting position
    if (direction == DIRECTION_RIGHT || direction == DIRECTION_LEFT)
      pts[0].x = pos;
    else
      pts[0].y = pos;

    // Calculate end position
    gdi_dim_t next_pos = pos + step;
    bool exceeds = (step > 0) ? (next_pos > end_pos) : (next_pos < end_pos);

    if (exceeds)
      {
      pattern_offset = abs(next_pos - end_pos);
      if (pattern_offset > GAP_LENGTH)
        {
        // if this is the last segment then we don't wrap the corner.  If the length of the line to be drawn is
        // <1 pixels then it is not drawn at all
        if (is_last_segment)
          {
          // Clip to end without wrapping
          if (direction == DIRECTION_RIGHT || direction == DIRECTION_LEFT)
            pts[1].x = end_pos;
          else
            pts[1].y = end_pos;

          polyline(canvas, clip_rect, color, num_points, pts);
          break;
          }

        // Wrap around corner
        num_points = 3;

        // Calculate the corner point and wrap-around point
        if (direction == DIRECTION_RIGHT)
          {
          pts[1].x = end_pos;
          pts[2].x = end_pos;
          pts[2].y = fixed_coord + (pattern_offset - GAP_LENGTH);
          }
        else if (direction == DIRECTION_DOWN)
          {
          pts[1].y = end_pos;
          pts[2].y = end_pos;
          pts[2].x = fixed_coord - (pattern_offset - GAP_LENGTH);
          }
        else if (direction == DIRECTION_LEFT)
          {
          pts[1].x = end_pos;
          pts[2].x = end_pos;
          pts[2].y = fixed_coord - (pattern_offset - GAP_LENGTH);
          }
        else // DIRECTION_UP
          {
          pts[1].y = end_pos;
          pts[2].y = end_pos;
          pts[2].x = fixed_coord + (pattern_offset - GAP_LENGTH);
          }        }
      }
    else
      {
      // Adjust for gap
      next_pos += (step > 0) ? -GAP_LENGTH : GAP_LENGTH;
      pattern_offset = 0;
      }

    // Set end position
    if (direction == DIRECTION_RIGHT || direction == DIRECTION_LEFT)
      pts[1].x = next_pos;
    else
      pts[1].y = next_pos;

    polyline(canvas, clip_rect, color, num_points, pts);
    }

  *pattern_offset_inout = pattern_offset;
  }

result_t draw_control_focus(handle_t canvas, const rect_t* clip_rect, const rect_t* rect)
  {
  result_t result;

  rect_t highlight_rect = *rect;

  highlight_rect.left += 2;
  highlight_rect.right -= 2;
  highlight_rect.top += 2;
  highlight_rect.bottom -= 2;

  gdi_dim_t pattern_offset = 0;

  // draw a focus rectangle with the style
  switch (dialog_theme->focus_highlight_style)
    {
  case focus_highlight_style_none:
    break;
  case focus_highlight_style_dashed:
  default:
    // top (this draws from left to right)
    draw_dashed_line_segment(canvas, clip_rect, dialog_theme->focused_highlight_color,
      highlight_rect.left, highlight_rect.right, highlight_rect.top, DIRECTION_RIGHT,
      false, &pattern_offset);
    // right (this draws from bottom to top)
    draw_dashed_line_segment(canvas, clip_rect, dialog_theme->focused_highlight_color,
      highlight_rect.top, highlight_rect.bottom, highlight_rect.right, DIRECTION_DOWN,
      false, &pattern_offset);
    // bottom  (this draws from right to left)
    draw_dashed_line_segment(canvas, clip_rect, dialog_theme->focused_highlight_color,
      highlight_rect.right, highlight_rect.left, highlight_rect.bottom, DIRECTION_LEFT,
      false, &pattern_offset);
    // left (this draws from top to bottom)
    draw_dashed_line_segment(canvas, clip_rect, dialog_theme->focused_highlight_color,
      highlight_rect.bottom, highlight_rect.top, highlight_rect.left, DIRECTION_UP,
      true, &pattern_offset);
    break;
  case focus_highlight_style_dots:
    break;
  case focus_highlight_style_dashdot:
    break;
  case focus_highlight_style_solid:
    break;
    }

  return s_ok;
  }
