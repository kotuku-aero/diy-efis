#include "comm_device.h"
#include "../../gdi-lib/device.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Comms driver

kotuku::comm_device_t::comm_device_t(const char *name, const char *device_name)
  : _hndl(CreateFileW(to_wstring(device_name), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0 /*FILE_FLAG_OVERLAPPED*/, NULL)),
  hal_device_t(name),
  _event_handler(0)
  {
  // set some timeouts
  COMMTIMEOUTS timeouts;

  timeouts.ReadIntervalTimeout = 50 ;  // 50msec
  timeouts.ReadTotalTimeoutConstant = 50;
  timeouts.ReadTotalTimeoutMultiplier = 10;
  timeouts.WriteTotalTimeoutConstant = 50;
  timeouts.WriteTotalTimeoutMultiplier = 10;

  DWORD result = SetCommTimeouts(_hndl, &timeouts);
  }

// structure used to extend the system OVERLAPPED struct with extra information to allow the
// comm routine to work.
struct OVERLAPPEDEX : public OVERLAPPED
  {
  kotuku::device_t::overlapped_t *ol;
  };

void kotuku::comm_device_t::device_close()
  {
  CloseHandle(_hndl);
  }

bool kotuku::comm_device_t::device_request(device_t::overlapped_t *ol)
  {
  switch(ol->operation)
    {
    case device_t::overlapped_t::ioctl_opn :
      return process_ioctl(ol);
    case device_t::overlapped_t::read_opn :
      return process_read(ol);
    case device_t::overlapped_t::write_opn :
      return process_write(ol);
    }

  *(ol->result) = e_invalid_operation;

  return true;
  }

