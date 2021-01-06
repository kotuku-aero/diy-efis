/*
#include "../../nano/CLR/Include/nanoCLR_Interop.h"
#include "../../nano/CLR/Include/nanoCLR_Runtime.h"
#include "../../nano/CLR/Include/nanoPackStruct.h"
*/

#include "../canflylib/CanFly_CoreLibrary.h"

#ifdef RGB
#undef RGB
#endif

#ifdef HWND_BROADCAST
#undef HWND_BROADCAST
#endif

#include "../../neutron/neutron.h"
#include "../../photon/photon.h"

#include "HeapBlockDispatcher.h"


extern "C" {
  static result_t screen_wndproc(handle_t hwnd, const canmsg_t *msg)
    {
    // post this to the message queue
    g_CLR_MessageDispatcher.SaveToProtonQueue(msg);
    }
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::OpenScreen___STATIC__U4__U2__U2(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  uint16_t param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 0, param0));

  uint16_t param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 1, param1));

  handle_t hwnd;
  hr = open_screen(param0, screen_wndproc, param1, &hwnd);

  NANOCLR_CHECK_HRESULT(hr);

  SetResult_UINT32(stack, (unsigned int) hwnd);
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::CreateWindow___STATIC__U4__U4__I4__I4__I4__I4__U2(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  signed int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 1, param1));

  signed int param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 2, param2));

  signed int param3;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 3, param3));

  signed int param4;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 4, param4));

  uint16_t param5;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 5, param5));

  rect_t bounds;
  bounds.left = param1;
  bounds.top = param2;
  bounds.right = param3;
  bounds.bottom = param4;
  handle_t hwnd;

  hr = create_window((handle_t)param0, &bounds, defwndproc, param5, &hwnd);

  NANOCLR_CHECK_HRESULT(hr);
  SetResult_UINT32(stack, (unsigned int)hwnd);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::CreateChildWindow___STATIC__U4__U4__I4__I4__I4__I4__U2(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  signed int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 1, param1));

  signed int param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 2, param2));

  signed int param3;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 3, param3));

  signed int param4;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 4, param4));

  uint16_t param5;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 5, param5));

  rect_t bounds;
  bounds.left = param1;
  bounds.top = param2;
  bounds.right = param3;
  bounds.bottom = param4;

  handle_t hwnd;

  hr = create_child_window((handle_t)param0, &bounds, defwndproc, param5, &hwnd);

  NANOCLR_CHECK_HRESULT(hr);
  SetResult_UINT32(stack, (unsigned int)hwnd);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::CloseWindow___STATIC__VOID__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  hr = close_window((handle_t)param0);
  NANOCLR_CHECK_HRESULT(hr);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::GetWindowRect___STATIC__VOID__U4__BYREF_I4__BYREF_I4__BYREF_I4__BYREF_I4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  rect_t wnd_rect;
  hr = get_window_rect((handle_t)param0, &wnd_rect);
  NANOCLR_CHECK_HRESULT(hr);

  stack.Arg2().NumericByRef().s4 = wnd_rect.left;
  stack.Arg3().NumericByRef().s4 = wnd_rect.top;
  stack.Arg4().NumericByRef().s4 = wnd_rect.right;
  stack.Arg5().NumericByRef().s4 = wnd_rect.bottom;

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::GetWindowPos___STATIC__VOID__U4__BYREF_I4__BYREF_I4__BYREF_I4__BYREF_I4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  rect_t pos;
  hr = get_window_pos((handle_t)param0, &pos);
  NANOCLR_CHECK_HRESULT(hr);

  stack.Arg2().NumericByRef().s4 = pos.left;
  stack.Arg3().NumericByRef().s4 = pos.top;
  stack.Arg4().NumericByRef().s4 = pos.right;
  stack.Arg5().NumericByRef().s4 = pos.bottom;

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::SetWindowPos___STATIC__VOID__U4__I4__I4__I4__I4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  signed int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 1, param1));

  signed int param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 2, param2));

  signed int param3;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 3, param3));

  signed int param4;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 4, param4));

  rect_t rect;
  rect.left = param1;
  rect.top = param2;
  rect.right = param3;
  rect.bottom = param4;

  hr = set_window_pos((handle_t)param0, &rect);
  
  NANOCLR_CHECK_HRESULT(hr);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::GetWindowData___STATIC__OBJECT__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  void *wnd_data;
  hr = get_wnddata((handle_t)param0, &wnd_data);

  NANOCLR_CHECK_HRESULT(hr);
 
  stack.SetResult_Object((CLR_RT_HeapBlock *)wnd_data);  

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::SetWindowData___STATIC__VOID__U4__OBJECT(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  hr = set_wnddata((handle_t)param0, stack.Arg1().Dereference());
  NANOCLR_CHECK_HRESULT(hr);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::GetParent___STATIC__U4__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  handle_t hwnd;
  hr = get_parent((handle_t)param0, &hwnd);
  NANOCLR_CHECK_HRESULT(hr);
  SetResult_UINT32(stack, (unsigned int)hwnd);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::GetWindowById___STATIC__U4__U4__U2(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  uint16_t param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 1, param1));

  handle_t hndl;
  hr = get_window_by_id((handle_t)param0, param1, &hndl);

  NANOCLR_CHECK_HRESULT(hr);
  SetResult_UINT32(stack, (unsigned int)hndl);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::GetFirstChild___STATIC__U4__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  handle_t hndl;
  hr = get_first_child((handle_t)param0, &hndl);

  NANOCLR_CHECK_HRESULT(hr);
  SetResult_UINT32(stack, (unsigned int)hndl);
  
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::GetNextSibling___STATIC__U4__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  handle_t hndl;
  hr = get_next_sibling((handle_t)param0, &hndl);

  NANOCLR_CHECK_HRESULT(hr);
  SetResult_UINT32(stack, (unsigned int)hndl);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::GetPreviousSibling___STATIC__U4__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  handle_t hndl;
  hr = get_previous_sibling((handle_t)param0, &hndl);

  NANOCLR_CHECK_HRESULT(hr);
  SetResult_UINT32(stack, (unsigned int)hndl);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::InsertBefore___STATIC__VOID__U4__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  unsigned int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  hr = insert_before((handle_t)param0, (handle_t) param1);

  NANOCLR_CHECK_HRESULT(hr);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::InsertAfter___STATIC__VOID__U4__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  unsigned int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  hr = insert_after((handle_t)param0, (handle_t)param1);

  NANOCLR_CHECK_HRESULT(hr);
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::GetZOrder___STATIC__U1__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  uint8_t retValue;
  hr = get_z_order((handle_t)param0, &retValue);
  NANOCLR_CHECK_HRESULT(hr);
  SetResult_UINT8(stack, retValue);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::SetZOrder___STATIC__VOID__U4__U1(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  uint8_t param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT8(stack, 1, param1));

  hr = set_z_order((handle_t)param0, param1);
  NANOCLR_CHECK_HRESULT(hr);

  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::CanvasClose___STATIC__VOID__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  hr = canvas_close((handle_t)param0);
  NANOCLR_CHECK_HRESULT(hr);

  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::CreateRectCanvas___STATIC__U4__I4__I4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  signed int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 0, param0));

  signed int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 1, param1));

  extent_t size;
  size.dx = param0;
  size.dy = param1;

  handle_t hndl;
  hr = create_rect_canvas(&size, &hndl);

    NANOCLR_CHECK_HRESULT(hr);
  SetResult_UINT32(stack, (unsigned int)hndl);
  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::CreatePngCanvas___STATIC__U4__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  handle_t hndl;
  hr = create_png_canvas((handle_t)param0, &hndl);

  NANOCLR_CHECK_HRESULT(hr);
  SetResult_UINT32(stack, (unsigned int)hndl);
  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::LoadPng___STATIC__VOID__U4__U4__I4__I4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  unsigned int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 1, param1));

  signed int param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 2, param2));

  signed int param3;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 3, param3));

  point_t pt;
  pt.x = param2;
  pt.y = param3;

  hr = load_png((handle_t)param0, (handle_t)param1, &pt);
  NANOCLR_CHECK_HRESULT(hr);

  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::GetCanvasExtents___STATIC__VOID__U4__BYREF_U2__BYREF_I4__BYREF_I4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  extent_t ex;
  uint16_t bpp;
  hr = get_canvas_extents((handle_t)param0, &ex, &bpp);
  NANOCLR_CHECK_HRESULT(hr);

  stack.Arg1().NumericByRef().s2 = bpp;
  stack.Arg2().NumericByRef().s4 = ex.dx;
  stack.Arg3().NumericByRef().s4 = ex.dy;
  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::GetOrientation___STATIC__U2__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  uint16_t orientation;
  hr = get_orientation((handle_t)param0, &orientation);
  NANOCLR_CHECK_HRESULT(hr);
  SetResult_UINT16(stack, orientation);
  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::SetOrientation___STATIC__VOID__U4__U2(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  uint16_t param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 1, param1));

  hr = set_orientation((handle_t)param0, param1);
  NANOCLR_CHECK_HRESULT(hr);

  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::CreatePen___STATIC__U4__U4__U2__U2(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  uint16_t param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 1, param1));

  uint16_t param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 2, param2));

  handle_t pen;
  hr = pen_create(param0, param1, (pen_style) param2, &pen);

  NANOCLR_CHECK_HRESULT(hr);
  SetResult_UINT32(stack, (unsigned int)pen);
  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::DisposePen___STATIC__VOID__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  hr = pen_release((handle_t)param0);
  NANOCLR_CHECK_HRESULT(hr);

  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::Polyline___STATIC__VOID__U4__I4__I4__I4__I4__U4__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  signed int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 1, param1));

  signed int param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 2, param2));

  signed int param3;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 3, param3));

  signed int param4;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 4, param4));

  unsigned int param5;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 5, param5));

  unsigned int param6;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 6, param6));

  // param0 is a canvas handle
  handle_t canvas = (handle_t)param0;

  rect_t clip_rect;
  clip_rect.left = param1;
  clip_rect.top = param2;
  clip_rect.right = param3;
  clip_rect.bottom = param4;

  // param5 is a handle to a pen
  handle_t pen = (handle_t)param5;

  // param6 is a handle to a point array
  vector_p hndl = (vector_p)param6;

  const point_t *pts;
  hr = vector_begin(hndl, (void **) &pts);
  NANOCLR_CHECK_HRESULT(hr);
  uint16_t count;
  hr = vector_count(hndl, &count);

  hr = polyline(canvas, &clip_rect, pen, count, pts);
  NANOCLR_CHECK_HRESULT(hr);

  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::Ellipse___STATIC__VOID__U4__I4__I4__I4__I4__U4__U4__I4__I4__I4__I4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  signed int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 1, param1));

  signed int param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 2, param2));

  signed int param3;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 3, param3));

  signed int param4;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 4, param4));

  unsigned int param5;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 5, param5));

  unsigned int param6;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 6, param6));

  signed int param7;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 7, param7));

  signed int param8;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 8, param8));

  signed int param9;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 9, param9));

  signed int param10;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 10, param10));


  // param0 is a canvas handle
  handle_t canvas = (handle_t)param0;

  rect_t clip_rect;
  clip_rect.left = param1;
  clip_rect.top = param2;
  clip_rect.right = param3;
  clip_rect.bottom = param4;

  handle_t pen = (handle_t)param5;
  color_t color = (color_t)param6;

  rect_t rect;
  rect.left = param7;
  rect.top = param8;
  rect.right = param9;
  rect.bottom = param10;

  hr = ellipse(canvas, &clip_rect, pen, color, &rect);
  NANOCLR_CHECK_HRESULT(hr);

  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::Polygon___STATIC__VOID__U4__I4__I4__I4__I4__U4__U4__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  signed int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 1, param1));

  signed int param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 2, param2));

  signed int param3;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 3, param3));

  signed int param4;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 4, param4));

  unsigned int param5;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 5, param5));

  unsigned int param6;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 6, param6));

  unsigned int param7;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 7, param7));

  // param0 is a canvas handle
  handle_t canvas = (handle_t)param0;

  rect_t clip_rect;
  clip_rect.left = param1;
  clip_rect.top = param2;
  clip_rect.right = param3;
  clip_rect.bottom = param4;

  // param5 is a handle to a pen
  handle_t pen = (handle_t)param5;

  color_t color = (color_t)param6;

  // param6 is a handle to a point array
  vector_p hndl = (vector_p)param7;

  const point_t *pts;
  hr = vector_begin(hndl, (void **)&pts);
  NANOCLR_CHECK_HRESULT(hr);
  uint16_t count;
  hr = vector_count(hndl, &count);

  hr = polygon(canvas, &clip_rect, pen, color, count, pts);

  NANOCLR_CHECK_HRESULT(hr);

  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::Rectangle___STATIC__VOID__U4__I4__I4__I4__I4__U4__U4__I4__I4__I4__I4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  signed int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 1, param1));

  signed int param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 2, param2));

  signed int param3;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 3, param3));

  signed int param4;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 4, param4));

  unsigned int param5;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 5, param5));

  unsigned int param6;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 6, param6));

  signed int param7;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 7, param7));

  signed int param8;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 8, param8));

  signed int param9;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 9, param9));

  signed int param10;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 10, param10));

  // param0 is a canvas handle
  handle_t canvas = (handle_t)param0;

  rect_t clip_rect;
  clip_rect.left = param1;
  clip_rect.top = param2;
  clip_rect.right = param3;
  clip_rect.bottom = param4;

  handle_t pen = (handle_t)param5;
  color_t color = (color_t)param6;

  rect_t rect;
  rect.left = param7;
  rect.top = param8;
  rect.right = param9;
  rect.bottom = param10;

  hr = rectangle(canvas, &clip_rect, pen, color, &rect);
  NANOCLR_CHECK_HRESULT(hr);

  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::RoundRect___STATIC__VOID__U4__I4__I4__I4__I4__U4__U4__I4__I4__I4__I4__I4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  signed int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 1, param1));

  signed int param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 2, param2));

  signed int param3;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 3, param3));

  signed int param4;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 4, param4));

  unsigned int param5;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 5, param5));

  unsigned int param6;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 6, param6));

  signed int param7;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 7, param7));

  signed int param8;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 8, param8));

  signed int param9;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 9, param9));

  signed int param10;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 10, param10));

  signed int param11;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 11, param11));

  // param0 is a canvas handle
  handle_t canvas = (handle_t)param0;

  rect_t clip_rect;
  clip_rect.left = param1;
  clip_rect.top = param2;
  clip_rect.right = param3;
  clip_rect.bottom = param4;

  handle_t pen = (handle_t)param5;
  color_t color = (color_t)param6;

  rect_t rect;
  rect.left = param7;
  rect.top = param8;
  rect.right = param9;
  rect.bottom = param10;

  hr = round_rect(canvas, &clip_rect, pen, color, &rect, param11);
  NANOCLR_CHECK_HRESULT(hr);

  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::BitBlt___STATIC__VOID__U4__I4__I4__I4__I4__I4__I4__I4__I4__U4__I4__I4__I4__I4__I4__I4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  signed int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 1, param1));

  signed int param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 2, param2));

  signed int param3;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 3, param3));

  signed int param4;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 4, param4));

  signed int param5;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 5, param5));

  signed int param6;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 6, param6));

  signed int param7;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 7, param7));

  signed int param8;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 8, param8));

  unsigned int param9;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 9, param9));

  signed int param10;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 10, param10));

  signed int param11;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 11, param11));

  signed int param12;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 12, param12));

  signed int param13;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 13, param13));

  signed int param14;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 14, param14));

  signed int param15;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 15, param15));
  // param0 is a canvas handle
  handle_t canvas = (handle_t)param0;

  rect_t clip_rect;
  clip_rect.left = param1;
  clip_rect.top = param2;
  clip_rect.right = param3;
  clip_rect.bottom = param4;

  rect_t dest_rect;
  dest_rect.left = param5;
  dest_rect.top = param6;
  dest_rect.right = param7;
  dest_rect.bottom = param8;

  handle_t src_canvas = (handle_t)param9;

  rect_t src_clip_rect;
  src_clip_rect.left = param10;
  src_clip_rect.top = param11;
  src_clip_rect.right = param12;
  src_clip_rect.bottom = param13;

  point_t src_pt;
  src_pt.x = param14;
  src_pt.y = param15;

  hr = bit_blt(canvas, &clip_rect, &dest_rect, src_canvas, &src_clip_rect, &src_pt);
  NANOCLR_CHECK_HRESULT(hr);

  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::GetPixel___STATIC__U4__U4__I4__I4__I4__I4__I4__I4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  signed int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 1, param1));

  signed int param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 2, param2));

  signed int param3;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 3, param3));

  signed int param4;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 4, param4));

  signed int param5;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 5, param5));

  signed int param6;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 6, param6));

  // param0 is a canvas handle
  handle_t canvas = (handle_t)param0;

  rect_t clip_rect;
  clip_rect.left = param1;
  clip_rect.top = param2;
  clip_rect.right = param3;
  clip_rect.bottom = param4;

  point_t pt;
  pt.x = param5;
  pt.y = param6;

  unsigned int retValue;
  hr = get_pixel(canvas, &clip_rect, &pt, (color_t *)&retValue);
  NANOCLR_CHECK_HRESULT(hr);
  SetResult_UINT32(stack, retValue);
  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::SetPixel___STATIC__U4__U4__I4__I4__I4__I4__I4__I4__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  signed int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 1, param1));

  signed int param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 2, param2));

  signed int param3;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 3, param3));

  signed int param4;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 4, param4));

  signed int param5;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 5, param5));

  signed int param6;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 6, param6));

  unsigned int param7;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 7, param7));

  // param0 is a canvas handle
  handle_t canvas = (handle_t)param0;

  rect_t clip_rect;
  clip_rect.left = param1;
  clip_rect.top = param2;
  clip_rect.right = param3;
  clip_rect.bottom = param4;

  point_t pt;
  pt.x = param5;
  pt.y = param6;

  color_t color = param7;

  unsigned int retValue;
  hr = set_pixel(canvas, &clip_rect, &pt, color, (color_t *)&retValue);
  NANOCLR_CHECK_HRESULT(hr);
  SetResult_UINT32(stack, retValue);
  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::Arc___STATIC__VOID__U4__I4__I4__I4__I4__U4__I4__I4__I4__I4__I4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  signed int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 1, param1));

  signed int param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 2, param2));

  signed int param3;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 3, param3));

  signed int param4;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 4, param4));

  unsigned int param5;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 5, param5));

  signed int param6;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 6, param6));

  signed int param7;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 7, param7));

  signed int param8;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 8, param8));

  signed int param9;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 9, param9));

  signed int param10;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 10, param10));
  handle_t canvas = (handle_t)param0;

  rect_t clip_rect;
  clip_rect.left = param1;
  clip_rect.top = param2;
  clip_rect.right = param3;
  clip_rect.bottom = param4;

  handle_t pen = (handle_t)param5;

  point_t pt;
  pt.x = param6;
  pt.y = param7;

  hr = arc(canvas, &clip_rect, pen, &pt, param8, param9, param10);
  NANOCLR_CHECK_HRESULT(hr);

  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::Pie___STATIC__VOID__U4__I4__I4__I4__I4__U4__U4__I4__I4__I4__I4__I4__I4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  signed int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 1, param1));

  signed int param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 2, param2));

  signed int param3;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 3, param3));

  signed int param4;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 4, param4));

  unsigned int param5;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 5, param5));

  unsigned int param6;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 6, param6));

  signed int param7;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 7, param7));

  signed int param8;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 8, param8));

  signed int param9;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 9, param9));

  signed int param10;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 10, param10));

  signed int param11;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 11, param11));

  signed int param12;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 12, param12));
  handle_t canvas = (handle_t)param0;

  rect_t clip_rect;
  clip_rect.left = param1;
  clip_rect.top = param2;
  clip_rect.right = param3;
  clip_rect.bottom = param4;

  handle_t pen = (handle_t)param5;

  color_t color = (color_t)param6;

  point_t pt;
  pt.x = param7;
  pt.y = param8;

  hr = pie(canvas, &clip_rect, pen, color, &pt, param9, param10, param11, param12);
  NANOCLR_CHECK_HRESULT(hr);

  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::OpenFont___STATIC__U4__STRING__U2(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  const char* param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 0, param0));

  uint16_t param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 1, param1));

  unsigned int retValue;
  hr = open_font(param0, param1, (handle_t *)&retValue);
  NANOCLR_CHECK_HRESULT(hr);
  SetResult_UINT32(stack, retValue);
  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::DrawText___STATIC__VOID__U4__I4__I4__I4__I4__U4__U4__U4__STRING__I4__I4__I4__I4__I4__I4__U2(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  signed int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 1, param1));

  signed int param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 2, param2));

  signed int param3;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 3, param3));

  signed int param4;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 4, param4));

  unsigned int param5;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 5, param5));

  unsigned int param6;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 6, param6));

  unsigned int param7;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 7, param7));

  const char* param8;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 8, param8));

  signed int param9;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 9, param9));

  signed int param10;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 10, param10));

  signed int param11;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 11, param11));

  signed int param12;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 12, param12));

  signed int param13;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 13, param13));

  signed int param14;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 14, param14));

  unsigned short int param15;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 15, param15));

  handle_t canvas = (handle_t)param0;

  rect_t clip_rect;
  clip_rect.left = param1;
  clip_rect.top = param2;
  clip_rect.right = param3;
  clip_rect.bottom = param4;

  handle_t font = (handle_t)param5;

  color_t fg = (color_t)param6;
  color_t bg = (color_t)param7;

  point_t pt;
  pt.x = param9;
  pt.y = param10;

  rect_t txt_clip_rect;
  txt_clip_rect.left = param11;
  txt_clip_rect.top = param12;
  txt_clip_rect.right = param13;
  txt_clip_rect.bottom = param14;

  hr = draw_text(canvas, &clip_rect, font, fg, bg, param8, 0, &pt, &txt_clip_rect, param15, 0);

  NANOCLR_CHECK_HRESULT(hr);

  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::TextExtent___STATIC__VOID__U4__U4__STRING__BYREF_I4__BYREF_I4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  unsigned int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 1, param1));

  const char* param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 2, param2));

  handle_t canvas = (handle_t)param0;

  handle_t font = (handle_t)param1;
  
  extent_t ex;

  hr = text_extent(canvas, font, param2, 0, &ex);
  NANOCLR_CHECK_HRESULT(hr);

  stack.Arg3().NumericByRef().s4 = ex.dx;
  stack.Arg4().NumericByRef().s4 = ex.dy;

  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::InvalidateRect___STATIC__VOID__U4__I4__I4__I4__I4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  signed int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 1, param1));

  signed int param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 2, param2));

  signed int param3;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 3, param3));

  signed int param4;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 4, param4));
  handle_t hwnd = (handle_t)param0;

  rect_t rect;
  rect.left = param1;
  rect.top = param2;
  rect.right = param3;
  rect.bottom = param4;

  hr = invalidate_rect(hwnd, &rect);
  NANOCLR_CHECK_HRESULT(hr);

  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::IsValid___STATIC__BOOLEAN__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  handle_t hwnd = (handle_t)param0;

  bool retValue = is_invalid(hwnd) == s_ok;
  
  SetResult_bool(stack, retValue);
  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::BeginPaint___STATIC__VOID__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  handle_t hwnd = (handle_t)param0;

  hr = begin_paint(hwnd);
  NANOCLR_CHECK_HRESULT(hr);

  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Photon::EndPaint___STATIC__VOID__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  handle_t hwnd = (handle_t)param0;

  hr = end_paint(hwnd);
  NANOCLR_CHECK_HRESULT(hr);

  }
  NANOCLR_NOCLEANUP();
  }
