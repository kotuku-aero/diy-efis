#include "gauge_widget.h"

static bool is_bar_style(gauge_widget_t* wnd);

static int calculate_rotation(gauge_widget_t* wnd, float value);
static color_t calculate_pen(gauge_widget_t* wnd, float value);
static color_t calculate_color(gauge_widget_t* wnd, float value);

static void on_paint_hbar(handle_t canvas, gauge_widget_t* wnd, const rect_t* wnd_rect);
static void on_paint_dial(handle_t canvas, gauge_widget_t* wnd, const rect_t* wnd_rect);
static void on_paint_bar(handle_t canvas, gauge_widget_t* wnd, const rect_t* wnd_rect);

static void on_paint_background_hbar(handle_t canvas, gauge_widget_t* wnd, const rect_t* wnd_rect);
static void on_paint_background_dial(handle_t canvas, gauge_widget_t* wnd, const rect_t* wnd_rect);
static void on_paint_background_bar(handle_t canvas, gauge_widget_t* wnd, const rect_t* wnd_rect);

static void draw_pointer(handle_t canvas, gauge_widget_t* wnd, const rect_t* wnd_rect, color_t pen, int rotation);
static void draw_sweep(handle_t canvas, gauge_widget_t* wnd, const rect_t* wnd_rect, color_t pen, color_t fill, int rotation);
static void draw_bar(handle_t canvas, gauge_widget_t* wnd, const rect_t* wnd_rect, color_t pen, int rotation);
static void draw_point(handle_t canvas, gauge_widget_t* wnd, const rect_t* wnd_rect, color_t pen, int rotation);
static void draw_graph_value(handle_t canvas, gauge_widget_t* wnd, const rect_t* wnd_rect, color_t pen, int32_t offset);
static void draw_bar_graph(handle_t canvas, gauge_widget_t* wnd, const rect_t* wnd_rect, const rect_t* rect);

static void on_paint(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata)
  {
  gauge_widget_t* wnd = (gauge_widget_t*)wnddata;

  if (wnd->background_canvas == nullptr)
    {
    extent_t ex;
    rect_extents(wnd_rect, &ex);
    canvas_create(&ex, &wnd->background_canvas);

    on_paint_widget_background(wnd->background_canvas, wnd_rect, msg, wnddata);

    if (wnd->style == gs_hbar)
      on_paint_background_hbar(wnd->background_canvas, wnd, wnd_rect);
    else if (is_bar_style(wnd))
      on_paint_background_bar(wnd->background_canvas, wnd, wnd_rect);
    else
      on_paint_background_dial(wnd->background_canvas, wnd, wnd_rect);
    }

  point_t pt = { 0, 0 };
  bit_blt(canvas, wnd_rect, wnd_rect, wnd->background_canvas, wnd_rect, &pt, src_copy);

  if (wnd->style == gs_hbar)
    on_paint_hbar(canvas, wnd, wnd_rect);
  else if (is_bar_style(wnd))
    on_paint_bar(canvas, wnd, wnd_rect);
  else
    on_paint_dial(canvas, wnd, wnd_rect);
  }

static void on_paint_background_hbar(handle_t canvas, gauge_widget_t* wnd, const rect_t* wnd_rect)
  {
  int32_t bar_top = 5;
  int32_t bar_left = 2;

  size_t num_steps = wnd->num_steps;

  if (num_steps == 0)
    return;

  const step_t* steps = wnd->steps;
  int16_t range = steps[num_steps - 1].value - steps[0].value;

  int32_t wnd_width = rect_width(wnd_rect);
  float pixels_per_unit = ((float)wnd_width - 16) / ((float)range);

  // so we now have the increment, just start at the bottom
  rect_t drawing_rect;
  rect_create(wnd_rect->left + 8, wnd_rect->top + 15, wnd_rect->left + 8, wnd_rect->top + 20, &drawing_rect);

  for (uint16_t step = 1; step < num_steps; step++)
    {
    float relative_value = (float)(steps[step].value - steps[step - 1].value);

    int32_t pixels = (int32_t)(relative_value * pixels_per_unit);
    drawing_rect.right += pixels;

    rectangle(canvas, wnd_rect, color_hollow, steps[step].color, &drawing_rect);
    drawing_rect.left += pixels;
    }

  if (wnd->base.style & DRAW_NAME)
    {
    //    font(name_font);
    //    background_color(color_black);
    //    text_color(color_white);
    point_t name_pt;
    point_copy(&wnd->base.name_pt, &name_pt);

    extent_t sz;
    text_extent(wnd->base.name_font, 0, wnd->base.name, &sz);

    name_pt.x -= sz.dx >> 1;
    name_pt.y -= sz.dy >> 1;

    draw_text(canvas, wnd_rect, wnd->base.name_font,
      wnd->base.name_color, wnd->base.background_color,
      0, wnd->base.name, &name_pt, 0, 0, 0);
    }
  }

