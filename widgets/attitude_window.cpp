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
#include "attitude_window.h"
#include "pfd_application.h"

static const gdi_dim_t median_x = 120;
static const gdi_dim_t median_y = 120;
static const gdi_dim_t window_x = 240;
static const gdi_dim_t window_y = 240;
static const gdi_dim_t border = 15;
static const gdi_dim_t pixels_per_degree = 49;
static const kotuku::point_t median(median_x, median_y);

kotuku::attitude_window_t::attitude_window_t(widget_t &parent, const char *section)
: widget_t("ATT", parent, get_window_rect(section)),
	_pitch(0),
	_pitch_degrees(0),
	_roll(0),
	_roll_degrees(0),
  _abs_roll(0),
  _yaw_degrees(0),
  _localizer(0),
  _glideslope(0),
  _glideslope_aquired(false),
	_localizer_aquired(false),
	_image_canvas(*this, window_rect().extents()),
	_temp_canvas(*this, window_rect().extents()),
	_mask_canvas(*this, window_rect().extents())
	{
	// we create a mask bitmap at this point.
	_mask_canvas.fill_rect(rect_t(0, 0, window_x, window_y), color_black);
	_mask_canvas.background_color(color_white);
	_mask_canvas.pen(&white_pen);
	_mask_canvas.ellipse(rect_t(border, border, window_x - border, window_y - border));

  int value;
  if(failed(the_app()->get_config_value(section, "critical-aoa", value)))
    _critical_aoa = 0;
    else
      _critical_aoa = (short) value;

  if(failed(the_app()->get_config_value(section, "approach-aoa", value)))
    _approach_aoa = 0;
  else
    _approach_aoa = (short) value;

  if(failed(the_app()->get_config_value(section, "climb-aoa", value)))
      _climb_aoa = 0;
    else
      _climb_aoa = (short) value;

  if(failed(the_app()->get_config_value(section, "cruise-aoa", value)))
      _cruise_aoa = 0;
    else
      _cruise_aoa = (short) value;

  if(failed(the_app()->get_config_value(section, "yaw-max", value)))
      _yaw_max = 45;
    else
      _yaw_max = (short) value;

  if(failed(the_app()->get_config_value(section, "show-aoa", value)))
      _show_aoa = false;
    else
      _show_aoa = value != 0;

  if(failed(the_app()->get_config_value(section, "show-glideslope", value)))
      _show_glideslope = false;
    else
      _show_glideslope = value != 0;

  _aoa_degrees = _cruise_aoa;

  // aoa is 40 pixels
  _aoa_pixels_per_degree = 40.0 / double(_critical_aoa - _cruise_aoa);
  _aoa_degrees_per_mark = double(_critical_aoa - _cruise_aoa) / 8;

  subscribe(id_pitch_angle);
  subscribe(id_yaw_angle);
  subscribe(id_roll_angle);
	}

