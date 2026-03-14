#include "code_generator.h"
#include <string.h>

void code_generator::create_variant(std::ofstream& out, const pugi::xml_node& node, const char* member_name,
                                    const char* value_type, const char* value, const char* prefix)
  {
  out << prefix << member_name << ".vt = " << value_type << "," << std::endl;

  if (strcmp(value_type, "v_bool") == 0)
    out << prefix << member_name << ".value.boolean = " << value << "," << std::endl;
  else if (strcmp(value_type, "v_int8") == 0)
    out << prefix << member_name << ".value.int8 = " << value << "," << std::endl;
  else if (strcmp(value_type, "v_uint8") == 0)
    out << prefix << member_name << ".value.uint8 = " << value << "," << std::endl;
  else if (strcmp(value_type, "v_int16") == 0)
    out << prefix << member_name << ".value.int16 = " << value << "," << std::endl;
  else if (strcmp(value_type, "v_uint16") == 0)
    out << prefix << member_name << ".value.uint16 = " << value << "," << std::endl;
  else if (strcmp(value_type, "v_int32") == 0)
    out << prefix << member_name << ".value.int32 = " << value << "," << std::endl;
  else if (strcmp(value_type, "v_uint32") == 0)
    out << prefix << member_name << ".value.uint32 = " << value << "," << std::endl;
  else if (strcmp(value_type, "v_float") == 0)
    out << prefix << member_name << ".value.flt = " << value << "," << std::endl;
  else
    out << "#error the default initializer type of " << value << " is not supported." << std::endl;
  }
