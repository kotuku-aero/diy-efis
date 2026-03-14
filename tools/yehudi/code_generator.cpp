#include "code_generator.h"
#include <iostream>
#include <string.h>

bool code_generator::generate(const std::string& output_file)
  {
  std::ofstream out(output_file);
  if (!out)
    {
    std::cerr << "Failed to open output file: " << output_file << "" <<
      std::endl;
    return false;
    }

  // Find the application node
  // Try with namespace first, then without
  pugi::xml_node app = parser_.document().select_node("//application").node();
  if (!app)
    {
    app = parser_.document().select_node("//application").node();
    }

  create_application(out, app);

  return true;
  }

pugi::xml_attribute get_named_attribute(const pugi::xml_node& node,
                                        const char* name)
  {
  return node.find_attribute([name](const pugi::xml_attribute& att)
    {
    return strcmp(att.name(), name) == 0;
    });
  }

void code_generator::emit_property(std::ofstream& out,
                                   const pugi::xml_node& node,
                                   const char* attribute,
                                   const char* field_name,
                                   const char* field_prefix,
                                   const char* value_prefix,
                                   const char* dflt)
  {
  pugi::xml_attribute attr = node.attribute(attribute);
  if (!attr.empty())
    out << ((field_prefix == nullptr) ? "" : field_prefix) << field_name << " = "
      << ((value_prefix == nullptr) ? "" : value_prefix) << attr.value()
      << "," << std::endl;
  else if (dflt != nullptr)
    out << ((field_prefix == nullptr) ? "" : field_prefix) << field_name << " = "
      << ((value_prefix == nullptr) ? "" : value_prefix) << dflt << "," << std::endl;
  }

void code_generator::emit_string_property(std::ofstream& out,
                                          const pugi::xml_node& node,
                                          const char* attribute,
                                          const char* field_name,
                                          const char* field_prefix,
                                          const char* value_prefix,
                                          const char* dflt)
  {
  pugi::xml_attribute attr = node.attribute(attribute);
  if (!attr.empty())
    out << ((field_prefix == nullptr) ? "" : field_prefix) << field_name << " = " << ((value_prefix == nullptr) ? "" : value_prefix) << "\"" << attr.value()
      << "\"," << std::endl;
  else if (dflt != nullptr)
    out << ((field_prefix == nullptr) ? "" : field_prefix) << field_name << " = \"" << dflt << "\"," << std::endl;
  }

void code_generator::emit_rect(std::ofstream& out, const pugi::xml_node& node,
                               const char* element, const char* value_name,
                               const char* prefix)
  {
  pugi::xml_node rect_node = node.child(element);
  std::string rect_name = make_literal(prefix, value_name);
  emit_property(out, rect_node, "left", ".left", rect_name.c_str());
  emit_property(out, rect_node, "top", ".top", rect_name.c_str());

  emit_property(out, rect_node, "left", ".right", rect_name.c_str(),
                make_literal(get_named_attribute(rect_node, "width").value(),
                             " + ").c_str());

  emit_property(out, rect_node, "top", ".bottom", rect_name.c_str(),
                make_literal(get_named_attribute(rect_node, "height").value(),
                             " + ").c_str());
  }

void code_generator::emit_point(std::ofstream& out, const pugi::xml_node& node,
                                const char* element, const char* value_name,
                                const char* prefix)
  {
  pugi::xml_node point_node = node.child(element);
  std::string rect_name = make_literal(prefix, value_name);

  emit_property(out, point_node, "x", ".x", rect_name.c_str());
  emit_property(out, point_node, "y", ".y", rect_name.c_str());
  }
