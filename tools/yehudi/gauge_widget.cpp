#include "code_generator.h"

void code_generator::create_gauge_widget(std::ofstream &out, const pugi::xml_node &node)
{
  pugi::xpath_node_set nodes = node.select_nodes("./step");

  if (!nodes.empty())
    {
    out << "static const step_t " << get_named_attribute(node, "id").value() << "_steps[] = {" << std::endl;
    for (auto it = nodes.begin(); it != nodes.end(); it++)
      {
      out << "{" << std::endl;
      emit_property(out, it->node(), "value", ".value");
      emit_property(out, it->node(), "pen", ".pen");
      emit_property(out, it->node(), "color", ".color");
      out << "}," << std::endl;
      }

    out << "};" << std::endl << std::endl;
    }

  nodes = node.select_nodes("./tick");
  if (!nodes.empty())
    {
    out << "static const tick_mark_t " << get_named_attribute(node, "id").value() << "_ticks[] = {" << std::endl;
    for (auto it = nodes.begin(); it != nodes.end(); it++)
    {
      out << "{" << std::endl;
      emit_string_property(out, it->node(), "text", ".text");
      emit_property(out, it->node(), "value", ".value");
      emit_property(out, it->node(), "color", ".color");
      emit_property(out, it->node(), "width", ".width");
      emit_property(out, it->node(), "length", ".length");
      out << "}," << std::endl;
    }

    out << "};" << std::endl << std::endl;
    }

  out << "static gauge_widget_t " << get_named_attribute(node, "id").value() << " = {" << std::endl;
  // out << ".base.base.type = &gauge_widget_type," << std::endl;

  generate_widget(out, node, ".base");

  // generate the style
  std::string style;
  std::string attr;

  attr = get_named_attribute(node, "style").value();
  if (attr == "pointer")
    out << ".style = gs_pointer," << std::endl;
  else if (attr == "sweep")
    out << ".style = gs_sweep," << std::endl;
  else if (attr == "bar")
    out << ".style = gs_bar," << std::endl;
  else if (attr == "point")
    out << ".style = gs_point," << std::endl;
  else if (attr == "pointer_minmax")
    out << ".style = gs_pointer_minmax," << std::endl;
  else if (attr == "point_minmax")
    out << ".style = gs_point_minmax," << std::endl;
  else if (attr == "hbar")
    out << ".style = gs_hbar," << std::endl;
  else if (attr == "small")
    out << ".style = bgs_small," << std::endl;

  emit_address_property(out, node, "font", ".font");
  emit_property(out, node, "can-id", ".can_id");
  emit_property(out, node, "reset-id", ".reset_id");
  emit_property(out, node, "radii", ".gauge_radii");
  emit_property(out, node, "arc-width", ".arc_width");
  emit_property(out, node, "bar-width", ".bar_width");
  emit_property(out, node, "arc-begin", ".arc_begin");
  emit_property(out, node, "arc-range", ".arc_range");
  emit_property(out, node, "reset-value", ".reset_value");
  emit_property(out, node, "pointer-width", ".pointer_width");
  emit_property(out, node, "draw-value", ".draw_value");
  emit_address_property(out, node, "value-font", ".value_font");
  emit_property(out, node, "draw-value-box", ".draw_value_box");
  emit_address_property(out, node, "converter", ".converter");
  emit_property(out, node, "incremental", ".incremental");
  emit_point(out, node, "name-pt", ".name_pt", ".base");
  emit_point(out, node, "center", ".center");
  emit_rect(out, node, "value-rect", ".value_rect");

  nodes = node.select_nodes("./step");
  if (!nodes.empty())
    {
    out << ".steps = " << get_named_attribute(node, "id").value() << "_steps," << std::endl;
    out << ".num_steps = numelements(" << get_named_attribute(node, "id").value() << "_steps)," << std::endl;
    }

  nodes = node.select_nodes("./tick");
  if (!nodes.empty())
  {
    out << ".ticks = " << get_named_attribute(node, "id").value() << "_ticks," << std::endl;
    out << ".num_ticks = numelements(" << get_named_attribute(node, "id").value() << "_ticks)," << std::endl;
  }

  out << "};" << std::endl << std::endl;
}