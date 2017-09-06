#include "../neutron/neutron.h"
#include <string.h>

#define MINEXTEND      32768
#define LINEBUF_EXTRA  32

#ifndef TABSIZE
#define TABSIZE        8
#endif

#ifndef INDENT
#define INDENT "  "
#endif

#define CLRSCR           "\033[0J"
#define CLREOL           "\033[K"
#define GOTOXY           "\033[%d;%dH"
#define RESET_COLOR      "\033[0m"

#define COLOR
#ifdef COLOR
#define TEXT_COLOR       "\033[44m\033[37m\033[1m"
#define SELECT_COLOR     "\033[47m\033[37m\033[1m"
#define STATUS_COLOR     "\033[0m\033[47m\033[30m"
#else
#define TEXT_COLOR       "\033[0m"
#define SELECT_COLOR     "\033[7m\033[1m"
#define STATUS_COLOR     "\033[1m\033[7m"
#endif

//
// Key codes
//

#define KEY_BACKSPACE        0x101
#define KEY_ESC              0x102
#define KEY_INS              0x103
#define KEY_DEL              0x104
#define KEY_LEFT             0x105
#define KEY_RIGHT            0x106
#define KEY_UP               0x107
#define KEY_DOWN             0x108
#define KEY_HOME             0x109
#define KEY_END              0x10A
#define KEY_ENTER            0x10B
#define KEY_TAB              0x10C
#define KEY_PGUP             0x10D
#define KEY_PGDN             0x10E

#define KEY_CTRL_LEFT        0x10F
#define KEY_CTRL_RIGHT       0x110
#define KEY_CTRL_UP          0x111
#define KEY_CTRL_DOWN        0x112
#define KEY_CTRL_HOME        0x113
#define KEY_CTRL_END         0x114
#define KEY_CTRL_TAB         0x115

#define KEY_SHIFT_LEFT       0x116
#define KEY_SHIFT_RIGHT      0x117
#define KEY_SHIFT_UP         0x118
#define KEY_SHIFT_DOWN       0x119
#define KEY_SHIFT_PGUP       0x11A
#define KEY_SHIFT_PGDN       0x11B
#define KEY_SHIFT_HOME       0x11C
#define KEY_SHIFT_END        0x11D
#define KEY_SHIFT_TAB        0x11E

#define KEY_SHIFT_CTRL_LEFT  0x11F
#define KEY_SHIFT_CTRL_RIGHT 0x120
#define KEY_SHIFT_CTRL_UP    0x121
#define KEY_SHIFT_CTRL_DOWN  0x122
#define KEY_SHIFT_CTRL_HOME  0x123
#define KEY_SHIFT_CTRL_END   0x124

#define KEY_F1               0x125
#define KEY_F3               0x126
#define KEY_F5               0x127

#define KEY_UNKNOWN          0xFFF

#define ctrl(c) ((c) - 0x60)

//
// Editor data block
//
// Structure of split buffer:
//
//    +------------------+------------------+------------------+
//    | text before gap  |        gap       |  text after gap  |
//    +------------------+------------------+------------------+
//    ^                  ^                  ^                  ^
//    |                  |                  |                  |
//  start               gap                rest               end
//

typedef struct _undo_t
  {
  int pos; // Editor position
  int erased; // Size of erased contents
  int inserted; // Size of inserted contents
  char *undobuf; // Erased contents for undo
  char *redobuf; // Inserted contents for redo
  struct _undo_t *next; // Next undo buffer
  struct _undo_t *prev; // Previous undo buffer
  } undo_t;

typedef struct _editor_t
  {
  char *start; // Start of text buffer
  char *gap; // Start of gap
  char *rest; // End of gap
  char *end; // End of text buffer

  int toppos; // Text position for current top screen line
  int topline; // Line number for top of screen
  int margin; // Position for leftmost column on screen

  int linepos; // Text position for current line
  int line; // Current document line
  int col; // Current document column
  int lastcol; // Remembered column from last horizontal navigation
  int anchor; // Anchor position for selection

  undo_t *undohead; // Start of undo buffer list
  undo_t *undotail; // End of undo buffer list
  undo_t *undo; // Undo/redo boundary

  int refresh; // Flag to trigger screen redraw
  int lineupdate; // Flag to trigger redraw of current line
  int dirty; // Dirty flag is set when the editor buffer has been changed

  int newfile; // File is a new file

  char *clipboard; // Clipboard
  int clipsize; // Clipboard size

  char *search; // Search text
  char *linebuf; // Scratch buffer

  int cols; // Console columns
  int lines; // Console lines

  int untitled; // Counter for untitled files

  handle_t stdin;
  handle_t stdout;

  handle_t stream;
  const char *title;        // name of the stream
  } editor_t;

static char _getchar(editor_t *ed)
  {
  char ch;
  stream_read(ed->stdin, &ch, 1, 0);
  return ch;
  }

//
// Editor buffer functions
//

void clear_undo(editor_t *ed)
  {
  undo_t *undo = ed->undohead;
  while (undo)
    {
    undo_t *next = undo->next;
    kfree(undo->undobuf);
    kfree(undo->redobuf);
    kfree(undo);
    undo = next;
    }
  ed->undohead = ed->undotail = ed->undo = 0;
  }

void reset_undo(editor_t *ed)
  {
  while (ed->undotail != ed->undo)
    {
    undo_t *undo = ed->undotail;
    if (!undo)
      {
      ed->undohead = 0;
      ed->undotail = 0;
      break;
      }
    ed->undotail = undo->prev;
    if (undo->prev) undo->prev->next = 0;
    kfree(undo->undobuf);
    kfree(undo->redobuf);
    kfree(undo);
    }
  ed->undo = ed->undotail;
  }

result_t load_file(editor_t *ed, handle_t stream)
  {
  uint16_t length;
  int f;
  result_t result;

  if(failed(result = stream_length(stream, &length)))
    return result;

  ed->stream = stream;

  ed->start = (char *) kmalloc(length + MINEXTEND);
  if (!ed->start)
    return e_no_space;

  if(succeeded(result = stream_setpos(stream, 0)) &&
     succeeded(result = stream_read(stream, ed->start, length, 0)))
    {
    ed->gap = ed->start + length;
    ed->rest = ed->end = ed->gap + MINEXTEND;
    ed->anchor = -1;
    }

  if (failed(result) && ed->start)
    {
    kfree(ed->start);
    ed->start = 0;
    }

  return result;
  }

result_t save_file(editor_t *ed)
  {
  result_t result;

  if(failed(result = stream_setpos(ed->stream, 0)))
    return result;

  if(failed(result = stream_write(ed->stream, ed->start, ed->gap - ed->start)))
    return result;

  if((ed->end - ed->rest) > 0 &&
     failed(result = stream_write(ed->stream, ed->rest, ed->end - ed->rest)))
     return result;

  // and set the size of the stream
  uint16_t pos;

  if (failed(result = stream_getpos(ed->stream, &pos)) ||
    failed(result = stream_truncate(ed->stream, pos)))
    return result;

  ed->dirty = false;
  clear_undo(ed);

  return s_ok;
  }

