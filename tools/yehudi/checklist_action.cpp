#include "code_generator.h"

void code_generator::generate_checklist_action(std::ofstream& out, const std::string &name, const pugi::xml_node& action_node)
  {
  out << "static menu_item_checklist_t " << name << " = {" << std::endl;
  out << ".base.item_type = mi_checklist," << std::endl;

  pugi::xml_attribute caption_attr = get_named_attribute(action_node, "caption");
  if (!caption_attr.empty())
    out << ".base.caption = \"" << caption_attr.value() << "\"," << std::endl;

  out << ".base.event = item_checklist_event," << std::endl;
  out << ".base.paint = default_paint_handler," << std::endl;
  out << ".base.is_enabled = default_enable_handler," << std::endl;
  out << ".base.evaluate = item_checklist_evaluate," << std::endl;

  out << ".get_selected = " << get_named_attribute(action_node, "get-index").value() << "," << std::endl;
  out << ".popup = &" << get_named_attribute(action_node, "popup").value() << "," << std::endl;
  out << "};" << std::endl <<std::endl;

  }