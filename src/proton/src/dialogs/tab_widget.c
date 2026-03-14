#include "../../include/tab_widget.h"
#include "../../include/panel_widget.h"

static void on_paint(handle_t canvas, const rect_t *wnd_rect,
                     const canmsg_t *msg, void *wnddata)
  {
  tab_widget_t *wnd = (tab_widget_t *)wnddata;
  extent_t ex;
  rect_extents(wnd_rect, &ex);

  rect_t name_rect;
  point_t pt;
  point_t pts[4];
  rect_create_pt(point_create(0, 0, &pt), &wnd->tab_name_extent, &name_rect);
  //
  // // the reference for a vertical tab is bottom left, not top left
  // if ((wnd->base.base.style & TAB_VERTICAL) != 0)
  //   rect_bottom_left(&name_rect, &pt);

  // draw the tab buttons
  for (uint32_t i = 0; i < wnd->num_tabs; i++)
    {
    color_t background_color = (i == wnd->selected_index)
                                  ? dialog_theme->background_color_selected
                                  : dialog_theme->background_color;
    color_t foreground_color = (i == wnd->selected_index)
                              ? dialog_theme->foreground_color_selected
                              : dialog_theme->foreground_color;

    rectangle(canvas, wnd_rect, color_hollow, background_color, &name_rect);

    draw_text(canvas, &name_rect, dialog_theme->label_font,
              foreground_color, background_color, 0,
              wnd->tabs[i]->base.base.base.name, &pt, &name_rect,
              wnd->base.base.style , 0);

    if ((wnd->base.base.style & TAB_VERTICAL) != 0)
      {
      if (i == wnd->selected_index)
        {
        // draw a selected outline
        pts[0].x = name_rect.right;
        pts[0].y = name_rect.top;
        pts[1].x = name_rect.left;
        pts[1].y = name_rect.top;
        pts[2].x = name_rect.left;
        pts[2].y = name_rect.bottom -1;
        pts[3].x = name_rect.right;
        pts[3].y = name_rect.bottom -1;

        polyline(canvas, wnd_rect, dialog_theme->border_color, 4, pts);
        }
      else
      {
        // draw a deselected baseline
        pts[0].x = name_rect.right -1;
        pts[0].y = name_rect.top;
        pts[1].x = pts[0].x;
        pts[1].y = name_rect.bottom;

        polyline(canvas, wnd_rect, dialog_theme->border_color, 2, pts);
      }
      }
    else
    {
      if (i == wnd->selected_index)
      {
        // draw a selected outline
        pts[0].x = name_rect.left;
        pts[0].y = name_rect.bottom;
        pts[1].x = name_rect.left;
        pts[1].y = name_rect.top;
        pts[2].x = name_rect.right -1;
        pts[2].y = name_rect.top;
        pts[3].x = name_rect.right -1;
        pts[3].y = name_rect.bottom;

        polyline(canvas, wnd_rect, dialog_theme->border_color, 4, pts);
      }
      else
      {
        // draw a deselected baseline
        pts[0].x = name_rect.left;
        pts[0].y = name_rect.bottom -1;
        pts[1].x = name_rect.right;
        pts[1].y = pts[0].y;

        polyline(canvas, wnd_rect, dialog_theme->border_color, 2, pts);
      }

      if (i == wnd->selected_index && succeeded(is_focused(wnd->base.base.hwnd)))
        draw_control_focus(canvas, wnd_rect, &name_rect);
    }

    // outline the focused box with a grey line dashed line
    if (i == wnd->focused_index && wnd->base.focused)
      draw_control_focus(canvas, wnd_rect, &name_rect);

    if ((wnd->base.base.style & TAB_VERTICAL) != 0)
      {
      name_rect.top += wnd->tab_name_extent.dy;
      name_rect.bottom += wnd->tab_name_extent.dy;
      pt.y += wnd->tab_name_extent.dy;

      if (name_rect.top > wnd_rect->bottom)
        break;
      }
    else
      {
      name_rect.left += wnd->tab_name_extent.dx;
      name_rect.right += wnd->tab_name_extent.dx;
      pt.x += wnd->tab_name_extent.dx;

      if (name_rect.left > wnd_rect->right)
        break;
      }

     }

  if ((wnd->base.base.style & TAB_VERTICAL) != 0)
    {
    // draw the underline to the end of the drawing area
    pts[0].x = name_rect.right -1;
    pts[0].y = wnd->num_tabs * wnd->tab_name_extent.dy;;
    pts[1].x = pts[0].x;
    pts[1].y = wnd_rect->bottom -1;

    // draw along the baseline to the end of the window (if visible)
    if (pts[0].y < wnd_rect->bottom)
      polyline(canvas, wnd_rect, dialog_theme->border_color, 2, pts);
   }
  else
    {
    // draw the underline to the end of the drawing area
    pts[0].x = wnd->num_tabs * wnd->tab_name_extent.dx;
    pts[0].y = name_rect.bottom -1;
    pts[1].x = wnd_rect->right;
    pts[1].y = pts[0].y;

    // draw along the baseline to the end of the window (if visible)
    if (pts[0].x < wnd_rect->right)
      polyline(canvas, wnd_rect, dialog_theme->border_color, 2, pts);
    }
  }

result_t select_tab_page(handle_t hwnd, uint32_t index)
{
  result_t result;
  tab_widget_t *wnd;
  if (failed(result = get_wnddata(hwnd, (void**) &wnd)))
    return result;

  if (index >= wnd->num_tabs)
    return e_bad_parameter;

  if (index == wnd->selected_index)
    return s_ok;

  if (wnd->selected_index < wnd->num_tabs)
    hide_window(wnd->tabs[wnd->selected_index]->base.base.base.hwnd);

  wnd->selected_index = index;
  show_window(wnd->tabs[index]->base.base.base.hwnd);

  invalidate(hwnd);

  return s_ok;
}

static result_t  wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  tab_widget_t *wnd = (tab_widget_t *)wnddata;

  switch (get_can_id(msg))
    {
    default:
      break;
    case id_up :
      if (wnd->selected_index < wnd->num_tabs -1)
        return select_tab_page(hwnd, wnd->selected_index+1);

      return s_ok;
    case id_down :
      if (wnd->selected_index > 0)
        return select_tab_page(hwnd, wnd->selected_index-1);

      return s_ok;
    }

  return dialog_wndproc(hwnd, msg, wnddata);
  }

result_t create_tab_widget(handle_t parent, uint32_t flags, tab_widget_t *wnd, handle_t *out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result = create_widget(parent, flags, wndproc, &wnd->base.base, &hndl)))
    return result;

  // make invalid so first select will enable the window
  wnd->selected_index = 0xffffffff;

  wnd->base.base.on_paint = on_paint;

  if (out != 0)
    *out = hndl;

  return s_ok;
  }
