#include "code_generator.h"

void code_generator::create_hsi_widget(std::ofstream &out,
                                       const pugi::xml_node &node)
{
  out << "static hsi_widget_t " << get_named_attribute(node, "id").value() << " = {" << std::endl;
  // out << ".base.base.type = &hsi_widget_type," << std::endl;

  generate_widget(out, node, ".base");

  out << "};" << std::endl << std::endl;
}