static void on_paint_hbar(handle_t canvas, gauge_widget_t* wnd, const rect_t* wnd_rect)
  {
  size_t num_steps = wnd->num_steps;

  if (num_steps == 0)
    return;

  const step_t* steps = wnd->steps;

  float min_value = steps[0].value;
  float max_value = steps[num_steps - 1].value;
  if (wnd->converter != 0)
    {
    min_value = (*wnd->converter)->convert_float(min_value);
    max_value = (*wnd->converter)->convert_float(max_value);

#ifdef _WIN32
    min_value = (*wnd->converter)->convert_float(steps[0].value);
    max_value = (*wnd->converter)->convert_float(steps[num_steps - 1].value);
#endif
    }

  int32_t wnd_width = rect_width(wnd_rect);

  int16_t range = (int16_t)(max_value - min_value);
  float pixels_per_unit = ((float)wnd_width - 16) / ((float)range);

  float value = wnd->value;

  if (value > max_value)
    value = max_value;
  else if (value < min_value)
    value = min_value;

  // make relative
  value -= min_value;
  int32_t position = (int32_t)((value * pixels_per_unit) + 8);
  point_t pts[4];
  pts[0].x = position + 4; pts[0].y = 9;
  pts[1].x = position; pts[1].y = 21;
  pts[2].x = position - 4; pts[2].y = 9;
  pts[3].x = pts[0].x; pts[3].y = pts[0].y;

  polygon(canvas, wnd_rect, color_white, color_white, 4, pts);

  // draw the value of the gauge as a text string
  if (wnd->draw_value && wnd->value_font != 0)
    {
    //      font(_value_font);
    //      text_color(color_lightblue);
    //      background_color(color_black);

    char str[32];
    sprintf(str, "%d", (int)wnd->value);

    uint16_t len = (uint16_t)strlen(str);
    extent_t size;
    text_extent(wnd->value_font, len, str, &size);

    //      pen(&gray_pen);
    //      background_color(color_black);

    if (wnd->draw_value_box)
      {
      // draw a rectangle around the text
      rectangle(canvas, wnd_rect, color_gray, color_hollow, &wnd->value_rect);

      point_t pt = {
        wnd->value_rect.right - (size.dx + 2),
        wnd->value_rect.top + 2 };

      draw_text(canvas, wnd_rect, wnd->value_font, color_lightblue, color_hollow,
        len, str, &pt, &wnd->value_rect, eto_clipped, 0);
      }
    else
      {
      point_t pt;
      rect_center(&wnd->value_rect, &pt);
      // text out is always top left
      pt.x = wnd->value_rect.left;
      pt.y -= size.dy >> 1;

      draw_text(canvas, wnd_rect, wnd->value_font, color_lightblue, color_hollow,
        len, str, &pt, &wnd->value_rect, eto_clipped, 0);
      }
    }
  }

