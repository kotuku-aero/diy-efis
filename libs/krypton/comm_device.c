#define time_t __time_t
#include <stdlib.h>
#include <string.h>
#undef time_t

#include "../../libs/neutron/neutron.h"

#include <Windows.h>

typedef struct _comm_rx_buffer_t {
  uint16_t version;           // reserved
  uint16_t len;
  byte_t data[];              // is len bytes long
} comm_rx_buffer_t;

typedef struct _comm_device_t {
  size_t version;
  handle_t tx_ready;
  handle_t rx_ready;
  handle_t event_handler;
  int hndl;
} comm_device_t;
static result_t check_handle(handle_t hndl)
  {
  if(hndl == 0 || ((comm_device_t *)hndl)->version != sizeof(comm_device_t))
    return e_bad_handle;

  return s_ok;
  }

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Comms driver

result_t comm_create_device(memid_t key, handle_t *device)
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

  if(failed(result = reg_get_string(key, "device", driver, &len)))
    return result;

  // MUST be defined.
  uint32_t baud_rate;
  if(failed(result = reg_get_uint32(key, "baud-rate", &baud_rate)))
    return result;

  memset(dev, 0, sizeof(comm_device_t));
  dev->version = sizeof(comm_device_t);
  dev->hndl = CreateFile(driver, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0 /*FILE_FLAG_OVERLAPPED*/, NULL);

 // set some timeouts
  COMMTIMEOUTS timeouts;

  timeouts.ReadIntervalTimeout = 50 ;  // 50msec
  timeouts.ReadTotalTimeoutConstant = 50;
  timeouts.ReadTotalTimeoutMultiplier = 10;
  timeouts.WriteTotalTimeoutConstant = 50;
  timeouts.WriteTotalTimeoutMultiplier = 10;

  SetCommTimeouts(dev->hndl, &timeouts);

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

  comm_ioctl(dev, set_device_ctl, &ioctl, sizeof(comms_state_ioctl_t), 0, 0, 0);
  return s_ok;
  }

result_t comm_close_device(handle_t hndl)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  comm_device_t *device = (comm_device_t *)hndl;
  CloseHandle(device->hndl);
  semaphore_close(device->rx_ready);
  semaphore_close(device->tx_ready);

  return s_ok;

   }

result_t comm_write(handle_t hndl, const byte_t *data, uint16_t len, uint32_t timeout)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  comm_device_t *device = (comm_device_t *)hndl;

  int xfer_length;

  // perform a read file operation and process when done.
  if(!WriteFile(device->hndl, data, len, &xfer_length, NULL))
    {
    if(errno != 0)
      return e_generic_error;
    }

  return s_ok;
    }

result_t comm_read(handle_t hndl, byte_t *data, uint16_t len, uint16_t *bytes_read, uint32_t timeout)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  comm_device_t *device = (comm_device_t *)hndl;

  int xfer_length;

  if(bytes_read != 0)
    *bytes_read = 0;

  // perform a read file operation and process when done.
  if(device->hndl <= 0 ||
      !ReadFile(device->hndl, data, len, &xfer_length, NULL) ||
    xfer_length == 0)
    {
    if(device->hndl <= 0 || GetLastError() != 0)
      {
      return e_generic_error;
      }
    }

  if(bytes_read != 0)
    *bytes_read = xfer_length;

  return s_ok;
  }

