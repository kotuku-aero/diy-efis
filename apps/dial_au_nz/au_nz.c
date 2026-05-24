#include "au_nz_priv.h"
#include "../../src/proton/dialog_widgets.h"
#include "../../src/proton/instrument_widgets.h"
#include "../../src/mfdlib/menu_handlers.h"
// menu widget for this application
extern menu_widget_t au_nz__menu_widget;
// Forward declarations
static keys_t root;
static keys_t alarm_keys;
static keys_t popup_keys;
static keys_t edit_keys;
static menu_t settings_menu;
static menu_t config_menu;
static menu_t fuel_menu;
static menu_t ap_menu;
static menu_t ap_mode_menu;
static menu_item_menu_t root_key0 = {
.base.item_type = mi_menu,
.base.caption = "Settings",
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.evaluate = item_menu_evaluate,
.menu = &settings_menu,
};

static const menu_item_event_t root_decka_up = {
.can_id = id_marquee_next,
.base.item_type = mi_event,
.value.vt = v_int16,
.value.value.int16 = 1,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "Next",
};

static const menu_item_event_t root_decka_dn = {
.can_id = id_marquee_prev,
.base.item_type = mi_event,
.value.vt = v_int16,
.value.value.int16 = 1,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "Prev",
};

static const menu_item_event_t root_key1 = {
.can_id = id_cancel,
.base.item_type = mi_event,
.value.vt = v_int16,
.value.value.int16 = 1,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "Cancel",
};

static const menu_item_event_t root_deckb_up = {
.can_id = id_heading_change,
.base.item_type = mi_event,
.value.vt = v_int16,
.value.value.int16 = 1,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "Up",
};

static const menu_item_event_t root_deckb_dn = {
.can_id = id_heading_change,
.base.item_type = mi_event,
.value.vt = v_int16,
.value.value.int16 = -1,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "Up",
};

static const menu_item_event_t root_deckb_press_up = {
.can_id = id_qnh_change,
.base.item_type = mi_event,
.value.vt = v_int16,
.value.value.int16 = 1,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "Up",
};

static const menu_item_event_t root_deckb_press_dn = {
.can_id = id_qnh_change,
.base.item_type = mi_event,
.value.vt = v_int16,
.value.value.int16 = -1,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "Dn",
};

static const menu_item_event_t root_key2 = {
.can_id = id_toggle_vs_mode,
.base.item_type = mi_event,
.value.vt = v_int16,
.value.value.int16 = 1,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "Cancel",
};

static const menu_item_event_t root_hold_key2 = {
.can_id = id_ap_reverse,
.base.item_type = mi_event,
.value.vt = v_int16,
.value.value.int16 = 1,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "AP Reverse Left",
};

static menu_item_menu_t root_key3 = {
.base.item_type = mi_menu,
.base.caption = "AP Mode",
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.evaluate = item_menu_evaluate,
.menu = &ap_menu,
};

static const menu_item_event_t root_hold_key3 = {
.can_id = id_autopilot_enable,
.base.item_type = mi_event,
.value.vt = v_int16,
.value.value.int16 = 0,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "AP Cancel",
};

static const menu_item_event_t root_key4 = {
.can_id = id_toggle_hs_mode,
.base.item_type = mi_event,
.value.vt = v_int16,
.value.value.int16 = 1,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "VS Up",
};

static const menu_item_event_t root_hold_key4 = {
.can_id = id_ap_reverse,
.base.item_type = mi_event,
.value.vt = v_int16,
.value.value.int16 = -1,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "AP Reverse right",
};

static keys_t root = {
.key0 = (menu_item_t *) &root_key0,
.key0_caption = "Info/Settings",
.decka_up = (menu_item_t *) &root_decka_up,
.decka_dn = (menu_item_t *) &root_decka_dn,
.key1 = (menu_item_t *) &root_key1,
.key1_caption = "HDG/QNH",
.deckb_up = (menu_item_t *) &root_deckb_up,
.deckb_dn = (menu_item_t *) &root_deckb_dn,
.deckb_press_up = (menu_item_t *) &root_deckb_press_up,
.deckb_press_dn = (menu_item_t *) &root_deckb_press_dn,
.key2 = (menu_item_t *) &root_key2,
.key2_caption = "VS Mode",
.hold_key2 = (menu_item_t *) &root_hold_key2,
.key3 = (menu_item_t *) &root_key3,
.key3_caption = "AP Mode",
.hold_key3 = (menu_item_t *) &root_hold_key3,
.key4 = (menu_item_t *) &root_key4,
.key4_caption = "HS Mode",
.hold_key4 = (menu_item_t *) &root_hold_key4,
};

static const menu_item_event_t alarm_keys_key0 = {
.can_id = id_alarm_close,
.base.item_type = mi_event,
.value.vt = v_int16,
.value.value.int16 = 1,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "Close",
};

static const menu_item_event_t alarm_keys_key1 = {
.can_id = id_alarm_park,
.base.item_type = mi_event,
.value.vt = v_int16,
.value.value.int16 = 1,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "Park",
};

static keys_t alarm_keys = {
.key0 = (menu_item_t *) &alarm_keys_key0,
.key1 = (menu_item_t *) &alarm_keys_key1,
};

static const menu_item_event_t popup_keys_key0 = {
.can_id = id_ok,
.base.item_type = mi_event,
.value.vt = v_int16,
.value.value.int16 = 1,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "OK",
};

static const menu_item_event_t popup_keys_key1 = {
.can_id = id_cancel,
.base.item_type = mi_event,
.value.vt = v_int16,
.value.value.int16 = 1,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "Cancel",
};

static const menu_item_event_t popup_keys_decka_up = {
.can_id = id_up,
.base.item_type = mi_event,
.value.vt = v_int16,
.value.value.int16 = 1,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "Up",
};

static const menu_item_event_t popup_keys_decka_dn = {
.can_id = id_down,
.base.item_type = mi_event,
.value.vt = v_int16,
.value.value.int16 = 1,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "Dn",
};

static keys_t popup_keys = {
.key0 = (menu_item_t *) &popup_keys_key0,
.key1 = (menu_item_t *) &popup_keys_key1,
.decka_up = (menu_item_t *) &popup_keys_decka_up,
.decka_dn = (menu_item_t *) &popup_keys_decka_dn,
};

static const menu_item_event_t edit_keys_key0 = {
.can_id = id_ok,
.base.item_type = mi_event,
.value.vt = v_int16,
.value.value.int16 = 1,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "OK",
};

static const menu_item_event_t edit_keys_key2 = {
.can_id = id_cancel,
.base.item_type = mi_event,
.value.vt = v_int16,
.value.value.int16 = 1,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "Cancel",
};

static const menu_item_event_t edit_keys_decka_up = {
.can_id = id_spin_edit,
.base.item_type = mi_event,
.value.vt = v_int16,
.value.value.int16 = 10,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "Up",
};

static const menu_item_event_t edit_keys_decka_dn = {
.can_id = id_spin_edit,
.base.item_type = mi_event,
.value.vt = v_int16,
.value.value.int16 = -10,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "Dn",
};

static const menu_item_event_t edit_keys_decka_press_up = {
.can_id = id_spin_edit,
.base.item_type = mi_event,
.value.vt = v_int16,
.value.value.int16 = 1,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "Up",
};

static const menu_item_event_t edit_keys_decka_press_dn = {
.can_id = id_spin_edit,
.base.item_type = mi_event,
.value.vt = v_int16,
.value.value.int16 = -1,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "Dn",
};

static keys_t edit_keys = {
.key0 = (menu_item_t *) &edit_keys_key0,
.key0_caption = "OK",
.key2 = (menu_item_t *) &edit_keys_key2,
.key2_caption = "Cancel",
.decka_up = (menu_item_t *) &edit_keys_decka_up,
.decka_dn = (menu_item_t *) &edit_keys_decka_dn,
.decka_press_up = (menu_item_t *) &edit_keys_decka_press_up,
.decka_press_dn = (menu_item_t *) &edit_keys_decka_press_dn,
};

// Menus
// Menu definitions
static menu_item_menu_t settings_menu_item_1 = {
.base.item_type = mi_menu,
.base.caption = "Config",
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.evaluate = item_menu_evaluate,
.menu = &config_menu,
};

static menu_item_menu_t settings_menu_item_2 = {
.base.item_type = mi_menu,
.base.caption = "Fuel",
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.evaluate = item_menu_evaluate,
.menu = &fuel_menu,
};


static menu_item_t *settings_menu_items[] = {
(menu_item_t *) &settings_menu_item_1,
(menu_item_t *) &settings_menu_item_2,
};

static menu_t settings_menu = {
.menu_items = settings_menu_items,
.num_items = 2,
.name = "settings_menu",
.caption = "Settings",
.keys = &popup_keys,
};

// Menu definitions
static menu_item_spin_edit_t config_menu_item_1 = {
.base.item_type = mi_edit,
.base.caption = "Brightness",
.base.event = item_spin_edit_event,
.base.paint = default_paint_handler,
.base.edit_paint = item_spin_edit_paint,
.base.is_enabled = default_enable_handler,
.base.evaluate = item_spin_edit_evaluate,
.get_value = get_brightness,
.set_value = set_brightness,
.keys = &edit_keys,
.format = "%0.0f",
.min_value = 10,
.max_value = 100,
};


static menu_item_t *config_menu_items[] = {
(menu_item_t *) &config_menu_item_1,
};

static menu_t config_menu = {
.menu_items = config_menu_items,
.num_items = 1,
.name = "config_menu",
.caption = "Config",
.keys = &popup_keys,
};

// Menu definitions
static const menu_item_event_t fuel_menu_item_1 = {
.can_id = id_set_fuel_available,
.base.item_type = mi_event,
.get_value = get_fuel_max,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "Full Fuel",
};

