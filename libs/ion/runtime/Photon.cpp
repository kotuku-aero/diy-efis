#include "../nano/CLR/CorLib/mscorlib/corlib_native.h"
#include "../../neutron/neutron.h"

#ifdef RGB
#undef RGB
#endif

#ifdef HWND_BROADCAST
#undef HWND_BROADCAST
#endif

#include "../../photon/photon.h"
#include "../../photon/window.h"

static semaphore_p screen_init = 0;

struct canfly_wnddata_t {
  CLR_RT_HeapBlock *obj;
  CLR_RT_HeapBlock_Delegate *wndproc;
  };

HRESULT CreatePhotonInstance(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  semaphore_create(&screen_init);

  // create the window proc, this will be released when the scren is opened

  return S_OK;
  }

static handle_t screen;

result_t RunPhoton()
  {
  // initialize photon

  // wait till photon is ready
  semaphore_wait(screen_init, INDEFINITE_WAIT);
  canmsg_t msg;
  result_t result;
  handle_t hwnd = 0;
  while (succeeded(result = get_message(screen, &hwnd, &msg)))
    dispatch_message(hwnd, &msg);

  return result;
  }

HRESULT Library_corlib_native_CanFly_Syscall::OpenScreen___STATIC__I4__U2__U2__BYREF_U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint16_t param0;
  uint16_t param1;
  if (FAILED(hr = Interop_Marshal_UINT16(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_UINT16(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg2().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  handle_t hwnd;
  hr = open_screen(param0, defwndproc, param1, &hwnd);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg2().Dereference()->SetInteger((uint32_t)hwnd);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::CreateWindow___STATIC__I4__U4__I4__I4__I4__I4__U2__BYREF_U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  int32_t param1;
  int32_t param2;
  int32_t param3;
  int32_t param4;
  uint16_t param5;

  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 1, param1)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 2, param2)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 3, param3)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 4, param4)) ||
    FAILED(hr = Interop_Marshal_UINT16(stack, 5, param5)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg6().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  rect_t bounds;
  bounds.left = param1;
  bounds.top = param2;
  bounds.right = param3;
  bounds.bottom = param4;
  handle_t hwnd;

  hr = create_window((handle_t)param0, &bounds, defwndproc, param5, &hwnd);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg6().Dereference()->SetInteger((uint32_t)hwnd);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::CreateChildWindow___STATIC__I4__U4__I4__I4__I4__I4__U2__BYREF_U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;
  uint32_t param0;
  int32_t param1;
  int32_t param2;
  int32_t param3;
  int32_t param4;
  uint16_t param5;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 1, param1)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 2, param2)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 3, param3)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 4, param4)) ||
    FAILED(hr = Interop_Marshal_UINT16(stack, 5, param5)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg6().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  rect_t bounds;
  bounds.left = param1;
  bounds.top = param2;
  bounds.right = param3;
  bounds.bottom = param4;

  handle_t hwnd;

  hr = create_child_window((handle_t)param0, &bounds, defwndproc, param5, &hwnd);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg6().Dereference()->SetInteger((uint32_t)hwnd);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::CloseWindow___STATIC__I4__U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  handle_t hwnd = (handle_t)param0;
  void *wnddata;
  hr = get_wnddata(hwnd, &wnddata);

  neutron_free(wnddata);

  stack.SetResult_I4(close_window(hwnd));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetWindowRect___STATIC__I4__U4__BYREF_I4__BYREF_I4__BYREF_I4__BYREF_I4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg1().Dereference() == 0 ||
    stack.Arg2().Dereference() == 0 ||
    stack.Arg3().Dereference() == 0 ||
    stack.Arg4().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  rect_t wnd_rect;
  hr = get_window_rect((handle_t)param0, &wnd_rect);

  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    {
    stack.Arg1().Dereference()->SetInteger(wnd_rect.left);
    stack.Arg2().Dereference()->SetInteger(wnd_rect.top);
    stack.Arg3().Dereference()->SetInteger(wnd_rect.right);
    stack.Arg4().Dereference()->SetInteger(wnd_rect.bottom);
    }

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetWindowPos___STATIC__I4__U4__BYREF_I4__BYREF_I4__BYREF_I4__BYREF_I4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg1().Dereference() == 0 ||
    stack.Arg2().Dereference() == 0 ||
    stack.Arg3().Dereference() == 0 ||
    stack.Arg4().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  rect_t pos;
  hr = get_window_pos((handle_t)param0, &pos);

  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    {
    stack.Arg1().Dereference()->SetInteger(pos.left);
    stack.Arg2().Dereference()->SetInteger(pos.top);
    stack.Arg3().Dereference()->SetInteger(pos.right);
    stack.Arg4().Dereference()->SetInteger(pos.bottom);
    }

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::SetWindowPos___STATIC__I4__U4__I4__I4__I4__I4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  int32_t param1;
  int32_t param2;
  int32_t param3;
  int32_t param4;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 1, param1)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 2, param2)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 3, param3)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 4, param4)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  rect_t rect;
  rect.left = param1;
  rect.top = param2;
  rect.right = param3;
  rect.bottom = param4;

  stack.SetResult_I4(set_window_pos((handle_t)param0, &rect));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetWindowData___STATIC__I4__U4__BYREF_OBJECT(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg1().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  canfly_wnddata_t *wnd_data;
  hr = get_wnddata((handle_t)param0, (void **)&wnd_data);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg1().Dereference()->SetObjectReference(wnd_data->obj);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::SetWindowData___STATIC__I4__U4__OBJECT__CanFlyCanFlyEventHandler(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg1().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  CLR_RT_HeapBlock *obj = stack.Arg1().Dereference();
  CLR_RT_HeapBlock_Delegate *del = stack.Arg2().DereferenceDelegate();

  canfly_wnddata_t *wnddata = (canfly_wnddata_t *)neutron_malloc(sizeof(canfly_wnddata_t));
  wnddata->obj = obj;
  wnddata->wndproc = del;

  stack.SetResult_I4(set_wnddata((handle_t)param0, wnddata));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetParent___STATIC__I4__U4__BYREF_U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg1().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  handle_t hwnd;
  hr = get_parent((handle_t)param0, &hwnd);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg1().Dereference()->SetInteger((uint32_t)hwnd);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetWindowById___STATIC__I4__U4__U2__BYREF_U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;
  uint32_t param0;
  uint16_t param1;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_UINT16(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg2().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  handle_t hndl;
  hr = get_window_by_id((handle_t)param0, param1, &hndl);

  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg2().Dereference()->SetInteger((uint32_t)hndl);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetFirstChild___STATIC__I4__U4__BYREF_U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg1().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  handle_t hndl;
  hr = get_first_child((handle_t)param0, &hndl);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg1().Dereference()->SetInteger((uint32_t)hndl);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetNextSibling___STATIC__I4__U4__BYREF_U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg1().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  handle_t hndl;
  hr = get_next_sibling((handle_t)param0, &hndl);

  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg1().Dereference()->SetInteger((uint32_t)hndl);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetPreviousSibling___STATIC__I4__U4__BYREF_U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg1().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  handle_t hndl;
  hr = get_previous_sibling((handle_t)param0, &hndl);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg1().Dereference()->SetInteger((uint32_t)hndl);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::InsertBefore___STATIC__I4__U4__U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  uint32_t param1;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  stack.SetResult_I4(insert_before((handle_t)param0, (handle_t)param1));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::InsertAfter___STATIC__I4__U4__U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  uint32_t param1;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  stack.SetResult_I4(insert_after((handle_t)param0, (handle_t)param1));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetZOrder___STATIC__I4__U4__BYREF_U1(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg1().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  uint8_t retValue;
  hr = get_z_order((handle_t)param0, &retValue);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg1().Dereference()->SetInteger(retValue);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::SetZOrder___STATIC__I4__U4__U1(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  uint8_t param1;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_UINT8(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  stack.SetResult_I4(set_z_order((handle_t)param0, param1));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::CanvasClose___STATIC__I4__U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  stack.SetResult_I4(canvas_close((handle_t)param0));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::CreateRectCanvas___STATIC__I4__I4__I4__BYREF_U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  int32_t param0;
  int32_t param1;
  if (FAILED(hr = Interop_Marshal_INT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg2().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  extent_t size;
  size.dx = param0;
  size.dy = param1;

  handle_t hndl;
  hr = create_rect_canvas(&size, &hndl);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg2().Dereference()->SetInteger((uint32_t)hndl);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::CreatePngCanvas___STATIC__I4__U4__BYREF_U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg1().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  handle_t hndl;
  hr = create_png_canvas((handle_t)param0, &hndl);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg1().Dereference()->SetInteger((uint32_t)hndl);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::LoadPng___STATIC__I4__U4__U4__I4__I4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;
  {

  uint32_t param0;
  uint32_t param1;
  int32_t param2;
  int32_t param3;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_UINT32(stack, 1, param1)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 2, param2)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 3, param3)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  point_t pt;
  pt.x = param2;
  pt.y = param3;

  hr = load_png((handle_t)param0, (handle_t)param1, &pt);


  }
  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetCanvasExtents___STATIC__I4__U4__BYREF_U2__BYREF_I4__BYREF_I4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg1().Dereference() == 0 ||
    stack.Arg2().Dereference() == 0 ||
    stack.Arg3().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  extent_t ex;
  uint16_t bpp;
  hr = get_canvas_extents((handle_t)param0, &ex, &bpp);

  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    {
    stack.Arg1().Dereference()->SetInteger(bpp);
    stack.Arg2().Dereference()->SetInteger(ex.dx);
    stack.Arg3().Dereference()->SetInteger(ex.dy);
    }

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetOrientation___STATIC__I4__U4__BYREF_U2(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg1().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  uint16_t orientation;
  hr = get_orientation((handle_t)param0, &orientation);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg1().Dereference()->SetInteger(orientation);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::SetOrientation___STATIC__I4__U4__U2(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  uint16_t param1;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_UINT16(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  stack.SetResult_I4(set_orientation((handle_t)param0, param1));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::CreatePen___STATIC__I4__U4__U2__U2__BYREF_U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  uint16_t param1;
  uint16_t param2;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_UINT16(stack, 1, param1)) ||
    FAILED(hr = Interop_Marshal_UINT16(stack, 2, param2)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg3().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  handle_t pen;
  hr = pen_create(param0, param1, (pen_style)param2, &pen);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg3().Dereference()->SetInteger((uint32_t)pen);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetPenColor___STATIC__I4__U4__BYREF_U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg1().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  uint32_t retValue;
  hr = get_pen_color((handle_t)param0, (color_t *)&retValue);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg1().Dereference()->SetInteger(retValue);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetPenWidth___STATIC__I4__U4__BYREF_U2(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg1().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  uint16_t retValue;
  hr = get_pen_width((handle_t)param0, &retValue);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg1().Dereference()->SetInteger(retValue);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetPenStyle___STATIC__I4__U4__BYREF_U2(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg1().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  uint16_t retValue;
  hr = get_pen_style((handle_t)param0, (pen_style *)&retValue);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg1().Dereference()->SetInteger(retValue);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::DisposePen___STATIC__I4__U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  stack.SetResult_I4(pen_release((handle_t)param0));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::Polyline___STATIC__I4__U4__I4__I4__I4__I4__U4__U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  int32_t param1;
  int32_t param2;
  int32_t param3;
  int32_t param4;
  uint32_t param5;
  uint32_t param6;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 1, param1)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 2, param2)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 3, param3)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 4, param4)) ||
    FAILED(hr = Interop_Marshal_UINT32(stack, 5, param5)) ||
    FAILED(hr = Interop_Marshal_UINT32(stack, 6, param6)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

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
  uint16_t count;
  if (failed(hr = vector_begin(hndl, (void **)&pts)) ||
    failed(hr = vector_count(hndl, &count)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  stack.SetResult_I4(polyline(canvas, &clip_rect, pen, count, pts));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::Ellipse___STATIC__I4__U4__I4__I4__I4__I4__U4__U4__I4__I4__I4__I4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  int32_t param1;
  int32_t param2;
  int32_t param3;
  int32_t param4;
  uint32_t param5;
  uint32_t param6;
  int32_t param7;
  int32_t param8;
  int32_t param9;
  int32_t param10;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 1, param1)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 2, param2)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 3, param3)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 4, param4)) ||
    FAILED(hr = Interop_Marshal_UINT32(stack, 5, param5)) ||
    FAILED(hr = Interop_Marshal_UINT32(stack, 6, param6)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 7, param7)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 8, param8)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 9, param9)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 10, param10)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }


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

  stack.SetResult_I4(ellipse(canvas, &clip_rect, pen, color, &rect));


  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::Polygon___STATIC__I4__U4__I4__I4__I4__I4__U4__U4__U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  int32_t param1;
  int32_t param2;
  int32_t param3;
  int32_t param4;
  uint32_t param5;
  uint32_t param6;
  uint32_t param7;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 1, param1)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 2, param2)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 3, param3)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 4, param4)) ||
    FAILED(hr = Interop_Marshal_UINT32(stack, 5, param5)) ||
    FAILED(hr = Interop_Marshal_UINT32(stack, 6, param6)) ||
    FAILED(hr = Interop_Marshal_UINT32(stack, 7, param7)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

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
  uint16_t count;
  if (failed(hr = vector_begin(hndl, (void **)&pts)) ||
    failed(hr = vector_count(hndl, &count)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  stack.SetResult_I4(polygon(canvas, &clip_rect, pen, color, count, pts));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::Rectangle___STATIC__I4__U4__I4__I4__I4__I4__U4__U4__I4__I4__I4__I4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  int32_t param1;
  int32_t param2;
  int32_t param3;
  int32_t param4;
  uint32_t param5;
  uint32_t param6;
  int32_t param7;
  int32_t param8;
  int32_t param9;
  int32_t param10;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 1, param1)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 2, param2)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 3, param3)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 4, param4)) ||
    FAILED(hr = Interop_Marshal_UINT32(stack, 5, param5)) ||
    FAILED(hr = Interop_Marshal_UINT32(stack, 6, param6)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 7, param7)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 8, param8)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 9, param9)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 10, param10)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

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

  stack.SetResult_I4(rectangle(canvas, &clip_rect, pen, color, &rect));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::RoundRect___STATIC__I4__U4__I4__I4__I4__I4__U4__U4__I4__I4__I4__I4__I4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  int32_t param1;
  int32_t param2;
  int32_t param3;
  int32_t param4;
  uint32_t param5;
  uint32_t param6;
  int32_t param7;
  int32_t param8;
  int32_t param9;
  int32_t param10;
  int32_t param11;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 1, param1)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 2, param2)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 3, param3)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 4, param4)) ||
    FAILED(hr = Interop_Marshal_UINT32(stack, 5, param5)) ||
    FAILED(hr = Interop_Marshal_UINT32(stack, 6, param6)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 7, param7)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 8, param8)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 9, param9)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 10, param10)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 11, param11)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

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

  stack.SetResult_I4(round_rect(canvas, &clip_rect, pen, color, &rect, param11));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::BitBlt___STATIC__I4__U4__I4__I4__I4__I4__I4__I4__I4__I4__U4__I4__I4__I4__I4__I4__I4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  int32_t param1;
  int32_t param2;
  int32_t param3;
  int32_t param4;
  int32_t param5;
  int32_t param6;
  int32_t param7;
  int32_t param8;
  uint32_t param9;
  int32_t param10;
  int32_t param11;
  int32_t param12;
  int32_t param13;
  int32_t param14;
  int32_t param15;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 1, param1)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 2, param2)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 3, param3)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 4, param4)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 5, param5)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 6, param6)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 7, param7)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 8, param8)) ||
    FAILED(hr = Interop_Marshal_UINT32(stack, 9, param9)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 10, param10)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 11, param11)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 12, param12)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 13, param13)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 14, param14)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 15, param15)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

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

  stack.SetResult_I4(bit_blt(canvas, &clip_rect, &dest_rect, src_canvas, &src_clip_rect, &src_pt));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetPixel___STATIC__I4__U4__I4__I4__I4__I4__I4__I4__BYREF_U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  int32_t param1;
  int32_t param2;
  int32_t param3;
  int32_t param4;
  int32_t param5;
  int32_t param6;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 1, param1)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 2, param2)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 3, param3)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 4, param4)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 5, param5)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 6, param6)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg7().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

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

  uint32_t retValue;
  hr = get_pixel(canvas, &clip_rect, &pt, (color_t *)&retValue);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg7().Dereference()->SetInteger(retValue);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::SetPixel___STATIC__I4__U4__I4__I4__I4__I4__I4__I4__U4__BYREF_U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  int32_t param1;
  int32_t param2;
  int32_t param3;
  int32_t param4;
  int32_t param5;
  int32_t param6;
  uint32_t param7;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 1, param1)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 2, param2)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 3, param3)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 4, param4)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 5, param5)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 6, param6)) ||
    FAILED(hr = Interop_Marshal_UINT32(stack, 7, param7)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

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

  uint32_t retValue;
  stack.SetResult_I4(set_pixel(canvas, &clip_rect, &pt, color, (color_t *)&retValue));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::Arc___STATIC__I4__U4__I4__I4__I4__I4__U4__I4__I4__I4__I4__I4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  int32_t param1;
  int32_t param2;
  int32_t param3;
  int32_t param4;
  uint32_t param5;
  int32_t param6;
  int32_t param7;
  int32_t param8;
  int32_t param9;
  int32_t param10;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 1, param1)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 2, param2)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 3, param3)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 4, param4)) ||
    FAILED(hr = Interop_Marshal_UINT32(stack, 5, param5)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 6, param6)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 7, param7)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 8, param8)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 9, param9)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 10, param10)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

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

  SetResult_INT32(stack, arc(canvas, &clip_rect, pen, &pt, param8, param9, param10));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::Pie___STATIC__I4__U4__I4__I4__I4__I4__U4__U4__I4__I4__I4__I4__I4__I4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  int32_t param1;
  int32_t param2;
  int32_t param3;
  int32_t param4;
  uint32_t param5;
  uint32_t param6;
  int32_t param7;
  int32_t param8;
  int32_t param9;
  int32_t param10;
  int32_t param11;
  int32_t param12;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 1, param1)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 2, param2)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 3, param3)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 4, param4)) ||
    FAILED(hr = Interop_Marshal_UINT32(stack, 5, param5)) ||
    FAILED(hr = Interop_Marshal_UINT32(stack, 6, param6)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 7, param7)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 8, param8)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 9, param9)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 10, param10)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 11, param11)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 12, param12)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

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

  SetResult_INT32(stack, pie(canvas, &clip_rect, pen, color, &pt, param9, param10, param11, param12));
  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::OpenFont___STATIC__I4__STRING__U2__BYREF_U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  const char *param0;
  uint16_t param1;
  if (FAILED(hr = Interop_Marshal_LPCSTR(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_UINT16(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg2().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  uint32_t retValue;
  hr = open_font(param0, param1, (handle_t *)&retValue);
  SetResult_INT32(stack, hr);
  if (succeeded(hr))
    stack.Arg2().Dereference()->SetInteger(retValue);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::DrawText___STATIC__I4__U4__I4__I4__I4__I4__U4__U4__U4__STRING__I4__I4__I4__I4__I4__I4__U2(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  int32_t param1;
  int32_t param2;
  int32_t param3;
  int32_t param4;
  uint32_t param5;
  uint32_t param6;
  uint32_t param7;
  const char *param8;
  int32_t param9;
  int32_t param10;
  int32_t param11;
  int32_t param12;
  int32_t param13;
  int32_t param14;
  unsigned short int param15;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 1, param1)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 2, param2)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 3, param3)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 4, param4)) ||
    FAILED(hr = Interop_Marshal_UINT32(stack, 5, param5)) ||
    FAILED(hr = Interop_Marshal_UINT32(stack, 6, param6)) ||
    FAILED(hr = Interop_Marshal_UINT32(stack, 7, param7)) ||
    FAILED(hr = Interop_Marshal_LPCSTR(stack, 8, param8)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 9, param9)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 10, param10)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 11, param11)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 12, param12)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 13, param13)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 14, param14)) ||
    FAILED(hr = Interop_Marshal_UINT16(stack, 15, param15)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

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

  SetResult_INT32(stack, draw_text(canvas, &clip_rect, font, fg, bg, param8, 0, &pt, &txt_clip_rect, param15, 0));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::TextExtent___STATIC__I4__U4__U4__STRING__BYREF_I4__BYREF_I4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  uint32_t param1;
  const char *param2;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_UINT32(stack, 1, param1)) ||
    FAILED(hr = Interop_Marshal_LPCSTR(stack, 2, param2)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  handle_t canvas = (handle_t)param0;

  handle_t font = (handle_t)param1;

  extent_t ex;

  hr = text_extent(canvas, font, param2, 0, &ex);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    {
    stack.Arg3().NumericByRef().s4 = ex.dx;
    stack.Arg4().NumericByRef().s4 = ex.dy;
    }

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::InvalidateRect___STATIC__I4__U4__I4__I4__I4__I4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  int32_t param1;
  int32_t param2;
  int32_t param3;
  int32_t param4;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 1, param1)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 2, param2)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 3, param3)) ||
    FAILED(hr = Interop_Marshal_INT32(stack, 4, param4)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  handle_t hwnd = (handle_t)param0;

  rect_t rect;
  rect.left = param1;
  rect.top = param2;
  rect.right = param3;
  rect.bottom = param4;

  SetResult_INT32(stack, invalidate_rect(hwnd, &rect));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::IsInvalid___STATIC__I4__U4__BYREF_BOOLEAN(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg1().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  handle_t hwnd = (handle_t)param0;
  bool retValue = is_invalid(hwnd) == s_ok;
  stack.Arg1().Dereference()->SetBoolean(retValue);
  SetResult_INT32(stack, s_ok);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::BeginPaint___STATIC__I4__U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  handle_t hwnd = (handle_t)param0;

  SetResult_INT32(stack, begin_paint(hwnd));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::EndPaint___STATIC__I4__U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  handle_t hwnd = (handle_t)param0;

  SetResult_INT32(stack, end_paint(hwnd));

  return S_OK;
  }

extern result_t queue_callback(CLR_RT_HeapBlock_Delegate *dlg, const canmsg_t *msg);

static result_t photon_event(handle_t hwnd, struct _event_proxy_t *proxy, const canmsg_t *msg)
  {
  result_t result;
  // find the hwnd wnd data
  canfly_wnddata_t *wndData;
  if (failed(result = get_wnddata(hwnd, (void **)&wndData)))
    return result;

  if (wndData->wndproc == 0)
    return s_false;

  return queue_callback(wndData->wndproc, msg);
  }

HRESULT Library_corlib_native_CanFly_Syscall::AddWidgetEvent___STATIC__I4__U4__U2(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;
  uint32_t param0;
  uint16_t param1;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_UINT16(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  CLR_RT_HeapBlock *widgetObject = stack.This();

  SetResult_INT32(stack, add_event((handle_t)param0, param1, widgetObject, photon_event));

  return S_OK;
  }
