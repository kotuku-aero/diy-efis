
function lla_t() {
    var _lat = 0.0;
    var _lng = 0.0;
   Object.defineProperties(this, {
       lat : { enumerable: true, configurable: true, get: function() { return this._lat; } , set: function() { return this._lat; } },
       lng : { enumerable: true, configurable: true, get: function() { return this._lng; } , set: function() { return this._lng; } },
  });
}

function line_t() {
    var P0 = new lla_t();
    var P1 = new llat_();
}


  
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
function dot(u, v) {
  return u.lng * v.lng + u.lat * v.lat;
  }
  
function to_vector(u, v, r) {
  r.lng = v.lng - u.lng;
  r.lat = v.lat - u.lat;
  }
  
function add_vector(p, v, r) {
  r.lng = p.lng + v.lng;
  r.lat = p.lat + v.lat;
  }
  
function multiply_vector(s, v, r) {
  r.lng = v.lng * s;
  r.lat = v.lat * s;
  }
  
// distance = norm of difference
//#define dot(u,v)   ((u).x * (v).x + (u).y * (v).y + (u).z * (v).z)
//#define norm(v)     sqrt(dot(v,v))     // norm = length of  vector
//#define d(u,v)      norm(u-v)          // distance = norm of difference

// dist_Point_to_Line(): get the distance of a point to a line
//     Input:  a Point P and a Line L (in any dimension)
//     Return: the shortest distance from P to L
function dist_lla_to_line(p, l)
  {
  var v = new lla_t();
  var w = new lla_t();
  c1;
  c2;
  b;
  var pb = new lla_t();
  var v2 = new lla_t();
  
  to_vector(l.P1, l.P0, v);
  to_vector(p, l.P0, w);
  
  c1 = dot(w, v);
  c2 = dot(v, v);
  b = c1 / c2;
  
  multiply_vector(b, v, v2);
  add_vector(l.P0, v2, pb);
  
  return distance(p, pb);
  }
  
//
// routine to calculate the angle between two points, relative to true north (0,0)
function angle_of_vector(l) {
  return 0;  // TODO:
  }

var distance_to_next = 0.0;
var distance_to_destination = 0.0;
var track;
var deviation;
var track_angle_error;
var estimated_time_to_go; // time in seconds
var estimated_time_of_arrival;
var estimated_time_to_next;
var desired_track_angle;
var current_waypoint; // current waypoint in the list

var max_route_waypoints = 16;

function radians_to_degrees(radians) {
  return radians / (Math.PI / 180);
}

function send_track(track) {
  // id_track                             computed track to fly to arrive at destination including drift
  publish_uint16(id_track, radians_to_degrees(track));
  }

function send_deviation(deviation)
  {
  // id_deviation                         how far off the track the aircraft is.
  // todo: make sure only +/- 16km
  publish_int16(id_deviation, deviation);
  }

function send_track_angle_error(track_angle_error)
  {
  // id_track_angle_error                 angle between the start of the track and current position and desired position
  publish_int16(id_track_angle_error, radians_to_degrees(track_angle_error));
  }

function send_estimated_time_to_go(estimate)
  {
  // id_estimated_time_to_go              time at current groundspeed to get to destination
  publish_uint32(id_estimated_enroute_time, estimate);
  }

function send_estimated_time_of_arrival(estimate)
  {
  // id_estimated_time_of_arrival         current time plus estimated time to go

  // est time is in seconds so build into an char3 as hrs/min/sec
  uint8_t data[3];
  data[2] = (uint8_t) (estimated_time_of_arrival % 60);
  data[1] = (uint8_t) ((estimated_time_of_arrival % 3600) / 60);
  data[0] = (uint8_t) (estimated_time_of_arrival / 3600);

  can_send(create_can_msg_uint8_3(&param, id_estimated_time_of_arrival, 0, data));
  }

function send_estimated_time_to_next(estimate)
  {
  // id_estimated_time_to_next            time to get to final destination, corrected for wind and airsped
  publish_uint32(id_estimated_time_to_next, estimate);
  }

function send_desired_track_angle()
  {
  // id_desired_track_angle               computer track, exclusing drift correction
  can_send(create_can_msg_int16(&param, id_desired_track_angle, 0, (int16_t) radians_to_degrees(desired_track_angle)));
  }

function send_distance_to_next()
  {
  // id_desired_track_angle               computer track, exclusing drift correction
  can_send(create_can_msg_int16(&param, id_distance_to_next, 0, (int16_t) distance_to_next));
  }

function send_distance_to_destination()
  {
  // id_desired_track_angle               computer track, exclusing drift correction
  can_send(create_can_msg_int16(&param, id_distance_to_destination, 0, (int16_t) distance_to_destination));
  }

// this is our current GPS track definition.

function waypoint_t() {
    var icao;                           // ico type string
    var type;                           // type of waypoint
    var position = new lla_t();         // position of the waypoint
    var min_altitude = 0;               // minimum altitude over waypoint
    var max_altitude = 0;               // maximum altitude over waypoint
  };

