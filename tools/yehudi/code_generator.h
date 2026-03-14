#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "xml_parser.h"
#include <fstream>
#include <string>
#include <string.h>
#include <stdlib.h>

/**
 * Generates the implementation file (.c) from XML
 * This is the most complex generator - it creates all the widget
 * creation code, menu definitions, etc.
 */
class code_generator
  {
public:
  code_generator(const XmlParser& parser, const std::string& app_name)
    : parser_(parser), app_name_(app_name)
    {
    }

  bool generate(const std::string& output_file);

private:
  // Individual widget generators
  void create_menu(std::ofstream& out, const pugi::xml_node& node);
  void create_keymap(std::ofstream& out, const pugi::xml_node& node);
  void generate_key_handler(std::ofstream& out, const pugi::xml_node& node, const char *keymap_name,
                            const char* postfix);
  void emit_keymap_details(std::ofstream& out, const pugi::xml_node& node, const char *keymap_name,
                           const char* name);
  void generate_event_action(std::ofstream& out, const std::string& name,
                             const pugi::xml_node& node);
  void generate_popup_action(std::ofstream& out, const std::string& name,
                             const pugi::xml_node& node);
  void generate_checklist_action(std::ofstream& out, const std::string& name,
                                 const pugi::xml_node& node);
  void generate_spinedit_action(std::ofstream& out, const std::string& name,
                                const pugi::xml_node& node);
  void generate_alert_dialog(std::ofstream& out, const std::string& name,
                             const pugi::xml_node& node);
  void create_menu_widget(std::ofstream& out, const pugi::xml_node& node,
                          const char* decl_name, bool is_right = false);
  void create_menu_captions(std::ofstream& out, const pugi::xml_node& node);
  void create_menu_refs(std::ofstream& out, const pugi::xml_node& node);
  void create_menu_fwd(std::ofstream& out, const pugi::xml_node& node);
  void create_action_handler(std::ofstream& out, const pugi::xml_node& node,
                             const std::string& name);
  void create_checklist_item(std::ofstream& out, const pugi::xml_node& node,
                             const std::string& name);
  void create_popup_item(std::ofstream& out, const pugi::xml_node& node,
                         const std::string& name);
  void create_event_item(std::ofstream& out, const pugi::xml_node& node,
                         const std::string& name);

  // main application generation
  void create_application(std::ofstream& out, const pugi::xml_node& node);

  void generate_dialog_widget(std::ofstream& out, const pugi::xml_node& node,
                              const char* prefix, const char* style = nullptr,
                              const char* on_msg = nullptr, const char* on_paint = nullptr);

  void create_variant(std::ofstream& out, const pugi::xml_node& node,
                      const char* member_name, const char* value_type,
                      const char* value, const char* prefix = "");
  void create_map_theme(std::ofstream& out, const pugi::xml_node& node,
                        const char* prefix);

  void create_container_widget_constructor(std::ofstream& out,
                                           const pugi::xml_node& node,
                                           const char* prefix, int depth, const char* constructor_name = nullptr);
  void create_container_widget_fwd_decl(std::ofstream& out,
                                        const pugi::xml_node& node,
                                        const char* prefix);

  void generate_data_bound_container_widget(std::ofstream& out,
                                            const pugi::xml_node& node,
                                            const char* prefix,
                                            const char* container_name,
                                            const char* style = nullptr,
                                            const char* on_msg = nullptr,
                                            const char* on_paint = nullptr);
  void generate_data_bound_widget(std::ofstream& out,
                                  const pugi::xml_node& node,
                                  const char* prefix,
                                  const char* style = nullptr,
                                  const char* on_msg = nullptr,
                                  const char* on_paint = nullptr);
  void generate_widget(std::ofstream& out, const pugi::xml_node& widget,
                       const char* prefix,
                       const char* style = nullptr,
                       const char* on_msg = nullptr,
                       const char* on_paint = nullptr);

  void generate_text_annunciator(std::ofstream& out, const pugi::xml_node& node,
                                 const char* prefix);

  void create_panel_widget(std::ofstream& out, const pugi::xml_node& node,
                           const char* name_prefix, const char* style = nullptr,
                           const pugi::xml_node* panel_ref_definition = nullptr);
  void create_groupbox_widget(std::ofstream& out, const pugi::xml_node& node,
                            const char* prefix = "",
                            const char* name_prefix = "");
  void generate_panel_or_groupbox_widget_content(std::ofstream& out,
                            const pugi::xml_node& node,
                            const char* prefix,
                            const std::string &name_prefix,
                            const std::string &panel_prefix);
  void create_tab_widget(std::ofstream& out, const pugi::xml_node& node,
                         const char* name_prefix = "");
  void create_button_widget(std::ofstream& out, const pugi::xml_node& node,
                            const char* prefix = "",
                            const char* name_prefix = "");
  void create_edit_widget(std::ofstream& out, const pugi::xml_node& node,
                          const char* prefix = "",
                          const char* name_prefix = "");
  void create_label_widget(std::ofstream& out, const pugi::xml_node& node,
                           const char* prefix = "",
                           const char* name_prefix = "");
  void create_combobox_widget(std::ofstream& out, const pugi::xml_node& node,
                              const char* prefix = "",
                              const char* name_prefix = "");
  void create_listbox_widget(std::ofstream& out, const pugi::xml_node& node,
                             const char* prefix = "",
                             const char* name_prefix = "");
  void create_checkbox_widget(std::ofstream& out, const pugi::xml_node& node,
                              const char* prefix = "",
                              const char* name_prefix = "");

  void generate_tempbar(std::ofstream& out, const pugi::xml_node& node,
                        const char* name);

  // Annunciators
  void create_text_annunciator_widget(std::ofstream& out,
                                      const pugi::xml_node& node,
                                      const char* name = nullptr);
  void create_auto_annunciator_widget(std::ofstream& out,
                                      const pugi::xml_node& node,
                                      const char* name = nullptr);
  void create_datetime_annunciator_widget(std::ofstream& out,
                                          const pugi::xml_node& node,
                                          const char* name = nullptr);
  void create_hobbs_annunciator_widget(std::ofstream& out,
                                       const pugi::xml_node& node,
                                       const char* name = nullptr);
  void create_hp_annunciator_widget(std::ofstream& out,
                                    const pugi::xml_node& node,
                                    const char* name = nullptr);
  void create_ecu_annunciator_widget(std::ofstream& out,
                                     const pugi::xml_node& node,
                                     const char* name = nullptr);
  void create_apmode_annunciator_widget(std::ofstream& out,
                                        const pugi::xml_node& node,
                                        const char* name = nullptr);
  void create_waypoint_annunciator_widget(std::ofstream& out,
                                          const pugi::xml_node& node,
                                          const char* name = nullptr);

  void create_compass_widget(std::ofstream& out, const pugi::xml_node& node);
  void create_altitude_widget(std::ofstream& out, const pugi::xml_node& node);
  void create_attitude_widget(std::ofstream& out, const pugi::xml_node& node);
  void create_hsi_widget(std::ofstream& out, const pugi::xml_node& node);
  void create_airspeed_widget(std::ofstream& out, const pugi::xml_node& node);
  void create_edutemps_widget(std::ofstream& out, const pugi::xml_node& node);
  void create_gauge_widget(std::ofstream& out, const pugi::xml_node& node);
  void create_map_widget(std::ofstream& out, const pugi::xml_node& node);
  void create_marquee_widget(std::ofstream& out, const pugi::xml_node& node);
  void create_pancake_widget(std::ofstream& out, const pugi::xml_node& node);
  /**
   * Emit the code to initialize a rectangle type
   * @param out stream to write to
   * @param node node with properties
   * @param element element name with the rectangle definition
   * @param field_name name of the generated field
   * @param value_prefix prefix for the field
   */
  void emit_rect(std::ofstream& out, const pugi::xml_node& node,
                 const char* element, const char* field_name,
                 const char* prefix = "");
  /**
   * Emit the code to initialize a point type
   * @param out stream to write to
   * @param node node with properties
   * @param element element name with the pont definition
   * @param field_name name of the generated field
   * @param value_prefix prefix for the field
   */
  void emit_point(std::ofstream& out, const pugi::xml_node& node,
                  const char* element, const char* field_name,
                  const char* value_prefix = "");
  /**
   * emit a property
   * @param out stream
   * @param node node with the properties
   * @param attribute_name name of the attribute on the node
   * @param field_name field name to emit
   * @param field_prefix prefix to the field name
   * @param value_prefix prefix to the value assignment text
   * @param dflt if not provided, and not null then use this as the default
   */
  void emit_property(std::ofstream& out, const pugi::xml_node& node,
                     const char* attribute_name, const char* field_name,
                     const char* field_prefix = nullptr,
                     const char* value_prefix = nullptr, const char* dflt = nullptr);

  /**
   * Emit a property that takes the address of an value
   * @param out stream
   * @param node node with the properties
   * @param attribute_name name of the node property
   * @param field_name name of the generated field
   * @param field_prefix optional string to prefix the attribute name with
   */
  void emit_address_property(std::ofstream& out,
                             const pugi::xml_node& node,
                             const char* attribute_name,
                             const char* field_name,
                             const char* field_prefix = nullptr)
    {
    emit_property(out, node, attribute_name, field_name, field_prefix, "&");
    }

  /**
   * emit a string property
   * @param out stream
   * @param node node with the properties
   * @param attribute name of the attribute on the node
   * @param field_name field name to emit
   * @param field_prefix prefix to the field name
   * @param value_prefix prefix to the value assignment text
   * @param dflt if not provided, and not null then use this as the default
   */
  void emit_string_property(std::ofstream& out, const pugi::xml_node& node,
                            const char* attribute, const char* field_name,
                            const char* field_prefix = "",
                            const char* value_prefix = "", const char* dflt = nullptr);

  const XmlParser& parser_;
  std::string app_name_;
  int indent_level_ = 0;
  };

extern pugi::xml_attribute get_named_attribute(const pugi::xml_node& node,
                                               const char* name);

inline std::string make_literal(const char* s1, const char* s2,
                                const char* s3 = nullptr)
  {
  std::string result(s1);
  if (s2 != nullptr)
    result += s2;
  if (s3 != nullptr)
    result += s3;

  return result;
  }

#endif // CODE_GENERATOR_H