int text_length(editor_t *ed)
  {
  return (ed->gap - ed->start) + (ed->end - ed->rest);
  }

char *text_ptr(editor_t *ed, int pos)
  {
  char *p = ed->start + pos;
  if (p >= ed->gap) p += (ed->rest - ed->gap);
  return p;
  }

void move_gap(editor_t *ed, int pos, int minsize)
  {
  int gapsize = ed->rest - ed->gap;
  char *p = text_ptr(ed, pos);
  if (minsize < 0) minsize = 0;

  if (minsize <= gapsize)
    {
    if (p != ed->rest)
      {
      if (p < ed->gap)
        {
        memmove(p + gapsize, p, ed->gap - p);
        }
      else
        {
        memmove(ed->gap, ed->rest, p - ed->rest);
        }
      ed->gap = ed->start + pos;
      ed->rest = ed->gap + gapsize;
      }
    }
  else
    {
    int newsize;
    char *start;
    char *gap;
    char *rest;
    char *end;

    if (gapsize + MINEXTEND > minsize) minsize = gapsize + MINEXTEND;
    newsize = (ed->end - ed->start) - gapsize + minsize;
    start = (char *) kmalloc(newsize); // TODO check for out of memory
    gap = start + pos;
    rest = gap + minsize;
    end = start + newsize;

    if (p < ed->gap)
      {
      memcpy(start, ed->start, pos);
      memcpy(rest, p, ed->gap - p);
      memcpy(end - (ed->end - ed->rest), ed->rest, ed->end - ed->rest);
      }
    else
      {
      memcpy(start, ed->start, ed->gap - ed->start);
      memcpy(start + (ed->gap - ed->start), ed->rest, p - ed->rest);
      memcpy(rest, p, ed->end - p);
      }

    kfree(ed->start);
    ed->start = start;
    ed->gap = gap;
    ed->rest = rest;
    ed->end = end;
    }

#ifdef DEBUG
  memset(ed->gap, 0, ed->rest - ed->gap);
#endif
  }

void close_gap(editor_t *ed)
  {
  int len = text_length(ed);
  move_gap(ed, len, 1);
  ed->start[len] = 0;
  }

int get(editor_t *ed, int pos)
  {
  char *p = text_ptr(ed, pos);
  if (p >= ed->end) return -1;
  return *p;
  }

int compare(editor_t *ed, char *buf, int pos, int len)
  {
  char *bufptr = buf;
  char *p = ed->start + pos;
  if (p >= ed->gap) p += (ed->rest - ed->gap);

  while (len > 0)
    {
    if (p == ed->end) return 0;
    if (*bufptr++ != *p) return 0;
    len--;
    if (++p == ed->gap) p = ed->rest;
    }

  return 1;
  }

int copy(editor_t *ed, char *buf, int pos, int len)
  {
  char *bufptr = buf;
  char *p = ed->start + pos;
  if (p >= ed->gap) p += (ed->rest - ed->gap);

  while (len > 0)
    {
    if (p == ed->end) break;
    *bufptr++ = *p;
    len--;
    if (++p == ed->gap) p = ed->rest;
    }

  return bufptr - buf;
  }

void replace(editor_t *ed, int pos, int len, char *buf, int bufsize, int doundo)
  {
  char *p;
  undo_t *undo;

  // Store undo information
  if (doundo)
    {
    reset_undo(ed);
    undo = ed->undotail;
    if (undo && len == 0 && bufsize == 1 && undo->erased == 0 && pos == undo->pos + undo->inserted)
      {
      // Insert character at end of current redo buffer
      undo->redobuf = krealloc(undo->redobuf, undo->inserted + 1);
      undo->redobuf[undo->inserted] = *buf;
      undo->inserted++;
      }
    else if (undo && len == 1 && bufsize == 0 && undo->inserted == 0 && pos == undo->pos)
      {
      // Erase character at end of current undo buffer
      undo->undobuf = krealloc(undo->undobuf, undo->erased + 1);
      undo->undobuf[undo->erased] = get(ed, pos);
      undo->erased++;
      }
    else if (undo && len == 1 && bufsize == 0 && undo->inserted == 0 && pos == undo->pos - 1)
      {
      // Erase character at beginning of current undo buffer
      undo->pos--;
      undo->undobuf = krealloc(undo->undobuf, undo->erased + 1);
      memmove(undo->undobuf + 1, undo->undobuf, undo->erased);
      undo->undobuf[0] = get(ed, pos);
      undo->erased++;
      }
    else
      {
      // Create new undo buffer
      undo = (undo_t *) kmalloc(sizeof (struct _undo_t));
      if (ed->undotail) ed->undotail->next = undo;
      undo->prev = ed->undotail;
      undo->next = 0;
      ed->undotail = ed->undo = undo;
      if (!ed->undohead) ed->undohead = undo;

      undo->pos = pos;
      undo->erased = len;
      undo->inserted = bufsize;
      undo->undobuf = undo->redobuf = 0;
      if (len > 0)
        {
        undo->undobuf = kmalloc(len);
        copy(ed, undo->undobuf, pos, len);
        }
      if (bufsize > 0)
        {
        undo->redobuf = kmalloc(bufsize);
        memcpy(undo->redobuf, buf, bufsize);
        }
      }
    }

  p = ed->start + pos;
  if (bufsize == 0 && p <= ed->gap && p + len >= ed->gap)
    {
    // Handle deletions at the edges of the gap
    ed->rest += len - (ed->gap - p);
    ed->gap = p;
    }
  else
    {
    // Move the gap
    move_gap(ed, pos + len, bufsize - len);

    // Replace contents
    memcpy(ed->start + pos, buf, bufsize);
    ed->gap = ed->start + pos + bufsize;
    }

  // Mark buffer as dirty
  ed->dirty = 1;
  }

void insert(editor_t *ed, int pos, char *buf, int bufsize)
  {
  replace(ed, pos, 0, buf, bufsize, 1);
  }

void erase(editor_t *ed, int pos, int len)
  {
  replace(ed, pos, len, 0, 0, 1);
  }

//
// Navigation functions
//

int line_length(editor_t *ed, int linepos)
  {
  int pos = linepos;
  while (1)
    {
    int ch = get(ed, pos);
    if (ch < 0 || ch == '\n' || ch == '\r') break;
    pos++;
    }

  return pos - linepos;
  }

int line_start(editor_t *ed, int pos)
  {
  while (1)
    {
    if (pos == 0) break;
    if (get(ed, pos - 1) == '\n') break;
    pos--;
    }

  return pos;
  }

int next_line(editor_t *ed, int pos)
  {
  while (1)
    {
    int ch = get(ed, pos);
    if (ch < 0) return -1;
    pos++;
    if (ch == '\n') return pos;
    }
  }

