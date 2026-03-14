#include "code_generator.h"

void code_generator::generate_widget(std::ofstream &out,
                                     const pugi::xml_node &node,
                                     const char *prefix, const char *style,
                                     const char *on_msg, const char *on_paint)
  {
  std::string style_str;
  if (style != nullptr && strlen(style) > 0)
    {
    style_str = style;
    style_str += " | ";
    }

  pugi::xml_attribute attr = get_named_attribute(node, "border-left");
  if (!attr.empty() && strcmp(attr.value(), "true") == 0)
    style_str += "BORDER_LEFT | ";

  attr = get_named_attribute(node, "border-right");
  if (!attr.empty() && strcmp(attr.value(), "true") == 0)
    style_str += "BORDER_RIGHT | ";

  attr = get_named_attribute(node, "border-top");
  if (!attr.empty() && strcmp(attr.value(), "true") == 0)
    style_str += "BORDER_TOP | ";

  attr = get_named_attribute(node, "border-bottom");
  if (!attr.empty() && strcmp(attr.value(), "true") == 0)
    style_str += "BORDER_BOTTOM | ";

  attr = get_named_attribute(node, "draw-name");
  if (!attr.empty() && strcmp(attr.value(), "true") == 0)
    style_str += "DRAW_NAME | ";

  attr = get_named_attribute(node, "fill-background");
  if (!attr.empty() && strcmp(attr.value(), "true") == 0)
    style_str += "FILL_BACKGROUND |";

  style_str += "BORDER_NONE";

  out << prefix << ".style = " << style_str << "," << std::endl;

  emit_property(out, node, "background-gutter", ".background_gutter", prefix);
  emit_property(out, node, "gutter-color", ".gutter_color", prefix);
  emit_property(out, node, "background-color", ".background_color", prefix);
  emit_property(out, node, "name-color", ".name_color", prefix);
  emit_string_property(out, node, "name", ".name", prefix);
  emit_address_property(out, node, "name-font", ".name_font", prefix);
  emit_property(out, node, "./name-pt", ".name_pt", prefix);
  emit_property(out, node, "z-order", ".z_order", prefix);
  emit_property(out, node, "border-color", ".border_color", prefix, "", "color_white");

  if (on_paint != nullptr)
    out << prefix << ".on_paint = " << on_paint << "," << std::endl;
  else
    emit_property(out, node, "on-paint", ".on_paint", prefix);

  if (on_msg != nullptr)
    out << prefix << ".on_message = " << on_msg << "," << std::endl;
  else
    emit_property(out, node, "on-message", ".on_message", prefix);

  emit_property(out, node, "on-paint-background", ".on_paint_background",
                prefix);
  emit_property(out, node, "on-create", ".on_create", prefix);
  emit_property(out, node, "on-event", ".on_event", prefix);
  emit_property(out, node, "status-timeout", ".status_timeout", prefix,
                "10000");

  pugi::xml_attribute sensor_id = get_named_attribute(node, "sensor-id");
  if (!sensor_id.empty())
    {
    out << prefix << ".sensor_id = " << sensor_id.value() << "," << std::endl;
    out << prefix << ".sensor_failed = true," << std::endl;
    }

  emit_property(out, node, "alarm-id", ".alarm_id", prefix);
  emit_property(out, node, "alarm-color", ".alarm_color", prefix, "",
                "color_red");

  emit_rect(out, node, "rect", ".rect", prefix);
  }

static bool get_container_controls(const pugi::xml_node &node, pugi::xpath_node_set &children)
  {
  // a tab widget has direct children that can be a panel or a panel-ref.
  // panels controls are located under a node called widgets.
  std::string container_type = node.name();
  if (container_type == "tab")
    children = node.select_nodes("./*");
  else if (container_type == "panel" || container_type == "panel-ref" ||
           container_type == "groupbox")
    children = node.select_nodes("./widgets/*");
  else
    return false;

  return true;
  }

