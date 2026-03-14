#ifndef __dialog_theme_h__
#define __dialog_theme_h__

#include "../../../include/sys_canfly.h"

typedef enum _dialog_focus_highlight_style
  {
  focus_highlight_style_none,
  focus_highlight_style_dashed,
  focus_highlight_style_dots,
  focus_highlight_style_dashdot,
  focus_highlight_style_solid,
  } dialog_focus_highlight_style;

typedef struct _dialog_theme_t
  {
  const font_t *label_font;
  color_t label_color;
  const font_t *edit_font;
  color_t edit_color;
  color_t border_color;
  color_t alarm_color;
  
  color_t background_color;
  color_t background_color_selected;
  color_t background_color_disabled;
  color_t background_color_pressed;
  color_t background_color_editing;

  color_t foreground_color;
  color_t foreground_color_selected;
  color_t foreground_color_disabled;
  color_t foreground_color_pressed;
  color_t foreground_color_editing;

  color_t focused_highlight_color;
  dialog_focus_highlight_style focus_highlight_style;
  } dialog_theme_t;

extern const dialog_theme_t *dialog_theme;
/**
 * Called when an application is loaded to set the application theme
 * @param theme Theme to use
 */
extern result_t set_layout_theme(const dialog_theme_t *theme);
extern result_t draw_control_focus(handle_t canvas, const rect_t *clip_rect, const rect_t *rect);

#endif
