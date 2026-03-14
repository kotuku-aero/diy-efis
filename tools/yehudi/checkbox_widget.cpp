#include "code_generator.h"

void code_generator::create_checkbox_widget(std::ofstream& out, const pugi::xml_node& node, const char* prefix,
                                            const char* name_prefix)
  {
  out << "static checkbox_widget_t " << name_prefix << get_named_attribute(node, "id").value() << " = {" << std::endl;
  // out << ".base.base.base.base.type = &checkbox_widget_type," << std::endl;

  generate_data_bound_widget(out, node, make_literal(prefix, ".base").c_str());

  std::string value_type;
  pugi::xml_attribute value_type_attr = get_named_attribute(node, "value_type");
  if (value_type_attr.empty())
    value_type = "v_bool";
  else
    value_type = value_type_attr.value();

  std::string default_value;
  pugi::xml_attribute default_value_attr = get_named_attribute(node, "checked");
  if (default_value_attr.empty())
    default_value = "false";
  else
    default_value = default_value_attr.value();

  create_variant(out, node, ".base.value", value_type.c_str(),default_value.c_str());

  out << "};" << std::endl << std::endl;
  }
