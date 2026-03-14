#include "code_generator.h"

void code_generator::create_button_widget(std::ofstream& out, const pugi::xml_node& node, const char* prefix, const char* name_prefix)
  {
  out << "static button_widget_t " << name_prefix << get_named_attribute(node, "id").value() << " = {" << std::endl;
  // out << ".base.base.base.base.type = &button_widget_type," << std::endl;

  generate_data_bound_widget(out, node, make_literal(prefix, ".base").c_str(), "eto_center | eto_middle");
  out << "};" << std::endl << std::endl;
  }