bool kotuku::attitude_window_t::ev_msg(const msg_t &data)
	{
	assert_valid(this);
	bool changed = false;

	switch(data.message_id())
		{
    case id_yaw_angle :
      changed |= assign_msg((short)radians_to_degrees(data.value<double>()), _yaw_degrees);
      while(_yaw_degrees > 179)
        _yaw_degrees -= 360;

      while(_yaw_degrees < -180)
        _yaw_degrees += 360;

      break;
		case id_roll_angle :
      {
			changed |= assign_msg((short)radians_to_degrees(data.value<double>()), _roll_degrees);
      while(_roll_degrees > 179)
        _roll_degrees -= 360;

      while(_roll_degrees < -180)
        _roll_degrees += 360;

			_roll = degrees_to_radians(_roll_degrees);

      // roll is now +- 180
      short abs_roll = _roll_degrees;

      if(abs_roll > 90 || abs_roll < -90)
        abs_roll -= 180;

      while(abs_roll < -180)
        abs_roll += 360;

      abs_roll = std::min((short)90, std::max((short)-90, abs_roll));
      _abs_roll = degrees_to_radians(abs_roll);
      }
		  break;
		case id_pitch_angle :
		  {
		  short angle = (short)radians_to_degrees(data.value<double>());

      while(angle < -180)
        angle += 180;

      while(angle > 180)
        angle -= 180;

      // angle is now within range
      if(angle > 90)
        angle = 180 - angle;
      else if(angle < -90)
        angle = -180 - angle;

			changed |= assign_msg(angle, _pitch_degrees);

			_pitch = degrees_to_radians(_pitch_degrees);
		  }
  		break;
		//case id_glideslope_deviation :
		//	// glideslope is 0.4 / 4096
		//	// we represent this as +/- 120 pixels
		//	changed |= assign_msg(long(data.msg_data_16() *(120 / 4096)), _glideslope);
		//break;
  //  case id_glideslope_aquired :
  //    changed |= assign_msg(data.msg_data_16() != 0, _glideslope_aquired);
  //  break;
		//case id_localizer_deviation :
		//	changed |= assign_msg(long(data.msg_data_16() *(120 / 4096)), _localizer);
		//break;
  //  case id_localizer_aquired :
  //    changed |= assign_msg(data.msg_data_16() != 0, _localizer_aquired);
  //  break;
  //  case id_angle_of_attack :
  //    changed |= assign_msg(data.msg_data_16(), _aoa_degrees);
  //    break;
		default:
			return false;
		}

	if(changed)
	  invalidate();

	return true;
	}

