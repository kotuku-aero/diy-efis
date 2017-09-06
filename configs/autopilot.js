#include "autopilot.h"
#include "../can-aerospace/can_aerospace.h"
#include "../dspic-lib/publisher.h"
#include "../dspic30f-lib/can_driver.h"

/*
 * This module implements the autopilot functionality and will send commands
 * to the pitch and roll servo's
 *
 * One day it will also manage the trim servo's
 *
 * The Autopilot is commanded into one of several modes based on the AP
 * control panel.
 *
 * The commands handled are:
 *
 * id_autopilot_engage                  Turns on/off the autopilot
 * id_autopilot_set_max_roll            Set the maximum roll rate
 * id_autopilot_set_vs_rate             vertical speed rate
 *
 * To enable the controller to interface to a variety of servos we use the MGL
 * canbus protocol.  Built into the FDU is a converter for GRT servos.
 */

static published_datapoint_t datapoints[2] = {
  { id_roll_servo_set_position, 10 },
  { id_pitch_servo_set_postion, 10 },
  };

static published_datapoints_t published_datapoints = { datapoints, 2 };

void send_roll_command(int roll_amount)
  {
  publish_short(roll_amount, &datapoints[0]);
  }

void send_pitch_command(int pitch_amount)
  {
  publish_short(pitch_amount, &datapoints[1]);
  }

void autopilot(void *parg)
  {
  }

void receive_command(const can_msg_t *msg)
  {
  switch(msg->id)
   {
    case id_autopilot_engage :
      break;
    case id_waypoint_turn_heading :
      break;
    case id_autopilot_set_max_roll :
      break;
    case id_autopilot_set_vs_rate :
      break;
    case id_track :
      break;
    case id_deviation :
      break;
    case id_track_angle_error :
      break;
    case id_distance_to_next :
      break;
    case id_desired_track_angle :
      break;
   }
  }

static message_listener_t command_listener = { 0, receive_command };

void autopilot_init()
  {
  register_datapoints(&published_datapoints);
  
  // subscribe to the incoming messages
  subscribe(&command_listener);
  }
