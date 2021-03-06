﻿#include <iostream>

// Project
#include "View.h"
#include "Effects.h"
#include "KinectViz/VizPipeline.h"

// KinectTable
#include "Global/KinectTable_c.h"
#include "Util/Drawing.h"
#include "Util/Helpers.h"


// depth lookup table
#define MAX_DEPTH 10000
static unsigned short s_depth[MAX_DEPTH];

KinectTable::Client* client = NULL;
extern bool showDebug;

KinectViz::VizPipeline viz;


// Function prototypes
int InitLocalSession(const SessionParameters& params);
void SpecialInput(int key, int x, int y);
void Init();
void DeInit();
void Keyboard (unsigned char key, int x, int y);
void Display(void);
static void raw2depth();
void Binary2Color(const BinaryImage& binaryImage, ColorImage& colorImage);
void depth2color(const DepthImage& depthImage, ColorImage& colorImage);
void HighlightPoints(const ColorImage& input, const std::vector<Point2Di> points, const ColorPixel& color, ColorImage& output);
void HighlightRegions(const ColorImage& colorImage, const BinaryImage& highlightImage, const ColorImage& highlightedColorImage);
bool CreateGraphicalImage(KinectData& data, ColorImage& testImage);


//---------------------------------------------------------------------------
// Entry Point
//---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	// Set up the KinectTable session parameters
	SessionParameters params;
	params.dataParams.colorImageEnable = true;
	params.dataParams.depthImageEnable = true;
	params.dataParams.validityImageEnable = false;
	params.dataParams.testImageEnable = showDebug;
	params.dataParams.handsEnable = true;

	// Initialize KinectTable as local session
	int ret = InitLocalSession(params);
	if(ret != 0) {
		return -1;
	}	

	// Set up the initialization parameters for viewing
	static View::InitParams initParams;
	initParams.initFunc = Init;
	initParams.deInitFunc = DeInit;
	initParams.idleFunc = NULL;
	initParams.displayFunc = Display;
	initParams.keyboardFunc = Keyboard;
	initParams.specialInputFunc = SpecialInput;
	initParams.resizeFunc = NULL;
	
	// Initialize the viewing
	View::Init(initParams);

	// build a depth lookup table
	raw2depth();

	// Load effect texture images
	if(!loadTattoos())
		return 0;

	//
	// Set up effects.
	// Note how to change tweakable parameters for each effect - look at Tint for example.
	//

	// Background removal
	viz.getEffect(KinectViz::kBackgroundRemover).enabled = false;

	// Skeleton
	viz.getEffect(KinectViz::kSkeleton).enabled = false;
	viz.getEffect(KinectViz::kSkeleton).minHeight = 0;
	viz.getEffect(KinectViz::kSkeleton).maxHeight = 1000;

	// Rainbow
	viz.getEffect(KinectViz::kRainbow).enabled = false;
	viz.getEffect(KinectViz::kRainbow).minHeight = 0;
	viz.getEffect(KinectViz::kRainbow).maxHeight = 1000;

	// Outline
	viz.getEffect(KinectViz::kOutline).enabled = false;
	viz.getEffect(KinectViz::kOutline).minHeight = 0;
	viz.getEffect(KinectViz::kOutline).maxHeight = 1000;

	// Pointer circle
	viz.getEffect(KinectViz::kPointerCircle).enabled = false;
	viz.getEffect(KinectViz::kPointerCircle).minHeight = 0;
	viz.getEffect(KinectViz::kPointerCircle).maxHeight = 1000;

	// Tint
	viz.getEffect(KinectViz::kTint).enabled = false;
	((KinectViz::Tint&)(viz.getEffect(KinectViz::kTint))).handColors[0] = ColorPixel(255,100,100);
	((KinectViz::Tint&)(viz.getEffect(KinectViz::kTint))).handColors[1] = ColorPixel(100,255,100);
	((KinectViz::Tint&)(viz.getEffect(KinectViz::kTint))).handColors[2] = ColorPixel(100,100,255);
	viz.getEffect(KinectViz::kTint).minHeight = 0;
	viz.getEffect(KinectViz::kTint).maxHeight = 1000;

	// Motion blur
	viz.getEffect(KinectViz::kMotionBlur).enabled = false;
	viz.getEffect(KinectViz::kMotionBlur).minHeight = 0;
	viz.getEffect(KinectViz::kMotionBlur).maxHeight = 1000;

	// Traces
	viz.getEffect(KinectViz::kTraces).enabled = false;
	viz.getEffect(KinectViz::kTraces).minHeight = 0;
	viz.getEffect(KinectViz::kTraces).maxHeight = 1000;

	// Shadows
	viz.getEffect(KinectViz::kShadow).enabled = false;
	viz.getEffect(KinectViz::kShadow).minHeight = 0;
	viz.getEffect(KinectViz::kShadow).maxHeight = 1000;

	// Transparency
	viz.getEffect(KinectViz::kTransparency).enabled = false;
	viz.getEffect(KinectViz::kTransparency).minHeight = 0;
	viz.getEffect(KinectViz::kTransparency).maxHeight = 1000;

	// Tattoos
	viz.getEffect(KinectViz::kTattoo).enabled = false;
	viz.getEffect(KinectViz::kTattoo).minHeight = 0;
	viz.getEffect(KinectViz::kTattoo).maxHeight = 1000;

	// Start the application
	View::StartLoop();

	return 0;
}


