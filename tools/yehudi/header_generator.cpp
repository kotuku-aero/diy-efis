/*
 * header_generator.cpp - Generates public header files (.h)
 */

#include "header_generator.h"
#include "code_generator.h"
#include <algorithm>
#include <iostream>

bool HeaderGenerator::generate(const std::string &output_file)
  {


    std::ofstream out(output_file);
  if (!out)
    {
    std::cerr << "Failed to open output file: " << output_file << ""  << std::endl;
        return false;
    }

    // Convert app_name to uppercase for header guard
    std::string guard = app_name_;
    std::transform(guard.begin(), guard.end(), guard.begin(), ::toupper);

    // Generate header file (matches factory.xsl output)
  out << "#ifndef __" << app_name_ << "_h__"  << std::endl;
  out << "#define __" << app_name_ << "_h__"  << std::endl;
  out << std::endl;
    out << "#include \"../../src/proton/include/menu_window.h\"" << std::endl;
  out << std::endl;
  out << std::endl;
  out << "#ifdef __cplusplus"  << std::endl;
  out << "extern \"C\" {"  << std::endl;
  out << "#endif"  << std::endl;
  out << std::endl;
  out << "extern result_t create_" << app_name_ << "(handle_t hwnd);"  << std::endl;
  out << std::endl;
  out << "#ifdef __cplusplus"  << std::endl;
  out << "}"  << std::endl;
  out << "#endif"  << std::endl;
  out << std::endl;
  out << "#endif"  << std::endl;

    out.close();
    return true;
}