int prev_line(editor_t *ed, int pos)
  {
  if (pos == 0) return -1;

  while (pos > 0)
    {
    int ch = get(ed, --pos);
    if (ch == '\n') break;
    }

  while (pos > 0)
    {
    int ch = get(ed, --pos);
    if (ch == '\n') return pos + 1;
    }

  return 0;
  }

int column(editor_t *ed, int linepos, int col)
  {
  char *p = text_ptr(ed, linepos);
  int c = 0;
  while (col > 0)
    {
    if (p == ed->end) break;
    if (*p == '\t')
      {
      int spaces = TABSIZE - c % TABSIZE;
      c += spaces;
      }
    else
      {
      c++;
      }
    col--;
    if (++p == ed->gap) p = ed->rest;
    }
  return c;
  }

void moveto(editor_t *ed, int pos, int center)
  {
  int scroll = 0;
  for (;;)
    {
    int cur = ed->linepos + ed->col;
    if (pos < cur)
      {
      if (pos >= ed->linepos)
        {
        ed->col = pos - ed->linepos;
        }
      else
        {
        ed->col = 0;
        ed->linepos = prev_line(ed, ed->linepos);
        ed->line--;

        if (ed->topline > ed->line)
          {
          ed->toppos = ed->linepos;
          ed->topline--;
          ed->refresh = 1;
          scroll = 1;
          }
        }
      }
    else if (pos > cur)
      {
      int next = next_line(ed, ed->linepos);
      if (next == -1)
        {
        ed->col = line_length(ed, ed->linepos);
        break;
        }
      else if (pos < next)
        {
        ed->col = pos - ed->linepos;
        }
      else
        {
        ed->col = 0;
        ed->linepos = next;
        ed->line++;

        if (ed->line >= ed->topline + ed->lines)
          {
          ed->toppos = next_line(ed, ed->toppos);
          ed->topline++;
          ed->refresh = 1;
          scroll = 1;
          }
        }
      }
    else
      {
      break;
      }
    }

  if (scroll && center)
    {
    int tl = ed->line - ed->lines / 2;
    if (tl < 0) tl = 0;
    for (;;)
      {
      if (ed->topline > tl)
        {
        ed->toppos = prev_line(ed, ed->toppos);
        ed->topline--;
        }
      else if (ed->topline < tl)
        {
        ed->toppos = next_line(ed, ed->toppos);
        ed->topline++;
        }
      else
        {
        break;
        }
      }
    }
  }

//
// Text selection
//

int get_selection(editor_t *ed, int *start, int *end)
  {
  if (ed->anchor == -1)
    {
    *start = *end = -1;
    return 0;
    }
  else
    {
    int pos = ed->linepos + ed->col;
    if (pos == ed->anchor)
      {
      *start = *end = -1;
      return 0;
      }
    else if (pos < ed->anchor)
      {
      *start = pos;
      *end = ed->anchor;
      }
    else
      {
      *start = ed->anchor;
      *end = pos;
      }
    }
  return 1;
  }

int get_selected_text(editor_t *ed, char *buffer, int size)
  {
  int selstart, selend, len;

  if (!get_selection(ed, &selstart, &selend)) return 0;
  len = selend - selstart;
  if (len >= size) return 0;
  copy(ed, buffer, selstart, len);
  buffer[len] = 0;
  return len;
  }

void update_selection(editor_t *ed, int select)
  {
  if (select)
    {
    if (ed->anchor == -1) ed->anchor = ed->linepos + ed->col;
    ed->refresh = 1;
    }
  else
    {
    if (ed->anchor != -1) ed->refresh = 1;
    ed->anchor = -1;
    }
  }

int erase_selection(editor_t *ed)
  {
  int selstart, selend;

  if (!get_selection(ed, &selstart, &selend)) return 0;
  moveto(ed, selstart, 0);
  erase(ed, selstart, selend - selstart);
  ed->anchor = -1;
  ed->refresh = 1;
  return 1;
  }

void select_all(editor_t *ed)
  {
  ed->anchor = 0;
  ed->refresh = 1;
  moveto(ed, text_length(ed), 0);
  }

//
// Screen functions
//

#define SCREEN_X  80
#define SCREEN_Y  24

void get_console_size(editor_t *ed)
  {
  ed->cols = SCREEN_X;
  ed->lines = SCREEN_Y - 1;

  ed->linebuf = krealloc(ed->linebuf, ed->cols + LINEBUF_EXTRA);
  }

static void outch(editor_t *ed, char c)
  {
  stream_write(ed->stdout, &c, 1);
  }

static void outbuf(editor_t *ed, char *buf, int len)
  {
  stream_write(ed->stdout, buf, len);
  }

static void outstr(editor_t *ed, char *str)
  {
  outbuf(ed, str, strlen(str));
  }

static void clear_screen(editor_t *ed)
  {
  outstr(ed, CLRSCR);
  }

static void gotoxy(editor_t *ed, int col, int line)
  {
  char buf[32];

  snprintf(buf, 32, GOTOXY, line + 1, col + 1);
  outstr(ed, buf);
  }

//
// Keyboard functions
//
/**
 * Interrogate the keyboard for the status of the shift and control
 * @param shift
 * @param ctrl
 */
void get_modifier_keys(int *shift, int *ctrl)
  {
  *shift = *ctrl = 0;
  }

/**
 * Get a key
 * @param ed
 * @return
 */
