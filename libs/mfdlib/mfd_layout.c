#include "mfd_layout.h"

#include "../proton/map_widget.h"

result_t on_create_navigator_widget(handle_t hwnd, widget_t* widget)
  {
  result_t result;
  map_widget_t *wnd = (map_widget_t *)widget;

    

  wnd->params.ned_center.elevation = (int16_t)feet_to_meters(2500);      // about 2500 ft
  // rangioira
  // -43.290001,172.542007
  //wnd->params.ned_center.y = float_to_fixed(-43.290001);
  //wnd->params.ned_center.x = float_to_fixed(172.542007);

  // stratford aerodrome
  // -39.318319, 174.308126
  //wnd->params.ned_center.y = float_to_fixed(-39.318319f);
  //wnd->params.ned_center.x = float_to_fixed(174.308126f);

  // npl VOR
  // -39.007241, 174.183977
  wnd->params.ned_center.latlng.lng = float_to_fixed(-39.007241f);
  wnd->params.ned_center.latlng.lat = float_to_fixed(174.183977f);

  // Ardmore
  // -37.064376, 174.978367
  //wnd->params.ned_center.y = float_to_fixed(-37.064376);
  //wnd->params.ned_center.x = float_to_fixed(174.978367);

  // oshkosh wi
  // 43.998287, -88.567753
  //wnd->params.ned_center.y = float_to_fixed(43.998287);
  //wnd->params.ned_center.x = float_to_fixed(-88.567753);

  // sfo
  // 37.466539, -122.242943
  //wnd->params.ned_center.y = float_to_fixed(37.466539);
  //wnd->params.ned_center.x = float_to_fixed(-122.242943);

  // OAK
  // 37.619909, -122.207544
  //wnd->params.ned_center.y = float_to_fixed(37.619909f);
  //wnd->params.ned_center.x = float_to_fixed(-122.207544f);

  wnd->params.scale = (int32_t)(90 * meters_per_nm);             // width of the scaled screen in meters
  wnd->map_center.x = rect_width(&widget->rect) >> 1;
  //wnd->map_center.y = ex.dy - 120;  // the HSI is 240 x 240 so center above bottom
  //wnd->params.mode = mdm_track_up;
  wnd->map_center.y = rect_height(&widget->rect) >> 1;     // north up mode centers map
  wnd->params.mode = mdm_north;

  /*********************************************************************/
  //  Terrain
  wnd->show_terrain = false;

  wnd->terrain_params.show_terrain_warning = false;
  wnd->terrain_params.alarm_elevation = (uint32_t)feet_to_meters(100);
  wnd->terrain_params.warning_elevation = (uint32_t)feet_to_meters(1000);

  wnd->terrain_params.show_hillshade = false;

  wnd->terrain_params.show_terrain = true;

  wnd->terrain_params.hypsometric_tint = true;

  /*********************************************************************/
  //  Contours
  wnd->show_contours = true;

  /*********************************************************************/
  //  Cities
  wnd->show_cities = true;


  /*********************************************************************/
  //  Cities
  wnd->show_water = true;


  if (failed(result = load_layers(wnd, "C:\\Projects\\map-data")))
    return result;

  // hard coded for now.


  text_extent(wnd->params.font, 1, "M", &wnd->font_cell_size);
  wnd->font_cell_size.dx += 2;

  return result;
  }
