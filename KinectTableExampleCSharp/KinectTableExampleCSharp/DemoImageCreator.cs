using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Diagnostics;

using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;

using KinectTableNet;


namespace KinectTableExampleCSharp
{

    //!! Should use GDI+ to create the demo image or just manipulate a data array and set it.
    static class DemoImageCreator
    {

        public static bool CreateDemoImage(Bitmap bitmap, KinectData data)
        {

            // Get color image
            if (data.Available.colorImageEnable)
            {
                ImageFrameConverter.SetColorImage(bitmap, data.ColorImage);
            }


            // Lighten table
            if (data.Available.tableEnable)
            {
                ImageFrame tableBlob = data.TableInfo.CreateTableBlob(true);
                DarkenRegion(bitmap, tableBlob, 0.25f);
            }

            // Draw arms on image
            if (data.Available.handsEnable)
            {

                // Draw arm shadows
                //DrawArmShadows(bitmap, data.TableInfo, data.Hands);

                // Draw boundaries around arms
                Color[] armColors = { Color.Black, Color.Red, Color.Green, Color.Blue };
                foreach (Hand hand in data.Hands)
                {
                    if (hand.Id + 1 < armColors.Length)
                    {
                        HighlightPoints(bitmap, hand.Boundary, armColors[hand.Id + 1]);
                    }

                }


                // Draw finger tips
                IEnumerable<Point> allFingerTips = data.Hands.Select(x => x.FingerTips).SelectMany(x => x);
                DrawPoints(bitmap, allFingerTips, Color.Green, 3);

                // Draw finger bases
                IEnumerable<Point> allFingerBases = data.Hands.Select(x => x.FingerBases).SelectMany(x => x);
                DrawPoints(bitmap, allFingerBases, Color.Red, 3);

                // Draw arm bases
                IEnumerable<Point> allArmBases = data.Hands.Select(x => x.ArmBase);
                DrawPoints(bitmap, allArmBases, Color.Blue, 3);

                // Draw hand palm
                IEnumerable<Point> allPalmCenters = data.Hands.Select(x => x.PalmCenter);
                DrawPoints(bitmap, allPalmCenters, Color.Purple, 3);

            }

            return data.Available.colorImageEnable;

        }





        static void DarkenRegion(Bitmap bitmap, ImageFrame region, float darkenFactor)
        {

            unsafe
            {
                Debug.Assert(region.bytesPerPixel == 1);

                Rectangle rectangle = new Rectangle(0, 0, region.width, region.height);
                BitmapData bitmapData = bitmap.LockBits(rectangle, ImageLockMode.WriteOnly, bitmap.PixelFormat);


                const int pixelSize = 3;

                for (int y = 0; y < region.height; y++)
                {

                    //get the data from the new image
                    byte* nRow = (byte*)bitmapData.Scan0 + (y * bitmapData.Stride);

                    for (int x = 0; x < region.width; x++)
                    {
                        int byteNum = (y * region.width + x) * region.bytesPerPixel;

                        if (region.Bytes[byteNum] == 0)
                        {
                            //set the new image's pixel to the grayscale version
                            nRow[x * pixelSize] = (byte)(nRow[x * pixelSize] * darkenFactor); //B
                            nRow[x * pixelSize + 1] = (byte)(nRow[x * pixelSize + 1] * darkenFactor); //G
                            nRow[x * pixelSize + 2] = (byte)(nRow[x * pixelSize + 2] * darkenFactor); //R
                        }

                    }
                }

                //unlock the bitmaps
                bitmap.UnlockBits(bitmapData);

                return;
            }
        }


        static void DrawArmShadows(Bitmap bitmap, Table table, IEnumerable<Hand> hands)
        {
            // Draw arm shadows
            foreach (Hand hand in hands)
            {

                // Get arm blob
                ImageFrame armBlob = hand.CreateArmBlob();

                // Get points of arm blob
                Point[] points = GetIndices(armBlob);

                // Get shadow shift
                float heightAboveTable = table.Depth - hand.MeanDepth;
                int xShift = (int)(heightAboveTable / 20);
                int yShift = (int)(heightAboveTable / 100);
                Point shift = new Point(xShift, yShift);

                // Draw shadow
                foreach (Point point in points.Select(x => new Point(x.X + shift.X, x.Y + shift.Y)))
                {

                    bool xValueGood = point.X >= 0 && point.X < armBlob.width;
                    bool yValueGood = point.Y >= 0 && point.Y < armBlob.height;

                    if (xValueGood && yValueGood)
                    {
                        int byteNum = (point.Y * armBlob.width + point.X) * armBlob.bytesPerPixel;
                        bool isBelowArm = armBlob.Bytes[byteNum] == 0;
                        if (!isBelowArm)
                        {
                            Color color = bitmap.GetPixel(point.X, point.Y);
                            int red = (int)(color.R * 0.3f);
                            int green = (int)(color.G * 0.3f);
                            int blue = (int)(color.B * 0.3f);

                            Color newColor = Color.FromArgb(red, blue, green);
                            bitmap.SetPixel(point.X, point.Y, newColor);

                        }
                    }

                }

            }

            return;
        }


        static Point[] GetIndices(ImageFrame image)
        {
            Debug.Assert(image.bytesPerPixel == 1);

            List<Point> points = new List<Point>();
            for (int y = 0; y < image.height; y++)
            {
                for (int x = 0; x < image.width; x++)
                {
                    int byteNum = (y * image.width + x) * image.bytesPerPixel;

                    if (image.Bytes[byteNum] != 0)
                    {
                        points.Add(new Point(x, y));
                    }
                }
            }

            return points.ToArray();
        }


        static void HighlightPoints(Bitmap bitmap, IEnumerable<Point> points, Color color)
        {
            foreach (Point point in points)
            {
                bitmap.SetPixel(point.X, point.Y, color);
            }

            return;
        }


        static void DrawPoints(Bitmap bitmap, IEnumerable<Point> points, Color color, int size)
        {
            foreach (Point point in points)
            {

                Point minPoint = new Point(point.X - (size - 1), point.Y - (size - 1));
                if (minPoint.X < 0)
                    minPoint.X = 0;
                if (minPoint.Y < 0)
                    minPoint.Y = 0;

                Point maxPoint = new Point(point.X + (size - 1), point.Y + (size - 1));
                if (maxPoint.X > bitmap.Width - 1)
                    maxPoint.X = bitmap.Width - 1;
                if (maxPoint.Y > bitmap.Height - 1)
                    maxPoint.Y = bitmap.Height - 1;

                for (int y = minPoint.Y; y <= maxPoint.Y; y++)
                {
                    for (int x = minPoint.X; x <= maxPoint.X; x++)
                    {
                        bitmap.SetPixel(x, y, color);
                    }
                }

            }

            return;
        }



    }
}