int getkey(editor_t *ed)
  {
  int ch;
  int shift;
  int ctrl;

  ch = _getchar(ed);

  if (ch < 0)
    return ch;

  switch (ch)
    {
    case 0x08:
      return KEY_BACKSPACE;
    case 0x09:
      get_modifier_keys(&shift, &ctrl);
      if (shift)
        return KEY_SHIFT_TAB;
      if (ctrl)
        return KEY_CTRL_TAB;
      return KEY_TAB;
    case 0x0D:
      return KEY_ENTER;
    case 0x0A:
      return KEY_ENTER;
    case 0x1B:
      ch = _getchar(ed);
      switch (ch)
        {
        case 0x1B:
          return KEY_ESC;
        case 0x4F:
          ch = _getchar(ed);
          switch (ch)
            {
            case 0x46:
              return KEY_END;
            case 0x48:
              return KEY_HOME;
            case 0x50:
              return KEY_F1;
            case 0x52:
              return KEY_F3;
            case 0x54:
              return KEY_F5;
            default:
              return KEY_UNKNOWN;
            }
          break;

        case 0x5B:
          get_modifier_keys(&shift, &ctrl);
          ch = _getchar(ed);
          if (ch == 0x31)
            {
            ch = _getchar(ed);
            switch (ch)
              {
              case 0x3B:
                ch = _getchar(ed);
                if (ch == 0x32)
                  shift = 1;
                if (ch == 0x35)
                  ctrl = 1;
                if (ch == 0x36)
                  shift = ctrl = 1;
                ch = _getchar(ed);
                break;
              case 0x35:
                return
                  _getchar(ed)  == 0x7E ? KEY_F5 : KEY_UNKNOWN;
              case 0x7E:
                if (shift && ctrl)
                  return KEY_SHIFT_CTRL_HOME;
                if (shift)
                  return KEY_SHIFT_HOME;
                if (ctrl)
                  return KEY_CTRL_HOME;
                return KEY_HOME;
              default:
                return KEY_UNKNOWN;
              }
            }

          switch (ch)
            {
            case 0x31:
              if (_getchar(ed) != 0x7E)
                return KEY_UNKNOWN;
              if (shift && ctrl)
                return KEY_SHIFT_CTRL_HOME;
              if (shift)
                return KEY_SHIFT_HOME;
              if (ctrl)
                return KEY_CTRL_HOME;
              return KEY_HOME;
            case 0x32:
              return _getchar(ed) == 0x7E ? KEY_INS: KEY_UNKNOWN;
            case 0x33:
              return _getchar(ed) == 0x7E ? KEY_DEL: KEY_UNKNOWN;
            case 0x34:
              if (_getchar(ed) != 0x7E)
                return KEY_UNKNOWN;
              if (shift && ctrl)
                return KEY_SHIFT_CTRL_END;
              if (shift)
                return KEY_SHIFT_END;
              if (ctrl)
                return KEY_CTRL_END;
              return KEY_END;
            case 0x35:
              if (_getchar(ed) != 0x7E) return KEY_UNKNOWN;
              if (shift) return KEY_SHIFT_PGUP;
              return KEY_PGUP;
            case 0x36:
              if (_getchar(ed) != 0x7E) return KEY_UNKNOWN;
              if (shift) return KEY_SHIFT_PGDN;
              return KEY_PGDN;
            case 0x41:
              if (shift && ctrl)
                return KEY_SHIFT_CTRL_UP;
              if (shift)
                return KEY_SHIFT_UP;
              if (ctrl)
                return KEY_CTRL_UP;
              return KEY_UP;
            case 0x42:
              if (shift && ctrl)
                return KEY_SHIFT_CTRL_DOWN;
              if (shift)
                return KEY_SHIFT_DOWN;
              if (ctrl)
                return KEY_CTRL_DOWN;
              return KEY_DOWN;
            case 0x43:
              if (shift && ctrl)
                return KEY_SHIFT_CTRL_RIGHT;
              if (shift)
                return KEY_SHIFT_RIGHT;
              if (ctrl)
                return KEY_CTRL_RIGHT;
              return KEY_RIGHT;
            case 0x44:
              if (shift && ctrl)
                return KEY_SHIFT_CTRL_LEFT;
              if (shift)
                return KEY_SHIFT_LEFT;
              if (ctrl)
                return KEY_CTRL_LEFT;
              return KEY_LEFT;
            case 0x46:
              if (shift && ctrl)
                return KEY_SHIFT_CTRL_END;
              if (shift)
                return KEY_SHIFT_END;
              if (ctrl)
                return KEY_CTRL_END;
              return KEY_END;
            case 0x48:
              if (shift && ctrl)
                return KEY_SHIFT_CTRL_HOME;
              if (shift)
                return KEY_SHIFT_HOME;
              if (ctrl)
                return KEY_CTRL_HOME;
              return KEY_HOME;
            case 0x5A:
              return KEY_SHIFT_TAB;
            case 0x5B:
              ch = _getchar(ed);
              switch (ch)
                {
                case 0x41:
                  return KEY_F1;
                case 0x43:
                  return KEY_F3;
                case 0x45:
                  return KEY_F5;
                }
              return KEY_UNKNOWN;

            default:
              return KEY_UNKNOWN;
            }
          break;

        default:
          return KEY_UNKNOWN;
        }
      break;

    case 0x00:
    case 0xE0:
      ch = _getchar(ed);
      switch (ch)
        {
        case 0x0F:
          return KEY_SHIFT_TAB;
        case 0x3B:
          return KEY_F1;
        case 0x3D:
          return KEY_F3;
        case 0x3F:
          return KEY_F5;
        case 0x47:
          return KEY_HOME;
        case 0x48:
          return KEY_UP;
        case 0x49:
          return KEY_PGUP;
        case 0x4B:
          return KEY_LEFT;
        case 0x4D:
          return KEY_RIGHT;
        case 0x4F:
          return KEY_END;
        case 0x50:
          return KEY_DOWN;
        case 0x51:
          return KEY_PGDN;
        case 0x52:
          return KEY_INS;
        case 0x53:
          return KEY_DEL;
        case 0x73:
          return KEY_CTRL_LEFT;
        case 0x74:
          return KEY_CTRL_RIGHT;
        case 0x75:
          return KEY_CTRL_END;
        case 0x77:
          return KEY_CTRL_HOME;
        case 0x8D:
          return KEY_CTRL_UP;
        case 0x91:
          return KEY_CTRL_DOWN;
        case 0x94:
          return KEY_CTRL_TAB;
        case 0xB8:
          return KEY_SHIFT_UP;
        case 0xB7:
          return KEY_SHIFT_HOME;
        case 0xBF:
          return KEY_SHIFT_END;
        case 0xB9:
          return KEY_SHIFT_PGUP;
        case 0xBB:
          return KEY_SHIFT_LEFT;
        case 0xBD:
          return KEY_SHIFT_RIGHT;
        case 0xC0:
          return KEY_SHIFT_DOWN;
        case 0xC1:
          return KEY_SHIFT_PGDN;
        case 0xDB:
          return KEY_SHIFT_CTRL_LEFT;
        case 0xDD:
          return KEY_SHIFT_CTRL_RIGHT;
        case 0xD8:
          return KEY_SHIFT_CTRL_UP;
        case 0xE0:
          return KEY_SHIFT_CTRL_DOWN;
        case 0xD7:
          return KEY_SHIFT_CTRL_HOME;
        case 0xDF:
          return KEY_SHIFT_CTRL_END;

        default:
          return KEY_UNKNOWN;
        }
      break;

    case 0x7F:
      return KEY_BACKSPACE;

    default:
      return ch;
    }
  }

int prompt(editor_t *ed, char *msg, int selection)
  {
  int maxlen, len, ch;
  char *buf = ed->linebuf;

  gotoxy(ed, 0, ed->lines);
  outstr(ed, STATUS_COLOR);
  outstr(ed, msg);
  outstr(ed, CLREOL);

  len = 0;
  maxlen = ed->cols - strlen(msg) - 1;
  if (selection)
    {
    len = get_selected_text(ed, buf, maxlen);
    outbuf(ed, buf, len);
    }

  for (;;)
    {
    ch = getkey(ed);
    if (ch == KEY_ESC)
      {
      return 0;
      }
    else if (ch == KEY_ENTER)
      {
      buf[len] = 0;
      return len > 0;
      }
    else if (ch == KEY_BACKSPACE)
      {
      if (len > 0)
        {
        outstr(ed, "\b \b");
        len--;
        }
      }
    else if (ch >= ' ' && ch < 0x100 && len < maxlen)
      {
      outch(ed, ch);
      buf[len++] = ch;
      }
    }
  }

