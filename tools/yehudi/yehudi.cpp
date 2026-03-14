/*
 * Yehudi - XML to C Code Generator
 * 
 * Replaces XSLT-based code generation with maintainable C++ code.
 * Generates three files from XML layout definitions:
 *   - <name>.c       - Implementation file
 *   - <name>.h       - Public header
 *   - <name>_priv.h  - Private header with forward declarations
 * 
 * Usage: yehudi -a=<name> <xml_file> [-o <output_dir>]
 *
 * Debug arguments:
 * -a=edd4_us "C:\Projects\canfly2\apps\kEDD\edd4_us.xml"
 */

#include "xml_parser.h"
#include "code_generator.h"
#include "header_generator.h"
#include "definitions_generator.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <cstring>

namespace fs = std::filesystem;

struct Arguments {
    std::string xml_file;
    std::string app_name;
    std::string output_dir;
    bool verbose = false;
};

void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " -a=<name> <xml_file> [options]\n"
              << "\n"
              << "Options:\n"
              << "  -a=<name>         Application name (required)\n"
              << "  -o=<dir>          Output directory (default: same as XML file)\n"
              << "  -v, --verbose     Verbose output\n"
              << "  -h, --help        Show this help\n"
              << "\n"
              << "Example:\n"
              << "  " << program_name << " -a=edd4_us edd4_us.xml\n"
              << "\n"
              << "Generates:\n"
              << "  edd4_us.c         Implementation file\n"
              << "  edd4_us.h         Public header\n"
              << "  edd4_us_priv.h    Private header\n";
}

bool parse_arguments(int argc, char* argv[], Arguments& args) {
    if (argc < 3) {
        return false;
    }

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            return false;
        }
        else if (arg == "-v" || arg == "--verbose") {
            args.verbose = true;
        }
        else if (arg.substr(0, 3) == "-a=") {
            args.app_name = arg.substr(3);
        }
        else if (arg.substr(0, 3) == "-o=") {
            args.output_dir = arg.substr(3);
        }
        else if (arg[0] != '-') {
            args.xml_file = arg;
        }
        else {
            std::cerr << "Unknown option: " << arg << "\n";
            return false;
        }
    }

    // Validate required arguments
    if (args.app_name.empty()) {
        std::cerr << "Error: Application name (-a=<name>) is required\n";
        return false;
    }

    if (args.xml_file.empty()) {
        std::cerr << "Error: XML file is required\n";
        return false;
    }

    // Check if XML file exists
    if (!fs::exists(args.xml_file)) {
        std::cerr << "Error: XML file not found: " << args.xml_file << "\n";
        return false;
    }

    // Default output directory is same as XML file
    if (args.output_dir.empty()) {
        fs::path xml_path(args.xml_file);
        args.output_dir = xml_path.parent_path().string();
        if (args.output_dir.empty()) {
            args.output_dir = ".";
        }
    }

    return true;
}

int main(int argc, char* argv[]) {
    Arguments args;

    if (!parse_arguments(argc, argv, args)) {
        print_usage(argv[0]);
        return 1;
    }

    try {
        if (args.verbose) {
            std::cout << "Yehudi C++ Code Generator\n";
            std::cout << "=========================\n";
            std::cout << "XML file:     " << args.xml_file << "\n";
            std::cout << "App name:     " << args.app_name << "\n";
            std::cout << "Output dir:   " << args.output_dir << "\n";
            std::cout << "\n";
        }

        // Parse XML file
        if (args.verbose) {
            std::cout << "Parsing XML file...\n";
        }

        XmlParser parser;
        if (!parser.load(args.xml_file)) {
            std::cerr << "Error: Failed to parse XML file\n";
            return 1;
        }

        // Create output directory if it doesn't exist
        fs::create_directories(args.output_dir);

        // Generate output file paths
        fs::path output_path(args.output_dir);
        std::string c_file = (output_path / (args.app_name + ".c")).string();
        std::string h_file = (output_path / (args.app_name + ".h")).string();
        std::string priv_h_file = (output_path / (args.app_name + "_priv.h")).string();

        // Generate the three files
        if (args.verbose) {
            std::cout << "Generating files...\n";
        }

        // 1. Generate public header (.h)
        HeaderGenerator header_gen(parser, args.app_name);
        if (!header_gen.generate(h_file)) {
            std::cerr << "Error: Failed to generate " << h_file << "\n";
            return 1;
        }
        if (args.verbose) {
            std::cout << "  Generated: " << h_file << "\n";
        }

        // 2. Generate private header (_priv.h)
        DefinitionsGenerator defs_gen(parser, args.app_name);
        if (!defs_gen.generate(priv_h_file)) {
            std::cerr << "Error: Failed to generate " << priv_h_file << "\n";
            return 1;
        }
        if (args.verbose) {
            std::cout << "  Generated: " << priv_h_file << "\n";
        }

        // 3. Generate implementation (.c)
        code_generator code_gen(parser, args.app_name);
        if (!code_gen.generate(c_file)) {
            std::cerr << "Error: Failed to generate " << c_file << "\n";
            return 1;
        }
        if (args.verbose) {
            std::cout << "  Generated: " << c_file << "\n";
        }

        std::cout << "Success! Generated 3 files:\n";
        std::cout << "  - " << args.app_name << ".c\n";
        std::cout << "  - " << args.app_name << ".h\n";
        std::cout << "  - " << args.app_name << "_priv.h\n";

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
