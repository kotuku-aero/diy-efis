/*
 * header_generator.cpp - Generates public header files (.h)
 */

#include "header_generator.h"
#include <iostream>
#include <algorithm>

bool HeaderGenerator::generate(const std::string& output_file) {
    std::ofstream out(output_file);
    if (!out) {
        std::cerr << "Failed to open output file: " << output_file << "\n";
        return false;
    }

    // Convert app_name to uppercase for header guard
    std::string guard = app_name_;
    std::transform(guard.begin(), guard.end(), guard.begin(), ::toupper);

    // Generate header file (matches factory.xsl output)
    out << "#ifndef __" << app_name_ << "_h__\n";
    out << "#define __" << app_name_ << "_h__\n";
    out << "\n";
    out << "#include \"../../src/proton/include/menu_window.h\"\n";
    out << "\n";
    out << "\n";
    out << "#ifdef __cplusplus\n";
    out << "extern \"C\" {\n";
    out << "#endif\n";
    out << "\n";
    out << "extern result_t create_" << app_name_ << "(handle_t hwnd);\n";
    out << "\n";
    out << "#ifdef __cplusplus\n";
    out << "}\n";
    out << "#endif\n";
    out << "\n";
    out << "#endif\n";

    out.close();
    return true;
}
