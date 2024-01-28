#include "annunciator_widget.h"

void on_paint_background_auto(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, annunciator_t* wnd)
  {
  auto_annunciator_t* ann = (auto_annunciator_t*)wnd;

  rect_t clip_rect;
  on_paint_widget_background(canvas, wnd_rect, msg, ann);

  if ((ann->base.base.base.style & DRAW_NAME) != 0)
    {
    // calculate the size of the label
    rect_create(ann->base.label_offset, wnd_rect->top + 1, ann->base.text_offset - 1, wnd_rect->bottom - 1, &clip_rect);

    uint16_t text_len = (uint16_t)strlen(ann->base.base.base.name);

    point_t label_origin = { ann->base.label_offset, wnd_rect->top + 1 };

    draw_text(canvas, wnd_rect, ann->base.small_font, ann->base.label_color, ann->base.base.base.background_color,
      text_len, ann->base.base.base.name, &label_origin, &clip_rect, 0, 0);
    }

  }

/**
 * @brief Draw a basic text annunciator
 * @param canvas    canvas to draw with
 * @param wnd_rect  rectange of the window
 * @param pt        point in window to draw at
 * @param label     Text label
 * @param value     value to print
 */
static void on_draw_text(handle_t canvas, const rect_t* wnd_rect, annunciator_t* wnd, const char* value)
  {
  text_annunciator_t* ann = (text_annunciator_t*)wnd;
  rect_t clip_rect;

  point_t origin = { ann->text_offset, wnd_rect->top + 1 };
  //rect_create(origin.x, origin.y, wnd_rect->right, wnd_rect->bottom - 1, &clip_rect);
  rect_create(1, 1, wnd_rect->right, wnd_rect->bottom - 1, &clip_rect);

  // erase the old text
  rectangle(canvas, &clip_rect, color_hollow, color_hollow, &clip_rect);

  draw_text(canvas, wnd_rect, ann->small_font, ann->text_color, wnd->base.background_color,
    0, value, &origin, &clip_rect, 0, 0);
  }

result_t on_auto_msg(handle_t hwnd, uint16_t can_id, const canmsg_t* msg, void* wnddata)
  {
  auto_annunciator_t* ann = (auto_annunciator_t*)wnddata;

  bool changed = false;

  if (get_can_id(msg) == ann->base.can_id)
    {

    variant_t old_value;
    copy_variant(&ann->value, &old_value);

    variant_t new_value;
    if (failed(msg_to_variant(msg, &new_value)))
      return false;

    if (failed(coerce_variant(&new_value, &ann->value, ann->value_type)))
      return false;


    changed = compare_variant(&old_value, &ann->value) != 0;
    }

  return changed ? s_ok : s_false;
  }

void on_paint_auto(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnd)
  {
  auto_annunciator_t* ann = (auto_annunciator_t*)wnd;
  char txt[16];
  switch (ann->value.vt)
    {
    case v_float:
      if (ann->converter != 0)
        sprintf(txt, ann->fmt, (*(*ann->converter)->convert_float)(ann->value.value.flt));
      else
        sprintf(txt, ann->fmt, ann->value.value.flt);
      break;
    case v_int16:
      if (ann->converter != 0)
        sprintf(txt, ann->fmt, (*(*ann->converter)->convert_int16)(ann->value.value.int16));
      else
        sprintf(txt, ann->fmt, ann->value.value.int16);
      break;
    case v_uint16:
      if (ann->converter != 0)
        sprintf(txt, ann->fmt, (*(*ann->converter)->convert_uint16)(ann->value.value.uint16));
      else
        sprintf(txt, ann->fmt, ann->value.value.uint16);
      break;
    case v_int32:
      if (ann->converter != 0)
        sprintf(txt, ann->fmt, (*(*ann->converter)->convert_int32)(ann->value.value.int32));
      else
        sprintf(txt, ann->fmt, ann->value.value.int32);
      break;
    case v_uint32:
      if (ann->converter != 0)
        sprintf(txt, ann->fmt, (*(*ann->converter)->convert_uint32)(ann->value.value.uint32));
      else
        sprintf(txt, ann->fmt, ann->value.value.uint32);
      break;
    default:
      strcpy(txt, "---");
      break;
    }

  on_draw_text(canvas, wnd_rect, (annunciator_t*)ann, txt);
  }