static menu_item_spin_edit_t fuel_menu_item_2 = {
.base.item_type = mi_edit,
.base.caption = "Fuel total",
.base.event = item_spin_edit_event,
.base.paint = default_paint_handler,
.base.edit_paint = item_spin_edit_paint,
.base.is_enabled = default_enable_handler,
.base.evaluate = item_spin_edit_evaluate,
.get_value = get_fuel_total,
.set_value = set_fuel_total,
.keys = &edit_keys,
.converter = &convert_cc_to_litres,
.min_value = 0,
.get_max_value = get_fuel_max,
};


static menu_item_t *fuel_menu_items[] = {
(menu_item_t *) &fuel_menu_item_1,
(menu_item_t *) &fuel_menu_item_2,
};

static menu_t fuel_menu = {
.menu_items = fuel_menu_items,
.num_items = 2,
.name = "fuel_menu",
.caption = "Fuel",
.keys = &popup_keys,
};

// Menu definitions
static menu_item_checklist_t ap_menu_item_1 = {
.base.item_type = mi_checklist,
.base.caption = "Mode",
.base.event = item_checklist_event,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.evaluate = item_checklist_evaluate,
.get_selected = get_autopilot_mode,
.popup = &ap_mode_menu,
};


static menu_item_t *ap_menu_items[] = {
(menu_item_t *) &ap_menu_item_1,
};

static menu_t ap_menu = {
.menu_items = ap_menu_items,
.num_items = 1,
.name = "ap_menu",
.caption = "AP",
.keys = &popup_keys,
};

// Menu definitions
static const menu_item_event_t ap_mode_menu_item_1 = {
.can_id = id_autopilot_enable,
.base.item_type = mi_event,
.value.vt = v_uint16,
.value.value.uint16 = 0x0000,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "None",
};

static const menu_item_event_t ap_mode_menu_item_2 = {
.can_id = id_autopilot_enable,
.base.item_type = mi_event,
.value.vt = v_uint16,
.value.value.uint16 = 0x0003,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "ALT",
};

static const menu_item_event_t ap_mode_menu_item_3 = {
.can_id = id_autopilot_enable,
.base.item_type = mi_event,
.value.vt = v_uint16,
.value.value.uint16 = 0x0005,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "NAV",
};

static const menu_item_event_t ap_mode_menu_item_4 = {
.can_id = id_autopilot_enable,
.base.item_type = mi_event,
.value.vt = v_uint16,
.value.value.uint16 = 0x0007,
.base.evaluate = item_event_evaluate,
.base.event = default_msg_handler,
.base.paint = default_paint_handler,
.base.is_enabled = default_enable_handler,
.base.caption = "ALT+NAV",
};


static menu_item_t *ap_mode_menu_items[] = {
(menu_item_t *) &ap_mode_menu_item_1,
(menu_item_t *) &ap_mode_menu_item_2,
(menu_item_t *) &ap_mode_menu_item_3,
(menu_item_t *) &ap_mode_menu_item_4,
};

static menu_t ap_mode_menu = {
.menu_items = ap_mode_menu_items,
.num_items = 4,
.name = "ap_mode_menu",
.caption = "AP Mode",
.keys = &popup_keys,
};

// Menu references
static menu_t *menus[] = {
&settings_menu,
&config_menu,
&fuel_menu,
&ap_menu,
&ap_mode_menu,
};

static menu_item_t *menu_items[] = {
(menu_item_t *) &settings_menu_item_1,
(menu_item_t *) &settings_menu_item_2,
(menu_item_t *) &config_menu_item_1,
(menu_item_t *) &fuel_menu_item_1,
(menu_item_t *) &fuel_menu_item_2,
(menu_item_t *) &ap_menu_item_1,
(menu_item_t *) &ap_mode_menu_item_1,
(menu_item_t *) &ap_mode_menu_item_2,
(menu_item_t *) &ap_mode_menu_item_3,
(menu_item_t *) &ap_mode_menu_item_4,
};

// Menu Captions 
static button_t menu_widget_captions[] = {
{
.key = kn_decka,
.foreground_color = color_white,
.background_color = color_black,
.selected_color = color_black,
.highlight_color = color_lightblue,
},
{
.key = kn_key2,
.foreground_color = color_white,
.background_color = color_black,
.selected_color = color_black,
.highlight_color = color_lightblue,
},
{
.key = kn_key3,
.foreground_color = color_white,
.background_color = color_black,
.selected_color = color_black,
.highlight_color = color_lightblue,
},
{
.key = kn_key4,
.foreground_color = color_white,
.background_color = color_black,
.selected_color = color_black,
.highlight_color = color_lightblue,
},
{
.key = kn_key5,
.foreground_color = color_white,
.background_color = color_black,
.selected_color = color_black,
.highlight_color = color_lightblue,
},
{
.key = kn_key6,
.foreground_color = color_white,
.background_color = color_black,
.selected_color = color_black,
.highlight_color = color_lightblue,
},
{
.key = kn_deckb,
.foreground_color = color_white,
.background_color = color_black,
.selected_color = color_black,
.highlight_color = color_lightblue,
},
};

menu_widget_t au_nz_menu_widget = {
.base.style = DS_ENABLED | BORDER_NONE,
.base.background_color = color_black,
.base.border_color = color_yellow,
.base.on_paint = menu_widget_on_paint,
.base.alarm_color = color_red,
.base.rect.left = 220,
.base.rect.top = 340,
.base.rect.right = 584 + 220,
.base.rect.bottom = 240 + 340,
.num_menu_items = numelements(menu_items),
.menu_items = menu_items,
.num_menus = numelements(menus),
.menus = menus,
.menu_rect.x = 0,
.menu_rect.y = 0,
.menu_start.x = 0,
.menu_start.y = 240,
.selected_background_color = color_white,
.text_color = color_green,
.selected_color = color_magenta,
.menu_border_color = color_yellow,
.menu_timeout = 7,
.font = &neo_9_font,
.root_keys = &root,
.alarm_keys = &alarm_keys,
.key_captions = menu_widget_captions,
.num_key_captions = numelements(menu_widget_captions),
};

// container control constructor forward declarations
static const step_t asi_widget_steps[] = {
{
.value = 40,
.pen = color_white,
.color = color_white,
},
{
.value = 240,
.pen = color_white,
.color = color_white,
},
};

static const tick_mark_t asi_widget_ticks[] = {
{
.text = "40",
.value = 40,
.length = 7,
},
{
.value = 50,
.length = 5,
},
{
.text = "60",
.value = 60,
.length = 7,
},
{
.value = 70,
.length = 5,
},
{
.text = "80",
.value = 80,
.length = 7,
},
{
.value = 90,
.length = 5,
},
{
.text = "100",
.value = 100,
.length = 7,
},
{
.value = 110,
.length = 5,
},
{
.text = "120",
.value = 120,
.length = 7,
},
{
.value = 130,
.length = 5,
},
{
.text = "140",
.value = 140,
.length = 7,
},
{
.value = 150,
.length = 5,
},
{
.text = "160",
.value = 160,
.length = 7,
},
{
.value = 170,
.length = 5,
},
{
.text = "180",
.value = 180,
.length = 7,
},
{
.value = 190,
.length = 5,
},
{
.text = "200",
.value = 200,
.length = 7,
},
{
.value = 210,
.length = 5,
},
{
.text = "220",
.value = 220,
.length = 7,
},
{
.value = 230,
.length = 5,
},
{
.text = "240",
.value = 240,
.length = 7,
},
};

static gauge_widget_t asi_widget = {
.base.style = FILL_BACKGROUND |BORDER_NONE,
.base.background_color = color_black,
.base.name_color = color_white,
.base.name = "ASI",
.base.name_font = &neo_9_bold_font,
.base.border_color = color_white,
.base.on_paint_background = on_paint_airspeed_gauge_background,
.base.alarm_color = color_red,
.base.rect.left = 0,
.base.rect.top = 0,
.base.rect.right = 320 + 0,
.base.rect.bottom = 300 + 0,
.style = gs_pointer,
.font = &neo_9_bold_font,
.can_id = id_indicated_airspeed,
.gauge_radii = 120,
.arc_width = 3,
.arc_begin = 310,
.arc_range = 280,
.bar_width = 5,
.draw_value = true,
.value_font = &neo_18_font,
.converter = &convert_ms_to_kts,
.center.x = 160,
.center.y = 150,
.value_rect.left = 240,
.value_rect.top = 0,
.value_rect.right = 80 + 240,
.value_rect.bottom = 60 + 0,
.steps = asi_widget_steps,
.num_steps = numelements(asi_widget_steps),
.ticks = asi_widget_ticks,
.num_ticks = numelements(asi_widget_ticks),
};

static attitude_widget_t att_widget = {
.base.style = BORDER_BOTTOM | BORDER_NONE,
.base.name = "ATT",
.base.name_font = &neo_9_bold_font,
.base.z_order = 0,
.base.border_color = color_white,
.base.alarm_color = color_red,
.base.rect.left = 328,
.base.rect.top = 8,
.base.rect.right = 284 + 328,
.base.rect.bottom = 284 + 8,
.median.x = 142,
.median.y = 142,
.skid_indicator.left = 42,
.skid_indicator.top = 264,
.skid_indicator.right = 200 + 42,
.skid_indicator.bottom = 20 + 264,
};

static const step_t alt_widget_steps[] = {
{
.value = 0,
.pen = color_white,
.color = color_white,
},
{
.value = 1000,
.pen = color_white,
.color = color_white,
},
};