static void on_paint_background_bar(handle_t canvas, gauge_widget_t* wnd, const rect_t* wnd_rect)
  {
  // each bar is drawn with the following dimensions:
  //
  //  *****************
  //                5 pixels
  //        ********
  //        *      *
  //        *      *
  //   +    *      *
  //   |\   *      *
  //   | \  *      *
  //   |  * *      *
  //   | /  *      *
  //   |/   *      *
  //   +    *      *
  //        ********
  //                5 pixels
  //  *****************
  //  ^ ^  ^    ^    ^
  //  | |  |    |    +- 2 Pixels
  //  | |  |    +------ 6 Pixels
  //  | |  +----------- 1 Pixel
  //  | +-------------- 5 Pixels
  //  +---------------- 1 Pixel
  //
  //
  rect_t graph =
    { wnd_rect->left, wnd_rect->top, wnd_rect->left + 14, rect_height(wnd_rect) };

  draw_bar_graph(canvas, wnd, wnd_rect, &graph);

  if (wnd->base.style & DRAW_NAME)
    {
    extent_t sz;
    text_extent(wnd->font, 0, wnd->base.name, &sz);
    point_t pt;
    rect_bottom_right(wnd_rect, &pt);

    pt.x -= 4;
    pt.x -= sz.dx;
    pt.y -= sz.dy;

    draw_text(canvas, wnd_rect, wnd->font, wnd->base.name_color, wnd->base.background_color,
      0, wnd->base.name, &pt, 0, 0, 0);
    }

  int32_t height = rect_height(&graph) - 12;

  size_t num_ticks = wnd->num_ticks;
  if (num_ticks != 0)
    {
    const tick_mark_t* first_tick = wnd->ticks;
    const tick_mark_t* last_tick = wnd->ticks + num_ticks - 1;


    int16_t range = last_tick->value - first_tick->value;
    float pixels_per_unit = ((float)height) / ((float)range);
    size_t i;
    for (i = 0; i < num_ticks; i++)
      {
      last_tick = wnd->ticks + i;

      if (last_tick->text[0] == 0)
        continue;

      long value = last_tick->value;

      uint16_t txt_len = (uint16_t)strlen(last_tick->text);
      extent_t sz;
      text_extent(wnd->font, txt_len, last_tick->text, &sz);

      float relative_value = (float)(value - first_tick->value);
      int32_t pixels = (int32_t)(relative_value * pixels_per_unit);
      point_t pt =
        {
        graph.left + 2,
        (graph.bottom - pixels) - 7 - (sz.dy >> 1)
        };

      draw_text(canvas, wnd_rect, wnd->font,
        wnd->base.name_color, wnd->base.background_color,
        txt_len, last_tick->text, &pt, 0, 0, 0);
      }
    }
  }

static void on_paint_bar(handle_t canvas, gauge_widget_t* wnd, const rect_t* wnd_rect)
  {
  // we support up to 4 ID's.
  int32_t offset = 1;

  draw_graph_value(canvas, wnd, wnd_rect, color_lightblue, offset);
  }

