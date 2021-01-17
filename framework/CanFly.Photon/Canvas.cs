
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
      return new Canvas(Syscall.CreateRectCanvas(extent.Dx, extent.Dy));
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
      return new Canvas(Syscall.CreatePngCanvas(pngStream.Handle));
    }
  }
}