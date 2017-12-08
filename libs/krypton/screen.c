#include <Windows.h>
#include "../neutron/neutron.h"
#include "../proton/proton.h"

#define BITS_PER_PIXEL 32
#define PIXELS_PER_LONG 1

#define CHUNKX 32
#define CHUNKY 20

uint32_t *crcs;

handle_t h_worker;

static HWND window;
static bool repaint = false;
static uint16_t dimx;
static uint16_t dimy;

extern const uint32_t *fb_buffer;

static LRESULT CALLBACK wndproc(HWND   hwnd, UINT   uMsg, WPARAM wParam, LPARAM lParam)
  {
  switch (uMsg)
    {
    case WM_PAINT:
    {
    HDC dc = GetDC(hwnd);
    /*
    Sample all chunks for changes in shared memory buffer and
    eventually repaint individual chunks. Repaint everything if
    repaint is true (see above)
    */
    for (int iy = 0; iy < dimy / CHUNKY; iy++)
      for (int ix = 0; ix < dimx / CHUNKX; ix++)
        {
        uint32_t crc;
        size_t off;

        // calculate the crc of the display area to check
        off = (ix * CHUNKX) + (iy * CHUNKY * dimx);
        crc = 0x8154711;

        for (int x = 0; x < CHUNKX; x++)
          for (int y = 0; y < CHUNKY; y++)
            {
            unsigned long dat;
            size_t pixel = off + x + (y * dimx);
            dat = fb_buffer[pixel];

            /*     crc^=((crc^dat)<<1)^((dat&0x8000) ? 0x1048:0);
            */
            crc += (crc % 211 + dat);
            /* crc=(crc<<1)+((crc&0x80000000) ? 1:0);  */
            }

        int offset = ix + (iy * (dimx / CHUNKX));

        if (crc != crcs[offset])
          {
          int dest_x = ix * CHUNKX;
          int dest_y = iy * CHUNKY;
          // not the same, or a forced redraw so copy the
          // pixels over
          for (int y = 0; y < CHUNKY; y++)
            for (int x = 0; x < CHUNKX; x++)
              {
              uint32_t color = fb_buffer[off + x + (y * dimx)];

              uint32_t c1 = color;
              color &= 0x0000FF00;
              color |= (c1 & 0xff) << 16;
              color |= (c1 & 0x00ff0000) >> 16;

              SetPixel(dc, dest_x + x, dest_y + y, color & 0x00FFFFFF);
              }


          crcs[offset] = crc;
          }
        }
      }
      break;
    case WM_TIMER:
      InvalidateRect(window, NULL, FALSE);
      break;
    }

  return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }

static const char *className = "msh-class";

static void worker(void *argv)
  {
  WNDCLASS wndclass;
  memset(&wndclass, 0, sizeof(WNDCLASS));
  wndclass.lpfnWndProc = wndproc;
  wndclass.lpszClassName = className;

  RegisterClass(&wndclass);

  int size = (dimx / CHUNKX) * (dimy / CHUNKY);

  char name[80];
  sprintf(name, "MSH: %d * %d", dimx, dimy);
  crcs = (uint32_t *)malloc(sizeof(uint32_t) * size);
  RECT rect;
  rect.left = 0;
  rect.top = 0;
  rect.right = dimx;
  rect.bottom = dimy;

  AdjustWindowRect(&rect, WS_CAPTION | WS_THICKFRAME | WS_VISIBLE, FALSE);

  rect.bottom += 0 - rect.top;
  rect.right += 0 - rect.left;
  rect.left = 0;
  rect.top = 0;

  window = CreateWindowEx(0, className, name, WS_CAPTION | WS_THICKFRAME | WS_VISIBLE, 
    rect.left, rect.top, rect.right, rect.bottom,
    NULL, NULL, NULL, NULL);

  ShowWindow(window, SW_SHOW);

  DWORD idEvent;
  SetTimer(window, &idEvent, 100, NULL);
  MSG msg;
  while (true)
    {
    while (GetMessage(&msg, window, 0, 0))
      DispatchMessage(&msg);
    }
  }

void start_fb(uint16_t x, uint16_t y, uint8_t *buffer)
  {
  dimx = x;
  dimy = y;
  fb_buffer = (const uint32_t *)buffer;

  task_create("wnd", DEFAULT_STACK_SIZE, worker, 0, BELOW_NORMAL, &h_worker);
  }
