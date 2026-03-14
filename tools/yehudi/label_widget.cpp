#include "code_generator.h"

void code_generator::create_label_widget(std::ofstream& out, const pugi::xml_node& node, const char* prefix,
                                         const char* name_prefix)
  {
  out << "static label_widget_t " << name_prefix
    << get_named_attribute(node, "id").value() << " = {" << std::endl;
  // out << ".base.base.type = &label_widget_type," << std::endl;

  // generate the style
  std::string style;
  std::string attr;

  attr = get_named_attribute(node, "h-align").value();
  if (attr == "left")
    style = "eto_left |";
  else if (attr == "center")
    style = "eto_middle |";
  else if (attr == "right")
    style = "eto_right |";

  attr = get_named_attribute(node, "v-align").value();
  if (attr == "top")
    style += "eto_top |";
  else if (attr == "middle")
    style += "eto_middle |";
  else if (attr == "bottom")
    style += "eto_bottom |";

  attr = get_named_attribute(node, "multiline").value();
  if (attr == "true" || attr == "1")
    style += "eto_wrap |";

  style += "eto_none ";

  generate_widget(out, node, make_literal(prefix, ".base").c_str(), style.c_str());

  out << "};" << std::endl << std::endl;
 }
