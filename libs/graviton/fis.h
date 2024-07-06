#ifndef __fis_h__
#define __fis_h__

#include "../graviton/schema.h"

typedef enum _e_notam_traffic {
  nt_unknown,
  nt_ifr,
  nt_vfr,
  nt_ifr_vfr,
  nt_checklist,
  } e_notam_traffic;

typedef enum _e_notam_scope {
  ns_unknown = 0x0000,
  ns_aerodrome = 0x0001,
  ns_enroute = 0x0002,
  ns_warning = 0x0004,
  nz_checklist = 0x0008,
  ns_aerodrome_enroute = 0x0003,
  ns_aerodrome_warning = 0x0005,
  } e_notam_scope;

typedef enum _e_notam_purpose {
  np_unknown = 0x0000,
  np_air_operators = 0x0001,  // N
  np_briefing = 0x0002,       // B
  np_operations = 0x0004,     // O
  np_miscellaneous = 0x0008,  // M
  np_checklist = 0x0010,      // K
  } e_notam_purpose;

// this file holds all of the flight information received.
typedef struct _fir_briefing_t fir_briefing_t;

typedef struct _notam_t {
  fir_briefing_t *parent;
  e_notam_traffic traffic;
  e_notam_scope scope;
  e_notam_purpose purpose;

  spatial_point_t position;
  uint32_t radius;          // is meters * 100
  uint16_t lower_flight_level;
  uint16_t upper_flight_level;
  icao_id_t nof;
  uint16_t series;
  uint16_t year;
  //icao_ids_t item_a;
  char *item_d;
  char *item_e;
  char *item_f;
  char *item_g;
  char *code_23;
  char *coide_45;
  icao_id_t fir;
  tm_t effective_from;
  tm_t effective_to;
  } notam_t;

typedef notam_t *notam_p;
vector_t(notam_p);

/**
 * @brief A briefing for a given area.
*/
typedef struct _fir_briefing_t {
  icao_id_t fir;
  notam_ps_t notams;
  tm_t last_changed;
  tm_t effective_from;
  } fir_briefing_t;

#endif
