#include "code_generator.h"

void code_generator::create_map_widget(std::ofstream &out,
                                       const pugi::xml_node &node)
  {
  out << "static map_widget_t " << get_named_attribute(node, "id").value()
      << " = {" << std::endl;
  // out << ".base.base.type = &map_widget_type," << std::endl;

  generate_widget(out, node, ".base");

  create_map_theme(out, node.select_node("./day-theme").node(), ".day_theme");
  create_map_theme(out, node.select_node("./night-theme").node(), ".night_theme");

  if (node.select_node("./center").node().empty())
    {
    out << ".map_center.x = " << get_named_attribute(node, "width").value() << " / 2," << std::endl;
    out << ".map_center.y = " << get_named_attribute(node, "height").value() << " / 2," << std::endl;
    }
  else
    emit_point(out, node, "center", ".map_center");

  emit_string_property(out, node, "db-path", ".db_path");
  emit_string_property(out, node, "config-path", ".config_path");
  emit_address_property(out, node, "font", ".font");
  emit_property(out, node, "show-terrain", ".show_terrain");
  emit_property(out, node, "show-contours", ".show_contours");
  emit_property(out, node, "show-cities", ".show_cities");
  emit_property(out, node, "show-water", ".show_water");

  out << "};" << std::endl << std::endl;
  }

void code_generator::create_map_theme(std::ofstream& out, const pugi::xml_node& node, const char *prefix)
  {
  emit_property(out, node, "alarm-color", ".alarm_color", prefix);
  emit_property(out, node, "warning-color", ".warning_color", prefix);
  emit_property(out, node, "water-color", ".water_color", prefix);
  emit_property(out, node, "land-color", ".land_color", prefix);
  emit_property(out, node, "coastline-color", ".coastline_color", prefix);
  emit_property(out, node, "residential-street", ".residential_street", prefix);
  emit_property(out, node, "residential-area", ".residential_area", prefix);
  emit_property(out, node, "big-road", ".big_road", prefix);
  emit_property(out, node, "major-road", ".major_road", prefix);
  emit_property(out, node, "highway", ".highway", prefix);
  emit_property(out, node, "class-a-airspace", ".class_a_airspace", prefix);
  emit_property(out, node, "class-b-airspac", ".class_b_airspace", prefix);
  emit_property(out, node, "class-c-airspace", ".class_c_airspace", prefix);
  emit_property(out, node, "class-d-airspace", ".class_d_airspace", prefix);
  emit_property(out, node, "class-e-airspace", ".class_e_airspace", prefix);
  emit_property(out, node, "class-f-airspace", ".class_f_airspace", prefix);
  emit_property(out, node, "class-g-airspace", ".class_g_airspace", prefix);
  emit_property(out, node, "class-m-airspace", ".class_m_airspace", prefix);
  emit_property(out, node, "cfz-airspace", ".cfz_airspace", prefix);
  emit_property(out, node, "mbz-airspace", ".mbz_airspace", prefix);
  emit_property(out, node, "danger-area", ".danger_area", prefix);
  emit_property(out, node, "restricted-area", ".restricted_area", prefix);
  emit_property(out, node, "grass-runway-outline", ".runway_grass_pen", prefix);
  emit_property(out, node, "grass-runway-fill", ".runway_grass_color", prefix);
  emit_property(out, node, "grass-runway-extension", ".runway_grass_ext_color", prefix);
  emit_property(out, node, "runway-outline", ".runway_pen", prefix);
  emit_property(out, node, "runway-fill", ".runway_color", prefix);
  emit_property(out, node, "runway-extension", ".runway_ext_color", prefix);
  emit_property(out, node, "aerodrome", ".aerodrome", prefix);
  emit_property(out, node, "aerodrome-border", ".aerodrome_border", prefix);

  pugi::xpath_node_set nodes = node.select_nodes("./contour");
  int position = 0;
  for (auto it = nodes.begin(); it != nodes.end(); it++, position++)
    {
    std::string value_name = prefix;
    value_name += ".theme[";
    value_name += std::to_string(position);
    value_name += "]";
    emit_property(out, it->node(), "elevation", make_literal(value_name.c_str(), ".elevation").c_str());
    emit_property(out, it->node(), "color", make_literal(value_name.c_str(), ".color").c_str());
    emit_property(out, it->node(), "color-color", make_literal(value_name.c_str(), ".contour_color").c_str());
    }
  }