static void on_paint_background_dial(handle_t canvas, gauge_widget_t* wnd, const rect_t* wnd_rect)
  {
  if (wnd->base.style & DRAW_NAME)
    {
    //    font(name_font);
    //    background_color(color_black);
    //    text_color(color_white);
    point_t name_pt;
    point_copy(&wnd->base.name_pt, &name_pt);

    extent_t sz;
    text_extent(wnd->base.name_font, 0, wnd->base.name, &sz);

    name_pt.x -= sz.dx >> 1;
    name_pt.y -= sz.dy >> 1;

    draw_text(canvas, wnd_rect, wnd->base.name_font,
      wnd->base.name_color, wnd->base.background_color,
      0, wnd->base.name, &name_pt, 0, 0, 0);
    }

  int16_t min_range = 0;
  int16_t max_range = 0;
  float degrees_per_unit = 1.0;

  // we now draw an arc.  The range is the last value in the arc
  // minus the first value.  Note that the values can be -
  // the range of the gauge is gauge_y - 360 degrees
  size_t num_steps = wnd->num_steps;
  if (num_steps != 0)
    {
    const step_t* first_step = wnd->steps;
    const step_t* last_step = wnd->steps + num_steps - 1;

    min_range = first_step->value;
    max_range = last_step->value;

    int16_t steps_range = max_range - min_range;

    degrees_per_unit = ((float)wnd->arc_range) / steps_range;

    float arc_start = wnd->arc_begin;
    size_t i;

    int32_t inner_radii = wnd->gauge_radii - wnd->arc_width;

    for (i = 1; i < num_steps; i++)
      {
      last_step = wnd->steps + i;

      // the arc angle is the step end
      float arc_end = (last_step->value - min_range) * degrees_per_unit;
      arc_end += wnd->arc_begin;

      int arc_angles[3] = { (int)arc_start, (int)arc_end, -1 };

      if (arc_angles[0] > 360)
        {
        arc_angles[0] -= 360;
        arc_angles[1] -= 360;
        }
      else if (arc_angles[1] > 360)
        {
        arc_angles[2] = arc_angles[1] - 360;
        arc_angles[1] = 360;
        }

      pie(canvas, wnd_rect, last_step->pen, last_step->color, &wnd->center,
        arc_angles[0], arc_angles[1],
        wnd->gauge_radii, inner_radii);

      if (arc_angles[2] >= 0)
        pie(canvas, wnd_rect, last_step->pen, last_step->color, &wnd->center,
          0, arc_angles[2],
          wnd->gauge_radii, inner_radii);

      arc_start = arc_end;
      }
    }

  size_t num_ticks = wnd->num_ticks;
  if (num_ticks > 0)
    {
    size_t i;
    // we now draw the tick marks.  They are a line from the point 5 pixels long.

    // we create a white pen for this
    //pen(&white_pen_2);

    int32_t line_start = wnd->gauge_radii;
    line_start += wnd->bar_width >> 1;              // add the line half width

    int32_t line_end = line_start - wnd->bar_width - 5;
    int arc_start;

    for (i = 0; i < num_ticks; i++)
      {
      const tick_mark_t* tick = wnd->ticks + i;

      arc_start = (int)((tick->value - min_range) * degrees_per_unit);
      arc_start += wnd->arc_begin;
      // make it point to correct spot....
      arc_start += 90;

      point_t pts[2];

      point_copy(&wnd->center, &pts[0]);
      pts[0].y -= line_start;

      rotate_point(&wnd->center, arc_start, &pts[0]);

      point_copy(&wnd->center, &pts[1]);
      pts[1].y -= line_end;

      rotate_point(&wnd->center, arc_start, &pts[1]);

      polyline(canvas, wnd_rect, color_white, 2, pts);

      if (tick->text != 0 && wnd->font != 0)
        {
        // write the text at the point
        extent_t size;
        text_extent(wnd->font, 0, tick->text, &size);

        // the text is above the tick marks
        point_t top_left = {
          wnd->center.x,
          wnd->center.y - line_start - (size.dy >> 1)
          };

        rotate_point(&wnd->center, arc_start, &top_left);

        top_left.x -= (size.dx >> 1);
        top_left.y -= (size.dy >> 1);

        draw_text(canvas, wnd_rect, wnd->font, color_white, wnd->base.background_color,
          0, tick->text, &top_left, 0, 0, 0);
        }
      }
    }
  }

