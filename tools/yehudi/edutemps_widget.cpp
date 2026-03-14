#include "code_generator.h"

void code_generator::create_edutemps_widget(std::ofstream& out,
                                            const pugi::xml_node& node)
{
  out << "static edutemps_widget_t " << get_named_attribute(node, "id").value() << " = {" << std::endl;
  // out << ".base.base.type = &edutemps_widget_type," << std::endl;

  generate_widget(out, node, ".base");

  emit_property(out, node, "num-cyl", ".num_cylinders");
  emit_property(out, node, "cht-redline", ".cht_red_line");
  emit_property(out, node, "egt-redline", ".egt_red_line");
  emit_property(out, node, "cht-min", ".cht_min");
  emit_property(out, node, "egt-min", ".egt_min");
  emit_property(out, node, "left-gutter", ".left_gutter");
  emit_property(out, node, "right-gutte", ".right_gutter");
  emit_property(out, node, "cylinder-draw-width", ".cylinder_draw_width");
  emit_property(out, node, "egt-line", ".egt_line");
  emit_property(out, node, "cht-line", ".cht_line");
  emit_property(out, node, "redline-pos", ".red_line_pos");
  emit_property(out, node, "bar-top", ".bar_top");
  emit_property(out, node, "bar-bottom", ".bar_bottom");
  emit_property(out, node, "status-top", ".status_top");
  emit_address_property(out, node, "font", ".font");
  emit_property(out, node, "cht-color", ".cht_color");
  emit_property(out, node, "egt-color", ".egt_color");
  emit_address_property(out, node, "converter", ".converter");

  pugi::xpath_node_set bars = node.select_nodes("./cht");
  int position = 0;
  for (auto it = bars.begin(); it != bars.end(); it++, position++)
    {
    std::string name = ".cht[" + std::to_string(position) + "]";
    generate_tempbar(out, it->node(), name.c_str());
    }

  bars = node.select_nodes("./egt");
  position = 0;
  for (auto it = bars.begin(); it != bars.end(); it++, position++)
  {
    std::string name = ".egt[" + std::to_string(position) + "]";
    generate_tempbar(out, it->node(), name.c_str());
  }

  out << "};" << std::endl << std::endl;
}

void code_generator::generate_tempbar(std::ofstream& out, const pugi::xml_node& node, const char *name)
  {
  emit_property(out, node, "can-id", make_literal(name, ".can_id").c_str());
  emit_property(out, node, "color", make_literal(name, ".default_color").c_str());
  emit_property(out, node, "alarm-id", make_literal(name, ".alarm_id").c_str());
  emit_property(out, node, "alam-color", make_literal(name, ".alarm_color").c_str());
  emit_string_property(out, node, "name", make_literal(name, ".bar_name").c_str());
  emit_property(out, node, "sensor-id", make_literal(name, ".sensor_id").c_str());
  out << name << ".value = 273," << std::endl;
  }