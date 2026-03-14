#include "code_generator.h"

void code_generator::generate_text_annunciator(std::ofstream& out, const pugi::xml_node& node, const char* prefix)
  {
  generate_widget(out, node, make_literal(prefix, ".base.base").c_str());

  emit_property(out, node, "can-id", ".can_id", prefix);
  emit_address_property(out, node, "small-font", ".small_font", prefix);
  emit_property(out, node, "compact", ".compact", prefix);
  emit_property(out, node, "text-color", ".text_color", prefix);
  emit_property(out, node, "text-format", ".text_format", prefix);
  emit_property(out, node, "text-offset", ".text_offset", prefix);
  emit_property(out, node, "label-color", ".label_color", prefix);
  emit_property(out, node, "label_offset", ".label_offset", prefix);
  emit_property(out, node, "label-format", ".label_format", prefix);
  }

void  code_generator::create_text_annunciator_widget(std::ofstream& out, const pugi::xml_node& node, const char *name)
  {
  out << "static text_annunciator_t " << (name == nullptr ? get_named_attribute(node, "id").value() : name) << " = {" << std::endl;
  // out << ".base.base.base.type = &text_annunciator_widget_type," << std::endl;
  generate_text_annunciator(out, node, "");
  out << "};" << std::endl << std::endl;
  }

void code_generator::create_auto_annunciator_widget(std::ofstream& out, const pugi::xml_node& node, const char *name)
  {
  out << "static auto_annunciator_t " << (name == nullptr ? get_named_attribute(node, "id").value() : name) << " = {" << std::endl;
  generate_text_annunciator(out, node, ".base");

  emit_address_property(out, node, "converter", ".converter");
  emit_property(out, node, "value-type", ".value_type");
  emit_string_property(out, node, "format", ".fmt");
  out << ".base.base.base.on_message = on_auto_msg," << std::endl;
  out << ".base.base.base.on_paint = on_paint_auto," << std::endl;

  out << "};" << std::endl << std::endl;
  }

void code_generator::create_datetime_annunciator_widget(std::ofstream& out, const pugi::xml_node& node, const char *name)
  {
  out << "static datetime_annunciator_t " << (name == nullptr ? get_named_attribute(node, "id").value() : name) << " = {" << std::endl;
  generate_text_annunciator(out, node, ".base");
  emit_string_property(out, node, "format", ".format");
  out << ".base.base.base.on_message = on_datetime_msg," << std::endl;
  out << ".base.base.base.on_paint = on_paint_datetime," << std::endl;

  out << "};" << std::endl << std::endl;
  }

void code_generator::create_hobbs_annunciator_widget(std::ofstream& out, const pugi::xml_node& node, const char* name)
  {
  out << "static hobbs_annunciator_t " << (name == nullptr ? get_named_attribute(node, "id").value() : name) << " = {" << std::endl;
  generate_text_annunciator(out, node, ".base");
  out << ".base.base.base.on_message = on_hobbs_msg," << std::endl;
  out << ".base.base.base.on_paint = on_paint_hobbs," << std::endl;

  out << "};" << std::endl << std::endl;
  }

void code_generator::create_hp_annunciator_widget(std::ofstream& out, const pugi::xml_node& node, const char* name)
  {
  out << "static hp_annunciator_t " << (name == nullptr ? get_named_attribute(node, "id").value() : name) << " = {" << std::endl;
  generate_text_annunciator(out, node, ".base");
  out << ".base.base.base.on_message = on_hp_msg," << std::endl;
  out << ".base.base.base.on_paint = on_paint_hp," << std::endl;

  out << "};" << std::endl << std::endl;
  }

void code_generator::create_ecu_annunciator_widget(std::ofstream& out, const pugi::xml_node& node, const char* name)
{
  out << "static ecu_annunciator_t " << (name == nullptr ? get_named_attribute(node, "id").value() : name) << " = {" << std::endl;
  generate_widget(out, node, ".base.base");
  out << ".base.base.on_message = on_kmag_msg," << std::endl;
  out << ".base.base.on_paint = on_paint_kmag," << std::endl;

  out << "};" << std::endl << std::endl;
}

void code_generator::create_apmode_annunciator_widget(std::ofstream &out,
                                        const pugi::xml_node &node,
                                        const char *name)
  {
  out << "static apmode_annunciator_t " << (name == nullptr ? get_named_attribute(node, "id").value() : name) << " = {" << std::endl;
  generate_text_annunciator(out, node, ".base");
  out << ".base.base.base.on_message = on_hobbs_msg," << std::endl;
  out << ".base.base.base.on_paint = on_paint_hobbs," << std::endl;

  out << "};" << std::endl << std::endl;
  }

void code_generator::create_waypoint_annunciator_widget(std::ofstream& out, const pugi::xml_node& node, const char* name)
{
  out << "static waypoint_annunciator_t " << (name == nullptr ? get_named_attribute(node, "id").value() : name) << " = {" << std::endl;
  generate_text_annunciator(out, node, ".base");
  out << ".base.base.base.on_message = on_waypoint_msg," << std::endl;
  out << ".base.base.base.on_paint = on_paint_waypoint," << std::endl;

  out << "};" << std::endl << std::endl;
}
