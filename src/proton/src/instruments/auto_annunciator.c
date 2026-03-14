#include "../../include/annunciator_widget.h"

result_t on_auto_msg(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  auto_annunciator_t* ann = (auto_annunciator_t*)wnddata;

  bool changed = false;

  uint16_t can_id = get_can_id(msg);
  if (can_id == ann->base.can_id)
    {

    variant_t old_value;
    copy_variant(&ann->value, &old_value);

    variant_t new_value;
    if (failed(msg_to_variant(msg, &new_value)))
      return s_false;

    if (failed(coerce_variant(&new_value, &ann->value, ann->value_type)))
      return s_false;


    changed = compare_variant(&old_value, &ann->value) != 0;
    }

  if (changed)
    invalidate(hwnd);

  return s_false;
  }

void on_paint_auto(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnd)
  {
  auto_annunciator_t* ann = (auto_annunciator_t*)wnd;
  char txt[16];
  const char *fmt = ann->converter != nullptr ? ann->converter->format : ann->fmt;
  switch (ann->value.vt)
    {
    case v_float:
      if (ann->converter != 0)
        sprintf(txt, fmt, ann->converter->convert(ann->value.value.flt));
      else
        sprintf(txt, ann->fmt, ann->value.value.flt);
      break;
    case v_int16:
      if (ann->converter != 0)
        sprintf(txt, fmt, (int32_t) ann->converter->convert(ann->value.value.int16));
      else
        sprintf(txt, ann->fmt, ann->value.value.int16);
      break;
    case v_uint16:
      if (ann->converter != 0)
        sprintf(txt, fmt, (uint32_t) ann->converter->convert(ann->value.value.uint16));
      else
        sprintf(txt, ann->fmt, ann->value.value.uint16);
      break;
    case v_int32:
      if (ann->converter != 0)
        sprintf(txt, fmt, (int32_t) ann->converter->convert(ann->value.value.int32));
      else
        sprintf(txt, ann->fmt, ann->value.value.int32);
      break;
    case v_uint32:
      if (ann->converter != 0)
        {
        uint32_t val = ann->converter->is_float
          ? ann->converter->convert(ann->value.value.uint32)
          : (uint32_t)ann->converter->convert(ann->value.value.uint32);
        sprintf(txt, fmt, val);
        }
      else
        sprintf(txt, ann->fmt, ann->value.value.uint32);
      break;
    default:
      strcpy(txt, "---");
      break;
    }

  on_draw_text(canvas, wnd_rect, (annunciator_t*)ann, txt);
  }
