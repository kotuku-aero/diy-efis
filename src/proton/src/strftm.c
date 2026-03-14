#include "../../include/sys_canfly.h"
#include <string.h>
#include <time.h>

extern int timezone;

static int weeknumber(const tm_t* timeptr, int firstweekday);

#define range(low, item, hi)	max(low, min(item, hi))

#undef min	/* just in case */

/* min --- return minimum of two numbers */

static inline int min(int a, int b)
  {
  return (a < b ? a : b);
  }

#undef max	/* also, just in case */

/* max --- return maximum of two numbers */

static inline int max(int a, int b)
  {
  return (a > b ? a : b);
  }

/* months_a --- return the short name for the month */

static const char* months_a(int index)
  {
  static const char* data[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
    };

  return data[index];
  }

/* months_l --- return the short name for the month */

static const char* months_l(int index)
  {
  static const char* data[] = {
    "January", "February", "March", "April",
    "May", "June", "July", "August", "September",
    "October", "November", "December",
    };

  return data[index];
  }

/* days_a --- return am/pm string */

static const char* ampm(int index)
  {
  static const char* data[] = {
    "AM", "PM",
    };

  return data[index];
  }

static inline uint32_t append(char* s, uint32_t maxsize, const char* tbuf)
  {
  uint32_t len = 0;

  while (maxsize > 1 && *tbuf != 0)
    {
    *s++ = *tbuf;
    tbuf++;
    maxsize--;
    len++;
    }

  *s = 0;
  return len;
  }

/**
 * @brief print a number as 2 digits
 * @param s         buffer
 * @param maxsize   length of buffer
 * @param number    number to print (0-99)
 * @return
 */
static uint32_t print_number(char* s, uint32_t maxsize, int number)
  {
  uint32_t len = 0;
  if (number == 0)      // special case
    {
    *s++ = '0';
    len++;
    }
  else
    {
    while (maxsize > 1 && number > 0)
      {
      *s = '0' + number % 10;
      number /= 10;
      s++;
      maxsize--;
      }
    }
  *s = 0;
  return len;
  }

/**
 * @brief print a number as 2 digits with leading 0
 * @param s         buffer
 * @param maxsize   length of buffer
 * @param number    number to print (0-99)
 * @return
 */
static uint32_t print_number_padded(char* s, uint32_t maxsize, int number, char pad)
  {
  uint32_t len = 0;
  if (number == 0)      // special case
    {
    *s++ = pad;
    *s++ = pad;
    len++;
    len++;
    }
  else
    {
    if(number < 10)
      {
      *s++ = pad;
      len++;
      }
    while (maxsize > 1 && number > 0)
      {
      *s = '0' + number % 10;
      number /= 10;
      s++;
      maxsize--;
      }
    }
  *s = 0;
  return len;
  }

/* strftime --- produce formatted time */

uint32_t strftm(char* s, uint32_t maxsize, const char* format, const tm_t* timeptr)
  {
  char* endp = s + maxsize;
  char* start = s;

  struct tm systime = {
    .tm_sec = timeptr->second,
    .tm_min = timeptr->minute,
    .tm_hour = timeptr->hour,
    .tm_mday = timeptr->day,
    .tm_mon = timeptr->month,
    .tm_year = timeptr->year - 1900,
    .tm_wday = -1,
    .tm_yday = -1,
    .tm_isdst = -1,
  };

  int i;

  if (s == nullptr || format == nullptr || timeptr == nullptr || maxsize == 0)
    return 0;

  /* quick check if we even need to bother */
  if (strchr(format, '%') == nullptr && strlen(format) + 1 >= maxsize)
    return 0;

  for (; *format && s < endp - 1; format++)
    {
    if (*format != '%')
      {
      *s++ = *format;
      continue;
      }

    switch (*++format)
      {
      case '\0':
        *s++ = '%';
        goto out;

      case '%':
        *s++ = '%';
        continue;
      case 'b':	/* abbreviated month name */
      short_month:
        s += append(s, endp - s, months_a(timeptr->month));
        break;
      case 'B':	/* full month name */
        s += append(s, endp - s, months_l(timeptr->month));
        break;

      case 'c':	/* appropriate date and time representation */
        /*
         * This used to be:
         *
         * strftime(tbuf, sizeof tbuf, "%a %b %e %H:%M:%S %Y", timeptr);
         *
         * Per the ISO 1999 C standard, it was this:
         * strftime(tbuf, sizeof tbuf, "%A %B %d %T %Y", timeptr);
         *
         * Per the ISO 2011 C standard, it is now this:
         */
        s += strftime(s, endp - s, "%a %b %e %T %Y", &systime);
        break;

      case 'C':
        s += print_number(s, endp - s, timeptr->year / 100);
        break;

      case 'd':	/* day of the month, 01 - 31 */
        s += print_number_padded(s, endp - s, timeptr->day, '0');
        break;

      case 'D':	/* date as %m/%d/%y */
        s += strftime(s, endp - s, "%m/%d/%y", &systime);
        break;

      case 'e':	/* day of month, blank padded */
        s += print_number_padded(s, endp - s, timeptr->day, ' ');
        break;

      case 'F':	/* ISO 8601 date representation */
        s += strftime(s, endp - s, "%Y-%m-%d", &systime);
        break;

      case 'h':	/* abbreviated month name */
        goto short_month;

      case 'H':	/* hour, 24-hour clock, 00 - 23 */
        s += print_number_padded(s, endp - s, timeptr->hour, '0');
        break;

      case 'I':	/* hour, 12-hour clock, 01 - 12 */
        i = timeptr->hour;
        if (i == 0)
          i = 12;
        else if (i > 12)
          i -= 12;

        s += print_number_padded(s, endp - s, i, '0');
        break;

      case 'm':	/* month, 01 - 12 */
        s += print_number_padded(s, endp - s, timeptr->month, '0');
        break;

      case 'M':	/* minute, 00 - 59 */
        s += print_number_padded(s, endp - s, timeptr->minute, '0');
        break;

      case 'p':	/* am or pm based on 12-hour clock */
        i = range(0, timeptr->hour, 23);
        if (i < 12)
          s += append(s, endp - s, ampm(0));
        else
          s += append(s, endp - s, ampm(1));
        break;

      case 'r':	/* time as %I:%M:%S %p */
        s += strftime(s, endp-s, "%I:%M:%S %p", &systime);
        break;

      case 'R':	/* time as %H:%M */
        s += strftime(s, endp - s, "%H:%M", &systime);
        break;

      case 'S':	/* second, 00 - 60 */
        i = range(0, timeptr->second, 60);
        s += print_number_padded(s, endp - s, i, '0');
        break;

      case 'T':	/* time as %H:%M:%S */
      the_time:
        s += strftime(s, endp - s, "%H:%M:%S", &systime);
        break;

      case 'x':	/* appropriate date representation */
        s += strftime(s, endp - s, "%m/%d/%y", &systime);
        break;

      case 'X':	/* appropriate time representation */
        goto the_time;
        break;

      case 'y':	/* year without a century, 00 - 99 */
        s += print_number_padded(s, endp - s, timeptr->year % 100, '0');
        break;

      case 'Y':	/* year with century */
        s += print_number(s, endp - s, timeptr->year/100);
        s += print_number_padded(s, endp - s, timeptr->year % 100, '0');
        break;
      default:
        s[0] = '%';
        s[1] = *format;
        s[2] = '\0';
        s += 2;
        break;
      }
    }
out:
  if (s < endp && *format == '\0') {
    *s = '\0';
    return (s - start);
    }

  return 0;
  }
