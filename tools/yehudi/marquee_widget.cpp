#include "code_generator.h"

void code_generator::create_marquee_widget(std::ofstream &out,
                                           const pugi::xml_node &node)
  {
  pugi::xml_attribute name = get_named_attribute(node, "id");

  pugi::xml_node rect_node = node.child("rect");
  // emit the alarms.
  pugi::xpath_node_set alarm_nodes = node.select_nodes("./alarm");
  int position = 1;

  for (auto it = alarm_nodes.begin(); it != alarm_nodes.end(); it++, position++)
    {
    out << "static const uint16_t " << name.value() << "_alarm_ids_"
        << std::to_string(position) << "[] = {" << std::endl;
    pugi::xpath_node_set alarm_ids = it->node().select_nodes("./alarm-id");
    for (auto alarm_it = alarm_ids.begin(); alarm_it != alarm_ids.end();
         alarm_it++)
      out << alarm_it->node().first_child().value() << "," << std::endl;

    out << "};" << std::endl << std::endl;
    }

  // emit all of the annunciators in the marquee
  pugi::xpath_node_set widget_nodes = node.select_nodes("./*");
  position = 1;
  for (auto it = widget_nodes.begin(); it != widget_nodes.end();
       it++)
    {
    std::string widget_name = it->node().name();

    std::string annunciator_id =
        make_literal(name.value(), "_ann_", std::to_string(position).c_str());

    // see if the annunciator has an ID, this is used instead of the default
    pugi::xml_attribute id_attr = get_named_attribute(it->node(), "id");
    if (!id_attr.empty())
      annunciator_id = id_attr.value();

    if (widget_name == "text")
      create_text_annunciator_widget(out, it->node(), annunciator_id.c_str());
    else if (widget_name == "auto")
      create_auto_annunciator_widget(out, it->node(), annunciator_id.c_str());
    else if (widget_name == "datetime")
      create_datetime_annunciator_widget(out, it->node(),
                                         annunciator_id.c_str());
    else if (widget_name == "hours")
      create_hobbs_annunciator_widget(out, it->node(), annunciator_id.c_str());
    else if (widget_name == "hp")
      create_hp_annunciator_widget(out, it->node(), annunciator_id.c_str());
    else if (widget_name == "hobbs")
      create_hobbs_annunciator_widget(out, it->node(), annunciator_id.c_str());
    else
      position--;     // is not an annunciator

    position++;
    }

  // now the references
  out << "static annunciator_t *" << name.value() << "_annunciators[] = {"
      << std::endl;
  position = 1;
  for (auto it = widget_nodes.begin(); it != widget_nodes.end(); it++)
    {
    std::string widget_name = it->node().name();

    if (widget_name == "text" || widget_name == "auto" ||
        widget_name == "datetime" || widget_name == "hours" ||
        widget_name == "hp" || widget_name == "hobbs")
      {
      std::string annunciator_id =
          make_literal(name.value(), "_ann_", std::to_string(position).c_str());

      // see if the annunciator has an ID, this is used instead of the default
      pugi::xml_attribute id_attr = get_named_attribute(it->node(), "id");
      if (!id_attr.empty())
        annunciator_id = id_attr.value();

      out << "(annunciator_t *) &" << annunciator_id.c_str() << "," << std::endl;
      position++;
      }
    }

  out << "};" << std::endl << std::endl;

  out << "static alarm_t " << name.value() << "_alarms[] = {" << std::endl;

  position = 1;
  for (auto it = alarm_nodes.begin(); it != alarm_nodes.end(); it++, position++)
    {
    pugi::xml_node alarm_node = it->node();

    out << "{" << std::endl;
    out << ".can_ids = " << name.value() << "_alarm_ids_"
        << std::to_string(position).c_str() << "," << std::endl;
    out << ".num_ids = numelements(" << name.value() << "_alarm_ids_"
        << std::to_string(position).c_str() << ")," << std::endl;
    if (!alarm_node.attribute("annunciator").empty())
      out << ".annunciator = (annunciator_t *) &"
          << alarm_node.attribute("annunciator").value() << ", " << std::endl;

    emit_property(out, alarm_node, "priority", ".priority");
    emit_property(out, alarm_node, "can-park", ".can_park");
    emit_string_property(out, alarm_node, "message", ".message");
    out << ".base.style = FILL_BACKGROUND," << std::endl;
    emit_property(out, alarm_node, "alarm-background", "background_color",
                  ".base");
    emit_property(out, alarm_node, "alarm-test", "name_color", ".base");
    out << ".base.rect.left = 1," << std::endl;
    out << ".base.rect.top = 1," << std::endl;
    out << ".base.rect.right = " << get_named_attribute(rect_node, "width").value()
        << " -2," << std::endl;
    out << ".base.rect.bottom = " << get_named_attribute(rect_node, "height").value()
        << " -2," << std::endl;
    emit_property(out, alarm_node, "font", ".font", ".base");
    out << ".base.on_paint = on_paint_alarm_foreground," << std::endl;
    emit_address_property(out, node, "font", ".name_font", ".base");
    emit_property(out, node, "alarm-text", ".name_color", ".base");
    emit_property(out, node, "alarm-background", ".background_color",".base");
    out << "}," << std::endl;
    }

  out << "};" << std::endl << std::endl;

  out << "static marquee_widget_t " << name.value() << " = {" << std::endl;
  // out << ".base.base.type = &marquee_widget_type," << std::endl;

  generate_widget(out, node, ".base");

  emit_property(out, node, "base-id", ".base_widget_id");
  emit_property(out, node, "base-alarm-id", ".base_alarm_id");

  out << ".annunciators = " << name.value() << "_annunciators," << std::endl;
  out << ".num_annunciators = numelements(" << name.value() << "_annunciators),"
      << std::endl;

  out << ".alarms = " << name.value() << "_alarms," << std::endl;
  out << ".num_alarms = numelements(" << name.value() << "_alarms),"
      << std::endl;

  out << "};" << std::endl << std::endl;
  }
