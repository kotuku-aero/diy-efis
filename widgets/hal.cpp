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
#include "hal.h"
#include "assert.h"
#include "window.h"
#include "application.h"

#include <vector>
#include <algorithm>
#include <limits>

#include "ini.h"

kotuku::hal_t::~hal_t()
 {
 }

kotuku::hal_t::hal_t()
  {
  }

kotuku::hal_t::time_base_t kotuku::hal_t::now()
 {

 return kotuku::hal_t::time_base_t();
 }

inline kotuku::hal_t::time_base_t scale(kotuku::hal_t::time_base_t val, size_t factor)
 {
 return val << factor;
 }

// this will work for 100 years. Since this is an embedded app
// we only need to worry about the year 2000
inline bool is_leap_year(uint16_t year)
 {
 return (year & 3) == 0 &&
     year != 2000;
 }

static const uint8_t months[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static const uint8_t months_ly[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static const uint16_t days[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
static const uint16_t days_ly[] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };

// this is the number of days since 1/1/1970. (min time ordinal)
// only works until 1/1/2070
static const uint16_t year_days[] = 
 {
 0, 366, 731, 1096, 1461, 1827, 2192, 2557, 2922, 3288,
 3653, 4018, 4383, 4749, 5114, 5479, 5844, 6210, 6575, 6940,
 7305, 7671, 8036, 8401, 8766, 9132, 9497, 9862, 10227, 10593,
 10958, 11323, 11688, 12054, 12419, 12784, 13149, 13515, 13880, 14245,
 14610, 14976, 15341, 15706, 16071, 16437, 16802, 17167, 17532, 17898,
 18263, 18628, 18993, 19359, 19724, 20089, 20454, 20820, 21185, 21550,
 21915, 22281, 22646, 23011, 23376, 23742, 24107, 24472, 24837, 25203,
 25568, 25933, 26298, 26664, 27029, 27394, 27759, 28125, 28490, 28855,
 29220, 29586, 29951, 30316, 30681, 31047, 31412, 31777, 32142, 32508,
 32873, 33238, 33603, 33969, 34334, 34699, 35064, 35430, 35795, 36160,
 36526
 };

// make time from the values passed
kotuku::hal_t::time_base_t kotuku::hal_t::mktime(uint16_t year,
                uint8_t month,
                uint8_t day,
                uint8_t hour,
                uint8_t minute,
                uint8_t second,
                uint16_t milliseconds,
                uint32_t nanoseconds)
 {
 assert(year >= 1970 && year < 2070);
 assert(month > 0 && month < 13);

 // add the number of days
 bool leap_year = is_leap_year(year);

 assert(day > 0 && day < (leap_year ? days_ly[month-1] : days[month-1]));
 assert(hour > 0 && hour < 25);
 assert(minute > 0 && minute < 60);
 assert(second > 0 && second < 60);
 assert(milliseconds < 1000);
 assert(nanoseconds < 1000000);

 // we use a int64_t that is the number of milliseconds
 time_base_t result = months[year - 1970];

 if(leap_year)
  result += days_ly[month -1];
 else
  result += days[month-1];

 result += day -1;

 // multiply the day by 24 to get hours
 result = scale(result, 4) + scale(result, 3);

 // add the hour
 result += hour -1;

 // multiply by 60 minutes as x * 32 + x * 16 + x * 8 + x * 4
 result = scale(result, 5) + scale(result, 4) + scale(result, 3) + scale(result, 2);

 // add the minute
 result += minute -1;

 // multiply by 60 seconds
 result = scale(result, 5) + scale(result, 4) + scale(result, 3) + scale(result, 2);

 // add seconds
 result += second -1;

 // multiply by 1000 to get milliseconds x * 512 + x * 256 + x * 128 + x * 64 + x * 32 + x * 8
 result = scale(result, 9) + scale(result, 8) + scale(result, 7) + scale(result, 6) + scale(result, 5) + scale(result, 3);

 // add the milliseconds
 result += milliseconds;

 // multiply by 10000 to get number of 100 nsec intervals
 // x * 8192 + x * 1024 + x * 512 + x * 256 + x * 16
 result = scale(result, 12) + scale(result, 10) + scale(result, 9) + scale(result, 8) + scale(result, 4);

 return result;
 }

static size_t find_value(const uint16_t *values, size_t elems, kotuku::hal_t::time_base_t when)
 {
 size_t first = 0;
 size_t last = elems;
 size_t mid;
 while(first <= last)
  {
  mid = (first + last) >> 1;
  if(when >= values[mid] && when < values[mid-1])
   break;
  else if(when >= values[mid+1])
   first = mid + 1;
  else if(when < values[mid])
   last = mid -1;
  }

 return mid;
 }

// split a time value
void kotuku::hal_t::gmtime(kotuku::hal_t::time_base_t when,
             uint16_t *year,
             uint8_t *month,
             uint8_t *day,
             uint8_t *hour,
             uint8_t *minute,
             uint8_t *second,
             uint16_t *milliseconds,
             uint32_t *nanoseconds)
 {
 // no fast way to do this.
 if(nanoseconds != 0)
  *nanoseconds = (when % 10000) * 100;

 when /= 10000;

 if(milliseconds != 0)
  *milliseconds = when % 1000;

 when /= 1000;

 if(second != 0)
  *second = when % 60;

 when /= 60;

 if(minute != 0)
  *minute = when % 60;

 when /= 60;

 if(hour != 0)
  *hour = when % 24;

 when /= 24;

 // result is number of days since 1970
 // calculate the year by looking though the months list
 size_t year_index = find_value(year_days, 100, when);

 // mid is the year and base date
 year_index += 1970;

 if(year != 0)
  *year = year_index;

 // remove the days so we just have the number of days left
 when -= months[year_index];

 bool leap_year = is_leap_year(year_index);

 size_t day_index = find_value(leap_year ? days_ly : days, 12, when);

 if(month != 0)
  *month = day_index + 1;

 if(leap_year)
  when -= days_ly[day_index];
 else
  when -= days[day_index];

 if(day != 0)
  *day = (uint8_t) when;
 }

long interlocked_decrement(volatile long &value)
  {
  return kotuku::application_t::instance->hal()->interlocked_decrement(value);
  }

long interlocked_increment(volatile long &value)
  {
  return kotuku::application_t::instance->hal()->interlocked_increment(value);
  }

result_t kotuku::hal_t::initialize(const char *ini_file)
  {
  if(ini_file == 0)
	  return e_bad_parameter;

  // open the settings file
  ini_parse(ini_file, ini_parser, this);

  return s_ok;
  }

static std::string make_section_name(const char *section, const char *name)
  {
  std::string result;
  result.append("[");
  if(section != 0)
    result.append(section);
  result.append("]");
  result.append(name);

  return result;
  }

int kotuku::hal_t::ini_parser(void* user, const char* section, const char* name, const char* value)
  {
  hal_t *hal = reinterpret_cast<hal_t *>(user);

  std::string section_name(section);
  std::vector<std::string>::iterator it = std::find(hal->_section_names.begin(), hal->_section_names.end(), section_name);

  if(it == hal->_section_names.end())
    hal->_section_names.push_back(section_name);

  // add the settings (section not used yet).
  hal->_ini_values.insert(
      std::make_pair(make_section_name(section, name), std::string(value)));

  return 0;
  }

result_t kotuku::hal_t::get_config_value(const char *section, const char *name, std::string &out_value) const
  {
  ini_values_t::const_iterator it = _ini_values.find(make_section_name(section, name));
  if(it == _ini_values.end())
    return e_path_not_found;

  out_value = (*it).second;

  return s_ok;
  }

result_t kotuku::hal_t::get_config_value(const char *section, const char *name, int &out_value) const
  {
  ini_values_t::const_iterator it = _ini_values.find(make_section_name(section, name));
  if(it == _ini_values.end())
    return e_path_not_found;

  out_value = atoi((*it).second.c_str());

  return s_ok;
  }

result_t kotuku::hal_t::get_config_value(const char *section, const char *name, double &out_value) const
  {
  ini_values_t::const_iterator it = _ini_values.find(make_section_name(section, name));
  if(it == _ini_values.end())
    return e_path_not_found;

  out_value = atof((*it).second.c_str());

  return s_ok;
  }

result_t kotuku::hal_t::get_config_value(const char *section, const char *name, bool &out_value) const
  {
  ini_values_t::const_iterator it = _ini_values.find(make_section_name(section, name));
  if(it == _ini_values.end())
    return e_path_not_found;

  out_value = (*it).second == "y" || (*it).second == "yes"
      || (*it).second == "1" || (*it).second=="true";

  return s_ok;
  }

result_t kotuku::hal_t::get_config_value(const char *section, const char *name,
    std::vector<uint8_t> &out_value) const
  {
  result_t rslt = e_path_not_found;

  return rslt;
  }