bool kotuku::comm_device_t::process_ioctl(device_t::overlapped_t *ol)
  {
  const comms_ioctl_t *comms_ioctl = reinterpret_cast<const comms_ioctl_t *>(ol->action.ioctl.write_ptr);

  // determine type
  comms_ioctl_type_t op_type = comms_ioctl->ioctl_type;

  // decide what to do
  switch(op_type)
    {
  case comms_state_ioctl :
    if(comms_ioctl->version != sizeof(comms_state_ioctl_t) &&
      ol->action.ioctl.write_size != sizeof(comms_state_ioctl_t))
      {
      *(ol->result) = e_ioctl_version_error;
      return true;
      }

    DCB dcb;
    memset(&dcb, 0, sizeof(DCB));
    dcb.DCBlength = sizeof(DCB);

    if(ol->action.ioctl.ioctl == device_t::set_device_ctl)
      {
      const comms_state_ioctl_t *set_comms_state_ioctl = reinterpret_cast<const comms_state_ioctl_t *>(ol->action.ioctl.write_ptr);

      dcb.BaudRate = set_comms_state_ioctl->baud_rate;
      dcb.ByteSize = set_comms_state_ioctl->byte_size;
      dcb.EofChar = set_comms_state_ioctl->eof_char;
      dcb.ErrorChar = set_comms_state_ioctl->error_character;
      dcb.EvtChar = set_comms_state_ioctl->event_char;
      dcb.fAbortOnError = set_comms_state_ioctl->abort_on_error ? 1 : 0;
      dcb.fBinary = set_comms_state_ioctl->binary_mode ? 1 : 0;
      dcb.fDsrSensitivity = set_comms_state_ioctl->dsr_in_enabled ? 1 : 0;

      switch(set_comms_state_ioctl->dtr_mode)
        {
        case dtr_disabled :
          dcb.fDtrControl = DTR_CONTROL_DISABLE;
          break;
        case dtr_enabled :
          dcb.fDtrControl = DTR_CONTROL_ENABLE;
          break;
        case dtr_flowcontrol :
          dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
          break;
        }

      dcb.fErrorChar = set_comms_state_ioctl->parity_character_enabled;
      dcb.fInX = set_comms_state_ioctl->xoff_in_enabled;
      dcb.fNull = set_comms_state_ioctl->ignore_null_characters;
      dcb.fOutX = set_comms_state_ioctl->xoff_out_enabled;
      dcb.fOutxCtsFlow = set_comms_state_ioctl->cts_out_enabled;
      dcb.fOutxDsrFlow = set_comms_state_ioctl->dsr_out_enabled;
      dcb.fParity = set_comms_state_ioctl->parity_checking_enabled ? 1 : 0;

      switch(set_comms_state_ioctl->parity)
        {
        case no_parity :
          dcb.Parity = NOPARITY;
          break;
        case odd_parity :
          dcb.Parity = ODDPARITY;
          break;
        case even_parity :
          dcb.Parity = EVENPARITY;
          break;
        case space_parity :
          dcb.Parity = SPACEPARITY;
          break;
        case mark_parity :
          dcb.Parity = MARKPARITY;
          break;
        }

      switch(set_comms_state_ioctl->rts_control)
        {
        case rts_control_disabled :
          dcb.fRtsControl = RTS_CONTROL_DISABLE;
          break;
        case rts_control_enabled :
          dcb.fRtsControl = RTS_CONTROL_ENABLE;
          break;
        case rts_control_handshake :
          dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
          break;
        case rts_control_toggle :
          dcb.fRtsControl = RTS_CONTROL_TOGGLE;
          break;
        }

      dcb.fTXContinueOnXoff = set_comms_state_ioctl->xmit_continue_on_xoff ? 1 : 0;
      dcb.StopBits = set_comms_state_ioctl->stop_bits;
      dcb.XoffChar = set_comms_state_ioctl->xoff_char;
      dcb.XoffLim = set_comms_state_ioctl->xoff_limit;
      dcb.XonChar = set_comms_state_ioctl->xon_char;
      dcb.XonLim = set_comms_state_ioctl->xon_limit;

      if(!SetCommState(_hndl, &dcb))
        {
        *(ol->result) = e_bad_parameter;
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

      comms_state_ioctl_t *get_comms_state_ioctl = reinterpret_cast<comms_state_ioctl_t *>(ol->action.ioctl.read_ptr);

      if(!GetCommState(_hndl, &dcb))
        {
        *(ol->result) = e_bad_parameter;
        return true;
        }

      get_comms_state_ioctl->abort_on_error = dcb.fAbortOnError != 0;
      get_comms_state_ioctl->baud_rate = (baud_rate_t) dcb.BaudRate;
      get_comms_state_ioctl->binary_mode = dcb.fBinary != 0;
      get_comms_state_ioctl->byte_size = dcb.ByteSize;
      get_comms_state_ioctl->cts_out_enabled = dcb.fOutxCtsFlow != 0;
      get_comms_state_ioctl->dsr_in_enabled = dcb.fDsrSensitivity != 0;
      get_comms_state_ioctl->dsr_out_enabled = dcb.fOutxDsrFlow != 0;

      switch(dcb.fDtrControl)
        {
        case DTR_CONTROL_DISABLE :
          get_comms_state_ioctl->dtr_mode = dtr_disabled;
          break;
        case DTR_CONTROL_ENABLE :
          get_comms_state_ioctl->dtr_mode = dtr_enabled;
          break;
        case DTR_CONTROL_HANDSHAKE :
          get_comms_state_ioctl->dtr_mode = dtr_flowcontrol;
          break;
        }

      get_comms_state_ioctl->eof_char = dcb.EofChar;
      get_comms_state_ioctl->error_character = dcb.ErrorChar;
      get_comms_state_ioctl->event_char = dcb.EvtChar;
      get_comms_state_ioctl->ignore_null_characters = dcb.fNull != 0;

      switch(dcb.Parity)
        {
        case EVENPARITY :
          get_comms_state_ioctl->parity = even_parity;
          break;
        case ODDPARITY :
          get_comms_state_ioctl->parity = odd_parity;
          break;
        case MARKPARITY :
          get_comms_state_ioctl->parity = mark_parity;
          break;
        case SPACEPARITY :
          get_comms_state_ioctl->parity = space_parity;
          break;
        case NOPARITY :
          get_comms_state_ioctl->parity = no_parity;
          break;
        }

      get_comms_state_ioctl->parity_character = dcb.ErrorChar;
      get_comms_state_ioctl->parity_character_enabled = dcb.fErrorChar;
      get_comms_state_ioctl->parity_checking_enabled = dcb.fParity != 0;

      switch(dcb.fRtsControl)
        {
      case RTS_CONTROL_DISABLE :
        get_comms_state_ioctl->rts_control = rts_control_disabled;
        break;
      case RTS_CONTROL_ENABLE :
        get_comms_state_ioctl->rts_control = rts_control_enabled;
        break;
      case RTS_CONTROL_HANDSHAKE :
        get_comms_state_ioctl->rts_control = rts_control_handshake;
        break;
      case RTS_CONTROL_TOGGLE :
        get_comms_state_ioctl->rts_control = rts_control_toggle;
        break;
        }

      get_comms_state_ioctl->stop_bits = (stop_bits_t) dcb.StopBits;
      get_comms_state_ioctl->xmit_continue_on_xoff = dcb.fTXContinueOnXoff != 0;
      get_comms_state_ioctl->xoff_char = dcb.XoffChar;
      get_comms_state_ioctl->xoff_in_enabled = dcb.fInX != 0;
      get_comms_state_ioctl->xoff_limit = dcb.XoffLim;
      get_comms_state_ioctl->xoff_out_enabled = dcb.fOutX != 0;
      get_comms_state_ioctl->xon_char = dcb.XonChar;
      get_comms_state_ioctl->xon_limit = dcb.XonLim;
      }

    *(ol->result) = s_ok;
    break;
  case comms_error_ioctl :
    if(comms_ioctl->version != sizeof(comms_error_ioctl_t) &&
      ol->action.ioctl.write_size != sizeof(comms_error_ioctl_t))
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
      comms_error_ioctl_t *get_error_state_ioctl = reinterpret_cast<comms_error_ioctl_t *>(ol->action.ioctl.read_ptr);

      DWORD errors;
      if(ClearCommError(_hndl, &errors, NULL)== 0)
        *(ol->result) = e_generic_error;
      else
        {
        get_error_state_ioctl->version = sizeof(comms_error_ioctl_t);
        get_error_state_ioctl->ioctl_type = comms_error_ioctl;
        get_error_state_ioctl->break_char = (errors & CE_BREAK) != 0;
        get_error_state_ioctl->framing_error = (errors & CE_FRAME) != 0;
        get_error_state_ioctl->overflow = (errors & CE_RXOVER) != 0;
        get_error_state_ioctl->overrun = (errors & CE_OVERRUN) != 0;
        get_error_state_ioctl->parity = (errors & CE_RXPARITY) != 0;
        }

      *(ol->result) = s_ok;
      }

    break;
  case comms_escape_ioctl :
    if(comms_ioctl->version != sizeof(comms_escape_ioctl_t) &&
      ol->action.ioctl.write_size < sizeof(comms_escape_ioctl_t))
      {
      *(ol->result) = e_ioctl_version_error;
      return true;
      }

    if(ol->action.ioctl.ioctl == device_t::set_device_ctl)
      {
      const comms_escape_ioctl_t *comms_escape_ioctl = reinterpret_cast<const comms_escape_ioctl_t *>(ol->action.ioctl.write_ptr);

      *(ol->result) = EscapeCommFunction(_hndl, comms_escape_ioctl->escape_type) == 0 ? e_generic_error : s_ok;
      }
    else
      *(ol->result) = e_invalid_operation;

    break;
  case comms_break_ioctl :
    if(comms_ioctl->version != sizeof(comms_break_ioctl_t) &&
      ol->action.ioctl.write_size < sizeof(comms_break_ioctl_t))
      {
      *(ol->result) = e_ioctl_version_error;
      return true;
      }

    if(ol->action.ioctl.ioctl == device_t::set_device_ctl)
      {
      const comms_break_ioctl_t *comms_break_ioctl = reinterpret_cast<const comms_break_ioctl_t *>(ol->action.ioctl.write_ptr);

      if(comms_break_ioctl->set_break)
        *(ol->result) = SetCommBreak(_hndl) == 0 ? e_generic_error : s_ok;
      else
        *(ol->result) = ClearCommBreak(_hndl) == 0 ? e_generic_error : s_ok;
      }
    else
      *(ol->result) = e_invalid_operation;

    break;
  case comms_event_mask_ioctl :
    if(comms_ioctl->version != sizeof(comms_event_mask_ioctl_t) &&
      ol->action.ioctl.write_size < sizeof(comms_event_mask_ioctl_t))
      {
      *(ol->result) = e_ioctl_version_error;
      return true;
      }

    if(ol->action.ioctl.ioctl == device_t::set_device_ctl)
      {
      const comms_event_mask_ioctl_t *set_comms_event_mask_ioctl = reinterpret_cast<const comms_event_mask_ioctl_t *>(ol->action.ioctl.write_ptr);

      // get the event to be handled
      _event_handler = set_comms_event_mask_ioctl->notification_event;

      if(_event_handler == 0)
        SetCommMask(_hndl, 0);
      else
        SetCommMask(_hndl, set_comms_event_mask_ioctl->mask);
      }
    else
      {
      if(ol->action.ioctl.read_size < sizeof(comms_event_mask_ioctl_t))
        {
        *(ol->result) = e_ioctl_too_small;
        return true;
        }

      comms_event_mask_ioctl_t *get_comms_event_mask_ioctl = reinterpret_cast<comms_event_mask_ioctl_t *>(ol->action.ioctl.read_ptr);

      get_comms_event_mask_ioctl->version = sizeof(comms_event_mask_ioctl_t);
      get_comms_event_mask_ioctl->ioctl_type = comms_event_mask_ioctl;
      get_comms_event_mask_ioctl->notification_event = _event_handler;

      DWORD mask = 0;
      if(GetCommMask(_hndl, &mask))
        get_comms_event_mask_ioctl->mask = (comms_event_mask_ioctl_t::event_mask_t)mask;
      else
        {
        get_comms_event_mask_ioctl->mask = 0;
        *(ol->result) = e_generic_error;
        return true;
        }
      }

    *(ol->result) = s_ok;
    break;
  case comms_modem_status_ioctl :
    if(comms_ioctl->version != sizeof(comms_modem_status_ioctl_t) &&
      ol->action.ioctl.write_size < sizeof(comms_modem_status_ioctl_t))
      {
      *(ol->result) = e_ioctl_version_error;
      return true;
      }

    if(ol->action.ioctl.ioctl == device_t::set_device_ctl)
      *(ol->result) = e_invalid_operation;
    else
      {
      comms_modem_status_ioctl_t *status_ioctl = reinterpret_cast<comms_modem_status_ioctl_t *>(ol->action.ioctl.read_ptr);

      DWORD status;
      if(GetCommModemStatus(_hndl, &status) == 0)
        {
        *(ol->result) = e_generic_error;
        return true;
        }

      status_ioctl->version = sizeof(comms_modem_status_ioctl_t);
      status_ioctl->cts_on = (status & MS_CTS_ON) != 0;
      status_ioctl->dsr_on = (status & MS_DSR_ON) != 0;
      status_ioctl->ring_on = (status & MS_RING_ON) != 0;
      status_ioctl->rlsd_on = (status & MS_RLSD_ON) != 0;

      *(ol->result) = s_ok;
      }

    break;
  case comms_timeouts_ioctl :
    if(comms_ioctl->version != sizeof(comms_timeouts_ioctl_t) &&
      ol->action.ioctl.write_size < sizeof(comms_timeouts_ioctl_t))
      {
      *(ol->result) = e_ioctl_version_error;
      return true;
      }

    if(ol->action.ioctl.ioctl == device_t::set_device_ctl)
      {
      const comms_timeouts_ioctl_t *timeouts_ioctl = reinterpret_cast<const comms_timeouts_ioctl_t *>(ol->action.ioctl.write_ptr);

      COMMTIMEOUTS timeouts;

      timeouts.ReadIntervalTimeout = timeouts_ioctl->read_interval_timeout;
      timeouts.ReadTotalTimeoutConstant = timeouts_ioctl->read_total_timeout_constant;
      timeouts.ReadTotalTimeoutMultiplier = timeouts_ioctl->read_total_timeout_multiplier;
      timeouts.WriteTotalTimeoutConstant = timeouts_ioctl->write_total_timeout_constant;
      timeouts.WriteTotalTimeoutMultiplier = timeouts_ioctl->write_total_timeout_multiplier;

      if(SetCommTimeouts(_hndl, &timeouts) == 0)
        {
        *(ol->result) = e_generic_error;
        return true;
        }

      *(ol->result) = s_ok;
      }
    else
      {
      comms_timeouts_ioctl_t *timeouts_ioctl = reinterpret_cast<comms_timeouts_ioctl_t *>(ol->action.ioctl.read_ptr);

      COMMTIMEOUTS timeouts;
      if(GetCommTimeouts(_hndl, &timeouts) == 0)
        {
        *(ol->result) = e_generic_error;
        return true;
        }

      timeouts_ioctl->ioctl_type = comms_timeouts_ioctl;
      timeouts_ioctl->version = sizeof(comms_timeouts_ioctl_t);
      timeouts_ioctl->read_interval_timeout = timeouts.ReadIntervalTimeout;
      timeouts_ioctl->read_total_timeout_constant = timeouts.ReadTotalTimeoutConstant;
      timeouts_ioctl->read_total_timeout_multiplier = timeouts.ReadTotalTimeoutMultiplier;
      timeouts_ioctl->write_total_timeout_constant = timeouts.WriteTotalTimeoutConstant;
      timeouts_ioctl->write_total_timeout_multiplier = timeouts.WriteTotalTimeoutMultiplier;

      *(ol->result) = s_ok;
      }

    break;
    }

  return true;
  }

