/*
 * definitions_generator.cpp - Generates private header files (_priv.h)
 */

#include "definitions_generator.h"

#include "code_generator.h"

#include <algorithm>
#include <iostream>
#include <set>

bool DefinitionsGenerator::generate(const std::string &output_file)
{
  std::ofstream out(output_file);
  if (!out)
  {
    std::cerr << "Failed to open output file: " << output_file << "\n";
    return false;
  }

  // Header guard
  out << "#ifndef __" << app_name_ << "_priv__h__\n";
  out << "#define __" << app_name_ << "_priv__h__\n";
  out << "\n";
  out << "#include \"../../src/proton/include/menu_window.h\"\n";
  out << "\n";
  out << "\n";
  out << "#ifdef __cplusplus\n";
  out << "extern \"C\" {\n";
  out << "#endif\n";

  // Generate various sections
  generate_id_definitions(out);
  generate_spin_edit_methods(out);
  generate_checklist_methods(out);
  generate_event_methods(out);
  generate_alarm_methods(out);
  generate_callback_methods(out);

  // Close extern C and header guard
  out << "\n";
  out << "#ifdef __cplusplus\n";
  out << "}\n";
  out << "#endif\n";
  out << "\n";
  out << "#endif\n";

  out.close();
  return true;
}

void DefinitionsGenerator::generate_id_definitions(std::ofstream &out)
{
  // pugi::xpath_node_set ids = parser_.document().document_element().
  //                                    select_nodes("//id");
  // int position = 1;
  // for (auto it = ids.begin(); it != ids.end(); ++it, position++)
  // {
  //   std::string name = it->node().first_child().value();
  //   out << "#define " << name << " (id_ap_action_id_first +" << position << ")"
  //       << std::endl;
  // }
  // out << std::endl << std::endl;
}

void DefinitionsGenerator::generate_spin_edit_methods(std::ofstream &out)
{
  // Find all <spin-edit-action> elements
  auto spin_edits = parser_.find_all("//spin-edit-action");

  std::set<std::string> methods_declared; // Avoid duplicates

  for (const auto &node : spin_edits)
  {
    // get-method
    if (parser_.has_attr(node, "get-method"))
    {
      std::string method = parser_.get_attr(node, "get-method");
      if (methods_declared.insert(method).second)
      {
        out << "extern result_t " << method <<
            "(menu_item_t *edit, variant_t *value);\n";
      }
    }

    // set-method
    if (parser_.has_attr(node, "set-method"))
    {
      std::string method = parser_.get_attr(node, "set-method");
      if (methods_declared.insert(method).second)
      {
        out << "extern result_t " << method <<
            "(menu_item_t *edit, const variant_t *value);\n";
      }
    }

    // get-max-value
    if (parser_.has_attr(node, "get-max-value"))
    {
      std::string method = parser_.get_attr(node, "get-max-value");
      if (methods_declared.insert(method).second)
      {
        out << "extern result_t " << method <<
            "(menu_item_t *edit, variant_t *value);\n";
      }
    }

    // set-max-value
    if (parser_.has_attr(node, "set-max-value"))
    {
      std::string method = parser_.get_attr(node, "set-max-value");
      if (methods_declared.insert(method).second)
      {
        out << "extern result_t " << method <<
            "(menu_item_t *edit, const variant_t *value);\n";
      }
    }
  }
}

void DefinitionsGenerator::generate_checklist_methods(std::ofstream &out)
{
  // Find all <checklist-action> elements
  auto checklists = parser_.find_all("//checklist-action");

  std::set<std::string> methods_declared;

  for (const auto &node : checklists)
  {
    // get-index
    if (parser_.has_attr(node, "get-index"))
    {
      std::string method = parser_.get_attr(node, "get-index");
      if (methods_declared.insert(method).second)
      {
        out << "extern result_t " << method <<
            "(menu_item_t *edit, uint32_t *value);\n";
      }
    }
  }
}

void DefinitionsGenerator::generate_event_methods(std::ofstream &out)
{
  // Find all <event-action> elements
  auto events = parser_.find_all("//event-action");

  std::set<std::string> methods_declared;

  for (const auto &node : events)
  {
    // get-value
    if (parser_.has_attr(node, "get-value"))
    {
      std::string method = parser_.get_attr(node, "get-value");
      if (methods_declared.insert(method).second)
      {
        out << "extern result_t " << method <<
            "(menu_item_t *item, variant_t *value);\n";
      }
    }
  }
}

