/*
diy-efis
Copyright (C) 2016 Kotuku Aerospace Limited

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice and
the GPL3 are subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.


If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the origional copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
#include "ahrs.h"
#include "../dspic33f-lib/can_driver.h"
#include "../dspic-lib/publisher.h"
#include "gps.h"
#include <math.h>

typedef struct
  {
  float x;
  float y;
  } point_t;

typedef struct
  {
  const point_t *P0;
  const point_t *P1;
  }  line_t;
  
// Assume that classes are already given for the objects:
//     Point and Vector with
//          coordinates {float x, y, z;} (z=0  for 2D)
//          appropriate operators for:
//               Point  = Point ± Vector
//               Vector = Point - Point
//               Vector = Scalar * Vector
//     Line with defining endpoints {Point P0, P1;}
//===================================================================

// dot product (3D) which allows vector operations in arguments
static float dot(const point_t *u, const point_t *v)
  {
  return u->x * v->x + u->y * v->y;
  }
  
// norm = length of  vector
static float norm(const point_t *v)
  {
  return sqrt(dot(v, v));
  }
  
static void to_vector(const point_t *u, const point_t *v, point_t *r)
  {
  r->x = v->x - u->x;
  r->y = v->y - u->y;
  }
  
static float distance(const point_t *u, const point_t *v)
  {
  point_t r;
  to_vector(u, v, &r);
  return norm(&r);
  }
  
static void add_vector(const point_t *p, const point_t *v, point_t *r)
  {
  r->x = p->x + v->x;
  r->y = p->y + v->y;
  }
  
static void multiply_vector(float s, const point_t *v, point_t *r)
  {
  r->x = v->x * s;
  r->y = v->y * s;
  }
  
// distance = norm of difference
//#define dot(u,v)   ((u).x * (v).x + (u).y * (v).y + (u).z * (v).z)
//#define norm(v)     sqrt(dot(v,v))     // norm = length of  vector
//#define d(u,v)      norm(u-v)          // distance = norm of difference

// dist_Point_to_Line(): get the distance of a point to a line
//     Input:  a Point P and a Line L (in any dimension)
//     Return: the shortest distance from P to L
static float dist_point_to_line(const point_t *p, const line_t *l)
  {
  point_t v;
  point_t w;
  float c1;
  float c2;
  float b;
  point_t pb;
  point_t v2;
  
  to_vector(l->P1, l->P0, &v);
  to_vector(p, l->P0, &w);
  
  c1 = dot(&w, &v);
  c2 = dot(&v, &v);
  b = c1 / c2;
  
  multiply_vector(b, &v, &v2);
  add_vector(l->P0, &v2, &pb);
  
  return distance(p, &pb);
  }
  
//
// routine to calculate the angle between two points, relative to true north (0,0)
static short angle_of_vector(const line_t *l)
  {
  return 0;  // TODO:
  }

static float distance_to_next;
static float distance_to_destination;
static float track;
static float deviation;
static float track_angle_error;
static interval_t estimated_time_to_go; // time in seconds
static interval_t estimated_time_of_arrival;
static interval_t estimated_time_to_next;
static float desired_track_angle;
static char current_waypoint; // current waypoint in the list
static semaphore_t navigation_semp;

#define MAXROUTEWAYPOINTS 16
// this is our current GPS track definition.

struct _waypoint_t
  {
  char icao[16]; // the 16 bits of the waypoint type are in the bit 7 of each byte so only 128 ascii chars
  point_t position; // x = latitude, y = longitude
  short min_altitude;
  short max_altitude;
  };
// structure is 28 bytes
typedef struct _waypoint_t waypoint_t;

// TODO: store this in the config memory...
// we support a route of up to 16 waypoints (448 bytes)
static waypoint_t waypoints[MAXROUTEWAYPOINTS];

static bool nav_active;
static int next_waypoint = -1;
// we publish the active route whenever it changes, or every 10 seconds
static int publish_timeout = -1;

static void *compute_address(int waypoint_number, int offset)
  {
  return ((unsigned char *) waypoints + waypoint_number) +offset;
  }

static uint16_t read_waypoint_word(int waypoint_number, int byte_offset)
  {
  return *((uint16_t *) compute_address(waypoint_number, byte_offset));
  }

// return true if a waypoint is defined

static bool waypoint_defined(int waypoint_number)
  {
  return read_waypoint_word(waypoint_number, 0) != 0;
  }

static void write_waypoint_word(int waypoint_number, int byte_offset, uint16_t word)
  {
  *((uint16_t *) compute_address(waypoint_number, byte_offset)) = word;
  }

static float read_waypoint_float(int waypoint_number, int byte_offset)
  {
  uint16_t value[2];
  value[0] = read_waypoint_word(waypoint_number, byte_offset);
  value[1] = read_waypoint_word(waypoint_number, byte_offset + sizeof (uint16_t));

  return *((float *) &value[0]);
  }

static void write_waypoint_float(int waypoint_number, int byte_offset, float value)
  {
  uint16_t *value_p = (uint16_t *) & value;
  write_waypoint_word(waypoint_number, byte_offset, value_p[0]);
  write_waypoint_word(waypoint_number, byte_offset + sizeof (uint16_t), value_p[1]);
  }

static short read_waypoint_short(int waypoint_number, int byte_offset)
  {
  return (short) read_waypoint_word(waypoint_number, byte_offset);
  }

static void write_waypoint_short(int waypoint_number, int byte_offset, short value)
  {
  write_waypoint_word(waypoint_number, byte_offset, (uint16_t) value);
  }

static uint16_t read_waypoint_type(int waypoint_number)
  {
  uint16_t bytes_read;
  uint16_t result = 0;

  bytes_read = read_waypoint_word(waypoint_number, 0);
  result = (((bytes_read & 0x8000) >> 15) & 0x0001) | (((bytes_read & 0x0080) >> 6) & 0x0002);

  bytes_read = read_waypoint_word(waypoint_number, 2);
  result = (((bytes_read & 0x8000) >> 13) & 0x0004) | (((bytes_read & 0x0080) >> 4) & 0x0008);

  bytes_read = read_waypoint_word(waypoint_number, 4);
  result = (((bytes_read & 0x8000) >> 11) & 0x0010) | (((bytes_read & 0x0080) >> 2) & 0x0020);

  bytes_read = read_waypoint_word(waypoint_number, 6);
  result = (((bytes_read & 0x8000) >> 9) & 0x0040) | (bytes_read & 0x0080);

  bytes_read = read_waypoint_word(waypoint_number, 8);
  result = (((bytes_read & 0x8000) >> 7) & 0x0100) | (((bytes_read & 0x0080) << 2) & 0x0200);

  bytes_read = read_waypoint_word(waypoint_number, 10);
  result = (((bytes_read & 0x8000) >> 5) & 0x0400) | (((bytes_read & 0x0080) << 4) & 0x0800);

  bytes_read = read_waypoint_word(waypoint_number, 12);
  result = (((bytes_read & 0x8000) >> 3) & 0x1000) | (((bytes_read & 0x0080) << 6) & 0x2000);

  bytes_read = read_waypoint_word(waypoint_number, 14);
  result = (((bytes_read & 0x8000) >> 1) & 0x4000) | (((bytes_read & 0x0080) << 8) & 0x8000);

  return result;
  }

static void write_waypoint_type(int waypoint_number, uint16_t value)
  {
  uint16_t merge_bits;
  // each bit is stored in the high bit of a word
  merge_bits = ((value & 0x0001) << 15) | ((value & 0x0002) << 6);
  write_waypoint_word(waypoint_number, 0, (read_waypoint_word(waypoint_number, 0) & 0x7F7F) | merge_bits);

  merge_bits = ((value & 0x0004) << 13) | ((value & 0x0008) << 4);
  write_waypoint_word(waypoint_number, 2, (read_waypoint_word(waypoint_number, 2) & 0x7F7F) | merge_bits);

  merge_bits = ((value & 0x0010) << 11) | ((value & 0x0020) << 2);
  write_waypoint_word(waypoint_number, 4, (read_waypoint_word(waypoint_number, 4) & 0x7F7F) | merge_bits);

  merge_bits = ((value & 0x0040) << 9) | (value & 0x0080);
  write_waypoint_word(waypoint_number, 6, (read_waypoint_word(waypoint_number, 6) & 0x7F7F) | merge_bits);

  merge_bits = ((value & 0x0100) << 7) | ((value & 0x0200) >> 2);
  write_waypoint_word(waypoint_number, 8, (read_waypoint_word(waypoint_number, 8) & 0x7F7F) | merge_bits);

  merge_bits = ((value & 0x0400) << 5) | ((value & 0x0800) >> 4);
  write_waypoint_word(waypoint_number, 10, (read_waypoint_word(waypoint_number, 10) & 0x7F7F) | merge_bits);

  merge_bits = ((value & 0x1000) << 3) | ((value & 0x2000) >> 6);
  write_waypoint_word(waypoint_number, 12, (read_waypoint_word(waypoint_number, 12) & 0x7F7F) | merge_bits);

  merge_bits = ((value & 0x4000) << 1) | ((value & 0x8000) >> 8);
  write_waypoint_word(waypoint_number, 14, (read_waypoint_word(waypoint_number, 14) & 0x7F7F) | merge_bits);
  }

static char read_waypoint_char(int waypoint_number, int char_number)
  {
  uint16_t bytes;

  bytes = read_waypoint_word(waypoint_number, char_number & 0x0E);

  // we are big endian around here...
  if ((char_number & 0x01) == 1)
    return (char) bytes & 0x7F;

  return (bytes >> 8) & 0x7f;
  }

static void write_waypoint_char(int waypoint_number, int char_number, char value)
  {
  uint16_t bytes;
  uint16_t value2 = value & 0x7F; // make sure high bit not set

  bytes = read_waypoint_word(waypoint_number, char_number & 0x0E);


  // we are big endian around here...
  if ((char_number & 0x01) == 1)
    {
    bytes &= 0xFF80;
    bytes |= value2;
    }
  else
    {
    bytes &= 0x80FF;
    bytes |= value2 << 8;
    }

  write_waypoint_word(waypoint_number, char_number & 0x0E, bytes);
  }

static int calculate_start_waypoint()
  {
  return 0; // always start at the begining (for now!)
  }

// routine to calculate the navigation values
//
// This routine is called every second to calculate where we are in space,
// and how far away from the planned track we are.
// these values are then published.  The routine does not directly publish them as they are
// send evenly within a quanta slot.
//
// The values computed are:
//
// id_distance_to_next                  distance to next waypoint
// id_distance_to_destination           distance to final waypoint
// id_track                             computed track to fly to arrive at destination including drift
// id_deviation                         how far off the track the aircraft is.
// id_track_angle_error                 angle between the start of the track and current position and desired position
// id_estimated_time_to_go              time at current groundspeed to get to destination
// id_estimated_time_of_arrival         current time plus estimated time to go
// id_estimated_time_to_next            time to get to final destination, corrected for wind and airsped
// id_desired_track_angle               computer track, exclusing drift correction
//
// the AHRS is a navigation tool.  The route is loaded into eeprom memory and
// is then displayed.  the AHRS assumes that if the aircraft is within 1nm of a waypoint then it
// is at that waypoint.
// When first started or when a new loadplan is loaded the ahrs looks at
// the current fix and calculates a route to the first fix.
// Once the aircraft is within 1nm (configurable) the fix is assumed to have been
// achieved and the next fix becomes current.
// the autopilot (or pilot) will then command the aircraft onto the next fix.
// note that if the aircraft is within 1nm (configurable) of the track it is assumed
// to be on-track
//
static point_t start_position;

static void compute_navigation()
  {
  if (!nav_active)
    return; // do nothing

  // this is called when a route is activated.
  // we need to find the nearest waypoint to the aircraft and set that as the
  // next waypoint.
  point_t current_position = {
                              ahrs_state.gps_position.lat,
                              ahrs_state.gps_position.lng
    };

  point_t *next_waypoint_pt = (point_t *) compute_address(next_waypoint, offsetof(waypoint_t, position));

  int waypoint_it;
  line_t sector;
  float desired_track;

  distance_to_next = distance(&current_position, next_waypoint_pt);

  // if the distance to the waypoint is < 1nm then we use the next waypoint.
  if (distance_to_next <= 1852)
    {
    // move next (if there is one)
    if (next_waypoint < 16 &&
        waypoint_defined(next_waypoint + 1))
      {
      next_waypoint++; // advance next
      next_waypoint_pt = (point_t *) compute_address(next_waypoint, offsetof(waypoint_t, position));

      distance_to_next = distance(&current_position, next_waypoint_pt);
      }
    }

  // calculate the total distance to go
  distance_to_destination = distance_to_next;

  // calculate the route points
  sector.P0 = next_waypoint == 0 ? &start_position : next_waypoint_pt;

  for (waypoint_it = next_waypoint + 1; waypoint_it < 16 && waypoint_defined(waypoint_it); waypoint_it++)
    {
    // set the end of the sector to the next point
    sector.P1 = (point_t *) compute_address(waypoint_it, offsetof(waypoint_t, position));
    distance_to_destination += distance(sector.P0, sector.P1);
    sector.P0 = sector.P1; // move along
    }

  // calculate the time to destination based on the current time and groundspeed
  estimated_time_to_next = distance_to_next / ahrs_state.gps_ground_speed;
  estimated_time_to_go = distance_to_destination / ahrs_state.gps_ground_speed;
  estimated_time_of_arrival = add_interval(ahrs_state.time, estimated_time_to_go, 0);

  // track deviation answers.
  // id_desired_track_angle
  // id_track                             computed track to fly to arrive at destination including drift
  // id_deviation                         how far off the track the aircraft is.
  // id_track_angle_error                 angle between the start of the track and current position and desired position

  sector.P0 = next_waypoint == 0 ? &start_position : next_waypoint_pt;
  sector.P1 = (point_t *) compute_address(waypoint_it, offsetof(waypoint_t, position));

  // calculate how far from the track we are.
  deviation = dist_point_to_line(&current_position, &sector);

  // the desired track angle is the angle from the aircraft to the destination
  desired_track_angle = angle_of_vector(&sector);
  // the track is the angle to fly allowing for wind direction and speed
  sector.P1 = next_waypoint_pt;
  sector.P0 = &current_position;

  desired_track = angle_of_vector(&sector);

  // add the wind to get the true heading corrected for wind
  }

static void deactivate_route()
  {
  nav_active = false;
  }

static void clear_route()
  {
  int waypoint;
  int offset;
  deactivate_route();

  for (waypoint = 0; waypoint < MAXROUTEWAYPOINTS; waypoint++)
    for (offset = 0; offset < sizeof (waypoint_t); offset += sizeof (uint16_t))
      write_waypoint_word(waypoint, offset, 0);
  }

static void activate_route()
  {
  nav_active = waypoint_defined(0);

  next_waypoint = calculate_start_waypoint();
  start_position.x = ahrs_state.gps_position.lat;
  start_position.y = ahrs_state.gps_position.lng;
  }

static void invert_route()
  {
  int num_waypoints;
  int first_waypoint;
  int last_waypoint;
  bool was_active = nav_active;

  nav_active = false;

  // find how many waypoint segments there are
  for (num_waypoints = 0; num_waypoints < MAXROUTEWAYPOINTS; num_waypoints++)
    if (!waypoint_defined(num_waypoints))
      break;

  for (first_waypoint = 0, last_waypoint = num_waypoints - 1; first_waypoint < last_waypoint; first_waypoint++, last_waypoint--)
    {
    int waypoint_offset;
    uint16_t last_word;

    for (waypoint_offset = 0; waypoint_offset < sizeof (waypoint_t); waypoint_offset += sizeof (uint16_t))
      {
      last_word = read_waypoint_word(last_waypoint, waypoint_offset);
      write_waypoint_word(last_waypoint, waypoint_offset, read_waypoint_word(first_waypoint, waypoint_offset));
      write_waypoint_word(first_waypoint, waypoint_offset, last_word);
      }
    }

  nav_active = was_active;
  }

static void can_callback(const can_msg_t *msg)
  {
  switch (msg->id)
    {
    case id_qnh_up:
      break;
    case id_qnh_dn:
      break;
    case id_nav_command:
      if (msg->msg.canas.service_code != 0)
        break; // we only work on the current route
      switch (get_param_short(msg))
        {
        case 0:
          clear_route();
          break;
        case 1:
          activate_route();
          break;
        case 2:
          deactivate_route();
          break;
        case 3:
          invert_route();
          break;
        }
      break;
    case id_waypoint_identifier_0_3:
      if (nav_active)
        break; // we only work on the current route, and if the route is not active

      write_waypoint_char(msg->msg.canas.service_code, 0, msg->msg.canas.data[0]);
      write_waypoint_char(msg->msg.canas.service_code, 1, msg->msg.canas.data[1]);
      write_waypoint_char(msg->msg.canas.service_code, 2, msg->msg.canas.data[2]);
      write_waypoint_char(msg->msg.canas.service_code, 3, msg->msg.canas.data[3]);
      break;
    case id_waypoint_identifier_4_7:
      if (nav_active)
        break; // we only work on the current route, and if the route is not active

      write_waypoint_char(msg->msg.canas.service_code, 4, msg->msg.canas.data[0]);
      write_waypoint_char(msg->msg.canas.service_code, 5, msg->msg.canas.data[1]);
      write_waypoint_char(msg->msg.canas.service_code, 6, msg->msg.canas.data[2]);
      write_waypoint_char(msg->msg.canas.service_code, 7, msg->msg.canas.data[3]);
      break;
    case id_waypoint_identifier_8_11:
      if (nav_active)
        break; // we only work on the current route, and if the route is not active

      write_waypoint_char(msg->msg.canas.service_code, 8, msg->msg.canas.data[0]);
      write_waypoint_char(msg->msg.canas.service_code, 9, msg->msg.canas.data[1]);
      write_waypoint_char(msg->msg.canas.service_code, 10, msg->msg.canas.data[2]);
      write_waypoint_char(msg->msg.canas.service_code, 11, msg->msg.canas.data[3]);
      break;
    case id_waypoint_identifier_12_15:
      if (nav_active)
        break; // we only work on the current route, and if the route is not active

      write_waypoint_char(msg->msg.canas.service_code, 12, msg->msg.canas.data[0]);
      write_waypoint_char(msg->msg.canas.service_code, 13, msg->msg.canas.data[1]);
      write_waypoint_char(msg->msg.canas.service_code, 14, msg->msg.canas.data[2]);
      write_waypoint_char(msg->msg.canas.service_code, 15, msg->msg.canas.data[3]);
      break;
    case id_waypoint_type_identifier:
      if (nav_active)
        break; // we only work on the current route, and if the route is not active

      write_waypoint_type(msg->msg.canas.service_code, get_param_short(msg));
      break;
    case id_waypoint_latitude:
      if (nav_active)
        break; // we only work on the current route, and if the route is not active

      write_waypoint_float(msg->msg.canas.service_code, offsetof(waypoint_t, position.y), get_param_float(msg));
      break;
    case id_waypoint_longitude:
      if (nav_active)
        break; // we only work on the current route, and if the route is not active

      write_waypoint_float(msg->msg.canas.service_code, offsetof(waypoint_t, position.x), get_param_float(msg));
      break;
    case id_waypoint_minimum_altitude:
      if (nav_active)
        break; // we only work on the current route, and if the route is not active

      write_waypoint_short(msg->msg.canas.service_code, offsetof(waypoint_t, min_altitude), get_param_short(msg));
      break;
    case id_waypoint_maximum_altitude:
      if (nav_active)
        break; // we only work on the current route, and if the route is not active

      write_waypoint_short(msg->msg.canas.service_code, offsetof(waypoint_t, max_altitude), get_param_short(msg));
      break;
    }
  }

can_msg_t *create_can_msg_achar4(can_msg_t *msg, uint16_t message_id, uint8_t service_code, const char *value, uint8_t length)
  {
  uint8_t i;
  msg->flags = length + 4;
  msg->id = message_id;
  msg->msg.canas.data_type = CANAS_DATATYPE_UCHAR4;
  msg->msg.canas.service_code = service_code;
  for (i = 0; i < length; i++)
    msg->msg.canas.data[i] = value[i];

  return msg;
  }

static can_msg_t param;
static int last_waypoint_sent = -1;

static bool send_waypoint_name(int waypoint_number, int byte_offset, uint16_t id, int plan_waypoint)
  {
  char txt[4];
  uint8_t length = 1;
  txt[0] = read_waypoint_char(waypoint_number, byte_offset);
  if (txt[0] == 0)
    return false; // end of string

  txt[1] = read_waypoint_char(waypoint_number, byte_offset + 1);

  if (txt[1] != 0)
    {
    length++;
    txt[2] = read_waypoint_char(waypoint_number, byte_offset + 2);
    if (txt[2] != 0)
      {
      length++;
      txt[3] = read_waypoint_char(waypoint_number, byte_offset + 3);
      if (txt[3] != 0)
        length++;
      }
    }

  publish(create_can_msg_achar4(&param, id, waypoint_number, txt, length));

  return length == 4;
  }

// this routine will check to see if our next waypoint has changed, if so it
// publishes the active route from our next waypoint to the end.

static void update_flightplan_if_changed()
  {
  if (nav_active &&
      next_waypoint > -1 &&
      ((next_waypoint != last_waypoint_sent) || publish_timeout < 1))
    {
    // clear the active plan waypoint database.  Note that this is ignored
    // if we get a loopback as we are in active navigation mode.
    publish(create_can_msg_short(&param, id_nav_command, 0, 0));
    // update the nav display with all of the current waypoints
    // we only send this when our next waypoint changes or if the timeout occurs
    int active_plan_waypoint = 0;
    int waypoint_to_send;
    for (waypoint_to_send = next_waypoint;
         waypoint_to_send < MAXROUTEWAYPOINTS && waypoint_defined(waypoint_to_send);
         waypoint_to_send++)
      {
      if (send_waypoint_name(waypoint_to_send, 0, id_waypoint_identifier_0_3, active_plan_waypoint))
        if (send_waypoint_name(waypoint_to_send, 4, id_waypoint_identifier_4_7, active_plan_waypoint))
          if (send_waypoint_name(waypoint_to_send, 8, id_waypoint_identifier_8_11, active_plan_waypoint))
            send_waypoint_name(waypoint_to_send, 16, id_waypoint_identifier_12_15, active_plan_waypoint);

      publish(create_can_msg_short(&param, id_waypoint_type_identifier, active_plan_waypoint, read_waypoint_type(waypoint_to_send)));
      publish(create_can_msg_float(&param, id_waypoint_latitude, active_plan_waypoint, read_waypoint_float(waypoint_to_send, offsetof(waypoint_t, position.y))));
      publish(create_can_msg_float(&param, id_waypoint_longitude, active_plan_waypoint, read_waypoint_float(waypoint_to_send, offsetof(waypoint_t, position.x))));
      publish(create_can_msg_short(&param, id_waypoint_minimum_altitude, active_plan_waypoint, read_waypoint_short(waypoint_to_send, offsetof(waypoint_t, min_altitude))));
      publish(create_can_msg_short(&param, id_waypoint_maximum_altitude, active_plan_waypoint, read_waypoint_short(waypoint_to_send, offsetof(waypoint_t, max_altitude))));
      }

    publish_timeout = 10;
    }
  else if (nav_active)
    publish_timeout--;
  }

static void send_track()
  {
  // id_track                             computed track to fly to arrive at destination including drift
  publish(create_can_msg_short(&param, id_track, 0, (short) radians_to_degrees(track)));
  }

static void send_deviation()
  {
  // id_deviation                         how far off the track the aircraft is.
  // todo: make sure only +/- 16km
  publish(create_can_msg_short(&param, id_deviation, 0, (short) deviation));
  }

static void send_track_angle_error()
  {
  // id_track_angle_error                 angle between the start of the track and current position and desired position
  publish(create_can_msg_float(&param, id_track_angle_error, 0, radians_to_degrees(track_angle_error)));
  }

static void send_estimated_time_to_go()
  {
  // id_estimated_time_to_go              time at current groundspeed to get to destination

  // est time is in seconds so build into an char3 as hrs/min/sec
  uint8_t data[3];
  data[2] = (uint8_t) (estimated_time_to_go % 60);
  data[1] = (uint8_t) ((estimated_time_to_go % 3600) / 60);
  data[0] = (uint8_t) (estimated_time_to_go / 3600);

  publish(create_can_msg_uchar3(&param, id_estimated_time_to_next, 0, data));
  }

static void send_estimated_time_of_arrival()
  {
  // id_estimated_time_of_arrival         current time plus estimated time to go

  // est time is in seconds so build into an char3 as hrs/min/sec
  uint8_t data[3];
  data[2] = (uint8_t) (estimated_time_of_arrival % 60);
  data[1] = (uint8_t) ((estimated_time_of_arrival % 3600) / 60);
  data[0] = (uint8_t) (estimated_time_of_arrival / 3600);

  publish(create_can_msg_uchar3(&param, id_estimated_time_of_arrival, 0, data));
  }

static void send_estimated_time_to_next()
  {
  // id_estimated_time_to_next            time to get to final destination, corrected for wind and airsped

  // est time is in seconds so build into an char3 as hrs/min/sec
  uint8_t data[3];
  data[2] = (uint8_t) (estimated_time_to_next % 60);
  data[1] = (uint8_t) ((estimated_time_to_next % 3600) / 60);
  data[0] = (uint8_t) (estimated_time_to_next / 3600);

  publish(create_can_msg_uchar3(&param, id_estimated_enroute_time, 0, data));
  }

static void send_desired_track_angle()
  {
  // id_desired_track_angle               computer track, exclusing drift correction
  publish(create_can_msg_short(&param, id_desired_track_angle, 0, (short) radians_to_degrees(desired_track_angle)));
  }

static void send_distance_to_next()
  {
  // id_desired_track_angle               computer track, exclusing drift correction
  publish(create_can_msg_short(&param, id_distance_to_next, 0, (short) distance_to_next));
  }

static void send_distance_to_destination()
  {
  // id_desired_track_angle               computer track, exclusing drift correction
  publish(create_can_msg_short(&param, id_distance_to_destination, 0, (short) distance_to_destination));
  }

// This is a scheduled task that is called off the timer.  It will be called every 90msec

static void navigation_task(void *parg)
  {
  while(true)
    {
    wait(&navigation_semp, 250);
    
    if(!nav_active)
      continue;

    update_flightplan_if_changed();
    compute_navigation();
    }
  }

static message_listener_t listener = { 0, can_callback };
int8_t navigation_init(uint16_t *stack, uint16_t stack_size)
  {
  int8_t id;
  current_waypoint = -1;
  
  // subscribe to the canaerospace FMS notifications
  subscribe(&listener);

  // run the navigation task every 250 msec
  resume(id = create_task("NAVIGATION", stack, stack_size, navigation_task, 0, NORMAL_PRIORITY));
  
  return id;
  
  }

bool navigation_config(semaphore_t *worker)
  {
  signal(worker);
  
  return true;
  }

