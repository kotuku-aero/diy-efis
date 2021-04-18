//
// Copyright (c) .NET Foundation and Contributors
// Original work from Oleg Rakhmatulin.
// See LICENSE file in the project root for full license information.
//

using System.Drawing;
using System.Drawing.Imaging;

namespace CanFly.Tools.MetadataProcessor
{
    internal sealed class nanoBitmapProcessor
    {
        private readonly Bitmap _bitmap;

        public nanoBitmapProcessor(
            Bitmap bitmap)
        {
            _bitmap = bitmap;
        }

        public void Process(
            CLRBinaryWriter writer)
        {
            writer.WriteUInt32((uint)_bitmap.Width);
            writer.WriteUInt32((uint)_bitmap.Height);

            writer.WriteUInt16(0x00);   // flags

            var nanoImageFormat = GetnanoImageFormat(_bitmap.RawFormat);

            if (nanoImageFormat != 0)
            {
                writer.WriteByte(0x01);     // bpp
                writer.WriteByte(nanoImageFormat);
                _bitmap.Save(writer.BaseStream, _bitmap.RawFormat);
            }
            else
            {
                writer.WriteByte(0x10);     // bpp
                writer.WriteByte(nanoImageFormat);

                var rect = new Rectangle(Point.Empty, _bitmap.Size);
                using (var convertedBitmap =
                    _bitmap.Clone(new Rectangle(Point.Empty, _bitmap.Size),
                        PixelFormat.Format16bppRgb565))
                {
                    var bitmapData = convertedBitmap.LockBits(
                        rect, ImageLockMode.ReadOnly, convertedBitmap.PixelFormat);

                    var buffer = new short[bitmapData.Stride * convertedBitmap.Height / sizeof(short)];
                    System.Runtime.InteropServices.Marshal.Copy(
                        bitmapData.Scan0, buffer, 0, buffer.Length);

                    convertedBitmap.UnlockBits(bitmapData);
                    foreach (var item in buffer)
                    {
                        writer.WriteInt16(item);
                    }
                }
            }
        }

        private byte GetnanoImageFormat(
            ImageFormat rawFormat)
        {
            if (rawFormat.Equals(ImageFormat.Gif))
            {
                return 1;
            }
            
            if (rawFormat.Equals(ImageFormat.Jpeg))
            {
                return 2;
            }

            return 0;
        }
    }
}