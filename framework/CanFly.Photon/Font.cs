using System;

namespace CanFly
{
  public class Font : IDisposable
  {
    private uint _hndl;

    internal uint Handle
    {
      get { return _hndl; }
    }

    public void Dispose()
    {
      CanFly.Syscall.ReleaseFont(_hndl);
      _hndl = 0;
    }
    internal Font(uint hndl)
    {
      _hndl = hndl;
    }
    /// <summary>
    /// Load a font from the font cache
    /// </summary>
    /// <param name="fontName">Name of the font to load</param>
    /// <param name="pointSize">Size of the font requested</param>
    public Font(string fontName, uint pointSize)
    {
      CanFly.Syscall.GetFont(fontName, pointSize, out _hndl);

      if (_hndl == 0)
        throw new InvalidOperationException();
    }

    public static void LoadFont(Stream fontStream)
    {
      CanFly.Syscall.LoadFont(fontStream.Handle);
    }
  }
}