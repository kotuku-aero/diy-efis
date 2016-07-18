/*
diy-efis
Copyright (C) 2016 Kotuku Aerospace Limited

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice and
the GPL3 are subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.


If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the origional copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <memory.h>

void X11_initialize(const char *fname, int dimx, int dimy);
void fbe_loop(int, int);

/* handler for XGetImage BadMatch errors */
static int x11_error_handler(Display *dpy, XErrorEvent *event)
  {
  return (event->request_code == 73 /*X_GetImage*/
  && event->error_code == BadMatch);
  }

static Display *display;
static GC gc;
static int screen;
static Window root, window;
static __uint32_t *crtbuf;
int dimx = 480;
int dimy = 640;

static bool repaint = false;
static Pixmap pixmap;

#define BITS_PER_PIXEL 32
#define PIXELS_PER_LONG 1

#define CHUNKX 32
#define CHUNKY 20

uint32_t *crcs;

int main(int argc, const char **argv)
  {
  if(argc < 2 || argc > 4)
    {
    printf("Usage: fb_emulator <fb_name> [<dimx>] [<dimy>]\n");
    printf("If dimx not provided, defaults to 480\n");
    printf("If dimy not provided, defaults to 640\n");
    printf("If the file <fb_name> does not exist it will be created\n");
    printf("Example: fb_emulator \tmp\fb0 480 640\n");
    return -1;
    }

  if(argc > 2)
    {
    dimx = atoi(argv[2]);
    if(dimx < 320 || dimx > 1024)
      {
      printf("Error: x-dimension must be 320-1024 pixels\n");
      return -1;
      }
    }

  if(argc > 3)
    {
    dimy = atoi(argv[3]);
    if(dimy > 1024 || dimy < 240)
      {
      printf("Error: y-dimension must be 240-1024 pixels\n");
      return -1;
      }
    }

  char *host;
  if((host = (char *) getenv("DISPLAY")) == NULL)
    {
    printf("Error: Cannot get DISPLAY environment variable\n");
    return -1;
    }

  if((display = XOpenDisplay(host)) == NULL)
    {
    printf("Error: Cannot open the display\n");
    return -1;
    }

  int size = (dimx / CHUNKX) * (dimy / CHUNKY);

  crcs = new uint32_t[size];

  screen = DefaultScreen(display);
  DefaultColormap(display, screen);
  root = RootWindow(display, screen);
  int depth = DefaultDepth(display, screen);

  XSelectInput(display, root, SubstructureNotifyMask);

  XSetWindowAttributes attr;
  attr.event_mask = ExposureMask | KeyPressMask | VisibilityChangeMask;
  attr.background_pixel = BlackPixel(display, screen);

  window = XCreateWindow(display, root, 0, 0, dimx, dimy, 0, depth,
      InputOutput, DefaultVisual(display, screen),
      CWEventMask | CWBackPixel, &attr);

  char name[80];
  sprintf(name, "Framebuffer Emulator: %d * %d * %d bpp", dimx, dimy, depth);

  XChangeProperty(display, window,
  XA_WM_NAME, XA_STRING, 8,
  PropModeReplace, reinterpret_cast<unsigned char *>(name), strlen(name));

  XMapWindow(display, window);

  Pixmap iconPixmap;
  XWMHints xwmhints;

  xwmhints.icon_pixmap = iconPixmap;
  xwmhints.initial_state = NormalState;
  xwmhints.flags = IconPixmapHint | StateHint;

  XSetErrorHandler(x11_error_handler);

  gc = XCreateGC(display, window, 0, NULL);
  pixmap = XCreatePixmap(display, window, CHUNKX, CHUNKY, depth);

  XSetWMHints(display, window, &xwmhints);
  XClearWindow(display, window);
  XSync(display, 0);

  int fd = open(argv[1], O_RDONLY);

  if(fd > 0)
    {
    close(fd);
    }
  else
    {
    unsigned char *block = new unsigned char[4096];

    for(size_t i = 0; i < 4096; i += 4)
      *reinterpret_cast<uint32_t *>(block + i) = 0;

    fd = open(argv[1], O_CREAT | O_WRONLY, 0777);
    for(size_t i = 0; i < ((dimx * dimy * sizeof(uint32_t )) / 4096); i++)
      write(fd, block, 4096);

    close(fd);
    }

  fd = open(argv[1], O_RDWR);
  size_t area = (dimx * dimy * 4);
  crtbuf = (uint32_t *) mmap(NULL, area, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  XEvent event;
  repaint = true;
  while(1)
    {
    /*
     Check if to force complete repaint because of window
     expose event
     */
    while(XPending(display) > 0)
      {
      XNextEvent(display, &event);
      if(event.type == Expose)
        repaint = true;
      }

    XRectangle xrect;
    xrect.x = 0;
    xrect.y = 0;
    xrect.width = dimx;
    xrect.height = dimy;

    XSetClipRectangles(display, gc, 0, 0, &xrect, 1, Unsorted);
    /*
     Sample all chunks for changes in shared memory buffer and
     eventually repaint individual chunks. Repaint everything if
     repaint is true (see above)
     */
    for(int iy = 0; iy < dimy / CHUNKY; iy++)
      for(int ix = 0; ix < dimx / CHUNKX; ix++)
        {
        uint32_t crc;
        size_t off;

        // calculate the crc of the display area to check
        off = (ix * CHUNKX) + (iy * CHUNKY * dimx);
        crc = 0x8154711;

        for(int x = 0; x < CHUNKX; x++)
          for(int y = 0; y < CHUNKY; y++)
            {
            unsigned long dat;
            size_t pixel = off + x + (y * dimx);
            dat = crtbuf[pixel];

            /*     crc^=((crc^dat)<<1)^((dat&0x8000) ? 0x1048:0);
             */
            crc += (crc % 211 + dat);
            /* crc=(crc<<1)+((crc&0x80000000) ? 1:0);  */
            }

        int offset = ix + (iy * (dimx/CHUNKX));

        if(repaint || crc != crcs[offset])
          {
          // not the same, or a forced redraw so copy the
          // pixels over
          for(int y = 0; y < CHUNKY; y++)
            for(int x = 0; x < CHUNKX; x++)
              {
              uint32_t color = crtbuf[off + x + (y * dimx)];

              XSetForeground(display, gc, color);
              XDrawPoint(display, pixmap, gc, x, y);
              }

          int dest_x = ix * CHUNKX;
          int dest_y = iy * CHUNKY;

          XCopyArea(display, pixmap, window, gc, 0, 0, CHUNKX, CHUNKY, dest_x, dest_y);

          crcs[offset] = crc;
          }
        }

    XSync(display, False);
    XFlush(display);
    usleep(1000);

    repaint = 0;
    }
  }
