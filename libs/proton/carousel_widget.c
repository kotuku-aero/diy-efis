#include "carousel_widget.h"
#include "menu_window.h"


static result_t deselect_page(carousel_window_t* wnd, uint16_t page_num)
  {
  if (wnd->num_pages <= page_num || wnd->child_windows[page_num].hndl == 0)
    return e_bad_parameter;

  if (wnd->child_windows[page_num].hndl != 0)
    hide_window(wnd->child_windows[page_num].hndl);

  return s_ok;
  }

static result_t select_page(carousel_window_t* wnd, uint16_t page_num)
  {
  if (wnd->num_pages <= page_num)
    return e_bad_parameter;

  carousel_page_t* page = &wnd->child_windows[page_num];
  // see if needing construction
  if (page->hndl == 0)
    {
    result_t result;
    rect_t wnd_rect;
    if (failed(result = window_rect(wnd->hwnd, &wnd_rect)))
      return result;

    wnd_rect.right = rect_width(&wnd_rect);
    wnd_rect.bottom = rect_height(&wnd_rect);
    wnd_rect.top = 0;
    wnd_rect.left = 0;

    // try to create the child window
    if (failed(result = (*page->create_page)(wnd->parent, &wnd_rect, page_num, page->wnddata, &page->hndl)))
      return result;
    }

  return show_window(page->hndl);
  }

result_t carousel_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  carousel_window_t* wnd = (carousel_window_t*)wnddata;

  switch (get_can_id(msg))
    {
    //case id_paint_background:
    //  on_paint_background(hwnd, wnd, msg);
    //  break;
    //case id_paint_foreground:
    //  on_paint(hwnd, wnd, msg);
    //  break;
      //case id_paint_background :
      //  on_paint_background(hwnd, wnd, msg);
      //  break;
    case id_carousel_left:
      deselect_page(wnd, wnd->selected_page);

      if(wnd->selected_page == 0)
        wnd->selected_page = wnd->num_pages-1;

      select_page(wnd, wnd->selected_page);
      break;
    case id_carousel_right :
      deselect_page(wnd, wnd->selected_page);

      wnd->selected_page++;
      if(wnd->selected_page >= wnd->num_pages)
        wnd->selected_page = 0;

      select_page(wnd, wnd->selected_page);
      break;
    }

  // pass to default
  return defwndproc(hwnd, msg, wnddata);
  }


result_t create_carousel_window(handle_t parent, uint16_t id, aircraft_t* aircraft, carousel_window_t* wnd, handle_t* hndl)
  {
  result_t result;

  if (failed(result = create_widget(parent, id, carousel_wndproc, &wnd->base, hndl)))
    return result;

  // create the selected page
  if(wnd->selected_page >= wnd->num_pages)
    return e_bad_parameter;

  return select_page(wnd, wnd->selected_page);
  }