int ask(editor_t *ed)
  {
  int ch = _getchar(ed);
  return ch == 'y' || ch == 'Y';
  }

//
// Display functions
//
static result_t printf_args(uint16_t arg,
                           void *parg,
                           scan_type dt,
                           void *value)
  {
  void *param = ((char *)value) + arg;

  switch(dt)
    {
    case s_int8 :
      *((int8_t *)value) = *((int8_t *) param);
      break;
    case s_uint8 :
      *((uint8_t *)value) = *((uint8_t *) param);
      break;
    case s_int16 :
      *((int16_t *)value) = *((int16_t *) param);
      break;
    case s_uint16 :
      *((uint16_t *)value) = *((uint16_t *) param);
      break;
    case s_int32 :
      *((int32_t *)value) = *((int32_t *) param);
      break;
    case s_uint32 :
      *((uint32_t *)value) = *((uint32_t *) param);
      break;
    case s_str :
      *((char **)value) = *((char **) param);
      break;
    case s_float :
      *((float *)value) = *((float *) param);
      break;
    default :
      return e_bad_parameter;
    }


  return s_ok;
  }

void display_message(editor_t *ed, char *fmt, ...)
  {
  gotoxy(ed, 0, ed->lines);
  outstr(ed, STATUS_COLOR);

  stream_printf(ed->stdout, fmt, printf_args, (&fmt)+1);

  outstr(ed, CLREOL TEXT_COLOR);
  }

void draw_full_statusline(editor_t *ed)
  {
  int namewidth = ed->cols - 28;
  gotoxy(ed, 0, ed->lines);

  sprintf(ed->linebuf, STATUS_COLOR "%*.*sF1=Help %c Ln %-6dCol %-4d" CLREOL TEXT_COLOR,
          -namewidth,
          namewidth,
          ed->title,
          ed->dirty ? '*' : ' ', ed->line + 1,
          column(ed, ed->linepos, ed->col) + 1);

  outstr(ed, ed->linebuf);
  }

void draw_statusline(editor_t *ed)
  {
  gotoxy(ed, ed->cols - 20, ed->lines);
  sprintf(ed->linebuf, STATUS_COLOR "%c Ln %-6dCol %-4d" CLREOL TEXT_COLOR, ed->dirty ? '*' : ' ', ed->line + 1, column(ed, ed->linepos, ed->col) + 1);
  outstr(ed, ed->linebuf);
  }

void display_line(editor_t *ed, int pos, int fullline)
  {
  int hilite = 0;
  int col = 0;
  int margin = ed->margin;
  int maxcol = ed->cols + margin;
  char *bufptr = ed->linebuf;
  char *p = text_ptr(ed, pos);
  int selstart, selend, ch;
  char *s;

  get_selection(ed, &selstart, &selend);
  while (col < maxcol)
    {
    if (margin == 0)
      {
      if (!hilite && pos >= selstart && pos < selend)
        {
        for (s = SELECT_COLOR; *s; s++)
          *bufptr++ = *s;
        hilite = 1;
        }
      else if (hilite && pos >= selend)
        {
        for (s = TEXT_COLOR; *s; s++)
          *bufptr++ = *s;
        hilite = 0;
        }
      }

    if (p == ed->end)
      break;
    ch = *p;
    if (ch == '\r' || ch == '\n')
      break;

    if (ch == '\t')
      {
      int spaces = TABSIZE - col % TABSIZE;
      while (spaces > 0 && col < maxcol)
        {
        if (margin > 0)
          {
          margin--;
          }
        else
          {
          *bufptr++ = ' ';
          }
        col++;
        spaces--;
        }
      }
    else
      {
      if (margin > 0)
        {
        margin--;
        }
      else
        {
        *bufptr++ = ch;
        }
      col++;
      }

    if (++p == ed->gap) p = ed->rest;
    pos++;
    }

  if (col < maxcol)
    {
    for (s = CLREOL; *s; s++) *bufptr++ = *s;
    if (fullline)
      {
      memcpy(bufptr, "\r\n", 2);
      bufptr += 2;
      }
    }

  if (hilite)
    {
    for (s = TEXT_COLOR; *s; s++) *bufptr++ = *s;
    }

  outbuf(ed, ed->linebuf, bufptr - ed->linebuf);
  }

void update_line(editor_t *ed)
  {
  gotoxy(ed, 0, ed->line - ed->topline);
  display_line(ed, ed->linepos, 0);
  }

void draw_screen(editor_t *ed)
  {
  int pos;
  int i;

  gotoxy(ed, 0, 0);
  outstr(ed, TEXT_COLOR);
  pos = ed->toppos;
  for (i = 0; i < ed->lines; i++)
    {
    if (pos < 0)
      {
      outstr(ed, CLREOL "\r\n");
      }
    else
      {
      display_line(ed, pos, 1);
      pos = next_line(ed, pos);
      }
    }
  }

void position_cursor(editor_t *ed)
  {
  int col = column(ed, ed->linepos, ed->col);
  gotoxy(ed, col - ed->margin, ed->line - ed->topline);
  }

//
// Cursor movement
//

void adjust(editor_t *ed)
  {
  int col;
  int ll = line_length(ed, ed->linepos);
  ed->col = ed->lastcol;
  if (ed->col > ll)
    ed->col = ll;

  col = column(ed, ed->linepos, ed->col);
  while (col < ed->margin)
    {
    ed->margin -= 4;
    if (ed->margin < 0)
      ed->margin = 0;
    ed->refresh = 1;
    }

  while (col - ed->margin >= ed->cols)
    {
    ed->margin += 4;
    ed->refresh = 1;
    }
  }

void up(editor_t *ed, int select)
  {
  int newpos;

  update_selection(ed, select);

  newpos = prev_line(ed, ed->linepos);
  if (newpos < 0) return;

  ed->linepos = newpos;
  ed->line--;
  if (ed->line < ed->topline)
    {
    ed->toppos = ed->linepos;
    ed->topline = ed->line;
    ed->refresh = 1;
    }

  adjust(ed);
  }

void down(editor_t *ed, int select)
  {
  int newpos;

  update_selection(ed, select);

  newpos = next_line(ed, ed->linepos);
  if (newpos < 0) return;

  ed->linepos = newpos;
  ed->line++;

  if (ed->line >= ed->topline + ed->lines)
    {
    ed->toppos = next_line(ed, ed->toppos);
    ed->topline++;
    ed->refresh = 1;
    }

  adjust(ed);
  }

void left(editor_t *ed, int select)
  {
  update_selection(ed, select);
  if (ed->col > 0)
    {
    ed->col--;
    }
  else
    {
    int newpos = prev_line(ed, ed->linepos);
    if (newpos < 0)
      return;

    ed->col = line_length(ed, newpos);
    ed->linepos = newpos;
    ed->line--;
    if (ed->line < ed->topline)
      {
      ed->toppos = ed->linepos;
      ed->topline = ed->line;
      ed->refresh = 1;
      }
    }

  ed->lastcol = ed->col;
  adjust(ed);
  }

