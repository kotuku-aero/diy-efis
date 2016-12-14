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
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "comm_device.h"
#include "../gdi-lib/device.h"

kotuku::comm_device_t::comm_device_t(const char *driver)
  {
  _hndl = ::open(driver, O_RDWR | O_NOCTTY /*| O_NONBLOCK*/);
  // todo: hanldle errors
  }

result_t kotuku::comm_device_t::device_close()
  {
  ::close(_hndl);

  return s_ok;
  }

bool kotuku::comm_device_t::device_request(device_t::overlapped_t *ol)
  {
  switch(ol->operation)
    {
  case device_t::overlapped_t::ioctl_opn:
    return process_ioctl(ol);
  case device_t::overlapped_t::read_opn:
    return process_read(ol);
  case device_t::overlapped_t::write_opn:
    return process_write(ol);
    }

  *(ol->result) = e_invalid_operation;

  return true;
  }

bool kotuku::comm_device_t::process_ioctl(device_t::overlapped_t *ol)
  {
  const comms_ioctl_t *comms_ioctl =
      reinterpret_cast<const comms_ioctl_t *>(ol->action.ioctl.write_ptr);

  // determine type
  comms_ioctl_type_t op_type = comms_ioctl->ioctl_type;

  // decide what to do
  switch(op_type)
    {
  case comms_state_ioctl:
    if(comms_ioctl->version != sizeof(comms_state_ioctl_t)
        && ol->action.ioctl.write_size != sizeof(comms_state_ioctl_t))
      {
      *(ol->result) = e_ioctl_version_error;
      return true;
      }

    termios tio;
    tcgetattr(_hndl, &tio);

//    memset(&tio, 0, sizeof(termios));

    if(ol->action.ioctl.ioctl == device_t::set_device_ctl)
      {
      const comms_state_ioctl_t *set_comms_state_ioctl =
          reinterpret_cast<const comms_state_ioctl_t *>(ol->action.ioctl.write_ptr);
      //
      // Input flags - Turn off input processing
      // convert break to null byte, no CR to NL translation,
      // no NL to CR translation, don't mark parity errors or breaks
      // no input parity check, don't strip high bit off,
      // no XON/XOFF software flow control
      //
      tio.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP
          | IXON);
      //
      // Output flags - Turn off output processing
      // no CR to NL translation, no NL to CR-NL translation,
      // no NL to CR translation, no column 0 CR suppression,
      // no Ctrl-D suppression, no fill characters, no case mapping,
      // no local output processing
      //
      tio.c_oflag &= ~(OCRNL | ONLCR | ONLRET | ONOCR | OFILL | OLCUC | OPOST);
      //
      // No line processing:
      // echo off, echo newline off, canonical mode off,
      // extended input processing off, signal chars off
      //
      tio.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
      //
      // Turn off character processing
      // clear current char size mask, no parity checking,
      // no output processing, force 8 bit input
      //
      tio.c_cflag &= ~(CSIZE | PARENB);
      tio.c_cflag |= CS8;
      //
      // One input byte is enough to return from read()
      // Inter-character timer off
      //
      tio.c_cc[VMIN] = 1;
      tio.c_cc[VTIME] = 0;
      //tio.c_cc[VEOL] = '\r';

      tio.c_cflag |= CREAD;
      speed_t speed;
      switch(set_comms_state_ioctl->baud_rate)
        {
      case cbr_110:
        speed = B110;
        break;
      case cbr_300:
        speed = B300;
        break;
      case cbr_600:
        speed = B600;
        break;
      case cbr_1200:
        speed = B1200;
        break;
      case cbr_2400:
        speed = B2400;
        break;
      case cbr_4800:
        speed = B4800;
        break;
      case cbr_9600:
        speed = B9600;
        break;
      case cbr_19200:
        speed = B19200;
        break;
      case cbr_38400:
        speed = B38400;
        break;
      case cbr_57600:
        speed = B57600;
        break;
      case cbr_115200:
        speed = B115200;
        break;
      default:
        return e_bad_parameter;
        }
      cfsetospeed(&tio, speed);
      cfsetispeed(&tio, speed);

//      tio.ByteSize = set_comms_state_ioctl->byte_size;
//      tio.EofChar = set_comms_state_ioctl->eof_char;
//      tio.ErrorChar = set_comms_state_ioctl->error_character;
//      tio.EvtChar = set_comms_state_ioctl->event_char;
//      tio.fAbortOnError = set_comms_state_ioctl->abort_on_error ? 1 : 0;
//      tio.fBinary = set_comms_state_ioctl->binary_mode ? 1 : 0;
//      tio.fDsrSensitivity = set_comms_state_ioctl->dsr_in_enabled ? 1 : 0;

//      switch(set_comms_state_ioctl->dtr_mode)
//        {
//        case dtr_disabled :
//          tio.fDtrControl = DTR_CONTROL_DISABLE;
//          break;
//        case dtr_enabled :
//          tio.fDtrControl = DTR_CONTROL_ENABLE;
//          break;
//        case dtr_flowcontrol :
//          tio.fDtrControl = DTR_CONTROL_HANDSHAKE;
//          break;
//        }
//
//      tio.fErrorChar = set_comms_state_ioctl->parity_character_enabled;
      tio.c_iflag |= set_comms_state_ioctl->xoff_in_enabled ? IXOFF : 0;
//      tio.fNull = set_comms_state_ioctl->ignore_null_characters;
      tio.c_iflag |= set_comms_state_ioctl->xoff_out_enabled ? IXON : 0;
//      tio.fOutxCtsFlow = set_comms_state_ioctl->cts_out_enabled;
//      tio.fOutxDsrFlow = set_comms_state_ioctl->dsr_out_enabled;
//      tio.fParity = set_comms_state_ioctl->parity_checking_enabled ? 1 : 0;
//
      switch(set_comms_state_ioctl->parity)
        {
      case no_parity:
        break;
      case odd_parity:
        tio.c_cflag |= PARENB | PARODD;
        break;
      case even_parity:
        tio.c_cflag |= PARENB;
        break;
      case space_parity:
//          tio.Parity = SPACEPARITY;
        break;
      case mark_parity:
//          tio.Parity = MARKPARITY;
        break;
        }

      switch(set_comms_state_ioctl->rts_control)
        {
      case rts_control_disabled:
        tio.c_cflag |= CLOCAL;
        break;
      case rts_control_enabled:
        tio.c_cflag |= CRTSCTS;
        break;
      case rts_control_handshake:
//          tio.fRtsControl = RTS_CONTROL_HANDSHAKE;
        break;
      case rts_control_toggle:
//          tio.fRtsControl = RTS_CONTROL_TOGGLE;
        break;
        }

//      tio.fTXContinueOnXoff = set_comms_state_ioctl->xmit_continue_on_xoff ? 1 : 0;
      tio.c_cflag |= set_comms_state_ioctl->stop_bits == 2 ? CSTOPB : 0;
//      tio.XoffChar = set_comms_state_ioctl->xoff_char;
//      tio.XoffLim = set_comms_state_ioctl->xoff_limit;
//      tio.XonChar = set_comms_state_ioctl->xon_char;
//      tio.XonLim = set_comms_state_ioctl->xon_limit;

      tcflush(_hndl, TCIFLUSH);

      if(tcsetattr(_hndl, TCSANOW, &tio) < 0)
        {
        *(ol->result) = e_generic_error;
        return true;
        }
      }
    else
      {
      if(ol->action.ioctl.read_size != sizeof(comms_state_ioctl_t))
        {
        *(ol->result) = e_ioctl_too_small;
        return true;
        }

      comms_state_ioctl_t *get_comms_state_ioctl =
          reinterpret_cast<comms_state_ioctl_t *>(ol->action.ioctl.read_ptr);

//      GetCommState(_hndl, &tio);
//
//      get_comms_state_ioctl->abort_on_error = tio.fAbortOnError != 0;
//      get_comms_state_ioctl->baud_rate = (baud_rate) tio.BaudRate;
//      get_comms_state_ioctl->binary_mode = tio.fBinary != 0;
//      get_comms_state_ioctl->byte_size = tio.ByteSize;
//      get_comms_state_ioctl->cts_out_enabled = tio.fOutxCtsFlow != 0;
//      get_comms_state_ioctl->dsr_in_enabled = tio.fDsrSensitivity != 0;
//      get_comms_state_ioctl->dsr_out_enabled = tio.fOutxDsrFlow != 0;
//
//      switch(tio.fDtrControl)
//        {
//        case DTR_CONTROL_DISABLE :
//          get_comms_state_ioctl->dtr_mode = dtr_disabled;
//          break;
//        case DTR_CONTROL_ENABLE :
//          get_comms_state_ioctl->dtr_mode = dtr_enabled;
//          break;
//        case DTR_CONTROL_HANDSHAKE :
//          get_comms_state_ioctl->dtr_mode = dtr_flowcontrol;
//          break;
//        }
//
//      get_comms_state_ioctl->eof_char = tio.EofChar;
//      get_comms_state_ioctl->error_character = tio.ErrorChar;
//      get_comms_state_ioctl->event_char = tio.EvtChar;
//      get_comms_state_ioctl->ignore_null_characters = tio.fNull != 0;
//
//      switch(tio.Parity)
//        {
//        case EVENPARITY :
//          get_comms_state_ioctl->parity = even_parity;
//          break;
//        case ODDPARITY :
//          get_comms_state_ioctl->parity = odd_parity;
//          break;
//        case MARKPARITY :
//          get_comms_state_ioctl->parity = mark_parity;
//          break;
//        case SPACEPARITY :
//          get_comms_state_ioctl->parity = space_parity;
//          break;
//        case NOPARITY :
//          get_comms_state_ioctl->parity = no_parity;
//          break;
//        }
//
//      get_comms_state_ioctl->parity_character = tio.ErrorChar;
//      get_comms_state_ioctl->parity_character_enabled = tio.fErrorChar;
//      get_comms_state_ioctl->parity_checking_enabled = tio.fParity != 0;
//
//      switch(tio.fRtsControl)
//        {
//      case RTS_CONTROL_DISABLE :
//        get_comms_state_ioctl->rts_control = rts_control_disabled;
//        break;
//      case RTS_CONTROL_ENABLE :
//        get_comms_state_ioctl->rts_control = rts_control_enabled;
//        break;
//      case RTS_CONTROL_HANDSHAKE :
//        get_comms_state_ioctl->rts_control = rts_control_handshake;
//        break;
//      case RTS_CONTROL_TOGGLE :
//        get_comms_state_ioctl->rts_control = rts_control_toggle;
//        break;
//        }
//
//      get_comms_state_ioctl->stop_bits = (stop_bits) tio.StopBits;
//      get_comms_state_ioctl->xmit_continue_on_xoff = tio.fTXContinueOnXoff != 0;
//      get_comms_state_ioctl->xoff_char = tio.XoffChar;
//      get_comms_state_ioctl->xoff_in_enabled = tio.fInX != 0;
//      get_comms_state_ioctl->xoff_limit = tio.XoffLim;
//      get_comms_state_ioctl->xoff_out_enabled = tio.fOutX != 0;
//      get_comms_state_ioctl->xon_char = tio.XonChar;
//      get_comms_state_ioctl->xon_limit = tio.XonLim;
      }

    *(ol->result) = s_ok;
    break;
  case comms_error_ioctl:
    if(comms_ioctl->version != sizeof(comms_error_ioctl_t)
        && ol->action.ioctl.write_size != sizeof(comms_error_ioctl_t))
      {
      *(ol->result) = e_ioctl_version_error;
      return true;
      }

    if(ol->action.ioctl.ioctl == device_t::set_device_ctl)
      *(ol->result) = e_invalid_operation;
    else
      {
      if(ol->action.ioctl.read_size < sizeof(comms_error_ioctl_t))
        {
        *(ol->result) = e_ioctl_too_small;
        return true;
        }
      comms_error_ioctl_t *get_error_state_ioctl =
          reinterpret_cast<comms_error_ioctl_t *>(ol->action.ioctl.read_ptr);

//      DWORD errors;
//      if(ClearCommError(_hndl, &errors, 0)== 0)
//        *(ol->result) = generic_error;
//      else
//        {
//        get_error_state_ioctl->version = sizeof(comms_error_ioctl_t);
//        get_error_state_ioctl->ioctl_type = comms_error_ioctl;
//        get_error_state_ioctl->break_char = (errors & CE_BREAK) != 0;
//        get_error_state_ioctl->framing_error = (errors & CE_FRAME) != 0;
//        get_error_state_ioctl->overflow = (errors & CE_RXOVER) != 0;
//        get_error_state_ioctl->overrun = (errors & CE_OVERRUN) != 0;
//        get_error_state_ioctl->parity = (errors & CE_RXPARITY) != 0;
//        }

      *(ol->result) = s_ok;
      }

    break;
  case comms_escape_ioctl:
    if(comms_ioctl->version != sizeof(comms_escape_ioctl_t)
        && ol->action.ioctl.write_size < sizeof(comms_escape_ioctl_t))
      {
      *(ol->result) = e_ioctl_version_error;
      return true;
      }

    if(ol->action.ioctl.ioctl == device_t::set_device_ctl)
      {
      const comms_escape_ioctl_t *comms_escape_ioctl =
          reinterpret_cast<const comms_escape_ioctl_t *>(ol->action.ioctl.write_ptr);

//      *(ol->result) = EscapeCommFunction(_hndl, comms_escape_ioctl->escape_type) == 0 ? generic_error : s_ok;
      }
    else
      *(ol->result) = e_invalid_operation;

    break;
  case comms_break_ioctl:
    if(comms_ioctl->version != sizeof(comms_break_ioctl_t)
        && ol->action.ioctl.write_size < sizeof(comms_break_ioctl_t))
      {
      *(ol->result) = e_ioctl_version_error;
      return true;
      }

    if(ol->action.ioctl.ioctl == device_t::set_device_ctl)
      {
      const comms_break_ioctl_t *comms_break_ioctl =
          reinterpret_cast<const comms_break_ioctl_t *>(ol->action.ioctl.write_ptr);

//      if(comms_break_ioctl->set_break)
//        *(ol->result) = SetCommBreak(_hndl) == 0 ? generic_error : s_ok;
//      else
//        *(ol->result) = ClearCommBreak(_hndl) == 0 ? generic_error : s_ok;
      }
    else
      *(ol->result) = e_invalid_operation;

    break;
  case comms_event_mask_ioctl:
    if(comms_ioctl->version != sizeof(comms_event_mask_ioctl_t)
        && ol->action.ioctl.write_size < sizeof(comms_event_mask_ioctl_t))
      {
      *(ol->result) = e_ioctl_version_error;
      return true;
      }

    if(ol->action.ioctl.ioctl == device_t::set_device_ctl)
      {
      const comms_event_mask_ioctl_t *set_comms_event_mask_ioctl =
          reinterpret_cast<const comms_event_mask_ioctl_t *>(ol->action.ioctl.write_ptr);

      // get the event to be handled
      _event_handler = set_comms_event_mask_ioctl->notification_event;

//      if(_event_handler == 0)
//        SetCommMask(_hndl, 0);
//      else
//        SetCommMask(_hndl, set_comms_event_mask_ioctl->mask);
      }
    else
      {
      if(ol->action.ioctl.read_size < sizeof(comms_event_mask_ioctl_t))
        {
        *(ol->result) = e_ioctl_too_small;
        return true;
        }

      comms_event_mask_ioctl_t *get_comms_event_mask_ioctl =
          reinterpret_cast<comms_event_mask_ioctl_t *>(ol->action.ioctl.read_ptr);

      get_comms_event_mask_ioctl->version = sizeof(comms_event_mask_ioctl_t);
      get_comms_event_mask_ioctl->ioctl_type = comms_event_mask_ioctl;
      get_comms_event_mask_ioctl->notification_event = _event_handler;

//      DWORD mask = 0;
//      if(GetCommMask(_hndl, &mask))
//        get_comms_event_mask_ioctl->mask = (comms_event_mask_ioctl_t::event_mask_t)mask;
//      else
//        {
//        get_comms_event_mask_ioctl->mask = 0;
//        *(ol->result) = generic_error;
//        return true;
//        }
      }

    *(ol->result) = s_ok;
    break;
  case comms_modem_status_ioctl:
    if(comms_ioctl->version != sizeof(comms_modem_status_ioctl_t)
        && ol->action.ioctl.write_size < sizeof(comms_modem_status_ioctl_t))
      {
      *(ol->result) = e_ioctl_version_error;
      return true;
      }

    if(ol->action.ioctl.ioctl == device_t::set_device_ctl)
      *(ol->result) = e_invalid_operation;
    else
      {
      comms_modem_status_ioctl_t *status_ioctl =
          reinterpret_cast<comms_modem_status_ioctl_t *>(ol->action.ioctl.read_ptr);

//      DWORD status;
//      if(GetCommModemStatus(_hndl, &status) == 0)
//        {
//        *(ol->result) = generic_error;
//        return true;
//        }
//
//      status_ioctl->version = sizeof(comms_modem_status_ioctl_t);
//      status_ioctl->cts_on = (status & MS_CTS_ON) != 0;
//      status_ioctl->dsr_on = (status & MS_DSR_ON) != 0;
//      status_ioctl->ring_on = (status & MS_RING_ON) != 0;
//      status_ioctl->rlsd_on = (status & MS_RLSD_ON) != 0;

      *(ol->result) = s_ok;
      }

    break;
  case comms_timeouts_ioctl:
    if(comms_ioctl->version != sizeof(comms_timeouts_ioctl_t)
        && ol->action.ioctl.write_size < sizeof(comms_timeouts_ioctl_t))
      {
      *(ol->result) = e_ioctl_version_error;
      return true;
      }

    if(ol->action.ioctl.ioctl == device_t::set_device_ctl)
      {
      const comms_timeouts_ioctl_t *timeouts_ioctl =
          reinterpret_cast<const comms_timeouts_ioctl_t *>(ol->action.ioctl.write_ptr);

//      COMMTIMEOUTS timeouts;
//
//      timeouts.ReadIntervalTimeout = timeouts_ioctl->read_interval_timeout;
//      timeouts.ReadTotalTimeoutConstant = timeouts_ioctl->read_total_timeout_constant;
//      timeouts.ReadTotalTimeoutMultiplier = timeouts_ioctl->read_total_timeout_multiplier;
//      timeouts.WriteTotalTimeoutConstant = timeouts_ioctl->write_total_timeout_constant;
//      timeouts.WriteTotalTimeoutMultiplier = timeouts_ioctl->write_total_timeout_multiplier;
//
//      if(SetCommTimeouts(_hndl, &timeouts) == 0)
//        {
//        *(ol->result) = generic_error;
//        return true;
//        }

      *(ol->result) = s_ok;
      }
    else
      {
      comms_timeouts_ioctl_t *timeouts_ioctl =
          reinterpret_cast<comms_timeouts_ioctl_t *>(ol->action.ioctl.read_ptr);

//      COMMTIMEOUTS timeouts;
//      if(GetCommTimeouts(_hndl, &timeouts) == 0)
//        {
//        *(ol->result) = generic_error;
//        return true;
//        }
//
//      timeouts_ioctl->ioctl_type = comms_timeouts_ioctl;
//      timeouts_ioctl->version = sizeof(comms_timeouts_ioctl_t);
//      timeouts_ioctl->read_interval_timeout = timeouts.ReadIntervalTimeout;
//      timeouts_ioctl->read_total_timeout_constant = timeouts.ReadTotalTimeoutConstant;
//      timeouts_ioctl->read_total_timeout_multiplier = timeouts.ReadTotalTimeoutMultiplier;
//      timeouts_ioctl->write_total_timeout_constant = timeouts.WriteTotalTimeoutConstant;
//      timeouts_ioctl->write_total_timeout_multiplier = timeouts.WriteTotalTimeoutMultiplier;

      *(ol->result) = s_ok;
      }

    break;
    }

  return true;
  }

