#ifndef __proton_h__
#define	__proton_h__

#include "../../include/vector.h"
#include "../../include/type_vector.h"

#include "../../include/canfly.h"

typedef struct _aircraft_t aircraft_t;

typedef result_t (*create_widgets_fn)(handle_t hwnd);
typedef result_t (*create_layout_fn)(handle_t parent, memid_t key, create_widgets_fn layout_fn, wndproc_fn wndproc, handle_t* hndl);
// these will hold the optional menu controls for a layout.
extern handle_t menu_handle;
extern handle_t right_menu_handle;

extern result_t run_proton(memid_t memid, create_layout_fn create_layout, create_widgets_fn create_widgets, wndproc_fn layout_wndproc);

extern const font_t neo_9_font;
extern const font_t neo_9_bold_font;
extern const font_t neo_12_font;
extern const font_t neo_12_bold_font;
extern const font_t neo_15_font;
extern const font_t neo_18_font;

// these are the ID's used by proton
// events
enum proton_events
  {
  id_left = id_last_photon,
  id_right,
  id_buttonpress,
  id_marquee_next,
  id_marquee_prev,
  id_spin_edit,
  id_alarm_close,
  id_alarm_park,
  id_set_mode,
  id_toggle_check,
  id_value_up,
  id_value_down,
  id_setup_menu,          // sent after the menu window is created to show the setup menu
  id_app_action_id_first,
  id_app_action_id_last = 1499
  };

// these will move to the hub code
#define VERT_MODE_MASK 0x0F00
#define HORZ_MODE_MASK 0x00F0
#define VERT_MODE_AVAILABLE 0x0004
#define HORZ_MODE_AVAILABLE 0x0002
#define AUTOPILOT_ENABLED 0x0001

// Horizontal navigation modes
#define HDG_MODE 0x0010
#define NAV_MODE 0x0020
#define APR_MODE 0x0030
#define REV_LEFT_MODE 0x0040
#define REV_RIGHT_MODE 0x0050

// Vertical navigation modes
#define ALT_MODE 0x0100
#define VERT_SEEK_MODE 0x0200
#define IAS_MODE 0x0300

#endif

