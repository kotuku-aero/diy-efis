/*
 * stream_printf.c
 *
 *  Created on: 8/06/2017
 *      Author: petern
 */

#include "neutron.h"

static void ftoa_fixed(char *buffer, float value);
static void ftoa_sci(char *buffer, float value);

static char *neutron_itoa(int64_t i, char *buf, uint16_t len)
  {
  char *p = buf;
  if(i < 0)
    {
    i = 0-i;
    *p++ = '-';
    len--;
    }

  if (i == 0)
    *p++ = '0';
  else
    {
    int64_t n = 1;

    while (i >= n)
      {
      n *= 10;
      if (n > UINT32_MAX)
        break;
      }

    // make == max digit
    n /= 10;

    while (len > 1 && n > 0)
      {
      int64_t digit = i / n;
      *p++ = '0' + digit;
      i -= digit * n;
      len--;
      n /= 10;
      }
    }

  *p = 0;

  return buf;
}

static char *neutron_itox(uint32_t value, char *buf, int len)
{
  char *p = buf; /* points to terminating '\0' */
  *p = 0;

  if(len < 19)
    return buf;

  *p++ = '0';
  *p++ = 'x';

  int i;
  for(i = 0; i < 8; i++)
    {
    uint32_t val = value >> 28;
    value <<= 4;
    val &= 0x0f;

    if(val > 9)
      *p++ = 'A' + (val -10);
    else
      *p++ = '0' + val;
    }


  *p = 0;
  return buf;
}

result_t stream_printf(handle_t stream, char const *fmt, ...)
  {
  va_list va;
  va_start(va, fmt);

  return stream_vprintf(stream, fmt, va);
  }

result_t stream_vprintf(handle_t stream, const char *fmt, va_list ap)
  {
  bool is_long = false;
  bool is_unsigned = false;
  uint16_t arg_num = 0;

  char ch;

  char buffer[512];

  while ((ch = *fmt++)!= 0)
    {
    if (is_long || is_unsigned || '%' == ch)
      {
      switch (ch = *fmt++)
        {
        case 'l' :
          is_long = true;
          fmt--;
          continue;
        case 'u' :
          is_unsigned = true;
          fmt--;
          continue;

        /* %% - print out a single %    */
        case '%':
          stream_putc(stream, '%');
          break;

        /* %c: print out a character    */
        case 'c':
          stream_putc(stream, (char) va_arg(ap, int));
          break;

        /* %s: print out a string       */
        case 's':
          stream_puts(stream, va_arg(ap, const char *));
          break;

        /* %d: print out an int         */
      case 'd':
          if(is_unsigned)
            neutron_itoa(va_arg(ap, uint32_t), buffer, 32);
          else
            neutron_itoa(va_arg(ap, int32_t), buffer, 32);

        stream_puts(stream, buffer);
        break;

        /* %x: print out an int in hex  */
      case 'x':
          neutron_itox(va_arg(ap, uint32_t), buffer, 32);

        stream_puts(stream, buffer);
        break;

      case 'e':
      case 'f':
        ftoa_fixed(buffer, (float)va_arg(ap, double));
        stream_puts(stream, buffer);
        break;
       }

      is_long = false;
      is_unsigned = false;
      }
    else
      stream_putc(stream, ch);
    }
  return s_ok;
  }

