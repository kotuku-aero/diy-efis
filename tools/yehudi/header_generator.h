/*
 * header_generator.h - Generates public header files (.h)
 * Replaces factory.xsl
 */

#ifndef HEADER_GENERATOR_H
#define HEADER_GENERATOR_H

#include "xml_parser.h"
#include <string>
#include <fstream>

/**
 * Generates the public header file (.h) from XML
 * This is a simple file that just declares the create function
 */
class HeaderGenerator {
public:
    HeaderGenerator(const XmlParser& parser, const std::string& app_name)
        : parser_(parser), app_name_(app_name) {}

    bool generate(const std::string& output_file);

private:
    const XmlParser& parser_;
    std::string app_name_;
};

#endif // HEADER_GENERATOR_H
