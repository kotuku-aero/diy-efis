#include "photon_priv.h"

typedef int out_code_t;

#define INSIDE 0
#define LEFT 1
#define RIGHT 2
#define ABOVE 4
#define BELOW 8

static inline out_code_t compute_out_code(const point_t* pt, const rect_t* rect)
  {
  out_code_t code = INSIDE;

  code |= (pt->x < rect->left) ? LEFT : ((pt->x > rect->right) ? RIGHT : 0);
  code |= (pt->y < rect->top) ? ABOVE : ((pt->y > rect->bottom) ? BELOW : 0);

  return code;
  }

bool clip_line(point_t* p1, point_t* p2, const rect_t* clip_rect)
  {
  out_code_t out_code_p1 = compute_out_code(p1, clip_rect);
  out_code_t out_code_p2 = compute_out_code(p2, clip_rect);
  point_t* p_out;
  out_code_t* out_code_out;

  while (true)
    {
    if (!(out_code_p1 | out_code_p2))
    {
      return true;
    }
    else if (out_code_p1 & out_code_p2)
    {
      return false;
      }
    else
      {
      float x, y;

      if (out_code_p1)
      {
        p_out = p1;
        out_code_out = &out_code_p1;
      }
      else
      {
        p_out = p2;
        out_code_out = &out_code_p2;
      }

      if (*out_code_out & BELOW)
      {
        x = (float)(p1->x + (p2->x - p1->x) * (clip_rect->bottom - p1->y) / (p2->y - p1->y));
        y = clip_rect->bottom;
        }
      else if (*out_code_out & ABOVE)
      {
        x = (float)(p1->x + (p2->x - p1->x) * (clip_rect->top - p1->y) / (p2->y - p1->y));
        y = clip_rect->top;
        }
      else if (*out_code_out & RIGHT)
      {
        y = (float)(p1->y + (p2->y - p1->y) * (clip_rect->right - p1->x) / (p2->x - p1->x));
        x = clip_rect->right;
        }
      else if (*out_code_out & LEFT)
      {
        y = (float)(p1->y + (p2->y - p1->y) * (clip_rect->left - p1->x) / (p2->x - p1->x));
        x = clip_rect->left;
        }

      p_out->x = (gdi_dim_t)x;
      p_out->y = (gdi_dim_t)y;
      *out_code_out = compute_out_code(p_out, clip_rect);
      }
    }
  }