static void on_paint_dial(handle_t canvas, gauge_widget_t* wnd, const rect_t* wnd_rect)
  {
  switch (wnd->style)
    {
    case gs_pointer_minmax:
      draw_point(canvas, wnd, wnd_rect,
        calculate_pen(wnd, wnd->min_value),
        calculate_rotation(wnd, wnd->min_value));
      draw_point(canvas, wnd, wnd_rect,
        calculate_pen(wnd, wnd->max_value),
        calculate_rotation(wnd, wnd->max_value));
      break;
    case gs_pointer:
      draw_pointer(canvas, wnd, wnd_rect,
        calculate_pen(wnd, wnd->value),
        calculate_rotation(wnd, wnd->value));
      break;
    case gs_sweep:
      //draw_sweep(canvas, wnd, wnd_rect,
      //  calculate_pen(wnd, wnd->value),
      //  calculate_color(wnd, wnd->value),
      //  calculate_rotation(wnd, wnd->value));
      //break;
    case gs_bar:
      draw_bar(canvas, wnd, wnd_rect,
        calculate_pen(wnd, wnd->value),
        calculate_rotation(wnd, wnd->value));
      break;
    case gs_point_minmax:
      draw_point(canvas, wnd, wnd_rect,
        calculate_pen(wnd, wnd->min_value),
        calculate_rotation(wnd, wnd->min_value));
      draw_point(canvas, wnd, wnd_rect,
        calculate_pen(wnd, wnd->max_value),
        calculate_rotation(wnd, wnd->max_value));
    case gs_point:
      draw_point(canvas, wnd, wnd_rect,
        calculate_pen(wnd, wnd->value),
        calculate_rotation(wnd, wnd->value));
      break;
    }

  // draw the value of the gauge as a text string
  if (wnd->draw_value && wnd->value_font != 0)
    {
    //      font(_value_font);
    //      text_color(color_lightblue);
    //      background_color(color_black);

    char str[32];
    sprintf(str, "%d", (int)wnd->value);

    uint16_t len = (uint16_t)strlen(str);
    extent_t size;
    text_extent(wnd->value_font, len, str, &size);

    //      pen(&gray_pen);
    //      background_color(color_black);

    if (wnd->draw_value_box)
      {
      // draw a rectangle around the text
      rectangle(canvas, wnd_rect, color_gray, color_black, &wnd->value_rect);

      point_t pt = {
        wnd->value_rect.right - (size.dx + 2),
        wnd->value_rect.top + 2 };

      draw_text(canvas, wnd_rect, wnd->value_font, color_lightblue, color_black,
        len, str, &pt, &wnd->value_rect, eto_clipped, 0);
      }
    else
      {
      point_t pt;
      rect_center(&wnd->value_rect, &pt);
      // text out is always top left
      pt.x -= size.dx >> 1;
      pt.y -= size.dy >> 1;

      draw_text(canvas, wnd_rect, wnd->value_font, color_lightblue, color_black,
        len, str, &pt, &wnd->value_rect, eto_clipped, 0);
      }
    }
  }

bool is_bar_style(gauge_widget_t* wnd)
  {
  return wnd->style >= bgs_point;
  }

static void draw_pointer(handle_t canvas,
  gauge_widget_t* wnd,
  const rect_t* wnd_rect,
  color_t outline_pen,
  int rotation)
  {
  // draw the marker line
  point_t pts[2] =
    {
    { wnd->center.x, wnd->center.y - 5 },
    { wnd->center.x, wnd->center.y - wnd->gauge_radii + 5}
    };

  rotate_point(&wnd->center, rotation, &pts[0]);
  rotate_point(&wnd->center, rotation, &pts[1]);

  polyline(canvas, wnd_rect, outline_pen, 2, pts);
  }

static void draw_sweep(handle_t canvas,
  gauge_widget_t* wnd,
  const rect_t* wnd_rect,
  color_t outline_pen,
  color_t fill_color,
  int rotation)
  {
  pie(canvas,
    wnd_rect,
    outline_pen,
    fill_color,
    &wnd->center,
    wnd->arc_begin,
    rotation + wnd->arc_begin,
    wnd->gauge_radii - wnd->arc_width -2,
    wnd->gauge_radii - wnd->arc_width - wnd->bar_width  - 2);
  }

static void draw_bar(handle_t canvas,
  gauge_widget_t* wnd,
  const rect_t* wnd_rect,
  color_t fill_color,
  int rotation)
  {
  //rotation += 90;

  int arc_angles[3] = { wnd->arc_begin, rotation, -1 };

  if (arc_angles[0] > 360)
    {
    arc_angles[0] -= 360;
    arc_angles[1] -= 360;
    }
  else if (arc_angles[1] > 360)
    {
    arc_angles[2] = arc_angles[1] - 360;
    arc_angles[1] = 360;
    }

  int32_t radii = wnd->gauge_radii - wnd->arc_width - 1;
  int32_t inner_radii = radii - wnd->bar_width;

  pie(canvas, wnd_rect, color_hollow, fill_color, &wnd->center, arc_angles[0], arc_angles[1], radii, inner_radii);

  if (arc_angles[2] > 0)
    pie(canvas, wnd_rect, color_hollow, fill_color, &wnd->center, 0, arc_angles[2], radii, inner_radii);
  }

