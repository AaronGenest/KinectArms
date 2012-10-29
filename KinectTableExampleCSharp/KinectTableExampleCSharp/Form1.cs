using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using System.Reflection;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;

using KinectTableNet;
using System.Diagnostics;


namespace KinectTableExampleCSharp
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();

            // Turn on key preview
            this.KeyPreview = true;

            // Allocate Kinect data structure for speed
            data = new KinectData();

            // Allocate bitmaps for speed
            //!! Should use System.Windows.Media.Imaging.WriteableBitmap
            colorBitmap = new Bitmap(640, 480, PixelFormat.Format24bppRgb);
            depthBitmap = new Bitmap(640, 480, PixelFormat.Format24bppRgb);
            testBitmap = new Bitmap(640, 480, PixelFormat.Format24bppRgb);
            demoBitmap = new Bitmap(640, 480, PixelFormat.Format24bppRgb);


            // Set up session parameters
            SessionParameters sessionParams = new SessionParameters(KinectDataParams.EnableType.All);
            sessionParams.DataParams.validityImageEnable = false;
            sessionParams.DataParams.testImageEnable = false;

            // Connect to a local Kinect and hook up to the data event
            client = KinectTableNet.KinectTable.ConnectLocal(sessionParams);
            client.DataReady += new Client.DataReadyHandler(client_DataReady);
            

            return;
        }

        /// <summary>
        /// Is called when the Form closes.
        /// </summary>
        /// <param name="e"></param>
        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            // Disconnect from the local Kinect
            KinectTableNet.KinectTable.Disconnect(client);

            base.OnFormClosing(e);
            return;
        }

        /// <summary>
        /// Is fired whenever a key is pressed with the form window active.
        /// </summary>
        /// <param name="e"></param>
        protected override void OnKeyPress(KeyPressEventArgs e)
        {
            // Recalculate table if key 't' is pressed
            if (e.KeyChar == 't')
                client.RecalculateTable();

            base.OnKeyPress(e);
            return;
        }


        /// <summary>
        /// Is fired whenever KinectTable has new data ready.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="data"></param>
        void client_DataReady(object sender, DataReadyEventArgs args)
        {

            // Get the current data
            args.GetData(out data);

            // Show color image if available
            if (data.Available.colorImageEnable)
            {
                ImageFrameConverter.SetColorImage(colorBitmap, data.ColorImage);
                pictureBoxColor.Image = colorBitmap;
            }

            // Show depth image if available
            if (data.Available.depthImageEnable)
            {
                ImageFrameConverter.SetDepthImage(depthBitmap, data.DepthImage);
                pictureBoxDepth.Image = depthBitmap;
            }
            
            // Show test image if available
            if (data.Available.testImageEnable)
            {
                ImageFrameConverter.SetColorImage(testBitmap, data.TestImage);
                pictureBoxTest.Image = testBitmap;
            }

            // Create demo image and show it
            bool createdDemoImage = DemoImageCreator.CreateDemoImage(demoBitmap, data);
            if (createdDemoImage)
                pictureBoxDemo.Image = demoBitmap;

            return;
        }



        
        readonly Client client;

        KinectData data;

        readonly Bitmap colorBitmap;
        readonly Bitmap depthBitmap;
        readonly Bitmap testBitmap;
        readonly Bitmap demoBitmap;

    }
}
