#include "annunciator_widget.h"

#include "../../libs/atom/board_id.h"

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


result_t on_kmag_msg(handle_t hwnd, uint16_t can_id, const canmsg_t* msg, void* wnddata)
  {
  ecu_annunciator_t* data = (ecu_annunciator_t*)wnddata;

  bool changed = false;
  int16_t value_i16;
  uint16_t value_u16;

  if (can_id >= id_status_node_0 && can_id <= id_status_node_15)
    {
    // status information
    // [0] = 0xFF  (BINARY)
    // [1] = NodeID
    // [2] = Board Status e_board_status
    // [3] = Board Type  Note bits 7--5 are the feature type
    // [4] = Serial Number 31..24
    // [5] = Serial Number 23..16
    // [6] = Serial Number 15..8
    // [7] = Serial Number 7..0
    if ((msg->data[3] & BOARD_TYPE_MASK) == mag_board_id)
      {

      switch (msg->data[3] & BOARD_FEATURE_MASK)
        {
        case mag_left_board_type:
          changed = data->left_status != (e_board_status)msg->data[2];
          data->left_status = (e_board_status)msg->data[2];
          break;
        case mag_right_board_type:
          changed = data->right_status != (e_board_status)msg->data[2];
          data->right_status = (e_board_status)msg->data[2];
          break;
        }
      }
    }
  else if (can_id == id_left_ignition_advance)
    {
    if (succeeded(get_param_int16(msg, &value_i16)))
      {
      changed = data->left_advance != value_i16;
      data->left_advance = value_i16;
      }
    }
  else if (can_id == id_right_ignition_advance)
    {
    if (succeeded(get_param_int16(msg, &value_i16)))
      {
      changed = data->right_advance != value_i16;
      data->right_advance = value_i16;
      }
    }
  else if (can_id == id_fuel_map)
    {
    if (succeeded(get_param_uint16(msg, &value_u16)))
      {
      changed = data->afr_map_mode != value_u16;
      data->afr_map_mode = value_u16;
      }
    }

  return changed ? s_ok : s_false;
  }

void on_paint_kmag(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, annunciator_t* wnddata)
  {
  ecu_annunciator_t* data = (ecu_annunciator_t*)wnddata;

  // draw the left status
  rect_t rect_status;
  int32_t height = rect_height(wnd_rect);

  int32_t indicator_height = height - 3;
  if (indicator_height > 12)
    indicator_height = 12;

  int32_t indicator_center = height >> 1;

  rect_status.left = wnd_rect->left + 2;
  rect_status.top = indicator_center - (indicator_height >> 1);
  rect_status.right = rect_status.left + indicator_height;
  rect_status.bottom = rect_status.top + indicator_height;

  if (data->left_status == bs_unknown || data->left_status == bs_fault)
    ellipse(canvas, wnd_rect, color_white, color_red, &rect_status);
  else if (data->left_status == bs_running)
    ellipse(canvas, wnd_rect, color_white, color_lightgreen, &rect_status);
  else if (data->left_status == bs_inhibited)
    ellipse(canvas, wnd_rect, color_white, color_orange, &rect_status);
  else
    ellipse(canvas, wnd_rect, color_white, color_black, &rect_status);

  rect_status.left = wnd_rect->right - indicator_height - 3;
  rect_status.right = rect_status.left + indicator_height;

  if (data->right_status == bs_unknown || data->right_status == bs_fault)
    ellipse(canvas, wnd_rect, color_white, color_red, &rect_status);
  else if (data->right_status == bs_running)
    ellipse(canvas, wnd_rect, color_white, color_lightgreen, &rect_status);
  else if (data->right_status == bs_inhibited)
    ellipse(canvas, wnd_rect, color_white, color_orange, &rect_status);
  else
    ellipse(canvas, wnd_rect, color_white, color_black, &rect_status);

  const char *afr_mode = nullptr;
  color_t afr_mode_color = color_lightblue;
  switch (data->afr_map_mode)
    {
    case 0 :
      afr_mode = "Manual";
      break;
    case 1:
      afr_mode = "Nitro";
      break;
    case 2:
      afr_mode = "Climb";
      break;
    case 3:
      afr_mode = "Cruise";
      break;
    case 4:
      afr_mode_color = color_purple;
      afr_mode = "ECU SYNC";
      break;
    case 5:
      afr_mode_color = color_red;
      afr_mode = "AFR ERR";
      break;
    }

  if (afr_mode != nullptr)
    {
    // draw the mixture stats
    extent_t ex;
    text_extent(data->base.base.name_font, 0, afr_mode, &ex);

    point_t pt = 
      {
      (rect_width(wnd_rect) >> 1) - (ex.dx >> 1),
      (height  >> 1) - (ex.dy >> 1)
      };

    draw_text(canvas, wnd_rect, data->base.base.name_font, afr_mode_color, color_hollow, 0, afr_mode, &pt, wnd_rect, 0, 0);
    }
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
