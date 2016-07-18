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
#include "altitude_window.h"
#include "pfd_application.h"

struct vsi_markers {
	const char *text;
	size_t length;
	int pos;
};

kotuku::altitude_window_t::altitude_window_t(widget_t &parent, const char *section)
: widget_t(parent, section),
  _background_canvas(*this, window_rect().extents()),
	_altitude(0),
	_vertical_speed(0)
	{
  subscribe(id_baro_corrected_altitude);
  subscribe(id_altitude_rate);

	_background_canvas.fill_rect(rect_t(8, 8, 83, 232), color_gray);

	_background_canvas.background_color(color_gray);
	_background_canvas.pen(&gray_pen);

	// create the background bitmap
	point_t pts[11] = 
		{
		// draw the VSI background
		point_t(88, 20),
		point_t(88, 79),
		point_t(91, 97),
		point_t(91, 143),
		point_t(88, 161),
		point_t(88, 220),
		point_t(113, 220),
		point_t(128, 210),
		point_t(128, 30),
		point_t(113, 20),		
		point_t(88, 20)
		};

	_background_canvas.polygon(pts, 11);
	
	// draw the 0-3000 tick marks(non linear scale)
	point_t tm[30] = {
		point_t(88, 32 ), point_t(103, 32),
		point_t(88, 52 ), point_t(103, 52),
		point_t(88, 72 ), point_t(103, 72),
		point_t(95, 82 ), point_t(103, 82),
		point_t(95, 91 ), point_t(103, 91),
		point_t(95, 101), point_t(103, 101),
		point_t(95, 110), point_t(103, 110),
		point_t(95, 120), point_t(103, 120),
		point_t(95, 130), point_t(103, 130),
		point_t(95, 139), point_t(103, 139),
		point_t(95, 149), point_t(103, 149),
		point_t(95, 158), point_t(103, 158),
		point_t(88, 168), point_t(103, 168),
		point_t(88, 188), point_t(103, 188),
		point_t(88, 208), point_t(103, 208)
		};

	_background_canvas.pen(&white_pen);

	int i;
	for(i = 0; i < 30; i+= 2)
		_background_canvas.polyline(tm + i, 2);

	color_t old_bg_color = _background_canvas.background_color(color_gray);
	color_t old_text_color = _background_canvas.text_color(color_white);
	const font_t *old_font = _background_canvas.font(&arial_12_font);

	static const vsi_markers marks[] = {
		{ "3", 1, 32 },
		{ "2", 1, 52 },
		{ "1", 1, 72 },
		{ "1", 1, 168 },
		{ "2", 1, 188 },
		{ "3", 1, 208 }
		};

	for(i = 0; i < 6; i++)
		{
		extent_t size = _background_canvas.text_extent(marks[i].text, marks[i].length);
		_background_canvas.draw_text(marks[i].text, marks[i].length,
																 point_t(116 - size.dx, marks[i].pos -(size.dy >> 1)));
		}
	}

bool kotuku::altitude_window_t::ev_msg(const msg_t &data)
	{
	bool changed = false;
	switch(data.message_id())
		{
		case id_baro_corrected_altitude :
			changed = assign_msg(short(meters_to_feet(data.value<double>())), _altitude);
			break;
		case id_altitude_rate :
			changed = assign_msg(short(meters_to_feet(data.value<double>())), _vertical_speed);
			break;
		default:
			return false;
		}

	if(changed)
	  invalidate();

	return true;
	}

