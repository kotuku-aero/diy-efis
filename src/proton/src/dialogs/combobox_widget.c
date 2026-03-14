#include "../../include/combobox_widget.h"

static void on_paint(handle_t canvas, const rect_t *wnd_rect,
                     const canmsg_t *msg, void *wnddata)
  {
  combobox_widget_t *wnd = (combobox_widget_t *)wnddata;

  on_paint_widget_background(canvas, wnd_rect, msg, (widget_t *)wnd);

  const char *selected_text = nullptr;

  if (wnd->selected_item >= 0 && wnd->selected_item < wnd->num_items)
    selected_text = wnd->items[wnd->selected_item].text;

  if (selected_text != nullptr)
    {
    point_t pt;
    rect_top_left(wnd_rect, &pt);

    pt.x += 2;
    pt.y += 2;

    draw_text(canvas, wnd_rect, dialog_theme->edit_font, dialog_theme->edit_color,
      dialog_theme->background_color, 0, selected_text, &pt,
      wnd_rect, wnd->base.base.base.style, nullptr);
    }

  // outline the focused box with a grey line dashed line
  if (wnd->base.base.focused)
    draw_control_focus(canvas, wnd_rect, wnd_rect);
  }

static result_t combobox_wndproc(handle_t hwnd, const canmsg_t *msg,
                                 void *wnddata)
  {
  uint16_t id = get_can_id(msg);
  combobox_widget_t *wnd = wnddata;

  // look through the items and see if the can_id matches an item
  // and if so, set the selected item if the value matches
  for (uint32_t i = 0; i < wnd->num_items; i++)
    {
    if (wnd->items[i].can_id == id)
      {
      uint32_t selected_value;
      // now check the value being set
      if (failed (get_param_uint32(msg, &selected_value)))
        continue;
      uint32_t value;
      if (succeeded(coerce_to_uint32(&wnd->items[i].value, &value)) &&
        value == selected_value)
        {
        wnd->selected_item = (int32_t) i;
        invalidate(hwnd);

        return s_ok;
        }
      }
    }

  // this will handle setfocus/loosefocus
  return dialog_wndproc(hwnd, msg, wnddata);
  }

result_t create_combobox_widget(handle_t parent, uint32_t flags,
                                combobox_widget_t *wnd, handle_t *out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result = create_widget(parent, flags, combobox_wndproc,
                                    &wnd->base.base.base, &hndl)))
    return result;

  wnd->base.base.base.on_paint = on_paint;

  // determine if this is a static or dynamic combobox
  if (wnd->num_items == 0 && wnd->base.base.menu_name != nullptr)
    {
    menu_widget_t *menu_widget;
    if (result = get_wnddata(menu_handle, (void **) &menu_widget))
      return result;

      // find the menu

    const menu_t *menu;
    if (failed(result = find_menu(menu_widget, wnd->base.base.menu_name, &menu)))
      return result;

    // create a list of the items
    wnd->items = calloc(menu->num_items, sizeof(combobox_widget_item_t));

    // copy them so the paint handler can render the selected item
    wnd->num_items = menu->num_items;
    for (uint32_t i = 0; i < wnd->num_items; i++)
      {
      // only support events
      if (menu->menu_items[i]->item_type != mi_event)
        return e_bad_parameter;

      const menu_item_event_t *event = (const menu_item_event_t *)menu->menu_items[i];
      // assign the text
      wnd->items[i].text = event->base.caption;
      wnd->items[i].can_id = event->can_id;
      copy_variant(&event->value, &wnd->items[i].value);
      }
    }

  wnd->selected_item = -1;

  if (out != 0)
    *out = hndl;

  return s_ok;
  }
