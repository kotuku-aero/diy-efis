#include "../../include/groupbox_widget.h"

static void on_paint(handle_t canvas, const rect_t *wnd_rect,
                     const canmsg_t *msg, void *wnddata)
  {
  groupbox_widget_t *wnd = (groupbox_widget_t *)wnddata;

  on_paint_widget_background(canvas, wnd_rect, msg, wnddata);

  rect_t group_area = *wnd_rect;
  group_area.left +=4;
  group_area.top +=6;
  group_area.right -=4;
  group_area.bottom -=4;

  rectangle(canvas, wnd_rect, dialog_theme->border_color, color_hollow, &group_area);

  if ((wnd->base.base.base.style & DRAW_NAME) != 0)
    {
    point_t name_pt = { .x = group_area.left + 4, .y = wnd_rect->top };

    draw_text(canvas, wnd_rect, dialog_theme->label_font, dialog_theme->label_color,
      dialog_theme->background_color, 0, wnd->base.base.base.name, &name_pt, wnd_rect,
      eto_opaque, nullptr);
    }
  }

result_t create_groupbox_widget(handle_t parent, uint32_t flags,
                              groupbox_widget_t *wnd, handle_t *out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result = create_widget(parent, flags, dialog_wndproc, &wnd->base.base.base, &hndl)))
    return result;

  wnd->base.base.base.on_paint = on_paint;

  if (out != 0)
    *out = hndl;

  return s_ok;
  }