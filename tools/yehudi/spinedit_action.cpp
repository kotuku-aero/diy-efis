#include "code_generator.h"

void code_generator::generate_spinedit_action(std::ofstream& out, const std::string &name, const pugi::xml_node& action_node)
  {
  std::string id = get_named_attribute(action_node, "id").value();
  out << "static menu_item_spin_edit_t " << name << " = {" << std::endl;
  out << ".base.item_type = mi_edit," << std::endl;
  out << ".base.caption = \"" << get_named_attribute(action_node, "caption").value() << "\"," << std::endl;
  out << ".base.event = item_spin_edit_event," << std::endl;
  out << ".base.paint = default_paint_handler," << std::endl;
  out << ".base.edit_paint = item_spin_edit_paint," << std::endl;
  out << ".base.is_enabled = default_enable_handler," << std::endl;
  out << ".base.evaluate = item_spin_edit_evaluate," << std::endl;
  out << ".get_value = " << get_named_attribute(action_node, "get-method").value() << "," << std::endl;
  out << ".set_value = " << get_named_attribute(action_node, "set-method").value() << ","<< std::endl;
  out << ".keys = &" << get_named_attribute(action_node, "keys").value() << "," << std::endl;

  pugi::xml_attribute circular_att = get_named_attribute(action_node, "circular");
  if (!circular_att.empty())
    out << ".circular = " << circular_att.value() << "," << std::endl;

  pugi::xml_attribute converter_att = get_named_attribute(action_node, "converter");
  if (converter_att.empty())
    {
    pugi::xml_attribute format_att = get_named_attribute(action_node, "format");
    if (format_att.empty())
      out << ".format = \"%0.0f\"," << std::endl;
    else
      out << ".format = \"" << format_att.value() << "\"," << std::endl;
    }
  else
    out << ".converter = &" << converter_att.value() << "," << std::endl;

  pugi::xml_attribute get_min_value_att = get_named_attribute(action_node, "get-min-value");
  if (get_min_value_att.empty())
    out << ".min_value = " << get_named_attribute(action_node, "min-value").value() << "," << std::endl;
  else
    out << ".get_min_value = " << get_min_value_att.value() << "," << std::endl;

  pugi::xml_attribute get_max_value_att = get_named_attribute(action_node, "get-max-value");
  if (get_max_value_att.empty())
    out << ".max_value = " << get_named_attribute(action_node, "max-value").value() << "," << std::endl;
  else
    out << ".get_max_value = " << get_max_value_att.value() << "," << std::endl;

  out << "};" << std::endl << std::endl;
  }