void kotuku::altitude_window_t::update_window()
	{
	rect_t window_size(0, 0, window_rect().width(), window_rect().height());
  clipping_rectangle(window_size);

  bit_blt(window_size, _background_canvas, point_t(0, 0), rop_srccopy);

  static const size_t buf_size = 64;
  char str[buf_size];
  rect_t paint_area(0, 0, 128, 240);

  clipping_rectangle(paint_area);

	// create the background bitmap
	pen(&white_pen);
	background_color(color_gray);
	text_color(color_white);
	font(&arial_12_font);

  // clip area setting
  clipping_rectangle(rect_t(8, 8, 89, 232));

	// the vertical tape displays 2750 feet around the current position
	// There are 224 display pixels, each 20 pixels is 250 ft
	gdi_dim_t top_altitude = _altitude + 1365;

	// round the height to 10 pixels
	top_altitude =(top_altitude -((top_altitude / 10) * 10)) > 5 ?((top_altitude / 10) + 1) * 10
																								:((top_altitude / 10) * 10);

	// assign the first line altitude
	gdi_dim_t line_altitude =(top_altitude / 250) * 250;
	// work out how many lines to the next lowest marker
	gdi_dim_t marker_line;
	for(marker_line =((top_altitude - line_altitude) / 10)+ 10;
			marker_line < 232; marker_line += 20)
		{
		// draw a line from 10 pixels to 20 pixels then the text
		point_t pts[2] =
			{
			point_t(10, marker_line),
			point_t(20, marker_line)
			};

		polyline(pts, 2);

		if(line_altitude ==((line_altitude / 500) * 500))
			{
			sprintf(str, "%d",(int)line_altitude);

			size_t len = strlen(str);
			extent_t size = text_extent(str, len);

			draw_text(str, len, point_t(23, marker_line -(size.dy >> 1)));
			}

		line_altitude -= 250;

		if(line_altitude < 0)
			break;
		}

  clipping_rectangle(rect_t(point_t(0, 0), window_rect().extents()));

	static const point_t roller[8] =
		{
		point_t(23,  120),
		point_t(35,  132),
		point_t(35,  140),
		point_t(89, 140),
		point_t(89, 100),
		point_t(35,  100),
		point_t(35,  108),
		point_t(23,  120)
		};

	background_color(color_black);
	polygon(roller, 8);

	static const rect_t text_rect(36, 101, 88, 139);
	display_roller(*this, text_rect, _altitude, 2);

	/////////////////////////////////////////////////////////////////////////////
	//
	//	Now display the vertical speed.
	//
	gdi_dim_t vs;
	// draw the marker.  There is a non-linear scale
	if(_vertical_speed < 1000 && _vertical_speed > -1000)
		// +/- 48 pixels
		vs = 120 - gdi_dim_t(double(_vertical_speed) *(48.0 / 1000.0));
	else
		{
    vs = std::min((short)3000, std::max((short)-3000, _vertical_speed));

    // make absolute
    vs = abs(vs);

    vs -= 1000;       // makes pixels from 1000 ft mark
    vs /= 50;         // 40 pixels = 2000 ft
    vs += 48;         // add the 1000 ft marks

    if(_vertical_speed < 0)
      vs *= -1;

    vs = 120 - vs;        // add the base marker
		}

  gdi_dim_t vs_base = 120;
  if(vs == 120)
    {
    vs_base--;
    vs++;
    }

  if(vs_base < vs)
    std::swap(vs_base, vs);

	rect_t vs_rect(point_t(117, vs), point_t(128, vs_base));
  fill_rect(vs_rect, color_white);

  // draw the text at the top of the VSI
  rect_t vsi_rect(point_t(88, 1), point_t(127, 18));
  font(&arial_9_font);
  pen(&white_pen);
  rectangle(vsi_rect);

  vsi_rect += extent_t(-1, -1);
  clipping_rectangle(vsi_rect);

  // round the vs to 10 feet
  vs = _vertical_speed;
  if(abs((vs / 5)%5) == 1)
    vs = ((vs /10) * 10)+(vs < 0 ? -10 : 10);
  else
    vs = (vs/10) * 10;

	sprintf(str, "%d", vs);

	size_t len = strlen(str);
	extent_t size = text_extent(str, len);

	draw_text(str, len, point_t(107 - (size.dx >> 1), vsi_rect.top));
	}