bool kotuku::comm_device_t::process_read(device_t::overlapped_t *ol)
  {
  int xfer_length;

  if(ol->action.read.xfer_length != 0)
    *ol->action.read.xfer_length = 0;

  // perform a read file operation and process when done.
  if(_hndl <= 0
      || (xfer_length = ::read(_hndl, ol->action.read.read_ptr,
          ol->action.read.read_size)) == 0)
    {
    if(_hndl <= 0 || errno != 0)
      {
      int err_value = errno;
      *(ol->result) = e_generic_error;
      return true;
      }
    *(ol->result) = e_operation_pending;
    }
  else
    {
    if(ol->action.read.xfer_length != 0)
      *ol->action.read.xfer_length = xfer_length;

    if(xfer_length == 0)
      *(ol->result) = e_timeout_error;
    else
      *(ol->result) = s_ok;

    return true;
    }

  return false;
  }

bool kotuku::comm_device_t::process_write(device_t::overlapped_t *ol)
  {
  int xfer_length;

  // perform a read file operation and process when done.
  if((xfer_length = ::write(_hndl, ol->action.write.write_ptr,
      ol->action.write.write_size)) == -1)
    {
    if(errno != 0)
      {
      *(ol->result) = e_generic_error;
      return true;
      }
    *(ol->result) = e_operation_pending;
    }
  else
    {
    if(ol->action.write.xfer_length != 0)
      *ol->action.write.xfer_length = xfer_length;

    if(xfer_length == 0)
      *(ol->result) = e_timeout_error;
    else
      *(ol->result) = s_ok;

    return true;
    }

  return false;
  }

result_t kotuku::comm_device_t::device_cancel_io()
  {
  return s_ok;
  }

void kotuku::comm_device_t::set_comm_state(int baud_rate)
  {
//  termios tio;
//  memset(&tio, 0, sizeof(termios));
//  tio.DCBlength = sizeof(termios);
//
//  tio.BaudRate = baud_rate;
//  tio.ByteSize = 8;
//  tio.fBinary = 1;
//  tio.StopBits = 1;
//
//  //BuildCommDCB(state, &tio);
//
//  SetCommState(_hndl, &tio);
  }

//void kotuku::comm_device_t::send_msg(const unsigned char *msg, size_t length)
//  {
//  device_t::overlapped_t ol;
//  size_t written;
//  result_t result;
//
//  ol.action.write.write_ptr = msg;
//  ol.action.write.write_size = length;
//  ol.action.write.xfer_length = &written;
//  ol.result = &result;
//  ol.operation = device_t::overlapped_t::write_opn;
//
//  event_t evt;
//  ol.completion = &evt;
//  if(!process_write(&ol))
//    event_t::lock_t lock(evt);
//  }