var route = new Array();                // the rout, composed of waypoints
var nav_active;
var next_waypoint = -1;
var next_waypoint_pt = null;

// we publish the active route whenever it changes, or every 10 seconds
var publish_timeout = -1;
var sector;     // line_t
var id_gps_aircraft_latitude = 1036;
var id_gps_aircraft_longitude = 1037;


function calculate_start_waypoint()  {
  return 0; // always start at the begining (for now!)
  }
  
  // calculate the next waypoint based on our current
  // position.  Always looks forwards
function calculate_next(last_waypoint, current_position) {
 
  return new lla_t();
}

function waypoint_defined(next_waypoint) {
  if(next_waypoint >= route.length)
    return false;
  
  return route[next_waypoint] != null;
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
 var start_position = new lla_t();

function compute_navigation()
  {
  if (!nav_active)
    return; // do nothing

  // this is called when a route is activated.
  // we need to find the nearest waypoint to the aircraft and set that as the
  // next waypoint.
  var current_position = new lla_t();
  
  current_position.lat = get_published_float(id_gps_aircraft_latitude);
  current_position.lng = get_published_float(id_gps_aircraft_longitude);
  
  next_waypoint_pt = calculate_next(next_waypoint, position);

  var distance_to_next = distance(current_position, next_waypoint_pt);

  // if the distance to the waypoint is < 1nm then we use the next waypoint.
 if (distance_to_next <= 1852) {
  // move next (if there is one)
  if (next_waypoint < route.length &&
      waypoint_defined(next_waypoint + 1))
      {
      next_waypoint++; // advance next
      // check to see if the next position is the one we want.
      next_waypoint_pt = calculate_next(next_waypoint, position);

      distance_to_next = distance(current_position, next_waypoint_pt);
      }
    }

  // calculate the total distance to go
  distance_to_destination = distance_to_next;

  // calculate the route points
  sector.P0 = next_waypoint == 0 ? start_position : next_waypoint_pt;

  var waypoint_it;
  for (waypoint_it = next_waypoint + 1;
       waypoint_it < route.length && waypoint_defined(waypoint_it);
       waypoint_it++)
    {
    // set the end of the sector to the next point
    sector.P1 = calculate_next(waypoint_it, position);
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
  sector.P1 = () compute_address(waypoint_it, offsetof(waypoint_t, position));

  // calculate how far from the track we are.
  deviation = dist_lla_to_line(&current_position, &sector);

  // the desired track angle is the angle from the aircraft to the destination
  desired_track_angle = angle_of_vector(&sector);
  // the track is the angle to fly allowing for wind direction and speed
  sector.P1 = next_waypoint_pt;
  sector.P0 = &current_position;

  desired_track = angle_of_vector(&sector);

  // add the wind to get the true heading corrected for wind
  }

function deactivate_route()
  {
  nav_active = false;
  }

function clear_route()
  {
  int waypoint;
  int offset;
  deactivate_route();

  for (waypoint = 0; waypoint < max_route_waypoints; waypoint++)
    for (offset = 0; offset < sizeof (waypoint_t); offset += sizeof (uint16_t))
      write_waypoint_word(waypoint, offset, 0);
  }

function activate_route()
  {
  nav_active = waypoint_defined(0);

  next_waypoint = calculate_start_waypoint();
  start_position.lat = ahrs_state.gps_position.lat;
  start_position.lng = ahrs_state.gps_position.lng;
  }

function invert_route()
  {
  int num_waypoints;
  int first_waypoint;
  int last_waypoint;
  bool was_active = nav_active;

  nav_active = false;

  // find how many waypoint segments there are
  for (num_waypoints = 0; num_waypoints < max_route_waypoints; num_waypoints++)
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

void nav_listener(const can_msg_t *msg)
  {
  switch (msg.id)
    {
    case id_nav_command:
      if (msg.canas.service_code != 0)
        break; // we only work on the current route
      switch (get_param_int16(msg, 0))
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

      write_waypoint_char(msg.canas.service_code, 0, msg.canas.data[0]);
      write_waypoint_char(msg.canas.service_code, 1, msg.canas.data[1]);
      write_waypoint_char(msg.canas.service_code, 2, msg.canas.data[2]);
      write_waypoint_char(msg.canas.service_code, 3, msg.canas.data[3]);
      break;
    case id_waypoint_identifier_4_7:
      if (nav_active)
        break; // we only work on the current route, and if the route is not active

      write_waypoint_char(msg.canas.service_code, 4, msg.canas.data[0]);
      write_waypoint_char(msg.canas.service_code, 5, msg.canas.data[1]);
      write_waypoint_char(msg.canas.service_code, 6, msg.canas.data[2]);
      write_waypoint_char(msg.canas.service_code, 7, msg.canas.data[3]);
      break;
    case id_waypoint_identifier_8_11:
      if (nav_active)
        break; // we only work on the current route, and if the route is not active

      write_waypoint_char(msg.canas.service_code, 8, msg.canas.data[0]);
      write_waypoint_char(msg.canas.service_code, 9, msg.canas.data[1]);
      write_waypoint_char(msg.canas.service_code, 10, msg.canas.data[2]);
      write_waypoint_char(msg.canas.service_code, 11, msg.canas.data[3]);
      break;
    case id_waypoint_identifier_12_15:
      if (nav_active)
        break; // we only work on the current route, and if the route is not active

      write_waypoint_char(msg.canas.service_code, 12, msg.canas.data[0]);
      write_waypoint_char(msg.canas.service_code, 13, msg.canas.data[1]);
      write_waypoint_char(msg.canas.service_code, 14, msg.canas.data[2]);
      write_waypoint_char(msg.canas.service_code, 15, msg.canas.data[3]);
      break;
    case id_waypoint_type_identifier:
      if (nav_active)
        break; // we only work on the current route, and if the route is not active

      write_waypoint_type(msg.canas.service_code, get_param_int16(msg, 0));
      break;
    case id_waypoint_latitude:
      if (nav_active)
        break; // we only work on the current route, and if the route is not active

      write_waypoint_float(msg.canas.service_code, offsetof(waypoint_t, position.lat), get_param_float(msg));
      break;
    case id_waypoint_longitude:
      if (nav_active)
        break; // we only work on the current route, and if the route is not active

      write_waypoint_float(msg.canas.service_code, offsetof(waypoint_t, position.lng), get_param_float(msg));
      break;
    case id_waypoint_minimum_altitude:
      if (nav_active)
        break; // we only work on the current route, and if the route is not active

      write_waypoint_short(msg.canas.service_code, offsetof(waypoint_t, min_altitude), get_param_int16(msg, 0));
      break;
    case id_waypoint_maximum_altitude:
      if (nav_active)
        break; // we only work on the current route, and if the route is not active

      write_waypoint_short(msg.canas.service_code, offsetof(waypoint_t, max_altitude), get_param_int16(msg, 0));
      break;
    }
  }

can_msg_t *create_can_msg_achar4(can_msg_t *msg, uint16_t message_id, uint8_t service_code, const char *value, uint8_t length)
  {
  uint8_t i;
  msg.flags = length + 4;
  msg.id = message_id;
  msg.canas.data_type = CANAS_DATATYPE_UCHAR4;
  msg.canas.service_code = service_code;
  for (i = 0; i < length; i++)
    msg.canas.data[i] = value[i];

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

  can_send(create_can_msg_uint8_4(&param, id, waypoint_number, (const uint8_t *)txt));

  return length == 4;
  }

// this routine will check to see if our next waypoint has changed, if so it
// publishes the active route from our next waypoint to the end.

function update_flightplan_if_changed()
  {
  if (nav_active &&
      next_waypoint > -1 &&
      ((next_waypoint != last_waypoint_sent) || publish_timeout < 1))
    {
    // clear the active plan waypoint database.  Note that this is ignored
    // if we get a loopback as we are in active navigation mode.
    publish_int16(get_param_int16(&param, 0), id_nav_command);
    // update the nav display with all of the current waypoints
    // we only send this when our next waypoint changes or if the timeout occurs
    int active_plan_waypoint = 0;
    int waypoint_to_send;
    for (waypoint_to_send = next_waypoint;
         waypoint_to_send < max_route_waypoints && waypoint_defined(waypoint_to_send);
         waypoint_to_send++)
      {
      if (send_waypoint_name(waypoint_to_send, 0, id_waypoint_identifier_0_3, active_plan_waypoint))
        if (send_waypoint_name(waypoint_to_send, 4, id_waypoint_identifier_4_7, active_plan_waypoint))
          if (send_waypoint_name(waypoint_to_send, 8, id_waypoint_identifier_8_11, active_plan_waypoint))
            send_waypoint_name(waypoint_to_send, 16, id_waypoint_identifier_12_15, active_plan_waypoint);

      can_send(create_can_msg_int16(&param, id_waypoint_type_identifier, active_plan_waypoint, read_waypoint_type(waypoint_to_send)));
      can_send(create_can_msg_float(&param, id_waypoint_latitude, active_plan_waypoint, read_waypoint_float(waypoint_to_send, offsetof(waypoint_t, position.lat))));
      can_send(create_can_msg_float(&param, id_waypoint_longitude, active_plan_waypoint, read_waypoint_float(waypoint_to_send, offsetof(waypoint_t, position.lng))));
      can_send(create_can_msg_int16(&param, id_waypoint_minimum_altitude, active_plan_waypoint, read_waypoint_short(waypoint_to_send, offsetof(waypoint_t, min_altitude))));
      can_send(create_can_msg_int16(&param, id_waypoint_maximum_altitude, active_plan_waypoint, read_waypoint_short(waypoint_to_send, offsetof(waypoint_t, max_altitude))));
      }

    publish_timeout = 10;
    }
  else if (nav_active)
    publish_timeout--;
  }

// This is a scheduled task that is called off the timer.  It will be called every 90msec

function navigation_task(void *parg)
  {
  while(true)
    {
    semaphore_wait(navigation_semp, 250);
    
    if(!nav_active)
      continue;

    update_flightplan_if_changed();
    compute_navigation();
    }
  }
