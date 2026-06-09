#include "../include/menu_window.h"

static const char *default_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

menu_item_action_result item_textedit_evaluate(menu_widget_t *wnd,
                                                menu_item_t *item,
                                                const canmsg_t *msg)
  {
  menu_item_textedit_t *edit = (menu_item_textedit_t *)item;

  wnd->text_edit = edit;
  wnd->active_keys = edit->keys;

  // start editing at the beginning
  edit->edit_posn = 0;
  // if we can't get the value then set to all blanks
  if (failed(edit->get_value(item, &edit->value)))
    create_variant_chars("", &edit->value);

  if (failed(edit->get_hint(item, &edit->hint)))
    create_variant_chars("", &edit->hint);

  item->is_selected = true;

  return mia_show;
  }

void item_textedit_event(menu_widget_t *wnd, menu_item_t *item,
                          const canmsg_t *msg)
  {
  // check the enabler
  default_enable_handler(wnd, item, msg);

  menu_item_textedit_t *edit = (menu_item_textedit_t *)item;
  int16_t i16;

  bool changed = false;
  // see if this changes the edit
  switch (get_can_id(msg))
    {
  case id_ok:
    if (item->is_selected)
      {
      // TODO: handle the set value conversion (trim hints etc?)

      (*edit->set_value)(item, &edit->value);
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
  case id_text_edit_next:
    if (item->is_selected)
      {
      get_param_int16(msg, &i16);
      // this will move up/down the text edit based on the 
      if (i16 < 0)
        {
        if (edit->edit_posn > 0)
          {
          edit->value.value.chars[edit->edit_posn] = 0;
          edit->edit_posn--;
          changed = true;
          }
        }
      else if (edit->edit_posn < 7)
          {
          edit->edit_posn++;
          changed = true;
          }
      }
    break;
  case id_text_edit_char:
    if (item->is_selected)
      {
      get_param_int16(msg, &i16);
      // this will move up/down the text edit based on the
      if (i16 < 0)
        {
        if (edit->value.value.chars[edit->edit_posn] == 0)
          edit->value.value.chars[edit->edit_posn] = '9';
        else
          {
          const char *cp =
              strchr(default_chars, edit->value.value.chars[edit->edit_posn]);

          if (cp == default_chars)
            // wrap around start
            edit->value.value.chars[edit->edit_posn] = '9';
          else
            {
            cp--;
            edit->value.value.chars[edit->edit_posn] = *cp;
            }
          }
        }
      else
        {
        if (edit->value.value.chars[edit->edit_posn] == 0)
          edit->value.value.chars[edit->edit_posn] = 'A';
        else
          {
          const char *cp =
              strchr(default_chars, edit->value.value.chars[edit->edit_posn]);

          cp++;
          if (*cp == 0)
            edit->value.value .chars[edit->edit_posn] = 'A';
          else
            edit->value.value.chars[edit->edit_posn] = *cp;
          }
        }

      // and redraw
      changed = true;
      }
    break;
    }

  if (changed)
    {
    // calculate a new hint
    create_variant_chars(edit->value.value.chars, &edit->hint);
    if (edit->get_hint != nullptr)
      (*edit->get_hint)(item, &edit->hint);

    invalidate(wnd->window);
    }

  }

void item_textedit_paint(handle_t canvas, menu_widget_t *wnd,
                          const rect_t *wnd_rect, menu_item_t *item,
                          const rect_t *rect, bool is_selected)
  {
  menu_item_textedit_t *edit = (menu_item_textedit_t *)item;

  rectangle(canvas, wnd_rect, wnd->menu_border_color,
            wnd->base.background_color,
            rect);

  point_t center_pt = { rect_width(rect) >> 1, rect_height(rect) >> 1};

  if (edit->char_width.dx == 0)
    text_extent(wnd->font, 1, "W", &edit->char_width);

  point_t pt;
  char ch[2] = { 0, 0 };
  color_t text_color;
  // work over the accepted vs hint
  for (int i = 0; i < sizeof(edit->value.value.chars); i++)
    {
    if (i == edit->edit_posn)
      text_color = color_magenta;
    else if (i < edit->edit_posn)
      text_color = color_lightgreen;
    else if (edit->hint.value.chars[i] != 0)
      text_color = color_lightblue;

    if (i <= edit->edit_posn)
      ch[0] = edit->value.value.chars[i];
    else
      ch[0] = edit->hint.value.chars[i];
      
    if (ch[0] == 0)
      break;
  
    // only ever 7 chars, so offset the position
    gdi_dim_t cp = i - 3;
    cp *= edit->char_width.dx;

    point_create(center_pt.x + cp + rect->left, rect->top + center_pt.y - (edit->char_width.dy >> 1), &pt);
    draw_text(canvas, wnd_rect, wnd->font, text_color, color_hollow, 1, ch, &pt,
              rect, eto_clipped, 0);
    }
  }