void code_generator::create_container_widget_constructor(
    std::ofstream &out, const pugi::xml_node &node,
    const char *prefix,
    int depth, const char *constructor_name)
  {
  pugi::xpath_node_set children;
  if (!get_container_controls(node, children))
    return;

  // create all of the child controls before the container control
  for (auto it = children.begin(); it != children.end(); ++it)
    {
    std::string node_name = it->node().name();
    if (node_name == "tab" || node_name == "panel" || node_name == "groupbox")
      {
      create_container_widget_constructor(out, it->node(), prefix, depth + 1);
      }
    else if (node_name == "panel-ref")
      {
      std::string panel_ref_query = "//templates/panel[@name='";
      panel_ref_query += get_named_attribute(it->node(), "ref-name").value();
      panel_ref_query += "']";
      pugi::xpath_node ref_node = parser_.document().document_element().select_node(panel_ref_query.c_str());
      pugi::xml_node ref = ref_node.node();
      pugi::xml_node panel_ref = it->node();

      std::string panel_name = make_literal(prefix, get_named_attribute(panel_ref, "prefix").value());
      std::string child_name = get_named_attribute(ref, "id").value();
      panel_name += child_name;

      std::string constructor_name = panel_name;
      panel_name += "_";

      out << "// panel-ref constructor" << std::endl;
      create_container_widget_constructor(out, ref, panel_name.c_str(), depth + 1, constructor_name.c_str());
      }
    }

  int position = 1;

  pugi::xml_attribute id_attr = get_named_attribute(node, "id");

  std::string constr;
  if (constructor_name != nullptr)
    constr = constructor_name;
  else
    {
    constr = prefix;
    constr += id_attr.value();
    }
  out << "static result_t " << constr
      << "_constructor(handle_t parent, handle_t *hwnd)" << std::endl;
  out << "{" << std::endl;
  out << "result_t result;" << std::endl;
  out << "handle_t child;" << std::endl;
  out << "(void)result;" << std::endl;
  out << "(void)child;" << std::endl;
  for (auto it = children.begin(); it != children.end(); ++it, ++position)
    {
    std::string node_name = it->node().name();
    std::string child_id = std::to_string(position + (100 * depth));
    std::string child_name = get_named_attribute(it->node(), "id").value();

    // decode the widget flags
    pugi::xml_attribute tabstop_attr = get_named_attribute(it->node(), "tab-stop");
    pugi::xml_attribute enabled_attr = get_named_attribute(it->node(), "enabled");
    pugi::xml_attribute visible_attr = get_named_attribute(it->node(), "visible");

    std::string create_flags = "";
    if (!tabstop_attr.empty() && strcmp(tabstop_attr.value(), "true")== 0)
      create_flags += "| DS_TABSTOP";

    if (!enabled_attr.empty() && strcmp(enabled_attr.value(), "true") == 0)
      create_flags += "| DS_ENABLED";

    if (!visible_attr.empty() && strcmp(visible_attr.value(), "true") == 0)
      create_flags += "| DS_VISIBLE";

    if (node_name == "tab")
      {
      out << "if(failed(result = create_tab_widget(parent, " << child_id << create_flags << ", &" << prefix << child_name << ", &child)) ||" << std::endl;
      out << "   failed(result = " << prefix << child_name << "_constructor(child, 0)))" << std::endl;
      out << "  return result;" << std::endl << std::endl;
      out << std::endl;
      out << "select_tab_page(child, 0);" << std::endl;
      }
    else if (node_name == "panel")
      {
      out << "if(failed(result = create_panel_widget(parent, " << child_id << create_flags << ", &" << prefix << child_name << ", &child)) ||" << std::endl;
      out << "   failed(result = " << prefix << child_name << "_constructor(child, 0)) ||" << std::endl;
      out << "   failed(hide_window(child)))" << std::endl;
      out << "  return result;" << std::endl << std::endl;
      }
    else if (node_name== "groupbox")
    {
      out << "if(failed(result = create_groupbox_widget(parent, " << child_id
          << create_flags << ", &" << prefix << child_name << ", &child)) ||" << std::endl;
      out << "   failed(result = " << prefix << child_name
          << "_constructor(child, 0)))" << std::endl;
      out << "  return result;" << std::endl << std::endl;
    }
    else if (node_name == "panel-ref")
      {
      // a panel-ref uses the name in constructed name
      // this is the case of a panel including another panel-ref
      std::string panel_ref_query = "//templates/panel[@name='";
      panel_ref_query += get_named_attribute(it->node(), "ref-name").value();
      panel_ref_query += "']";
      pugi::xpath_node ref_node = parser_.document().document_element().select_node(panel_ref_query.c_str());
      pugi::xml_node ref = ref_node.node();
      pugi::xml_node panel_ref = it->node();

      std::string panel_name = make_literal(prefix, get_named_attribute(panel_ref, "prefix").value());
      child_name = get_named_attribute(ref, "id").value();
      panel_name += child_name;

      out << "// call the panel-ref constructor" << std::endl;
      out << "if(failed(result = create_panel_widget(parent, " << child_id
          << create_flags << ", &" << prefix << panel_name << ", &child)) ||" << std::endl;
      out << "   failed(result = "<< prefix << panel_name
          << "_constructor(child, 0)) ||" << std::endl;
      out << "   failed(hide_window(child)))" << std::endl;
      out << "  return result;" << std::endl << std::endl;
      }
    else if (node_name == "button")
      {
      out << "if(failed(result = create_button_widget(parent, " << child_id
          << create_flags << ", &" << prefix << child_name << ", &child)))" << std::endl;
      out << "  return result;" << std::endl << std::endl;
      }
    else if (node_name == "edit")
      {
      out << "if(failed(result = create_edit_widget(parent, " << child_id
          << create_flags << ", &" << prefix << child_name << ", &child)))" << std::endl;
      out << "  return result;" << std::endl << std::endl;
      }
    else if (node_name == "combobox")
      {
      out << "if(failed(result = create_combobox_widget(parent, " << child_id
          << create_flags << ", &" << prefix << child_name << ", &child)))" << std::endl;
      out << "  return result;" << std::endl << std::endl;
      }
    else if (node_name == "checkbox")
      {
      out << "if(failed(result = create_checkbox_widget(parent, " << child_id
          << create_flags << ", &" << prefix << child_name << ", &child)))" << std::endl;
      out << "  return result;" << std::endl << std::endl;
      }
    else if (node_name == "listbox")
      {
      out << "if(failed(result = create_listbox_widget(parent, " << child_id
          << create_flags << ", &" << prefix << child_name << ", &child)))" << std::endl;
      out << "  return result;" << std::endl << std::endl;
      }
    else if (node_name == "label")
      {
      out << "if(failed(result = create_label_widget(parent, " << child_id
          << create_flags << ", &" << prefix << child_name << ", &child)))" << std::endl;
      out << "  return result;" << std::endl << std::endl;
      }
    }

  out << "return s_ok;" << std::endl;
  out << "}" << std::endl << std::endl;
  }