void right(editor_t *ed, int select)
  {
  update_selection(ed, select);
  if (ed->col < line_length(ed, ed->linepos))
    {
    ed->col++;
    }
  else
    {
    int newpos = next_line(ed, ed->linepos);
    if (newpos < 0)
      return;

    ed->col = 0;
    ed->linepos = newpos;
    ed->line++;

    if (ed->line >= ed->topline + ed->lines)
      {
      ed->toppos = next_line(ed, ed->toppos);
      ed->topline++;
      ed->refresh = 1;
      }
    }

  ed->lastcol = ed->col;
  adjust(ed);
  }

int wordchar(int ch)
  {
  return ch >= 'A' && ch <= 'Z' || ch >= 'a' && ch <= 'z' || ch >= '0' && ch <= '9';
  }

void wordleft(editor_t *ed, int select)
  {
  int pos, phase;

  update_selection(ed, select);
  pos = ed->linepos + ed->col;
  phase = 0;
  while (pos > 0)
    {
    int ch = get(ed, pos - 1);
    if (phase == 0)
      {
      if (wordchar(ch)) phase = 1;
      }
    else
      {
      if (!wordchar(ch))
        break;
      }

    pos--;
    if (pos < ed->linepos)
      {
      ed->linepos = prev_line(ed, ed->linepos);
      ed->line--;
      ed->refresh = 1;
      }
    }
  ed->col = pos - ed->linepos;
  if (ed->line < ed->topline)
    {
    ed->toppos = ed->linepos;
    ed->topline = ed->line;
    }

  ed->lastcol = ed->col;
  adjust(ed);
  }

void wordright(editor_t *ed, int select)
  {
  int pos, end, phase, next;

  update_selection(ed, select);
  pos = ed->linepos + ed->col;
  end = text_length(ed);
  next = next_line(ed, ed->linepos);
  phase = 0;
  while (pos < end)
    {
    int ch = get(ed, pos);
    if (phase == 0)
      {
      if (wordchar(ch)) phase = 1;
      }
    else
      {
      if (!wordchar(ch)) break;
      }

    pos++;
    if (pos == next)
      {
      ed->linepos = next;
      next = next_line(ed, ed->linepos);
      ed->line++;
      ed->refresh = 1;
      }
    }
  ed->col = pos - ed->linepos;
  if (ed->line >= ed->topline + ed->lines)
    {
    ed->toppos = next_line(ed, ed->toppos);
    ed->topline++;
    }

  ed->lastcol = ed->col;
  adjust(ed);
  }

void home(editor_t *ed, int select)
  {
  update_selection(ed, select);
  ed->col = ed->lastcol = 0;
  adjust(ed);
  }

void end(editor_t *ed, int select)
  {
  update_selection(ed, select);
  ed->col = ed->lastcol = line_length(ed, ed->linepos);
  adjust(ed);
  }

void top(editor_t *ed, int select)
  {
  update_selection(ed, select);
  ed->toppos = ed->topline = ed->margin = 0;
  ed->linepos = ed->line = ed->col = ed->lastcol = 0;
  ed->refresh = 1;
  }

void bottom(editor_t *ed, int select)
  {
  update_selection(ed, select);
  for (;;)
    {
    int newpos = next_line(ed, ed->linepos);
    if (newpos < 0) break;

    ed->linepos = newpos;
    ed->line++;

    if (ed->line >= ed->topline + ed->lines)
      {
      ed->toppos = next_line(ed, ed->toppos);
      ed->topline++;
      ed->refresh = 1;
      }
    }
  ed->col = ed->lastcol = line_length(ed, ed->linepos);
  adjust(ed);
  }

void pageup(editor_t *ed, int select)
  {
  int i;

  update_selection(ed, select);
  if (ed->line < ed->lines)
    {
    ed->linepos = ed->toppos = 0;
    ed->line = ed->topline = 0;
    }
  else
    {
    for (i = 0; i < ed->lines; i++)
      {
      int newpos = prev_line(ed, ed->linepos);
      if (newpos < 0) return;

      ed->linepos = newpos;
      ed->line--;

      if (ed->topline > 0)
        {
        ed->toppos = prev_line(ed, ed->toppos);
        ed->topline--;
        }
      }
    }

  ed->refresh = 1;
  adjust(ed);
  }

void pagedown(editor_t *ed, int select)
  {
  int i;

  update_selection(ed, select);
  for (i = 0; i < ed->lines; i++)
    {
    int newpos = next_line(ed, ed->linepos);
    if (newpos < 0) break;

    ed->linepos = newpos;
    ed->line++;

    ed->toppos = next_line(ed, ed->toppos);
    ed->topline++;
    }

  ed->refresh = 1;
  adjust(ed);
  }

//
// Text editing
//

void insert_char(editor_t *ed, char ch)
  {
  erase_selection(ed);
  insert(ed, ed->linepos + ed->col, &ch, 1);
  ed->col++;
  ed->lastcol = ed->col;
  adjust(ed);
  if (!ed->refresh) ed->lineupdate = 1;
  }

void newline(editor_t *ed)
  {
  int p;
  char ch;

  erase_selection(ed);
  insert(ed, ed->linepos + ed->col, "\r\n", 2);
  ed->col = ed->lastcol = 0;
  ed->line++;

  p = ed->linepos;
  ed->linepos = next_line(ed, ed->linepos);
  /*
  for (;;)
    {
    ch = get(ed, p++);
    if (ch == ' ' || ch == '\t')
      {
      insert(ed, ed->linepos + ed->col, &ch, 1);
      ed->col++;
      }
    else
      {
      break;
      }
    }
    */
  ed->lastcol = ed->col;

  ed->refresh = 1;

  if (ed->line >= ed->topline + ed->lines)
    {
    ed->toppos = next_line(ed, ed->toppos);
    ed->topline++;
    ed->refresh = 1;
    }

  adjust(ed);
  }

void backspace(editor_t *ed)
  {
  if (erase_selection(ed)) return;
  if (ed->linepos + ed->col == 0) return;
  if (ed->col == 0)
    {
    int pos = ed->linepos;
    erase(ed, --pos, 1);
    if (get(ed, pos - 1) == '\r') erase(ed, --pos, 1);

    ed->line--;
    ed->linepos = line_start(ed, pos);
    ed->col = pos - ed->linepos;
    ed->refresh = 1;

    if (ed->line < ed->topline)
      {
      ed->toppos = ed->linepos;
      ed->topline = ed->line;
      }
    }
  else
    {
    ed->col--;
    erase(ed, ed->linepos + ed->col, 1);
    ed->lineupdate = 1;
    }

  ed->lastcol = ed->col;
  adjust(ed);
  }