static const tick_mark_t alt_widget_ticks[] = {
{
.text = "0",
.value = 0,
.length = 7,
},
{
.value = 20,
.length = 5,
},
{
.value = 40,
.length = 5,
},
{
.value = 60,
.length = 5,
},
{
.value = 80,
.length = 5,
},
{
.text = "1",
.value = 100,
.length = 7,
},
{
.value = 120,
.length = 5,
},
{
.value = 140,
.length = 5,
},
{
.value = 160,
.length = 5,
},
{
.value = 180,
.length = 5,
},
{
.text = "2",
.value = 200,
.length = 7,
},
{
.value = 220,
.length = 5,
},
{
.value = 240,
.length = 5,
},
{
.value = 260,
.length = 5,
},
{
.value = 280,
.length = 5,
},
{
.text = "3",
.value = 300,
.length = 7,
},
{
.value = 320,
.length = 5,
},
{
.value = 340,
.length = 5,
},
{
.value = 360,
.length = 5,
},
{
.value = 380,
.length = 5,
},
{
.text = "4",
.value = 400,
.length = 7,
},
{
.value = 420,
.length = 5,
},
{
.value = 440,
.length = 5,
},
{
.value = 460,
.length = 5,
},
{
.value = 480,
.length = 5,
},
{
.text = "5",
.value = 500,
.length = 7,
},
{
.value = 520,
.length = 5,
},
{
.value = 540,
.length = 5,
},
{
.value = 560,
.length = 5,
},
{
.value = 580,
.length = 5,
},
{
.text = "6",
.value = 600,
.length = 7,
},
{
.value = 620,
.length = 5,
},
{
.value = 640,
.length = 5,
},
{
.value = 660,
.length = 5,
},
{
.value = 680,
.length = 5,
},
{
.text = "7",
.value = 700,
.length = 7,
},
{
.value = 720,
.length = 5,
},
{
.value = 740,
.length = 5,
},
{
.value = 760,
.length = 5,
},
{
.value = 780,
.length = 5,
},
{
.text = "7",
.value = 700,
.length = 7,
},
{
.value = 820,
.length = 5,
},
{
.value = 840,
.length = 5,
},
{
.value = 860,
.length = 5,
},
{
.value = 880,
.length = 5,
},
{
.text = "8",
.value = 800,
.length = 7,
},
{
.value = 920,
.length = 5,
},
{
.value = 940,
.length = 5,
},
{
.value = 960,
.length = 5,
},
{
.value = 980,
.length = 5,
},
{
.text = "9",
.value = 900,
.length = 7,
},
{
.value = 920,
.length = 5,
},
{
.value = 940,
.length = 5,
},
{
.value = 960,
.length = 5,
},
{
.value = 980,
.length = 5,
},
};

static gauge_widget_t alt_widget = {
.base.style = FILL_BACKGROUND |BORDER_NONE,
.base.background_color = color_black,
.base.name_color = color_white,
.base.name = "ALT",
.base.name_font = &neo_9_bold_font,
.base.border_color = color_white,
.base.alarm_color = color_red,
.base.rect.left = 620,
.base.rect.top = 0,
.base.rect.right = 300 + 620,
.base.rect.bottom = 300 + 0,
.style = gs_pointer,
.font = &neo_9_bold_font,
.can_id = id_baro_corrected_altitude,
.gauge_radii = 120,
.arc_width = 3,
.arc_begin = 270,
.arc_range = 360,
.bar_width = 5,
.draw_value = true,
.value_font = &neo_18_font,
.converter = &convert_meters_to_ft,
.incremental = true,
.center.x = 150,
.center.y = 150,
.value_rect.left = 0,
.value_rect.top = 0,
.value_rect.right = 80 + 0,
.value_rect.bottom = 60 + 0,
.steps = alt_widget_steps,
.num_steps = numelements(alt_widget_steps),
.ticks = alt_widget_ticks,
.num_ticks = numelements(alt_widget_ticks),
};

static const step_t vsi_widget_steps[] = {
{
.value = -3000,
.pen = color_white,
.color = color_white,
},
{
.value = 3000,
.pen = color_white,
.color = color_white,
},
};

static const tick_mark_t vsi_widget_ticks[] = {
{
.text = "3",
.value = -3000,
.length = 7,
},
{
.value = -2800,
.length = 5,
},
{
.value = -2600,
.length = 5,
},
{
.value = -2400,
.length = 5,
},
{
.value = -2200,
.length = 5,
},
{
.text = "2",
.value = -2000,
.length = 7,
},
{
.value = -1800,
.length = 5,
},
{
.value = -1600,
.length = 5,
},
{
.value = -1400,
.length = 5,
},
{
.value = -1200,
.length = 5,
},
{
.text = "1",
.value = -1000,
.length = 7,
},
{
.value = -800,
.length = 5,
},
{
.value = -600,
.length = 5,
},
{
.value = -400,
.length = 5,
},
{
.value = -200,
.length = 5,
},
{
.text = "0",
.value = 0,
.length = 7,
},
{
.value = 200,
.length = 5,
},
{
.value = 400,
.length = 5,
},
{
.value = 600,
.length = 5,
},
{
.value = 800,
.length = 5,
},
{
.text = "1",
.value = 1000,
.length = 7,
},
{
.value = 1200,
.length = 5,
},
{
.value = 1400,
.length = 5,
},
{
.value = 1600,
.length = 5,
},
{
.value = 1800,
.length = 5,
},
{
.text = "2",
.value = 2000,
.length = 7,
},
{
.value = 2200,
.length = 5,
},
{
.value = 2400,
.length = 5,
},
{
.value = 2600,
.length = 5,
},
{
.value = 2800,
.length = 5,
},
{
.text = "3",
.value = 3000,
.length = 7,
},
};

static gauge_widget_t vsi_widget = {
.base.style = FILL_BACKGROUND |BORDER_NONE,
.base.background_color = color_black,
.base.name_color = color_white,
.base.name = "VSI",
.base.name_font = &neo_9_bold_font,
.base.border_color = color_white,
.base.alarm_color = color_red,
.base.rect.left = 620,
.base.rect.top = 300,
.base.rect.right = 300 + 620,
.base.rect.bottom = 280 + 300,
.style = gs_pointer,
.font = &neo_9_bold_font,
.can_id = id_altitude_rate,
.gauge_radii = 120,
.arc_width = 3,
.arc_begin = 40,
.arc_range = 280,
.bar_width = 5,
.draw_value = true,
.value_font = &neo_18_font,
.converter = &convert_ms_to_fpm,
.center.x = 140,
.center.y = 140,
.value_rect.left = 0,
.value_rect.top = 0,
.value_rect.right = 80 + 0,
.value_rect.bottom = 60 + 0,
.steps = vsi_widget_steps,
.num_steps = numelements(vsi_widget_steps),
.ticks = vsi_widget_ticks,
.num_ticks = numelements(vsi_widget_ticks),
};

static hsi_widget_t hsi_widget = {
.base.style = BORDER_LEFT | FILL_BACKGROUND |BORDER_NONE,
.base.background_color = color_black,
.base.name = "HSI",
.base.name_font = &neo_9_bold_font,
.base.border_color = color_white,
.base.on_create = on_create_hsi_widget,
.base.alarm_color = color_red,
.base.rect.left = 320,
.base.rect.top = 300,
.base.rect.right = 300 + 320,
.base.rect.bottom = 280 + 300,
};

static const step_t map_gauge_steps[] = {
{
.value = 0,
.pen = color_white,
.color = color_hollow,
},
{
.value = 35,
.pen = color_white,
.color = color_green,
},
};

static const tick_mark_t map_gauge_ticks[] = {
{
.text = "10",
.value = 10,
},
{
.value = 15,
},
{
.text = "20",
.value = 20,
},
{
.value = 25,
},
{
.text = "30",
.value = 30,
},
};

static gauge_widget_t map_gauge = {
.base.style = BORDER_RIGHT | BORDER_TOP | BORDER_BOTTOM | DRAW_NAME | FILL_BACKGROUND |BORDER_NONE,
.base.background_color = color_black,
.base.name_color = color_white,
.base.name = "MAP",
.base.name_font = &neo_9_bold_font,
.base.border_color = color_white,
.base.sensor_id = id_manifold_pressure_sensor_status,
.base.sensor_failed = true,
.base.alarm_id = id_map_divergence_alarm,
.base.alarm_color = color_red,
.base.rect.left = 0,
.base.rect.top = 300,
.base.rect.right = 112 + 0,
.base.rect.bottom = 68 + 300,
.style = gs_point,
.font = &neo_9_bold_font,
.can_id = id_manifold_pressure,
.gauge_radii = 75,
.arc_width = 5,
.bar_width = 5,
.arc_begin = 230,
.arc_range = 80,
.draw_value = true,
.value_font = &neo_18_font,
.converter = &convert_hpa_to_inhg,
.base.name_pt.x = 20,
.base.name_pt.y = 8,
.center.x = 56,
.center.y = 90,
.value_rect.left = 15,
.value_rect.top = 40,
.value_rect.right = 80 + 15,
.value_rect.bottom = 26 + 40,
.steps = map_gauge_steps,
.num_steps = numelements(map_gauge_steps),
.ticks = map_gauge_ticks,
.num_ticks = numelements(map_gauge_ticks),
};

static const step_t rpm_gauge_steps[] = {
{
.value = 500,
.pen = color_white,
.color = color_red,
},
{
.value = 2750,
.pen = color_white,
.color = color_green,
},
{
.value = 3100,
.pen = color_white,
.color = color_green,
},
};

static const tick_mark_t rpm_gauge_ticks[] = {
{
.value = 500,
},
{
.value = 750,
},
{
.text = "1",
.value = 1000,
},
{
.value = 1250,
},
{
.value = 1500,
},
{
.value = 1750,
},
{
.text = "2",
.value = 2000,
},
{
.value = 2250,
},
{
.value = 2500,
},
{
.value = 2750,
},
{
.text = "3",
.value = 3000,
},
};

