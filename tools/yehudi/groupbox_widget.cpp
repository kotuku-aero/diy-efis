#include "code_generator.h"

void code_generator::create_groupbox_widget(std::ofstream& out,
                                            const pugi::xml_node& node,
                                            const char* prefix,
                                            const char* name_prefix)
  {
  std::string panel_name_prefix;
  if (name_prefix != nullptr)
    panel_name_prefix = name_prefix;

  std::string panel_prefix;

  if (name_prefix != nullptr)
    panel_prefix = name_prefix;

  generate_panel_or_groupbox_widget_content(out, node, name_prefix, panel_name_prefix, panel_prefix);

  pugi::xml_attribute name = get_named_attribute(node, "id");
  // body of the panel/groupbox
  out << "static groupbox_widget_t " << panel_prefix << name.value() << " = {" << std::endl;
  // out << ".base.base.base.base.type = &groupbox_widget_type," << std::endl;

  std::string container_name = make_literal(panel_prefix.c_str(), name.value());

  generate_data_bound_container_widget(out, node, ".base",  container_name.c_str());

  out << "};" << std::endl << std::endl;
  }
