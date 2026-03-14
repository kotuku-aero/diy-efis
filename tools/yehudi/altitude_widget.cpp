#include "code_generator.h"

void code_generator::create_altitude_widget(std::ofstream &out,
                                            const pugi::xml_node &node)
  {
  out << "static altitude_widget_t " << get_named_attribute(node, "id").value() << " = {" << std::endl;

  generate_widget(out, node, ".base");

  // out << ".base.base.type = &altitude_widget_type," << std::endl;

  emit_property(out, node, "text-color", ".text_color");
  emit_property(out, node, "pen-color", ".pen");
  emit_address_property(out, node, "font", ".font");
  emit_address_property(out, node, "large-roller-font", ".large_roller");
  emit_address_property(out, node, "small-roller-font", ".small_roller");
  emit_address_property(out, node, "altitude-converter", ".altitude_converter");
  emit_address_property(out, node, "alt-rate-converter", ".alt_rate_converter");
  emit_address_property(out, node, "qnh-converter", ".qnh_converter");

  out << "};" << std::endl << std::endl;
  }