static gauge_widget_t rpm_gauge = {
.base.style = BORDER_RIGHT | BORDER_TOP | BORDER_BOTTOM | DRAW_NAME | FILL_BACKGROUND |BORDER_NONE,
.base.background_color = color_black,
.base.name_color = color_white,
.base.name = "RPM",
.base.name_font = &neo_9_bold_font,
.base.border_color = color_white,
.base.alarm_id = id_rpm_high_alarm,
.base.alarm_color = color_red,
.base.rect.left = 112,
.base.rect.top = 300,
.base.rect.right = 112 + 112,
.base.rect.bottom = 68 + 300,
.style = gs_point,
.font = &neo_9_bold_font,
.can_id = id_engine_rpm,
.gauge_radii = 75,
.arc_width = 5,
.bar_width = 5,
.arc_begin = 230,
.arc_range = 80,
.draw_value = true,
.value_font = &neo_18_font,
.base.name_pt.x = 20,
.base.name_pt.y = 8,
.center.x = 56,
.center.y = 90,
.value_rect.left = 15,
.value_rect.top = 40,
.value_rect.right = 80 + 15,
.value_rect.bottom = 26 + 40,
.steps = rpm_gauge_steps,
.num_steps = numelements(rpm_gauge_steps),
.ticks = rpm_gauge_ticks,
.num_ticks = numelements(rpm_gauge_ticks),
};

static hp_annunciator_t hp_annunciator = {
.base.base.base.style = BORDER_RIGHT | BORDER_BOTTOM | DRAW_NAME | FILL_BACKGROUND |BORDER_NONE,
.base.base.base.background_color = color_blue,
.base.base.base.name_color = color_white,
.base.base.base.name = "HP",
.base.base.base.name_font = &neo_9_bold_font,
.base.base.base.border_color = color_white,
.base.base.base.alarm_color = color_red,
.base.base.base.rect.left = 0,
.base.base.base.rect.top = 368,
.base.base.base.rect.right = 60 + 0,
.base.base.base.rect.bottom = 20 + 368,
.base.can_id = id_engine_hp,
.base.small_font = &neo_9_bold_font,
.base.text_color = color_white,
.base.text_offset = 30,
.base.label_color = color_white,
.base.base.base.on_message = on_hp_msg,
.base.base.base.on_paint = on_paint_hp,
};

static ecu_annunciator_t kmag_annunciator = {
.base.base.style = BORDER_RIGHT | BORDER_BOTTOM | FILL_BACKGROUND |BORDER_NONE,
.base.base.background_color = color_black,
.base.base.name_color = color_white,
.base.base.name = "kMAG",
.base.base.name_font = &neo_9_font,
.base.base.border_color = color_white,
.base.base.alarm_color = color_red,
.base.base.rect.left = 60,
.base.base.rect.top = 368,
.base.base.rect.right = 104 + 60,
.base.base.rect.bottom = 20 + 368,
.base.base.on_message = on_kmag_msg,
.base.base.on_paint = on_paint_kmag,
};

static auto_annunciator_t oat_annunciator = {
.base.base.base.style = BORDER_RIGHT | BORDER_BOTTOM | DRAW_NAME | FILL_BACKGROUND |BORDER_NONE,
.base.base.base.background_color = color_blue,
.base.base.base.name = "OAT",
.base.base.base.name_font = &neo_9_bold_font,
.base.base.base.border_color = color_white,
.base.base.base.on_paint = on_paint_auto,
.base.base.base.on_message = on_auto_msg,
.base.base.base.alarm_color = color_red,
.base.base.base.rect.left = 164,
.base.base.base.rect.top = 368,
.base.base.base.rect.right = 60 + 164,
.base.base.base.rect.bottom = 20 + 368,
.base.can_id = id_outside_air_temperature,
.base.small_font = &neo_9_bold_font,
.base.text_color = color_white,
.base.text_offset = 30,
.base.label_color = color_white,
.converter = &convert_kelvin_to_c,
.value_type = v_uint16,
.fmt = "%d",
.base.base.base.on_message = on_auto_msg,
.base.base.base.on_paint = on_paint_auto,
};

static const step_t oilt_gauge_steps[] = {
{
.value = 32,
.pen = color_hollow,
.color = color_hollow,
},
{
.value = 90,
.pen = color_scarlet,
.color = color_scarlet,
},
{
.value = 220,
.pen = color_lightgreen,
.color = color_lightgreen,
},
{
.value = 250,
.pen = color_scarlet,
.color = color_scarlet,
},
};

static gauge_widget_t oilt_gauge = {
.base.style = BORDER_TOP | DRAW_NAME | FILL_BACKGROUND |BORDER_NONE,
.base.background_color = color_black,
.base.name_color = color_white,
.base.name = "OILT",
.base.name_font = &neo_9_bold_font,
.base.border_color = color_white,
.base.sensor_id = id_oil_temperature_sensor_status,
.base.sensor_failed = true,
.base.alarm_id = id_high_oil_temperature_alarm,
.base.alarm_color =  color_red,
.base.rect.left = 224,
.base.rect.top = 300,
.base.rect.right = 96 + 224,
.base.rect.bottom = 40 + 300,
.style = gs_hbar,
.font = &neo_12_font,
.can_id = id_oil_temperature,
.draw_value = true,
.value_font = &neo_12_bold_font,
.converter = &convert_kelvin_to_f,
.base.name_pt.x = 55,
.base.name_pt.y = 30,
.value_rect.left = 3,
.value_rect.top = 20,
.value_rect.right = 17 + 3,
.value_rect.bottom = 19 + 20,
.steps = oilt_gauge_steps,
.num_steps = numelements(oilt_gauge_steps),
};

static const step_t oilp_gauge_steps[] = {
{
.value = 0,
.pen = color_red,
.color = color_red,
},
{
.value = 20,
.pen = color_red,
.color = color_red,
},
{
.value = 120,
.pen = color_lightgreen,
.color = color_lightgreen,
},
};

static gauge_widget_t oilp_gauge = {
.base.style = BORDER_BOTTOM | DRAW_NAME | FILL_BACKGROUND |BORDER_NONE,
.base.background_color = color_black,
.base.name_color = color_white,
.base.name = "OILP",
.base.name_font = &neo_9_bold_font,
.base.border_color = color_white,
.base.sensor_id = id_oil_pressure_sensor_status,
.base.sensor_failed = true,
.base.alarm_id = id_low_oil_pressure_alarm,
.base.alarm_color =  color_red,
.base.rect.left = 224,
.base.rect.top = 340,
.base.rect.right = 96 + 224,
.base.rect.bottom = 40 + 340,
.style = gs_hbar,
.font = &neo_12_font,
.can_id = id_oil_pressure,
.draw_value = true,
.value_font = &neo_12_bold_font,
.converter = &convert_hpa_to_psi,
.base.name_pt.x = 55,
.base.name_pt.y = 30,
.value_rect.left = 3,
.value_rect.top = 20,
.value_rect.right = 17 + 3,
.value_rect.bottom = 19 + 20,
.steps = oilp_gauge_steps,
.num_steps = numelements(oilp_gauge_steps),
};

static const step_t fuelp_gauge_steps[] = {
{
.value = 0,
.pen = color_hollow,
.color = color_hollow,
},
{
.value = 15,
.pen = color_red,
.color = color_red,
},
{
.value = 45,
.pen = color_lightgreen,
.color = color_lightgreen,
},
};

static gauge_widget_t fuelp_gauge = {
.base.style = DRAW_NAME | FILL_BACKGROUND |BORDER_NONE,
.base.background_color = color_black,
.base.name_color = color_white,
.base.name = "FUELP",
.base.name_font = &neo_9_bold_font,
.base.border_color = color_white,
.base.sensor_id = id_fuel_pressure_sensor_status,
.base.sensor_failed = true,
.base.alarm_id = id_low_fuel_pressure_alarm,
.base.alarm_color =  color_red,
.base.rect.left = 224,
.base.rect.top = 380,
.base.rect.right = 96 + 224,
.base.rect.bottom = 40 + 380,
.style = gs_hbar,
.font = &neo_12_font,
.can_id = id_fuel_pressure,
.draw_value = true,
.value_font = &neo_12_bold_font,
.converter = &convert_hpa_to_psi,
.base.name_pt.x = 55,
.base.name_pt.y = 30,
.value_rect.left = 3,
.value_rect.top = 20,
.value_rect.right = 17 + 3,
.value_rect.bottom = 19 + 20,
.steps = fuelp_gauge_steps,
.num_steps = numelements(fuelp_gauge_steps),
};

static const step_t fuelf_gauge_steps[] = {
{
.value = 0,
.pen = color_hollow,
.color = color_hollow,
},
{
.value = 5,
.pen = color_red,
.color = color_red,
},
{
.value = 60,
.pen = color_lightgreen,
.color = color_lightgreen,
},
};

static gauge_widget_t fuelf_gauge = {
.base.style = BORDER_BOTTOM | DRAW_NAME | FILL_BACKGROUND |BORDER_NONE,
.base.background_color = color_black,
.base.name_color = color_white,
.base.name = "FUELF",
.base.name_font = &neo_9_bold_font,
.base.border_color = color_white,
.base.sensor_id = id_fuel_flow_sensor_status,
.base.sensor_failed = true,
.base.alarm_color = color_red,
.base.rect.left = 224,
.base.rect.top = 420,
.base.rect.right = 96 + 224,
.base.rect.bottom = 40 + 420,
.style = gs_hbar,
.font = &neo_12_font,
.can_id = id_fuel_flow_rate,
.draw_value = true,
.value_font = &neo_12_bold_font,
.converter = &convert_flow_to_litres,
.base.name_pt.x = 55,
.base.name_pt.y = 30,
.value_rect.left = 3,
.value_rect.top = 20,
.value_rect.right = 17 + 3,
.value_rect.bottom = 19 + 20,
.steps = fuelf_gauge_steps,
.num_steps = numelements(fuelf_gauge_steps),
};

static const step_t left_fuel_gauge_steps[] = {
{
.value = 0,
.pen = color_hollow,
.color = color_hollow,
},
{
.value = 5,
.pen = color_red,
.color = color_red,
},
{
.value = 55,
.pen = color_lightgreen,
.color = color_lightgreen,
},
};