void del(editor_t *ed)
  {
  int pos, ch;

  if (erase_selection(ed)) return;
  pos = ed->linepos + ed->col;
  ch = get(ed, pos);
  if (ch < 0) return;

  erase(ed, pos, 1);
  if (ch == '\r')
    {
    ch = get(ed, pos);
    if (ch == '\n') erase(ed, pos, 1);
    }

  if (ch == '\n')
    {
    ed->refresh = 1;
    }
  else
    {
    ed->lineupdate = 1;
    }
  }

void indent(editor_t *ed, char *indentation)
  {
  int start, end, i, lines, toplines, newline, ch;
  char *buffer, *p;
  int buflen;
  int width = strlen(indentation);
  int pos = ed->linepos + ed->col;

  if (!get_selection(ed, &start, &end))
    {
    insert_char(ed, '\t');
    return;
    }

  lines = 0;
  toplines = 0;
  newline = 1;
  for (i = start; i < end; i++)
    {
    if (i == ed->toppos) toplines = lines;
    if (newline)
      {
      lines++;
      newline = 0;
      }
    if (get(ed, i) == '\n') newline = 1;
    }
  buflen = end - start + lines * width;
  buffer = kmalloc(buflen);
  if (!buffer) return;

  newline = 1;
  p = buffer;
  for (i = start; i < end; i++)
    {
    if (newline)
      {
      memcpy(p, indentation, width);
      p += width;
      newline = 0;
      }
    ch = get(ed, i);
    *p++ = ch;
    if (ch == '\n') newline = 1;
    }

  replace(ed, start, end - start, buffer, buflen, 1);
  kfree(buffer);

  if (ed->anchor < pos)
    {
    pos += width * lines;
    }
  else
    {
    ed->anchor += width * lines;
    }

  ed->toppos += width * toplines;
  ed->linepos = line_start(ed, pos);
  ed->col = ed->lastcol = pos - ed->linepos;

  adjust(ed);
  ed->refresh = 1;
  }

void unindent(editor_t *ed, char *indentation)
  {
  int start, end, i, newline, ch, shrinkage, topofs;
  char *buffer, *p;
  int width = strlen(indentation);
  int pos = ed->linepos + ed->col;

  if (!get_selection(ed, &start, &end)) return;

  buffer = kmalloc(end - start);
  if (!buffer) return;

  newline = 1;
  p = buffer;
  i = start;
  shrinkage = 0;
  topofs = 0;
  while (i < end)
    {
    if (newline)
      {
      newline = 0;
      if (compare(ed, indentation, i, width))
        {
        i += width;
        shrinkage += width;
        if (i < ed->toppos) topofs -= width;
        continue;
        }
      }
    ch = get(ed, i++);
    *p++ = ch;
    if (ch == '\n') newline = 1;
    }

  if (!shrinkage)
    {
    kfree(buffer);
    return;
    }

  replace(ed, start, end - start, buffer, p - buffer, 1);
  kfree(buffer);

  if (ed->anchor < pos)
    {
    pos -= shrinkage;
    }
  else
    {
    ed->anchor -= shrinkage;
    }

  ed->toppos += topofs;
  ed->linepos = line_start(ed, pos);
  ed->col = ed->lastcol = pos - ed->linepos;

  ed->refresh = 1;
  adjust(ed);
  }

void undo(editor_t *ed)
  {
  if (!ed->undo) return;
  moveto(ed, ed->undo->pos, 0);
  replace(ed, ed->undo->pos, ed->undo->inserted, ed->undo->undobuf, ed->undo->erased, 0);
  ed->undo = ed->undo->prev;
  if (!ed->undo) ed->dirty = 0;
  ed->anchor = -1;
  ed->lastcol = ed->col;
  ed->refresh = 1;
  }

void redo(editor_t *ed)
  {
  if (ed->undo)
    {
    if (!ed->undo->next) return;
    ed->undo = ed->undo->next;
    }
  else
    {
    if (!ed->undohead) return;
    ed->undo = ed->undohead;
    }
  replace(ed, ed->undo->pos, ed->undo->erased, ed->undo->redobuf, ed->undo->inserted, 0);
  moveto(ed, ed->undo->pos, 0);
  ed->dirty = 1;
  ed->anchor = -1;
  ed->lastcol = ed->col;
  ed->refresh = 1;
  }

//
// Clipboard
//

void copy_selection(editor_t *ed)
  {
  int selstart, selend;

  if (!get_selection(ed, &selstart, &selend))
    return;
  ed->clipsize = selend - selstart;
  ed->clipboard = (char *) krealloc(ed->clipboard, ed->clipsize);
  if (!ed->clipboard) return;
  copy(ed, ed->clipboard, selstart, ed->clipsize);
  }

void cut_selection(editor_t *ed)
  {
  copy_selection(ed);
  erase_selection(ed);
  }

void paste_selection(editor_t *ed)
  {
  erase_selection(ed);
  insert(ed, ed->linepos + ed->col, ed->clipboard, ed->clipsize);
  moveto(ed, ed->linepos + ed->col + ed->clipsize, 0);
  ed->refresh = 1;
  }

//
// Editor Commands
//

void save_editor(editor_t *ed)
  {
  result_t rc;

  if (!ed->dirty && !ed->newfile)
    return;

  rc = save_file(ed);
  if (rc < 0)
    display_message(ed, "Error saving document (%d)", rc);

  ed->refresh = 1;
  ed->dirty = false;
  }

bool close_editor(editor_t *ed)
  {
  if (ed->dirty)
    {
    display_message(ed, "Close without saving changes (y/n)? ");
    if (!ask(ed))
      {
      ed->refresh = 1;
      return false;
      }
    }

  return true;
  }

void find_text(editor_t *ed, int next)
  {
  int slen;

  if (!next)
    {
    if (!prompt(ed, "Find: ", 1))
      {
      ed->refresh = 1;
      return;
      }
    if (ed->search)
      kfree(ed->search);
    ed->search = kstrdup(ed->linebuf);
    }

  if (!ed->search) return;
  slen = strlen(ed->search);
  if (slen > 0)
    {
    char *match;

    close_gap(ed);
    match = strstr(ed->start + ed->linepos + ed->col, ed->search);
    if (match != 0)
      {
      int pos = match - ed->start;
      ed->anchor = pos;
      moveto(ed, pos + slen, 1);
      }
    else
      {
      outch(ed, '\007');
      }
    }
  ed->refresh = 1;
  }

void goto_line(editor_t *ed)
  {
  int lineno, l, pos;

  ed->anchor = -1;
  if (prompt(ed, "Goto line: ", 1))
    {
    lineno = atoi(ed->linebuf);
    if (lineno > 0)
      {
      pos = 0;
      for (l = 0; l < lineno - 1; l++)
        {
        pos = next_line(ed, pos);
        if (pos < 0) break;
        }
      }
    else
      {
      pos = -1;
      }

    if (pos >= 0)
      {
      moveto(ed, pos, 1);
      }
    else
      {
      outch(ed, '\007');
      }
    }
  ed->refresh = 1;
  }

void redraw_screen(editor_t *ed)
  {
  get_console_size(ed);
  draw_screen(ed);
  }

