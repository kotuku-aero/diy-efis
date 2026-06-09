#include "code_generator.h"

void code_generator::create_application(std::ofstream &out, const pugi::xml_node& node)
  {
  // extra symbols will be created if this exists
  pugi::xml_attribute nav_db_path_attr =
      get_named_attribute(node, "nav-db-path");

  out << "#include \"" << app_name_ << "_priv.h\"" << std::endl;

  out << "#include \"../../src/proton/dialog_widgets.h\"" << std::endl;
  out << "#include \"../../src/proton/instrument_widgets.h\"" << std::endl;

  out << "#include \"../../src/mfdlib/menu_handlers.h\"" << std::endl;

  out << "// menu widget for this application" << std::endl;
  out << "extern menu_widget_t " << app_name_ << "_" << "_menu_widget;" << std::endl;

  if (node.child("right_menu").empty() == false)
    out << "extern menu_widget_t " << app_name_ << "_" << "_right_menu_widget;" << std::endl;

  // emit all of the menu and keys forward declarations
  out << "// Forward declarations" << std::endl;

  pugi::xpath_node_set keymaps = node.select_nodes("./menu/keys");
  for (auto it = keymaps.begin(); it != keymaps.end(); ++it)
    out << "static keys_t " << get_named_attribute(it->node(), "name").value() << ";" << std::endl;

  pugi::xml_node menu_node = node.child("menu");
  create_menu_fwd(out, menu_node);

  for (auto it = keymaps.begin(); it != keymaps.end(); it++)
    create_keymap(out, it->node());

  // emit all of the menu's
  out << "// Menus" << std::endl;
  pugi::xpath_node_set menus = menu_node.select_nodes("./menu");
  for (auto it = menus.begin(); it != menus.end(); ++it)
    create_menu(out, it->node());

  create_menu_refs(out, menu_node);
  create_menu_captions(out, menu_node);

  // emit the menu definition
  create_menu_widget(out, menu_node, "menu_widget", false);

  pugi::xml_node right_menu = node.child("./right-menu");
  if (!right_menu.empty())
    create_menu_widget(out, right_menu, "right_menu_widget", true);

  // emit all of the container widgets (tab, panel) forward declaratiions
  out << "// container control constructor forward declarations" << std::endl;
  pugi::xpath_node_set container_controls = node.select_nodes("./layout/tab");
  for (auto it = container_controls.begin(); it != container_controls.end(); it++)
    create_container_widget_fwd_decl(out, it->node(), "");

  // finally generate the actual widgets that are part of the application
  pugi::xpath_node_set widgets = node.select_nodes("./layout/*");
  for (auto it = widgets.begin(); it != widgets.end(); it++)
    {
    std::string name = it->node().name();
    if (name == "compass")
      create_compass_widget(out, it->node());
    else if (name == "altitude")
      create_altitude_widget(out, it->node());
    else if (name == "airspeed")
      create_airspeed_widget(out, it->node());
    else if (name == "attitude")
      create_attitude_widget(out, it->node());
    else if (name == "hsi")
      create_hsi_widget(out, it->node());
    else if (name == "text")
      create_text_annunciator_widget(out, it->node());
    else if (name == "auto")
      create_auto_annunciator_widget(out, it->node());
    else if (name == "datetime")
      create_datetime_annunciator_widget(out, it->node());
    else if (name == "hours")
      create_hobbs_annunciator_widget(out, it->node());
    else if (name == "hp")
      create_hp_annunciator_widget(out, it->node());
    else if (name == "ecu")
      create_ecu_annunciator_widget(out, it->node());
    else if (name == "hobbs")
      create_hobbs_annunciator_widget(out, it->node());
    else if (name == "autopilot")
      create_apmode_annunciator_widget(out, it->node());
    else if (name == "waypoint")
      create_waypoint_annunciator_widget(out, it->node());
    else if (name == "edutemps")
      create_edutemps_widget(out, it->node());
    else if (name == "gauge")
      create_gauge_widget(out, it->node());
    else if (name == "map")
      create_map_widget(out, it->node());
    else if (name == "pancake")
      create_pancake_widget(out, it->node());
    else if (name == "marquee")
      create_marquee_widget(out, it->node());
    else if (name == "tab")
      create_tab_widget(out, it->node());
    }

  pugi::xml_node theme_node = node.select_single_node("./layout/theme").node();
  if (!theme_node.empty())
    {
    // create the application theme
    out << "static const dialog_theme_t theme = {" << std::endl;
    emit_address_property(out, theme_node, "label-font", ".label_font");
    emit_property(out, theme_node, "label-color", ".label_color");
    emit_address_property(out, theme_node, "edit-font", ".edit_font");
    emit_property(out, theme_node, "edit-color", ".edit_color");
    emit_property(out, theme_node, "background-color", ".background_color");
    emit_property(out, theme_node, "alarm-color", ".alarm_color");
    emit_property(out, theme_node, "border-color", ".border_color");
    emit_property(out, theme_node, "background-color-selected", ".background_color_selected");
    emit_property(out, theme_node, "background-color-disabled", ".background_color_disabled");
    emit_property(out, theme_node, "background-color-pressed", ".background_color_pressed");
    emit_property(out, theme_node, "background-color-editing", ".background_color_editing");
    emit_property(out, theme_node, "foreground-color", ".foreground_color");
    emit_property(out, theme_node, "foreground-color-selected", ".foreground_color_selected");
    emit_property(out, theme_node, "foreground-color-disabled", ".foreground_color_disabled");
    emit_property(out, theme_node, "foreground-color-pressed", ".foreground_color_pressed");
    emit_property(out, theme_node, "foreground-color-editing", ".foreground_color_editing");
    emit_property(out, theme_node, "focus-highlight-color", ".focused_highlight_color");
    emit_property(out, theme_node, "focus-style", ".focus_highlight_style");
    out << "};" << std::endl << std::endl;
    }

  // emit all of the tab widget constructors
  pugi::xpath_node_set tabs = node.select_nodes("./layout/tab");
  for (auto it = tabs.begin(); it != tabs.end(); it++)
    create_container_widget_constructor(out, it->node(), "", 1);

  out << "// main application constructor" << std::endl;
  out << "result_t create_" << app_name_ << "(handle_t hwnd)" << std::endl;
  out << "{" << std::endl;
  out << "result_t result;" << std::endl;
  out << "handle_t child;" << std::endl;
  out << "handle_t widget;" << std::endl;
  out << "(void)child;" << std::endl;
  out << "(void)widget;" << std::endl;
  out << std::endl;

  // emit the navigation database loader.
  if (!nav_db_path_attr.empty())
    {
    out << std::endl;
    out << "handle_t spatial_db;" << std::endl;
    out << "if(failed(result = open_mfd_atom_db(\"" << nav_db_path_attr.value() << "\", &spatial_db)))" << std::endl;
    out << "  {" << std::endl;
    out << "  if(result != e_path_not_found)" << std::endl;
    out << "    return result;" << std::endl;
    out << "  }" << std::endl;
    out << std::endl;
    }

  bool focus_set = false;

  if (!theme_node.empty())
    {
    // set the theme at this point.
    out << "if(failed(result = set_layout_theme(&theme)))" << std::endl;
    out << "  return result;" << std::endl << std::endl;
    }

  out << "if(failed(result = create_menu_window("
         "hwnd, " << app_name_ << "_menu_widget.base.style" << ", &" << app_name_ << "_menu_widget, "
         "&menu_handle)) ||" << std::endl;
  out << "  failed(result = set_z_order(menu_handle, 100)))" << std::endl;
  out << "  return result;" << std::endl << std::endl;

  pugi::xml_node right_menu_node = node.child("right_menu");
  if (!right_menu_node.empty())
  {
    out << "if(failed(result = create_menu_window("
           "hwnd, " << app_name_ << "_right_menu_widget.base.style" << ", &" << app_name_ << "_right_menu_widget,"
           "&right_menu_handle))||" << std::endl;
    out << "  failed(result = set_z_order(right_menu_handle, 100)))" << std::endl;
    out << "  return result;" << std::endl << std::endl;
  }

  int widget_id = 1;
  pugi::xpath_node_set controls = node.select_nodes("./layout/*");
  for (auto it = controls.begin(); it != controls.end(); it++, widget_id++)
    {
    std::string name = it->node().name();
    std::string widget_name = get_named_attribute(it->node(), "id").value();

    if (name == "airspeed" ||
      name == "compass" ||
      name == "altitude" ||
      name == "attitude" ||
      name == "hsi" ||
      name == "edutemps" ||
      name == "gauge" ||
      name == "pancake")
      {
      out << "if(failed(result = create_" << name << "_widget(hwnd, " << widget_id << ", &" << widget_name << ", &child)))" << std::endl;
      out << "  return result;" << std::endl;
      }
    else if (name == "map")
      {
      out << "if(failed(result = create_map_widget(hwnd, spatial_db, "
          << widget_id << ", &" << widget_name << ", &child)))" << std::endl;
      out << "  return result;" << std::endl;
      }
    else if (name == "text" ||
             name == "auto" ||
             name == "autopilot" ||
             name == "waypoint" ||
             name == "datetime" ||
             name == "hours" ||
             name == "hp" ||
             name == "ecu" ||
             name == "hobbs")
      {
      out << "if(failed(result = create_annunciator_widget(hwnd, " << widget_id << ", (annunciator_t *) &" << widget_name << ", &child)))" << std::endl;
      out << "  return result;" << std::endl;
      }
    else if (name == "tab")
      {
      out << "if(failed(result = create_tab_widget(hwnd, " << widget_id << " | DS_TABSTOP | DS_VISIBLE | DS_ENABLED, &" << widget_name << ", &widget)) ||" << std::endl;
      out << "  failed(result = " << widget_name << "_constructor(widget, 0)))" << std::endl;
      out << "  return result;" << std::endl;
      out << std::endl;
      out << "select_tab_page(widget, 0);" << std::endl << std::endl;
      // bit of a hack.  Set the focus to the first tab widget.
      if (!focus_set)
        {
        out << "set_focused_window(widget);" << std::endl;
        focus_set = true;
        }
    }
    else if (name == "marquee")
      {
      out << "if(failed(result = create_marquee_widget(hwnd, " << widget_id << ", &" << widget_name << ", &widget)))" << std::endl;
      out << "  return result;" << std::endl << std::endl;

      out << widget_name << ".selected_alarm = -1;" << std::endl;
      out << "uint16_t child_num = 0;" << std::endl;

      int child_position = 1;
      pugi::xpath_node_set children = it->node().select_nodes("./*");
      for (auto child = children.begin(); child != children.end(); child++)
        {
        std::string child_name = child->node().name();
        if (child_name != "alarm" && child_name != "rect")
          {
          std::string annunciator_id = make_literal(widget_name.c_str(), "_ann_", std::to_string(child_position).c_str());

          // see if the annunciator has an ID, this is used instead of the default
          pugi::xml_attribute id_attr = get_named_attribute(child->node(), "id");
          if (!id_attr.empty())
            annunciator_id = id_attr.value();

          out << "if(failed(result = create_annunciator_widget(widget, " << widget_name
              << ".base_widget_id + child_num++, (annunciator_t *)"
              << "&" << annunciator_id << ", &child)))" << std::endl;
          out << "  return result;" << std::endl << std::endl;
          out << "hide_window(child);" << std::endl << std::endl;

          child_position++;
          }
        }

      out << "if(failed(result = show_marquee_child(widget, 0)))" << std::endl;
      out << "  return result;" << std::endl << std::endl;
      out << "// Alarms" << std::endl;
      out << "child_num = 0;" << std::endl;

      pugi::xpath_node_set alarms = it->node().select_nodes("./alarm");
      int alarm_position = 1;
      for (auto it = alarms.begin(); it != alarms.end(); it++, alarm_position++)
        {
        out << "if(failed(result = create_alarm_annunciator(widget, " << widget_name << ".base_alarm_id + child_num, &"
          << widget_name << "_alarms[child_num++], 0)))" << std::endl;
        out << "  return result;" << std::endl << std::endl;
        }
      }
    }

  out << "return s_ok;" << std::endl;

  out << "}" << std::endl;
  }