void DefinitionsGenerator::generate_alarm_methods(std::ofstream &out)
{
  // Find all <alarm> elements within <alarm-dialog>
  auto alarms = parser_.find_all("//alarm-dialog/alarm");

  std::set<std::string> methods_declared;

  for (const auto &node : alarms)
  {
    // message-fmt
    if (parser_.has_attr(node, "message-fmt"))
    {
      std::string method = parser_.get_attr(node, "message-fmt");
      if (methods_declared.insert(method).second)
      {
        // Generate both signatures as per XSL
        out << "extern result_t " << method <<
            "(menu_item_t *edit, int32_t *value);\n";
        out << "extern result_t " << method <<
            "(menu_item_t *edit, int32_t value);\n";
      }
    }
  }
}

void DefinitionsGenerator::generate_callback_methods(std::ofstream &out)
{
  std::set<std::string> methods_declared;
  pugi::xml_node doc = parser_.document().document_element();

  // on-message callbacks
  pugi::xpath_node_set methods = doc.select_nodes(
      "//*[./@on-message[normalize-space() != '']]");
  for (auto it = methods.begin(); it != methods.end(); ++it)
  {
    std::string method = get_named_attribute(it->node(), "on-message").value();
    if (!method.empty() && methods_declared.insert(method).second)
    {
      out << "extern result_t " << method <<
          "(handle_t hwnd, const canmsg_t* msg, void* wnddata);\n";
    }
  }

  // on-paint callbacks
  methods = doc.select_nodes("//*[./@on-paint[normalize-space() != '']]");
  for (auto it = methods.begin(); it != methods.end(); ++it)
  {
    std::string method = get_named_attribute(it->node(), "on-paint").value();
    if (!method.empty() && methods_declared.insert(method).second)
    {
      out << "extern void " << method <<
          "(handle_t canvas, const rect_t *wnd_rect, const canmsg_t* msg, void* wnddata);\n";
    }
  }

  // on-create callbacks
  methods = doc.select_nodes("//*[./@on-create[normalize-space() != '']]");
  for (auto it = methods.begin(); it != methods.end(); ++it)
  {
    std::string method = get_named_attribute(it->node(), "on-create").value();
    if (!method.empty() && methods_declared.insert(method).second)
    {
      out << "extern result_t " << method <<
          "(handle_t hwnd, widget_t *widget);\n";
    }
  }

  // on-selected callbacks
  methods = doc.select_nodes("//*[./@on-selected[normalize-space() != '']]");
  for (auto it = methods.begin(); it != methods.end(); ++it)
  {
    std::string method = get_named_attribute(it->node(), "on-selected").value();
    if (!method.empty() && methods_declared.insert(method).second)
    {
      out << "extern result_t " << method <<
          "(widget_t* widget, const event_args_t *args);\n";
    }
  }

  // on-item-selected callbacks
  methods = doc.select_nodes(
      "//*[./@on-item-selected[normalize-space() != '']]");
  for (auto it = methods.begin(); it != methods.end(); ++it)
  {
    std::string method = get_named_attribute(it->node(), "on-item-selected").
        value();
    if (!method.empty() && methods_declared.insert(method).second)
    {
      out << "extern result_t " << method <<
          "(widget_t* widget, const event_args_t *args);\n";
    }
  }

  // on-event callbacks
  methods = doc.select_nodes("//*[./@on-event[normalize-space() != '']]");
  for (auto it = methods.begin(); it != methods.end(); ++it)
  {
    std::string method = get_named_attribute(it->node(), "on-event").value();
    if (!method.empty() && methods_declared.insert(method).second)
    {
      out << "extern  result_t " << method <<
          "(widget_t* widget, const event_args_t *args);\n";
    }
  }

  // on-event callbacks
  methods = doc.select_nodes("//*[./@on-evaluate[normalize-space() != '']]");
  for (auto it = methods.begin(); it != methods.end(); ++it)
  {
    std::string method = get_named_attribute(it->node(), "on-evaluate").value();
    if (!method.empty() && methods_declared.insert(method).second)
    {
      out << "extern  menu_item_action_result " << method <<
          "(menu_widget_t* wnd, menu_item_t * item, const canmsg_t* msg);\n";
    }
  }
}