static void draw_point(handle_t canvas,
  gauge_widget_t* wnd,
  const rect_t* wnd_rect,
  color_t outline_pen,
  int rotation)
  {
  rotation += 90;

  point_t pts[4] =
    {
    { wnd->center.x, wnd->center.y - wnd->gauge_radii + 5 },
    { wnd->center.x - 6, wnd->center.y - wnd->gauge_radii + 11 },
    { wnd->center.x + 6, wnd->center.y - wnd->gauge_radii + 11 },
    { wnd->center.x, wnd->center.y - wnd->gauge_radii + 5 }
    };

  rotate_point(&wnd->center, rotation, &pts[0]);
  rotate_point(&wnd->center, rotation, &pts[1]);
  rotate_point(&wnd->center, rotation, &pts[2]);
  point_copy(&pts[0], &pts[3]);

  polygon(canvas, wnd_rect, outline_pen, outline_pen, 4, pts);
  }

static int calculate_rotation(gauge_widget_t* wnd, float value)
  {
  size_t count = wnd->num_steps;
  if (count == 0)
    return 0;

  const step_t* first_step = wnd->steps;
  const step_t* last_step = wnd->steps + count - 1;

  float min_range = first_step->value;
  float max_range = last_step->value;

  // get the percent that the gauge is displaying
  float percent = (max(min(value, max_range), min_range) - min_range) / (max_range - min_range);

  float rotation;
  rotation = wnd->arc_begin + (wnd->arc_range * percent);

  return (int)rotation;
  }

static color_t calculate_pen(gauge_widget_t* wnd, float value)
  {
  size_t count = wnd->num_steps;

  if (count == 0)
    return color_lightblue;

  // step 0 is only used to set the minimum value for the gauge
  // so it is ignored
  const step_t* step;
  color_t pen;
  for (size_t i = 1; i < count; i++)
    {
    // get the next one, and see if we are done.
    step = wnd->steps + i;

    // assume this is our color.
    pen = step->pen;
    if (value <= step->value)
      break;
    }

  return pen;
  }

static color_t calculate_color(gauge_widget_t* wnd, float value)
  {
  color_t fill_color = color_lightblue;
  size_t count = wnd->num_steps;

  if (count == 0)
    return fill_color;

  const step_t* step = wnd->steps;

  for (size_t i = 0; i < count; i++)
    {
    fill_color = step->color;

    step = wnd->steps + i;

    if (value >= step->value)
      break;
    }

  return fill_color;
  }

static void draw_bar_graph(handle_t canvas,
  gauge_widget_t* wnd,
  const rect_t* wnd_rect,
  const rect_t* rect)
  {
  size_t count = wnd->num_steps;

  if (count < 2)
    return;

  // calculate the height of the graph.
  long height = rect_height(rect) - 12;

  const step_t* first_step = wnd->steps;
  const step_t* last_step = wnd->steps + count - 1;

  long range = last_step->value - first_step->value;

  float pixels_per_unit = ((float)height) / ((float)range);

  // so we now have the increment, just start at the bottom
  rect_t drawing_rect = {
    rect->left + 8,
    rect->top + 3,
    rect->right - 2,
    rect->bottom - 7
    };

  for (size_t i = 1; i < count; i++)
    {
    last_step = wnd->steps + i;

    int16_t relative_value = last_step->value - first_step->value;
    memcpy(&first_step, &last_step, sizeof(step_t));

    float pixels = relative_value * pixels_per_unit;
    drawing_rect.top = drawing_rect.bottom - (int32_t)(pixels);

    rectangle(canvas, wnd_rect, 0, last_step->pen, &drawing_rect);
    drawing_rect.bottom = drawing_rect.top;
    }
  }