void kotuku::attitude_window_t::update_window()
	{
	// first step is to draw the background.  Then we can rotate it correctly
	// the background is 240x240 pixels with 20 pixels being the median line

  // the display is +/- 21.2132 degrees so we round to +/- 20 degrees
  // or 400 pixels
  short pitch = _pitch_degrees;

  // pitch is now +/- 90
  // limit to 25 degrees as we don't need any more than that
  pitch = std::min((short)25, std::max((short)-25, pitch));

  // make this == pixels that is the azimuth line
  pitch *= 10;

  // draw the upper area
	point_t pts[5];
	pts[0].x = -500; pts[0].y = median_y - 500 + pitch;
	pts[1].x = 500;  pts[1].y = median_y - 500 + pitch;
	pts[2].x = 500;  pts[2].y = median_y + pitch;
	pts[3].x = -500; pts[3].y = median_y + pitch;
	pts[4].x = -500; pts[4].y = median_y - 500 + pitch;

	// rotate the upper rect
	int pt;
	for(pt = 0; pt < 5; pt++)
		rotate_point(median, pts[pt], _roll);
	pts[4] = pts[0];

  _image_canvas.background_color(color_lightblue);
  _image_canvas.pen(&lightblue_pen);
	_image_canvas.polygon(pts, 5);

	pts[0].x = -500; pts[0].y = median_y  + pitch;
	pts[1].x = 500;  pts[1].y = median_y  + pitch;
	pts[2].x = 500;  pts[2].y = median_y  + pitch + 500;
	pts[3].x = -500; pts[3].y = median_y  + pitch + 500;
	pts[4].x = -500; pts[4].y = median_y  + pitch;

	// rotate the brown rect
	for(pt = 0; pt < 5; pt++)
		rotate_point(median, pts[pt], _roll);
  pts[4] = pts[0];

  _image_canvas.background_color(color_brown);
  _image_canvas.pen(&brown_pen);
	_image_canvas.polygon(pts, 5);

	/////////////////////////////////////////////////////////////////////////////
	//	Draw the pitch indicator

	_image_canvas.background_color(color_black);
	_image_canvas.pen(&white_pen);

	/////////////////////////////////////////////////////////////////////////////
	// we now draw the image of the bank angle marks
	point_t slip_indicator[18] = {
		point_t(224,  60), point_t(214,  66),
		point_t(180,  17), point_t(174,  27),
		point_t(160,  11), point_t(157,  19),
		point_t(140,   6), point_t(138,  14),
		point_t(100,   6), point_t(102,  14),
		point_t( 81,  11), point_t( 83,  19),
		point_t( 61,  17), point_t( 66,  27),
		point_t( 17,  60), point_t( 27,  66),
		point_t(120,   0), point_t(120,  12),
		};


	int i;
	for(i = 0; i < 18; i += 2)
		{
		rotate_point(median, slip_indicator[i], _abs_roll);
		rotate_point(median, slip_indicator[i + 1], _abs_roll);

		if(i == 16)
			_image_canvas.pen(&green_pen_3);

		_image_canvas.polyline(slip_indicator + i, 2);
		}

	_image_canvas.pen(&white_pen);

	/////////////////////////////////////////////////////////////////////////////
	//	Draw the rotated roll/pitch indicator

  // The window height is 240 pixels, and each 25 deg of pitch
  // is 20 pixels.  So the display is +/- 150 degrees (120/20)*25
  //
	gdi_dim_t pitch_angle =(gdi_dim_t(_pitch_degrees * 10)) + 150;
	gdi_dim_t line = 0;

	// we draw on the background DC so we can rotate blt it later
	_temp_canvas.pen(&white_pen);
	_temp_canvas.background_color(color_black);
	_temp_canvas.text_color(color_white);

	// fill with black
	_temp_canvas.fill_rect(rect_t(0, 0, window_x, window_y), color_black);

	// now we draw the pitch line(s)
	if(pitch_angle % 25)
		{
		gdi_dim_t new_pitch =((pitch_angle / 25) + 1) * 25;
		line = pitch_angle - new_pitch;
		pitch_angle = new_pitch;
		}

	while(line < window_y)
		{
    if(pitch_angle == 3600 || pitch_angle == 0)
			{
			pitch_angle -= 25;
			line += 20;
			}
		else if((pitch_angle % 100) == 0)
			{
			point_t pts[2] = 
				{
				point_t( 90, line),
				point_t(160, line)
				};

			rotate_point(median, pts[0], _roll);
			rotate_point(median, pts[1], _roll);

			_temp_canvas.polyline(pts, 2);

			// we have a bitmap which is the text to draw.  We then select the bitmap
			// from the text angle and the rotation angle.
			// the text is 19x19 pixels

			// calc the text angle as 10, 20..90
			char text_angle[2] = { char(pitch_angle / 100), 0 };
			text_angle[0] = text_angle[0] < 0 ? -text_angle[0] : text_angle[0];

			if(text_angle > 0)
				{

				// calc the left/right center point
				point_t pt_left( 73, line);
				point_t pt_right(168, line);

				rotate_point(median, pt_left, _roll);
				rotate_point(median, pt_right, _roll);

				// we now calc the left and right points to write the text to
				pt_left.x -= 9; pt_left.y -= 9;
				pt_right.x -= 9; pt_right.y -= 9;

				_temp_canvas.font(attitude_window_fonts[_roll_degrees < 0 ? _roll_degrees + 360 : _roll_degrees]);
				_temp_canvas.draw_text(text_angle, 1, pt_left);
				_temp_canvas.draw_text(text_angle, 1, pt_right);
				}
			pitch_angle -= 25;
			line += 20;
			}
		else if((pitch_angle % 50) == 0)
			{
			point_t pts[2] =
				{
				point_t( 94, line),
				point_t(146, line)
				};

			rotate_point(median, pts[0], _roll);
			rotate_point(median, pts[1], _roll);

			_temp_canvas.polyline(pts, 2);

			pitch_angle -= 25;
			line += 20;
			}
		else
			{
			point_t pts[2] =
				{
				point_t(108, line),
				point_t(132, line)
				};

			rotate_point(median, pts[0], _roll);
			rotate_point(median, pts[1], _roll);

			_temp_canvas.polyline(pts, 2);

			pitch_angle -= 25;
			line += 20;
			}
		}

	// mask the image before the transfer
	_temp_canvas.bit_blt(rect_t(0, 0, window_x, window_y),
														 _mask_canvas, point_t(0, 0), rop_srcand);
	_image_canvas.bit_blt(rect_t(border, border, window_x - border, window_y - border),
												_temp_canvas, point_t(border, border), rop_srcpaint);

  /////////////////////////////////////////////////////////////////////////////
  // Draw the angle-of-attack indicator
  //
  // this is 40 pixels up/down
  
  if(_show_aoa)
    {
    // calc the effective AOA
    short aoa = std::min(_critical_aoa, std::max(_cruise_aoa, _aoa_degrees));
    aoa -= _cruise_aoa;
    short pixels = short(double(aoa) * _aoa_pixels_per_degree);

    double aoa_marker = _critical_aoa;
    for(short offset = 60; offset > 0; offset -= 6)
      {
      if(aoa_marker > approach_aoa())
        {
        // draw red chevron.
        point_t chevron[3] = 
          {
          point_t(median_x - 15, pixels + median_y - offset),
          point_t(median_x, pixels + median_y - offset + 4),
          point_t(median_x + 15, pixels + median_y - offset)
          };

        _image_canvas.pen(&red_pen_3);
        _image_canvas.polyline(chevron, 3);
        }
      else if(aoa_marker > climb_aoa())
        {
        point_t marker[2] =
          {
          point_t(median_x - 15, pixels + median_y - offset),
          point_t(median_x + 15, pixels + median_y - offset)
          };

        _image_canvas.pen(&yellow_pen_3);
        _image_canvas.polyline(marker, 2);
        }
      else
        {
        point_t marker[2] =
          {
          point_t(median_x - 15, pixels + median_y - offset),
          point_t(median_x + 15, pixels + median_y - offset)
          };

        _image_canvas.pen(&green_pen_3);
        _image_canvas.polyline(marker, 2);
        }
    
      aoa_marker -= _aoa_degrees_per_mark;
      }
    }

	/////////////////////////////////////////////////////////////////////////////
	// draw the aircraft image
	point_t aircraft_points[2];
  _image_canvas.pen(&white_pen_3);

	aircraft_points[0].x = median_x - 7; aircraft_points[0].y = median_y;
	aircraft_points[1].x = median_x - 22; aircraft_points[1].y = median_y;

	_image_canvas.polygon(aircraft_points, 2);

	aircraft_points[0].x = median_x + 7; aircraft_points[0].y = median_y;
	aircraft_points[1].x = median_x + 22; aircraft_points[1].y = median_y;

	_image_canvas.polygon(aircraft_points, 2);

	aircraft_points[0].x = median_x; aircraft_points[0].y = median_y - 7;
	aircraft_points[1].x = median_x; aircraft_points[1].y = median_y - 15;

	_image_canvas.polygon(aircraft_points, 2);

	_image_canvas.background_color(color_hollow);
  _image_canvas.pen(&white_pen);
	_image_canvas.ellipse(rect_t(median_x - 7, median_y - 7, median_x + 7, median_y + 7));

	/////////////////////////////////////////////////////////////////////////////
	// draw the glideslope and localizer indicators
	if(_glideslope_aquired && _show_glideslope)
		{
		// draw the marker, 0.7 degrees = 59 pixels

		double deviation = std::max(-1.2, std::min(1.2, _glideslope / 100.0));

		gdi_dim_t pixels = gdi_dim_t(deviation /(1.0 / pixels_per_degree));

		pixels += 120;

		point_t pts[2] = {
			point_t(228, median_y),
			point_t(window_x, median_y)
			};

		_image_canvas.pen(&white_pen);
		_image_canvas.polyline(pts, 2);

		static rect_t glideslope[4] =
			{
			rect_t(230,  57, 238,  65),
			rect_t(230,  86, 238,  94),
			rect_t(230, 146, 238, 154),
			rect_t(230, 175, 238, 183)
			};

		// rest are hollow
		_image_canvas.background_color(color_hollow);
		_image_canvas.ellipse(glideslope[0]);
		_image_canvas.ellipse(glideslope[1]);
		_image_canvas.ellipse(glideslope[2]);
		_image_canvas.ellipse(glideslope[3]);

		// black filled ellipse
		_image_canvas.background_color(color_black);
		_image_canvas.ellipse(rect_t(230, pixels -4, 238, pixels + 4));

		}

	if(_localizer_aquired && _show_glideslope)
		{
		// draw the marker, 1.0 degrees = 74 pixels

		double deviation = std::max(-1.2, std::min(1.2, _localizer / 100.0));

		gdi_dim_t pixels = gdi_dim_t(deviation /(1.0 / pixels_per_degree));

		pixels += median_x;

		point_t pts[2] = {
			point_t(median_x, median_y-15),
			point_t(median_x, median_y)
			};

		_image_canvas.pen(&white_pen_3);
		_image_canvas.polyline(pts, 2);

		_image_canvas.pen(&white_pen);

		static rect_t localizer[4] = {
			rect_t( 57, 230,  65, 238),
			rect_t( 86, 230,  94, 238),
			rect_t(146, 230, 154, 238),
			rect_t(175, 230, 183, 238)
			};

		// rest are hollow
		_image_canvas.background_color(color_hollow);
		_image_canvas.ellipse(localizer[0]);
		_image_canvas.ellipse(localizer[1]);
		_image_canvas.ellipse(localizer[2]);
		_image_canvas.ellipse(localizer[3]);

		// black filled ellipse

		_image_canvas.background_color(color_black);
		_image_canvas.ellipse(rect_t(pixels - 4, 230, pixels + 4, 238));

		}

	/////////////////////////////////////////////////////////////////////////////
	// Draw the roll indicator
	// Draw the aircraft pointer at the top of the window.
	point_t roll_points[4] = {
		point_t(120, 12),
		point_t(114, 23),
		point_t(126, 23),
		point_t(120, 12)
		};

  // the roll indicator is shifted left/right by the yaw angle,  1degree = 2pix
  short offset = std::min(_yaw_max,
      std::max(short(-_yaw_max), short(_yaw_degrees << 1)));

  extent_t offset_pt(offset, 0);
  roll_points[0] += offset_pt;
  roll_points[1] += offset_pt;
  roll_points[2] += offset_pt;
  roll_points[3] += offset_pt;

	_image_canvas.polygon(roll_points, 4);

	point_t roll_points_base[5] = {
		point_t(114, 23),
		point_t(126, 23),
		point_t(129, 27),
		point_t(111, 27),
		point_t(114, 23)
		};

  _image_canvas.background_color(color_white);

	_image_canvas.polygon(roll_points_base, 5);

	/////////////////////////////////////////////////////////////////////////////
	// finally we fill the corners of the indicator with black
	// radii.
	_temp_canvas.fill_rect(rect_t(0, 0, 20, 20), color_black);
	_temp_canvas.fill_rect(rect_t(0, 220, 20, 240), color_black);
	_temp_canvas.fill_rect(rect_t(220, 0, 240, 20), color_black);
	_temp_canvas.fill_rect(rect_t(220, 220, 240, 240), color_black);

	_temp_canvas.pen(&white_pen);
	_temp_canvas.background_color(color_white);

	_temp_canvas.ellipse(rect_t(0, 0, 20, 20));
	_temp_canvas.ellipse(rect_t(0, 200, 20, 240));
	_temp_canvas.ellipse(rect_t(220, 0, 240, 20));
	_temp_canvas.ellipse(rect_t(220, 220, 240, 240));

	_temp_canvas.fill_rect(rect_t(0, 10, 20, 230), color_white);
	_temp_canvas.fill_rect(rect_t(10, 0, 230, 240), color_white);
	_temp_canvas.fill_rect(rect_t(230, 10, 240, 230), color_white);

	_image_canvas.bit_blt(rect_t(0, 0, 240, 240), _temp_canvas, 
												point_t(0, 0), rop_srcand);

  rect_t rect(point_t(0, 0), window_rect().extents());
  bit_blt(rect, _image_canvas, rect.top_left(), rop_srccopy);
	}
