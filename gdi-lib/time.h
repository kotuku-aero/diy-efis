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
#ifndef __time_h__
#define __time_h__

// the internal form of time is the number of 100 ns since 1-jan-1601
// we allow for the easy conversion of numbers using
// these constants

#include "stddef.h"

namespace kotuku {

  typedef int64_t interval_t;
  static const interval_t interval_per_second = 10000000;
  static const interval_t interval_per_minute = interval_per_second * 60;
  static const interval_t interval_per_hour = interval_per_minute * 60;
  static const interval_t interval_per_day = interval_per_hour * 24;

// number to add to a date_time_t to convert to an internal time.
  static const interval_t time_t_to_datum = (((1970 - 1601) / 4) - // number of leap years
      3 +                      // 1700, 1800, 1900 not leap years
      (365 * (1970 - 1601)))      // all other days
  * interval_per_day;

// Utility class to encapsulate a time value
  class date_time_t
    {
  public:
    // values returned by day_of_week
    enum day_type
      {
      Sunday,
      Monday,
      Tuesday,
      Wednesday,
      Thursday,
      Friday,
      Saturday
      };

    // calues returned by month
    enum month_type
      {
      January = 1,
      February,
      March,
      April,
      May,
      June,
      July,
      August,
      September,
      October,
      November,
      December
      };

    // values used to offset date_time_t(s)
    enum
      {
      _minute = 60,
      _hour = 3600,
      _day = 86400,
      _week = 604800
      };

    date_time_t();
    explicit date_time_t(uint16_t year, month_type month,
        uint8_t day, uint8_t hour = 0, uint8_t minute = 0,
        uint8_t second = 0, uint16_t millisecond = 0,
        uint32_t nanoseconds = 0);
    date_time_t(const date_time_t &d);
    date_time_t(interval_t);

    void operator =(const date_time_t& d);

    bool operator ==(const date_time_t& d) const;
    bool operator !=(const date_time_t& d) const;
    bool operator <(const date_time_t& d) const;
    bool operator >(const date_time_t& d) const;
    bool operator <=(const date_time_t& d) const;
    bool operator >=(const date_time_t& d) const;

    friend interval_t operator -(const date_time_t &t1, const date_time_t &t);

    date_time_t operator +(interval_t) const;
    date_time_t operator -(interval_t) const;
    date_time_t operator +(int32_t seconds) const;
    date_time_t operator -(int32_t seconds) const;

    void operator +=(interval_t);
    void operator -=(interval_t);
    void operator +=(int32_t seconds);
    void operator -=(int32_t seconds);

    uint16_t year() const;
    month_type month() const;
    uint8_t day() const;
    bool is_dst() const;
    uint32_t sec() const;
    uint16_t day_of_year() const;
    uint8_t minute() const;
    uint8_t hour() const;
    uint8_t second() const;
    day_type day_of_week() const;
    uint8_t days_in_month() const;
    uint16_t milliseconds() const;
    uint32_t nanoseconds() const;

    operator bool() const;
    bool is_leap_year() const;

    void gmtime(uint16_t *year = 0, month_type *month = 0,
        uint8_t *day = 0, uint8_t *hour = 0, uint8_t *minute =
            0, uint8_t *second = 0, uint16_t *milliseconds = 0,
        uint32_t *nanoseconds = 0) const;
  private:
    interval_t _t;
    };

////////////////////////////////////////////////////////////////////////////////
// Inlines

  inline date_time_t::date_time_t(const date_time_t& d) :
      _t(d._t)
    {
    }

  inline date_time_t::date_time_t(interval_t t) :
      _t(t)
    {
    }

  inline date_time_t::operator bool() const
    {
    return _t == ~0;
    }

  inline bool date_time_t::operator==(const date_time_t& d) const
    {
    return _t == d._t;
    }

  inline bool date_time_t::operator!=(const date_time_t& d) const
    {
    return _t != d._t;
    }

  inline bool date_time_t::operator<(const date_time_t& d) const
    {
    return _t < d._t;
    }

  inline bool date_time_t::operator>(const date_time_t& d) const
    {
    return _t > d._t;
    }

  inline bool date_time_t::operator<=(const date_time_t& d) const
    {
    return _t <= d._t;
    }

  inline bool date_time_t::operator>=(const date_time_t& d) const
    {
    return _t >= d._t;
    }

  inline void date_time_t::operator=(const date_time_t& d)
    {
    _t = d._t;
    }

  inline date_time_t date_time_t::operator+(interval_t interval) const
    {
    return date_time_t(_t + interval);
    }

  inline date_time_t date_time_t::operator-(interval_t interval) const
    {
    return date_time_t(_t - interval);
    }

  inline date_time_t date_time_t::operator+(int32_t _seconds) const
    {
    return operator +(_seconds * static_cast<int64_t>(interval_per_second));
    }

  inline date_time_t date_time_t::operator-(int32_t _seconds) const
    {
    return operator -(_seconds * static_cast<int64_t>(interval_per_second));
    }

  inline interval_t operator-(const date_time_t &t1, const date_time_t &t)
    {
    return t1._t - t._t;
    }

  inline void date_time_t::operator+=(interval_t interval)
    {
    _t += interval;
    }

  inline void date_time_t::operator-=(interval_t interval)
    {
    _t -= interval;
    }

  inline void date_time_t::operator+=(int32_t _seconds)
    {
    operator +=(_seconds * interval_per_second);
    }

  inline void date_time_t::operator-=(int32_t _seconds)
    {
    operator -=(_seconds * interval_per_second);
    }

  inline bool date_time_t::is_leap_year() const
    {
    uint16_t _year = year();
    if((_year % 400) == 0)
      return true;

    if((_year % 100) == 0)
      return false;

    return ((_year % 4) == 0);
    }

  inline uint16_t date_time_t::year() const
    {
    uint16_t v;
    gmtime(&v);
    return v;
    }

  inline date_time_t::month_type date_time_t::month() const
    {
    month_type v;
    gmtime(0, &v);
    return v;
    }

  inline uint8_t date_time_t::day() const
    {
    uint8_t v;
    gmtime(0, 0, &v);
    return v;
    }

  inline uint8_t date_time_t::hour() const
    {
    uint8_t v;
    gmtime(0, 0, 0, &v);
    return v;
    }

  inline uint8_t date_time_t::minute() const
    {
    uint8_t v;
    gmtime(0, 0, 0, 0, &v);
    return v;
    }

  inline uint8_t date_time_t::second() const
    {
    uint8_t v;
    gmtime(0, 0, 0, 0, 0, &v);
    return v;
    }

  inline uint16_t date_time_t::milliseconds() const
    {
    uint16_t v;
    gmtime(0, 0, 0, 0, 0, 0, &v);
    return v;
    }

  inline uint32_t date_time_t::nanoseconds() const
    {
    uint32_t v;
    gmtime(0, 0, 0, 0, 0, 0, 0, &v);
    return v;
    }

  inline date_time_t::day_type date_time_t::day_of_week() const
    {
    // 1-jan-1601 was a monday so we add 1 to the number of days
    return day_type(((_t / interval_per_day) + 1) % 7);
    }

  };

#endif