static void draw_graph_value(handle_t canvas,
  gauge_widget_t* wnd,
  const rect_t* wnd_rect,
  color_t outline_pen,
  int32_t offset)
  {
  // calculate the height of the graph.
  int32_t height = rect_height(wnd_rect) - 12;
  size_t count = wnd->num_steps;

  if (count < 2)
    return;

  const step_t* first_step = wnd->steps;
  const step_t* last_step = wnd->steps + count - 1;


  int32_t range = last_step->value - first_step->value;
  float pixels_per_unit = ((float)height) / ((float)range);

  float min_range = first_step->value;

  float value = wnd->value;
  value = max(value, min_range);
  value = min(value, min_range + range);

  float relative_value = value - min_range;
  int32_t position = (int32_t)(height - (relative_value * pixels_per_unit) - 8);

  position = max(position, 5);

  point_t pts[4] =
    {
      { offset, position + 5 },
      { offset + 5, position },
      { offset, position - 5 },
      { offset, position + 5 }
    };

  polygon(canvas, wnd_rect, outline_pen, outline_pen, 4, pts);

  if (wnd->style == bgs_pointer_minmax || wnd->style == bgs_pointer_max)
    {
    relative_value = wnd->max_value - min_range;
    if (relative_value >= 0.0)
      {
      position = (int32_t)(height - (relative_value * pixels_per_unit) - 8);
      position = max(position, 5);

      pts[0].x = offset; pts[0].y = position + 5;
      pts[1].x = offset + 5; pts[1].y = position;
      pts[2].x = offset; pts[2].y = position - 5;
      pts[3].x = offset; pts[3].y = position + 5;

      polygon(canvas, wnd_rect, outline_pen, color_hollow, 4, pts);
      }
    }

  if (wnd->style == bgs_pointer_minmax || wnd->style == bgs_pointer_min)
    {
    relative_value = wnd->min_value - min_range;
    if (relative_value >= 0.0)
      {
      position = (int32_t)(height - (relative_value * pixels_per_unit) - 8);
      position = max(position, 5);

      pts[0].x = offset; pts[0].y = position + 5;
      pts[1].x = offset + 5; pts[1].y = position;
      pts[2].x = offset; pts[2].y = position - 5;
      pts[3].x = offset; pts[3].y = position + 5;

      polygon(canvas, wnd_rect, outline_pen, color_hollow, 4, pts);
      }
    }
  }

/*
  uint16_t i;
  for (i = 0; i < wnd->num_values; i++)
    {
    add_event(*hwnd, wnd->labels[i], wnd, 0, on_value_label);
    }
*/
result_t gauge_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  gauge_widget_t* wnd = (gauge_widget_t*)wnddata;
  bool changed = false;

  uint16_t id = get_can_id(msg);

  if (wnd->reset_id == id)
    {
    wnd->min_value = wnd->reset_value;
    wnd->max_value = wnd->min_value;
    changed = true;
    }
  else if (wnd->can_id == id)
    {
    float float_value;
    if (succeeded(get_param_float(msg, &float_value)))
      {
      if (wnd->converter != 0)
        float_value = (*wnd->converter)->convert_float(float_value);

      changed = wnd->value != float_value;
      wnd->value = float_value;

      wnd->min_value = min(wnd->min_value, wnd->value);
      wnd->max_value = max(wnd->max_value, wnd->value);
      }
    }

  if (changed)
    invalidate(hwnd);

  return widget_wndproc(hwnd, msg, wnddata);
  }

result_t create_gauge_widget(handle_t parent, uint16_t id, aircraft_t* aircraft, gauge_widget_t* wnd, handle_t* out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result = create_widget(parent, id, gauge_wndproc, &wnd->base, &hndl)))
    return result;

  wnd->base.on_paint = on_paint;

  if (out != 0)
    *out = hndl;

  return s_ok;
  }