result_t stream_printf_cb(handle_t stream, const char *fmt, get_arg_fn cb, void *argv)
  {

  int16_t int16_temp;
  uint16_t uint16_temp;
  int32_t int32_temp;
  uint32_t uint32_temp;
  bool is_long = false;
  bool is_unsigned = false;
  int8_t char_temp;
  char *string_temp;
  float float_temp;
  uint16_t arg_num = 0;

  char ch;

  char buffer[512];

  while ((ch = *fmt++)!= 0)
    {
    if (is_long || is_unsigned || '%' == ch)
      {
      switch (ch = *fmt++)
        {
        case 'l' :
          is_long = true;
          continue;
        case 'u' :
          is_unsigned = true;
          continue;

        /* %% - print out a single %    */
        case '%':
          stream_putc(stream, '%');
          break;

        /* %c: print out a character    */
        case 'c':
          cb(arg_num++, argv, s_int8, &char_temp);
          stream_putc(stream, char_temp);
          break;

        /* %s: print out a string       */
        case 's':
          cb(arg_num++, argv, s_str, &string_temp);
          stream_puts(stream, string_temp);
          break;

        /* %d: print out an int         */
      case 'd':
        if(is_long)
          {
          if(is_unsigned)
            {
            cb(arg_num++, argv, s_uint32, &uint32_temp);
            neutron_itoa(uint32_temp, buffer, 32);
            }
          else
            {
            cb(arg_num++, argv, s_int32, &int32_temp);
            neutron_itoa(int32_temp, buffer, 320);
            }
          }
        else
          {
          if(is_unsigned)
            {
            cb(arg_num++, argv, s_uint16, &uint16_temp);
            neutron_itoa(uint16_temp, buffer, 320);
            }
          else
            {
            cb(arg_num++, argv, s_int16, &int16_temp);
            neutron_itoa(int16_temp, buffer, 32);
            }
          }

        stream_puts(stream, buffer);
        break;

        /* %x: print out an int in hex  */
      case 'x':
         if(is_long)
          {
          cb(arg_num++, argv, s_uint32, &uint32_temp);
          neutron_itox(uint32_temp, buffer, 32);
          }
        else
          {
          cb(arg_num++, argv, s_uint16, &uint16_temp);
          neutron_itox(uint16_temp, buffer, 32);
          }

        stream_puts(stream, buffer);
        break;

      case 'e':
      case 'f':
        cb(arg_num++, argv, s_float, &float_temp);

        ftoa_fixed(buffer, float_temp);
        stream_puts(stream, buffer);
        break;
       }

      is_long = false;
      is_unsigned = false;
      }
    else
      stream_putc(stream, ch);
    }
  return s_ok;
   }

result_t stream_scanf_cb(handle_t stream, const char *format, get_arg_fn cb, void *argv)
  {
  return e_not_implemented;
  }

static int normalize(float *val)
  {
  int exponent = 0;
  float value = *val;

  while (value >= 1.0)
    {
    value /= 10.0;
    ++exponent;
    }

  while (value < 0.1)
    {
    value *= 10.0;
    --exponent;
    }
  *val = value;
  return exponent;
  }

static void ftoa_fixed(char *buffer, float value)
  {
  /* carry out a fixed conversion of a float value to a string, with a precision of 5 decimal digits.
   * Values with absolute values less than 0.000001 are rounded to 0.0
   * Note: this blindly assumes that the buffer will be large enough to hold the largest possible result.
   * The largest value we expect is an IEEE 754 float precision real, with maximum magnitude of approximately
   * e+308. The C standard requires an implementation to allow a single conversion to produce up to 512
   * characters, so that's what we really expect as the buffer size.
   */

  int exponent = 0;
  int places = 0;
  static const int width = 4;

  if (value == 0.0)
    {
    buffer[0] = '0';
    buffer[1] = '\0';
    return;
    }

  if (value < 0.0)
    {
    *buffer++ = '-';
    value = -value;
    }

  exponent = normalize(&value);

  while (exponent > 0)
    {
    int digit = value * 10;
    *buffer++ = digit + '0';
    value = value * 10 - digit;
    ++places;
    --exponent;
    }

  if (places == 0)
    *buffer++ = '0';

  *buffer++ = '.';

  while (exponent < 0 && places < width)
    {
    *buffer++ = '0';
    --exponent;
    ++places;
    }

  while (places < width)
    {
    int digit = value * 10.0;
    *buffer++ = digit + '0';
    value = value * 10.0 - digit;
    ++places;
    }
  *buffer = '\0';
  }

static void ftoa_sci(char *buffer, float value)
  {
  int exponent = 0;
  int places = 0;
  static const int width = 4;

  if (value == 0.0)
    {
    buffer[0] = '0';
    buffer[1] = '\0';
    return;
    }

  if (value < 0.0)
    {
    *buffer++ = '-';
    value = -value;
    }

  exponent = normalize(&value);

  int digit = value * 10.0;
  *buffer++ = digit + '0';
  value = value * 10.0 - digit;
  --exponent;

  *buffer++ = '.';

  int i;
  for (i = 0; i < width; i++)
    {
    int digit = value * 10.0;
    *buffer++ = digit + '0';
    value = value * 10.0 - digit;
    }

  *buffer++ = 'e';
  neutron_itoa(exponent, buffer, 32);
  }
