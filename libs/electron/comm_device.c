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
#include <stdlib.h>
#include <string.h>

#include "../neutron/neutron.h"

typedef struct _comm_rx_buffer_t {
  uint16_t version;           // reserved
  uint16_t len;
  byte_t data[];              // is len bytes long
} comm_rx_buffer_t;

typedef struct _comm_device_t {
  size_t version;
  semaphore_p tx_ready;
  semaphore_p rx_ready;
  semaphore_p event_handler;
  int hndl;
} comm_device_t;

static result_t check_handle(semaphore_p hndl)
  {
  if(hndl == 0 || ((comm_device_t *)hndl)->version != sizeof(comm_device_t))
    return e_bad_handle;

  return s_ok;
  }

const char *serial_device_s = "device";
const char *baud_rate_s = "baud-rate";

result_t comm_create_device(memid_t key, comm_device_p *device)
  {
  if(device == 0 || key == 0)
    return e_bad_parameter;

  comm_device_t *dev = (comm_device_t *)malloc(sizeof(comm_device_t));
  if(dev == 0)
    return e_not_enough_memory;

  // open the registry key and get the settings
  result_t result;
  char driver[REG_STRING_MAX+1];
  uint16_t len = REG_STRING_MAX +1;

  if(failed(result = reg_get_string(key, serial_device_s, driver, &len)))
    return result;

  // MUST be defined.
  uint32_t baud_rate;
  if(failed(result = reg_get_uint32(key, baud_rate_s, &baud_rate)))
    return result;

  memset(dev, 0, sizeof(comm_device_t));
  dev->version = sizeof(comm_device_t);
  dev->hndl = open(driver, O_RDWR | O_NOCTTY /*| O_NONBLOCK*/);

  *device = dev;

  // now we configure the device...
  comms_state_ioctl_t ioctl;
  memset(&ioctl, 0, sizeof(comms_state_ioctl_t));

  ioctl.ioctl.version = sizeof(comms_state_ioctl_t);
  ioctl.ioctl.ioctl_type = comms_state_ioctl;
  // TODO: more settings from the registry...
  ioctl.baud_rate = baud_rate;
  ioctl.stop_bits = two_stop_bit;
  ioctl.eof_char = '\n';

  return comm_ioctl(dev, set_device_ctl, &ioctl, sizeof(comms_state_ioctl_t), 0, 0, 0);
  }

result_t comm_close_device(comm_device_p device)
  {
  result_t result;

  close(device->hndl);
  semaphore_close(device->rx_ready);
  semaphore_close(device->tx_ready);

  return s_ok;

  }

result_t comm_write(comm_device_p device, const byte_t *data, uint16_t len, uint32_t timeout)
  {
  result_t result;

  int xfer_length;

  // perform a read file operation and process when done.
  if((xfer_length = write(device->hndl, data, len)) == -1)
    {
    if(errno != 0)
      return e_generic_error;
    }

  return s_ok;
  }

result_t comm_read(comm_device_p device, byte_t *data, uint16_t len, uint16_t *bytes_read, uint32_t timeout)
  {
  result_t result;

  int xfer_length;

  if(bytes_read != 0)
    *bytes_read = 0;

  // perform a read file operation and process when done.
  if(device->hndl <= 0 ||
      (xfer_length = read(device->hndl, data, len)) == 0)
    {
    if(device->hndl <= 0 || errno != 0)
      {
      return e_generic_error;
      }
    }

  if(bytes_read != 0)
    *bytes_read = xfer_length;

  return s_ok;
  }

