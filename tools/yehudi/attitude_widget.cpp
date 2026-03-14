#include "code_generator.h"

void code_generator::create_attitude_widget(std::ofstream &out, const pugi::xml_node &node)
  {
  out << "static attitude_widget_t " << get_named_attribute(node, "id").value() << " = {" << std::endl;

  generate_widget(out, node, ".base");

  // out << ".base.base.type = &attitude_widget_type," << std::endl;

  emit_property(out, node, "text-color", ".text_color", ".base");
  emit_point(out, node, "median", ".median");
  emit_rect(out, node, "skid-indicator", ".skid_indicator");

  if (!node.attribute("show-aoa").empty())
    {
    emit_property(out, node, "show-aoa", ".show_aoa");
    out << ".aoa_pixels_per_degree = 5," << std::endl;
    out << ".aoa_degrees_per_mark = 5," << std::endl;
    }

  emit_property(out, node, "show-glideslope", ".show_glideslope");

  out << "};" << std::endl << std::endl;
  }