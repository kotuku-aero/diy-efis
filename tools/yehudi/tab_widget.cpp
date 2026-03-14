#include "code_generator.h"

void code_generator::create_tab_widget(std::ofstream& out, const pugi::xml_node& node, const char* name_prefix)
  {
  pugi::xpath_node_set panels = node.select_nodes("./panel");
  pugi::xpath_node_set panel_refs = node.select_nodes("./panel-ref");
  pugi::xml_attribute name = get_named_attribute(node, "id");

  for (auto it = panels.begin(); it != panels.end(); ++it)
    create_panel_widget(out, it->node(), name_prefix);

  for (auto it = panel_refs.begin(); it != panel_refs.end(); ++it)
    {
    std::string panel_ref_query = "//templates/panel[@name='";
    panel_ref_query += get_named_attribute(it->node(), "ref-name").value();
    panel_ref_query += "']";
    pugi::xpath_node ref_node = parser_.document().document_element().select_node(panel_ref_query.c_str());
    pugi::xml_node ref = ref_node.node();
    pugi::xml_node panel_ref = it->node();

    out << "// Panel reference " << panel_ref_query << std::endl;
    create_panel_widget(out, ref, name_prefix, nullptr, &panel_ref);
    }

  // this holds the name of the container for each tab page
  std::string tab_container_name = name.value();
  tab_container_name += "_tabs";

  if (!panels.empty() || !panel_refs.empty())
    {
    out << "static panel_widget_t *" << tab_container_name << "[] = {" << std::endl;

    pugi::xpath_node_set panels = node.select_nodes("./*");
    for (auto it = panels.begin(); it != panels.end(); ++it)
      {
      std::string panel_name = it->node().name();
      if (panel_name == "panel")
        out << "&" << get_named_attribute(it->node(), "id").value() << "," << std::endl;
      else if (panel_name == "panel-ref")
        {
        std::string panel_ref_query = "//templates/panel[@name='";
        panel_ref_query += get_named_attribute(it->node(), "ref-name").value();
        panel_ref_query += "']";
        pugi::xpath_node ref_node = parser_.document().document_element().select_node(panel_ref_query.c_str());

        out << "&" << get_named_attribute(it->node(), "prefix").value() << get_named_attribute(ref_node.node(), "id").value()
          << "," << std::endl;
        }
      }

    out << "};" << std::endl << std::endl;
    }

  pugi::xml_attribute tab_orientation = get_named_attribute(node, "orientation");
  pugi::xml_attribute tab_text_orientation = get_named_attribute(node, "text-orientation");
  pugi::xml_attribute tab_style = get_named_attribute(node, "style");

  std::string style;
  if (!tab_style.empty())
    style = tab_style.value();

  if (strcmp(tab_orientation.value(), "vertical") == 0)
    {
    if (!style.empty())
      style += " | ";

    style += "TAB_VERTICAL";
    }
  else
    {
    if (!style.empty())
      style += " | ";

    style += "TAB_HORIZONTAL";
    }

  if (strcmp(tab_text_orientation.value(), "vertical") == 0)
    {
    if (!style.empty())
      style += " | ";

    style += "eto_vertical | eto_middle | eto_center ";
    }
  else
    {
    if (!style.empty())
      style += " | ";

    style += "eto_middle | eto_center ";
    }

  out << "static tab_widget_t " << get_named_attribute(node, "id").value() << " = {" << std::endl;
  // out << ".base.base.base.type = &tab_widget_type," << std::endl;

  generate_dialog_widget(out, node, ".base", style.c_str());

  pugi::xpath_node tab_name_size = node.select_node("./tab-name-size");
  pugi::xpath_node tab_rect = node.select_node("./rect");

  if (strcmp(tab_orientation.value(), "vertical") == 0)
    {
    out << ".tab_page_rect.left = " << get_named_attribute(tab_rect.node(), "left").value() << " + " <<
      get_named_attribute(tab_name_size.node(), "dx").value() << "," << std::endl;
    out << ".tab_page_rect.top = " << get_named_attribute(tab_rect.node(), "top").value() << "," << std::endl;
    out << ".tab_page_rect.right = " << get_named_attribute(tab_rect.node(), "left").value() << " + " <<
      get_named_attribute(tab_rect.node(), "width").value() << "," << std::endl;
    out << ".tab_page_rect.bottom = " << get_named_attribute(tab_rect.node(), "top").value() << " + " <<
      get_named_attribute(tab_rect.node(), "height").value() << "," << std::endl;
    }
  else
    {
    out << ".tab_page_rect.left = " << get_named_attribute(tab_rect.node(), "left").value() << "," << std::endl;
    out << ".tab_page_rect.top = " << get_named_attribute(tab_rect.node(), "top").value() << " + " <<
      get_named_attribute(tab_name_size.node(), "dy").value() << "," << std::endl;
    out << ".tab_page_rect.right = " << get_named_attribute(tab_rect.node(), "left").value() << " + " <<
      get_named_attribute(tab_rect.node(), "width").value() << "," << std::endl;
    out << ".tab_page_rect.bottom = " << get_named_attribute(tab_rect.node(), "top").value() << " + " <<
      get_named_attribute(tab_rect.node(), "height").value() << "," << std::endl;
    }

  out << ".tab_name_extent.dx = " << get_named_attribute(tab_name_size.node(), "dx").value() << "," << std::endl;
  out << ".tab_name_extent.dy = " << get_named_attribute(tab_name_size.node(), "dy").value() << "," << std::endl;

  if (!panels.empty() || !panel_refs.empty())
    {
    out << ".tabs = " << tab_container_name << "," << std::endl;
    out << ".num_tabs = numelements(" << tab_container_name << ")," << std::endl;
    }

  emit_address_property(out, node, "on-tab-selected", ".on_tab_selected");
  emit_property(out, node, "selected-tab", ".selected_tab");

  out << "};" << std::endl << std::endl;
  }
