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

  public struct PhotonID
  {
    public static readonly ushort id_photon = 1750;
    public static readonly ushort id_paint = 1751;
    public static readonly ushort id_timer = 1754;
    public static readonly ushort id_key0 = 1755;
    public static readonly ushort id_key1 = 1756;
    public static readonly ushort id_key2 = 1757;
    public static readonly ushort id_key3 = 1758;
    public static readonly ushort id_key4 = 1759;
    public static readonly ushort id_key5 = 1760;
    public static readonly ushort id_key6 = 1761;
    public static readonly ushort id_key7 = 1762;
    public static readonly ushort id_decka = 1763;
    public static readonly ushort id_deckb = 1764;
    public static readonly ushort id_menu_up = 1765;
    public static readonly ushort id_menu_dn = 1766;
    public static readonly ushort id_menu_left = 1767;
    public static readonly ushort id_menu_right = 1768;
    public static readonly ushort id_menu_ok = 1769;
    public static readonly ushort id_menu_cancel = 1770;
    public static readonly ushort id_menu_select = 1771;
    public static readonly ushort id_buttonpress = 1772;
    public static readonly ushort IdCancel = 1773;
    /// <summary>
    /// Sent every 10msec to each window
    /// </summary>
    public static readonly ushort Timer = 1774;
    public static readonly ushort FuelTotal = 1775;
    public static readonly ushort IdLast = 1776;
  }
}