bool kotuku::comm_device_t::process_read(device_t::overlapped_t *ol)
  {
  if(ol->action.read.xfer_length == 0)
    {
    *(ol->result) = e_operation_pending;
    return true;
    }

  // queue up the operation
  OVERLAPPEDEX *olex = new OVERLAPPEDEX;

  olex->ol = ol;
  olex->hEvent =  reinterpret_cast<HANDLE>(ol->completion->handle());
  olex->Internal = 0;
  olex->InternalHigh = 0;
  olex->Offset = 0;
  olex->OffsetHigh = 0;
  //olex->Pointer = 0;
  ol->os_data = olex;

  // perform a read file operation and process when done.
  if(_hndl == NULL || ReadFile(_hndl, ol->action.read.read_ptr, ol->action.read.read_size, (LPDWORD) ol->action.read.xfer_length, olex)  == 0)
    {
    DWORD error = GetLastError();
    if(_hndl == NULL || error != ERROR_IO_PENDING)
      {
      *(ol->result) = e_generic_error;
      return true;
      }
    *(ol->result) = e_operation_pending;
    }
  else
    {
    if(*ol->action.read.xfer_length == 0)
      *(ol->result) = e_timeout_error;
    else
      *(ol->result) = s_ok;

    SetEvent(olex->hEvent);

    return true;
    }

  return false;
  }

