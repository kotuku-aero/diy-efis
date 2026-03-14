#include "../../include/annunciator_widget.h"

/**
 * @brief Draw a basic text annunciator
 * @param canvas    canvas to draw with
 * @param wnd_rect  rectange of the window
 * @param pt        point in window to draw at
 * @param label     Text label
 * @param value     value to print
 */
void on_draw_text(handle_t canvas, const rect_t* wnd_rect, annunciator_t* wnd, const char* value)
  {
  text_annunciator_t* ann = (text_annunciator_t*)wnd;
  rect_t clip_rect;
  extent_t ex;
  point_t origin;

  if (ann->base.background_canvas == nullptr)
    {
    rect_extents(wnd_rect, &ex);
    // create a canvas
    canvas_create(&ex, &ann->base.background_canvas);

    uint32_t style = wnd->base.style;

    if(ann->on_paint_background != 0)
      ann->on_paint_background(ann->base.background_canvas, wnd_rect, nullptr, wnd);
    else if((style & FILL_BACKGROUND)== 0)
      on_paint_text_background(ann->base.background_canvas, wnd_rect, nullptr, wnd);

    on_paint_widget_background(ann->base.background_canvas, wnd_rect, nullptr, (widget_t *) wnd);

    if ((style & DRAW_NAME) != 0)
      {
      // calculate the size of the label
      if(ann->compact)
        rect_create(ann->label_offset, wnd_rect->top + 1, rect_width(wnd_rect) -2, rect_height(wnd_rect) >> 1, &clip_rect);
      else
        rect_create(ann->label_offset, wnd_rect->top + 1, ann->text_offset - 1, wnd_rect->bottom - 1, &clip_rect);

      uint16_t text_len = (uint16_t)strlen(ann->base.base.name);

      if (ann->compact)
        {
        text_extent(ann->small_font, text_len, ann->base.base.name, &ex);
        point_create((rect_width(wnd_rect) >> 1) - (ex.dx >> 1), wnd_rect->top+1, &origin);
        }
      else
        point_create(ann->label_offset, wnd_rect->top + 1, &origin);

      draw_text(ann->base.background_canvas, wnd_rect, ann->small_font, ann->label_color, ann->base.base.background_color,
        text_len, ann->base.base.name, &origin, &clip_rect, ann->label_format, 0);
      }
    }

  point_t pt = { 0, 0 };
  bit_blt(canvas, wnd_rect, wnd_rect, wnd->background_canvas, wnd_rect, &pt, src_copy);

  if (ann->compact)
    {
    text_extent(ann->small_font, 0, value, &ex);
    point_create((rect_width(wnd_rect) >> 1) - (ex.dx >> 1), rect_height(wnd_rect) >> 1, &origin);
    }
  else
    point_create(ann->text_offset, wnd_rect->top + 1, &origin);

  rect_create(1, 1, wnd_rect->right, wnd_rect->bottom - 1, &clip_rect);

  draw_text(canvas, wnd_rect, ann->small_font, ann->text_color, wnd->base.background_color,
    0, value, &origin, &clip_rect, ann->text_format, 0);
  }

void on_paint_text_background(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnd)
  {
  text_annunciator_t* ann = (text_annunciator_t*)wnd;

  // fill with 2 colors
  rect_t rect;
  rect_copy(wnd_rect, &rect);

  if (ann->compact)
    {
    rect.bottom = rect_height(&rect) >> 1;
    rectangle(canvas, wnd_rect, color_hollow, ann->base.base.background_color, &rect);
    rect.top = rect.bottom;
    rect.bottom = wnd_rect->bottom;
    rectangle(canvas, wnd_rect, color_hollow, color_black, &rect);
    }
  else
    {
    rect.right = ann->text_offset - ann->label_offset;
    rectangle(canvas, wnd_rect, color_hollow, ann->base.base.background_color, &rect);
    rect.left = rect.right;
    rect.right = wnd_rect->right;
    rectangle(canvas, wnd_rect, color_hollow, color_black, &rect);
    }
  }