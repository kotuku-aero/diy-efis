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
  public class Canvas : GdiObject
  {
    protected override void OnDispose()
    {
      Syscall.CanvasClose(Handle);
    }
    
    private Canvas(uint hndl)
      : base(hndl)
    {

    }

    /// <summary>
    /// Create a new empty canvas
    /// </summary>
    /// <param name="extent">Size of the canvas</param>
    /// <returns>New Canvas</returns>
    public static Canvas Create(Extent extent)
    {
      uint handle;
      Syscall.CreateRectCanvas(extent.Dx, extent.Dy, out handle);

      return new Canvas(handle);
    }

    /// <summary>
    /// Create a new canvas and fill it with a PNG encoded stream
    /// </summary>
    /// <param name="pngStream">Stream to load the canvas from</param>
    /// <returns>New canvas</returns>
    /// <remarks>
    /// The stream only supports a PNG encoded stream in the format 
    /// </remarks>
    public static Canvas Create(Stream pngStream)
    {
      uint handle;
      Syscall.CreatePngCanvas(pngStream.Handle, out handle);
      return new Canvas(handle);
    }
    /// <summary>
    /// Create a new canvas that will render a gis database
    /// </summary>
    /// <param name="gisData">GIS database stream</param>
    /// <param name="key">Registry key describing the viewport and visibility</param>
    /// <returns>Canvas that can be updated</returns>
    public static Canvas CreateGeospatial(Stream gisData, uint key)
    {
      uint handle;
      Syscall.CreateGeospatialCanvas(gisData.Handle, key, out handle);
      return new Canvas(handle);
    }
    /// <summary>
    /// Render the canvas, applicable to Png and Spatial canvases
    /// </summary>
    public void Render()
    {
      Syscall.CanvasRender(Handle);
    }
  }
}