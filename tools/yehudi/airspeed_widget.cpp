#include "code_generator.h"

void code_generator::create_airspeed_widget(std::ofstream& out,
                                            const pugi::xml_node& node)
  {
  out << "static airspeed_widget_t " << get_named_attribute(node, "id").value() << " = {" << std::endl;

  generate_widget(out, node, ".base");

  // out << ".base.base.type = &airspeed_widget_type," << std::endl;

  emit_property(out, node, "text-color", ".text_color");
  emit_property(out, node, "pen-color", ".pen");
  emit_address_property(out, node, "font", ".font");
  emit_address_property(out, node, "large-roller-font", ".large_roller");
  emit_address_property(out, node, "small-roller-font", ".small_roller");
  emit_address_property(out, node, "converter", ".converter");


  if (node.attribute("pixels-per-unit").empty())
    out << ".pixels_per_unit = 3," << std::endl;
  else
    emit_property(out, node, "pixels-per-unit", ".pixels_per_unit");

  out << "};" << std::endl << std::endl;
  }
