#include "code_generator.h"

void code_generator::generate_event_action(std::ofstream& out, const std::string &name, const pugi::xml_node& action_node)
  {
  out << "static const menu_item_event_t " << name << " = {" << std::endl;

  emit_property(out, action_node, "can-id", ".can_id");
  out << ".base.item_type = mi_event," << std::endl;
  pugi::xml_attribute get_value_attr = get_named_attribute(action_node, "get-value");
  if (get_value_attr.empty())
    create_variant(out, action_node, ".value",
      get_named_attribute(action_node, "value-type").value(),
      get_named_attribute(action_node, "value").value(),
      "");
  else
    out << ".get_value = " << get_value_attr.value() << "," << std::endl;

  emit_property(out, action_node, "on-evaluate", ".evaluate", ".base", nullptr, "item_event_evaluate");
  out << ".base.event = default_msg_handler," << std::endl;
  out << ".base.paint = default_paint_handler," << std::endl;
  out << ".base.is_enabled = default_enable_handler," << std::endl;

  pugi::xml_attribute caption_attr = get_named_attribute(action_node, "caption");
  if (!caption_attr.empty())
    out << ".base.caption = \"" << caption_attr.value() << "\"," << std::endl;

  pugi::xml_attribute enable_regex_attr = get_named_attribute(action_node, "enable-regex");
  if (!enable_regex_attr.empty())
    out << ".base.enable_regex = " << enable_regex_attr.value() << "," << std::endl;

  pugi::xml_attribute enable_format_attr = get_named_attribute(action_node, "enable-format");
  if (!enable_format_attr.empty())
    out << ".base.enable_format = " << enable_format_attr.value() << "," << std::endl;

  pugi::xml_attribute controlling_param_attr = get_named_attribute(action_node, "controlling-param");
  if (!controlling_param_attr.empty())
    out << ".base.controlling_param = " << controlling_param_attr.value() << "," << std::endl;

  pugi::xml_attribute converter_attr = get_named_attribute(action_node, "converter");
  if (!converter_attr.empty())
    out << ".base.converter = " << converter_attr.value() << "," << std::endl;

  out << "};" << std::endl << std::endl;

  }