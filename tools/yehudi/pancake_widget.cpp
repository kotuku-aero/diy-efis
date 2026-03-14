#include "code_generator.h"

void code_generator::create_pancake_widget(std::ofstream &out,
                                           const pugi::xml_node &node)
{
  out << "static pancake_widget_t " << get_named_attribute(node, "id").value() << " = {" << std::endl;
  // out << ".base.base.type = &pancake_widget_type," << std::endl;

  generate_widget(out, node, ".base");

  emit_rect(out, node, "indicator-rect", ".indicator_rect");
  emit_property(out, node, "can-id", ".can_id");
  emit_property(out, node, "outline-color", ".outline_color");
  emit_property(out, node, "indicator-color", ".indicator_color");
  emit_property(out, node, "bar-width", ".bar_width");
  emit_property(out, node, "bar-height", ".bar_height");
  emit_property(out, node, "bar-gutter", ".bar_gutter");
  emit_property(out, node, "min-value", ".min_value");
  emit_property(out, node, "max-value", ".max_value");
  emit_property(out, node, "fill", ".bar_fill");
  emit_property(out, node, "horizontal", ".horizontal");
  emit_property(out, node, "draw-value", ".draw_value");
  emit_property(out, node, "draw-value-box", ".draw_value_box");
  emit_property(out, node, "value-font", ".");
  emit_property(out, node, "", ".value_font");

  emit_rect(out, node, "value-rect", ".value_rect");

  out << "};" << std::endl << std::endl;
}
