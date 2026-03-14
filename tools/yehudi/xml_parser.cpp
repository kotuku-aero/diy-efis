/*
 * xml_parser.cpp - XML parsing implementation
 */

#include "xml_parser.h"
#include <iostream>

bool XmlParser::load(const std::string& filename) {
    filename_ = filename;
    
    pugi::xml_parse_result result = doc_.load_file(filename.c_str());
    
    if (!result) {
        std::cerr << "XML parse error: " << result.description() << "\n";
        std::cerr << "  at offset: " << result.offset << "\n";
        return false;
    }
    
    return true;
}

std::vector<pugi::xml_node> XmlParser::find_all(const char* xpath) const {
    std::vector<pugi::xml_node> results;
    
    pugi::xpath_node_set nodes = doc_.select_nodes(xpath);
    for (auto& node : nodes) {
        results.push_back(node.node());
    }
    
    return results;
}

pugi::xml_node XmlParser::find_one(const char* xpath) const {
    return doc_.select_node(xpath).node();
}

std::string XmlParser::get_attr(const pugi::xml_node& node, const char* name, const std::string& default_val) const {
    pugi::xml_attribute attr = node.attribute(name);
    return attr ? attr.value() : default_val;
}

bool XmlParser::has_attr(const pugi::xml_node& node, const char* name) const {
    return node.attribute(name) != nullptr;
}