void Display(void)
{
	static ColorImage grayImage;
	memset(grayImage.data, 0x80, sizeof(grayImage.data));
	grayImage.rows = grayImage.maxRows;
	grayImage.cols = grayImage.maxCols;

	// Get data maps from Kinect sensor
	static KinectData data;
	static KinectData remoteData;
	bool dataChanged = false;
	try
	{
		dataChanged = client->GetData(data);
	}
	catch(...)
	{
		fprintf(stderr, "Error: Could not get data!");
		return;
	}
	
	if(dataChanged == false)
		return;

	//Get visual representation of the depth image
	static ColorImage visualDepthImage;
	if (showDebug)
		depth2color(data.depthImage, visualDepthImage);

	// Apply effects (new version)
	viz.updateData(data);
	viz.applyEffects();

	// Get test image
	static ColorImage testImage;
	if (showDebug)
		CreateGraphicalImage(data, testImage);

	// Set the images for viewing
	View::SetTopLeftImage(viz.getImage());
	View::SetTopRightImage(visualDepthImage);
	View::SetBottomLeftImage(testImage);
	
	return;
}

void Keyboard (unsigned char key, int x, int y)
{
	// If a number is pressed, toggle the effect with that id
	if (key >= '0' && key <= '9') {
		viz.getEffect(key - '0').toggle();
		return;
	}

	switch (key) {
	case 'b':
		viz.getEffect(KinectViz::kBackgroundRemover).toggle();
		break;
	case 'd':
		showDebug = !showDebug;
		break;
	case 27:  // escape
		View::StopLoop();
		break;
	case 'f':
		View::SetFullscreen();
		break;
	case 't':
		client->RecalculateTableCorners();
		break;
	default:
		break;
	}
}


//---------------------------------------------------------------------------
// Util Functions
//---------------------------------------------------------------------------

// builds a lookup table for depth2rgb
static void raw2depth()
{
		int i;
		for ( i=0; i<MAX_DEPTH; i++)
		{
			//float dist =100/(-0.0030711016*i + 3.3309495161); //ros distance (cm)
			//float dist2 = 12.36 * tanf(i/ 2842.5 + 1.1863);//St�phane Magnenat distance (cm)
			float v = (float)i/MAX_DEPTH; //for visualization purposes only
			v = powf(v, 2);
			v = v*36*256;
			s_depth[i] = (unsigned short)v;
		}
}

void Binary2Color(const BinaryImage& binaryImage, ColorImage& colorImage)
{
	const ColorPixel blackPixel(0,0,0);
	const ColorPixel whitePixel(255,255,255);

	for (size_t y=0; y<binaryImage.rows; y++)
	{
		for(size_t x=0; x<binaryImage.cols; x++)
		{
			colorImage.data[y][x] = binaryImage.data[y][x] == true ? whitePixel : blackPixel;
		}
	}

	colorImage.rows = binaryImage.rows;
	colorImage.cols = binaryImage.cols;

	return;
}

void depth2color(const DepthImage& depthImage, ColorImage& colorImage)
{
	for (size_t y=0; y<depthImage.rows; y++)
	{
		for(size_t x=0; x<depthImage.cols; x++)
		{   
			int pval = s_depth[depthImage.data[y][x]];
			int lb = pval & 0xff;

			ColorPixel& colorPixel = colorImage.data[y][x];

			switch (pval>>8)
			{
				case 0:
						colorPixel.red = 255;
						colorPixel.green = 255-lb;
						colorPixel.blue = 255-lb;
						break;
				case 1:
						colorPixel.red = 255;
						colorPixel.green = lb;
						colorPixel.blue = 0;
						break;
				case 2:
						colorPixel.red = 255-lb;
						colorPixel.green = 255;
						colorPixel.blue = 0;
						break;
				case 3:
						colorPixel.red = 0;
						colorPixel.green = 255;
						colorPixel.blue = lb;
						break;
				case 4:
						colorPixel.red = 0;
						colorPixel.green = 255-lb;
						colorPixel.blue = 255;
						break;
				case 5:
						colorPixel.red = 0;
						colorPixel.green = 0;
						colorPixel.blue = 255-lb;
						break;
				default:
						colorPixel.red = 0;
						colorPixel.green = 0;
						colorPixel.blue = 0;
						break;
			}
		}
	}

	colorImage.rows = depthImage.rows;
	colorImage.cols = depthImage.cols;

	return;
}


