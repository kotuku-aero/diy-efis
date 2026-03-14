#include "code_generator.h"

#include <cstring>

void code_generator::create_menu(std::ofstream &out,
                                   const pugi::xml_node &node)
  {
  out << "// Menu definitions" << std::endl;

  int i = 1;
  pugi::xml_attribute name_attr = get_named_attribute(node, "name");

  // work over the handlers
  for (auto handler = node.begin(); handler != node.end(); handler++, i++)
    {
    char i_str[16];
    snprintf(i_str, 16, "%d", i);

    std::string name = make_literal(name_attr.value(), "_item_", i_str);

    if (strcmp(handler->name(), "event-action") == 0)
      generate_event_action(out, name, *handler);
    else if (strcmp(handler->name(), "popup-action") == 0)
      generate_popup_action(out, name, *handler);
    else if (strcmp(handler->name(), "checklist-action") == 0)
      generate_checklist_action(out, name, *handler);
    else if (strcmp(handler->name(), "spin-edit-action") == 0)
      generate_spinedit_action(out, name, *handler);
    }

  out << std::endl;

  if (i > 1)
    {
    out << "static menu_item_t *" << make_literal(name_attr.value(), "_items")
        << "[] = {" << std::endl;
    // build an array of pointers
    for (int j = 1; j < i; j++)
      {
      char j_str[16];
      snprintf(j_str, 16, "%d", j);

      out << "(menu_item_t *) &"
          << make_literal(name_attr.value(), "_item_", j_str) << ","
          << std::endl;
      }

    out << "};" << std::endl << std::endl;
    }

  out << "static menu_t " << name_attr.value() << " = {" << std::endl;
  if (i > 1)
    {
    out << ".menu_items = " << name_attr.value() << "_items," << std::endl;
    out << ".num_items = " << i - 1 << "," << std::endl;
    }
  out << ".name = \"" << name_attr.value() << "\"," << std::endl;
  emit_string_property(out, node, "caption", ".caption");
  emit_address_property(out, node, "keys", ".keys");

  out << "};" << std::endl << std::endl;
  }

void code_generator::create_menu_widget(std::ofstream &out,
                                         const pugi::xml_node &node,
                                         const char *decl_name, bool is_right)
  {
  out << "menu_widget_t " << app_name_ << "_" << decl_name << " = {" << std::endl;

  generate_widget(out, node, ".base", "DS_ENABLED", nullptr, "menu_widget_on_paint");

  // out << ".base.base.type = &menu_widget_type," << std::endl;

  if (is_right)
    out << ".right_align = true," << std::endl;
  out << ".num_menu_items = numelements(menu_items)," << std::endl;
  out << ".menu_items = menu_items," << std::endl;
  out << ".num_menus = numelements(menus)," << std::endl;
  out << ".menus = menus," << std::endl;
  emit_point(out, node, "menu-rect", ".menu_rect");
  emit_point(out, node, "menu-start", ".menu_start");
  emit_property(out, node, "selected-background-color",
                ".selected_background_color");
  emit_property(out, node, "text-color", ".text_color");
  emit_property(out, node, "selected-color", ".selected_color");
  emit_property(out, node, "menu-border-color", ".menu_border_color", "", "",
                "color_yellow");
  emit_property(out, node, "menu-timeout", ".menu_timeout");
  emit_address_property(out, node, "font", ".font");
  emit_address_property(out, node, "root-keys", ".root_keys");
  emit_address_property(out, node, "alarm-keys", ".alarm_keys");
  emit_address_property(out, node, "setup-menu", ".setup_menu");

  pugi::xml_node captions = node.child("captions");
  if (!captions.empty())
    {
    emit_rect(out, captions, "rect", ".menu_caption_rect");
    out << ".key_captions = " << decl_name << "_captions," << std::endl;
    pugi::xpath_node_set buttons = captions.select_nodes("./button");
    out << ".num_key_captions = numelements(" << decl_name <<  "_captions),"
        << std::endl;
    }

  out << "};" << std::endl << std::endl;
  }

void code_generator::create_menu_captions(std::ofstream &out,
                                          const pugi::xml_node &node)
  {
  pugi::xml_node captions = node.child("captions");
  if (captions.empty())
    return;

  out << "// Menu Captions " << std::endl;
  out << "static button_t menu_widget_captions[] = {" << std::endl;

  pugi::xpath_node_set buttons = captions.select_nodes("./button");
  for (auto it = buttons.begin(); it != buttons.end(); it++)
    {
    out << "{" << std::endl;
    emit_rect(out, it->node(), "rect", ".rect");
    emit_property(out, it->node(), "key-name", ".key");
    emit_property(out, it->node(), "foreground-color", ".foreground_color", "",
                  "", "color_white");
    emit_property(out, it->node(), "background-color", ".background_color", "", "",
          "color_black");
    emit_property(out, it->node(), "selected-color", ".selected_color", "", "", "color_black");
    emit_property(out, it->node(), "highlight-color", ".highlight_color", "", "", "color_lightblue");
    out << "}," << std::endl;
    }

  out << "};" << std::endl << std::endl;
  }

void code_generator::create_menu_refs(std::ofstream &out,
                                      const pugi::xml_node &node)
  {
  out << "// Menu references" << std::endl;
  out << "static menu_t *menus[] = {" << std::endl;

  pugi::xpath_node_set menus = node.select_nodes("./menu");

  for (auto it = menus.begin(); it != menus.end(); it++)
    out << "&" << get_named_attribute(it->node(), "name").value() << "," << std::endl;
  out << "};" << std::endl << std::endl;

  // these are all of the menu items, created so that the code can pass messages to them all to
  // handle enable/evaluate actions
  out << "static menu_item_t *menu_items[] = {" << std::endl;
  for (auto it = menus.begin(); it != menus.end(); it++)
  {
    int position = 1;
    pugi::xpath_node_set items = it->node().select_nodes("./*");
    for (auto item = items.begin(); item != items.end(); item++)
      out << "(menu_item_t *) &" << get_named_attribute(it->node(), "name").value() << "_item_" << position++ << "," << std::endl;
  }
  out << "};" << std::endl << std::endl;
  }

void code_generator::create_menu_fwd(std::ofstream &out,
                                     const pugi::xml_node &node)
  {
  pugi::xpath_node_set menus = node.select_nodes("./menu");
  for (auto it = menus.begin(); it != menus.end(); ++it)
    out << "static menu_t " << get_named_attribute(it->node(), "name").value() << ";" << std::endl;
  }