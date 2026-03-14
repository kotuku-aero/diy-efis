/*
 * definitions_generator.h - Generates private header files (_priv.h)
 * Replaces definitions.xsl
 */

#ifndef DEFINITIONS_GENERATOR_H
#define DEFINITIONS_GENERATOR_H

#include "xml_parser.h"
#include <string>
#include <fstream>

/**
 * Generates the private header file (_priv.h) from XML
 * Contains:
 *  - ID definitions for actions
 *  - External function declarations referenced in XML
 */
class DefinitionsGenerator {
public:
    DefinitionsGenerator(const XmlParser& parser, const std::string& app_name)
        : parser_(parser), app_name_(app_name) {}

    bool generate(const std::string& output_file);

private:
    void generate_id_definitions(std::ofstream& out);
    void generate_spin_edit_methods(std::ofstream& out);
    void generate_checklist_methods(std::ofstream& out);
    void generate_event_methods(std::ofstream& out);
    void generate_alarm_methods(std::ofstream& out);
    void generate_callback_methods(std::ofstream& out);

    const XmlParser& parser_;
    std::string app_name_;
};

#endif // DEFINITIONS_GENERATOR_H