static gauge_widget_t left_fuel_gauge = {
.base.style = DRAW_NAME | FILL_BACKGROUND |BORDER_NONE,
.base.background_color = color_black,
.base.name_color = color_white,
.base.name = "LEFT",
.base.name_font = &neo_9_bold_font,
.base.border_color = color_white,
.base.sensor_id = id_left_fuel_quantity_sensor_status,
.base.sensor_failed = true,
.base.alarm_color = color_red,
.base.rect.left = 224,
.base.rect.top = 460,
.base.rect.right = 96 + 224,
.base.rect.bottom = 40 + 460,
.style = gs_hbar,
.font = &neo_12_font,
.can_id = id_left_fuel_quantity,
.draw_value = true,
.value_font = &neo_12_bold_font,
.converter = &convert_cc_to_litres,
.base.name_pt.x = 55,
.base.name_pt.y = 30,
.value_rect.left = 3,
.value_rect.top = 20,
.value_rect.right = 17 + 3,
.value_rect.bottom = 19 + 20,
.steps = left_fuel_gauge_steps,
.num_steps = numelements(left_fuel_gauge_steps),
};

static const step_t right_fuel_gauge_steps[] = {
{
.value = 0,
.pen = color_hollow,
.color = color_hollow,
},
{
.value = 5,
.pen = color_red,
.color = color_red,
},
{
.value = 55,
.pen = color_lightgreen,
.color = color_lightgreen,
},
};

static gauge_widget_t right_fuel_gauge = {
.base.style = DRAW_NAME | FILL_BACKGROUND |BORDER_NONE,
.base.background_color = color_black,
.base.name_color = color_white,
.base.name = "RIGHT",
.base.name_font = &neo_9_bold_font,
.base.border_color = color_white,
.base.sensor_id = id_right_fuel_quantity_sensor_status,
.base.sensor_failed = true,
.base.alarm_color = color_red,
.base.rect.left = 224,
.base.rect.top = 500,
.base.rect.right = 96 + 224,
.base.rect.bottom = 40 + 500,
.style = gs_hbar,
.font = &neo_12_font,
.can_id = id_right_fuel_quantity,
.draw_value = true,
.value_font = &neo_12_bold_font,
.converter = &convert_cc_to_litres,
.base.name_pt.x = 55,
.base.name_pt.y = 30,
.value_rect.left = 3,
.value_rect.top = 20,
.value_rect.right = 17 + 3,
.value_rect.bottom = 19 + 20,
.steps = right_fuel_gauge_steps,
.num_steps = numelements(right_fuel_gauge_steps),
};

static const step_t fuelt_gauge_steps[] = {
{
.value = 0,
.pen = color_hollow,
.color = color_hollow,
},
{
.value = 5000,
.pen = color_red,
.color = color_red,
},
{
.value = 110000,
.pen = color_lightgreen,
.color = color_lightgreen,
},
};

static gauge_widget_t fuelt_gauge = {
.base.style = DRAW_NAME | FILL_BACKGROUND |BORDER_NONE,
.base.background_color = color_black,
.base.name_color = color_white,
.base.name = "FUELT",
.base.name_font = &neo_9_bold_font,
.base.border_color = color_white,
.base.sensor_id = id_right_fuel_quantity_sensor_status,
.base.sensor_failed = true,
.base.alarm_color = color_red,
.base.rect.left = 224,
.base.rect.top = 540,
.base.rect.right = 96 + 224,
.base.rect.bottom = 40 + 540,
.style = gs_hbar,
.font = &neo_12_bold_font,
.can_id = id_fuel_total,
.draw_value = true,
.value_font = &neo_12_bold_font,
.converter = &convert_cc_to_litres,
.base.name_pt.x = 55,
.base.name_pt.y = 30,
.value_rect.left = 3,
.value_rect.top = 20,
.value_rect.right = 17 + 3,
.value_rect.bottom = 19 + 20,
.steps = fuelt_gauge_steps,
.num_steps = numelements(fuelt_gauge_steps),
};

static datetime_annunciator_t utc_annunciator = {
.base.base.base.style = BORDER_TOP | DRAW_NAME | BORDER_NONE,
.base.base.base.background_color = color_blue,
.base.base.base.name_color = color_white,
.base.base.base.name = "UTC",
.base.base.base.name_font = &neo_9_font,
.base.base.base.border_color = color_white,
.base.base.base.alarm_color = color_red,
.base.base.base.rect.left = 920,
.base.base.base.rect.top = 0,
.base.base.base.rect.right = 104 + 920,
.base.base.base.rect.bottom = 40 + 0,
.base.can_id = id_datetime_utc,
.base.small_font = &neo_9_font,
.base.compact = true,
.base.text_color = color_purple,
.base.text_offset = 30,
.base.label_color = color_white,
.format = "%H:%M",
.base.base.base.on_message = on_datetime_msg,
.base.base.base.on_paint = on_paint_datetime,
};

static datetime_annunciator_t utc_date_annunciator = {
.base.base.base.style = BORDER_TOP | DRAW_NAME | BORDER_NONE,
.base.base.base.background_color = color_blue,
.base.base.base.name_color = color_white,
.base.base.base.name = "UTC DATE",
.base.base.base.name_font = &neo_9_font,
.base.base.base.border_color = color_white,
.base.base.base.alarm_color = color_red,
.base.base.base.rect.left = 920,
.base.base.base.rect.top = 40,
.base.base.base.rect.right = 104 + 920,
.base.base.base.rect.bottom = 40 + 40,
.base.can_id = id_datetime_utc,
.base.small_font = &neo_9_font,
.base.compact = true,
.base.text_color = color_purple,
.base.text_offset = 30,
.base.label_color = color_white,
.format = "%y-%m-%d",
.base.base.base.on_message = on_datetime_msg,
.base.base.base.on_paint = on_paint_datetime,
};

static hobbs_annunciator_t air_annunciator = {
.base.base.base.style = BORDER_TOP | DRAW_NAME | BORDER_NONE,
.base.base.base.background_color = color_blue,
.base.base.base.name_color = color_white,
.base.base.base.name = "AIR",
.base.base.base.name_font = &neo_9_font,
.base.base.base.border_color = color_white,
.base.base.base.alarm_color = color_red,
.base.base.base.rect.left = 920,
.base.base.base.rect.top = 80,
.base.base.base.rect.right = 104 + 920,
.base.base.base.rect.bottom = 40 + 80,
.base.can_id = id_engine_hours,
.base.small_font = &neo_9_font,
.base.compact = true,
.base.text_color = color_purple,
.base.text_offset = 30,
.base.label_color = color_white,
.base.base.base.on_message = on_hobbs_msg,
.base.base.base.on_paint = on_paint_hobbs,
};

static hobbs_annunciator_t eng_annunciator = {
.base.base.base.style = BORDER_TOP | DRAW_NAME | BORDER_NONE,
.base.base.base.background_color = color_blue,
.base.base.base.name_color = color_white,
.base.base.base.name = "ENG",
.base.base.base.name_font = &neo_9_font,
.base.base.base.border_color = color_white,
.base.base.base.alarm_color = color_red,
.base.base.base.rect.left = 920,
.base.base.base.rect.top = 120,
.base.base.base.rect.right = 104 + 920,
.base.base.base.rect.bottom = 40 + 120,
.base.can_id = id_engine_hours,
.base.small_font = &neo_9_font,
.base.compact = true,
.base.text_color = color_purple,
.base.text_offset = 30,
.base.label_color = color_white,
.base.base.base.on_message = on_hobbs_msg,
.base.base.base.on_paint = on_paint_hobbs,
};

static datetime_annunciator_t lcl_annunciator = {
.base.base.base.style = BORDER_TOP | DRAW_NAME | BORDER_NONE,
.base.base.base.background_color = color_blue,
.base.base.base.name_color = color_white,
.base.base.base.name = "LCL",
.base.base.base.name_font = &neo_9_font,
.base.base.base.border_color = color_white,
.base.base.base.alarm_color = color_red,
.base.base.base.rect.left = 920,
.base.base.base.rect.top = 160,
.base.base.base.rect.right = 104 + 920,
.base.base.base.rect.bottom = 40 + 160,
.base.can_id = id_datetime_lcl,
.base.small_font = &neo_9_font,
.base.compact = true,
.base.text_color = color_purple,
.base.text_offset = 30,
.base.label_color = color_white,
.format = "%H:%M",
.base.base.base.on_message = on_datetime_msg,
.base.base.base.on_paint = on_paint_datetime,
};

static datetime_annunciator_t lcl_date_annunciator = {
.base.base.base.style = BORDER_TOP | BORDER_BOTTOM | DRAW_NAME | BORDER_NONE,
.base.base.base.background_color = color_blue,
.base.base.base.name_color = color_white,
.base.base.base.name = "LCL DATE",
.base.base.base.name_font = &neo_9_font,
.base.base.base.border_color = color_white,
.base.base.base.alarm_color = color_red,
.base.base.base.rect.left = 920,
.base.base.base.rect.top = 200,
.base.base.base.rect.right = 104 + 920,
.base.base.base.rect.bottom = 40 + 200,
.base.can_id = id_datetime_lcl,
.base.small_font = &neo_9_font,
.base.compact = true,
.base.text_color = color_purple,
.base.text_offset = 30,
.base.label_color = color_white,
.format = "%y-%m-%d",
.base.base.base.on_message = on_datetime_msg,
.base.base.base.on_paint = on_paint_datetime,
};

