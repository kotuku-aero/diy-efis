#include "code_generator.h"

void code_generator::generate_dialog_widget(std::ofstream& out, const pugi::xml_node& node, const char* prefix,
                                            const char* style, const char *on_msg, const char* on_paint)
  {
  generate_widget(out, node, make_literal(prefix, ".base").c_str(), style, on_msg, on_paint);

  emit_string_property(out, node, "menu", ".menu_name", prefix);
  }