bool on_def_utc_msg(handle_t hwnd, uint16_t can_id, const canmsg_t* msg, void* wnddata)
  {
  utc_annunciator_t* data = (utc_annunciator_t*)wnddata;
  bool changed = false;

  if (get_can_id(msg) == data->base.can_id)
    {

    tm_t tm;
    get_param_utc(msg, &tm);

    uint16_t minutes = tm.minute + (tm.hour * 60);

    changed = data->clock != minutes;
    data->clock = minutes;
    }

  return changed;
  }

void on_paint_utc(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, annunciator_t* wnd)
  {
  utc_annunciator_t* data = (utc_annunciator_t*)wnd;

  char txt[10];

  sprintf(txt, "%02.2d:%02.2d", data->clock / 60, data->clock % 60);
  on_draw_text(canvas, wnd_rect, wnd, txt);
  }

result_t on_hours_msg(handle_t hwnd, uint16_t can_id, const canmsg_t* msg, void* wnddata)
  {
  hours_annunciator_t* data = (hours_annunciator_t*)wnddata;
  bool changed = false;

  if (get_can_id(msg) == data->base.can_id)
    {

    uint32_t value;
    get_param_uint32(msg, &value);
    changed = data->hours != value;
    data->hours = value;
    }

  return changed ? s_ok : s_false;
  }

void on_paint_hours(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata)
  {
  hours_annunciator_t* data = (hours_annunciator_t*)wnddata;

  char txt[10];

  float hrs = ((float)data->hours) / 100;

  sprintf(txt, "%06.1f", hrs);
  on_draw_text(canvas, wnd_rect, wnddata, txt);
  }

result_t on_hp_msg(handle_t hwnd, uint16_t can_id, const canmsg_t* msg, void* wnddata)
  {
  hp_annunciator_t* data = (hp_annunciator_t*)wnddata;

  bool changed = false;
  if (get_can_id(msg) == data->base.can_id)
    {
    uint16_t value;
    get_param_uint16(msg, &value);

    changed = data->hp != value;
    data->hp = value;
    }

  return changed ? s_ok : s_false;
  }

void on_paint_hp(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, annunciator_t* wnddata)
  {
  hp_annunciator_t* data = (hp_annunciator_t*)wnddata;

  char txt[10];

  sprintf(txt, "%d %%", data->hp);
  on_draw_text(canvas, wnd_rect, wnddata, txt);
  }

result_t on_hobbs_msg(handle_t hwnd, uint16_t can_id, const canmsg_t* msg, void* wnddata)
  {
  hobbs_annunciator_t* data = (hobbs_annunciator_t*)wnddata;

  bool changed = false;

  if (get_can_id(msg) == data->base.can_id)
    {
    int16_t value;
    get_param_int16(msg, &value);

    changed = data->hobbs != value;
    data->hobbs = value;
    }

  return changed ? s_ok : s_false;
  }

void on_paint_hobbs(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, annunciator_t* wnddata)
  {
  hobbs_annunciator_t* data = (hobbs_annunciator_t*)wnddata;

  char txt[10];

  sprintf(txt, "%d", data->hobbs);
  on_draw_text(canvas, wnd_rect, wnddata, txt);
  }

result_t annunciator_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  uint16_t can_id = get_can_id(msg);
  annunciator_t* wnd = (annunciator_t*)wnddata;

  if (wnd != 0 && (*wnd->base.on_message)(hwnd, can_id, msg, wnd))
    {
    invalidate_foreground_rect(hwnd, 0);

    return s_ok;
    }

  // pass to default
  return widget_wndproc(hwnd, msg, wnddata);
  }


result_t create_annunciator_widget(handle_t parent, uint16_t id, aircraft_t* aircraft, annunciator_t* wnd, handle_t* out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result = create_widget(parent, id, annunciator_wndproc, &wnd->base, &hndl)))
    return result;

  if (out != 0)
    *out = hndl;

  return s_ok;
  }
