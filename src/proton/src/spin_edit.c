#include "../include/menu_window.h"

menu_item_action_result item_spin_edit_evaluate(menu_widget_t* wnd,
                                                menu_item_t* item,
                                                const canmsg_t* msg)
  {
  menu_item_spin_edit_t* edit = (menu_item_spin_edit_t*)item;

  wnd->edit = edit;
  wnd->active_keys = edit->keys;

  variant_t tmp_value;

  // the min-max are in the same format as the value (SI units)
  // so we need to convert them to the same format as converter
  // displays in

  (*edit->get_value)(item, &tmp_value);
  coerce_to_float(&tmp_value, &edit->value);

  if(edit->get_min_value != nullptr)
    {
    (*edit->get_min_value)(item, &tmp_value);
    coerce_to_float(&tmp_value, &edit->min_value);
    }

  if(edit->get_max_value != nullptr)
    {
    (*edit->get_max_value)(item, &tmp_value);
    coerce_to_float(&tmp_value, &edit->max_value);
    }

  // if there is a value converter, then convert the value
  if (edit->converter != nullptr)
    {
    edit->value = edit->converter->convert(edit->value);
    edit->min_value = edit->converter->convert(edit->min_value);
    edit->max_value = edit->converter->convert(edit->max_value);
    }

  item->is_selected = true;

  return mia_show;
  }

void item_spin_edit_event(menu_widget_t* wnd,
  menu_item_t* item,
  const canmsg_t* msg)
  {
  // check the enabler
  default_enable_handler(wnd, item, msg);

  menu_item_spin_edit_t* edit = (menu_item_spin_edit_t*)item;
  variant_t tmp_value;

  // see if this changes the edit
  switch (get_can_id(msg))
    {
    case id_ok:
      if (item->is_selected)
        {
        // this converts the scaled value to the SI value
        if(edit->converter != nullptr)
          create_variant_float(edit->converter->convert_inverse(edit->value), &tmp_value);
        else
          create_variant_float(edit->value, &tmp_value);

        (*edit->set_value)(item, &tmp_value);
        item->is_selected = false;
        close_menu(wnd);
        }
      break;
    case id_cancel:
      if (item->is_selected)
        {
        // wnd->edit = 0;
        item->is_selected = false;
        close_menu(wnd);
        }
      break;
    case id_spin_edit:
      if (item->is_selected)
        {
        int16_t value_int16;
        get_param_int16(msg, &value_int16);

        // this allows for adding 0.1 etc as the min increment
        float value = (float)value_int16;

        if(edit->converter != nullptr)
          value *= edit->converter->increment_factor;

        edit->value += value;

        if (edit->value < edit->min_value)
          {
          if (edit->circular)
            edit->value = edit->max_value - (edit->min_value - edit->value);
          else
            edit->value = edit->min_value;
          }

        if (edit->value > edit->max_value)
          {
          if (edit->circular)
            edit->value = edit->min_value + (edit->value - edit->max_value);
          else
            edit->value = edit->max_value;
          }

        // and redraw
        invalidate(wnd->window);
        }
    }
  }

void item_spin_edit_paint(handle_t canvas,
  menu_widget_t* wnd,
  const rect_t* wnd_rect,
  menu_item_t* item,
  const rect_t* rect,
  bool is_selected)
  {
  menu_item_spin_edit_t* edit = (menu_item_spin_edit_t*)item;

  point_t center_pt = {
                       (rect_width(rect) >> 1) + rect->left,
                       (rect_height(rect) >> 1) + rect->top
    };

  rectangle(canvas, wnd_rect, wnd->menu_border_color,
    /*is_selected ? wnd->selected_background_color :*/ wnd->base.background_color, rect);

  // calculate the text extents
  char str[32];

  snprintf(str, 31,
    edit->converter != nullptr ? edit->converter->format : edit->format,
    edit->value);
  str[31] = 0;

  extent_t ex;
  uint16_t len = (uint16_t)strlen(str);
  text_extent(wnd->font, len, str, &ex);

  center_pt.x -= ex.dx >> 1;
  center_pt.y -= ex.dy >> 1;

  draw_text(canvas, wnd_rect, wnd->font,
    is_selected ? wnd->selected_color : wnd->text_color,
    wnd->base.background_color,
    len, str, &center_pt, 0, 0, 0);
  }
