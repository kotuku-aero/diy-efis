#include "../../include/annunciator_widget.h"

/*
 * The quorum message sends a struct like this:
 * 
typedef PACKED struct _quorum_msg_t {
  uint8_t can_type;     // always CANFLY_BINARY
  uint8_t flags;        // see below
  uint16_t chk;         // crc16 of message with chk field == 0
  uint32_t cookie;      // 0 = abdication, otherwise 1..2^32-1
  } quorum_msg_t;

 * the flags are:
 * 
 */

#define FLAG_SIDE           0x01
#define FLAG_SIDE_LEFT      0x00
#define FLAG_SIDE_RIGHT     0x01

#define FLAG_ROLE           0x02
#define FLAG_ROLE_MASTER    0x00
#define FLAG_ROLE_SECONDARY 0x02


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
  else if(can_id == id_kmag_quorum)
    {
    if((msg->data[1] & FLAG_SIDE) == FLAG_SIDE_RIGHT)
      {
      bool active = (msg->data[1] & FLAG_ROLE) == FLAG_ROLE_MASTER;
      changed = active != data->quorum_right_active;
      data->quorum_right_active = active;
      ticks(&data->last_right_heartbeat);
      }
    else
      {
      bool active = (msg->data[1] & FLAG_ROLE) == FLAG_ROLE_MASTER;
      changed = active != data->quorum_left_active;
      data->quorum_left_active = active;
      ticks(&data->last_left_heartbeat);
      }
    }
  else if (can_id == id_left_mixture_lever)
    {
    if (data->afr_map_mode == 0 &&
        data->left_status == bs_running &&
        succeeded(get_param_uint16(msg, &value_u16)))
    {
      changed = data->left_mixture != value_u16;
      data->left_mixture = value_u16;
      }
    else
      data->left_mixture = 100;
    }
  else if (can_id == id_right_mixture_lever)
    {
    if (data->afr_map_mode == 0 &&
        data->right_status == bs_running &&
        succeeded(get_param_uint16(msg, &value_u16)))
      {
      changed = data->right_mixture != value_u16;
      data->right_mixture = value_u16;
      }
    else
      data->right_mixture = 100;
    }
  else if (can_id = id_timer)
    {
    uint32_t now;
    ticks(&now);
    if ((now - data->last_right_heartbeat) > 1000 &&
        (data->right_status != bs_unknown || data->quorum_right_active))
    {
      changed = true;
      data->quorum_right_active = false;
      data->right_status = bs_unknown;
    }

    if ((now - data->last_left_heartbeat) > 1000 &&
        (data->left_status != bs_unknown || data->quorum_left_active))
      {
      changed = true;
      data->quorum_left_active = false;
      data->left_status = bs_unknown;
      }
    }

  return changed;
  }

static char manual_msg[CFG_NAME_MAX];

