/*
 * xml_parser.h - XML parsing using pugixml
 */

#ifndef XML_PARSER_H
#define XML_PARSER_H

#include <string>
#include <vector>
#include <memory>
#include "pugixml.hpp"

// Forward declarations for element types
class XmlElement;

/**
 * XML Parser wrapper around pugixml
 * Loads and provides access to XML application definitions
 */
class XmlParser {
public:
    XmlParser() = default;
    ~XmlParser() = default;

    // Load XML from file
    bool load(const std::string& filename);

    // Get the root application element
    pugi::xml_node get_application() const;

    // Query helpers - find all elements matching XPath
    std::vector<pugi::xml_node> find_all(const char* xpath) const;

    // Find single element
    pugi::xml_node find_one(const char* xpath) const;

    // Get attribute value with default
    std::string get_attr(const pugi::xml_node& node, const char* name, const std::string& default_val = "") const;

    // Check if attribute exists
    bool has_attr(const pugi::xml_node& node, const char* name) const;

    // Get the raw document
    const pugi::xml_document& document() const { return doc_; }

private:
    pugi::xml_document doc_;
    std::string filename_;
};

#endif // XML_PARSER_H