int quit(editor_t *ed)
  {
  if (ed->dirty)
    {
    display_message(ed, "Close without saving changes (y/n)? ");
    if (!ask(ed))
      return 0;
    }

  return 1;
  }

void help(editor_t *ed)
  {
  gotoxy(ed, 0, 0);
  clear_screen(ed);
  outstr(ed,"Editor Command Summary\r\n");
  outstr(ed, "======================\r\n\r\n");
  outstr(ed, "<up>         Move one line up (*)         \r\n");
  outstr(ed, "<down>       Move one line down (*)       \r\n");
  outstr(ed, "<left>       Move one character left (*)  Ctrl+S  Save file\r\n");
  outstr(ed, "<right>      Move one character right (*) Ctrl+W  Close file\r\n");
  outstr(ed, "<pgup>       Move one page up (*)         Ctrl+Q  Quit\r\n");
  outstr(ed, "<pgdn>       Move one page down (*)       \r\n");
  outstr(ed, "Ctrl+<left>  Move to previous word (*)    Ctrl+A  Select all\r\n");
  outstr(ed, "Ctrl+<right> Move to next word (*)        Ctrl+C  Copy selection to clipboard\r\n");
  outstr(ed, "<home>       Move to start of line (*)    Ctrl+X  Cut selection to clipboard\r\n");
  outstr(ed, "<end>        Move to end of line (*)      Ctrl+V  Paste from clipboard\r\n");
  outstr(ed, "Ctrl+<home>  Move to start of file (*)    Ctrl+Z  Undo\r\n");
  outstr(ed, "Ctrl+<end>   Move to end of file (*)      Ctrl+R  Redo\r\n");
  outstr(ed, "<backspace>  Delete previous character    Ctrl+F  Find text\r\n");
  outstr(ed, "<delete>     Delete current character     Ctrl+G  Find next\r\n");
  outstr(ed, "                                          Ctrl+L  Goto line\r\n");
  outstr(ed, "<tab>        Indent selection             F1      Help\r\n");
  outstr(ed, "Shift+<tab>  Unindent selection           F3      Navigate to file\r\n");
  outstr(ed, "                                          F5      Redraw screen\r\n");
  outstr(ed, "\r\n(*) Extends selection if combined with Shift");
  outstr(ed, "\r\nPress any key to continue...");

  getkey(ed);
  draw_screen(ed);
  draw_full_statusline(ed);
  }

/**
 * Edit a stream
 * @param stdin   stream to read console from
 * @param stdout  stream to write console to
 * @param stream  stream to read/write file to
 */
void muon_edit(handle_t stdin, handle_t stdout, const char *title, handle_t stream)
  {
  int rc;
  int i;

  editor_t *ed = (editor_t *) kmalloc(sizeof(editor_t));
  memset(ed, 0, sizeof(editor_t));
  ed->stdin = stdin;
  ed->stdout = stdout;
  ed->title = title;
  ed->anchor = -1;

  rc = load_file(ed, stream);

  get_console_size(ed);

  bool done = false;
  int key;

  ed->refresh = 1;
  while (!done)
    {
    if (ed->refresh)
      {
      draw_screen(ed);
      draw_full_statusline(ed);
      ed->refresh = 0;
      ed->lineupdate = 0;
      }
    else if (ed->lineupdate)
      {
      update_line(ed);
      ed->lineupdate = 0;
      draw_statusline(ed);
      }
    else
      {
      draw_statusline(ed);
      }

    position_cursor(ed);

    key = getkey(ed);

    if (key >= ' ' && key <= 0x7F)
      {
      insert_char(ed, (char) key);
      }
    else
      {
      switch (key)
        {
        case KEY_F1:
          help(ed);
          break;
        case KEY_F5:
          redraw_screen(ed);
          break;
       case KEY_UP:
         up(ed, 0);
          break;
        case KEY_DOWN:
          down(ed, 0);
          break;
        case KEY_LEFT:
          left(ed, 0);
          break;
        case KEY_RIGHT:
          right(ed, 0);
          break;
        case KEY_HOME:
          home(ed, 0);
          break;
        case KEY_END:
          end(ed, 0);
          break;
        case KEY_PGUP:
          pageup(ed, 0);
          break;
        case KEY_PGDN:
          pagedown(ed, 0);
          break;
        case KEY_CTRL_RIGHT:
          wordright(ed, 0);
          break;
        case KEY_CTRL_LEFT:
          wordleft(ed, 0);
          break;
        case KEY_CTRL_HOME:
          top(ed, 0);
          break;
        case KEY_CTRL_END:
          bottom(ed, 0);
          break;
        case KEY_SHIFT_UP:
          up(ed, 1);
          break;
        case KEY_SHIFT_DOWN:
          down(ed, 1);
          break;
        case KEY_SHIFT_LEFT:
          left(ed, 1);
          break;
        case KEY_SHIFT_RIGHT:
          right(ed, 1);
          break;
        case KEY_SHIFT_PGUP:
          pageup(ed, 1);
          break;
        case KEY_SHIFT_PGDN:
          pagedown(ed, 1);
          break;
        case KEY_SHIFT_HOME:
          home(ed, 1);
          break;
        case KEY_SHIFT_END:
          end(ed, 1);
          break;
        case KEY_SHIFT_CTRL_RIGHT:
          wordright(ed, 1);
          break;
        case KEY_SHIFT_CTRL_LEFT:
          wordleft(ed, 1);
          break;
        case KEY_SHIFT_CTRL_HOME:
          top(ed, 1);
          break;
        case KEY_SHIFT_CTRL_END:
          bottom(ed, 1);
          break;
        case ctrl('a'):
          select_all(ed);
          break;
        case ctrl('c'):
          copy_selection(ed);
          break;
        case ctrl('f'):
          find_text(ed, 0);
          break;
        case ctrl('l'):
          goto_line(ed);
          break;
        case ctrl('g'):
          find_text(ed, 1);
          break;
        case KEY_TAB:
          indent(ed, INDENT);
          break;
        case KEY_SHIFT_TAB:
          unindent(ed, INDENT);
          break;
        case KEY_ENTER:
          newline(ed);
          break;
        case KEY_BACKSPACE:
          backspace(ed);
          break;
        case KEY_DEL:
          del(ed);
          break;
        case ctrl('x'):
          cut_selection(ed);
          break;
        case ctrl('z'):
          undo(ed);
          break;
        case ctrl('r'):
          redo(ed);
          break;
        case ctrl('v'):
          paste_selection(ed);
          break;
        case ctrl('s'):
          save_editor(ed);
          break;
        case ctrl('q'):
          done = close_editor(ed);
          break;
        }
      }
    }

  gotoxy(ed, 0, ed->lines + 1);

  outstr(ed, RESET_COLOR CLREOL);

  if (ed->clipboard)
    kfree(ed->clipboard);

  if (ed->search)
    kfree(ed->search);

  if (ed->linebuf)
    kfree(ed->linebuf);

  clear_undo(ed);
  kfree(ed);
  }

