#include "code_generator.h"

void code_generator::create_combobox_widget(std::ofstream &out,
                                            const pugi::xml_node &node,
                                            const char *prefix,
                                            const char *name_prefix)
  {
  pugi::xpath_node_set items = node.select_nodes("./item");

  std::string combobox_name = name_prefix;
  combobox_name.append(get_named_attribute(node, "id").value());

  if (!items.empty())
    {
    out << "static combobox_widget_item_t " << combobox_name << "_items[] = {" << std::endl;
    for (auto it = items.begin(); it != items.end(); ++it)
      {
      out << "{" << std::endl;
      emit_string_property(out, it->node(), "text", ".text");
      emit_property(out, it->node(), "can_id", ".can_id");
      create_variant(out, node, ".value",
                     get_named_attribute(it->node(), "value-type").value(),
                     get_named_attribute(it->node(), "value").value());
      out << "}," << std::endl;
      }
    out << "};" << std::endl << std::endl;
    }

  out << "static combobox_widget_t " << combobox_name << " = {" << std::endl;
  // out << ".base.base.base.base.type = &combobox_widget_type," << std::endl;

  generate_data_bound_widget(out, node, make_literal(prefix, ".base").c_str());

  emit_property(out, node, "can-id", ".can_id", prefix);
  emit_property(out, node, "on-item-selected", ".on_item_selected", prefix);

  if (!items.empty())
    {
    out << ".items = " << combobox_name << "_items," << std::endl;
    out << ".num_items = numelements("<< combobox_name << "_items)," << std::endl;

    pugi::xml_attribute attr = node.attribute("selected-item");
    if (attr.empty())
      out << ".selected_item = -1," << std::endl;
    else
      out << ".selected_item = " << attr.value() << "," << std::endl;
    }

  out << "};" << std::endl << std::endl;
  }
