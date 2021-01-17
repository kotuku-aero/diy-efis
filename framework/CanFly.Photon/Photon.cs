/*
diy-efis
Copyright (C) 2020 Kotuku Aerospace Limited

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
then the original copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
using System;

namespace CanFly
{
  [Flags]
  public enum TextOutStyle
  {
    Clipped = 0x02,
    Opaque = 0x04
  }

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
    public static readonly ushort id_cancel = 1773;
  }
}
