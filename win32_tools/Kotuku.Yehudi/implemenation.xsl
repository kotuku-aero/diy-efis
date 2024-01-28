<?xml version="1.0" encoding="utf-8" ?>
<stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:xs="http://www.w3.org/2001/XMLSchema"
                xmlns:kt="http://kotuku.aero/schema/kotuku.xsd"
                xmlns="http://www.w3.org/1999/XSL/Transform">
  <output method="text" indent="yes" />
  <param name="app-name"></param>

  <template match="//kt:application">
    #include "<value-of select="$app-name"/>.h"
    #include "../../libs/proton/airspeed_widget.h"
    #include "../../libs/proton/altitude_widget.h"
    #include "../../libs/proton/annunciator_widget.h"
    #include "../../libs/proton/ap_widget.h"
    #include "../../libs/proton/attitude_widget.h"
    #include "../../libs/proton/carousel_widget.h"
    #include "../../libs/proton/comm_widget.h"
    #include "../../libs/proton/edutemp_widget.h"
    #include "../../libs/proton/gauge_widget.h"
    #include "../../libs/proton/hsi_widget.h"
    #include "../../libs/proton/map_widget.h"
    #include "../../libs/proton/marquee_widget.h"
    #include "../../libs/proton/menu_window.h"
    #include "../../libs/proton/softkey_widget.h"
    #include "../../libs/proton/pancake_indicator_widget.h"
    <!-- emit all forward definitions -->

    <for-each select="./kt:menu/kt:keys">
      <call-template name="define-keymap-fwd">
        <with-param name="definition" select="."/>
      </call-template>
    </for-each>

    <for-each select="./kt:menu/kt:menu">
      <call-template name="define-menu-fwd">
        <with-param name="definition" select="."/>
        <with-param name="name" select="./@name"/>
      </call-template>
    </for-each>

    <!-- emit all of the keys -->
    <for-each select="./kt:menu/kt:keys">
      <call-template name="define-keymap">
        <with-param name="definition" select="."/>
      </call-template>
    </for-each>

    <!-- emit all of the menu definitions -->
    <for-each select="./kt:menu/kt:menu">
      <call-template name="define-menu">
        <with-param name="definition" select="."/>
        <with-param name="name" select="./@name"/>
      </call-template>
    </for-each>

    <!-- emit all of the menu variables for the window -->
    <call-template name="define-menu-refs">
      <with-param name="definition" select="./kt:menu/kt:menu"/>
    </call-template>

    <call-template name="define-menu-widget">
      <with-param name="definition" select="./kt:menu"/>
      <with-param name="decl-name" select="'menu_wnd'"/>
    </call-template>


    <for-each select="./kt:layout/*">
      <choose>
        <when test="local-name(.) = 'airspeed'">
          static airspeed_widget_t <value-of select="@id"/> = {
          <call-template name="define-widget">
            <with-param name="definition" select="."/>
            <with-param name="prefix" select="'.base'"/>
          </call-template>
          .text_color = <value-of select="@text-color"/>,
          .pen = <value-of select="@pen-color"/>,
          .font = <text disable-output-escaping="yes">&amp;</text><value-of select="@font"/>,
          .large_roller = <text disable-output-escaping="yes">&amp;</text><value-of select="@large-roller-font"/>,
          .small_roller = <text disable-output-escaping="yes">&amp;</text><value-of select="@small-roller-font"/>,
          <choose>
            <when test="@pixels-per-unit">
              .pixels_per_unit = <value-of select="pixels-per-unit"/>,
            </when>
            <otherwise>
              .pixels_per_unit = 10,
            </otherwise>
          </choose>
          };
        </when>
        <when test="local-name(.) = 'altitude'">
          static altitude_widget_t <value-of select="@id"/> = {
          <call-template name="define-widget">
            <with-param name="definition" select="."/>
            <with-param name="prefix" select="'.base'"/>
          </call-template>
          .text_color = <value-of select="@text-color"/>,
          .pen = <value-of select="@pen-color"/>,
          .font = <text disable-output-escaping="yes">&amp;</text><value-of select="@font"/>,
          .large_roller = <text disable-output-escaping="yes">&amp;</text><value-of select="@large-roller-font"/>,
          .small_roller = <text disable-output-escaping="yes">&amp;</text><value-of select="@small-roller-font"/>,
          };
        </when>
        <when test="local-name(.) = 'hsi'">
          static hsi_widget_t <value-of select="@id"/> = {
        <call-template name="define-widget">
          <with-param name="definition" select="."/>
          <with-param name="prefix" select="'.base'"/>
        </call-template>
        };
      </when>
        <when test="local-name(.) = 'text'">
          static text_annunciator_t <value-of select ="@id"/> ={
          <call-template name="define-text-annunciator">
            <with-param name="definition" select="."/>
            <with-param name="prefix" select="''"/>
          </call-template>
          };
        </when>
        <when test="local-name(.) = 'auto'">
          static auto_annunciator_t <value-of select ="@id"/> ={
          <call-template name="define-text-annunciator">
            <with-param name="definition" select="."/>
            <with-param name="prefix" select="'.base'"/>
          </call-template>
          <if test="@converter">
            .converter = <text disable-output-escaping="yes">&amp;</text><value-of select="@converter"/>,
          </if>
          .value_type = <value-of select="@value-type"/>,
          .fmt = "<value-of select="@format"/>",
          .base.base.base.on_message = on_auto_msg,
          .base.base.base.on_paint_foreground = on_paint_auto,
          .base.base.base.on_paint_background = on_paint_background_auto,
          };
        </when>
        <when test="local-name(.) = 'utc'">
          static utc_annunciator_t <value-of select ="@id"/> ={
          <call-template name="define-text-annunciator">
            <with-param name="definition" select="."/>
            <with-param name="prefix" select="'.base'"/>
          </call-template>
          .base.base.base.om_message = on_def_utc_msg,
          .base.base.base.on_paint_foreground = on_paint_utc,
          .base.base.base.on_paint_background = on_paint_background_auto,
          };
        </when>
        <when test="local-name(.) = 'hours'">
          static hours_annunciator_t <value-of select ="@id"/> ={
          <call-template name="define-text-annunciator">
            <with-param name="definition" select="."/>
            <with-param name="prefix" select="'.base'"/>
          </call-template>
          .base.base.base.on_message = on_hours_msg,
          .base.base.base.on_paint_foreground = on_paint_hours,
          .base.base.base.on_paint_background = on_paint_background_auto,
          };
        </when>
        <when test="local-name(.) = 'hp'">
          static hp_annunciator_t <value-of select ="@id"/> ={
          <call-template name="define-text-annunciator">
            <with-param name="definition" select="."/>
            <with-param name="prefix" select="'.base'"/>
          </call-template>
          .base.base.base.on_message = on_hp_msg,
          .base.base.base.on_paint_foreground = on_paint_hp,
          .base.base.base.on_paint_background = on_paint_background_auto,
          };
        </when>
        <when test="local-name(.) = 'ecu'">
          static ecu_annunciator_t <value-of select ="@id"/> ={
          <call-template name="define-widget">
            <with-param name="definition" select="."/>
            <with-param name="prefix" select="'.base.base'"/>
          </call-template>
          .base.base.on_message = on_kmag_msg,
          .base.base.on_paint_foreground = on_paint_kmag,
          };
        </when>
        <when test="local-name(.) = 'hobbs'">
          static hobbs_annunciator_t <value-of select ="@id"/> ={
          <call-template name="define-text-annunciator">
            <with-param name="definition" select="."/>
            <with-param name="prefix" select="'.base'"/>
          </call-template>
          .base.base.base.on_message = on_hobbs_mg,
          .base.base.base.on_paint_foreground = on_paint_hobbs,
          };
        </when>
        <when test="local-name(.) = 'autopilot'">
          static ap_widget_t <value-of select ="@id"/> ={
          <call-template name="define-widget">
            <with-param name="definition" select="."/>
            <with-param name="prefix" select="'.base'"/>
          </call-template>
          .button_height = <value-of select="./@button-height"/>,
          .height = <value-of select="./@height"/>,
          .gutter = <value-of select="./@gutter"/>,
          .info_color = <value-of select="./@info-color"/>,
          .active_color = <value-of select="./@active-color"/>,
          .standby_color = <value-of select="./@standby-color"/>,
          };
        </when>
        <when test="local-name(.) = 'attitude'">
          static attitude_widget_t <value-of select ="@id"/> ={
          <call-template name="define-widget">
            <with-param name="definition" select="."/>
            <with-param name="prefix" select="'.base'"/>
          </call-template>
          .median.x = <value-of select="./kt:median/@x"/>,
          .median.y = <value-of select="./kt:median/@y"/>,
          <if test="./@show-aoa">
          .show_aoa = <value-of select="./@show-aoa"/>,
          </if>
          <if test="./@show-aoa">
          .show_glideslope = <value-of select="./@show-glideslope"/>,
          </if>
          };
        </when>
        <when test="local-name(.) = 'edu-temps'">
          static edu_temp_widget_t <value-of select ="@id"/> ={
          <call-template name="define-widget">
            <with-param name="definition" select="."/>
            <with-param name="prefix" select="'.base'"/>
          </call-template>
          .num_cylinders = <value-of select="./@num-cyl"/>,
          .cht_red_line = <value-of select ="./@cht-redline"/>,
          .egt_red_line = <value-of select="./@egt-redline"/>,
          .cht_min = <value-of select="./@cht-min"/>,
          .egt_min = <value-of select="./@egt-min"/>,
          .left_gutter = <value-of select="./@left-gutter"/>,
          .right_gutter = <value-of select="./@right-gutter"/>,
          .cylinder_draw_width = <value-of select="./@cylinder-draw-width"/>,
          .egt_line = <value-of select="./@egt-line"/>,
          .cht_line = <value-of select="./@cht-line"/>,
          .cht_red_line_pos = <value-of select="./@cht-redline-pos"/>,
          .bar_top = <value-of select="./@bar-top"/>,
          .bar_bottom = <value-of select="./@bar-bottom"/>,
          .status_top = <value-of select="./@status-top"/>,
          .font = <text disable-output-escaping="yes">&amp;</text><value-of select="./@font"/>,
          .cht_color= <value-of select="./@cht-color"/>,
          .egt_color= <value-of select="./@egt-color"/>,
          
          <for-each select="./kt:cht">
            <variable name="index" select="position()"/>
            <call-template name="define-temp-bar">
              <with-param name="definition" select="."/>
              <with-param name="name" select="concat('.cht[',$index,'-1]')"/>
            </call-template>
          </for-each>
          <for-each select="./kt:egt">
            <variable name="index" select="position()"/>
            <call-template name="define-temp-bar">
              <with-param name="definition" select="."/>
              <with-param name="name" select="concat('.egt[',$index,'-1]')"/>
            </call-template>
          </for-each>
          };
         </when>
        <when test="local-name(.) = 'gauge'">
          <if test="./kt:step">
            static const step_t <value-of select ="@id"/>_steps[] = {
            <for-each select="./kt:step">
              {
              .value = <value-of select="./@value"/>,
              .pen = <value-of select="./@pen"/>,
              .color = <value-of select="./@color"/>,
              },
            </for-each>
            };
          </if>
          <if test="./kt:tick">
            static const tick_mark_t <value-of select ="@id"/>_ticks[] = {
            <for-each select="./kt:tick">
              {
              .value = <value-of select="./@value"/>,
              <if test="./@text">
                .text = "<value-of select="./@text"/>",
              </if>
              },
            </for-each>
            };
          </if>
          static gauge_widget_t <value-of select ="@id"/> ={
          <call-template name="define-widget">
            <with-param name="definition" select="."/>
            <with-param name="prefix" select="'.base'"/>
          </call-template>
          <choose>
            <when test="./@style = 'pointer'">
              .style = gs_pointer,
            </when>
            <when test="./@style = 'sweep'">
              .style = gs_sweep,
            </when>
            <when test="./@style = 'bar'">
              .style = gs_bar,
            </when>
            <when test="./@style = 'point'">
              .style = gs_point,
            </when>
            <when test="./@style = 'pointer_minmax'">
              .style = gs_pointer_minmax,
            </when>
            <when test="./@style = 'point_minmax'">
              .style = gs_point_minmax,
            </when>
            <when test="./@style = 'hbar'">
              .style = gs_hbar,
            </when>
            <when test="./@style = 'small'">
              .style = bgs_small,
            </when>
          </choose>
          .font = <text disable-output-escaping="yes">&amp;</text><value-of select="./@font"/>,
          .can_id = <value-of select="./@can-id"/>,
          <if test="./@reset-id">
            .reset_id = <value-of select="./@reset-id"/>,
          </if>
          <if test="./@radii">
            .gauge_radii = <value-of select="./@radii"/>,
          </if>
          <if test="./@arc-width">
            .arc_width = <value-of select="./@arc-width"/>,
          </if>
          <if test="./@bar-width">
            .bar_width = <value-of select="./@bar-width"/>,
          </if>
          <if test="./@arc-begin">
            .arc_begin = <value-of select="./@arc-begin"/>,
          </if>
          <if test="./@arc-range">
            .arc_range = <value-of select="./@arc-range"/>,
          </if>
          <if test="./@reset-value">
            .reset_value = <value-of select="./@reset-value"/>,
          </if>
          <if test="./@pointer-width">
            .bar_width = <value-of select="./@pointer-width"/>,
          </if>
          <if test="./@draw-value">
            .draw_value = <value-of select="./@draw-value"/>,
          </if>
          <if test="./@value-font">
            .value_font = <text disable-output-escaping="yes">&amp;</text><value-of select="./@value-font"/>,
          </if>
          <if test="./@draw-value-box">
            .draw_value_box = <value-of select="./@draw-value-box"/>,
          </if>
          <if test="./@converter">
            .converter = <text disable-output-escaping="yes">&amp;</text><value-of select="./@converter"/>,
          </if>
          <if test="./kt:name-pt">
            .base.name_pt.x = <value-of select="./kt:name-pt/@x"/>,
            .base.name_pt.y = <value-of select="./kt:name-pt/@y"/>,
          </if>
          <if test="./kt:center">
            .center.x = <value-of select="./kt:center/@x"/>,
            .center.y = <value-of select="./kt:center/@y"/>,
          </if>
          <if test="./kt:value-rect">
            .value_rect.left = <value-of select="./kt:value-rect/@left"/>,
            .value_rect.top = <value-of select="./kt:value-rect/@top"/>,
            .value_rect.right = <value-of select="./kt:value-rect/@left"/> + <value-of select="./kt:value-rect/@width"/>,
            .value_rect.bottom = <value-of select="./kt:value-rect/@top"/> + <value-of select="./kt:value-rect/@height"/>,
          </if>
          <if test="./kt:step">
            .steps = <value-of select ="@id"/>_steps,
            .num_steps = numelements(<value-of select ="@id"/>_steps),
          </if>
          <if test="./kt:tick">
            .ticks = <value-of select ="@id"/>_ticks,
            .num_ticks = numelements(<value-of select ="@id"/>_ticks),
          </if>

          };
        </when>
        <when test="local-name(.) = 'map'">
          static map_widget_t <value-of select ="@id"/> ={
          <call-template name="define-widget">
            <with-param name="definition" select="."/>
            <with-param name="prefix" select="'.base'"/>
          </call-template>
          <call-template name="define-map-theme">
            <with-param name="definition" select="./kt:day-theme"/>
            <with-param name="prefix" select="'.params.day_theme'"/>
          </call-template>
          <call-template name="define-map-theme">
            <with-param name="definition" select="./kt:night-theme"/>
            <with-param name="prefix" select="'.params.night_theme'"/>
          </call-template>
          .params.font = <text disable-output-escaping="yes">&amp;</text><value-of select="./@font"/>,
          <if test="./@show-terrain">
          .show_terrain = <value-of select="./@show-terrain"/>,
          </if>
          <choose>
            <when test="./@show-contours">
              .show_contours = <value-of select="./@show-contours"/>,
            </when>
            <otherwise>
              .show_contours = true,
            </otherwise>
          </choose>
          <choose>
            <when test="./@show-cities">
              .show_cities = <value-of select="./@show-cities"/>,
            </when>
            <otherwise>
              .show_cities = true,
            </otherwise>
          </choose>
          <choose>
            <when test="./@show-water">
              .show_water = <value-of select="./@show-water"/>,
            </when>
            <otherwise>
              .show_water = true,
            </otherwise>
          </choose>
          };
        </when>
        <when test="local-name(.) = 'pancake'">
          static pancake_widget_t <value-of select ="@id"/> ={
          <call-template name="define-widget">
            <with-param name="definition" select="."/>
            <with-param name="prefix" select="'.base'"/>
          </call-template>
          .indicator_rect.left = <value-of select="./kt:indicator-rect/@left"/>,
          .indicator_rect.top = <value-of select="./kt:indicator-rect/@top"/>,
          .indicator_rect.right = <value-of select="./kt:indicator-rect/@left"/> + <value-of select="./kt:indicator-rect/@width"/>,
          .indicator_rect.bottom = <value-of select="./kt:indicator-rect/@top"/> + <value-of select="./kt:indicator-rect/@height"/>,
          .can_id = <value-of select="./@can-id"/>,
          .outline_color = <value-of select="./@outline-color"/>,
          .indicator_color = <value-of select="./@indicator-color"/>,
          .bar_width = <value-of select="./@width"/>,
          .bar_height = <value-of select="./@height"/>,
          .bar_gutter = <value-of select="./@gutter"/>,
          .min_value = <value-of select="./@min-value"/>,
          .max_value = <value-of select="./@max-value"/>,
          <if test="./@horizontal">
          .horizontal = <value-of select="./@horizontal"/>,
          </if>
          <if test="./@draw-value">
          .draw_value = <value-of select="./@draw-value"/>,
          </if>
          <if test="./@draw-value-box">
          .draw_value_box = <value-of select="./@draw-value-box"/>,
          </if>
          <if test="./@value-font">
          .value_font = <value-of select="./@value-font"/>,
          </if>
          <if test="./kt:value-rect"><value-of select="./kt:value-rect/@left"/>,
          .value_rect.left = <value-of select="./kt:value-rect/@left"/>,
          .value_rect.top = <value-of select="./kt:value-rect/@top"/>,
          .value_rect.right = <value-of select="./kt:value-rect/@left"/> + <value-of select="./kt:value-rect/@width"/>,
          .value_rect.bottom = <value-of select="./kt:value-rect/@top"/> + <value-of select="./kt:value-rect/@height"/>,
          </if>
          };
        </when>
        <when test="local-name(.) = 'marquee'">

          <variable name="name" select ="./@id"/>
          <!-- Emit all of the alarms -->
          <for-each select="./kt:alarm">
            <variable name="alarm" select="position()"/>
            static const uint16_t <value-of select ="$name"/>_alarm_ids_<value-of select="$alarm"/>[] = {
            <for-each select="./kt:alarm-id">
              <value-of select="."/>,
            </for-each>
            };
          </for-each>

          static alarm_t <value-of select ="$name"/>_alarms[] = {
          <for-each select="./kt:alarm">
            <variable name="alarm" select="position()"/>
            {
            .can_ids = <value-of select ="$name"/>_alarm_ids_<value-of select="$alarm"/>,
            .num_ids = numelements(<value-of select ="$name"/>_alarm_ids_<value-of select="$alarm"/>),
            .priority = <value-of select="./@priority"/>,
            .can_park = <value-of select="./@can-park"/>,
            .message = "<value-of select="./@message"/>",
            .base.style = DRAW_BACKGROUND,
            .base.background_color = <value-of select="../@alarm-background"/>,
            .base.name_color = <value-of select="../@alarm-text"/>,
            .base.rect.left = 1,
            .base.rect.top = 1,
            .base.rect.right = <value-of select="../kt:rect/@width"/> -2,
            .base.rect.bottom = <value-of select="../kt:rect/@height"/> -2,
            .base.name_font = <text disable-output-escaping="yes">&amp;</text><value-of select="../@font"/>,
            .base.on_paint_foreground = on_paint_alarm_foreground,
            },
          </for-each>
          };

          <for-each select="./*">
            <variable name="index" select="position()"/>
            <choose>
              <when test="local-name(.) = 'text'">
                static text_annunciator_t <value-of select ="concat($name, '_ann_', $index)"/> ={
                <call-template name="define-text-annunciator">
                  <with-param name="definition" select="."/>
                  <with-param name="prefix" select="''"/>
                </call-template>
                };
              </when>
              <when test="local-name(.) = 'auto'">
                static auto_annunciator_t <value-of select ="concat($name, '_ann_', $index)"/> ={
                <call-template name="define-text-annunciator">
                  <with-param name="definition" select="."/>
                  <with-param name="prefix" select="'.base'"/>
                </call-template>
                <if test="@converter">
                  .converter = <text disable-output-escaping="yes">&amp;</text><value-of select="@converter"/>,
                </if>
                .value_type = <value-of select="@value-type"/>,
                .fmt = "<value-of select="@format"/>",
                };
              </when>
              <when test="local-name(.) = 'utc'">
                static utc_annunciator_t <value-of select ="concat($name, '_ann_', $index)"/> ={
                <call-template name="define-text-annunciator">
                  <with-param name="definition" select="."/>
                  <with-param name="prefix" select="'.base'"/>
                </call-template>
                .base.base.base.on_message = on_def_utc_msg,
                .base.base.base.on_paint_foreground = on_paint_utc,
                };
              </when>
              <when test="local-name(.) = 'hours'">
                static hours_annunciator_t <value-of select ="concat($name, '_ann_', $index)"/> ={
                <call-template name="define-text-annunciator">
                  <with-param name="definition" select="."/>
                  <with-param name="prefix" select="'.base'"/>
                </call-template>
                .base.base.base.on_message = on_hours_msg,
                .base.base.base.on_paint_foreground = on_paint_hours,
                };
              </when>
              <when test="local-name(.) = 'hp'">
                static hp_annunciator_t <value-of select ="concat($name, '_ann_', $index)"/> ={
                <call-template name="define-text-annunciator">
                  <with-param name="definition" select="."/>
                  <with-param name="prefix" select="'.base'"/>
                </call-template>
                .base.base.base.on_message = on_hp_msg,
                .base.base.base.on_paint_foreground = on_paint_hp,
                };
              </when>
              <when test="local-name(.) = 'hobbs'">
                static hobbs_annunciator_t <value-of select ="concat($name, '_ann_', $index)"/> ={
                <call-template name="define-text-annunciator">
                  <with-param name="definition" select="."/>
                  <with-param name="prefix" select="'.base'"/>
                </call-template>
                .base.base.base.on_message = on_hobbs_msg,
                .base.base.base.on_paint_foreground = on_paint_hobbs,
                };
              </when>

            </choose>
          </for-each>

          static annunciator_t* <value-of select ="@id"/>_annunciators[] = {
          <for-each select="./*">
            <variable name="index" select="position()"/>
            <choose>
              <when test="local-name(.) = 'alarm'"></when>
              <when test="local-name(.) = 'rect'"></when>
              <otherwise>
                (annunciator_t *) <text disable-output-escaping="yes">&amp;</text><value-of select="concat($name, '_ann_', $index)"/>,
              </otherwise>
            </choose>
          </for-each>
          };

          static marquee_widget_t <value-of select ="@id"/> ={
          <call-template name="define-widget">
            <with-param name="definition" select="."/>
            <with-param name="prefix" select="'.base'"/>
          </call-template>
          .base_widget_id = <value-of select="@base-id"/>,
          .base_alarm_id = <value-of select="@base-alarm-id"/>,
        .annunciators = <value-of select ="@id"/>_annunciators,
          .num_annunciators = numelements(<value-of select ="@id"/>_annunciators),
          .alarms = <value-of select ="@id"/>_alarms,
          .num_alarms = numelements(<value-of select ="@id"/>_alarms),
          };
        </when>
      </choose>
    </for-each>

    result_t create_<value-of select="$app-name"/>(handle_t hwnd, aircraft_t *aircraft)
    {
    result_t result;
    handle_t child;
    handle_t widget;
    
    <for-each select="./kt:layout/kt:*">
    <variable name="id" select="position()"/>
    <choose>
      <when test="local-name(.) = 'airspeed'">
        if(failed(result = create_airspeed_widget(hwnd, <value-of select="$id"/>, aircraft, <text disable-output-escaping="yes">&amp;</text><value-of select="./@id"/>, 0)))
        return result;
      </when>
      <when test="local-name(.) = 'altitude'">
        if(failed(result = create_altitude_widget(hwnd, <value-of select="$id"/>, aircraft, <text disable-output-escaping="yes">&amp;</text><value-of select="./@id"/>, 0)))
        return result;
      </when>
      <when test="local-name(.) = 'hsi'">
        if(failed(result = create_hsi_widget(hwnd, <value-of select="$id"/>, aircraft, <text disable-output-escaping="yes">&amp;</text><value-of select="./@id"/>, 0)))
        return result;
      </when>
      <when test="local-name(.) = 'text'">
        if(failed(result = create_annunciator_widget(hwnd, <value-of select="$id"/>, aircraft, (annunciator_t *) <text disable-output-escaping="yes">&amp;</text><value-of select="./@id"/>, 0)))
        return result;
      </when>
      <when test="local-name(.) = 'auto'">
        if(failed(result = create_annunciator_widget(hwnd, <value-of select="$id"/>, aircraft, (annunciator_t *) <text disable-output-escaping="yes">&amp;</text><value-of select="./@id"/>, 0)))
        return result;
      </when>
      <when test="local-name(.) = 'utc'">
        if(failed(result = create_annunciator_widget(hwnd, <value-of select="$id"/>, aircraft, (annunciator_t *) <text disable-output-escaping="yes">&amp;</text><value-of select="./@id"/>, 0)))
        return result;
      </when>
      <when test="local-name(.) = 'hours'">
        if(failed(result = create_annunciator_widget(hwnd, <value-of select="$id"/>, aircraft, (annunciator_t *) <text disable-output-escaping="yes">&amp;</text><value-of select="./@id"/>, 0)))
        return result;
      </when>
      <when test="local-name(.) = 'hp'">
        if(failed(result = create_annunciator_widget(hwnd, <value-of select="$id"/>, aircraft, (annunciator_t *) <text disable-output-escaping="yes">&amp;</text><value-of select="./@id"/>, 0)))
        return result;
      </when>
      <when test="local-name(.) = 'ecu'">
        if(failed(result = create_annunciator_widget(hwnd, <value-of select="$id"/>, aircraft, (annunciator_t *) <text disable-output-escaping="yes">&amp;</text><value-of select="./@id"/>, 0)))
        return result;
      </when>
      <when test="local-name(.) = 'hobbs'">
        if(failed(result = create_annunciator_widget(hwnd, <value-of select="$id"/>, aircraft, (annunciator_t *) <text disable-output-escaping="yes">&amp;</text><value-of select="./@id"/>, 0)))
        return result;
      </when>
      <when test="local-name(.) = 'autopilot'">
        if(failed(result = create_autopilot_widget(hwnd, <value-of select="$id"/>, aircraft, <text disable-output-escaping="yes">&amp;</text><value-of select="./@id"/>, 0)))
        return result;
      </when>
      <when test="local-name(.) = 'attitude'">
        if(failed(result = create_attitude_widget(hwnd, <value-of select="$id"/>, aircraft, <text disable-output-escaping="yes">&amp;</text><value-of select="./@id"/>, 0)))
        return result;
      </when>
      <when test="local-name(.) = 'edu-temps'">
        if(failed(result = create_edutemps_widget(hwnd, <value-of select="$id"/>, aircraft, <text disable-output-escaping="yes">&amp;</text><value-of select="./@id"/>, 0)))
        return result;
      </when>
      <when test="local-name(.) = 'gauge'">
        if(failed(result = create_gauge_widget(hwnd, <value-of select="$id"/>, aircraft, <text disable-output-escaping="yes">&amp;</text><value-of select="./@id"/>, 0)))
        return result;
      </when>
      <when test="local-name(.) = 'map'">
        if(failed(result = create_map_widget(hwnd, <value-of select="$id"/>, aircraft, <text disable-output-escaping="yes">&amp;</text><value-of select="./@id"/>, 0)))
        return result;
      </when>
      <when test="local-name(.) = 'pancake'">
        if(failed(result = create_pancake_widget(hwnd, <value-of select="$id"/>, aircraft, <text disable-output-escaping="yes">&amp;</text><value-of select="./@id"/>, 0)))
        return result;
      </when>
      <when test="local-name(.) = 'marquee'">

        if(failed(result = create_marquee_widget(hwnd, <value-of select="$id"/>, aircraft, <text disable-output-escaping="yes">&amp;</text><value-of select="./@id"/>, <text disable-output-escaping="yes">&amp;</text>widget)))
        return result;

        <value-of select="./@id"/>.selected_alarm = -1;

        uint16_t child_num = 0;
        <variable name="marquee" select="."/>

        <for-each select="./*">
          <variable name="index" select="position()"/>

          <if test="local-name(.) != 'alarm' and local-name(.) != 'rect'">
            if(failed(result = create_annunciator_widget(widget, <value-of select="$marquee/@id"/>.base_widget_id + child_num++, aircraft, (annunciator_t *) <text disable-output-escaping="yes">&amp;</text><value-of select ="concat($marquee/@id, '_ann_', $index)"/>, <text disable-output-escaping="yes">&amp;</text>child)))
            return result;

            hide_window(child);
          </if>
        </for-each>
        if(failed(result = show_marquee_child(widget, 0)))
        return result;

        child_num = 0;
        <for-each select="./kt:alarm">
          <variable name="alarm" select="position()"/>
          if(failed(result = create_alarm_annunciator(widget, <value-of select="$marquee/@id"/>.base_alarm_id + child_num, <text disable-output-escaping="yes">&amp;</text><value-of select ="../@id"/>_alarms[child_num], 0)))
          return result;
          child_num++;
        </for-each>
      </when>
    </choose>
  </for-each>

      return create_menu_window(hwnd, 0, aircraft, <text disable-output-escaping="yes">&amp;</text>menu_wnd, 0);
    }
  </template>

  <template name="define-text-annunciator">
    <param name="definition"/>
    <param name="prefix"/>

    <call-template name="define-widget">
      <with-param name="definition" select="$definition"/>
      <with-param name="prefix" select="concat($prefix, '.base.base')"/>
    </call-template>
    <value-of select="$prefix"/>.can_id = <value-of select="$definition/@can-id"/>,
    <value-of select="$prefix"/>.label_color = <value-of select="$definition/@label-color"/>,
    <value-of select="$prefix"/>.text_color  = <value-of select="$definition/@text-color"/>,
    <value-of select="$prefix"/>.small_font = <text disable-output-escaping="yes">&amp;</text><value-of select="$definition/@small-font"/>,
    <value-of select="$prefix"/>.label_offset = <value-of select="$definition/@label-offset"/>,
    <value-of select="$prefix"/>.text_offset = <value-of select="$definition/@text-offset"/>,
  </template>

  <template name="define-widget">
    <param name="definition"/>
    <param name="prefix"/>
    <param name="on-paint"/>
    <param name="on-paint-background"/>
    <param name="style" />

    <!-- todo fixme -->

    <value-of select="$prefix"/>.style =
    <if test="$style">
      <value-of select="$style"/>
    </if>
    <if test="$definition/@border-left = 'true'">
      BORDER_LEFT |
    </if>
    <if test="$definition/@border-top = 'true'">
      BORDER_TOP |
    </if>
    <if test="$definition/@border-right= 'true'">
      BORDER_RIGHT |
    </if>
    <if test="$definition/@border-bottom = 'true'">
      BORDER_BOTTOM |
    </if>
    <if test="$definition/@draw-name = 'true'">
      DRAW_NAME |
    </if>
    <if test="$definition/@fill-background = 'true'">
      DRAW_BACKGROUND |
    </if>
    BORDER_NONE,
    <if test="$definition/@border-color">
      <value-of select="$prefix"/>.border_color = <value-of select="$definition/@border-color"/>,
    </if>
    <if test="$definition/@background-color">
      <value-of select="$prefix"/>.background_color = <value-of select="$definition/@background-color"/>,
    </if>
    <if test="$definition/@name-color">
      <value-of select="$prefix"/>.name_color = <value-of select="$definition/@name-color"/>,
    </if>
    <if test="$definition/@name">
      <value-of select="$prefix"/>.name = "<value-of select="$definition/@name"/>",
    </if>
    <if test="$definition/@name-font">
      <value-of select="$prefix"/>.name_font  = <text disable-output-escaping="yes">&amp;</text><value-of select="$definition/@name-font"/>,
    </if>
    <if test="$definition/name-pt">
      <value-of select="$prefix"/>.name_pt.x = <value-of select="$definition/kt:name-pt/@x"/>,
      <value-of select="$prefix"/>.name_pt.y = <value-of select="$definition/kt:name-pt/@y"/>,
    </if>
    <if test="$definition/@z-order">
      <value-of select="$prefix"/>.z_order = <value-of select="$definition/@z-order"/>,
    </if>
    <if test="$definition/@on-paint">
      <value-of select="$prefix"/>.on_paint_foreground = <value-of select="$definition/@on-paint"/>,
    </if>
    <if test="$definition/@on-paint-background">
      <value-of select="$prefix"/>.on_paint_background = <value-of select="$definition/@on-paint-background"/>,
    </if>
    <if test="$definition/@on-message">
      <value-of select="$prefix"/>.on_message = <value-of select="$definition/@on-message"/>,
    </if>
    <if test="$definition/@on-create">
      <value-of select="$prefix"/>.on_create = <value-of select="$definition/@on-create"/>,
    </if>
    <choose>
      <when test="@status-timeout">
        <value-of select="$prefix"/>.status_timeout = <value-of select="$definition/@status-timeout"/>,
      </when>
      <otherwise>
        <value-of select="$prefix"/>.status_timeout = 10000,
      </otherwise>
    </choose>
    <choose>
      <when test="./@sensor-id">
        <value-of select="$prefix"/>.sensor_id = <value-of select="./@sensor-id"/>,
        <value-of select="$prefix"/>.sensor_failed = true,
      </when>
      <otherwise>
        <value-of select="$prefix"/>.sensor_failed = false,
      </otherwise>
    </choose>
    <if test="$definition/@alarm-id">
      <value-of select="$prefix"/>.alarm_id = <value-of select="$definition/@alarm-id"/>,
    </if>
    <choose>
      <when test="@alarm-color">
        <value-of select="$prefix"/>.alarm_color = <value-of select="$definition/@alarm-color"/>,
      </when>
      <otherwise>
        <value-of select="$prefix"/>.alarm_color = color_red,
      </otherwise>
    </choose>
    
    <value-of select="$prefix"/>.rect.left = <value-of select="$definition/kt:rect/@left"/>,
    <value-of select="$prefix"/>.rect.top = <value-of select="$definition/kt:rect/@top"/>,
    <value-of select="$prefix"/>.rect.right = <value-of select="$definition/kt:rect/@left"/> + <value-of select="$definition/kt:rect/@width"/>,
    <value-of select="$prefix"/>.rect.bottom = <value-of select="$definition/kt:rect/@top"/> + <value-of select="$definition/kt:rect/@height"/>,
  </template>

  <template name="define-menu-fwd">
    <param name="definition"/>

    static menu_t <value-of select="./@name"/>;
  </template>

  <template name="define-menu">
    <param name="definition"/>
    <param name="name"/>

    <!-- Emit the handlers -->
    <for-each select="./*">
      <variable name="i" select="position()" />
      <choose>
        <when test="local-name(.) = 'event-action'">
          <call-template name="define-event">
            <with-param name="definition" select="."/>
            <with-param name="name" select="concat($name, '_item_', $i)"/>
          </call-template>
        </when>
        <when test="local-name(.) = 'popup-action'">
          <call-template name="define-popup">
            <with-param name="definition" select ="."/>
            <with-param name="name" select="concat($name, '_item_', $i)"/>
          </call-template>
        </when>
        <when test="local-name(.) = 'checklist-action'">
          <call-template name="define-checklist">
            <with-param name="definition" select ="."/>
            <with-param name="name" select="concat($name, '_item_', $i)"/>
          </call-template>
        </when>
        <when test="local-name(.) = 'spin-edit-action'">
          <call-template name="define-spin-edit">
            <with-param name="definition" select ="."/>
            <with-param name="name" select="concat($name, '_item_', $i)"/>
          </call-template>
        </when>
      </choose>
    </for-each>

    static menu_item_t *<value-of select="concat($name, '_items')"/>[] = {
    <for-each select="./*">
      <variable name="i" select="position()" />
      <text  disable-output-escaping="yes">(menu_item_t *) &amp;</text><value-of select="concat($name, '_item_', $i)"/>,
    </for-each>
    };

    static menu_t <value-of select="$name"/> = {
    .menu_items = <text  disable-output-escaping="yes">(menu_item_t **) &amp;</text><value-of select="concat($name, '_items')"/>,
    .num_items = numelements(<value-of select="concat($name, '_items')"/>),
    .name = "<value-of select="./@name"/>",
    .caption = "<value-of select="./@caption"/>",
    .keys = <text  disable-output-escaping="yes">&amp;</text><value-of select="./@keys"/>
    };
  </template>

  <template name="define-menu-refs">
    <param name="definition"/>
    menu_t *menus[] = {
    <for-each select="$definition">
      <text  disable-output-escaping="yes">&amp;</text><value-of select="@name"/>,
    </for-each>
    };
    
    size_t num_menus = numelements(menus);

    menu_item_t *menu_items[] = {
    <for-each select="$definition">
      <variable name="name" select="./@name"/>
      <for-each select="./*">
        <variable name="i" select="position()" />
        <text  disable-output-escaping="yes">(menu_item_t *) &amp;</text><value-of select="concat($name, '_item_', $i)"/>,
      </for-each>
    </for-each>
    };

    size_t num_menu_items = numelements(menu_items);
  </template>

  <template name="define-menu-widget">
    <param name="definition"/>
    <param name="decl-name"/>
    static menu_widget_t <value-of select="$decl-name"/> = {
    <call-template name="define-widget">
      <with-param name="definition" select="$definition"/>
      <with-param name="prefix" select="'.base'"/>
      <with-param name="on-paint" select="'menu_window_on_paint'"/>
    </call-template>
    .menu_rect.x = <value-of select="$definition/kt:menu-rect/@x"/>,
    .menu_rect.y = <value-of select="$definition/kt:menu-rect/@y"/>,
    .menu_start.x = <value-of select="$definition/kt:menu-start/@x"/>,
    .menu_start.y = <value-of select="$definition/kt:menu-start/@y"/>,
    .selected_background_color = <value-of select="$definition/@selected-background-color"/>,
    .text_color = <value-of select="$definition/@text-color"/>,
    .selected_color = <value-of select="$definition/@selected-color"/>,
    .menu_timeout = <value-of select="$definition/@menu-timeout"/>,
    .font = <text  disable-output-escaping="yes">&amp;</text> <value-of select="$definition/@font"/>,
  .root_keys = <text  disable-output-escaping="yes">&amp;</text> <value-of select="$definition/@root-keys"/>,
    .alarm_keys = <text  disable-output-escaping="yes">&amp;</text> <value-of select="$definition/@alarm-keys"/>,
    };
  </template>

  <template name="define-keymap-fwd">
    <param name="definition"/>
    static keys_t <value-of select="./@name"/>;
  </template>

  <template name="define-keymap">
    <param name="definition"/>

    <if test="./kt:key0">
      <call-template name="generate-key-handler">
        <with-param name="definition" select="./kt:key0"/>
        <with-param name="name" select="concat(./@name, '_key0')"/>
      </call-template>
    </if>
    <if test="./kt:key1">
      <call-template name="generate-key-handler">
        <with-param name="definition" select="./kt:key1"/>
        <with-param name="name" select="concat(./@name, '_key1')"/>
      </call-template>
    </if>
    <if test="./kt:key2">
      <call-template name="generate-key-handler">
        <with-param name="definition" select="./kt:key2"/>
        <with-param name="name" select="concat(./@name, '_key2')"/>
      </call-template>
    </if>
    <if test="./kt:key3">
      <call-template name="generate-key-handler">
        <with-param name="definition" select="./kt:key3"/>
        <with-param name="name" select="concat(./@name, '_key3')"/>
      </call-template>
    </if>
    <if test="./kt:key4">
      <call-template name="generate-key-handler">
        <with-param name="definition" select="./kt:key4"/>
        <with-param name="name" select="concat(./@name, '_key4')"/>
      </call-template>
    </if>
    <if test="./kt:key5">
      <call-template name="generate-key-handler">
        <with-param name="definition" select="./kt:key5"/>
        <with-param name="name" select="concat(./@name, '_key5')"/>
      </call-template>
    </if>
    <if test="./kt:key6">
      <call-template name="generate-key-handler">
        <with-param name="definition" select="./kt:key6"/>
        <with-param name="name" select="concat(./@name, '_key6')"/>
      </call-template>
    </if>
    <if test="./kt:key7">
      <call-template name="generate-key-handler">
        <with-param name="definition" select="./kt:key7"/>
        <with-param name="name" select="concat(./@name, '_key7')"/>
      </call-template>
    </if>
    <if test="./kt:decka-up">
      <call-template name="generate-key-handler">
        <with-param name="definition" select="./kt:decka-up"/>
        <with-param name="name" select="concat(./@name, '_decka_up')"/>
      </call-template>
    </if>
    <if test="./kt:decka-dn">
      <call-template name="generate-key-handler">
        <with-param name="definition" select="./kt:decka-dn"/>
        <with-param name="name" select="concat(./@name, '_decka_dn')"/>
      </call-template>
    </if>
    <if test="./kt:deckb-up">
      <call-template name="generate-key-handler">
        <with-param name="definition" select="./kt:deckb-up"/>
        <with-param name="name" select="concat(./@name, '_deckb_up')"/>
      </call-template>
    </if>
    <if test="./kt:deckb-dn">
      <call-template name="generate-key-handler">
        <with-param name="definition" select="./deckb-dn"/>
        <with-param name="name" select="concat(./@name, '_deckb_dn')"/>
      </call-template>
    </if>
    <if test="./kt:decka-press-up">
      <call-template name="generate-key-handler">
        <with-param name="definition" select="./kt:decka-press-up"/>
        <with-param name="name" select="concat(./@name, '_decka_press_up')"/>
      </call-template>
    </if>
    <if test="./kt:decka-press-dn">
      <call-template name="generate-key-handler">
        <with-param name="definition" select="./kt:decka-press-dn"/>
        <with-param name="name" select="concat(./@name, '_decka_press_dn')"/>
      </call-template>
    </if>
    <if test="./kt:deckb-press-up">
      <call-template name="generate-key-handler">
        <with-param name="definition" select="./kt:deckb-press-up"/>
        <with-param name="name" select="concat(./@name, '_deckb_press_up')"/>
      </call-template>
    </if>
    <if test="./kt:deckb-press-dn">
      <call-template name="generate-key-handler">
        <with-param name="definition" select="./deckb-press-dn"/>
        <with-param name="name" select="concat(./@name, '_deckb_press_dn')"/>
      </call-template>
    </if>

    static keys_t <value-of select="./@name"/> = {
    <if test="./kt:key0">
      .key0 = <text  disable-output-escaping="yes">(menu_item_t *) &amp;menu_item_</text><value-of select="./@name"/>_key0,
    </if>
    <if test="./kt:key1">
      .key1 = <text  disable-output-escaping="yes">(menu_item_t *) &amp;menu_item_</text><value-of select="./@name"/>_key1,
    </if>
    <if test="./kt:key2">
      .key2 = <text  disable-output-escaping="yes">(menu_item_t *) &amp;menu_item_</text><value-of select="./@name"/>_key2,
    </if>
    <if test="./kt:key3">
      .key3 = <text  disable-output-escaping="yes">(menu_item_t *) &amp;menu_item_</text><value-of select="./@name"/>_key3,
    </if>
    <if test="./kt:key4">
      .key4 = <text  disable-output-escaping="yes">(menu_item_t *) &amp;menu_item_</text><value-of select="./@name"/>_key4,
    </if>
    <if test="./kt:key5">
      .key5 = <text  disable-output-escaping="yes">(menu_item_t *) &amp;menu_item_</text><value-of select="./@name"/>_key5,
    </if>
    <if test="./kt:key6">
      .key6 = <text  disable-output-escaping="yes">(menu_item_t *) &amp;menu_item_</text><value-of select="./@name"/>_key6,
    </if>
    <if test="./kt:key7">
      .key7 = <text  disable-output-escaping="yes">(menu_item_t *) &amp;menu_item_</text><value-of select="./@name"/>_key7,
    </if>
    <if test="./kt:decka-up">
      .decka_up = <text  disable-output-escaping="yes">(menu_item_t *) &amp;menu_item_</text><value-of select="./@name"/>_decka_up,
    </if>
    <if test="./kt:decka-dn">
      .decka_dn = <text  disable-output-escaping="yes">(menu_item_t *) &amp;menu_item_</text><value-of select="./@name"/>_decka_dn,
    </if>
    <if test="./kt:deckb-up">
      .deckb_up = <text  disable-output-escaping="yes">(menu_item_t *) &amp;menu_item_</text><value-of select="./@name"/>_deckb_up,
    </if>
    <if test="./kt:deckb-dn">
      .deckb_dn = <text  disable-output-escaping="yes">(menu_item_t *) &amp;menu_item_</text><value-of select="./@name"/>_deckb_dn,
    </if>
    <if test="./kt:decka-press-up">
      .decka_press_up = <text  disable-output-escaping="yes">(menu_item_t *) &amp;menu_item_</text><value-of select="./@name"/>_decka_press_up,
    </if>
    <if test="./kt:decka-press-dn">
      .decka_press_dn = <text  disable-output-escaping="yes">(menu_item_t *) &amp;menu_item_</text><value-of select="./@name"/>_decka_press_dn,
    </if>
    <if test="./kt:deckb-press-up">
      .deckb_press_up = <text  disable-output-escaping="yes">(menu_item_t *) &amp;menu_item_</text><value-of select="./@name"/>_deckb_press_up,
    </if>
    <if test="./kt:deckb-press-dn">
      .deckb_press_dn = <text  disable-output-escaping="yes">(menu_item_t *) &amp;menu_item_</text><value-of select="./@name"/>_deckb_press_dn,
    </if>
    };
  </template>

  <template name="generate-key-handler">
    <param name="definition"/>
    <param name="name"/>

    <choose>
      <when test="$definition/kt:event-action">
        <call-template name="define-event">
          <with-param name="definition" select="$definition/kt:event-action"/>
          <with-param name="name" select="concat('menu_item_', $name)"/>
        </call-template>
      </when>
      <when test="$definition/kt:popup-action">
        <call-template name="define-popup">
          <with-param name="definition" select ="$definition/kt:popup-action"/>
          <with-param name="name" select="concat('menu_item_', $name)"/>
        </call-template>
      </when>
      <when test="$definition/kt:checklist-action">
        <call-template name="define-checklist">
          <with-param name="definition" select ="$definition/kt:checklist-action"/>
          <with-param name="name" select="concat('menu_item_', $name)"/>
        </call-template>
      </when>
    </choose>
  </template>

  <template name="define-event">
    <param name="definition"/>
    <param name="name"/>

    static menu_item_event_t <value-of select="$name"/> = {
    .can_id = <value-of select="$definition/@can-id"/>,
    .base.evaluate = item_event_evaluate,
    .base.event = default_msg_handler,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    <call-template name="define-variant">
      <with-param name="member-name" select='"value"'/>
      <with-param name="value-type" select="$definition/@value-type"/>
      <with-param name="value" select="$definition/@value"/>
    </call-template>
    <if test="$definition/@caption">
      .base.caption = "<value-of select="$definition/@caption"/>",
    </if>
    <if test="$definition/@enable-regex">
      .base.enable_regex = "<value-of select="$definition/@enable-regex"/>",
    </if>
    <if test="$definition/@enable-format">
      .base.enable_format = "<value-of select="$definition/@enable-format"/>",
    </if>
    <if test="$definition/@controlline-param">
      .base.controlling_param = <value-of select="$definition/@controlling-param"/>,
    </if>

    };
  </template>

  <template name="define-popup">
    <param name="definition"/>
    <param name="name"/>

    static menu_item_menu_t  <value-of select="$name"/> = {
    .base.caption = "<value-of select="$definition/@caption"/>",
    .base.event = default_msg_handler,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    .base.evaluate = item_menu_evaluate,
    .menu = <text  disable-output-escaping="yes">&amp;</text><value-of select="$definition/@menu"/>
    };
  </template>

  <template name="define-checklist">
    <param name="definition"/>
    <param name="name"/>

    static menu_item_checklist_t  <value-of select="$name"/> = {
    .base.caption = "<value-of select="$definition/@caption"/>",
    .base.event = item_checklist_event,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    .base.evaluate = item_checklist_evaluate,
    .get_selected = <value-of select="$definition/@get-index"/>,
     .popup = <text disable-output-escaping="yes">&amp;</text><value-of select="$definition/@popup"/>
    };
  </template>

  <template name="define-spin-edit">
    <param name="definition"/>
    <param name="name"/>

    static menu_item_spin_edit_t  <value-of select="$name"/> = {
    .base.caption = "<value-of select="$definition/@caption"/>",
    .base.event = item_spin_edit_event,
    .base.paint = default_paint_handler,
    .base.is_enabled = default_enable_handler,
    .base.evaluate = item_spin_edit_evaluate,
    .get_value = <value-of select="$definition/@get-method"/>,
    .set_value = <value-of select="$definition/@set-method"/>,
    .keys = <text disable-output-escaping="yes">&amp;</text><value-of select="$definition/@keys"/>,
    <if test="$definition/@circular">
      .circular = <value-of select="$definition/@circular"/>,
    </if>
    <if test="$definition/@digits">
      .digits = <value-of select="$definition/@digits"/>,
    </if>
    <if test="$definition/@min-value">
      .min_value = <value-of select="$definition/@min-value"/>,
    </if>
    <if test="$definition/@max-value">
      .max_value = <value-of select="$definition/@max-value"/>,
    </if>
    };
  </template>

  <template name="define-button-widget">
    <param name="definition"/>
    <param name="name"/>

    static button_widget_t <value-of select="$name"/> = {
    <call-template name="define-widget">
      <with-param name="definition" select="$definition"/>
      <with-param name="prefix" select="'.base'"/>
    </call-template>

    .can_id = <value-of select="$definition/@can-id"/>,
    <call-template name="define-variant">
      <with-param name="member-name" select="'value'"/>
      <with-param name="value-type" select ="$definition/@value-type"/>
      <with-param name="value" select="$definition/@value"/>
    </call-template>
    };
  </template>

  <template name="define-text-box-widget">
    <param name="definition"/>
    <param name="name"/>

    static textbox_widget_t <value-of select="$name"/> = {
    <variable name="style">
      <choose>
        <when test="$definition/@h-align = 'left'">
          TEXT_LEFT |
        </when>
        <when test="$definition/@h-align = 'center'">
          TEXT_CENTER |
        </when>
        <when test="$definition/@h-align = 'right'">
          TEXT_RIGHT |
        </when>
      </choose>
      <choose>
        <when test="$definition/@v-align = 'top'">
          TEXT_TOP |
        </when>
        <when test="$definition/@v-align = 'middle'">
          TEXT_MIDDLE |
        </when>
        <when test="$definition/@h-align = 'bottom'">
          TEXT_BOTTOM |
        </when>
      </choose>
      <if test="$definition/@wrap = 'true'">
        TEXT_WRAP |
      </if>
    </variable>
    <call-template name="define-widget">
      <with-param name="definition" select="$definition"/>
      <with-param name="style" select="$style"/>
      <with-param name="prefix" select="'.base'"/>
    </call-template>
    };
  </template>

  <template name="define-label-widget">
    <param name="definition"/>
    <param name="name"/>

    static label_t <value-of select="$name"/> = {
    <variable name="style">
      <choose>
        <when test="$definition/@h-align = 'left'">
          TEXT_LEFT |
        </when>
        <when test="$definition/@h-align = 'center'">
          TEXT_CENTER |
        </when>
        <when test="$definition/@h-align = 'right'">
          TEXT_RIGHT |
        </when>
      </choose>
      <choose>
        <when test="$definition/@v-align = 'top'">
          TEXT_TOP |
        </when>
        <when test="$definition/@v-align = 'middle'">
          TEXT_MIDDLE |
        </when>
        <when test="$definition/@h-align = 'bottom'">
          TEXT_BOTTOM |
        </when>
      </choose>
      <if test="$definition/@wrap = 'true'">
        TEXT_WRAP |
      </if>
    </variable>
    <call-template name="define-widget">
      <with-param name="definition" select="$definition"/>
      <with-param name="style" select="$style"/>
      <with-param name="prefix" select="'.base'"/>
    </call-template>
    };
  </template>

  <template name="define-alert">
    <param name="definition"/>
    <param name="name"/>

    static uint16_t <value-of select="concat($name, '_alarms[]')"/> = {
    <for-each select="$definition/alarm-id"/>,
    };

    static dialog_t <value-of select="$name"/> = {

    };
  </template>

  <template name="define-variant">
    <param name="member-name"/>
    <param name="value-type"/>
    <param name="value"/>

    .<value-of select="$member-name"/>.vt = <value-of select="$value-type"/>,
    <choose>
      <when test="$value-type = 'v_bool'">
        .<value-of select="$member-name"/>.value.boolean = <value-of select="$value"/>,
      </when>
      <when test="$value-type = 'v_int8'">
        .<value-of select="$member-name"/>.value.int8 = <value-of select="$value"/>,
      </when>
      <when test="$value-type = 'v_int16'">
        .<value-of select="$member-name"/>.value.int16 = <value-of select="$value"/>,
      </when>
      <when test="$value-type = 'v_int32'">
        .<value-of select="$member-name"/>.value.int32 = <value-of select="$value"/>,
      </when>
      <when test="$value-type = 'v_uint8'">
        .<value-of select="$member-name"/>.value.uint8 = <value-of select="$value"/>,
      </when>
      <when test="$value-type = 'v_uint16'">
        .<value-of select="$member-name"/>.value.uint16 = <value-of select="$value"/>,
      </when>
      <when test="$value-type = 'v_uint32'">
        .<value-of select="$member-name"/>.value.uint32 = <value-of select="$value"/>,
      </when>
      <when test="$value-type = 'v_float'">
        .<value-of select="$member-name"/>.value.flt = <value-of select="$value"/>,
      </when>
    </choose>
  </template>

  <template name="define-temp-bar">
    <param name="definition"/>
    <param name="name"/>
    <value-of select="$name"/>.can_id = <value-of select="$definition/@can-id"/>,
    <value-of select="$name"/>.default_color = <value-of select="$definition/@color"/>,
    <value-of select="$name"/>.alarm_id = <value-of select="$definition/@alarm-id"/>,
    <value-of select="$name"/>.alarm_color = <value-of select="$definition/@alarm-color"/>,
    <value-of select="$name"/>.bar_name = "<value-of select="$definition/@name"/>",
    <value-of select="$name"/>.sensor_id = <value-of select="$definition/@sensor-id"/>,
    <value-of select="$name"/>.value = 273,
  </template>

  <template name="define-map-theme">
    <param name="definition"/>
    <param name="prefix"/>
    <value-of select="$prefix"/>.alarm_color = <value-of select="$definition/@alarm-color"/>,
    <value-of select="$prefix"/>.warning_color = <value-of select="$definition/@warning-color"/>,
    <value-of select="$prefix"/>.water_color = <value-of select="$definition/@water-color"/>,
    <value-of select="$prefix"/>.land_color = <value-of select="$definition/@land-color"/>,
    <value-of select="$prefix"/>.coastline_color = <value-of select="$definition/@coastline-color"/>,
    <value-of select="$prefix"/>.residential_street = <value-of select="$definition/@residential-street"/>,
    <value-of select="$prefix"/>.residential_area = <value-of select="$definition/@residential-area"/>,
    <value-of select="$prefix"/>.big_road = <value-of select="$definition/@big-road"/>,
    <value-of select="$prefix"/>.major_road = <value-of select="$definition/@major-road"/>,
    <value-of select="$prefix"/>.highway = <value-of select="$definition/@highway"/>,
    <value-of select="$prefix"/>.class_a_airspace = <value-of select="$definition/@class-a-airspace"/>,
    <value-of select="$prefix"/>.class_b_airspace = <value-of select="$definition/@class-b-airspace"/>,
    <value-of select="$prefix"/>.class_c_airspace = <value-of select="$definition/@class-c-airspace"/>,
    <value-of select="$prefix"/>.class_d_airspace = <value-of select="$definition/@class-d-airspace"/>,
    <value-of select="$prefix"/>.class_e_airspace = <value-of select="$definition/@class-e-airspace"/>,
    <value-of select="$prefix"/>.class_f_airspace = <value-of select="$definition/@class-f-airspace"/>,
    <value-of select="$prefix"/>.class_g_airspace = <value-of select="$definition/@class-g-airspace"/>,
    <value-of select="$prefix"/>.class_m_airspace = <value-of select="$definition/@class-m-airspace"/>,
    <value-of select="$prefix"/>.cfz_airspace = <value-of select="$definition/@cfz-airspace"/>,
    <value-of select="$prefix"/>.mbz_airspace = <value-of select="$definition/@mbz-airspace"/>,
    <value-of select="$prefix"/>.danger_area = <value-of select="$definition/@danger-area"/>,
    <value-of select="$prefix"/>.restricted_area = <value-of select="$definition/@restricted-area"/>,
    <value-of select="$prefix"/>.runway_grass_pen = <value-of select="$definition/@grass-runway-outline"/>,
    <value-of select="$prefix"/>.runway_grass_color = <value-of select="$definition/@grass-runway-fill"/>,
    <value-of select="$prefix"/>.runway_grass_ext_color = <value-of select="$definition/@grass-runway-extension"/>,
    <value-of select="$prefix"/>.runway_pen = <value-of select="$definition/@runway-outline"/>,
    <value-of select="$prefix"/>.runway_color = <value-of select="$definition/@runway-fill"/>,
    <value-of select="$prefix"/>.runway_ext_color = <value-of select="$definition/@runway-extension"/>,
    <for-each select="./kt:contour">
      <variable name="index" select="position()"/>
      <value-of select="$prefix"/>.theme[<value-of select="$index"/>-1].elevation = <value-of select="./@elevation"/>,
      <value-of select="$prefix"/>.theme[<value-of select="$index"/>-1].color = <value-of select="./@color"/>,
      <if test="./@contour_color">
      <value-of select="$prefix"/>.theme[<value-of select="$index"/>-1].contour_color = <value-of select="./@contour_color"/>,
      </if>
    </for-each>
  </template>

</stylesheet>