void on_paint_kmag(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata)
  {
  ecu_annunciator_t* data = (ecu_annunciator_t*)wnddata;

  // draw the left status
  rect_t rect_status;
  rect_t rect_master;
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

  // draw the master status
  if(data->quorum_left_active)
    {
    // draw a rectangle around the status as white
    rect_master.left = rect_status.left -2;
    rect_master.top = wnd_rect->top;
    rect_master.right = rect_status.right +2;
    rect_master.bottom = wnd_rect->bottom -2;
    
    rectangle(canvas, wnd_rect, color_lightgreen, color_lightgreen, &rect_master);
    }

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

  // draw the master status
  if(data->quorum_right_active)
    {
    // draw a rectangle around the status as white
    rect_master.left = rect_status.left -2;
    rect_master.top = wnd_rect->top;
    rect_master.right = rect_status.right +1;
    rect_master.bottom = wnd_rect->bottom-2;
    
    rectangle(canvas, wnd_rect, color_lightgreen, color_lightgreen, &rect_master);
    }

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
      {
      uint16_t quorum_mix = 0;
      if(data->left_status == bs_running)
        quorum_mix = data->left_mixture;

      if(data->right_status == bs_running)
        quorum_mix += data->right_mixture;

      if(data->left_status == bs_running && data->right_status == bs_running)
        quorum_mix >>= 1;

      snprintf(manual_msg, CFG_NAME_MAX, "Manual %d%%", quorum_mix);
      afr_mode = manual_msg;
      }
      break;
    case 2:
      afr_mode = "Auto";
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

static e_board_status left_status = bs_unknown;
static e_board_status right_status = bs_unknown;

static uint16_t left_fuel_map_active = 0;
static uint16_t right_fuel_map_active = 0;

static uint16_t afr_map_active = 0;

static handle_t doorbell;
static uint32_t afr_map_change_delay = 0;


static void kmag_worker(void* parg)
  {
  semaphore_create(&doorbell);
  while (true)
    {
    // wake up every 100msec
    semaphore_wait(doorbell, 100);

    // to allow for the mode change of the mixture mode the
    // status of 4 (Error) is only entered after 5 seconds

    if (left_status == bs_running || right_status == bs_running)
      {
      // see if in debounce time
      if (left_status == bs_running && right_status != bs_running)
        afr_map_active = left_fuel_map_active;
      else if (right_status == bs_running && left_status != bs_running)
        afr_map_active = right_fuel_map_active;
      else if (right_status == bs_running && left_status == bs_running)
        {
        // Both mags are running.
        // first see if both status are same
        if (left_fuel_map_active == right_fuel_map_active)
          {
          afr_map_active = left_fuel_map_active; // just use left
          afr_map_change_delay = 0; // and reset counter
          }
        else if (afr_map_active < 5) // error is sticky
          {
          // divergence between 2 mags, so delay for 2500 msec
          // first check if delay counter is running
          if (afr_map_change_delay > 0)
            {
            // already running so add 100msec
            afr_map_change_delay += 100;

            // 2.5 secs?
            if (afr_map_change_delay >= 2500)
              {
              if (left_fuel_map_active != right_fuel_map_active)
                afr_map_active = 5; // error
              else
                // this condition should have been handled above.
                afr_map_active = left_fuel_map_active; // just use left
              }
            else
              afr_map_active = 4; // In Sync
            }
          else
            {
            afr_map_change_delay = 100; // start the counter running
            afr_map_active = 4; // In Sync
            }
          }
        }

      canmsg_t msg;
      create_can_msg_uint16(&msg, id_fuel_map, afr_map_active);

      // publish the map that is active
      publish_local(&msg, INDEFINITE_WAIT);
      }
    }
  }

// called when a msg arrives.
static bool ev_msg(const canmsg_t* msg, void* parg)
  {
  if (msg == 0)
    return false;

  uint32_t now;
  ticks(&now);

  uint16_t tmp_uint16;
  uint32_t tmp_uint32;

  switch (get_can_id(msg))
    {
    case id_status_node_2:
    case id_status_node_3:
      if (msg->data[0] == 0xFF &&
          (msg->data[3] & BOARD_TYPE_MASK) == mag_board_id)
        {
        // status information
        // [0] = 0xFF  (BINARY)
        // [1] = NodeID
        // [2] = Board Status
        // [3] = Board Type
        // [4..7] = Board serial number 

        switch (msg->data[3] & BOARD_FEATURE_MASK)
          {
          case mag_left_board_type:
            left_status = (e_board_status)(msg->data[2]);
            break;
          case mag_right_board_type:
            right_status = (e_board_status)(msg->data[2]);
            break;
          }
        }
      break;
    case id_left_fuel_map:
      get_param_uint16(msg, &left_fuel_map_active);
      break;
    case id_right_fuel_map:
      get_param_uint16(msg, &right_fuel_map_active);
      break;
    case id_left_mixture_mode :
      // TODO: cutoff??
      get_param_uint16(msg, &tmp_uint16);
      left_fuel_map_active = tmp_uint16 == 2 ? 2 : 0;
      break;
    case id_right_mixture_mode :
      // TODO: cuttoff??
      get_param_uint16(msg, &tmp_uint16);
      right_fuel_map_active = tmp_uint16 == 2 ? 2 : 0;
      break;
    }

  return false;
  }

result_t on_create_kmag(handle_t hwnd, widget_t* widget)
  {
  result_t result;

  // hook the publisher
  subscribe(ev_msg, 0, 0);

  // create the worker
  return task_create("EDU", DEFAULT_STACK_SIZE, kmag_worker, 0, HIGH_PRIORITY, 0);
  }