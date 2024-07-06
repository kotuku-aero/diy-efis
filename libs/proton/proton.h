#ifndef __proton_h__
#define	__proton_h__
// #include "../ion/ion.h"
#include "../../libs/photon/photon_priv.h"
#include "widget.h"

typedef struct _aircraft_t aircraft_t;

typedef result_t (*create_widgets_fn)(handle_t hwnd, aircraft_t* aircraft);
typedef result_t (*create_layout_fn)(handle_t parent, memid_t key, create_widgets_fn layout_fn, handle_t* hndl);

typedef struct _proton_params_t {
  photon_parameters_t photon_params;
  create_widgets_fn create_widgets;
  create_layout_fn create_layout;
  } proton_params_t;

/**
 * @function run_proton(void *parg, handle_t ci, handle_t co, handle_t cerr)
 * Run the main window application loop
 * @param parg  Argument to pass
 */
extern result_t run_proton(proton_params_t *args);

extern const font_t neo_9_font;
extern const font_t neo_12_font;
extern const font_t neo_15_font;
extern const font_t neo_18_font;

// these are the ID's used by proton
// events
#define id_key0 1420
#define id_key1 1421
#define id_key2 1422
#define id_key3 1423
#define id_key4 1424
#define id_key5 1425
#define id_key6 1426
#define id_key7 1427
#define id_decka 1428
#define id_press_decka 1429
#define id_deckb 1430
#define id_press_deckb 1431
#define id_menu_up 1432
#define id_menu_dn 1433
#define id_menu_left 1434
#define id_menu_right 1435
#define id_menu_ok 1436
#define id_menu_cancel 1437
#define id_menu_select 1438
#define id_buttonpress 1439

#define id_marquee_next 1440
#define id_marquee_prev 1441
#define id_spin_edit 1442
#define id_alarm_close 1443
#define id_alarm_park 1444
#define id_display_units  1445

#define id_app_action_id_first 1450
#define id_app_action_id_last  1499

#endif

