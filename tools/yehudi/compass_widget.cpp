#include "code_generator.h"

void code_generator::create_compass_widget(std::ofstream &out,
                                           const pugi::xml_node &node)
  {
  out << "static compass_widget_t " << get_named_attribute(node, "id").value()
      << " = {" << std::endl;
  // out << ".base.base.type = &compass_widget_type," << std::endl;

  generate_widget(out, node, ".base");

  if (node.attribute("hdg-offset").empty())
    {
    out << ".hdg_offset = " <<
      get_named_attribute(node.select_node("./rect").node(), "width").value()
        << " / 2," << std::endl;
    }
  else
    emit_property(out, node, "hdg-offset", ".hdg_offset");

  out << "};" << std::endl << std::endl;
  }