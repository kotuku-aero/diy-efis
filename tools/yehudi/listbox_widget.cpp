#include "code_generator.h"

void code_generator::create_listbox_widget(std::ofstream& out, const pugi::xml_node& node, const char* prefix,
                                           const char* name_prefix)
  {
  out << "static listbox_widget_t " << name_prefix << get_named_attribute(node, "id").value() << " = {" << std::endl;
  // out << ".base.base.base.base.type = &label_widget_type," << std::endl;

  generate_data_bound_widget(out, node, make_literal(prefix, ".base").c_str());

  emit_property(out, node, "can-id", ".can_id", prefix);

  create_variant(out, node, ".base.value","v_int32", "0");

  pugi::xml_attribute attr = node.attribute("selected-item");
  if (attr.empty())
    out << ".selected_item = -1," << std::endl;
  else
    out << ".selected_item = " << attr.value() << "," << std::endl;

  out << "};" << std::endl << std::endl;
  }