static pancake_widget_t pitch_indicator = {
.base.style = FILL_BACKGROUND |BORDER_NONE,
.base.background_color = color_black,
.base.name = "PITCH",
.base.border_color = color_white,
.base.alarm_color = color_red,
.base.rect.left = 920,
.base.rect.top = 394,
.base.rect.right = 104 + 920,
.base.rect.bottom = 104 + 394,
.indicator_rect.left = 38,
.indicator_rect.top = 5,
.indicator_rect.right = 30 + 38,
.indicator_rect.bottom = 102 + 5,
.can_id = id_pitch_trim_value,
.outline_color = color_white,
.indicator_color = color_lightblue,
.min_value = -100,
.max_value = 100,
.horizontal = false,
};

static pancake_widget_t roll_indicator = {
.base.style = FILL_BACKGROUND |BORDER_NONE,
.base.background_color = color_black,
.base.name = "ROLL",
.base.border_color = color_white,
.base.alarm_color = color_red,
.base.rect.left = 920,
.base.rect.top = 498,
.base.rect.right = 104 + 920,
.base.rect.bottom = 32 + 498,
.indicator_rect.left = 1,
.indicator_rect.top = 1,
.indicator_rect.right = 102 + 1,
.indicator_rect.bottom = 24 + 1,
.can_id = id_roll_trim_value,
.outline_color = color_white,
.indicator_color = color_lightblue,
.min_value = -100,
.max_value = 100,
.horizontal = true,
};

static const step_t flap_gauge_steps[] = {
{
.value = 0,
.pen = color_lightblue,
.color = color_green,
},
{
.value = 40,
.pen = color_lightblue,
.color = color_green,
},
};

static const tick_mark_t flap_gauge_ticks[] = {
{
.value = 0,
},
{
.value = 20,
},
{
.value = 40,
},
};

static gauge_widget_t flap_gauge = {
.base.style = DRAW_NAME | FILL_BACKGROUND |BORDER_NONE,
.base.background_color = color_black,
.base.name_color = color_white,
.base.name = "FLAP",
.base.name_font = &neo_9_bold_font,
.base.border_color = color_white,
.base.alarm_color = color_red,
.base.rect.left = 920,
.base.rect.top = 530,
.base.rect.right = 104 + 920,
.base.rect.bottom = 70 + 530,
.style = gs_bar,
.font = &neo_9_bold_font,
.can_id = id_flap_position_value,
.gauge_radii = 70,
.arc_width = 5,
.bar_width = 5,
.arc_begin = 0,
.arc_range = 40,
.base.name_pt.x = 30,
.base.name_pt.y = 15,
.center.x = 3,
.center.y = 3,
.steps = flap_gauge_steps,
.num_steps = numelements(flap_gauge_steps),
.ticks = flap_gauge_ticks,
.num_ticks = numelements(flap_gauge_ticks),
};

static const uint16_t marquee_alarm_ids_1[] = {
id_low_oil_pressure_alarm,
};

static const uint16_t marquee_alarm_ids_2[] = {
id_low_fuel_pressure_alarm,
};

static const uint16_t marquee_alarm_ids_3[] = {
id_high_oil_temperature_alarm,
};

static const uint16_t marquee_alarm_ids_4[] = {
id_timing_divergence_alarm,
id_rpm_divergence_alarm,
id_map_divergence_alarm,
id_iat_divergence_alarm,
id_fuel_pressure_divergence_alarm,
};

static const uint16_t marquee_alarm_ids_5[] = {
id_cylinder_head_temperature1_alarm,
id_cylinder_head_temperature2_alarm,
id_cylinder_head_temperature3_alarm,
id_cylinder_head_temperature4_alarm,
id_cylinder_head_temperature5_alarm,
id_cylinder_head_temperature6_alarm,
};

static const uint16_t marquee_alarm_ids_6[] = {
id_low_fuel_alarm,
};

static const uint16_t marquee_alarm_ids_7[] = {
id_cold_shock_alarm,
};

static const uint16_t marquee_alarm_ids_8[] = {
id_bus_volts_high_alarm,
};

static const uint16_t marquee_alarm_ids_9[] = {
id_bus_volts_low_alarm,
};

static const uint16_t marquee_alarm_ids_10[] = {
id_high_fuel_pressure_alarm,
};

static const uint16_t marquee_alarm_ids_11[] = {
id_high_amps_alarm,
};

static const uint16_t marquee_alarm_ids_12[] = {
id_mag_temperature_alarm,
};

static const uint16_t marquee_alarm_ids_13[] = {
id_fuel_flow_rate_alarm,
};

static const uint16_t marquee_alarm_ids_14[] = {
id_exhaust_gas_temperature1_alarm,
id_exhaust_gas_temperature2_alarm,
id_exhaust_gas_temperature3_alarm,
id_exhaust_gas_temperature4_alarm,
id_exhaust_gas_temperature5_alarm,
id_exhaust_gas_temperature6_alarm,
};

static auto_annunciator_t marquee_ann_1 = {
.base.base.base.style = BORDER_RIGHT | DRAW_NAME | FILL_BACKGROUND |BORDER_NONE,
.base.base.base.background_color = color_black,
.base.base.base.name = "Tot. Fuel",
.base.base.base.border_color = color_white,
.base.base.base.on_paint = on_paint_auto,
.base.base.base.on_message = on_auto_msg,
.base.base.base.alarm_color = color_red,
.base.base.base.rect.left = 0,
.base.base.base.rect.top = 0,
.base.base.base.rect.right = 224 + 0,
.base.base.base.rect.bottom = 33 + 0,
.base.can_id = id_fuel_total,
.base.small_font = &neo_15_font,
.base.text_color = color_lightblue,
.base.text_offset = 110,
.base.label_color = color_white,
.converter = &convert_cc_to_litres,
.value_type = v_uint32,
.fmt = "%d",
.base.base.base.on_message = on_auto_msg,
.base.base.base.on_paint = on_paint_auto,
};

static hobbs_annunciator_t m_fuel_tot = {
.base.base.base.style = BORDER_RIGHT | DRAW_NAME | FILL_BACKGROUND |BORDER_NONE,
.base.base.base.background_color = color_black,
.base.base.base.name = "Fuel Endur.",
.base.base.base.border_color = color_white,
.base.base.base.on_paint = on_paint_hours,
.base.base.base.on_message = on_hours_msg,
.base.base.base.alarm_color = color_red,
.base.base.base.rect.left = 0,
.base.base.base.rect.top = 0,
.base.base.base.rect.right = 224 + 0,
.base.base.base.rect.bottom = 33 + 0,
.base.can_id = id_fuel_endurance,
.base.small_font = &neo_15_font,
.base.text_color = color_lightblue,
.base.text_offset = 110,
.base.label_color = color_white,
.base.base.base.on_message = on_hobbs_msg,
.base.base.base.on_paint = on_paint_hobbs,
};

static auto_annunciator_t m_fuel_pressure = {
.base.base.base.style = BORDER_RIGHT | DRAW_NAME | FILL_BACKGROUND |BORDER_NONE,
.base.base.base.background_color = color_black,
.base.base.base.name = "Fuel Pres.",
.base.base.base.border_color = color_white,
.base.base.base.on_paint = on_paint_auto,
.base.base.base.on_message = on_auto_msg,
.base.base.base.alarm_color = color_red,
.base.base.base.rect.left = 0,
.base.base.base.rect.top = 0,
.base.base.base.rect.right = 224 + 0,
.base.base.base.rect.bottom = 33 + 0,
.base.can_id = id_fuel_pressure,
.base.small_font = &neo_15_font,
.base.text_color = color_lightblue,
.base.text_offset = 110,
.base.label_color = color_white,
.converter = &convert_hpa_to_psi,
.value_type = v_int16,
.fmt = "%d",
.base.base.base.on_message = on_auto_msg,
.base.base.base.on_paint = on_paint_auto,
};

static auto_annunciator_t m_fuel_flow = {
.base.base.base.style = BORDER_RIGHT | DRAW_NAME | FILL_BACKGROUND |BORDER_NONE,
.base.base.base.background_color = color_black,
.base.base.base.name = "Fuel Flow",
.base.base.base.border_color = color_white,
.base.base.base.on_paint = on_paint_auto,
.base.base.base.on_message = on_auto_msg,
.base.base.base.alarm_color = color_red,
.base.base.base.rect.left = 0,
.base.base.base.rect.top = 0,
.base.base.base.rect.right = 224 + 0,
.base.base.base.rect.bottom = 33 + 0,
.base.can_id = id_fuel_flow_rate,
.base.small_font = &neo_15_font,
.base.text_color = color_lightblue,
.base.text_offset = 110,
.base.label_color = color_white,
.converter = &convert_flow_to_litres,
.value_type = v_uint32,
.fmt = "%d",
.base.base.base.on_message = on_auto_msg,
.base.base.base.on_paint = on_paint_auto,
};

static hobbs_annunciator_t marquee_ann_5 = {
.base.base.base.style = BORDER_RIGHT | DRAW_NAME | FILL_BACKGROUND |BORDER_NONE,
.base.base.base.background_color = color_black,
.base.base.base.name = "Eng. Hrs.",
.base.base.base.border_color = color_white,
.base.base.base.on_paint = on_paint_hours,
.base.base.base.on_message = on_hours_msg,
.base.base.base.alarm_color = color_red,
.base.base.base.rect.left = 0,
.base.base.base.rect.top = 0,
.base.base.base.rect.right = 224 + 0,
.base.base.base.rect.bottom = 33 + 0,
.base.can_id = id_engine_hours,
.base.small_font = &neo_15_font,
.base.text_color = color_lightblue,
.base.text_offset = 110,
.base.label_color = color_white,
.base.base.base.on_message = on_hobbs_msg,
.base.base.base.on_paint = on_paint_hobbs,
};