void HighlightPoints(const ColorImage& input, const std::vector<Point2Di> points, const ColorPixel& color, ColorImage& output)
{
	for(int i=0; i<points.size(); i++)
	{
		const Point2Di& point = points[i];
		input.data[point.y][point.x] = color;
	}

	return;
}

void HighlightRegions(const ColorImage& colorImage, const BinaryImage& highlightImage, const ColorImage& highlightedColorImage)
{
	const ColorPixel greenPixel(0,255,0);

	for(size_t y=0; y<colorImage.rows; y++)
	{
		for(size_t x=0; x<colorImage.cols; x++)
		{
			highlightedColorImage.data[y][x] = highlightImage.data[y][x] ? greenPixel : colorImage.data[y][x];
		}
	}

	return;
}

bool CreateGraphicalImage(KinectData& data, ColorImage& testImage)
{
	// Use color image as background
	if(data.available.colorImageEnable)
	{
		// Copy over color image
		data.colorImage.CopyTo(testImage);
	}


	// Create table image
	static BinaryImage tableBlob;
	data.table.CreateTableBlob(tableBlob);
	
	
	// Draw arms on image
	if(data.available.handsEnable)
	{	
		// Draw boundaries around arms
		static const ColorPixel armColors[] = { ColorPixel(ColorPixel::Black), ColorPixel(ColorPixel::Red), ColorPixel(ColorPixel::Green), ColorPixel(ColorPixel::Blue) };
		for(int i=0; i<data.hands.size(); i++)
		{
			const std::vector<Point2Di>& boundary = data.hands[i].boundary;

			if(data.hands[i].id + 1 < sizeof(armColors) / sizeof(ColorPixel));
			{
				for(int j=0; j<boundary.size(); j++)
				{
					const Point2Di& point = boundary[j];
					testImage.data[point.y][point.x] = armColors[data.hands[i].id + 1];
				}
			}
		}
		
		
		// Draw the finger tips
		for(int i=0; i<data.hands.size(); i++)
		{
			const Hand& hand = data.hands[i];
			Util::Drawing::DrawPoints(testImage, hand.fingerTips, ColorPixel(ColorPixel::Green), 3);
		}

		// Draw the finger bases
		/*for(int i=0; i<data.hands.size(); i++)
		{
			const Hand& hand = data.hands[i];
			Util::Drawing::DrawPoints(testImage, hand.fingerBases, ColorPixel(ColorPixel::Red), 3);
		}*/

		// Draw the arm base
		for(int i=0; i<data.hands.size(); i++)
		{
			const Hand& hand = data.hands[i];
			if(hand.armBase.x != -1 && hand.armBase.y != -1)
				Util::Drawing::DrawPoints(testImage, hand.armBase, ColorPixel(ColorPixel::Blue), 5);
		}

		
		// Draw the hand palm
		for(int i=0; i<data.hands.size(); i++)
		{
			const Hand& hand = data.hands[i];
			if(hand.palmCenter.x != -1 && hand.palmCenter.y != -1)
				Util::Drawing::DrawPoints(testImage, hand.palmCenter, ColorPixel(255, 0, 255), 5);
		}

		// Centroid
		for(int i=0; i<data.hands.size(); i++)
		{
			const Hand& hand = data.hands[i];
			if(hand.centroid.x != -1 && hand.centroid.y != -1)
				Util::Drawing::DrawPoints(testImage, hand.centroid, ColorPixel(255, 0, 0), 3);
		}
		
	}
	

	return true;
}


//---------------------------------------------------------------------------
// GlutCode
//---------------------------------------------------------------------------

void SpecialInput(int key, int x, int y)
{
}

void Init()
{
	return;
}

void DeInit()
{
	// Clean up //
	return;
}

int InitLocalSession(const SessionParameters& params)
{
	try {
		client = KinectTableConnectLocalPublic(&params, "127.0.0.1");
	}
	catch(Errno err)
	{
		// Check if the session started correctly
		switch(err)
		{
			case -1: fprintf(stderr, "Error: Could not initialize Kinect sensor.\n"); return -1;
			case ENODEV: fprintf(stderr, "Error: Could not initialize Kinect sensor.\n"); return -1;
			case ENOTSUP: fprintf(stderr, "Error: Could not initialize Kinect sensor.  Session is not supported.\n"); return -1;
			default: assert(false); break;
		}
	}

	return 0;
}
