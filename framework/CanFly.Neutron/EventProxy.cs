/*
diy-efis
Copyright (C) 2021 Kotuku Aerospace Limited

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice is
subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.

If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the original copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
using System;

namespace CanFly
{
  /// <summary>
  /// Generic callback for a CanFly message
  /// </summary>
  /// <param name="msg"></param>
  public delegate void CanFlyMsgHandler(CanFlyMsg msg);
  /// <summary>
  /// Holder of a callback from the event system to the CLR
  /// </summary>
  public sealed class EventProxy : IDisposable
  {
    private ushort _canId;
    private CanFlyMsgHandler _callback;
    public EventProxy(ushort canId, CanFlyMsgHandler callback)
    {
      _canId = canId;
      _callback = callback;
      Syscall.AddEvent(canId, OnMessage);
    }

    private void OnMessage(ushort flags, byte b0, byte b1, byte b2, byte b3, byte b4, byte b5, byte b6, byte b7)
    {
      CanFlyMsg msg = new CanFlyMsg(flags, b0, b1, b2, b3, b4, b5, b6, b7);
      _callback(msg);
    }

    public void Dispose()
    {
      Syscall.RemoveEvent(_canId, OnMessage);
    }
  }
}