bool kotuku::comm_device_t::process_write(device_t::overlapped_t *ol)
  {
  if(ol->action.write.xfer_length == 0)
    {
    *(ol->result) = e_operation_pending;
    return true;
    }

  // queue up the operation
  OVERLAPPEDEX *olex = new OVERLAPPEDEX;

  olex->ol = ol;
  olex->hEvent = reinterpret_cast<HANDLE>(ol->completion->handle());
  olex->Internal = 0;
  olex->InternalHigh = 0;
  olex->Offset = 0;
  olex->OffsetHigh = 0;
  //olex->Pointer = 0;
  ol->os_data = olex;

  // perform a read file operation and process when done.
  if(WriteFile(_hndl, ol->action.write.write_ptr, ol->action.write.write_size, (LPDWORD)ol->action.write.xfer_length, olex) == 0)
    {
    if (GetLastError() != ERROR_IO_PENDING)
      {
      *(ol->result) = e_generic_error;
      return true;
      }
    *(ol->result) = e_operation_pending;
    }
  else
    {
    if(*ol->action.write.xfer_length == 0)
      *(ol->result) = e_timeout_error;
    else
      *(ol->result) = s_ok;

    SetEvent(olex->hEvent);

    return true;
    }


  return false;
  }

result_t kotuku::comm_device_t::device_cancel_io()
  {
#ifndef _WIN32_WCE
  return CancelIo(_hndl) == 0 ? e_generic_error : s_ok;
#else
  return s_ok;
#endif
  }

void kotuku::comm_device_t::set_comm_state(int baud_rate)
  {
  DCB dcb;
  memset(&dcb, 0, sizeof(DCB));
  dcb.DCBlength = sizeof(DCB);

  dcb.BaudRate = baud_rate;
  dcb.ByteSize = 8;
  dcb.fBinary = 1;
  dcb.StopBits = 1;

  //BuildCommDCB(state, &dcb);

  SetCommState(_hndl, &dcb);
  }

void kotuku::comm_device_t::send_msg(const unsigned char *msg, size_t length)
  {
  device_t::overlapped_t ol;
  size_t written;
  result_t result;

  ol.action.write.write_ptr = msg;
  ol.action.write.write_size = length;
  ol.action.write.xfer_length = &written;
  ol.result = &result;
  ol.operation = device_t::overlapped_t::write_opn;

  event_t evt;
  ol.completion = &evt;
  if(!process_write(&ol))
    event_t::lock_t lock(evt);
  }

