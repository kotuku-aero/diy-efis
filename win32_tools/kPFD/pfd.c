
    #include "pfd.h"
    #include "../../libs/proton/airspeed_widget.h"
    #include "../../libs/proton/altitude_widget.h"
    #include "../../libs/proton/annunciator_widget.h"
    #include "../../libs/proton/ap_widget.h"
    #include "../../libs/proton/attitude_widget.h"
    #include "../../libs/proton/carousel_widget.h"
    #include "../../libs/proton/comm_widget.h"
    #include "../../libs/proton/edutemp_widget.h"
    #include "../../libs/proton/gauge_widget.h"
    #include "../../libs/proton/hsi_widget.h"
    #include "../../libs/proton/map_widget.h"
    #include "../../libs/proton/marquee_widget.h"
    #include "../../libs/proton/menu_window.h"
    #include "../../libs/proton/softkey_widget.h"
    #include "../../libs/proton/pancake_indicator_widget.h"
    
    static keys_t root;
  
    static keys_t alarm_keys;
  
    static keys_t popup_keys;
  
    static keys_t edit_keys;
  

    static menu_t settings_menu;
  

    static menu_t config_menu;
  

    static menu_t profile_menu;
  

    static menu_t units_menu;
  

    static menu_item_menu_t  menu_item_root_key0 = {
    .base.caption = "Settings",
    .base.event = default_msg_handler,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    .base.evaluate = item_menu_evaluate,
    .menu = &settings_menu
    };
  

    static menu_item_event_t menu_item_root_key1 = {
    .can_id = id_menu_cancel,
    .base.evaluate = item_event_evaluate,
    .base.event = default_msg_handler,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    

    .value.vt = v_int16,
    
        .value.value.int16 = 1,
      
      .base.caption = "Cancel",
    

    };
  

    static menu_item_event_t menu_item_root_decka_up = {
    .can_id = id_marquee_next,
    .base.evaluate = item_event_evaluate,
    .base.event = default_msg_handler,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    

    .value.vt = v_int16,
    
        .value.value.int16 = 1,
      
      .base.caption = "Next",
    

    };
  

    static menu_item_event_t menu_item_root_decka_dn = {
    .can_id = id_marquee_prev,
    .base.evaluate = item_event_evaluate,
    .base.event = default_msg_handler,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    

    .value.vt = v_int16,
    
        .value.value.int16 = 1,
      
      .base.caption = "Prev",
    

    };
  

    static keys_t root = {
    
      .key0 = (menu_item_t *) &menu_item_root_key0,
    
      .key1 = (menu_item_t *) &menu_item_root_key1,
    
      .decka_up = (menu_item_t *) &menu_item_root_decka_up,
    
      .decka_dn = (menu_item_t *) &menu_item_root_decka_dn,
    
    };
  

    static menu_item_event_t menu_item_alarm_keys_key0 = {
    .can_id = id_alarm_close,
    .base.evaluate = item_event_evaluate,
    .base.event = default_msg_handler,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    

    .value.vt = v_int16,
    
        .value.value.int16 = 1,
      
      .base.caption = "Close",
    

    };
  

    static menu_item_event_t menu_item_alarm_keys_key1 = {
    .can_id = id_alarm_park,
    .base.evaluate = item_event_evaluate,
    .base.event = default_msg_handler,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    

    .value.vt = v_int16,
    
        .value.value.int16 = 1,
      
      .base.caption = "Park",
    

    };
  

    static keys_t alarm_keys = {
    
      .key0 = (menu_item_t *) &menu_item_alarm_keys_key0,
    
      .key1 = (menu_item_t *) &menu_item_alarm_keys_key1,
    
    };
  

    static menu_item_event_t menu_item_popup_keys_key0 = {
    .can_id = id_menu_ok,
    .base.evaluate = item_event_evaluate,
    .base.event = default_msg_handler,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    

    .value.vt = v_int16,
    
        .value.value.int16 = 1,
      
      .base.caption = "OK",
    

    };
  

    static menu_item_event_t menu_item_popup_keys_key1 = {
    .can_id = id_menu_cancel,
    .base.evaluate = item_event_evaluate,
    .base.event = default_msg_handler,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    

    .value.vt = v_int16,
    
        .value.value.int16 = 1,
      
      .base.caption = "Cancel",
    

    };
  

    static menu_item_event_t menu_item_popup_keys_decka_up = {
    .can_id = id_menu_up,
    .base.evaluate = item_event_evaluate,
    .base.event = default_msg_handler,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    

    .value.vt = v_int16,
    
        .value.value.int16 = 1,
      
      .base.caption = "Up",
    

    };
  

    static menu_item_event_t menu_item_popup_keys_decka_dn = {
    .can_id = id_menu_dn,
    .base.evaluate = item_event_evaluate,
    .base.event = default_msg_handler,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    

    .value.vt = v_int16,
    
        .value.value.int16 = 1,
      
      .base.caption = "Dn",
    

    };
  

    static keys_t popup_keys = {
    
      .key0 = (menu_item_t *) &menu_item_popup_keys_key0,
    
      .key1 = (menu_item_t *) &menu_item_popup_keys_key1,
    
      .decka_up = (menu_item_t *) &menu_item_popup_keys_decka_up,
    
      .decka_dn = (menu_item_t *) &menu_item_popup_keys_decka_dn,
    
    };
  

    static menu_item_event_t menu_item_edit_keys_key0 = {
    .can_id = id_menu_ok,
    .base.evaluate = item_event_evaluate,
    .base.event = default_msg_handler,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    

    .value.vt = v_int16,
    
        .value.value.int16 = 1,
      
      .base.caption = "OK",
    

    };
  

    static menu_item_event_t menu_item_edit_keys_key1 = {
    .can_id = id_menu_cancel,
    .base.evaluate = item_event_evaluate,
    .base.event = default_msg_handler,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    

    .value.vt = v_int16,
    
        .value.value.int16 = 1,
      
      .base.caption = "Cancel",
    

    };
  

    static menu_item_event_t menu_item_edit_keys_decka_up = {
    .can_id = id_spin_edit,
    .base.evaluate = item_event_evaluate,
    .base.event = default_msg_handler,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    

    .value.vt = v_int16,
    
        .value.value.int16 = 10,
      
      .base.caption = "Up",
    

    };
  

    static menu_item_event_t menu_item_edit_keys_decka_dn = {
    .can_id = id_spin_edit,
    .base.evaluate = item_event_evaluate,
    .base.event = default_msg_handler,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    

    .value.vt = v_int16,
    
        .value.value.int16 = -10,
      
      .base.caption = "Dn",
    

    };
  

    static menu_item_event_t menu_item_edit_keys_decka_press_up = {
    .can_id = id_spin_edit,
    .base.evaluate = item_event_evaluate,
    .base.event = default_msg_handler,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    

    .value.vt = v_int16,
    
        .value.value.int16 = 1,
      
      .base.caption = "Up",
    

    };
  

    static menu_item_event_t menu_item_edit_keys_decka_press_dn = {
    .can_id = id_spin_edit,
    .base.evaluate = item_event_evaluate,
    .base.event = default_msg_handler,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    

    .value.vt = v_int16,
    
        .value.value.int16 = -1,
      
      .base.caption = "Dn",
    

    };
  

    static keys_t edit_keys = {
    
      .key0 = (menu_item_t *) &menu_item_edit_keys_key0,
    
      .key1 = (menu_item_t *) &menu_item_edit_keys_key1,
    
      .decka_up = (menu_item_t *) &menu_item_edit_keys_decka_up,
    
      .decka_dn = (menu_item_t *) &menu_item_edit_keys_decka_dn,
    
      .decka_press_up = (menu_item_t *) &menu_item_edit_keys_decka_press_up,
    
      .decka_press_dn = (menu_item_t *) &menu_item_edit_keys_decka_press_dn,
    
    };
  

    static menu_item_menu_t  settings_menu_item_1 = {
    .base.caption = "Config",
    .base.event = default_msg_handler,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    .base.evaluate = item_menu_evaluate,
    .menu = &config_menu
    };
  

    static menu_item_checklist_t  settings_menu_item_2 = {
    .base.caption = "ECU Mode",
    .base.event = item_checklist_event,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    .base.evaluate = item_checklist_evaluate,
    .get_selected = get_fuel_map,
     .popup = &profile_menu
    };
  

    static menu_item_t *settings_menu_items[] = {
    (menu_item_t *) &settings_menu_item_1,
    (menu_item_t *) &settings_menu_item_2,
    
    };

    static menu_t settings_menu = {
    .menu_items = (menu_item_t **) &settings_menu_items,
    .num_items = numelements(settings_menu_items),
    .name = "settings_menu",
    .caption = "Settings",
    .keys = &popup_keys
    };
  

    static menu_item_checklist_t  config_menu_item_1 = {
    .base.caption = "Units",
    .base.event = item_checklist_event,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    .base.evaluate = item_checklist_evaluate,
    .get_selected = get_selected_units,
     .popup = &units_menu
    };
  

    static menu_item_spin_edit_t  config_menu_item_2 = {
    .base.caption = "Brightness",
    .base.event = item_spin_edit_event,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    .base.evaluate = item_spin_edit_evaluate,
    .get_value = get_brightness,
    .set_value = set_brightness,
    .keys = &edit_keys,
    
      .digits = 0,
    
      .min_value = 10,
    
      .max_value = 100,
    
    };
  

    static menu_item_t *config_menu_items[] = {
    (menu_item_t *) &config_menu_item_1,
    (menu_item_t *) &config_menu_item_2,
    
    };

    static menu_t config_menu = {
    .menu_items = (menu_item_t **) &config_menu_items,
    .num_items = numelements(config_menu_items),
    .name = "config_menu",
    .caption = "Config",
    .keys = &popup_keys
    };
  

    static menu_item_event_t profile_menu_item_1 = {
    .can_id = id_set_fuel_map,
    .base.evaluate = item_event_evaluate,
    .base.event = default_msg_handler,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    

    .value.vt = v_uint16,
    
        .value.value.uint16 = 0x0002,
      
      .base.caption = "Climb",
    

    };
  

    static menu_item_event_t profile_menu_item_2 = {
    .can_id = id_set_fuel_map,
    .base.evaluate = item_event_evaluate,
    .base.event = default_msg_handler,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    

    .value.vt = v_uint16,
    
        .value.value.uint16 = 0x0003,
      
      .base.caption = "Cruise",
    

    };
  

    static menu_item_t *profile_menu_items[] = {
    (menu_item_t *) &profile_menu_item_1,
    (menu_item_t *) &profile_menu_item_2,
    
    };

    static menu_t profile_menu = {
    .menu_items = (menu_item_t **) &profile_menu_items,
    .num_items = numelements(profile_menu_items),
    .name = "profile_menu",
    .caption = "Lean",
    .keys = &popup_keys
    };
  

    static menu_item_event_t units_menu_item_1 = {
    .can_id = id_display_units,
    .base.evaluate = item_event_evaluate,
    .base.event = default_msg_handler,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    

    .value.vt = v_uint16,
    
        .value.value.uint16 = 0x8000,
      
      .base.caption = "SI",
    

    };
  

    static menu_item_event_t units_menu_item_2 = {
    .can_id = id_display_units,
    .base.evaluate = item_event_evaluate,
    .base.event = default_msg_handler,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    

    .value.vt = v_uint16,
    
        .value.value.uint16 = 0x8001,
      
      .base.caption = "US",
    

    };
  

    static menu_item_t *units_menu_items[] = {
    (menu_item_t *) &units_menu_item_1,
    (menu_item_t *) &units_menu_item_2,
    
    };

    static menu_t units_menu = {
    .menu_items = (menu_item_t **) &units_menu_items,
    .num_items = numelements(units_menu_items),
    .name = "units_menu",
    .caption = "Units",
    .keys = &popup_keys
    };
  
    menu_t *menus[] = {
    &settings_menu,
    &config_menu,
    &profile_menu,
    &units_menu,
    
    };
    
    size_t num_menus = numelements(menus);

    menu_item_t *menu_items[] = {
    (menu_item_t *) &settings_menu_item_1,
      (menu_item_t *) &settings_menu_item_2,
      (menu_item_t *) &config_menu_item_1,
      (menu_item_t *) &config_menu_item_2,
      (menu_item_t *) &profile_menu_item_1,
      (menu_item_t *) &profile_menu_item_2,
      (menu_item_t *) &units_menu_item_1,
      (menu_item_t *) &units_menu_item_2,
      
    };

    size_t num_menu_items = numelements(menu_items);
  
    static menu_widget_t menu_wnd = {
    .base.style =
    
    BORDER_NONE,
    .base.border_color = color_yellow,
    .base.background_color = color_black,
    .base.status_timeout = 10000,
      .base.sensor_failed = false,
      .base.alarm_color = color_red,
      .base.rect.left = 0,
    .base.rect.top = 0,
    .base.rect.right = 0 + 320,
    .base.rect.bottom = 0 + 240,
  
    .menu_rect.x = 0,
    .menu_rect.y = 0,
    .menu_start.x = 0,
    .menu_start.y = 240,
    .selected_background_color = color_white,
    .text_color = color_green,
    .selected_color = color_magenta,
    .menu_timeout = 25,
    .font = &neo_9_font,
  .root_keys = &root,
    .alarm_keys = &alarm_keys,
    };
  
          static airspeed_widget_t asi_widget = {
          .base.style =
    
      BORDER_BOTTOM |
    
    BORDER_NONE,
    .base.border_color = color_white,
    .base.name = "ASI",
    .base.status_timeout = 10000,
      .base.sensor_failed = false,
      .base.alarm_color = color_red,
      .base.rect.left = 0,
    .base.rect.top = 0,
    .base.rect.right = 0 + 60,
    .base.rect.bottom = 0 + 240,
  
          .text_color = color_white,
          .pen = color_white,
          .font = &neo_9_font,
          .large_roller = &neo_15_font,
          .small_roller = &neo_9_font,
          
              .pixels_per_unit = 10,
            
          };
        
          static attitude_widget_t att_widget ={
          .base.style =
    
      BORDER_BOTTOM |
    
    BORDER_NONE,
    .base.border_color = color_white,
    .base.name = "ATT",
    .base.status_timeout = 10000,
      .base.sensor_failed = false,
      .base.alarm_color = color_red,
      .base.rect.left = 60,
    .base.rect.top = 0,
    .base.rect.right = 60 + 240,
    .base.rect.bottom = 0 + 240,
  
          .median.x = 120,
          .median.y = 120,
          
          };
        
          static altitude_widget_t alt_widget = {
          .base.style =
    
      BORDER_BOTTOM |
    
    BORDER_NONE,
    .base.border_color = color_white,
    .base.name = "ALT",
    .base.status_timeout = 10000,
      .base.sensor_failed = false,
      .base.alarm_color = color_red,
      .base.rect.left = 300,
    .base.rect.top = 0,
    .base.rect.right = 300 + 80,
    .base.rect.bottom = 0 + 240,
  
          .text_color = color_white,
          .pen = color_white,
          .font = &neo_9_font,
          .large_roller = &neo_15_font,
          .small_roller = &neo_9_font,
          };
        
          static hsi_widget_t hsi_widget = {
        .base.style =
    
      BORDER_LEFT |
    
      BORDER_RIGHT |
    
      BORDER_BOTTOM |
    
    BORDER_NONE,
    .base.border_color = color_white,
    .base.name = "HSI",
    .base.name_font  = &neo_9_font,
    .base.on_create = on_create_hsi_widget,
    .base.status_timeout = 10000,
      .base.sensor_failed = false,
      .base.alarm_color = color_red,
      .base.rect.left = 60,
    .base.rect.top = 240,
    .base.rect.right = 60 + 240,
    .base.rect.bottom = 240 + 240,
  
        };
      
          static ap_widget_t ap_widget ={
          .base.style =
    
      BORDER_BOTTOM |
    
    BORDER_NONE,
    .base.border_color = color_white,
    .base.name = "AP",
    .base.name_font  = &neo_9_font,
    .base.on_create = on_create_autopilot_widget,
    .base.status_timeout = 10000,
      .base.sensor_failed = false,
      .base.alarm_color = color_red,
      .base.rect.left = 300,
    .base.rect.top = 240,
    .base.rect.right = 300 + 80,
    .base.rect.bottom = 240 + 240,
  
          .button_height = 18,
          .height = 15,
          .gutter = 2,
          .info_color = color_yellow,
          .active_color = color_purple,
          .standby_color = color_green,
          };
        
            static const step_t map_gauge_steps[] = {
            
              {
              .value = 5,
              .pen = color_lightblue,
              .color = color_hollow,
              },
            
              {
              .value = 31,
              .pen = color_lightblue,
              .color = color_green,
              },
            
            };
          
            static const tick_mark_t map_gauge_ticks[] = {
            
              {
              .value = 10,
              
                .text = "10",
              
              },
            
              {
              .value = 15,
              
              },
            
              {
              .value = 20,
              
                .text = "20",
              
              },
            
              {
              .value = 25,
              
              },
            
              {
              .value = 30,
              
                .text = "30",
              
              },
            
            };
          
          static gauge_widget_t map_gauge ={
          .base.style =
    
      BORDER_RIGHT |
    
      BORDER_BOTTOM |
    
      DRAW_NAME |
    
      DRAW_BACKGROUND |
    
    BORDER_NONE,
    .base.border_color = color_white,
    .base.name_color = color_white,
    .base.name = "MAP",
    .base.name_font  = &neo_9_font,
    .base.status_timeout = 10000,
      .base.sensor_id = id_manifold_pressure_sensor_status,
        .base.sensor_failed = true,
      .base.alarm_id = id_map_divergence_alarm,
    .base.alarm_color = color_red,
      .base.rect.left = 0,
    .base.rect.top = 480,
    .base.rect.right = 0 + 128,
    .base.rect.bottom = 480 + 67,
  
              .style = gs_bar,
            
          .font = &neo_9_font,
          .can_id = id_manifold_pressure,
          
            .gauge_radii = 75,
          
            .arc_width = 5,
          
            .bar_width = 5,
          
            .arc_begin = 230,
          
            .arc_range = 80,
          
            .draw_value = true,
          
            .value_font = &neo_18_font,
          
            .converter = &to_display_map,
          
            .base.name_pt.x = 55,
            .base.name_pt.y = 35,
          
            .center.x = 55,
            .center.y = 90,
          
            .value_rect.left = 15,
            .value_rect.top = 40,
            .value_rect.right = 15 + 80,
            .value_rect.bottom = 40 + 26,
          
            .steps = map_gauge_steps,
            .num_steps = numelements(map_gauge_steps),
          
            .ticks = map_gauge_ticks,
            .num_ticks = numelements(map_gauge_ticks),
          

          };
        
            static const step_t rpm_gauge_steps[] = {
            
              {
              .value = 500,
              .pen = color_lightblue,
              .color = color_red,
              },
            
              {
              .value = 2750,
              .pen = color_lightblue,
              .color = color_green,
              },
            
              {
              .value = 3100,
              .pen = color_lightblue,
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
              .value = 1000,
              
                .text = "1",
              
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
              .value = 2000,
              
                .text = "2",
              
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
              .value = 3000,
              
                .text = "3",
              
              },
            
            };
          
          static gauge_widget_t rpm_gauge ={
          .base.style =
    
      BORDER_RIGHT |
    
      BORDER_BOTTOM |
    
      DRAW_NAME |
    
      DRAW_BACKGROUND |
    
    BORDER_NONE,
    .base.border_color = color_white,
    .base.name_color = color_white,
    .base.name = "RPM",
    .base.name_font  = &neo_9_font,
    .base.status_timeout = 10000,
      .base.sensor_failed = false,
      .base.alarm_id = id_rpm_high_alarm,
    .base.alarm_color = color_red,
      .base.rect.left = 128,
    .base.rect.top = 480,
    .base.rect.right = 128 + 128,
    .base.rect.bottom = 480 + 67,
  
              .style = gs_bar,
            
          .font = &neo_9_font,
          .can_id = id_engine_rpm,
          
            .gauge_radii = 75,
          
            .arc_width = 5,
          
            .bar_width = 5,
          
            .arc_begin = 230,
          
            .arc_range = 80,
          
            .draw_value = true,
          
            .value_font = &neo_18_font,
          
            .base.name_pt.x = 55,
            .base.name_pt.y = 35,
          
            .center.x = 55,
            .center.y = 90,
          
            .value_rect.left = 15,
            .value_rect.top = 40,
            .value_rect.right = 15 + 80,
            .value_rect.bottom = 40 + 26,
          
            .steps = rpm_gauge_steps,
            .num_steps = numelements(rpm_gauge_steps),
          
            .ticks = rpm_gauge_ticks,
            .num_ticks = numelements(rpm_gauge_ticks),
          

          };
        
          static hp_annunciator_t hp_annunciator ={
          .base.base.base.style =
    
      BORDER_RIGHT |
    
      BORDER_BOTTOM |
    
      DRAW_NAME |
    
      DRAW_BACKGROUND |
    
    BORDER_NONE,
    .base.base.base.border_color = color_white,
    .base.base.base.background_color = color_blue,
    .base.base.base.name_color = color_white,
    .base.base.base.name = "HP",
    .base.base.base.name_font  = &neo_9_font,
    .base.base.base.status_timeout = 10000,
      .base.base.base.sensor_failed = false,
      .base.base.base.alarm_color = color_red,
      .base.base.base.rect.left = 0,
    .base.base.base.rect.top = 547,
    .base.base.base.rect.right = 0 + 60,
    .base.base.base.rect.bottom = 547 + 20,
  .base.can_id = id_engine_hp,
    .base.label_color = color_white,
    .base.text_color  = color_white,
    .base.small_font = &neo_9_font,
    .base.label_offset = 2,
    .base.text_offset = 30,
  
          .base.base.base.on_message = on_hp_msg,
          .base.base.base.on_paint_foreground = on_paint_hp,
          .base.base.base.on_paint_background = on_paint_background_auto,
          };
        
          static ecu_annunciator_t kmag_annunciator ={
          .base.base.style =
    
      BORDER_RIGHT |
    
      BORDER_BOTTOM |
    
      DRAW_BACKGROUND |
    
    BORDER_NONE,
    .base.base.border_color = color_white,
    .base.base.background_color = color_black,
    .base.base.name_color = color_white,
    .base.base.name = "kMAG",
    .base.base.status_timeout = 10000,
      .base.base.sensor_failed = false,
      .base.base.alarm_color = color_red,
      .base.base.rect.left = 60,
    .base.base.rect.top = 547,
    .base.base.rect.right = 60 + 136,
    .base.base.rect.bottom = 547 + 20,
  
          .base.base.on_message = on_kmag_msg,
          .base.base.on_paint_foreground = on_paint_kmag,
          };
        
          static auto_annunciator_t oat_annunciator ={
          .base.base.base.style =
    
      BORDER_RIGHT |
    
      BORDER_BOTTOM |
    
      DRAW_NAME |
    
      DRAW_BACKGROUND |
    
    BORDER_NONE,
    .base.base.base.border_color = color_white,
    .base.base.base.background_color = color_blue,
    .base.base.base.name = "OAT",
    .base.base.base.name_font  = &neo_9_font,
    .base.base.base.on_paint_foreground = on_paint_auto,
    .base.base.base.on_message = on_auto_msg,
    .base.base.base.status_timeout = 10000,
      .base.base.base.sensor_failed = false,
      .base.base.base.alarm_color = color_red,
      .base.base.base.rect.left = 196,
    .base.base.base.rect.top = 547,
    .base.base.base.rect.right = 196 + 60,
    .base.base.base.rect.bottom = 547 + 20,
  .base.can_id = id_outside_air_temperature,
    .base.label_color = color_white,
    .base.text_color  = color_white,
    .base.small_font = &neo_9_font,
    .base.label_offset = 2,
    .base.text_offset = 30,
  
            .converter = &to_display_temperature,
          
          .value_type = v_uint16,
          .fmt = "%d",
          .base.base.base.on_message = on_auto_msg,
          .base.base.base.on_paint_foreground = on_paint_auto,
          .base.base.base.on_paint_background = on_paint_background_auto,
          };
        
          static edu_temp_widget_t edu_temps ={
          .base.style =
    
      BORDER_RIGHT |
    
      BORDER_BOTTOM |
    
    BORDER_NONE,
    .base.border_color = color_white,
    .base.name = "TEMPS",
    .base.status_timeout = 10000,
      .base.sensor_failed = false,
      .base.alarm_color = color_red,
      .base.rect.left = 256,
    .base.rect.top = 480,
    .base.rect.right = 256 + 256,
    .base.rect.bottom = 480 + 120,
  
          .num_cylinders = 4,
          .cht_red_line = 505,
          .egt_red_line = 1173,
          .cht_min = 363,
          .egt_min = 526,
          .left_gutter = 24,
          .right_gutter = 196,
          .cylinder_draw_width = 40,
          .egt_line = 5,
          .cht_line = 17,
          .cht_red_line_pos = 68,
          .bar_top = 31,
          .bar_bottom = 104,
          .status_top = 102,
          .font = &neo_9_font,
          .cht_color= color_lightblue,
          .egt_color= color_lightgreen,
          
          .cht[1-1].can_id = id_cylinder_head_temperature1,
    .cht[1-1].default_color = color_lightgreen,
    .cht[1-1].alarm_id = id_cylinder_head_temperature1_alarm,
    .cht[1-1].alarm_color = color_red,
    .cht[1-1].bar_name = "1",
    .cht[1-1].sensor_id = id_cylinder_head_temperature1_status,
    .cht[1-1].value = 273,
  .cht[2-1].can_id = id_cylinder_head_temperature2,
    .cht[2-1].default_color = color_lightgreen,
    .cht[2-1].alarm_id = id_cylinder_head_temperature2_alarm,
    .cht[2-1].alarm_color = color_red,
    .cht[2-1].bar_name = "2",
    .cht[2-1].sensor_id = id_cylinder_head_temperature2_status,
    .cht[2-1].value = 273,
  .cht[3-1].can_id = id_cylinder_head_temperature3,
    .cht[3-1].default_color = color_lightgreen,
    .cht[3-1].alarm_id = id_cylinder_head_temperature3_alarm,
    .cht[3-1].alarm_color = color_red,
    .cht[3-1].bar_name = "3",
    .cht[3-1].sensor_id = id_cylinder_head_temperature3_status,
    .cht[3-1].value = 273,
  .cht[4-1].can_id = id_cylinder_head_temperature1,
    .cht[4-1].default_color = color_lightgreen,
    .cht[4-1].alarm_id = id_cylinder_head_temperature4_alarm,
    .cht[4-1].alarm_color = color_red,
    .cht[4-1].bar_name = "4",
    .cht[4-1].sensor_id = id_cylinder_head_temperature4_status,
    .cht[4-1].value = 273,
  .egt[1-1].can_id = id_exhaust_gas_temperature1,
    .egt[1-1].default_color = color_lightblue,
    .egt[1-1].alarm_id = id_exhaust_gas_temperature1_alarm,
    .egt[1-1].alarm_color = color_red,
    .egt[1-1].bar_name = "1",
    .egt[1-1].sensor_id = id_exhaust_gas_temperature1_status,
    .egt[1-1].value = 273,
  .egt[2-1].can_id = id_exhaust_gas_temperature2,
    .egt[2-1].default_color = color_lightblue,
    .egt[2-1].alarm_id = id_exhaust_gas_temperature2_alarm,
    .egt[2-1].alarm_color = color_red,
    .egt[2-1].bar_name = "2",
    .egt[2-1].sensor_id = id_exhaust_gas_temperature2_status,
    .egt[2-1].value = 273,
  .egt[3-1].can_id = id_exhaust_gas_temperature3,
    .egt[3-1].default_color = color_lightblue,
    .egt[3-1].alarm_id = id_exhaust_gas_temperature3_alarm,
    .egt[3-1].alarm_color = color_red,
    .egt[3-1].bar_name = "3",
    .egt[3-1].sensor_id = id_exhaust_gas_temperature3_status,
    .egt[3-1].value = 273,
  .egt[4-1].can_id = id_exhaust_gas_temperature4,
    .egt[4-1].default_color = color_lightblue,
    .egt[4-1].alarm_id = id_exhaust_gas_temperature4_alarm,
    .egt[4-1].alarm_color = color_red,
    .egt[4-1].bar_name = "4",
    .egt[4-1].sensor_id = id_exhaust_gas_temperature4_status,
    .egt[4-1].value = 273,
  
          };
         
            static const step_t oilt_gauge_steps[] = {
            
              {
              .value = 273,
              .pen = color_hollow,
              .color = color_hollow,
              },
            
              {
              .value = 305,
              .pen = color_red,
              .color = color_red,
              },
            
              {
              .value = 383,
              .pen = color_lightgreen,
              .color = color_lightgreen,
              },
            
              {
              .value = 393,
              .pen = color_red,
              .color = color_red,
              },
            
            };
          
          static gauge_widget_t oilt_gauge ={
          .base.style =
    
      DRAW_NAME |
    
    BORDER_NONE,
    .base.name_color = color_white,
    .base.name = "OILT",
    .base.name_font  = &neo_9_font,
    .base.status_timeout = 10000,
      .base.sensor_id = id_oil_temperature_sensor_status,
        .base.sensor_failed = true,
      .base.alarm_id = id_high_oil_temperature_alarm,
    .base.alarm_color =  color_red,
      .base.rect.left = 512,
    .base.rect.top = 480,
    .base.rect.right = 512 + 128,
    .base.rect.bottom = 480 + 40,
  
              .style = gs_hbar,
            
          .font = &neo_12_font,
          .can_id = id_oil_temperature,
          
            .draw_value = true,
          
            .value_font = &neo_12_font,
          
            .converter = &to_display_temperature,
          
            .base.name_pt.x = 55,
            .base.name_pt.y = 30,
          
            .value_rect.left = 3,
            .value_rect.top = 20,
            .value_rect.right = 3 + 17,
            .value_rect.bottom = 20 + 19,
          
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
              .value = 1379,
              .pen = color_red,
              .color = color_red,
              },
            
              {
              .value = 6895,
              .pen = color_lightgreen,
              .color = color_lightgreen,
              },
            
            };
          
          static gauge_widget_t oilp_gauge ={
          .base.style =
    
      BORDER_BOTTOM |
    
      DRAW_NAME |
    
    BORDER_NONE,
    .base.border_color = color_white,
    .base.name_color = color_white,
    .base.name = "OILP",
    .base.name_font  = &neo_9_font,
    .base.status_timeout = 10000,
      .base.sensor_id = id_oil_pressure_sensor_status,
        .base.sensor_failed = true,
      .base.alarm_id = id_low_oil_pressure_alarm,
    .base.alarm_color =  color_red,
      .base.rect.left = 512,
    .base.rect.top = 520,
    .base.rect.right = 512 + 128,
    .base.rect.bottom = 520 + 40,
  
              .style = gs_hbar,
            
          .font = &neo_12_font,
          .can_id = id_oil_pressure,
          
            .draw_value = true,
          
            .value_font = &neo_12_font,
          
            .converter = &to_display_pressure,
          
            .base.name_pt.x = 55,
            .base.name_pt.y = 30,
          
            .value_rect.left = 3,
            .value_rect.top = 20,
            .value_rect.right = 3 + 17,
            .value_rect.bottom = 20 + 19,
          
            .steps = oilp_gauge_steps,
            .num_steps = numelements(oilp_gauge_steps),
          

          };
        
            static const step_t volts_gauge_steps[] = {
            
              {
              .value = 0,
              .pen = color_hollow,
              .color = color_hollow,
              },
            
              {
              .value = 9,
              .pen = color_red,
              .color = color_red,
              },
            
              {
              .value = 15,
              .pen = color_lightgreen,
              .color = color_lightgreen,
              },
            
              {
              .value = 20,
              .pen = color_red,
              .color = color_red,
              },
            
            };
          
          static gauge_widget_t volts_gauge ={
          .base.style =
    
      BORDER_BOTTOM |
    
      DRAW_NAME |
    
    BORDER_NONE,
    .base.border_color = color_white,
    .base.name_color = color_white,
    .base.name = "VOLTS",
    .base.name_font  = &neo_9_font,
    .base.status_timeout = 10000,
      .base.sensor_id = id_volts_sensor_status,
        .base.sensor_failed = true,
      .base.alarm_id = id_bus_volts_high_alarm,
    .base.alarm_color =  color_red,
      .base.rect.left = 512,
    .base.rect.top = 560,
    .base.rect.right = 512 + 128,
    .base.rect.bottom = 560 + 40,
  
              .style = gs_hbar,
            
          .font = &neo_12_font,
          .can_id = id_dc_voltage,
          
            .draw_value = true,
          
            .value_font = &neo_12_font,
          
            .base.name_pt.x = 55,
            .base.name_pt.y = 30,
          
            .value_rect.left = 3,
            .value_rect.top = 20,
            .value_rect.right = 3 + 17,
            .value_rect.bottom = 20 + 19,
          
            .steps = volts_gauge_steps,
            .num_steps = numelements(volts_gauge_steps),
          

          };
        
            static const step_t fuelp_gauge_steps[] = {
            
              {
              .value = 689,
              .pen = color_hollow,
              .color = color_hollow,
              },
            
              {
              .value = 1103,
              .pen = color_red,
              .color = color_red,
              },
            
              {
              .value = 2206,
              .pen = color_lightgreen,
              .color = color_lightgreen,
              },
            
            };
          
          static gauge_widget_t fuelp_gauge ={
          .base.style =
    
      DRAW_NAME |
    
    BORDER_NONE,
    .base.name_color = color_white,
    .base.name = "FUELP",
    .base.name_font  = &neo_9_font,
    .base.status_timeout = 10000,
      .base.sensor_id = id_fuel_pressure_sensor_status,
        .base.sensor_failed = true,
      .base.alarm_id = id_low_fuel_pressure_alarm,
    .base.alarm_color =  color_red,
      .base.rect.left = 640,
    .base.rect.top = 480,
    .base.rect.right = 640 + 128,
    .base.rect.bottom = 480 + 40,
  
              .style = gs_hbar,
            
          .font = &neo_12_font,
          .can_id = id_fuel_pressure,
          
            .draw_value = true,
          
            .value_font = &neo_12_font,
          
            .converter = &to_display_pressure,
          
            .base.name_pt.x = 55,
            .base.name_pt.y = 30,
          
            .value_rect.left = 3,
            .value_rect.top = 20,
            .value_rect.right = 3 + 17,
            .value_rect.bottom = 20 + 19,
          
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
              .value = 45,
              .pen = color_lightgreen,
              .color = color_lightgreen,
              },
            
            };
          
          static gauge_widget_t fuelf_gauge ={
          .base.style =
    
      BORDER_BOTTOM |
    
      DRAW_NAME |
    
    BORDER_NONE,
    .base.border_color = color_white,
    .base.name_color = color_white,
    .base.name = "FUELF",
    .base.name_font  = &neo_9_font,
    .base.status_timeout = 10000,
      .base.sensor_id = id_fuel_flow_sensor_status,
        .base.sensor_failed = true,
      .base.alarm_color = color_red,
      .base.rect.left = 640,
    .base.rect.top = 520,
    .base.rect.right = 640 + 128,
    .base.rect.bottom = 520 + 40,
  
              .style = gs_hbar,
            
          .font = &neo_12_font,
          .can_id = id_fuel_flow_rate,
          
            .draw_value = true,
          
            .value_font = &neo_12_font,
          
            .converter = &to_display_flow,
          
            .base.name_pt.x = 55,
            .base.name_pt.y = 30,
          
            .value_rect.left = 3,
            .value_rect.top = 20,
            .value_rect.right = 3 + 17,
            .value_rect.bottom = 20 + 19,
          
            .steps = fuelf_gauge_steps,
            .num_steps = numelements(fuelf_gauge_steps),
          

          };
        
            static const step_t amps_gauge_steps[] = {
            
              {
              .value = -30,
              .pen = color_hollow,
              .color = color_hollow,
              },
            
              {
              .value = -20,
              .pen = color_red,
              .color = color_red,
              },
            
              {
              .value = 20,
              .pen = color_lightgreen,
              .color = color_lightgreen,
              },
            
              {
              .value = 30,
              .pen = color_red,
              .color = color_red,
              },
            
            };
          
          static gauge_widget_t amps_gauge ={
          .base.style =
    
      BORDER_BOTTOM |
    
      DRAW_NAME |
    
    BORDER_NONE,
    .base.border_color = color_white,
    .base.name_color = color_white,
    .base.name = "AMPS",
    .base.name_font  = &neo_9_font,
    .base.status_timeout = 10000,
      .base.sensor_id = id_amps_sensor_status,
        .base.sensor_failed = true,
      .base.alarm_id = id_high_amps_alarm,
    .base.alarm_color =  color_red,
      .base.rect.left = 640,
    .base.rect.top = 560,
    .base.rect.right = 640 + 128,
    .base.rect.bottom = 560 + 40,
  
              .style = gs_hbar,
            
          .font = &neo_12_font,
          .can_id = id_dc_current,
          
            .draw_value = true,
          
            .value_font = &neo_12_font,
          
            .base.name_pt.x = 55,
            .base.name_pt.y = 30,
          
            .value_rect.left = 3,
            .value_rect.top = 20,
            .value_rect.right = 3 + 17,
            .value_rect.bottom = 20 + 19,
          
            .steps = amps_gauge_steps,
            .num_steps = numelements(amps_gauge_steps),
          

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
              .value = 80,
              .pen = color_lightgreen,
              .color = color_lightgreen,
              },
            
            };
          
          static gauge_widget_t left_fuel_gauge ={
          .base.style =
    
      DRAW_NAME |
    
    BORDER_NONE,
    .base.border_color = color_white,
    .base.name_color = color_white,
    .base.name = "LEFT",
    .base.name_font  = &neo_9_font,
    .base.status_timeout = 10000,
      .base.sensor_id = id_left_fuel_quantity_sensor_status,
        .base.sensor_failed = true,
      .base.alarm_color = color_red,
      .base.rect.left = 768,
    .base.rect.top = 480,
    .base.rect.right = 768 + 128,
    .base.rect.bottom = 480 + 40,
  
              .style = gs_hbar,
            
          .font = &neo_12_font,
          .can_id = id_left_fuel_quantity,
          
            .draw_value = true,
          
            .value_font = &neo_12_font,
          
            .converter = &to_display_volume,
          
            .base.name_pt.x = 55,
            .base.name_pt.y = 30,
          
            .value_rect.left = 3,
            .value_rect.top = 20,
            .value_rect.right = 3 + 17,
            .value_rect.bottom = 20 + 19,
          
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
              .value = 80,
              .pen = color_lightgreen,
              .color = color_lightgreen,
              },
            
            };
          
          static gauge_widget_t right_fuel_gauge ={
          .base.style =
    
      DRAW_NAME |
    
    BORDER_NONE,
    .base.border_color = color_white,
    .base.name_color = color_white,
    .base.name = "RIGHT",
    .base.name_font  = &neo_9_font,
    .base.status_timeout = 10000,
      .base.sensor_id = id_right_fuel_quantity_sensor_status,
        .base.sensor_failed = true,
      .base.alarm_color = color_red,
      .base.rect.left = 768,
    .base.rect.top = 520,
    .base.rect.right = 768 + 128,
    .base.rect.bottom = 520 + 40,
  
              .style = gs_hbar,
            
          .font = &neo_12_font,
          .can_id = id_right_fuel_quantity,
          
            .draw_value = true,
          
            .value_font = &neo_12_font,
          
            .converter = &to_display_volume,
          
            .base.name_pt.x = 55,
            .base.name_pt.y = 30,
          
            .value_rect.left = 3,
            .value_rect.top = 20,
            .value_rect.right = 3 + 17,
            .value_rect.bottom = 20 + 19,
          
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
              .value = 5,
              .pen = color_red,
              .color = color_red,
              },
            
              {
              .value = 160,
              .pen = color_lightgreen,
              .color = color_lightgreen,
              },
            
            };
          
          static gauge_widget_t fuelt_gauge ={
          .base.style =
    
      DRAW_NAME |
    
    BORDER_NONE,
    .base.border_color = color_white,
    .base.name_color = color_white,
    .base.name = "FUELT",
    .base.name_font  = &neo_9_font,
    .base.status_timeout = 10000,
      .base.sensor_id = id_right_fuel_quantity_sensor_status,
        .base.sensor_failed = true,
      .base.alarm_color = color_red,
      .base.rect.left = 768,
    .base.rect.top = 560,
    .base.rect.right = 768 + 128,
    .base.rect.bottom = 560 + 40,
  
              .style = gs_hbar,
            
          .font = &neo_12_font,
          .can_id = id_fuel_total,
          
            .draw_value = true,
          
            .value_font = &neo_12_font,
          
            .converter = &to_display_flow,
          
            .base.name_pt.x = 55,
            .base.name_pt.y = 30,
          
            .value_rect.left = 3,
            .value_rect.top = 20,
            .value_rect.right = 3 + 17,
            .value_rect.bottom = 20 + 19,
          
            .steps = fuelt_gauge_steps,
            .num_steps = numelements(fuelt_gauge_steps),
          

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
          
          static gauge_widget_t flap_gauge ={
          .base.style =
    
      DRAW_NAME |
    
      DRAW_BACKGROUND |
    
    BORDER_NONE,
    .base.background_color = color_black,
    .base.name_color = color_white,
    .base.name = "FLAP",
    .base.name_font  = &neo_9_font,
    .base.status_timeout = 10000,
      .base.sensor_failed = false,
      .base.alarm_color = color_red,
      .base.rect.left = 936,
    .base.rect.top = 550,
    .base.rect.right = 936 + 88,
    .base.rect.bottom = 550 + 50,
  
              .style = gs_bar,
            
          .font = &neo_9_font,
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
        
          static pancake_widget_t pitch_indicator ={
          .base.style =
    
    BORDER_NONE,
    .base.name = "PITCH",
    .base.status_timeout = 10000,
      .base.sensor_failed = false,
      .base.alarm_color = color_red,
      .base.rect.left = 896,
    .base.rect.top = 480,
    .base.rect.right = 896 + 40,
    .base.rect.bottom = 480 + 120,
  
          .indicator_rect.left = 5,
          .indicator_rect.top = 5,
          .indicator_rect.right = 5 + 30,
          .indicator_rect.bottom = 5 + 110,
          .can_id = id_pitch_trim_value,
          .outline_color = color_white,
          .indicator_color = color_lightblue,
          .bar_width = 26,
          .bar_height = 9,
          .bar_gutter = 2,
          .min_value = -100,
          .max_value = 100,
          
          .horizontal = false,
          
          };
        
          static pancake_widget_t roll_indicator ={
          .base.style =
    
    BORDER_NONE,
    .base.name = "ROLL",
    .base.status_timeout = 10000,
      .base.sensor_failed = false,
      .base.alarm_color = color_red,
      .base.rect.left = 936,
    .base.rect.top = 518,
    .base.rect.right = 936 + 88,
    .base.rect.bottom = 518 + 32,
  
          .indicator_rect.left = 1,
          .indicator_rect.top = 1,
          .indicator_rect.right = 1 + 86,
          .indicator_rect.bottom = 1 + 24,
          .can_id = id_roll_trim_value,
          .outline_color = color_white,
          .indicator_color = color_lightblue,
          .bar_width = 22,
          .bar_height = 7,
          .bar_gutter = 1,
          .min_value = -100,
          .max_value = 100,
          
          .horizontal = true,
          
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
          

          static alarm_t marquee_alarms[] = {
          
            {
            .can_ids = marquee_alarm_ids_1,
            .num_ids = numelements(marquee_alarm_ids_1),
            .priority = 1,
            .can_park = true,
            .message = "Low oil pressure",
            .base.style = DRAW_BACKGROUND,
            .base.background_color = color_red,
            .base.name_color = color_black,
            .base.rect.left = 1,
            .base.rect.top = 1,
            .base.rect.right = 256 -2,
            .base.rect.bottom = 53 -2,
            .base.name_font = &neo_15_font,
            .base.on_paint_foreground = on_paint_alarm_foreground,
            },
          
            {
            .can_ids = marquee_alarm_ids_2,
            .num_ids = numelements(marquee_alarm_ids_2),
            .priority = 2,
            .can_park = true,
            .message = "Low oil pressure",
            .base.style = DRAW_BACKGROUND,
            .base.background_color = color_red,
            .base.name_color = color_black,
            .base.rect.left = 1,
            .base.rect.top = 1,
            .base.rect.right = 256 -2,
            .base.rect.bottom = 53 -2,
            .base.name_font = &neo_15_font,
            .base.on_paint_foreground = on_paint_alarm_foreground,
            },
          
            {
            .can_ids = marquee_alarm_ids_3,
            .num_ids = numelements(marquee_alarm_ids_3),
            .priority = 3,
            .can_park = true,
            .message = "High oil temperature",
            .base.style = DRAW_BACKGROUND,
            .base.background_color = color_red,
            .base.name_color = color_black,
            .base.rect.left = 1,
            .base.rect.top = 1,
            .base.rect.right = 256 -2,
            .base.rect.bottom = 53 -2,
            .base.name_font = &neo_15_font,
            .base.on_paint_foreground = on_paint_alarm_foreground,
            },
          
            {
            .can_ids = marquee_alarm_ids_4,
            .num_ids = numelements(marquee_alarm_ids_4),
            .priority = 5,
            .can_park = true,
            .message = "Timing divergence",
            .base.style = DRAW_BACKGROUND,
            .base.background_color = color_red,
            .base.name_color = color_black,
            .base.rect.left = 1,
            .base.rect.top = 1,
            .base.rect.right = 256 -2,
            .base.rect.bottom = 53 -2,
            .base.name_font = &neo_15_font,
            .base.on_paint_foreground = on_paint_alarm_foreground,
            },
          
            {
            .can_ids = marquee_alarm_ids_5,
            .num_ids = numelements(marquee_alarm_ids_5),
            .priority = 6,
            .can_park = true,
            .message = "High CHT",
            .base.style = DRAW_BACKGROUND,
            .base.background_color = color_red,
            .base.name_color = color_black,
            .base.rect.left = 1,
            .base.rect.top = 1,
            .base.rect.right = 256 -2,
            .base.rect.bottom = 53 -2,
            .base.name_font = &neo_15_font,
            .base.on_paint_foreground = on_paint_alarm_foreground,
            },
          
            {
            .can_ids = marquee_alarm_ids_6,
            .num_ids = numelements(marquee_alarm_ids_6),
            .priority = 7,
            .can_park = true,
            .message = "Low Fuel",
            .base.style = DRAW_BACKGROUND,
            .base.background_color = color_red,
            .base.name_color = color_black,
            .base.rect.left = 1,
            .base.rect.top = 1,
            .base.rect.right = 256 -2,
            .base.rect.bottom = 53 -2,
            .base.name_font = &neo_15_font,
            .base.on_paint_foreground = on_paint_alarm_foreground,
            },
          
            {
            .can_ids = marquee_alarm_ids_7,
            .num_ids = numelements(marquee_alarm_ids_7),
            .priority = 8,
            .can_park = true,
            .message = "CHT Cold Shock",
            .base.style = DRAW_BACKGROUND,
            .base.background_color = color_red,
            .base.name_color = color_black,
            .base.rect.left = 1,
            .base.rect.top = 1,
            .base.rect.right = 256 -2,
            .base.rect.bottom = 53 -2,
            .base.name_font = &neo_15_font,
            .base.on_paint_foreground = on_paint_alarm_foreground,
            },
          
            {
            .can_ids = marquee_alarm_ids_8,
            .num_ids = numelements(marquee_alarm_ids_8),
            .priority = 9,
            .can_park = true,
            .message = "High voltage",
            .base.style = DRAW_BACKGROUND,
            .base.background_color = color_red,
            .base.name_color = color_black,
            .base.rect.left = 1,
            .base.rect.top = 1,
            .base.rect.right = 256 -2,
            .base.rect.bottom = 53 -2,
            .base.name_font = &neo_15_font,
            .base.on_paint_foreground = on_paint_alarm_foreground,
            },
          
            {
            .can_ids = marquee_alarm_ids_9,
            .num_ids = numelements(marquee_alarm_ids_9),
            .priority = 10,
            .can_park = true,
            .message = "Low Voltage",
            .base.style = DRAW_BACKGROUND,
            .base.background_color = color_red,
            .base.name_color = color_black,
            .base.rect.left = 1,
            .base.rect.top = 1,
            .base.rect.right = 256 -2,
            .base.rect.bottom = 53 -2,
            .base.name_font = &neo_15_font,
            .base.on_paint_foreground = on_paint_alarm_foreground,
            },
          
            {
            .can_ids = marquee_alarm_ids_10,
            .num_ids = numelements(marquee_alarm_ids_10),
            .priority = 11,
            .can_park = true,
            .message = "High Fuel Pressure",
            .base.style = DRAW_BACKGROUND,
            .base.background_color = color_red,
            .base.name_color = color_black,
            .base.rect.left = 1,
            .base.rect.top = 1,
            .base.rect.right = 256 -2,
            .base.rect.bottom = 53 -2,
            .base.name_font = &neo_15_font,
            .base.on_paint_foreground = on_paint_alarm_foreground,
            },
          
            {
            .can_ids = marquee_alarm_ids_11,
            .num_ids = numelements(marquee_alarm_ids_11),
            .priority = 12,
            .can_park = true,
            .message = "High Current",
            .base.style = DRAW_BACKGROUND,
            .base.background_color = color_red,
            .base.name_color = color_black,
            .base.rect.left = 1,
            .base.rect.top = 1,
            .base.rect.right = 256 -2,
            .base.rect.bottom = 53 -2,
            .base.name_font = &neo_15_font,
            .base.on_paint_foreground = on_paint_alarm_foreground,
            },
          
            {
            .can_ids = marquee_alarm_ids_12,
            .num_ids = numelements(marquee_alarm_ids_12),
            .priority = 13,
            .can_park = true,
            .message = "kMAG Temperature",
            .base.style = DRAW_BACKGROUND,
            .base.background_color = color_red,
            .base.name_color = color_black,
            .base.rect.left = 1,
            .base.rect.top = 1,
            .base.rect.right = 256 -2,
            .base.rect.bottom = 53 -2,
            .base.name_font = &neo_15_font,
            .base.on_paint_foreground = on_paint_alarm_foreground,
            },
          
            {
            .can_ids = marquee_alarm_ids_13,
            .num_ids = numelements(marquee_alarm_ids_13),
            .priority = 14,
            .can_park = true,
            .message = "Low Fuel Flow",
            .base.style = DRAW_BACKGROUND,
            .base.background_color = color_red,
            .base.name_color = color_black,
            .base.rect.left = 1,
            .base.rect.top = 1,
            .base.rect.right = 256 -2,
            .base.rect.bottom = 53 -2,
            .base.name_font = &neo_15_font,
            .base.on_paint_foreground = on_paint_alarm_foreground,
            },
          
            {
            .can_ids = marquee_alarm_ids_14,
            .num_ids = numelements(marquee_alarm_ids_14),
            .priority = 14,
            .can_park = true,
            .message = "EGT Temperature",
            .base.style = DRAW_BACKGROUND,
            .base.background_color = color_red,
            .base.name_color = color_black,
            .base.rect.left = 1,
            .base.rect.top = 1,
            .base.rect.right = 256 -2,
            .base.rect.bottom = 53 -2,
            .base.name_font = &neo_15_font,
            .base.on_paint_foreground = on_paint_alarm_foreground,
            },
          
          };

          
                static auto_annunciator_t marquee_ann_2 ={
                .base.base.base.style =
    
      DRAW_NAME |
    
    BORDER_NONE,
    .base.base.base.name = "Tot. Fuel",
    .base.base.base.on_paint_foreground = on_paint_auto,
    .base.base.base.on_paint_background = on_paint_background_auto,
    .base.base.base.on_message = on_auto_msg,
    .base.base.base.status_timeout = 10000,
      .base.base.base.sensor_failed = false,
      .base.base.base.alarm_color = color_red,
      .base.base.base.rect.left = 0,
    .base.base.base.rect.top = 0,
    .base.base.base.rect.right = 0 + 254,
    .base.base.base.rect.bottom = 0 + 33,
  .base.can_id = id_fuel_total,
    .base.label_color = color_white,
    .base.text_color  = color_lightblue,
    .base.small_font = &neo_15_font,
    .base.label_offset = 2,
    .base.text_offset = 110,
  
                  .converter = &to_display_volume,
                
                .value_type = v_uint16,
                .fmt = "%d",
                };
              
                static hours_annunciator_t marquee_ann_3 ={
                .base.base.base.style =
    
      DRAW_NAME |
    
    BORDER_NONE,
    .base.base.base.name = "Fuel Endur.",
    .base.base.base.on_paint_foreground = on_paint_hours,
    .base.base.base.on_paint_background = on_paint_background_auto,
    .base.base.base.on_message = on_hours_msg,
    .base.base.base.status_timeout = 10000,
      .base.base.base.sensor_failed = false,
      .base.base.base.alarm_color = color_red,
      .base.base.base.rect.left = 0,
    .base.base.base.rect.top = 0,
    .base.base.base.rect.right = 0 + 254,
    .base.base.base.rect.bottom = 0 + 33,
  .base.can_id = id_fuel_endurance,
    .base.label_color = color_white,
    .base.text_color  = color_lightblue,
    .base.small_font = &neo_15_font,
    .base.label_offset = 2,
    .base.text_offset = 110,
  
                .base.base.base.on_message = on_hours_msg,
                .base.base.base.on_paint_foreground = on_paint_hours,
                };
              
                static auto_annunciator_t marquee_ann_4 ={
                .base.base.base.style =
    
      DRAW_NAME |
    
    BORDER_NONE,
    .base.base.base.name = "Tot. Fuel",
    .base.base.base.on_paint_foreground = on_paint_auto,
    .base.base.base.on_paint_background = on_paint_background_auto,
    .base.base.base.on_message = on_auto_msg,
    .base.base.base.status_timeout = 10000,
      .base.base.base.sensor_failed = false,
      .base.base.base.alarm_color = color_red,
      .base.base.base.rect.left = 0,
    .base.base.base.rect.top = 0,
    .base.base.base.rect.right = 0 + 254,
    .base.base.base.rect.bottom = 0 + 33,
  .base.can_id = id_fuel_pressure,
    .base.label_color = color_white,
    .base.text_color  = color_lightblue,
    .base.small_font = &neo_15_font,
    .base.label_offset = 2,
    .base.text_offset = 110,
  
                  .converter = &to_display_pressure,
                
                .value_type = v_int16,
                .fmt = "%d",
                };
              
                static auto_annunciator_t marquee_ann_5 ={
                .base.base.base.style =
    
      DRAW_NAME |
    
    BORDER_NONE,
    .base.base.base.name = "Fuel F.",
    .base.base.base.on_paint_foreground = on_paint_auto,
    .base.base.base.on_paint_background = on_paint_background_auto,
    .base.base.base.on_message = on_auto_msg,
    .base.base.base.status_timeout = 10000,
      .base.base.base.sensor_failed = false,
      .base.base.base.alarm_color = color_red,
      .base.base.base.rect.left = 0,
    .base.base.base.rect.top = 0,
    .base.base.base.rect.right = 0 + 254,
    .base.base.base.rect.bottom = 0 + 33,
  .base.can_id = id_fuel_flow_rate,
    .base.label_color = color_white,
    .base.text_color  = color_lightblue,
    .base.small_font = &neo_15_font,
    .base.label_offset = 2,
    .base.text_offset = 110,
  
                  .converter = &to_display_flow,
                
                .value_type = v_uint16,
                .fmt = "%d",
                };
              
                static hobbs_annunciator_t marquee_ann_6 ={
                .base.base.base.style =
    
      DRAW_NAME |
    
    BORDER_NONE,
    .base.base.base.name = "Eng. Hrs.",
    .base.base.base.on_paint_foreground = on_paint_hours,
    .base.base.base.on_paint_background = on_paint_background_auto,
    .base.base.base.on_message = on_hours_msg,
    .base.base.base.status_timeout = 10000,
      .base.base.base.sensor_failed = false,
      .base.base.base.alarm_color = color_red,
      .base.base.base.rect.left = 0,
    .base.base.base.rect.top = 0,
    .base.base.base.rect.right = 0 + 254,
    .base.base.base.rect.bottom = 0 + 33,
  .base.can_id = id_engine_hours,
    .base.label_color = color_white,
    .base.text_color  = color_lightblue,
    .base.small_font = &neo_15_font,
    .base.label_offset = 2,
    .base.text_offset = 110,
  
                .base.base.base.on_message = on_hobbs_msg,
                .base.base.base.on_paint_foreground = on_paint_hobbs,
                };
              
                static auto_annunciator_t marquee_ann_7 ={
                .base.base.base.style =
    
      DRAW_NAME |
    
    BORDER_NONE,
    .base.base.base.name = "Oil Pres.",
    .base.base.base.on_paint_foreground = on_paint_auto,
    .base.base.base.on_paint_background = on_paint_background_auto,
    .base.base.base.on_message = on_auto_msg,
    .base.base.base.status_timeout = 10000,
      .base.base.base.sensor_failed = false,
      .base.base.base.alarm_color = color_red,
      .base.base.base.rect.left = 0,
    .base.base.base.rect.top = 0,
    .base.base.base.rect.right = 0 + 254,
    .base.base.base.rect.bottom = 0 + 33,
  .base.can_id = id_oil_pressure,
    .base.label_color = color_white,
    .base.text_color  = color_lightblue,
    .base.small_font = &neo_15_font,
    .base.label_offset = 2,
    .base.text_offset = 110,
  
                  .converter = &to_display_pressure,
                
                .value_type = v_uint16,
                .fmt = "%d",
                };
              
                static auto_annunciator_t marquee_ann_8 ={
                .base.base.base.style =
    
      DRAW_NAME |
    
    BORDER_NONE,
    .base.base.base.name = "Oil Temp.",
    .base.base.base.on_paint_foreground = on_paint_auto,
    .base.base.base.on_paint_background = on_paint_background_auto,
    .base.base.base.on_message = on_auto_msg,
    .base.base.base.status_timeout = 10000,
      .base.base.base.sensor_failed = false,
      .base.base.base.alarm_color = color_red,
      .base.base.base.rect.left = 0,
    .base.base.base.rect.top = 0,
    .base.base.base.rect.right = 0 + 254,
    .base.base.base.rect.bottom = 0 + 33,
  .base.can_id = id_oil_temperature,
    .base.label_color = color_white,
    .base.text_color  = color_lightblue,
    .base.small_font = &neo_15_font,
    .base.label_offset = 2,
    .base.text_offset = 110,
  
                  .converter = &to_display_temperature,
                
                .value_type = v_uint16,
                .fmt = "%d",
                };
              
                static auto_annunciator_t marquee_ann_9 ={
                .base.base.base.style =
    
      DRAW_NAME |
    
    BORDER_NONE,
    .base.base.base.name = "Volts",
    .base.base.base.on_paint_foreground = on_paint_auto,
    .base.base.base.on_paint_background = on_paint_background_auto,
    .base.base.base.on_message = on_auto_msg,
    .base.base.base.status_timeout = 10000,
      .base.base.base.sensor_failed = false,
      .base.base.base.alarm_color = color_red,
      .base.base.base.rect.left = 0,
    .base.base.base.rect.top = 0,
    .base.base.base.rect.right = 0 + 254,
    .base.base.base.rect.bottom = 0 + 33,
  .base.can_id = id_dc_voltage,
    .base.label_color = color_white,
    .base.text_color  = color_lightblue,
    .base.small_font = &neo_15_font,
    .base.label_offset = 2,
    .base.text_offset = 110,
  
                .value_type = v_uint16,
                .fmt = "%d",
                };
              
                static auto_annunciator_t marquee_ann_10 ={
                .base.base.base.style =
    
      DRAW_NAME |
    
    BORDER_NONE,
    .base.base.base.name = "Amps",
    .base.base.base.on_paint_foreground = on_paint_auto,
    .base.base.base.on_paint_background = on_paint_background_auto,
    .base.base.base.on_message = on_auto_msg,
    .base.base.base.status_timeout = 10000,
      .base.base.base.sensor_failed = false,
      .base.base.base.alarm_color = color_red,
      .base.base.base.rect.left = 0,
    .base.base.base.rect.top = 0,
    .base.base.base.rect.right = 0 + 254,
    .base.base.base.rect.bottom = 0 + 33,
  .base.can_id = id_dc_current,
    .base.label_color = color_white,
    .base.text_color  = color_lightblue,
    .base.small_font = &neo_15_font,
    .base.label_offset = 2,
    .base.text_offset = 110,
  
                .value_type = v_int16,
                .fmt = "%d",
                };
              
                static auto_annunciator_t marquee_ann_11 ={
                .base.base.base.style =
    
      DRAW_NAME |
    
    BORDER_NONE,
    .base.base.base.name = "IAT",
    .base.base.base.on_paint_foreground = on_paint_auto,
    .base.base.base.on_paint_background = on_paint_background_auto,
    .base.base.base.on_message = on_auto_msg,
    .base.base.base.status_timeout = 10000,
      .base.base.base.sensor_failed = false,
      .base.base.base.alarm_color = color_red,
      .base.base.base.rect.left = 0,
    .base.base.base.rect.top = 0,
    .base.base.base.rect.right = 0 + 254,
    .base.base.base.rect.bottom = 0 + 33,
  .base.can_id = id_inlet_air_temperature,
    .base.label_color = color_white,
    .base.text_color  = color_lightblue,
    .base.small_font = &neo_15_font,
    .base.label_offset = 2,
    .base.text_offset = 110,
  
                  .converter = &to_display_temperature,
                
                .value_type = v_int16,
                .fmt = "%d",
                };
              

          static annunciator_t* marquee_annunciators[] = {
          
                (annunciator_t *) &marquee_ann_2,
              
                (annunciator_t *) &marquee_ann_3,
              
                (annunciator_t *) &marquee_ann_4,
              
                (annunciator_t *) &marquee_ann_5,
              
                (annunciator_t *) &marquee_ann_6,
              
                (annunciator_t *) &marquee_ann_7,
              
                (annunciator_t *) &marquee_ann_8,
              
                (annunciator_t *) &marquee_ann_9,
              
                (annunciator_t *) &marquee_ann_10,
              
                (annunciator_t *) &marquee_ann_11,
              
          };

          static marquee_widget_t marquee ={
          .base.style =
    
      BORDER_RIGHT |
    
      DRAW_BACKGROUND |
    
    BORDER_NONE,
    .base.border_color = color_white,
    .base.background_color = color_black,
    .base.name = "ALARMS",
    .base.status_timeout = 10000,
      .base.sensor_failed = false,
      .base.alarm_color = color_red,
      .base.rect.left = 0,
    .base.rect.top = 547,
    .base.rect.right = 0 + 256,
    .base.rect.bottom = 547 + 53,
  
          .base_widget_id = 1000,
          .base_alarm_id = 2000,
        .annunciators = marquee_annunciators,
          .num_annunciators = numelements(marquee_annunciators),
          .alarms = marquee_alarms,
          .num_alarms = numelements(marquee_alarms),
          };
        
          static map_widget_t nav ={
          .base.style =
    
      BORDER_LEFT |
    
      BORDER_BOTTOM |
    
    BORDER_NONE,
    .base.border_color = color_white,
    .base.name = "NAV",
    .base.on_create = on_create_navigator_widget,
    .base.status_timeout = 10000,
      .base.sensor_failed = false,
      .base.alarm_color = color_red,
      .base.rect.left = 380,
    .base.rect.top = 0,
    .base.rect.right = 380 + 644,
    .base.rect.bottom = 0 + 480,
  .params.day_theme.alarm_color = color_red,
    .params.day_theme.warning_color = color_yellow,
    .params.day_theme.water_color = RGB(205, 231, 244),
    .params.day_theme.land_color = RGB(214, 211, 63),
    .params.day_theme.coastline_color = color_black,
    .params.day_theme.residential_street = color_black,
    .params.day_theme.residential_area = color_khaki,
    .params.day_theme.big_road = color_black,
    .params.day_theme.major_road = color_black,
    .params.day_theme.highway = color_black,
    .params.day_theme.class_a_airspace = color_blue,
    .params.day_theme.class_b_airspace = color_blue,
    .params.day_theme.class_c_airspace = color_purple,
    .params.day_theme.class_d_airspace = color_purple,
    .params.day_theme.class_e_airspace = color_blue,
    .params.day_theme.class_f_airspace = color_blue,
    .params.day_theme.class_g_airspace = color_blue,
    .params.day_theme.class_m_airspace = color_red,
    .params.day_theme.cfz_airspace = color_blue,
    .params.day_theme.mbz_airspace = color_blue,
    .params.day_theme.danger_area = color_red,
    .params.day_theme.restricted_area = color_red,
    .params.day_theme.runway_grass_pen = RGBA(247, 247, 247, 255),
    .params.day_theme.runway_grass_color = RGBA(186, 221, 105, 204),
    .params.day_theme.runway_grass_ext_color = RGBA(216, 229, 179, 230),
    .params.day_theme.runway_pen = RGBA(65, 71, 76, 255),
    .params.day_theme.runway_color = RGBA(178, 187, 193, 230),
    .params.day_theme.runway_ext_color = RGBA(132, 142, 150, 230),
    .params.night_theme.alarm_color = color_red,
    .params.night_theme.warning_color = color_yellow,
    .params.night_theme.water_color = RGB(205, 231, 244),
    .params.night_theme.land_color = RGB(214, 211, 63),
    .params.night_theme.coastline_color = color_black,
    .params.night_theme.residential_street = color_black,
    .params.night_theme.residential_area = color_khaki,
    .params.night_theme.big_road = color_black,
    .params.night_theme.major_road = color_black,
    .params.night_theme.highway = color_black,
    .params.night_theme.class_a_airspace = color_blue,
    .params.night_theme.class_b_airspace = color_blue,
    .params.night_theme.class_c_airspace = color_purple,
    .params.night_theme.class_d_airspace = color_purple,
    .params.night_theme.class_e_airspace = color_blue,
    .params.night_theme.class_f_airspace = color_blue,
    .params.night_theme.class_g_airspace = color_blue,
    .params.night_theme.class_m_airspace = color_red,
    .params.night_theme.cfz_airspace = color_blue,
    .params.night_theme.mbz_airspace = color_blue,
    .params.night_theme.danger_area = color_red,
    .params.night_theme.restricted_area = color_red,
    .params.night_theme.runway_grass_pen = RGBA(247, 247, 247, 255),
    .params.night_theme.runway_grass_color = RGBA(186, 221, 105, 204),
    .params.night_theme.runway_grass_ext_color = RGBA(216, 229, 179, 230),
    .params.night_theme.runway_pen = RGBA(65, 71, 76, 255),
    .params.night_theme.runway_color = RGBA(178, 187, 193, 230),
    .params.night_theme.runway_ext_color = RGBA(132, 142, 150, 230),
    
          .params.font = &neo_9_font,
          
              .show_contours = true,
            
              .show_cities = true,
            
              .show_water = true,
            
          };
        

    result_t create_pfd(handle_t hwnd, aircraft_t *aircraft)
    {
    result_t result;
    handle_t child;
    handle_t widget;
    
    
        if(failed(result = create_airspeed_widget(hwnd, 1, aircraft, &asi_widget, 0)))
        return result;
      
        if(failed(result = create_attitude_widget(hwnd, 2, aircraft, &att_widget, 0)))
        return result;
      
        if(failed(result = create_altitude_widget(hwnd, 3, aircraft, &alt_widget, 0)))
        return result;
      
        if(failed(result = create_hsi_widget(hwnd, 4, aircraft, &hsi_widget, 0)))
        return result;
      
        if(failed(result = create_autopilot_widget(hwnd, 5, aircraft, &ap_widget, 0)))
        return result;
      
        if(failed(result = create_gauge_widget(hwnd, 6, aircraft, &map_gauge, 0)))
        return result;
      
        if(failed(result = create_gauge_widget(hwnd, 7, aircraft, &rpm_gauge, 0)))
        return result;
      
        if(failed(result = create_annunciator_widget(hwnd, 8, aircraft, (annunciator_t *) &hp_annunciator, 0)))
        return result;
      
        if(failed(result = create_annunciator_widget(hwnd, 9, aircraft, (annunciator_t *) &kmag_annunciator, 0)))
        return result;
      
        if(failed(result = create_annunciator_widget(hwnd, 10, aircraft, (annunciator_t *) &oat_annunciator, 0)))
        return result;
      
        if(failed(result = create_edutemps_widget(hwnd, 11, aircraft, &edu_temps, 0)))
        return result;
      
        if(failed(result = create_gauge_widget(hwnd, 12, aircraft, &oilt_gauge, 0)))
        return result;
      
        if(failed(result = create_gauge_widget(hwnd, 13, aircraft, &oilp_gauge, 0)))
        return result;
      
        if(failed(result = create_gauge_widget(hwnd, 14, aircraft, &volts_gauge, 0)))
        return result;
      
        if(failed(result = create_gauge_widget(hwnd, 15, aircraft, &fuelp_gauge, 0)))
        return result;
      
        if(failed(result = create_gauge_widget(hwnd, 16, aircraft, &fuelf_gauge, 0)))
        return result;
      
        if(failed(result = create_gauge_widget(hwnd, 17, aircraft, &amps_gauge, 0)))
        return result;
      
        if(failed(result = create_gauge_widget(hwnd, 18, aircraft, &left_fuel_gauge, 0)))
        return result;
      
        if(failed(result = create_gauge_widget(hwnd, 19, aircraft, &right_fuel_gauge, 0)))
        return result;
      
        if(failed(result = create_gauge_widget(hwnd, 20, aircraft, &fuelt_gauge, 0)))
        return result;
      
        if(failed(result = create_gauge_widget(hwnd, 21, aircraft, &flap_gauge, 0)))
        return result;
      
        if(failed(result = create_pancake_widget(hwnd, 22, aircraft, &pitch_indicator, 0)))
        return result;
      
        if(failed(result = create_pancake_widget(hwnd, 23, aircraft, &roll_indicator, 0)))
        return result;
      

        if(failed(result = create_marquee_widget(hwnd, 24, aircraft, &marquee, &widget)))
        return result;

        marquee.selected_alarm = -1;

        uint16_t child_num = 0;
        
            if(failed(result = create_annunciator_widget(widget, marquee.base_widget_id + child_num++, aircraft, (annunciator_t *) &marquee_ann_2, &child)))
            return result;

            hide_window(child);
          
            if(failed(result = create_annunciator_widget(widget, marquee.base_widget_id + child_num++, aircraft, (annunciator_t *) &marquee_ann_3, &child)))
            return result;

            hide_window(child);
          
            if(failed(result = create_annunciator_widget(widget, marquee.base_widget_id + child_num++, aircraft, (annunciator_t *) &marquee_ann_4, &child)))
            return result;

            hide_window(child);
          
            if(failed(result = create_annunciator_widget(widget, marquee.base_widget_id + child_num++, aircraft, (annunciator_t *) &marquee_ann_5, &child)))
            return result;

            hide_window(child);
          
            if(failed(result = create_annunciator_widget(widget, marquee.base_widget_id + child_num++, aircraft, (annunciator_t *) &marquee_ann_6, &child)))
            return result;

            hide_window(child);
          
            if(failed(result = create_annunciator_widget(widget, marquee.base_widget_id + child_num++, aircraft, (annunciator_t *) &marquee_ann_7, &child)))
            return result;

            hide_window(child);
          
            if(failed(result = create_annunciator_widget(widget, marquee.base_widget_id + child_num++, aircraft, (annunciator_t *) &marquee_ann_8, &child)))
            return result;

            hide_window(child);
          
            if(failed(result = create_annunciator_widget(widget, marquee.base_widget_id + child_num++, aircraft, (annunciator_t *) &marquee_ann_9, &child)))
            return result;

            hide_window(child);
          
            if(failed(result = create_annunciator_widget(widget, marquee.base_widget_id + child_num++, aircraft, (annunciator_t *) &marquee_ann_10, &child)))
            return result;

            hide_window(child);
          
            if(failed(result = create_annunciator_widget(widget, marquee.base_widget_id + child_num++, aircraft, (annunciator_t *) &marquee_ann_11, &child)))
            return result;

            hide_window(child);
          
        if(failed(result = show_marquee_child(widget, 0)))
        return result;

        child_num = 0;
        
          if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num], 0)))
          return result;
          child_num++;
        
          if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num], 0)))
          return result;
          child_num++;
        
          if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num], 0)))
          return result;
          child_num++;
        
          if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num], 0)))
          return result;
          child_num++;
        
          if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num], 0)))
          return result;
          child_num++;
        
          if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num], 0)))
          return result;
          child_num++;
        
          if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num], 0)))
          return result;
          child_num++;
        
          if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num], 0)))
          return result;
          child_num++;
        
          if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num], 0)))
          return result;
          child_num++;
        
          if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num], 0)))
          return result;
          child_num++;
        
          if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num], 0)))
          return result;
          child_num++;
        
          if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num], 0)))
          return result;
          child_num++;
        
          if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num], 0)))
          return result;
          child_num++;
        
          if(failed(result = create_alarm_annunciator(widget, marquee.base_alarm_id + child_num, &marquee_alarms[child_num], 0)))
          return result;
          child_num++;
        
        if(failed(result = create_map_widget(hwnd, 25, aircraft, &nav, 0)))
        return result;
      

      return create_menu_window(hwnd, 0, aircraft, &menu_wnd, 0);
    }
  