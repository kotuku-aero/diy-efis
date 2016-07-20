#include "windows_screen.h"

kotuku::windows_screen_t::windows_screen_t(HWND wnd, size_t x, size_t y, size_t bpp)
: gdi_screen_t(GetDC(wnd), x, y, bpp),
  _owner_window(0)
  {
  _hwnd = wnd;
  }

kotuku::windows_screen_t::~windows_screen_t()
  {
  ReleaseDC(_hwnd, DC());
  }

inline RECT *as_rect(const kotuku::rect_t &r)
  {
  return reinterpret_cast<RECT *>(const_cast<kotuku::rect_t *>(&r));
  }

void kotuku::windows_screen_t::invalidate_rect(const rect_t &rect)
  {
  ::InvalidateRect(_hwnd, as_rect(rect), TRUE);
  }

void kotuku::windows_screen_t::update_window()
  {
  ::UpdateWindow(_hwnd);
  }

void kotuku::windows_screen_t::paint(HDC dc, const rect_t &rect)
  {
  if(_owner_window != 0)
    _owner_window->paint(true);
  }

int kotuku::windows_screen_t::z_order() const
  {
  return 0;
  }

void kotuku::windows_screen_t::z_order(int order)
  {
  SetWindowPos(_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  }
