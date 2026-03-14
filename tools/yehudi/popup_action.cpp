#include "code_generator.h"

void code_generator::generate_popup_action(std::ofstream& out, const std::string &name, const pugi::xml_node& action_node)
  {
  out << "static menu_item_menu_t " << name << " = {" << std::endl;
  out << ".base.item_type = mi_menu," << std::endl;

  pugi::xml_attribute caption_attr = get_named_attribute(action_node, "caption");
  if (!caption_attr.empty())
    out << ".base.caption = \"" << caption_attr.value() << "\"," << std::endl;

  out << ".base.event = default_msg_handler," << std::endl;
  out << ".base.paint = default_paint_handler," << std::endl;
  out << ".base.is_enabled = default_enable_handler," << std::endl;
  out << ".base.evaluate = item_menu_evaluate," << std::endl;

  pugi::xml_attribute menu_attr = get_named_attribute(action_node, "menu");
  if (!menu_attr.empty())
    out << ".menu = &" << menu_attr.value() << "," << std::endl;
  out << "};" << std::endl << std::endl;
  }