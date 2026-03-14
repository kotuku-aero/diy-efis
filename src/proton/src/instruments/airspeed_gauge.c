#include "../../include/airspeed_gauge.h"
#include "../mfdlib/mfd.h"

static void draw_speed_band(handle_t canvas, const rect_t* wnd_rect,
                            gauge_widget_t* wnd, float degrees_per_unit, int16_t min_range, color_t color, int32_t inner_radii, int16_t width, float arc_start, float arc_end)
  {
  float start_degrees = (arc_start - min_range) * degrees_per_unit;
  float end_degrees = (arc_end - min_range) * degrees_per_unit;

  start_degrees += wnd->arc_begin;
  end_degrees += wnd->arc_begin;

  int arc_angles[3] = { (int)start_degrees, (int)end_degrees, -1 };

  while (arc_angles[0] > 360)
    {
    arc_angles[0] -= 360;
    arc_angles[1] -= 360;
    }

  while (arc_angles[1] > 360)
    {
    arc_angles[2] = arc_angles[1] - 360;
    arc_angles[1] = 360;
    }

  pie(canvas, wnd_rect, color, color, &wnd->center,
    arc_angles[0], arc_angles[1],
    inner_radii + width, inner_radii);

  if (arc_angles[2] >= 0)
    pie(canvas, wnd_rect, color, color, &wnd->center,
      0, arc_angles[2],
      inner_radii + width, inner_radii);
  }

void on_paint_airspeed_gauge_background(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata)
  {
  on_paint_gauge_background(canvas, wnd_rect, msg, wnddata);

  gauge_widget_t* wnd = (gauge_widget_t*)wnddata;

  int16_t min_range = 0;
  int16_t max_range = 0;
  float degrees_per_unit = 1.0;

  size_t num_steps = wnd->num_steps;
  if (num_steps != 0)
    {
    const step_t* first_step = wnd->steps;
    const step_t* last_step = wnd->steps + num_steps - 1;

    min_range = first_step->value;
    max_range = last_step->value;

    int16_t steps_range = max_range - min_range;

    float arc_start = wnd->arc_begin;
    size_t i;

    degrees_per_unit = ((float)wnd->arc_range) / steps_range;

    // TODO: hard coded for now....

      // draw vs0->vno as green
    // draw vno->vne as yellow
    // draw vne->end arc range as red
    // draw vs1->vfe as white
    // draw vy->vx as blue

    int16_t band_width = wnd->arc_width << 1;

    draw_speed_band(canvas, wnd_rect, wnd, degrees_per_unit, min_range, color_green,
      wnd->gauge_radii + wnd->arc_width, band_width,
      wnd->converter->convert(mfd.aircraft.vs0), wnd->converter->convert(mfd.aircraft.vno));
    draw_speed_band(canvas, wnd_rect, wnd, degrees_per_unit, min_range, color_yellow,
      wnd->gauge_radii + wnd->arc_width, band_width,
      wnd->converter->convert(mfd.aircraft.vno), wnd->converter->convert(mfd.aircraft.vne));
    draw_speed_band(canvas, wnd_rect, wnd, degrees_per_unit, min_range, color_red,
      wnd->gauge_radii + wnd->arc_width, band_width,
      wnd->converter->convert(mfd.aircraft.vne), max_range);
    draw_speed_band(canvas, wnd_rect, wnd, degrees_per_unit, min_range, color_white,
      wnd->gauge_radii + (wnd->arc_width * 4), band_width,
      wnd->converter->convert(mfd.aircraft.vs1), wnd->converter->convert(mfd.aircraft.vfe));
    draw_speed_band(canvas, wnd_rect, wnd, degrees_per_unit, min_range, color_blue,
      wnd->gauge_radii + (wnd->arc_width * 6) + 2, band_width,
      wnd->converter->convert(mfd.aircraft.vx), wnd->converter->convert(mfd.aircraft.vy));
    }
  }
