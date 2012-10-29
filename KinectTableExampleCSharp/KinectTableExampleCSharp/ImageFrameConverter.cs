using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Reflection;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.Diagnostics;

using KinectTableNet;

namespace KinectTableExampleCSharp
{
    static class ImageFrameConverter
    {

        /// <summary>
        /// Convert the color image frame to a color bitmap.
        /// </summary>
        /// <param name="bitmap"></param>
        /// <param name="image"></param>
        public static void SetColorImage(Bitmap bitmap, ImageFrame image)
        {
            Rectangle rectangle = new Rectangle(0, 0, image.width, image.height);
            BitmapData bitmapData = bitmap.LockBits(rectangle, ImageLockMode.WriteOnly, bitmap.PixelFormat);

            IntPtr ptr = bitmapData.Scan0;
            Marshal.Copy(image.Bytes, 0, ptr, image.width * image.height * image.bytesPerPixel);

            bitmap.UnlockBits(bitmapData);

            return;
        }

        /// <summary>
        /// Convert the depth image frame to a color bitmap.
        /// </summary>
        /// <param name="bitmap"></param>
        /// <param name="image"></param>
        public static void SetDepthImage(Bitmap bitmap, ImageFrame image)
        {

            unsafe
            {
                Debug.Assert(image.bytesPerPixel == 2);

                Rectangle rectangle = new Rectangle(0, 0, image.width, image.height);
                BitmapData bitmapData = bitmap.LockBits(rectangle, ImageLockMode.WriteOnly, bitmap.PixelFormat);


                const int pixelSize = 3;

                for (int y = 0; y < image.height; y++)
                {

                    //get the data from the new image
                    byte* nRow = (byte*)bitmapData.Scan0 + (y * bitmapData.Stride);

                    for (int x = 0; x < image.width; x++)
                    {
                        int byteNum = (y * image.width + x) * image.bytesPerPixel;
                        byte value = (byte)(((image.Bytes[byteNum] << 8) + image.Bytes[byteNum + 1]) / 1000); //8191);

                        //set the new image's pixel to the grayscale version
                        nRow[x * pixelSize] = value; //B
                        nRow[x * pixelSize + 1] = value; //G
                        nRow[x * pixelSize + 2] = value; //R
                    }
                }

                //unlock the bitmaps
                bitmap.UnlockBits(bitmapData);

                return;
            }

        }

        /// <summary>
        /// Convert the binary image frame to a color bitmap.
        /// </summary>
        /// <param name="bitmap"></param>
        /// <param name="image"></param>
        public static void SetBinaryImage(Bitmap bitmap, ImageFrame image)
        {

            unsafe
            {
                Debug.Assert(image.bytesPerPixel == 1);

                Rectangle rectangle = new Rectangle(0, 0, image.width, image.height);
                BitmapData bitmapData = bitmap.LockBits(rectangle, ImageLockMode.WriteOnly, bitmap.PixelFormat);


                const int pixelSize = 3;

                for (int y = 0; y < image.height; y++)
                {

                    //get the data from the new image
                    byte* nRow = (byte*)bitmapData.Scan0 + (y * bitmapData.Stride);

                    for (int x = 0; x < image.width; x++)
                    {
                        int byteNum = (y * image.width + x) * image.bytesPerPixel;
                        byte value = (byte)(image.Bytes[byteNum] == 0 ? 0 : 255);

                        //set the new image's pixel to the grayscale version
                        nRow[x * pixelSize] = value; //B
                        nRow[x * pixelSize + 1] = value; //G
                        nRow[x * pixelSize + 2] = value; //R
                    }
                }

                //unlock the bitmaps
                bitmap.UnlockBits(bitmapData);

                return;
            }

        }

        /*
        Bitmap DepthImageToBitmap(ImageFrame image)
        {

            Debug.Assert(image.bytesPerPixel == 2);

            Bitmap bitmap = new Bitmap(image.width, image.height, PixelFormat.Format16bppGrayScale);
            Rectangle rectangle = new Rectangle(0, 0, image.width, image.height);
            BitmapData bitmapData = bitmap.LockBits(rectangle, ImageLockMode.WriteOnly, bitmap.PixelFormat);

            IntPtr ptr = bitmapData.Scan0;
            Marshal.Copy(image.Bytes, 0, ptr, image.width * image.height * image.bytesPerPixel);

            bitmap.UnlockBits(bitmapData);

            return bitmap;
        }
        */



    }
}
