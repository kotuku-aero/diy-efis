#include "code_generator.h"

void code_generator::generate_alert_dialog(std::ofstream& out, const std::string &name, const pugi::xml_node& node)
  {
  out << "static uin16_t " << name << "_alarms[] = {" << std::endl;
  for (auto it = node.begin(); it != node.end(); ++it)
    out << it->value() << "," << std::endl;
  out << "};" << std::endl << std::endl;

  out << "static dialog_t " << name << " = {" << std::endl;

  out << "};" << std::endl << std::endl;
  }
