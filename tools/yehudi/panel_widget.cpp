#include "code_generator.h"

void code_generator::create_panel_widget(std::ofstream& out, const pugi::xml_node& node,
                                         const char* name_prefix,
                                         const char* style,
                                         const pugi::xml_node* panel_ref_definition)
  {
  std::string panel_name_prefix;
  if (panel_ref_definition != nullptr)
    {
    // this is expanding a template so take the definition from the ref node
    panel_name_prefix = make_literal(name_prefix, get_named_attribute(*panel_ref_definition, "prefix").value());
    std::string child_name = get_named_attribute(node, "id").value();
    panel_name_prefix += child_name;
    panel_name_prefix += "_";
    }
  else if (name_prefix != nullptr)
    panel_name_prefix = name_prefix;

  std::string panel_prefix;

  if (panel_ref_definition != nullptr)
    panel_prefix = make_literal(name_prefix, get_named_attribute(*panel_ref_definition, "prefix").value());
  else if (name_prefix != nullptr)
    panel_prefix = name_prefix;

  generate_panel_or_groupbox_widget_content(out, node, name_prefix, panel_name_prefix, panel_prefix);

  pugi::xml_attribute name = get_named_attribute(node, "id");
  // body of the panel/groupbox
  out << "static panel_widget_t " << panel_prefix << name.value() << " = {" << std::endl;
  // out << ".base.base.base.base.type = &panel_widget_type," << std::endl;

  std::string container_name = make_literal(panel_prefix.c_str(), name.value());

  generate_data_bound_container_widget(out, node, ".base",  container_name.c_str(), style);

  out << "};" << std::endl << std::endl;
  }