static auto_annunciator_t m_oil_pressure = {
.base.base.base.style = BORDER_RIGHT | DRAW_NAME | FILL_BACKGROUND |BORDER_NONE,
.base.base.base.background_color = color_black,
.base.base.base.name = "Oil Pres.",
.base.base.base.border_color = color_white,
.base.base.base.on_paint = on_paint_auto,
.base.base.base.on_message = on_auto_msg,
.base.base.base.alarm_color = color_red,
.base.base.base.rect.left = 0,
.base.base.base.rect.top = 0,
.base.base.base.rect.right = 224 + 0,
.base.base.base.rect.bottom = 33 + 0,
.base.can_id = id_oil_pressure,
.base.small_font = &neo_15_font,
.base.text_color = color_lightblue,
.base.text_offset = 110,
.base.label_color = color_white,
.converter = &convert_hpa_to_psi,
.value_type = v_uint16,
.fmt = "%d",
.base.base.base.on_message = on_auto_msg,
.base.base.base.on_paint = on_paint_auto,
};

static auto_annunciator_t m_oil_temp = {
.base.base.base.style = BORDER_RIGHT | DRAW_NAME | FILL_BACKGROUND |BORDER_NONE,
.base.base.base.background_color = color_black,
.base.base.base.name = "Oil Temp.",
.base.base.base.border_color = color_white,
.base.base.base.on_paint = on_paint_auto,
.base.base.base.on_message = on_auto_msg,
.base.base.base.alarm_color = color_red,
.base.base.base.rect.left = 0,
.base.base.base.rect.top = 0,
.base.base.base.rect.right = 224 + 0,
.base.base.base.rect.bottom = 33 + 0,
.base.can_id = id_oil_temperature,
.base.small_font = &neo_15_font,
.base.text_color = color_lightblue,
.base.text_offset = 110,
.base.label_color = color_white,
.converter = &convert_kelvin_to_c,
.value_type = v_uint16,
.fmt = "%d",
.base.base.base.on_message = on_auto_msg,
.base.base.base.on_paint = on_paint_auto,
};

static auto_annunciator_t m_volts = {
.base.base.base.style = BORDER_RIGHT | DRAW_NAME | FILL_BACKGROUND |BORDER_NONE,
.base.base.base.background_color = color_black,
.base.base.base.name = "Volts",
.base.base.base.border_color = color_white,
.base.base.base.on_paint = on_paint_auto,
.base.base.base.on_message = on_auto_msg,
.base.base.base.alarm_color = color_red,
.base.base.base.rect.left = 0,
.base.base.base.rect.top = 0,
.base.base.base.rect.right = 224 + 0,
.base.base.base.rect.bottom = 33 + 0,
.base.can_id = id_dc_voltage,
.base.small_font = &neo_15_font,
.base.text_color = color_lightblue,
.base.text_offset = 110,
.base.label_color = color_white,
.value_type = v_uint16,
.fmt = "%d",
.base.base.base.on_message = on_auto_msg,
.base.base.base.on_paint = on_paint_auto,
};

static auto_annunciator_t m_amps = {
.base.base.base.style = BORDER_RIGHT | DRAW_NAME | FILL_BACKGROUND |BORDER_NONE,
.base.base.base.background_color = color_black,
.base.base.base.name = "Amps",
.base.base.base.border_color = color_white,
.base.base.base.on_paint = on_paint_auto,
.base.base.base.on_message = on_auto_msg,
.base.base.base.alarm_color = color_red,
.base.base.base.rect.left = 0,
.base.base.base.rect.top = 0,
.base.base.base.rect.right = 224 + 0,
.base.base.base.rect.bottom = 33 + 0,
.base.can_id = id_dc_current,
.base.small_font = &neo_15_font,
.base.text_color = color_lightblue,
.base.text_offset = 110,
.base.label_color = color_white,
.value_type = v_int16,
.fmt = "%d",
.base.base.base.on_message = on_auto_msg,
.base.base.base.on_paint = on_paint_auto,
};

static auto_annunciator_t m_iat = {
.base.base.base.style = BORDER_RIGHT | DRAW_NAME | FILL_BACKGROUND |BORDER_NONE,
.base.base.base.background_color = color_black,
.base.base.base.name = "IAT",
.base.base.base.border_color = color_white,
.base.base.base.on_paint = on_paint_auto,
.base.base.base.on_message = on_auto_msg,
.base.base.base.alarm_color = color_red,
.base.base.base.rect.left = 0,
.base.base.base.rect.top = 0,
.base.base.base.rect.right = 224 + 0,
.base.base.base.rect.bottom = 33 + 0,
.base.can_id = id_inlet_air_temperature,
.base.small_font = &neo_15_font,
.base.text_color = color_lightblue,
.base.text_offset = 110,
.base.label_color = color_white,
.converter = &convert_kelvin_to_c,
.value_type = v_int16,
.fmt = "%d",
.base.base.base.on_message = on_auto_msg,
.base.base.base.on_paint = on_paint_auto,
};

static annunciator_t *marquee_annunciators[] = {
(annunciator_t *) &marquee_ann_1,
(annunciator_t *) &m_fuel_tot,
(annunciator_t *) &m_fuel_pressure,
(annunciator_t *) &m_fuel_flow,
(annunciator_t *) &marquee_ann_5,
(annunciator_t *) &m_oil_pressure,
(annunciator_t *) &m_oil_temp,
(annunciator_t *) &m_volts,
(annunciator_t *) &m_amps,
(annunciator_t *) &m_iat,
};

static alarm_t marquee_alarms[] = {
{
.can_ids = marquee_alarm_ids_1,
.num_ids = numelements(marquee_alarm_ids_1),
.annunciator = (annunciator_t *) &m_oil_pressure, 
.priority = 1,
.can_park = true,
.message = "Low oil pressure",
.base.style = FILL_BACKGROUND,
.base.rect.left = 1,
.base.rect.top = 1,
.base.rect.right = 224 -2,
.base.rect.bottom = 33 -2,
.base.on_paint = on_paint_alarm_foreground,
.base.name_font = &neo_15_font,
.base.name_color = color_black,
.base.background_color = color_red,
},
{
.can_ids = marquee_alarm_ids_2,
.num_ids = numelements(marquee_alarm_ids_2),
.annunciator = (annunciator_t *) &m_fuel_pressure, 
.priority = 2,
.can_park = true,
.message = "Low fuel pressure",
.base.style = FILL_BACKGROUND,
.base.rect.left = 1,
.base.rect.top = 1,
.base.rect.right = 224 -2,
.base.rect.bottom = 33 -2,
.base.on_paint = on_paint_alarm_foreground,
.base.name_font = &neo_15_font,
.base.name_color = color_black,
.base.background_color = color_red,
},
{
.can_ids = marquee_alarm_ids_3,
.num_ids = numelements(marquee_alarm_ids_3),
.annunciator = (annunciator_t *) &m_oil_temp, 
.priority = 3,
.can_park = true,
.message = "High oil temperature",
.base.style = FILL_BACKGROUND,
.base.rect.left = 1,
.base.rect.top = 1,
.base.rect.right = 224 -2,
.base.rect.bottom = 33 -2,
.base.on_paint = on_paint_alarm_foreground,
.base.name_font = &neo_15_font,
.base.name_color = color_black,
.base.background_color = color_red,
},
{
.can_ids = marquee_alarm_ids_4,
.num_ids = numelements(marquee_alarm_ids_4),
.priority = 5,
.can_park = true,
.message = "Timing divergence",
.base.style = FILL_BACKGROUND,
.base.rect.left = 1,
.base.rect.top = 1,
.base.rect.right = 224 -2,
.base.rect.bottom = 33 -2,
.base.on_paint = on_paint_alarm_foreground,
.base.name_font = &neo_15_font,
.base.name_color = color_black,
.base.background_color = color_red,
},
{
.can_ids = marquee_alarm_ids_5,
.num_ids = numelements(marquee_alarm_ids_5),
.priority = 6,
.can_park = true,
.message = "High CHT",
.base.style = FILL_BACKGROUND,
.base.rect.left = 1,
.base.rect.top = 1,
.base.rect.right = 224 -2,
.base.rect.bottom = 33 -2,
.base.on_paint = on_paint_alarm_foreground,
.base.name_font = &neo_15_font,
.base.name_color = color_black,
.base.background_color = color_red,
},
{
.can_ids = marquee_alarm_ids_6,
.num_ids = numelements(marquee_alarm_ids_6),
.annunciator = (annunciator_t *) &m_fuel_tot, 
.priority = 7,
.can_park = true,
.message = "Low Fuel",
.base.style = FILL_BACKGROUND,
.base.rect.left = 1,
.base.rect.top = 1,
.base.rect.right = 224 -2,
.base.rect.bottom = 33 -2,
.base.on_paint = on_paint_alarm_foreground,
.base.name_font = &neo_15_font,
.base.name_color = color_black,
.base.background_color = color_red,
},
{
.can_ids = marquee_alarm_ids_7,
.num_ids = numelements(marquee_alarm_ids_7),
.priority = 8,
.can_park = true,
.message = "CHT Cold Shock",
.base.style = FILL_BACKGROUND,
.base.rect.left = 1,
.base.rect.top = 1,
.base.rect.right = 224 -2,
.base.rect.bottom = 33 -2,
.base.on_paint = on_paint_alarm_foreground,
.base.name_font = &neo_15_font,
.base.name_color = color_black,
.base.background_color = color_red,
},
{
.can_ids = marquee_alarm_ids_8,
.num_ids = numelements(marquee_alarm_ids_8),
.annunciator = (annunciator_t *) &m_volts, 
.priority = 9,
.can_park = true,
.message = "High voltage",
.base.style = FILL_BACKGROUND,
.base.rect.left = 1,
.base.rect.top = 1,
.base.rect.right = 224 -2,
.base.rect.bottom = 33 -2,
.base.on_paint = on_paint_alarm_foreground,
.base.name_font = &neo_15_font,
.base.name_color = color_black,
.base.background_color = color_red,
},
{
.can_ids = marquee_alarm_ids_9,
.num_ids = numelements(marquee_alarm_ids_9),
.annunciator = (annunciator_t *) &m_volts, 
.priority = 10,
.can_park = true,
.message = "Low Voltage",
.base.style = FILL_BACKGROUND,
.base.rect.left = 1,
.base.rect.top = 1,
.base.rect.right = 224 -2,
.base.rect.bottom = 33 -2,
.base.on_paint = on_paint_alarm_foreground,
.base.name_font = &neo_15_font,
.base.name_color = color_black,
.base.background_color = color_red,
},
{
.can_ids = marquee_alarm_ids_10,
.num_ids = numelements(marquee_alarm_ids_10),
.annunciator = (annunciator_t *) &m_fuel_pressure, 
.priority = 11,
.can_park = true,
.message = "High Fuel Pressure",
.base.style = FILL_BACKGROUND,
.base.rect.left = 1,
.base.rect.top = 1,
.base.rect.right = 224 -2,
.base.rect.bottom = 33 -2,
.base.on_paint = on_paint_alarm_foreground,
.base.name_font = &neo_15_font,
.base.name_color = color_black,
.base.background_color = color_red,
},
{
.can_ids = marquee_alarm_ids_11,
.num_ids = numelements(marquee_alarm_ids_11),
.annunciator = (annunciator_t *) &m_amps, 
.priority = 12,
.can_park = true,
.message = "High Current",
.base.style = FILL_BACKGROUND,
.base.rect.left = 1,
.base.rect.top = 1,
.base.rect.right = 224 -2,
.base.rect.bottom = 33 -2,
.base.on_paint = on_paint_alarm_foreground,
.base.name_font = &neo_15_font,
.base.name_color = color_black,
.base.background_color = color_red,
},
{
.can_ids = marquee_alarm_ids_12,
.num_ids = numelements(marquee_alarm_ids_12),
.priority = 13,
.can_park = true,
.message = "kMAG Temperature",
.base.style = FILL_BACKGROUND,
.base.rect.left = 1,
.base.rect.top = 1,
.base.rect.right = 224 -2,
.base.rect.bottom = 33 -2,
.base.on_paint = on_paint_alarm_foreground,
.base.name_font = &neo_15_font,
.base.name_color = color_black,
.base.background_color = color_red,
},
{
.can_ids = marquee_alarm_ids_13,
.num_ids = numelements(marquee_alarm_ids_13),
.annunciator = (annunciator_t *) &m_fuel_flow, 
.priority = 14,
.can_park = true,
.message = "Low Fuel Flow",
.base.style = FILL_BACKGROUND,
.base.rect.left = 1,
.base.rect.top = 1,
.base.rect.right = 224 -2,
.base.rect.bottom = 33 -2,
.base.on_paint = on_paint_alarm_foreground,
.base.name_font = &neo_15_font,
.base.name_color = color_black,
.base.background_color = color_red,
},
{
.can_ids = marquee_alarm_ids_14,
.num_ids = numelements(marquee_alarm_ids_14),
.priority = 14,
.can_park = true,
.message = "EGT Temperature",
.base.style = FILL_BACKGROUND,
.base.rect.left = 1,
.base.rect.top = 1,
.base.rect.right = 224 -2,
.base.rect.bottom = 33 -2,
.base.on_paint = on_paint_alarm_foreground,
.base.name_font = &neo_15_font,
.base.name_color = color_black,
.base.background_color = color_red,
},
};