void code_generator::create_container_widget_fwd_decl(
    std::ofstream &out, const pugi::xml_node &node, const char *prefix)
  {
  // emit forward declarations of constructors
  pugi::xpath_node_set children;
  if (!get_container_controls(node, children))
    return;

  int position = 1;
  for (auto it = children.begin(); it != children.end(); ++it)
    {
    std::string node_name = it->node().name();
    std::string child_name = get_named_attribute(it->node(), "id").value();
    if (node_name == "tab" || node_name == "panel" || node_name == "groupbox")
      {
      out << "static result_t " << prefix << child_name
          << "_constructor(handle_t parent, handle_t *hwnd);" << std::endl;
      // recursive call to generate all of the forward declared children
      // TODO: should this be widgets??
      create_container_widget_fwd_decl(out, it->node(), prefix);
      }
    else if (node_name == "panel-ref")
      {
      std::string panel_ref_query = "//templates/panel[@name='";
      panel_ref_query += get_named_attribute(it->node(), "ref-name").value();
      panel_ref_query += "']";
      pugi::xpath_node ref_node = parser_.document().document_element().select_node(panel_ref_query.c_str());
      pugi::xml_node ref = ref_node.node();
      pugi::xml_node panel_ref = it->node();

      std::string panel_name = make_literal(prefix, get_named_attribute(panel_ref, "prefix").value());
      child_name = get_named_attribute(ref, "id").value();
      panel_name += child_name;

      out << "static result_t " << panel_name << "_constructor(handle_t parent, handle_t *hwnd);" << std::endl;
      create_container_widget_fwd_decl(out, ref, make_literal(panel_name.c_str(), "_").c_str());
      }
    }
  }
