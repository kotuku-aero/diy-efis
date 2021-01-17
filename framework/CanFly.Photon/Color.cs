using System;

namespace CanFly
{
  public struct Colors
  {
    public static uint FromRGBA(byte r, byte g, byte b, byte a)
    {
      return ((uint)((((uint)(r)) << 24) | (((uint)(g)) << 16) | (((uint)(b)) << 8) | ((uint)(a))));
    }

    public static uint FromRGB(byte r, byte g, byte b)
    {
      return FromRGBA(r, g, b, 0xff);
    }
    public static readonly uint White = FromRGB(255, 255, 255);
    public static readonly uint Black = FromRGB(0, 0, 0);
    public static readonly uint Gray = FromRGB(128, 128, 128);
    public static readonly uint LightGray = FromRGB(192, 192, 192);
    public static readonly uint DarkGray = FromRGB(64, 64, 64);
    public static readonly uint Red = FromRGB(255, 0, 0);
    public static readonly uint Pink = FromRGB(255, 128, 128);
    public static readonly uint Blue = FromRGB(0, 0, 255);
    public static readonly uint Green = FromRGB(0, 255, 0);
    public static readonly uint LightGreen = FromRGB(0, 192, 0);
    public static readonly uint Yellow = FromRGB(255, 255, 64);
    public static readonly uint Magenta = FromRGB(255, 0, 255);
    public static readonly uint Cyan = FromRGB(0, 255, 255);
    public static readonly uint PaleYellow = FromRGB(255, 255, 208);
    public static readonly uint LightYellow = FromRGB(255, 255, 128);
    public static readonly uint LimeGreen = FromRGB(192, 220, 192);
    public static readonly uint Teal = FromRGB(64, 128, 128);
    public static readonly uint DarkGreen = FromRGB(0, 128, 0);
    public static readonly uint Maroon = FromRGB(128, 0, 0);
    public static readonly uint Purple = FromRGB(128, 0, 128);
    public static readonly uint Orange = FromRGB(255, 192, 64);
    public static readonly uint Khaki = FromRGB(167, 151, 107);
    public static readonly uint Olive = FromRGB(128, 128, 0);
    public static readonly uint Brown = FromRGB(192, 128, 32);
    public static readonly uint Navy = FromRGB(0, 64, 128);
    public static readonly uint LightBlue = FromRGB(128, 128, 255);
    public static readonly uint FadedBlue = FromRGB(192, 192, 255);
    public static readonly uint LightGrey = FromRGB(192, 192, 192);
    public static readonly uint DarkGrey = FromRGB(64, 64, 64);
    public static readonly uint Hollow = FromRGBA(0, 0, 0, 0);
  }
}