static marquee_widget_t marquee = {
.base.style = BORDER_RIGHT | FILL_BACKGROUND |BORDER_NONE,
.base.background_color = color_black,
.base.name = "ALARMS",
.base.border_color = color_white,
.base.on_paint = on_paint_marquee,
.base.alarm_color = color_red,
.base.rect.left = 0,
.base.rect.top = 567,
.base.rect.right = 224 + 0,
.base.rect.bottom = 33 + 567,
.base_widget_id = 1000,
.base_alarm_id = 2000,
.annunciators = marquee_annunciators,
.num_annunciators = numelements(marquee_annunciators),
.alarms = marquee_alarms,
.num_alarms = numelements(marquee_alarms),
};

// main application constructor
result_t create_au_nz(handle_t hwnd)
{
result_t result;
handle_t child;
handle_t widget;
(void)child;
(void)widget;

if(failed(result = create_menu_window(hwnd, au_nz_menu_widget.base.style, &au_nz_menu_widget, &menu_handle)) ||
  failed(result = set_z_order(menu_handle, 100)))
  return result;

if(failed(result = create_gauge_widget(hwnd, 1, &asi_widget, &child)))
  return result;
if(failed(result = create_attitude_widget(hwnd, 2, &att_widget, &child)))
  return result;
if(failed(result = create_gauge_widget(hwnd, 3, &alt_widget, &child)))
  return result;
if(failed(result = create_gauge_widget(hwnd, 4, &vsi_widget, &child)))
  return result;
if(failed(result = create_hsi_widget(hwnd, 5, &hsi_widget, &child)))
  return result;
if(failed(result = create_gauge_widget(hwnd, 6, &map_gauge, &child)))
  return result;
if(failed(result = create_gauge_widget(hwnd, 7, &rpm_gauge, &child)))
  return result;
if(failed(result = create_annunciator_widget(hwnd, 8, (annunciator_t *) &hp_annunciator, &child)))
  return result;
if(failed(result = create_annunciator_widget(hwnd, 9, (annunciator_t *) &kmag_annunciator, &child)))
  return result;
if(failed(result = create_annunciator_widget(hwnd, 10, (annunciator_t *) &oat_annunciator, &child)))
  return result;
if(failed(result = create_gauge_widget(hwnd, 12, &oilt_gauge, &child)))
  return result;
if(failed(result = create_gauge_widget(hwnd, 13, &oilp_gauge, &child)))
  return result;
if(failed(result = create_gauge_widget(hwnd, 14, &fuelp_gauge, &child)))
  return result;
if(failed(result = create_gauge_widget(hwnd, 15, &fuelf_gauge, &child)))
  return result;
if(failed(result = create_gauge_widget(hwnd, 16, &left_fuel_gauge, &child)))
  return result;
if(failed(result = create_gauge_widget(hwnd, 17, &right_fuel_gauge, &child)))
  return result;
if(failed(result = create_gauge_widget(hwnd, 18, &fuelt_gauge, &child)))
  return result;
if(failed(result = create_annunciator_widget(hwnd, 19, (annunciator_t *) &utc_annunciator, &child)))
  return result;
if(failed(result = create_annunciator_widget(hwnd, 20, (annunciator_t *) &utc_date_annunciator, &child)))
  return result;
if(failed(result = create_annunciator_widget(hwnd, 21, (annunciator_t *) &air_annunciator, &child)))
  return result;
if(failed(result = create_annunciator_widget(hwnd, 22, (annunciator_t *) &eng_annunciator, &child)))
  return result;
if(failed(result = create_annunciator_widget(hwnd, 23, (annunciator_t *) &lcl_annunciator, &child)))
  return result;
if(failed(result = create_annunciator_widget(hwnd, 24, (annunciator_t *) &lcl_date_annunciator, &child)))
  return result;
if(failed(result = create_pancake_widget(hwnd, 25, &pitch_indicator, &child)))
  return result;
if(failed(result = create_pancake_widget(hwnd, 26, &roll_indicator, &child)))
  return result;
if(failed(result = create_gauge_widget(hwnd, 27, &flap_gauge, &child)))
  return result;
if(failed(result = create_marquee_widget(hwnd, 28, &marquee, &widget)))
  return result;

marquee.selected_alarm = -1;
uint16_t child_num = 0;
if(failed(result = create_annunciator_widget(widget, marquee.base_widget_id + child_num++, (annunciator_t *)&marquee_ann_1, &child)))
  return result;

hide_window(child);

if(failed(result = create_annunciator_widget(widget, marquee.base_widget_id + child_num++, (annunciator_t *)&m_fuel_tot, &child)))
  return result;

hide_window(child);

if(failed(result = create_annunciator_widget(widget, marquee.base_widget_id + child_num++, (annunciator_t *)&m_fuel_pressure, &child)))
  return result;

hide_window(child);

if(failed(result = create_annunciator_widget(widget, marquee.base_widget_id + child_num++, (annunciator_t *)&m_fuel_flow, &child)))
  return result;

hide_window(child);

if(failed(result = create_annunciator_widget(widget, marquee.base_widget_id + child_num++, (annunciator_t *)&marquee_ann_5, &child)))
  return result;

hide_window(child);

if(failed(result = create_annunciator_widget(widget, marquee.base_widget_id + child_num++, (annunciator_t *)&m_oil_pressure, &child)))
  return result;

hide_window(child);

if(failed(result = create_annunciator_widget(widget, marquee.base_widget_id + child_num++, (annunciator_t *)&m_oil_temp, &child)))
  return result;

hide_window(child);

if(failed(result = create_annunciator_widget(widget, marquee.base_widget_id + child_num++, (annunciator_t *)&m_volts, &child)))
  return result;

hide_window(child);

if(failed(result = create_annunciator_widget(widget, marquee.base_widget_id + child_num++, (annunciator_t *)&m_amps, &child)))
  return result;

hide_window(child);

if(failed(result = create_annunciator_widget(widget, marquee.base_widget_id + child_num++, (annunciator_t *)&m_iat, &child)))
  return result;

hide_window(child);

if(failed(result = show_marquee_child(widget, 0)))
  return result;

// Alarms
child_num = 0;
if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num++], 0)))
  return result;

if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num++], 0)))
  return result;

if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num++], 0)))
  return result;

if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num++], 0)))
  return result;

if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num++], 0)))
  return result;

if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num++], 0)))
  return result;

if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num++], 0)))
  return result;

if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num++], 0)))
  return result;

if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num++], 0)))
  return result;

if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num++], 0)))
  return result;

if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num++], 0)))
  return result;

if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num++], 0)))
  return result;

if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num++], 0)))
  return result;

if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num++], 0)))
  return result;

return s_ok;
}
