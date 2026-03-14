#include "code_generator.h"
#include <string.h>

void code_generator::generate_key_handler(std::ofstream& out, const pugi::xml_node &node, const char *keymap_name, const char *postfix)
  {
  // the key handler will hold an action
  pugi::xml_node content = node.first_child();
  if (content.empty())
    return;

  std::string local_name = content.name();

  if (local_name == "event-action")
    generate_event_action(out, make_literal(keymap_name, postfix), content);
  else if (local_name == "popup-action")
    generate_popup_action(out, make_literal(keymap_name, postfix), content);
  else if (local_name == "checklist-action")
    generate_checklist_action(out, make_literal(keymap_name, postfix), content);
  }

void code_generator::emit_keymap_details(std::ofstream& out, const pugi::xml_node &node, const char *keymap_name, const char *name)
  {
  out << "." << name << " = (menu_item_t *) &" << keymap_name << "_" << name << "," << std::endl;
  emit_string_property(out, node, "caption", make_literal(".", name, "_caption").c_str());

  }

void code_generator::create_keymap(std::ofstream& out, const pugi::xml_node &node)
  {
  pugi::xpath_node_set key_handler = node.select_nodes("./*");
  pugi::xml_attribute keymap_name_attr = get_named_attribute(node, "name");
  const char *keymap_name = keymap_name_attr.value();
  
  for (auto it = key_handler.begin(); it != key_handler.end(); ++it)
    {
    std::string local_name = it->node().name();

    // if there is no name then just try again
    if (local_name.empty())
      continue;

    if (local_name == "key0")
      generate_key_handler(out, it->node(), keymap_name, "_key0");
    else if (local_name == "key1")
      generate_key_handler(out, it->node(), keymap_name, "_key1");
    else if (local_name == "key2")
      generate_key_handler(out, it->node(), keymap_name, "_key2");
    else if (local_name == "key3")
      generate_key_handler(out, it->node(), keymap_name, "_key3");
    else if (local_name == "key4")
      generate_key_handler(out, it->node(), keymap_name, "_key4");
    else if (local_name == "key5")
      generate_key_handler(out, it->node(), keymap_name, "_key5");
    else if (local_name == "key6")
      generate_key_handler(out, it->node(), keymap_name, "_key6");
    else if (local_name == "key7")
      generate_key_handler(out, it->node(), keymap_name, "_key7");
    else if (local_name == "hold-key0")
      generate_key_handler(out, it->node(), keymap_name, "_hold_key0");
    else if (local_name == "hold-key1")
      generate_key_handler(out, it->node(), keymap_name, "_hold_key1");
    else if (local_name == "hold-key2")
      generate_key_handler(out, it->node(), keymap_name, "_hold_key2");
    else if (local_name == "hold-key3")
      generate_key_handler(out, it->node(), keymap_name, "_hold_key3");
    else if (local_name == "hold-key4")
      generate_key_handler(out, it->node(), keymap_name, "_hold_key4");
    else if (local_name == "hold-key5")
      generate_key_handler(out, it->node(), keymap_name, "_hold_key5");
    else if (local_name == "hold-key6")
      generate_key_handler(out, it->node(), keymap_name, "_hold_key6");
    else if (local_name == "hold-key7")
      generate_key_handler(out, it->node(), keymap_name, "_hold_key7");
    else if (local_name == "decka-up")
      generate_key_handler(out, it->node(), keymap_name, "_decka_up");
    else if (local_name == "decka-dn")
      generate_key_handler(out, it->node(), keymap_name, "_decka_dn");
    else if (local_name == "deckb-up")
      generate_key_handler(out, it->node(), keymap_name, "_deckb_up");
    else if (local_name == "deckb-dn")
      generate_key_handler(out, it->node(), keymap_name, "_deckb_dn");
    else if (local_name == "decka-press-up")
      generate_key_handler(out, it->node(), keymap_name, "_decka_press_up");
    else if (local_name == "decka-press-dn")
      generate_key_handler(out, it->node(), keymap_name, "_decka_press_dn");
    else if (local_name == "deckb-press-up")
      generate_key_handler(out, it->node(), keymap_name, "_deckb_press_up");
    else if (local_name == "deckb-press-dn")
      generate_key_handler(out, it->node(), keymap_name, "_deckb_press_dn");
    }

  // all of the handlers are generated.  now emit the keys
  out << "static keys_t " << keymap_name << " = {" << std::endl;
  for (auto it = key_handler.begin(); it != key_handler.end(); ++it)
    {
    std::string local_name = it->node().name();

    // if there is no name then just try again
    if (local_name.empty())
      continue;

    if (local_name == "key0" ||
      local_name == "key1" ||
      local_name == "key2" ||
      local_name == "key3" ||
      local_name == "key4" ||
      local_name == "key5" ||
      local_name == "key6" ||
      local_name == "key7")
      emit_keymap_details(out, it->node(), keymap_name, local_name.c_str());
    else if (local_name == "hold-key0")
      emit_keymap_details(out, it->node(), keymap_name, "hold_key0");
    else if (local_name == "hold-key1")
      emit_keymap_details(out, it->node(), keymap_name, "hold_key1");
    else if (local_name == "hold-key2")
      emit_keymap_details(out, it->node(), keymap_name, "hold_key2");
    else if (local_name == "hold-key3")
      emit_keymap_details(out, it->node(), keymap_name, "hold_key3");
    else if (local_name == "hold-key4")
      emit_keymap_details(out, it->node(), keymap_name, "hold_key4");
    else if (local_name == "hold-key5")
      emit_keymap_details(out, it->node(), keymap_name, "hold_key5");
    else if (local_name == "hold-key6")
      emit_keymap_details(out, it->node(), keymap_name, "hold_key6");
    else if (local_name == "hold-key7")
      emit_keymap_details(out, it->node(), keymap_name, "hold_key7");
    else if (local_name == "decka-up")
      emit_keymap_details(out, it->node(), keymap_name, "decka_up");
    else if (local_name == "decka-dn")
      emit_keymap_details(out, it->node(), keymap_name, "decka_dn");
    else if (local_name == "deckb-up")
      emit_keymap_details(out, it->node(), keymap_name, "deckb_up");
    else if (local_name == "deckb-dn")
      emit_keymap_details(out, it->node(), keymap_name, "deckb_dn");
    else if (local_name == "decka-press-up")
      emit_keymap_details(out, it->node(), keymap_name, "decka_press_up");
    else if (local_name == "decka-press-dn")
      emit_keymap_details(out, it->node(), keymap_name, "decka_press_dn");
    else if (local_name == "deckb-press-up")
      emit_keymap_details(out, it->node(), keymap_name, "deckb_press_up");
    else if (local_name == "deckb-press-dn")
      emit_keymap_details(out, it->node(), keymap_name, "deckb_press_dn");
    }
  out << "};" << std::endl << std::endl;
  }


