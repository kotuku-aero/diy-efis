#include "photon_priv.h"

typedef int out_code_t;

// 0000
#define INSIDE 0
// 0001
#define LEFT 1
// 0010
#define RIGHT 2
// 0100
#define ABOVE 4
// 1000
#define BELOW 8


out_code_t compute_out_code(const point_t* pt, const rect_t* rect)
  {
  out_code_t code;

  code = INSIDE;          // initialised as being inside of clip window

  if (pt->x < rect->left)             // to the left of clip window
    code |= LEFT;
  else if (pt->x > rect->right)       // to the right of clip window
    code |= RIGHT;
  if (pt->y < rect->top)              // above the clip window
    code |= ABOVE;
  else if (pt->y > rect->bottom)      // below the clip window
    code |= BELOW;

  return code;
  }

// clip a line to within the clipping rectangle.  Return true if part of line is within the rectangle
bool clip_line(point_t* p1, point_t* p2, const rect_t* clip_rect)
  {
  // compute outcodes for P0, P1, and whatever point lies outside the clip rectangle
  out_code_t out_code_0 = compute_out_code(p1, clip_rect);
  out_code_t out_code_1 = compute_out_code(p2, clip_rect);

  while (true)
    {
    if (!(out_code_0 | out_code_1))
      { // Bitwise OR is 0. Trivially accept and get out of loop
      return true;        // line inside rectange
      }
    else if (out_code_0 & out_code_1)
      { // Bitwise AND is not 0. Trivially reject and get out of loop
      return false;       // line outside rectangle
      }
    else
      {
      // failed both tests, so calculate the line segment to clip
      // from an outside point to an intersection with clip edge
      float x, y;

      // At least one endpoint is outside the clip rectangle; pick it.
      out_code_t outcodeOut = out_code_0 ? out_code_0 : out_code_1;

      // Now find the intersection point;
      // use formulas y = y0 + slope * (x - x0), x = x0 + (1 / slope) * (y - y0)
      if (outcodeOut & BELOW)
        {           // point is above the clip rectangle
        x = (float)(p1->x + (p2->x - p1->x) * (clip_rect->bottom - p1->y) / (p2->y - p1->y));
        y = clip_rect->bottom;
        }
      else if (outcodeOut & ABOVE)
        { // point is below the clip rectangle
        x = (float)(p1->x + (p2->x - p1->x) * (clip_rect->top - p1->y) / (p2->y - p1->y));
        y = clip_rect->top;
        }
      else if (outcodeOut & RIGHT)
        {  // point is to the right of clip rectangle
        y = (float)(p1->y + (p2->y - p1->y) * (clip_rect->right - p1->x) / (p2->x - p1->x));
        x = clip_rect->right;
        }
      else if (outcodeOut & LEFT)
        {   // point is to the left of clip rectangle
        y = (float)(p1->y + (p2->y - p1->y) * (clip_rect->left - p1->x) / (p2->x - p1->x));
        x = clip_rect->left;
        }

      // Now we move outside point to intersection point to clip
      // and get ready for next pass.
      if (outcodeOut == out_code_0)
        {
        p1->x = (gdi_dim_t)x;
        p1->y = (gdi_dim_t)y;
        out_code_0 = compute_out_code(p1, clip_rect);
        }
      else
        {
        p2->x = (gdi_dim_t)x;
        p2->y = (gdi_dim_t)y;
        out_code_1 = compute_out_code(p2, clip_rect);
        }
      }
    }
  }