result_t comm_ioctl(handle_t hndl, ioctl_type type, const void *in_buffer,
    uint16_t in_buffer_size, void *out_buffer, uint16_t out_buffer_size,
               uint16_t *size_returned)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  comm_device_t *device = (comm_device_t *)hndl;

  const comms_ioctl_t *comms_ioctl = (const comms_ioctl_t *)in_buffer;

  // determine type
  comms_ioctl_type_t op_type = comms_ioctl->ioctl_type;

  // decide what to do
  switch(op_type)
    {
  case comms_state_ioctl :
    if(comms_ioctl->version != sizeof(comms_state_ioctl_t) &&
          out_buffer_size != sizeof(comms_state_ioctl_t))
      {

      return e_bad_ioctl;
      }

    DCB dcb;
    memset(&dcb, 0, sizeof(DCB));
    dcb.DCBlength = sizeof(DCB);

    if(type == set_device_ctl)
      {
      const comms_state_ioctl_t *set_comms_state_ioctl =
          (const comms_state_ioctl_t *)in_buffer;
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

      if(!SetCommState(device->hndl, &dcb))
        return e_bad_parameter;
      }
    else
      {
      if(out_buffer_size != sizeof(comms_state_ioctl_t))
        return e_bad_ioctl;


      comms_state_ioctl_t *get_comms_state_ioctl = (comms_state_ioctl_t *) out_buffer;

      if(!GetCommState(device->hndl, &dcb))
        return e_bad_parameter;

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

    return s_ok;
  case comms_error_ioctl :
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
        return e_ioctl_buffer_too_small;

      comms_error_ioctl_t *get_error_state_ioctl = (comms_error_ioctl_t *) in_buffer;

      DWORD errors;
      if(ClearCommError(device->hndl, &errors, NULL)== 0)
        return e_generic_error;
      else
        {
        get_error_state_ioctl->break_char = (errors & CE_BREAK) != 0;
        get_error_state_ioctl->framing_error = (errors & CE_FRAME) != 0;
        get_error_state_ioctl->overflow = (errors & CE_RXOVER) != 0;
        get_error_state_ioctl->overrun = (errors & CE_OVERRUN) != 0;
        get_error_state_ioctl->parity = (errors & CE_RXPARITY) != 0;
        }

      return s_ok;
      }

    break;
  case comms_escape_ioctl :
    if(comms_ioctl->version != sizeof(comms_escape_ioctl_t) &&
        out_buffer_size < sizeof(comms_escape_ioctl_t))
      {
      return e_bad_ioctl;
      }

    if(type == set_device_ctl)
      {
      const comms_escape_ioctl_t *comms_escape_ioctl = (const comms_escape_ioctl_t *)out_buffer;

      return EscapeCommFunction(device->hndl, comms_escape_ioctl->escape_type) == 0 ? e_generic_error : s_ok;
      }
    else
      return e_invalid_operation;

    break;
  case comms_break_ioctl :
    if(comms_ioctl->version != sizeof(comms_break_ioctl_t) &&
        out_buffer_size < sizeof(comms_break_ioctl_t))
      {
      return e_bad_ioctl;
      }

    if(type == set_device_ctl)
      {
      const comms_break_ioctl_t *comms_break_ioctl = (const comms_break_ioctl_t *) out_buffer;

      if(comms_break_ioctl->set_break)
        return SetCommBreak(device->hndl) == 0 ? e_generic_error : s_ok;
      else
        return ClearCommBreak(device->hndl) == 0 ? e_generic_error : s_ok;
      }
    else
      return e_invalid_operation;

    break;
  case comms_event_mask_ioctl :
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

      SetCommMask(device->hndl, set_comms_event_mask_ioctl->mask);
      }
    else
      {
      if(in_buffer_size< sizeof(comms_event_mask_ioctl_t))
        return e_bad_ioctl;

      comms_event_mask_ioctl_t *get_comms_event_mask_ioctl =
          (comms_event_mask_ioctl_t *) in_buffer;

      get_comms_event_mask_ioctl->notification_event = device->event_handler;

      DWORD mask = 0;
      if(GetCommMask(device->hndl, &mask))
        get_comms_event_mask_ioctl->mask = (event_mask_t)mask;
      else
        {
        get_comms_event_mask_ioctl->mask = 0;
        return e_generic_error;
        }
      }

    return s_ok;
    break;
  case comms_modem_status_ioctl :
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

      DWORD status;
      if(GetCommModemStatus(device->hndl, &status) == 0)
        {
        return e_generic_error;
        }

      status_ioctl->cts_on = (status & MS_CTS_ON) != 0;
      status_ioctl->dsr_on = (status & MS_DSR_ON) != 0;
      status_ioctl->ring_on = (status & MS_RING_ON) != 0;
      status_ioctl->rlsd_on = (status & MS_RLSD_ON) != 0;

      return s_ok;
      }

    break;
  case comms_timeouts_ioctl :
    if(comms_ioctl->version != sizeof(comms_timeouts_ioctl_t) &&
        out_buffer_size < sizeof(comms_timeouts_ioctl_t))
      {
      return e_bad_ioctl;
      }

    if(type == set_device_ctl)
      {
      const comms_timeouts_ioctl_t *timeouts_ioctl =(const comms_timeouts_ioctl_t *) out_buffer;

      COMMTIMEOUTS timeouts;

      timeouts.ReadIntervalTimeout = timeouts_ioctl->read_interval_timeout;
      timeouts.ReadTotalTimeoutConstant = timeouts_ioctl->read_total_timeout_constant;
      timeouts.ReadTotalTimeoutMultiplier = timeouts_ioctl->read_total_timeout_multiplier;
      timeouts.WriteTotalTimeoutConstant = timeouts_ioctl->write_total_timeout_constant;
      timeouts.WriteTotalTimeoutMultiplier = timeouts_ioctl->write_total_timeout_multiplier;

      if(SetCommTimeouts(device->hndl, &timeouts) == 0)
        {
        return e_generic_error;
        }

      return s_ok;
      }
    else
      {
      comms_timeouts_ioctl_t *timeouts_ioctl = (comms_timeouts_ioctl_t *) in_buffer;

      COMMTIMEOUTS timeouts;
      if(GetCommTimeouts(device->hndl, &timeouts) == 0)
        return e_generic_error;

      timeouts_ioctl->read_interval_timeout = timeouts.ReadIntervalTimeout;
      timeouts_ioctl->read_total_timeout_constant = timeouts.ReadTotalTimeoutConstant;
      timeouts_ioctl->read_total_timeout_multiplier = timeouts.ReadTotalTimeoutMultiplier;
      timeouts_ioctl->write_total_timeout_constant = timeouts.WriteTotalTimeoutConstant;
      timeouts_ioctl->write_total_timeout_multiplier = timeouts.WriteTotalTimeoutMultiplier;

      return s_ok;
      }

    break;
    }

  return e_invalid_operation;
  }