result_t comm_ioctl(comm_device_p device, ioctl_type type, const void *in_buffer,
    uint16_t in_buffer_size, void *out_buffer, uint16_t out_buffer_size,
               uint16_t *size_returned)
  {
  result_t result;

  const comms_ioctl_t *comms_ioctl = (const comms_ioctl_t *)in_buffer;

  // determine type
  comms_ioctl_type_t op_type = comms_ioctl->ioctl_type;

  // decide what to do
  switch(op_type)
    {
    case comms_state_ioctl:
      if(comms_ioctl->version != sizeof(comms_state_ioctl_t) &&
          out_buffer_size != sizeof(comms_state_ioctl_t))
      {

      return e_bad_ioctl;
      }

      struct termios tio;
    tcgetattr(device->hndl, &tio);

//    memset(&tio, 0, sizeof(termios));

    if(type == set_device_ctl)
      {
      const comms_state_ioctl_t *set_comms_state_ioctl =
          (const comms_state_ioctl_t *)in_buffer;
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
//        tio.c_cflag |= CRTSCTS;
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

      tcflush(device->hndl, TCIFLUSH);

      if(tcsetattr(device->hndl, TCSANOW, &tio) < 0)
        return e_generic_error;
      }
    else
      {
      if(out_buffer_size != sizeof(comms_state_ioctl_t))
        return e_bad_ioctl;

      comms_state_ioctl_t *get_comms_state_ioctl = (comms_state_ioctl_t *) out_buffer;

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

    return s_ok;
  case comms_error_ioctl:
    if(comms_ioctl->version != sizeof(comms_error_ioctl_t) &&
        out_buffer_size != sizeof(comms_error_ioctl_t))
      {
      return e_bad_ioctl;
      }

    if(type == set_device_ctl)
      return e_invalid_operation;
    else
      {
      if(in_buffer_size < sizeof(comms_error_ioctl_t))
        {
        return e_ioctl_buffer_too_small;
        }
      comms_error_ioctl_t *get_error_state_ioctl = (comms_error_ioctl_t *) in_buffer;

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

      return s_ok;
      }

    break;
  case comms_escape_ioctl:
    if(comms_ioctl->version != sizeof(comms_escape_ioctl_t) &&
        out_buffer_size < sizeof(comms_escape_ioctl_t))
      {
      return e_bad_ioctl;
      }

    if(type == set_device_ctl)
      {
      const comms_escape_ioctl_t *comms_escape_ioctl = (const comms_escape_ioctl_t *)out_buffer;

//      *(ol->result) = EscapeCommFunction(_hndl, comms_escape_ioctl->escape_type) == 0 ? generic_error : s_ok;
      }
    else
      return e_invalid_operation;

    break;
  case comms_break_ioctl:
    if(comms_ioctl->version != sizeof(comms_break_ioctl_t) &&
        out_buffer_size < sizeof(comms_break_ioctl_t))
      {
      return e_bad_ioctl;
      }

    if(type == set_device_ctl)
      {
      const comms_break_ioctl_t *comms_break_ioctl = (const comms_break_ioctl_t *) out_buffer;

//      if(comms_break_ioctl->set_break)
//        *(ol->result) = SetCommBreak(_hndl) == 0 ? generic_error : s_ok;
//      else
//        *(ol->result) = ClearCommBreak(_hndl) == 0 ? generic_error : s_ok;
      }
    else
      return e_invalid_operation;

    break;
  case comms_event_mask_ioctl:
    if(comms_ioctl->version != sizeof(comms_event_mask_ioctl_t) &&
        out_buffer_size < sizeof(comms_event_mask_ioctl_t))
      {
      return e_bad_ioctl;
      }

    if(type == set_device_ctl)
      {
      const comms_event_mask_ioctl_t *set_comms_event_mask_ioctl = (const comms_event_mask_ioctl_t *) out_buffer;

      // get the event to be handled
      device->event_handler = set_comms_event_mask_ioctl->notification_event;

//      if(_event_handler == 0)
//        SetCommMask(_hndl, 0);
//      else
//        SetCommMask(_hndl, set_comms_event_mask_ioctl->mask);
      }
    else
      {
      if(in_buffer_size< sizeof(comms_event_mask_ioctl_t))
        {
        return e_bad_ioctl;
        }

      comms_event_mask_ioctl_t *get_comms_event_mask_ioctl =
          (comms_event_mask_ioctl_t *) in_buffer;

      get_comms_event_mask_ioctl->ioctl.version = sizeof(comms_event_mask_ioctl_t);
      get_comms_event_mask_ioctl->ioctl.ioctl_type = comms_event_mask_ioctl;
      get_comms_event_mask_ioctl->notification_event = device->event_handler;

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

    return s_ok;
    break;
  case comms_modem_status_ioctl:
    if(comms_ioctl->version != sizeof(comms_modem_status_ioctl_t) &&
        out_buffer_size < sizeof(comms_modem_status_ioctl_t))
      {
      return e_bad_ioctl;
      }

    if(type == set_device_ctl)
      return e_invalid_operation;
    else
      {
      comms_modem_status_ioctl_t *status_ioctl =(comms_modem_status_ioctl_t *) in_buffer;

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

      return s_ok;
      }

    break;
  case comms_timeouts_ioctl:
    if(comms_ioctl->version != sizeof(comms_timeouts_ioctl_t) &&
        out_buffer_size < sizeof(comms_timeouts_ioctl_t))
      {
      return e_bad_ioctl;
      }

    if(type == set_device_ctl)
      {
      const comms_timeouts_ioctl_t *timeouts_ioctl =(const comms_timeouts_ioctl_t *) out_buffer;

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

      return s_ok;
      }
    else
      {
      comms_timeouts_ioctl_t *timeouts_ioctl = (comms_timeouts_ioctl_t *) in_buffer;

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

      return s_ok;
      }

    break;
    }

  return e_invalid_operation;
  }