void code_generator::create_checklist_item(std::ofstream& out, const pugi::xml_node& node,
                                           const std::string& name)
  {
  out << "static menu_item_checklist_t " << name << " = {" << std::endl;
  out << ".base.item_type = mi_menu," << std::endl;
  emit_string_property(out, node, "caption", ".caption", ".base");
  out << ".base.event = item_checklist_event," << std::endl;
  out << ".base.paint = default_paint_handler," << std::endl;
  out << ".base.is_enabled = default_enable_handler," << std::endl;
  out << ".base.evaluate = item_checklist_evaluate," << std::endl;
  emit_property(out, node, "get-index", ".get_selected");
  emit_address_property(out, node, "popup", ".popup");
  out << "};" << std::endl << std::endl;
  }

void code_generator::create_popup_item(std::ofstream& out, const pugi::xml_node& node, const std::string& name)
  {
  out << "static menu_item_menu_t " << name << " = {" << std::endl;
  out << ".base.item_type = mi_menu," << std::endl;
  emit_string_property(out, node, "caption", ".caption", ".base");
  out << ".base.event = default_msg_handler," << std::endl;
  out << ".base.paint = default_paint_handler," << std::endl;
  out << ".base.is_enabled = default_enable_handler," << std::endl;
  out << ".base.evaluate = item_menu_evaluate," << std::endl;
  emit_address_property(out, node, "menu", ".menu");
  out << "};" << std::endl << std::endl;
  }

void code_generator::create_event_item(std::ofstream& out, const pugi::xml_node& node, const std::string& name)
  {
  out << "static menu_item_event_t " << name << " = {" << std::endl;
  emit_property(out, node, "can-id", ".can_id");
  out << ".base.item_type = mi_event," << std::endl;

  pugi::xml_attribute get_value_attr = get_named_attribute(node, "get-value");
  if (get_value_attr.empty())
    create_variant(out, node, "value", "value-type", "value");
  else
    out << ".get_value = " << get_value_attr.value() << "," << std::endl;

  out << ".base.event = default_msg_handler," << std::endl;
  out << ".base.paint = default_paint_handler," << std::endl;
  out << ".base.is_enabled = default_enable_handler," << std::endl;
  out << ".base.evaluate = item_event_evaluate," << std::endl;
  emit_string_property(out, node, "caption", ".caption", ".base");
  emit_string_property(out, node, "enable-regex", ".enable_regex", ".base");
  emit_string_property(out, node, "enable-format", ".enable_format", ".base");
  emit_property(out, node, "contrilling-param", ".controlling_param", ".base");
  emit_address_property(out, node, "converter", ".converter");
  out << "};" << std::endl << std::endl;
  }

void code_generator::create_action_handler(std::ofstream& out, const pugi::xml_node& node, const std::string& name)
  {
  pugi::xml_node action_node = node.child("event-action");
  if (!action_node.empty())
    {
    create_event_item(out, action_node, name);
    return;
    }

  action_node = node.child("popup-action");
  if (!action_node.empty())
    {
    create_popup_item(out, action_node, name);
    return;
    }

  action_node = node.child("checklist-action");
  if (!action_node.empty())
    {
    create_checklist_item(out, action_node, name);
    return;
    }

  out << "#error the menu action is unknown" << std::endl;
  }
