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
#include "time.h"
#include "hal.h"
#include "application.h"

kotuku::date_time_t::date_time_t()
  {
  _t = application_t::hal->now();
  }

kotuku::date_time_t::date_time_t(uint16_t year,
                       month_type month,
                       uint8_t day,
                       uint8_t hour,
                       uint8_t minute,
                       uint8_t second,
                       uint16_t milliseconds,
                       uint32_t nanoseconds)
  {
  _t = application_t::hal->mktime(year, month, day, hour, minute, second, milliseconds, nanoseconds);
  }

uint16_t kotuku::date_time_t::day_of_year() const
  {
  static uint16_t days[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
  static uint16_t leap_days[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };

  uint16_t year;
  month_type month;
  uint8_t day;
  gmtime(&year, &month, &day);

  bool leap_year = (year % 4) == 0;
  if((year % 400) != 0 && (year % 100) == 0)
    leap_year = false;

  if(leap_year)
    return leap_days[month-1] + day -1;

  return days[month-1] + day -1;
  }

uint8_t kotuku::date_time_t::days_in_month() const
  {
  static uint8_t days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  static uint8_t leap_days[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

  uint16_t year;
  month_type month;

  gmtime(&year, &month);

  bool leap_year = (year % 4) == 0;
  if((year % 400) != 0 && (year % 100) == 0)
    leap_year = false;

  if(leap_year)
    return leap_days[month-1];
  
  return days[month-1];
  }

void kotuku::date_time_t::gmtime(uint16_t *year,
                            month_type *month,
                            uint8_t *day,
                            uint8_t *hour,
                            uint8_t *minute,
                            uint8_t *second,
                            uint16_t *milliseconds,
                            uint32_t *nanoseconds) const
  {
  if(month != 0)
    *month = (month_type)0;

  application_t::hal->gmtime(_t, year, reinterpret_cast<uint8_t *>(month), day, hour, minute, second, milliseconds, nanoseconds);
  }

