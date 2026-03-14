#include "code_generator.h"

void code_generator::generate_data_bound_widget(
    std::ofstream &out, const pugi::xml_node &node, const char *prefix,
    const char *style, const char *on_msg, const char *on_paint)
  {
  generate_dialog_widget(out, node, make_literal(prefix, ".base").c_str(), style,
                  on_msg, on_paint);

  pugi::xpath_node binding = node.select_single_node("./binding");
  if (!binding.node().empty())
    {
    emit_string_property(out, binding.node(), "path", ".path", prefix);
    emit_property(out, binding.node(), "value-type", ".datatype", prefix);

    pugi::xml_attribute default_value =
        get_named_attribute(node, "default-value");
    if (!default_value.empty())
      create_variant(out, node, ".default_value",
                     get_named_attribute(binding.node(), "value-type").value(),
                     default_value.value(), prefix);

    pugi::xml_attribute container =
        get_named_attribute(binding.node(), "container");
    if (!container.empty())
      out << prefix << ".container = (data_bound_widget_container_t *) &"
          << container.value() << "," << std::endl;

    emit_address_property(out, node, "converter", ".converter", prefix);

    pugi::xml_attribute node_id =
        get_named_attribute(binding.node(), "node-id");
    if (!node_id.empty())
      {
      if (strcmp(node_id.value(), "edu_node_id") == 0)
        out << prefix << ".node_id = 1," << std::endl;
      else if (strcmp(node_id.value(), "mag_left_id") == 0)
        out << prefix << ".node_id = 2," << std::endl;
      else if (strcmp(node_id.value(), "mag_right_id") == 0)
        out << prefix << ".node_id = 3," << std::endl;
      else if (strcmp(node_id.value(), "hub_node_id") == 0)
        out << prefix << ".node_id = 4," << std::endl;
      else if (strcmp(node_id.value(), "roll_srv_node_id") == 0)
        out << prefix << ".node_id = 5," << std::endl;
      else if (strcmp(node_id.value(), "pitch_srv_node_id") == 0)
        out << prefix << ".node_id = 6," << std::endl;
      else if (strcmp(node_id.value(), "yaw_srv_node_id") == 0)
        out << prefix << ".node_id = 7," << std::endl;
      else if (strcmp(node_id.value(), "ecb_node_id") == 0)
        out << prefix << ".node_id = 15," << std::endl;
      else if (strcmp(node_id.value(), "LOCAL") == 0)
        out << prefix << ".node_id = 16," << std::endl;
      else
        out << prefix << ".node_id = " << node_id.value() << "," << std::endl;
      }
    else
      out << prefix << ".node_id = 16," << std::endl;
    }
  }

void code_generator::generate_data_bound_container_widget(
    std::ofstream &out, const pugi::xml_node &node, const char *prefix,
    const char *container_name, const char *style, const char *on_msg,
    const char *on_paint)
  {
  generate_dialog_widget(out, node, make_literal(prefix, ".base").c_str(),
                             style, on_msg, on_paint);

  pugi::xml_attribute container = get_named_attribute(node, "contents");

  out << prefix << ".contents = " << container_name << "_widgets," << std::endl;
  out << prefix << ".size = numelements(" << container_name << "_widgets),"
      << std::endl;
  }
