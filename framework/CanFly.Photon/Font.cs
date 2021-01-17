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
      ReleaseFont(_hndl);
      _hndl = 0;
    }
    /// <summary>
    /// Load a font from the font cache
    /// </summary>
    /// <param name="fontName">Name of the font to load</param>
    /// <param name="pointSize">Size of the font requested</param>
    public Font(string fontName, uint pointSize)
    {
      _hndl = GetFont(fontName, pointSize);
      if (_hndl == 0)
        throw new InvalidOperationException();
    }

    public static void LoadFont(Stream fontStream)
    {
      LoadFont(fontStream.Handle);
    }
  }
}