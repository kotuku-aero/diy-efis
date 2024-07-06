#include "neutron.h"

#include <ctype.h>
#include <errno.h>

#define CANFLYID(id, n, t, s) { #id , n },

const enum_t can_ids[] = {

#include "../../resources/CanFlyID.def"

    { 0,0 }
  };

result_t convert_string_to_enum(const char * token, const enum_t *enums, uint16_t *value)
  {
  if (token == 0 || value == 0)
    return e_bad_parameter;

  if (strlen(token) == 0)
    {
    *value = 0;
    return e_unexpected; /* optional argument wasn't provided */
    }

  if (isdigit(*token))
    {
    // numeric
    errno = 0;
    uint32_t v32 = strtoul(token, 0, 0);

    if (errno != 0)
      return e_bad_parameter;

    if (v32 > UINT16_MAX)
      return e_bad_parameter;

    *value = (uint16_t)v32;
    return s_ok;
    }

  // look up string...
  const enum_t *id;
  for (id = enums; id->name != 0; id++)
    {
    if (strcmp(token, id->name) == 0)
      {
      *value = id->value;
      return s_ok;
      }
    }
  return e_not_found;
  }
