#ifndef __windows_screen_h__
#define __windows_screen_h__

#include "gdi_screen.h"
namespace kotuku {
  class windows_screen_t : public gdi_screen_t {
  public:
    windows_screen_t(HWND wnd, size_t x, size_t y, size_t bpp);
    ~windows_screen_t();
    virtual void invalidate_rect(const rect_t &);
    virtual void update_window();

    void paint(HDC dc, const rect_t &rect);
    void update_background();
    window_t *owner() const { return _owner_window; }
    void owner(window_t *window) { _owner_window = window; }

    int z_order() const;
    void z_order(int);

    HWND handle() const { return _hwnd; }
  private:
    HWND _hwnd;
    window_t *_owner_window;

    bool _rotate;

  };

  // this class holds the handles
  inline windows_screen_t *as_screen_handle(screen_t *h)
  {
    return reinterpret_cast<windows_screen_t *>(h);
  }
}

#endif
