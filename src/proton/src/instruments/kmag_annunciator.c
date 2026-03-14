#include "../../include/annunciator_widget.h"

result_t on_kmag_msg(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  ecu_annunciator_t* data = (ecu_annunciator_t*)wnddata;

  bool changed = false;
  int16_t value_i16;
  uint16_t value_u16;

  uint16_t can_id = get_can_id(msg);
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
    if ((msg->data[3] & 0x1F) == 3)
      {

      switch (msg->data[3] & 0xE0)
        {
        case 0x00:
          changed = data->left_status != (e_board_status)msg->data[2];
          data->left_status = (e_board_status)msg->data[2];
          break;
        case 0x20:
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

  if (changed)
    invalidate(hwnd);

  return s_false;
  }

void on_paint_kmag(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata)
  {
  ecu_annunciator_t* data = (ecu_annunciator_t*)wnddata;

  // draw the left status
  rect_t rect_status;
  int32_t height = rect_height(wnd_rect);

  on_paint_widget_background(canvas, wnd_rect, msg, wnddata);

  int32_t indicator_height = height - 3;
  if (indicator_height > 12)
    indicator_height = 12;

  int32_t indicator_center = height >> 1;

  rect_status.left = wnd_rect->left + 2;
  rect_status.top = indicator_center - (indicator_height >> 1);
  rect_status.right = rect_status.left + indicator_height;
  rect_status.bottom = rect_status.top + indicator_height;

  if (data->left_status == bs_unknown)
    ellipse(canvas, wnd_rect, color_white, color_red, &rect_status);
  else if (data->left_status == bs_running)
    ellipse(canvas, wnd_rect, color_white, color_lightgreen, &rect_status);
  else if (data->left_status == bs_inhibited)
    ellipse(canvas, wnd_rect, color_white, color_orange, &rect_status);
  else
    ellipse(canvas, wnd_rect, color_white, color_black, &rect_status);

  rect_status.left = wnd_rect->right - indicator_height - 3;
  rect_status.right = rect_status.left + indicator_height;

  if (data->right_status == bs_unknown)
    ellipse(canvas, wnd_rect, color_white, color_red, &rect_status);
  else if (data->right_status == bs_running)
    ellipse(canvas, wnd_rect, color_white, color_lightgreen, &rect_status);
  else if (data->right_status == bs_inhibited)
    ellipse(canvas, wnd_rect, color_white, color_orange, &rect_status);
  else
    ellipse(canvas, wnd_rect, color_white, color_black, &rect_status);

  const char* afr_mode = nullptr;
  color_t afr_mode_color = color_lightblue;
  switch (data->afr_map_mode)
    {
    case 0:
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
      (height >> 1) - (ex.dy >> 1)
      };

    draw_text(canvas, wnd_rect, data->base.base.name_font, afr_mode_color, color_hollow, 0, afr_mode, &pt, wnd_rect, 0, 0);
    }
  }
