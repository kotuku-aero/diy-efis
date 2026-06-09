#include "code_generator.h"

void code_generator::generate_textedit_action(std::ofstream &out,
                                              const std::string &name,
                                              const pugi::xml_node &action_node)
  {
  std::string id = get_named_attribute(action_node, "id").value();
  out << "static menu_item_textedit_t " << name << " = {" << std::endl;
  out << ".base.item_type = mi_edit," << std::endl;

  emit_string_property(out, action_node, "caption", ".caption", ".base");
  
  out << ".base.event = item_textedit_event," << std::endl;
  out << ".base.paint = default_paint_handler," << std::endl;
  out << ".base.edit_paint = item_textedit_paint," << std::endl;
  out << ".base.is_enabled = default_enable_handler," << std::endl;
  out << ".base.evaluate = item_textedit_evaluate," << std::endl;

  emit_property(out, action_node, "get-method", ".get_value");
  emit_property(out, action_node, "set-method", ".set_value");
  emit_property(out, action_node, "get-hint-method", ".get_hint");
  emit_property(out, action_node, "max-length", ".max_length");
  emit_address_property(out, action_node, "keys", ".keys");


  out << "};" << std::endl << std::endl;
  }
