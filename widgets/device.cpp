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
#include "device.h"
#include "hal.h"

#include <deque>

static std::deque<kotuku::event_t *> event_pool;
static kotuku::critical_section_t *_event_cs;

static kotuku::critical_section_t &event_pool_cs()
  {
  if(_event_cs == 0)
    _event_cs = new kotuku::critical_section_t();

  return *_event_cs;
  }

static kotuku::event_t *allocate_event()
  {
  kotuku::critical_section_t::lock_t lock(event_pool_cs());

  kotuku::event_t *allocated_event = 0;
  if(event_pool.size() > 0)
    {
    allocated_event = event_pool.front();
    event_pool.pop_front();
    }
  else
    allocated_event = new kotuku::event_t();

  return allocated_event;
  }

static void release_event(kotuku::event_t *evt)
  {
  kotuku::critical_section_t::lock_t lock(event_pool_cs());

  event_pool.push_back(evt);
  }

kotuku::device_t::device_t()
  {
  }

kotuku::device_t::~device_t()
  {
  }

result_t kotuku::device_t::close()
  {
  return device_close();
  }

result_t kotuku::device_t::ioctl(ioctl_type type, const void *in_buffer,
    size_t in_buffer_size, void *out_buffer, size_t out_buffer_size,
    size_t *size_returned, event_t *completion, result_t *rslt)
  {
  // must be both there if an overlapped operation
  if(completion != 0 && rslt == 0)
    return e_bad_parameter;

  overlapped_t overlapped;

  overlapped.action.ioctl.ioctl = type;
  overlapped.action.ioctl.read_ptr = out_buffer;
  overlapped.action.ioctl.read_size = out_buffer_size;
  overlapped.action.ioctl.write_ptr = in_buffer;
  overlapped.action.ioctl.write_size = in_buffer_size;
  overlapped.action.ioctl.xfer_length = 0;
  overlapped.operation = overlapped_t::ioctl_opn;

  result_t result;
  if(rslt == 0)
    rslt = &result;

  overlapped.result = rslt;

  // overlapped is always used
  if(completion == 0)
    {
    overlapped.completion = allocate_event();

    if(!device_request(&overlapped))
      event_t::lock_t lock(*overlapped.completion);

    release_event(overlapped.completion);
    overlapped.completion = 0;

    return s_ok;
    }

  overlapped.completion = completion;

  if(device_request(&overlapped))
    return s_ok;

  return e_operation_pending;
  }

result_t kotuku::device_t::read(void *buf, size_t len, size_t *read,
    event_t *completion, result_t *rslt, overlapped_t **olp)
  {
  bool can_overlap = olp != 0;

  result_t result;

  if(read != 0)
    *read = 0;

  overlapped_t *ol = new overlapped_t;
  if(can_overlap)
    *olp = ol;

  ol->action.read.read_ptr = buf;
  ol->action.read.read_size = len;
  ol->action.read.xfer_length = read;

  if(rslt == 0)
    ol->result = &result;
  else
    ol->result = rslt;

  ol->operation = overlapped_t::read_opn;

  if(completion == 0 || !can_overlap)
    {
    ol->completion = allocate_event();
    if(!device_request(ol))
      event_t::lock_t lock(*ol->completion);

    release_event(ol->completion);
    ol->completion = 0;

    result_t rslt = *(ol->result);

    if(!can_overlap)
      delete ol;

    return rslt;
    }
  else
    {
    ol->completion = completion;

    if(device_request(ol))
      return *(ol->result);

    return e_operation_pending;
    }
  }

result_t kotuku::device_t::write(const void *buf, size_t len, size_t *written,
    event_t *completion, result_t *rslt, overlapped_t **olp)
  {
  bool can_overlap = olp != 0;

  result_t result = s_ok;

  overlapped_t *ol = new overlapped_t;
  if(can_overlap)
    *olp = ol;

  ol->action.write.write_ptr = buf;
  ol->action.write.write_size = len;
  ol->action.write.xfer_length = written;

  if(rslt == 0)
    ol->result = &result;
  else
    ol->result = rslt;

  ol->operation = overlapped_t::write_opn;

  if(completion == 0 || !can_overlap)
    {
    ol->completion = allocate_event();
    if(!device_request(ol))
      event_t::lock_t lock(*ol->completion);

    release_event(ol->completion);
    ol->completion = 0;

    result_t rslt = *(ol->result);

    if(!can_overlap)
      delete ol;

    return rslt;
    }
  else
    {
    ol->completion = completion;

    if(device_request(ol))
      return *(ol->result);

    return e_operation_pending;
    }
  }

result_t kotuku::device_t::seek(seek_type where, size_t posn, size_t *np,
    event_t *completion, result_t *rslt, overlapped_t **olp)
  {
  bool can_overlap = olp != 0;

  result_t result;

  overlapped_t *ol = new overlapped_t;
  if(can_overlap)
    *olp = ol;

  ol->action.seek.where = where;
  ol->action.seek.posn = posn;
  ol->action.seek.new_posn = np;
  ol->operation = overlapped_t::seek_opn;

  if(rslt == 0)
    ol->result = &result;
  else
    ol->result = rslt;

  if(completion == 0 || !can_overlap)
    {
    ol->completion = allocate_event();
    if(!device_request(ol))
      event_t::lock_t lock(*ol->completion);

    release_event(ol->completion);
    ol->completion = 0;

    result_t rslt = *(ol->result);

    if(!can_overlap)
      delete ol;

    return rslt;
    }
  else
    {
    ol->completion = completion;

    if(device_request(ol))
      return *(ol->result);

    return e_operation_pending;
    }
  }

result_t kotuku::device_t::cancel_io()
  {
  return device_cancel_io();
  }

kotuku::device_t::overlapped_t::overlapped_t() :
    completion(0), os_data(0), result(0)
  {
  }

kotuku::device_t::overlapped_t::~overlapped_t()
  {
  delete (uint8_t *) os_data;
  }
