#include "code_generator.h"

void code_generator::generate_panel_or_groupbox_widget_content(std::ofstream& out,
                                                               const pugi::xml_node& node,
                                                               const char* prefix,
                                                               const std::string &name_prefix,
                                                               const std::string &panel_prefix)
  {
  pugi::xpath_node_set widgets = node.select_nodes("./widgets/*");
  pugi::xml_attribute name = get_named_attribute(node, "id");

  for (auto it = widgets.begin(); it != widgets.end(); ++it)
    {
    std::string local_name = it->node().name();

    if (local_name == "button")
      create_button_widget(out, it->node(), "", name_prefix.c_str());
    else if (local_name == "groupbox")
      create_groupbox_widget(out, it->node(), "", name_prefix.c_str());
    else if (local_name == "edit")
      create_edit_widget(out, it->node(), "", name_prefix.c_str());
    else if (local_name == "combobox")
      create_combobox_widget(out, it->node(), "", name_prefix.c_str());
    else if (local_name == "listbox")
      create_listbox_widget(out, it->node(), "", name_prefix.c_str());
    else if (local_name == "checkbox")
      create_checkbox_widget(out, it->node(), "", name_prefix.c_str());
    else if (local_name == "label")
      create_label_widget(out, it->node(), "", name_prefix.c_str());
    else if (local_name == "tab")
      create_tab_widget(out, it->node(), name_prefix.c_str());
    else if (local_name == "panel")
      create_panel_widget(out, it->node(), name_prefix.c_str());
    else if (local_name == "panel-ref")
      {
      // this is the case of a panel including another panel-ref
      std::string panel_ref_query = "//templates/panel[@name='";
      panel_ref_query += get_named_attribute(it->node(), "ref-name").value();
      panel_ref_query += "']";
      pugi::xpath_node ref_node = parser_.document().document_element().select_node(panel_ref_query.c_str());
      pugi::xml_node ref = ref_node.node();
      pugi::xml_node panel_ref = it->node();

      create_panel_widget(out, ref, name_prefix.c_str(), nullptr, &panel_ref);
      }
    }

  out << std::endl;

  // emit the references to the embedded widgets
  out << "static data_bound_widget_t *" << panel_prefix << name.value() << "_widgets[] = {" << std::endl;
  for (auto it = widgets.begin(); it != widgets.end(); ++it)
    out << "(data_bound_widget_t *) &" << name_prefix << get_named_attribute(it->node(), "id").value() << ", " <<
      std::endl;

  out << "};" << std::endl << std::endl;
  }